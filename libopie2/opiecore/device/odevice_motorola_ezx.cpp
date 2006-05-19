/*
 � � � � � � � �             This file is part of the Opie Project
              =.             (C) 2006 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#include "odevice_motorola_ezx.h"

/* OPIE */
#include <opie2/oinputsystem.h>
#include <opie2/oresource.h>

#include <qpe/config.h>
#include <qpe/sound.h>

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>
#include <qcopchannel_qws.h>

/* STD */
#include <assert.h>
#include <string.h>
#include <errno.h>
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

struct ezx_button ezx_buttons [] = {
    { Qt::Key_F9, QT_TRANSLATE_NOOP("Button", "Calendar Button"),
    "devicebuttons/z_calendar",
    "datebook", "nextView()",
    "today", "raise()" },
    { Qt::Key_F10, QT_TRANSLATE_NOOP("Button", "Contacts Button"),
    "devicebuttons/z_contact",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
    { Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Home Button"),
    "devicebuttons/z_home",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    { Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Menu Button"),
    "devicebuttons/z_menu",
    "QPE/TaskBar", "toggleMenu()",
    "QPE/TaskBar", "toggleStartMenu()" },
    { Qt::Key_F13, QT_TRANSLATE_NOOP("Button", "Mail Button"),
    "devicebuttons/z_mail",
    "opiemail", "raise()",
    "opiemail", "newMail()" },
};

void Motorola_EZX::init(const QString& cpu_info)
{
    qDebug( "Motorola_EZX::init()" );
    // Set the time to wait until the system is really suspended
    // the delta between apm --suspend and sleeping
    setAPMTimeOut( 15000 );

    d->m_vendorstr = "OpenEZX Team";
    d->m_systemstr = "OpenEZX";
    d->m_system = System_OpenEZX;
    // sysver already gathered

    // check the Motorola_EZX model
    QString model;
    int loc = cpu_info.find( ":" );
    if ( loc != -1 )
        model = cpu_info.mid( loc+2 ).simplifyWhiteSpace();
    else
        model = cpu_info;

    if ( model == "Motorola Ezx Platform" ) {
        d->m_model = Model_Motorola_EZX;
        d->m_modelstr = "Motorola_EZX";
    } else assert( 0 );

    // set path to backlight device in kernel 2.6
    m_backlightdev = "/sys/class/backlight/ezx-bl/";
    d->m_rotation = Rot0;
    //initHingeSensor();

    // set default qte driver and led state
    d->m_qteDriver = "Transformed";
    m_leds[0] = Led_Off;

    qDebug( "Motorola_EZX::init() - Using the 2.6 OpenEZX HAL on a %s", (const char*) d->m_modelstr );
}

void Motorola_EZX::initButtons()
{
    qDebug( "Motorola_EZX::initButtons()" );
    if ( d->m_buttons )
        return;

    d->m_buttons = new QValueList <ODeviceButton>;

    struct ezx_button * ezx_buttons;
    int buttoncount;

/*

    switch ( d->m_model )
    {
        case Model_Motorola_EZX_SL6000:
            pezx_buttons = ezx_buttons_6000;
            buttoncount = ARRAY_SIZE(ezx_buttons_6000);
            break;
        case Model_Motorola_EZX_SLC3100: // fallthrough
        case Model_Motorola_EZX_SLC3000: // fallthrough
        case Model_Motorola_EZX_SLC1000: // fallthrough
        case Model_Motorola_EZX_SLC7x0:
            if ( isQWS( ) )
            {
                addPreHandler(this);
            }
            pezx_buttons = ezx_buttons_c700;
            buttoncount = ARRAY_SIZE(ezx_buttons_c700);
            break;
        default:
            pezx_buttons = ezx_buttons;
            buttoncount = ARRAY_SIZE(ezx_buttons);
            break;
    }

    for ( int i = 0; i < buttoncount; i++ ) {
        struct ezx_button *zb = pezx_buttons + i;
        ODeviceButton b;

        b.setKeycode( zb->code );
        b.setUserText( QObject::tr( "Button", zb->utext ));
        b.setPixmap( OResource::loadPixmap( zb->pix ));
        b.setFactoryPresetPressedAction( OQCopMessage( makeChannel ( zb->fpressedservice ), zb->fpressedaction ));
        b.setFactoryPresetHeldAction( OQCopMessage( makeChannel ( zb->fheldservice ), zb->fheldaction ));
        d->m_buttons->append( b );
    }
*/
    reloadButtonMapping();
}

void Motorola_EZX::playAlarmSound()
{
#ifndef QT_NO_SOUND
    static Sound snd ( "alarm" );
    if(!snd.isFinished())
    return;

    changeMixerForAlarm(0, "/dev/sound/mixer", &snd );
    snd. play();
#endif
}

QValueList <OLed> Motorola_EZX::ledList() const
{
    QValueList <OLed> vl;
    vl << Led_Mail;
    return vl;
}

QValueList <OLedState> Motorola_EZX::ledStateList( OLed l ) const
{
    QValueList <OLedState> vl;

    if ( l == Led_Mail )
        vl << Led_Off << Led_On << Led_BlinkSlow;
    return vl;
}

OLedState Motorola_EZX::ledState( OLed which ) const
{
    if ( which == Led_Mail )
        return m_leds [0];
    else
        return Led_Off;
}

bool Motorola_EZX::setLedState( OLed which, OLedState st )
{
     // Currently not supported
    qDebug( "Motorola_EZX::setLedState: ODevice handling not yet implemented" );
    return false;
}

int Motorola_EZX::displayBrightnessResolution() const
{
     // Currently not supported
    qDebug( "Motorola_EZX::displayBrightnessResolution: ODevice handling not yet implemented" );
    return 100;

#if 0
    int res = 1;
    if (m_embedix)
    {
        int fd = ::open( SHARP_FL_IOCTL_DEVICE, O_RDWR|O_NONBLOCK );
        if ( fd )
        {
            int value = ::ioctl( fd, SHARP_FL_IOCTL_GET_STEP, 0 );
            ::close( fd );
            return value ? value : res;
        }
    }
    else
    {
        int fd = ::open( m_backlightdev + "max_brightness", O_RDONLY|O_NONBLOCK );
        if ( fd )
        {
            char buf[100];
            if ( ::read( fd, &buf[0], sizeof buf ) ) ::sscanf( &buf[0], "%d", &res );
            ::close( fd );
        }
    }
    return res;
#endif
}

bool Motorola_EZX::setDisplayBrightness( int bright )
{
    qDebug( "Motorola_EZX::setDisplayBrightness( %d )", bright );
    return false;

#if 0
    bool res = false;

    if ( bright > 255 ) bright = 255;
    if ( bright < 0 ) bright = 0;

    int numberOfSteps = displayBrightnessResolution();
    int val = ( bright == 1 ) ? 1 : ( bright * numberOfSteps ) / 255;

    if ( m_embedix )
    {
        int fd = ::open( SHARP_FL_IOCTL_DEVICE, O_WRONLY|O_NONBLOCK );
        if ( fd )
        {
            res = ( ::ioctl( fd, SHARP_FL_IOCTL_STEP_CONTRAST, val ) == 0 );
            ::close( fd );
        }
    }
    else
    {
        int fd = ::open( m_backlightdev + "brightness", O_WRONLY|O_NONBLOCK );
        if ( fd )
        {
            char buf[100];
            int len = ::snprintf( &buf[0], sizeof buf, "%d", val );
            res = ( ::write( fd, &buf[0], len ) == 0 );
            ::close( fd );
        }
    }
    return res;
#endif
}

bool Motorola_EZX::setDisplayStatus( bool on )
{
    bool res = false;
    int fd = ::open( m_backlightdev + "power", O_WRONLY|O_NONBLOCK );
    if ( fd )
    {
        char buf[10];
        buf[0] = on ? FB_BLANK_UNBLANK : FB_BLANK_POWERDOWN;
        buf[1] = '\0';
        res = ( ::write( fd, &buf[0], 2 ) == 0 );
        ::close( fd );
    }
    return res;
}

void Motorola_EZX::systemMessage( const QCString &msg, const QByteArray & )
{
    if ( msg == "deviceButtonMappingChanged()" ) {
        reloadButtonMapping();
    }
}

bool Motorola_EZX::suspend() {
    if ( !isQWS( ) ) // only qwsserver is allowed to suspend
        return false;

    bool res = false;
    QCopChannel::send( "QPE/System", "aboutToSuspend()" );

    struct timeval tvs, tvn;
    ::gettimeofday ( &tvs, 0 );

    ::sync(); // flush fs caches
    res = ( ::system ( "apm --suspend" ) == 0 );

    QCopChannel::send( "QPE/System", "returnFromSuspend()" );

    return res;
}
