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

#include "odevice_jornada.h"

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

/* KERNEL */
#define OD_IOC(dir,type,number,size)    (( dir << 30 ) | ( type << 8 ) | ( number ) | ( size << 16 ))

#define OD_IO(type,number)              OD_IOC(0,type,number,0)
#define OD_IOW(type,number,size)        OD_IOC(1,type,number,sizeof(size))
#define OD_IOR(type,number,size)        OD_IOC(2,type,number,sizeof(size))
#define OD_IORW(type,number,size)       OD_IOC(3,type,number,sizeof(size))

typedef struct {
  unsigned char OffOnBlink;       /* 0=off 1=on 2=Blink */
  unsigned char TotalTime;        /* Units of 5 seconds */
  unsigned char OnTime;           /* units of 100m/s */
  unsigned char OffTime;          /* units of 100m/s */
} LED_IN;

typedef struct {
        unsigned char mode;
        unsigned char pwr;
        unsigned char brightness;
} FLITE_IN;

#define LED_ON    OD_IOW( 'f', 5, LED_IN )
#define FLITE_ON  OD_IOW( 'f', 7, FLITE_IN )

using namespace Opie::Core;
using namespace Opie::Core::Internal;

struct j_button jornada56x_buttons [] = {
    { Model_Jornada_56x,
    Qt::Key_F10, QT_TRANSLATE_NOOP("Button", "Calendar Button"),
    "devicebuttons/jornada56x_calendar",
    "datebook", "nextView()",
    "today", "raise()" },
    { Model_Jornada_56x,
    Qt::Key_F9, QT_TRANSLATE_NOOP("Button", "Contacts Button"),
    "devicebuttons/jornada56x_contact",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
    { Model_Jornada_56x,
    Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Todo Button"),
    "devicebuttons/jornada56x_todo",
    "todolist", "raise()",
    "todolist", "create()" },
    { Model_Jornada_56x,
    Qt::Key_F8, QT_TRANSLATE_NOOP("Button", "Home Button"),
    "devicebuttons/jornada56x_home",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    { Model_Jornada_56x,
    Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Record Button"),
    "devicebuttons/jornada56x_record",
    "QPE/VMemo", "toggleRecord()",
    "sound", "raise()" },
};

void Jornada::init(const QString& cpu_info)
{
    d->m_vendorstr = "HP";
    d->m_vendor = Vendor_HP;
    
    QString model;
    int loc = cpu_info.find( ":" );
    if ( loc != -1 )
        model = cpu_info.mid( loc+2 ).simplifyWhiteSpace();
    else
        model = cpu_info;
    
    if ( model == "HP Jornada 56x" ) {
        d->m_modelstr = "Jornada 56x";
        d->m_model = Model_Jornada_56x;
    } else if ( model == "HP Jornada 720" ) {
        d->m_modelstr = "Jornada 720";
	d->m_model = Model_Jornada_720;
    }
    
    d->m_rotation = Rot0; //all Jornadas need this rotation
    //Distribution detecting code is now in base class
}

void Jornada::initButtons()
{
    if ( d->m_buttons )
        return;

    d->m_buttons = new QValueList <ODeviceButton>;

    for ( uint i = 0; i < ( sizeof( jornada56x_buttons ) / sizeof( j_button )); i++ ) {
        j_button *ib = jornada56x_buttons + i;
        ODeviceButton b;

        if (( ib->model & d->m_model ) == d->m_model ) {
            b. setKeycode ( ib->code );
            b. setUserText ( QObject::tr ( "Button", ib->utext ));
            b. setPixmap ( Resource::loadPixmap ( ib->pix ));
            b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( ib->fpressedservice ), ib->fpressedaction ));
            b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( ib->fheldservice ), ib->fheldaction ));

            d->m_buttons->append ( b );
        }
    }
    reloadButtonMapping();
}

int Jornada::displayBrightnessResolution() const
{
    return 255;
}


bool Jornada::setDisplayBrightness( int bright )
{
    bool res = false;

    if ( bright > 255 )
        bright = 255;
    if ( bright < 0 )
        bright = 0;

    QString cmdline;

    if ( d->m_model == Model_Jornada_56x ) {
        if ( !bright )
    	    cmdline = QString::fromLatin1( "echo 4 > /sys/class/backlight/sa1100fb/power");
        else
    	    cmdline = QString::fromLatin1( "echo 0 > /sys/class/backlight/sa1100/power; echo %1 > /sys/class/backlight/sa1100/brightness" ).arg( bright );
    } else if ( d->m_model == Model_Jornada_720 ) {
        cmdline = QString::fromLatin1( "echo %1 > /sys/class/backlight/e1356fb/brightness" ).arg( bright );
    }

    // No Global::shellQuote as we gurantee it to be sane
    res = ( ::system( QFile::encodeName(cmdline) ) == 0 );

    return res;
}


bool Jornada::setDisplayStatus ( bool on )
{
    bool res = false;
    
    QString cmdline;
    
    if ( d->m_model == Model_Jornada_56x ) {
        cmdline = QString::fromLatin1( "echo %1 > /sys/class/lcd/sa1100fb/power; echo %2 > /sys/class/backlight/sa1100fb/power").arg( on ? "0" : "4" ).arg( on ? "0" : "4" );
    } else if ( d->m_model == Model_Jornada_720 ) {
        cmdline = QString::fromLatin1( "echo %1 > /sys/class/lcd/e1356fb/power").arg( on ? "0" : "4" );
    }

    res = ( ::system( QFile::encodeName(cmdline) ) == 0 );

    return res;
}

