/*
 � � � � � � � �             This file is part of the Opie Project
� � � � � � �                Copyright (C) The Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This program is free software; you can
.> <`_, � > �. � <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU Library General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
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

#include "odevice_simpad.h"

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>

/* OPIE */
#include <qpe/config.h>
#include <qpe/resource.h>
#include <qpe/sound.h>
#include <qpe/qcopenvelope_qws.h>
#include <opie2/okeyfilter.h>

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

struct s_button simpad_buttons [] = {
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F9, QT_TRANSLATE_NOOP("Button", "Lower+Up"),
    "devicebuttons/simpad_lower_up",
    "datebook", "nextView()",
    "today", "raise()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F10, QT_TRANSLATE_NOOP("Button", "Lower+Down"),
    "devicebuttons/simpad_lower_down",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Lower+Right"),
    "devicebuttons/simpad_lower_right",
    "QPE/TaskBar", "toggleMenu()",
    "QPE/TaskBar", "toggleStartMenu()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F13, QT_TRANSLATE_NOOP("Button", "Lower+Left"),
    "devicebuttons/simpad_lower_left",
    "opiemail", "raise()",
    "opiemail", "newMail()" },

    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F5, QT_TRANSLATE_NOOP("Button", "Upper+Up"),
    "devicebuttons/simpad_upper_up",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F6, QT_TRANSLATE_NOOP("Button", "Upper+Down"),
    "devicebuttons/simpad_upper_down",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F7, QT_TRANSLATE_NOOP("Button", "Upper+Right"),
    "devicebuttons/simpad_upper_right",
    "QPE/TaskBar", "toggleMenu()",
    "QPE/TaskBar", "toggleStartMenu()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F13, QT_TRANSLATE_NOOP("Button", "Upper+Left"),
    "devicebuttons/simpad_upper_left",
    "QPE/Rotation", "flip()",
    "QPE/Rotation", "flip()" },
    /*
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Lower+Upper"),
    "devicebuttons/simpad_lower_upper",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Lower+Upper"),
    "devicebuttons/simpad_upper_lower",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    */
};

void SIMpad::init(const QString&)
{
    d->m_vendorstr = "SIEMENS";
    d->m_vendor = Vendor_SIEMENS;


    //TODO Implement model checking
    //FIXME For now we assume an SL4

    d->m_modelstr = "SL4";
    d->m_model = Model_SIMpad_SL4;

    switch ( d->m_model ) {
        default:
            d->m_rotation = Rot0;
            d->m_direction = CCW;
            d->m_holdtime = 1000; // 1000ms

            break;
    }

    //Distribution detecting code is now in base class
    m_leds [0] = m_leds [1] = Led_Off;

    m_power_timer = 0;

}

void SIMpad::initButtons()
{
    if ( d->m_buttons )
        return;

    if ( isQWS( ) ) {
        addPreHandler(this);
    }


    d->m_buttons = new QValueList <ODeviceButton>;

    for ( uint i = 0; i < ( sizeof( simpad_buttons ) / sizeof( s_button )); i++ ) {
        s_button *sb = simpad_buttons + i;
        ODeviceButton b;

        if (( sb->model & d->m_model ) == d->m_model ) {
            b. setKeycode ( sb->code );
            b. setUserText ( QObject::tr ( "Button", sb->utext ));
            b. setPixmap ( Resource::loadPixmap ( sb->pix ));
            b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( sb->fpressedservice ), sb->fpressedaction ));
            b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( sb->fheldservice ), sb->fheldaction ));

            d->m_buttons->append ( b );
        }
    }
    reloadButtonMapping();

    QCopChannel *sysch = new QCopChannel ( "QPE/System", this );
    connect ( sysch, SIGNAL( received(const QCString&,const QByteArray&)), this, SLOT( systemMessage(const QCString&,const QByteArray&)));
}

// SIMpad boardcontrol register CS3
#define SIMPAD_BOARDCONTROL "/proc/cs3"
#define SIMPAD_VCC_5V_EN                   0x0001 // For 5V PCMCIA
#define SIMPAD_VCC_3V_EN                   0x0002 // FOR 3.3V PCMCIA
#define SIMPAD_EN1                         0x0004 // This is only for EPROM's
#define SIMPAD_EN0                         0x0008 // Both should be enable for 3.3V or 5V
#define SIMPAD_DISPLAY_ON                  0x0010
#define SIMPAD_PCMCIA_BUFF_DIS             0x0020
#define SIMPAD_MQ_RESET                    0x0040
#define SIMPAD_PCMCIA_RESET                0x0080
#define SIMPAD_DECT_POWER_ON               0x0100
#define SIMPAD_IRDA_SD                     0x0200 // Shutdown for powersave
#define SIMPAD_RS232_ON                    0x0400
#define SIMPAD_SD_MEDIAQ                   0x0800 // Shutdown for powersave
#define SIMPAD_LED2_ON                     0x1000
#define SIMPAD_IRDA_MODE                   0x2000 // Fast/Slow IrDA mode
#define SIMPAD_ENABLE_5V                   0x4000 // Enable 5V circuit
#define SIMPAD_RESET_SIMCARD               0x8000

//SIMpad touchscreen backlight strength control
#define SIMPAD_BACKLIGHT_CONTROL "/proc/driver/mq200/registers/PWM_CONTROL"
#define SIMPAD_BACKLIGHT_MASK    0x00a10044


/*
 * The SIMpad exposes ChipSelect3 to userspace
 * via a proc filesystem file. Using this register
 * one can toggle power of serial, irda, dect circuits
 * change the video driver and display status and
 * many more things.
 * To not lose the current setting we read the current
 * cs3 setting and toggle the necessary bits and then
 * write it.
 */
static bool setCS3Bit(  bool bitset, int bit ) {
    int cs3_fd = ::open( SIMPAD_BOARDCONTROL, O_RDONLY );

    if ( cs3_fd < 0 )
        return false;

    static char line[32];
    int  val = 0;
    bool ok  = false;

    /*
     * try to read and parse the Chipselect3 status
     * be paranoid and make sure line[31] is null
     * terminated
     */
    while( !ok && ::read(cs3_fd, &line, sizeof(line)) > 0 ) {
        line[31] = '\0';
        if (::sscanf(line, "Chipselect3 : %x", &val ))
            ok = true;
    }

    ::close(cs3_fd);

    /*
     * we were not able to find the current value
     * and as a result we won't set it
     */
    if ( !ok )
        return false;

    /*
     * change the value
     */
    val = bitset ? (val | bit) : (val & ~bit);

    /*
     * write it back
     */
    cs3_fd = ::open( SIMPAD_BOARDCONTROL, O_WRONLY );
    if ( cs3_fd < 0 )
        return false;

    ::snprintf(line, sizeof(line), "0x%04x\n", val);
    ::write(cs3_fd, line, strlen(line));
    ::close(cs3_fd);

    return true;
}


QValueList <OLed> SIMpad::ledList() const
{
    QValueList <OLed> vl;
    vl << Led_Power; //FIXME which LED is LED2 ? The green one or the amber one?
    //vl << Led_Mail; //TODO find out if LED1 is accessible anyway
    return vl;
}

QValueList <OLedState> SIMpad::ledStateList ( OLed l ) const
{
    QValueList <OLedState> vl;

    if ( l == Led_Power )  //FIXME which LED is LED2 ? The green one or the amber one?
        vl << Led_Off << Led_On;
    //else if ( l == Led_Mail ) //TODO find out if LED1 is accessible anyway
        //vl << Led_Off;
    return vl;
}

OLedState SIMpad::ledState ( OLed l ) const
{
    switch ( l ) {
        case Led_Power:
            return m_leds [0];
        //case Led_Mail:
        //  return m_leds [1];
        default:
            return Led_Off;
    }
}

bool SIMpad::setLedState ( OLed l, OLedState st )
{
    if ( l == Led_Power ) {
        m_leds [0] = st;
        setCS3Bit(st == Led_On, SIMPAD_LED2_ON);
	return true;
    }

    return false;
}


bool SIMpad::filter ( int /*unicode*/, int /*keycode*/, int /*modifiers*/, bool /*isPress*/, bool /*autoRepeat*/ )
{
    //TODO
    return false;
}

void SIMpad::timerEvent ( QTimerEvent * )
{
    killTimer ( m_power_timer );
    m_power_timer = 0;
    QWSServer::sendKeyEvent ( -1, HardKey_Backlight, 0, true, false );
    QWSServer::sendKeyEvent ( -1, HardKey_Backlight, 0, false, false );
}


void SIMpad::playAlarmSound()
{
#ifndef QT_NO_SOUND
    static Sound snd ( "alarm" );

    /* save as the Sound is static! */
    changeMixerForAlarm( 0, "/dev/sound/mixer" , &snd);
    snd. play();
#endif
}


bool SIMpad::suspend() // Must override because SIMpad does NOT have apm
{
    qDebug( "ODevice for SIMpad: suspend()" );
    if ( !isQWS( ) ) // only qwsserver is allowed to suspend
        return false;

    /*
     * we need to save the screen content
     * then go to suspend using ODevice::suspend
     * and finally restore the screen content
     */
    (void)::system( "cat /dev/fb/0 > /tmp/.buffer" );
    bool res  = ODevice::suspend();

    /*
     * restore
     */
    if ( res )
        ::system( "cat /tmp/.buffer > /dev/fb/0" );

    return res;
}


bool SIMpad::setSoftSuspend ( bool soft )
{
    qDebug( "ODevice for SIMpad: UNHANDLED setSoftSuspend(%s)", soft? "on" : "off" );
    return false;
}


bool SIMpad::setDisplayStatus ( bool on )
{
    qDebug( "ODevice for SIMpad: setDisplayStatus(%s)", on? "on" : "off" );

    return setCS3Bit(on, SIMPAD_DISPLAY_ON);
}


bool SIMpad::setDisplayBrightness ( int bright )
{
    qDebug( "ODevice for SIMpad: setDisplayBrightness( %d )", bright );
    bool res = false;
    int fd;

    if ( bright > 255 )
        bright = 255;
    if ( bright < 1 )
        bright = 0;

    if (( fd = ::open ( SIMPAD_BACKLIGHT_CONTROL, O_WRONLY )) >= 0 ) {
        int value = 255 - bright;
        const int mask = SIMPAD_BACKLIGHT_MASK;
        value = value << 8;
        value += mask;
        char writeCommand[100];
        const int count = snprintf( writeCommand, sizeof(writeCommand), "0x%x\n", value );
        res = ( ::write ( fd, writeCommand, count ) != -1 );
        ::close ( fd );
    }
    return res;
}


int SIMpad::displayBrightnessResolution() const
{
    return 255; // All SIMpad models share the same display
}

