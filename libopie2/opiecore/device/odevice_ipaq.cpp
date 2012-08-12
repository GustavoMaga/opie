/*
 � � � � � � � �             This file is part of the Opie Project
             =.              (C) 2002-2005 The Opie Team <opie-devel@lists.sourceforge.net>
            .=l.
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This program is free software; you can
.> <`_, � > �. � <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU Library General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; version 2 of the License.
� � �._= =} � � � :
� � .%`+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This program is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful,  but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|`    MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.= � � � = � � � ;      Library General Public License for more
++= � -. � � .` � � .:       details.
�: � � = �...= . :.=-
�-. � .:....=;==+<;          You should have received a copy of the GNU
� -_. . . � )=. �=           Library General Public License along with
� � -- � � � �:-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "odevice_ipaq.h"

/* QT */
#include <qapplication.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>
#include <qgfx_qws.h>

/* OPIE */
#include <qpe/config.h>
#include <qpe/sound.h>
#include <qpe/qcopenvelope_qws.h>

#include <opie2/okeyfilter.h>
#include <opie2/oresource.h>

/* STD */
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#ifndef QT_NO_SOUND
#include <linux/soundcard.h>
#endif


using namespace Opie::Core;
using namespace Opie::Core::Internal;

/* KERNEL */
#define OD_IOC(dir,type,number,size)    (( dir << 30 ) | ( type << 8 ) | ( number ) | ( size << 16 ))

#define OD_IO(type,number)              OD_IOC(0,type,number,0)
#define OD_IOW(type,number,size)        OD_IOC(1,type,number,sizeof(size))
#define OD_IOR(type,number,size)        OD_IOC(2,type,number,sizeof(size))
#define OD_IORW(type,number,size)       OD_IOC(3,type,number,sizeof(size))

typedef struct {
        unsigned char mode;
        unsigned char pwr;
        unsigned char brightness;
} FLITE_IN;

#define FLITE_ON  OD_IOW( 'f', 7, FLITE_IN )

#define Model_Keyboardless_2_6 (Model_iPAQ_H191x | Model_iPAQ_H22xx | Model_iPAQ_HX4700 | Model_iPAQ_H4xxx | Model_iPAQ_RX3xxx)

struct ODeviceButtonComboStruct ipaq_combos[] = {
    // Centre of joypad + Calendar -> recalibrate
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx | Model_iPAQ_H5xxx | Model_Keyboardless_2_6, 
    Qt::Key_Return, Qt::Key_F9, Qt::Key_unknown, false, QT_TRANSLATE_NOOP("Button", "Joypad + Calendar"),
    "QPE/Application/calibrate", "raise()", QT_TRANSLATE_NOOP("ComboAction", "Recalibrate screen"), false },
    // Hold down Record (only when device locked) -> recalibrate
    { Model_iPAQ_H191x | Model_iPAQ_HX4700 | Model_iPAQ_H4xxx |
      Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx | Model_iPAQ_H5xxx,
    Qt::Key_F24, Qt::Key_unknown, Qt::Key_unknown, true, QT_TRANSLATE_NOOP("Button", "Record Button"),
    "QPE/Application/calibrate", "raise()", QT_TRANSLATE_NOOP("ComboAction", "Recalibrate screen"), true }
};

void iPAQ::init(const QString& model)
{
    d->m_vendorstr = "HP";
    d->m_vendor = Vendor_HP;

    d->m_modelstr = model.mid(model.findRev('H'));

    if ( d->m_modelstr == "H3100" )
        d->m_model = Model_iPAQ_H31xx;
    else if ( d->m_modelstr == "H3600" )
        d->m_model = Model_iPAQ_H36xx;
    else if ( d->m_modelstr == "H3700" )
        d->m_model = Model_iPAQ_H37xx;
    else if ( d->m_modelstr == "H3800" )
        d->m_model = Model_iPAQ_H38xx;
    else if ( d->m_modelstr == "H3900" )
        d->m_model = Model_iPAQ_H39xx;
    else if ( d->m_modelstr == "H5400" )
        d->m_model = Model_iPAQ_H5xxx;
    else if ( d->m_modelstr == "H2200" )
        d->m_model = Model_iPAQ_H22xx;
    else if ( d->m_modelstr == "H1910" )
        d->m_model = Model_iPAQ_H191x;
    else if ( d->m_modelstr == "H1940" )
        d->m_model = Model_iPAQ_H1940;
    else if ( d->m_modelstr == "HX4700" )
        d->m_model = Model_iPAQ_HX4700;
    else if ( d->m_modelstr == "H4000" )
        d->m_model = Model_iPAQ_H4xxx;
    else if ( d->m_modelstr == "H4100" )
        d->m_model = Model_iPAQ_H4xxx;
    else if ( d->m_modelstr == "H4300" )
        d->m_model = Model_iPAQ_H4xxx;
    else {
            d->m_modelstr = model.mid(model.findRev("RX"));
            if ( d->m_modelstr == "RX3000" )
                d->m_model = Model_iPAQ_RX3xxx;
            else if ( d->m_modelstr == "RX1950" )
                d->m_model = Model_iPAQ_RX1950;
            else
                d->m_model = Model_Unknown;
    }


    switch ( d->m_model ) {
        case Model_iPAQ_H31xx:
        case Model_iPAQ_H38xx:
            d->m_rotation = Rot90;
            break;
        case Model_iPAQ_H5xxx:
        case Model_iPAQ_H22xx:
        case Model_iPAQ_H191x:
        case Model_iPAQ_H1940:
        case Model_iPAQ_HX4700:
        case Model_iPAQ_H4xxx:
        case Model_iPAQ_RX3xxx:
        case Model_iPAQ_RX1950:
            d->m_rotation = Rot0;
            break;
        case Model_iPAQ_H36xx:
        case Model_iPAQ_H37xx:
        case Model_iPAQ_H39xx:
            d->m_rotation = Rot270;
        break;
        default:
            d->m_rotation = Rot0;
            break;

        }

    m_power_timer = 0;

}

void iPAQ::initButtons()
{
    if ( d->m_buttons )
        return;

    if ( isQWS( ) ) {
        addPreHandler(this);
    }

    ODevice::initButtons();
}

void iPAQ::initButtonCombos()
{
    if ( d->m_buttonCombos )
        return;

    d->m_buttonCombos = new QValueList<ODeviceButtonCombo>;
    loadButtonCombos( ipaq_combos, sizeof( ipaq_combos ) / sizeof( ODeviceButtonComboStruct ) );
}

bool iPAQ::filter ( int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat )
{
    int newkeycode = keycode;

    switch ( keycode ) {
        // H38xx/H39xx have no "Q" key anymore - this is now the Mail key
        case HardKey_Menu: {
            if (( d->m_model == Model_iPAQ_H38xx ) ||
                ( d->m_model == Model_iPAQ_H39xx ) ||
                ( d->m_model == Model_iPAQ_H5xxx)) {
                newkeycode = HardKey_Mail;
            }
            break;
        }

        // QT has strange screen coordinate system, so depending
        // on native device screen orientation, we need to rotate cursor keys
        case Key_Left :
        case Key_Right:
        case Key_Up   :
        case Key_Down : {
            int quarters;
            switch (d->m_rotation) {
                case Rot0:   quarters = 0/*0deg*/; break;
                case Rot90:  quarters = 3/*90deg*/; break;
                case Rot180: quarters = 2/*180deg*/; break;
                case Rot270: quarters = 1/*270deg*/; break;
            }
            newkeycode = Key_Left + ( keycode - Key_Left + quarters + qt_screen->transformOrientation() ) % 4;
            break;
        }

        // map Power Button short/long press to F34/F35
        case HardKey_Suspend: // Hope we don't have infinite recursion here
        case Key_SysReq: {
            if ( isPress ) {
                if ( m_power_timer )
                    killTimer ( m_power_timer );
                m_power_timer = startTimer ( 500 );
            }
            else if ( m_power_timer ) {
                killTimer ( m_power_timer );
                m_power_timer = 0;
                QWSServer::sendKeyEvent ( -1, HardKey_Suspend, 0, true, false );
                QWSServer::sendKeyEvent ( -1, HardKey_Suspend, 0, false, false );
            }
            newkeycode = Key_unknown;
            break;
        }
    }

    if ( newkeycode != keycode ) {
        if ( newkeycode != Key_unknown )
            QWSServer::sendKeyEvent ( -1, newkeycode, modifiers, isPress, autoRepeat );
        return true;
    }
    else
        return false;
}

void iPAQ::timerEvent ( QTimerEvent * )
{
    killTimer ( m_power_timer );
    m_power_timer = 0;
    QWSServer::sendKeyEvent ( -1, HardKey_Backlight, 0, true, false );
    QWSServer::sendKeyEvent ( -1, HardKey_Backlight, 0, false, false );
}


bool iPAQ::setDisplayBrightness ( int bright )
{
    bool res = false;

    if ( bright > 255 )
        bright = 255;
    if ( bright < 0 )
        bright = 0;

    res = ODevice::setDisplayBrightness( bright );
    if ( ! res ) {
        int fd;
        if (( fd = ::open ( "/dev/touchscreen/0", O_WRONLY )) >= 0 ) {
            FLITE_IN bl;
            bl. mode = 1;
            bl. pwr = bright ? 1 : 0;
            bl. brightness = ( bright * ( displayBrightnessResolution() - 1 ) + 127 ) / 255;
            res = ( ::ioctl ( fd, FLITE_ON, &bl ) == 0 );
            ::close ( fd );
        }
    }

    return res;
}

int iPAQ::displayBrightnessResolution() const
{
    int res = ODevice::displayBrightnessResolution();
    if( res != 16 )
        return res;

    switch ( model()) {
        case Model_iPAQ_H31xx:
        case Model_iPAQ_H36xx:
        case Model_iPAQ_H37xx:
            return 128;     // really 256, but >128 could damage the LCD

        case Model_iPAQ_H38xx:
        case Model_iPAQ_H39xx:
            return 64;
        case Model_iPAQ_H5xxx:
        case Model_iPAQ_HX4700:
        case Model_iPAQ_H4xxx:
        case Model_iPAQ_H191x:
            return 255;
        case Model_iPAQ_H1940:
        case Model_iPAQ_RX1950:
            return 44;
        case Model_iPAQ_RX3xxx:
            return 900;
        default:
            return 2;
    }
}

bool iPAQ::setDisplayStatus ( bool on )
{
    bool res = false;

    QString cmdline;

    QDir sysClass( "/sys/class/lcd/" );
    sysClass.setFilter(QDir::Dirs);
    if ( sysClass.exists() && sysClass.count() > 2 ) {
        QString sysClassPath = sysClass.absFilePath( sysClass[2] + "/power" );
        int fd = ::open( sysClassPath, O_WRONLY|O_NONBLOCK );
        if ( fd >= 0 ) {
            char buf[10];
            buf[0] = on ? 0 : 4;
            buf[1] = '\0';
            res = ( ::write( fd, &buf[0], 2 ) == 0 );
            ::close( fd );
        }
    } else {
         res = OAbstractMobileDevice::setDisplayStatus(on);
    }

    return res;
}

bool iPAQ::hasLightSensor() const
{
    switch (model()) {
        case Model_iPAQ_H191x:
        case Model_iPAQ_H22xx:
        case Model_iPAQ_H4xxx:
        case Model_iPAQ_RX3xxx:
        case Model_iPAQ_RX1950:
        case Model_iPAQ_H1940:
            return false;
        default:
            return true;
    }
}

int iPAQ::readLightSensor()
{
    int fd;
    int val = -1;

    if (( fd = ::open ( "/proc/hal/light_sensor", O_RDONLY )) >= 0 ) {
        char buffer [8];

        if ( ::read ( fd, buffer, 5 ) == 5 ) {
            char *endptr;

            buffer [4] = 0;
            val = ::strtol ( buffer + 2, &endptr, 16 );

            if ( *endptr != 0 )
                val = -1;
        }
        ::close ( fd );
    }

    return val;
}

int iPAQ::lightSensorResolution() const
{
    return 256;
}
