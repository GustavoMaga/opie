/*  This file is part of the OPIE libraries
    Copyright (C) 2002 Robert Griebl (sandman@handhelds.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/time.h>
#include <linux/soundcard.h>
#include <math.h>

#include <qapplication.h>

#include <qfile.h>
#include <qtextstream.h>
#include <qpe/sound.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>

#include "odevice.h"

#include <qwindowsystem_qws.h>


// _IO and friends are only defined in kernel headers ...

#define OD_IOC(dir,type,number,size)    (( dir << 30 ) | ( type << 8 ) | ( number ) | ( size << 16 ))

#define OD_IO(type,number)              OD_IOC(0,type,number,0)
#define OD_IOW(type,number,size)        OD_IOC(1,type,number,sizeof(size))
#define OD_IOR(type,number,size)        OD_IOC(2,type,number,sizeof(size))
#define OD_IORW(type,number,size)       OD_IOC(3,type,number,sizeof(size))

using namespace Opie;

class ODeviceData {
public:
	bool m_qwsserver;

	QString m_vendorstr;
	OVendor m_vendor;
	
	QString m_modelstr;
	OModel m_model;

	QString m_systemstr;
	OSystem m_system;
	
	QString m_sysverstr;

	Transformation m_rotation;
	
	QValueList <ODeviceButton> m_buttons;
	uint                       m_holdtime;
};


class iPAQ : public ODevice, public QWSServer::KeyboardFilter {
protected: 
	virtual void init ( );
	
public:
	virtual bool setSoftSuspend ( bool soft );

	virtual bool setDisplayBrightness ( int b );
	virtual int displayBrightnessResolution ( ) const;

	virtual void alarmSound ( );
	
    virtual QValueList <OLed> ledList ( ) const;
	virtual QValueList <OLedState> ledStateList ( OLed led ) const;
	virtual OLedState ledState ( OLed led ) const;
	virtual bool setLedState ( OLed led, OLedState st );

	virtual bool hasLightSensor ( ) const;
	virtual int readLightSensor ( );
	virtual int lightSensorResolution ( ) const;
	
protected:
	virtual bool filter ( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat );
	virtual void timerEvent ( QTimerEvent *te );
	
	int m_power_timer;
	
	OLedState m_leds [2];
};

class Zaurus : public ODevice {
protected:
	virtual void init ( );

public:	
	virtual bool setSoftSuspend ( bool soft );

	virtual bool setDisplayBrightness ( int b );
	virtual int displayBrightnessResolution ( ) const;

	virtual void alarmSound ( );
	virtual void keySound ( );
	virtual void touchSound ( );	

    virtual QValueList <OLed> ledList ( ) const;
	virtual QValueList <OLedState> ledStateList ( OLed led ) const;
	virtual OLedState ledState ( OLed led ) const;
	virtual bool setLedState ( OLed led, OLedState st );
	
protected:
	virtual void buzzer ( int snd );
	
	OLedState m_leds [1];
};


struct i_button {
	uint model;
    Qt::Key code;
    char *utext;
    char *pix;
    char *fpressedservice;
    char *fpressedaction;
    char *fheldservice;
    char *fheldaction;
} ipaq_buttons [] = {
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx, 
    Qt::Key_F9, QT_TRANSLATE_NOOP("Button", "Calendar Button"), 
	"devicebuttons/ipaq_calendar", 
	"datebook", "nextView()", 
	"today", "raise()" },
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx,
    Qt::Key_F10, QT_TRANSLATE_NOOP("Button", "Contacts Button"), 
	"devicebuttons/ipaq_contact",  
	"addressbook", "raise()",
	"addressbook", "beamBusinessCard()" },
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx, 
    Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Menu Button"), 
	"devicebuttons/ipaq_menu",
 	"QPE/TaskBar", "toggleMenu()",
	"QPE/TaskBar", "toggleStartMenu()" },
    { Model_iPAQ_H38xx | Model_iPAQ_H39xx,
    Qt::Key_F13, QT_TRANSLATE_NOOP("Button", "Mail Button"), 
	"devicebuttons/ipaq_mail",
	"mail", "raise()",
	"mail", "newMail()" },
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx,
    Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Home Button"), 
	"devicebuttons/ipaq_home", 
	"QPE/Launcher", "home()",
	"buttonsettings", "raise()" },
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx,
    Qt::Key_F24, QT_TRANSLATE_NOOP("Button", "Record Button"), 
	"devicebuttons/ipaq_record", 
	"QPE/VMemo", "toggleRecord()",
	"sound", "raise()" },
};

struct z_button {
    Qt::Key code;
    char *utext;
    char *pix;
    char *fpressedservice;
    char *fpressedaction;
    char *fheldservice;
    char *fheldaction;
} z_buttons [] = {
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
	"mail", "raise()",
	"mail", "newMail()" },
};

static QCString makeChannel ( const char *str )
{
	if ( str && !::strchr ( str, '/' ))
		return QCString ( "QPE/Application/" ) + str;
	else
		return str;
}




ODevice *ODevice::inst ( )
{
	static ODevice *dev = 0;
	
	if ( !dev ) {
		if ( QFile::exists ( "/proc/hal/model" ))
			dev = new iPAQ ( );
		else if ( QFile::exists ( "/dev/sharp_buz" ) || QFile::exists ( "/dev/sharp_led" ))
			dev = new Zaurus ( );
		else
			dev = new ODevice ( );
			
		dev-> init ( );
	}
	return dev;
}


/**************************************************
 *
 * common
 *
 **************************************************/


ODevice::ODevice ( )
{
	d = new ODeviceData;

	d-> m_qwsserver = qApp ? ( qApp-> type ( ) == QApplication::GuiServer ) : false;

	d-> m_modelstr = "Unknown";
	d-> m_model = Model_Unknown;
	d-> m_vendorstr = "Unknown";
	d-> m_vendor = Vendor_Unknown;
	d-> m_systemstr = "Unknown";
	d-> m_system = System_Unknown;
	d-> m_sysverstr = "0.0";
	d-> m_rotation = Rot0;
	
	d-> m_holdtime = 1000; // 1000ms
	
	QCopChannel *sysch = new QCopChannel ( "QPE/System", this );
	connect ( sysch, SIGNAL( received( const QCString &, const QByteArray & )), this, SLOT( systemMessage ( const QCString &, const QByteArray & )));	                     	
}

void ODevice::systemMessage ( const QCString &msg, const QByteArray & )
{
	if ( msg == "deviceButtonMappingChanged()" ) {
		reloadButtonMapping ( );
	} 
}

void ODevice::init ( )
{
	// Simulation uses iPAQ 3660 device buttons

	for ( uint i = 0; i < ( sizeof( ipaq_buttons ) / sizeof( i_button )); i++ ) {
		i_button *ib = ipaq_buttons + i;	
		ODeviceButton b;
		
		if (( ib-> model & Model_iPAQ_H36xx ) == Model_iPAQ_H36xx ) {		
			b. setKeycode ( ib-> code );
			b. setUserText ( qApp-> translate ( "Button", ib-> utext ));
			b. setPixmap ( Resource::loadPixmap ( ib-> pix ));
			b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( ib-> fpressedservice ), ib-> fpressedaction ));
			b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( ib-> fheldservice ), ib-> fheldaction ));
                                        
			d-> m_buttons. append ( b );
		}
	}
	reloadButtonMapping ( );
}

ODevice::~ODevice ( )
{
	delete d;
}

bool ODevice::setSoftSuspend ( bool /*soft*/ )
{
	return false;
}

//#include <linux/apm_bios.h>

#define APM_IOC_SUSPEND          OD_IO( 'A', 2 )


bool ODevice::suspend ( )
{
	if ( !d-> m_qwsserver ) // only qwsserver is allowed to suspend
		return false;

	if ( d-> m_model == Model_Unknown ) // better don't suspend in qvfb / on unkown devices
		return false;

	bool res = false;
	
	struct timeval tvs, tvn;
	::gettimeofday ( &tvs, 0 );
	
	::sync ( ); // flush fs caches
	res = ( ::system ( "apm --suspend" ) == 0 );

	// This is needed because the iPAQ apm implementation is asynchronous and we
	// can not be sure when exactly the device is really suspended
	// This can be deleted as soon as a stable familiar with a synchronous apm implementation exists.

	if ( res ) {	
		do { // wait at most 1.5 sec: either suspend didn't work or the device resumed
			::usleep ( 200 * 1000 );
			::gettimeofday ( &tvn, 0 );				
		} while ((( tvn. tv_sec - tvs. tv_sec ) * 1000 + ( tvn. tv_usec - tvs. tv_usec ) / 1000 ) < 1500 );
	}
	
	return res;
}

//#include <linux/fb.h> better not rely on kernel headers in userspace ...

#define FBIOBLANK             OD_IO( 'F', 0x11 ) // 0x4611

/* VESA Blanking Levels */
#define VESA_NO_BLANKING      0
#define VESA_VSYNC_SUSPEND    1
#define VESA_HSYNC_SUSPEND    2
#define VESA_POWERDOWN        3


bool ODevice::setDisplayStatus ( bool on )
{
	if ( d-> m_model == Model_Unknown )
		return false;

	bool res = false;
	int fd;
	
	if (( fd = ::open ( "/dev/fb0", O_RDWR )) >= 0 ) {
		res = ( ::ioctl ( fd, FBIOBLANK, on ? VESA_NO_BLANKING : VESA_POWERDOWN ) == 0 );
		::close ( fd );
	}	                                                                                       
	return res;
}

bool ODevice::setDisplayBrightness ( int )
{
	return false;
}

int ODevice::displayBrightnessResolution ( ) const
{
	return 16;
}

QString ODevice::vendorString ( ) const
{
	return d-> m_vendorstr;
}

OVendor ODevice::vendor ( ) const
{
	return d-> m_vendor;
}

QString ODevice::modelString ( ) const
{
	return d-> m_modelstr;
}

OModel ODevice::model ( ) const
{
	return d-> m_model;
}

QString ODevice::systemString ( ) const
{
	return d-> m_systemstr;
}

OSystem ODevice::system ( ) const
{
	return d-> m_system;
}

QString ODevice::systemVersionString ( ) const
{
	return d-> m_sysverstr;
}

Transformation ODevice::rotation ( ) const
{
	return d-> m_rotation;
}

void ODevice::alarmSound ( )
{
#ifndef QT_NO_SOUND
	static Sound snd ( "alarm" );

	if ( snd. isFinished ( ))
		snd. play ( );
#endif
}

void ODevice::keySound ( )
{
#ifndef QT_NO_SOUND
	static Sound snd ( "keysound" );

	if ( snd. isFinished ( ))
		snd. play ( );
#endif
}

void ODevice::touchSound ( )
{

#ifndef QT_NO_SOUND
	static Sound snd ( "touchsound" );

	if ( snd. isFinished ( ))
		snd. play ( );
#endif
}


QValueList <OLed> ODevice::ledList ( ) const
{
	return QValueList <OLed> ( );
}

QValueList <OLedState> ODevice::ledStateList ( OLed /*which*/ ) const
{
	return QValueList <OLedState> ( );
}

OLedState ODevice::ledState ( OLed /*which*/ ) const
{
	return Led_Off;
}

bool ODevice::setLedState ( OLed /*which*/, OLedState /*st*/ )
{
	return false;
}

bool ODevice::hasLightSensor ( ) const
{
	return false;
}

int ODevice::readLightSensor ( )
{
	return -1;
}

int ODevice::lightSensorResolution ( ) const
{
	return 0;
}

const QValueList <ODeviceButton> &ODevice::buttons ( ) const
{
	return d-> m_buttons;
}

uint ODevice::buttonHoldTime ( ) const
{
	return d-> m_holdtime;
}

const ODeviceButton *ODevice::buttonForKeycode ( ushort code )
{
	for ( QValueListConstIterator<ODeviceButton> it = d-> m_buttons. begin ( ); it != d-> m_buttons. end ( ); ++it ) {
		if ( (*it). keycode ( ) == code )
			return &(*it);
	}
	return 0;
}

void ODevice::reloadButtonMapping ( )
{
	Config cfg ( "ButtonSettings" );
	
	for ( uint i = 0; i < d-> m_buttons. count ( ); i++ ) {
		ODeviceButton &b = d-> m_buttons [i];
		QString group = "Button" + QString::number ( i );

		QCString pch, hch;
		QCString pm, hm;
		QByteArray pdata, hdata;
		
		if ( cfg. hasGroup ( group )) {
			cfg. setGroup ( group );
			pch = cfg. readEntry ( "PressedActionChannel" ). latin1 ( );
			pm  = cfg. readEntry ( "PressedActionMessage" ). latin1 ( );
			// pdata = decodeBase64 ( buttonFile. readEntry ( "PressedActionArgs" ));

			hch = cfg. readEntry ( "HeldActionChannel" ). latin1 ( );
			hm  = cfg. readEntry ( "HeldActionMessage" ). latin1 ( );
			// hdata = decodeBase64 ( buttonFile. readEntry ( "HeldActionArgs" ));
		}
		
		b. setPressedAction ( OQCopMessage ( pch, pm, pdata ));
		b. setHeldAction ( OQCopMessage ( hch, hm, hdata ));
	}
}

void ODevice::remapPressedAction ( int button, const OQCopMessage &action )
{
	if ( button >= (int) d-> m_buttons. count ( ))
		return;
		
	ODeviceButton &b = d-> m_buttons [button];
    b. setPressedAction ( action );

	Config buttonFile ( "ButtonSettings" );
	buttonFile. setGroup ( "Button" + QString::number ( button ));
	buttonFile. writeEntry ( "PressedActionChannel", (const char*) b. pressedAction ( ). channel ( ));
	buttonFile. writeEntry ( "PressedActionMessage", (const char*) b. pressedAction ( ). message ( ));

//	buttonFile. writeEntry ( "PressedActionArgs", encodeBase64 ( b. pressedAction ( ). data ( )));

	QCopEnvelope ( "QPE/System", "deviceButtonMappingChanged()" );
}

void ODevice::remapHeldAction ( int button, const OQCopMessage &action )
{
	if ( button >= (int) d-> m_buttons. count ( ))
		return;
		
	ODeviceButton &b = d-> m_buttons [button];
    b. setHeldAction ( action );

	Config buttonFile ( "ButtonSettings" );
	buttonFile. setGroup ( "Button" + QString::number ( button ));
	buttonFile. writeEntry ( "HeldActionChannel", (const char *) b. heldAction ( ). channel ( ));
	buttonFile. writeEntry ( "HeldActionMessage", (const char *) b. heldAction ( ). message ( ));

//	buttonFile. writeEntry ( "HeldActionArgs", decodeBase64 ( b. heldAction ( ). data ( )));

	QCopEnvelope ( "QPE/System", "deviceButtonMappingChanged()" );
}




/**************************************************
 *
 * iPAQ
 *
 **************************************************/

void iPAQ::init ( )
{
	d-> m_vendorstr = "HP";
	d-> m_vendor = Vendor_HP;

	QFile f ( "/proc/hal/model" );

	if ( f. open ( IO_ReadOnly )) {
		QTextStream ts ( &f );

		d-> m_modelstr = "H" + ts. readLine ( );

		if ( d-> m_modelstr == "H3100" )
			d-> m_model = Model_iPAQ_H31xx;
		else if ( d-> m_modelstr == "H3600" )
			d-> m_model = Model_iPAQ_H36xx;
		else if ( d-> m_modelstr == "H3700" )
			d-> m_model = Model_iPAQ_H37xx;
		else if ( d-> m_modelstr == "H3800" )
			d-> m_model = Model_iPAQ_H38xx;
		else if ( d-> m_modelstr == "H3900" )
			d-> m_model = Model_iPAQ_H39xx;
		else
			d-> m_model = Model_Unknown;

		f. close ( );
	}

	switch ( d-> m_model ) {
		case Model_iPAQ_H31xx:
		case Model_iPAQ_H38xx:
			d-> m_rotation = Rot90;
			break;
		case Model_iPAQ_H36xx:
		case Model_iPAQ_H37xx:
		case Model_iPAQ_H39xx:
		default:
			d-> m_rotation = Rot270;
			break;
	}

	f. setName ( "/etc/familiar-version" );
	if ( f. open ( IO_ReadOnly )) { 
		d-> m_systemstr = "Familiar";
		d-> m_system = System_Familiar;
		
		QTextStream ts ( &f );
		d-> m_sysverstr = ts. readLine ( ). mid ( 10 );
		
		f. close ( );
	}

	m_leds [0] = m_leds [1] = Led_Off;
	
	m_power_timer = 0;
	
	for ( uint i = 0; i < ( sizeof( ipaq_buttons ) / sizeof( i_button )); i++ ) {
		i_button *ib = ipaq_buttons + i;	
		ODeviceButton b;
		
		if (( ib-> model & d-> m_model ) == d-> m_model ) {		
			b. setKeycode ( ib-> code );
			b. setUserText ( qApp-> translate ( "Button", ib-> utext ));
			b. setPixmap ( Resource::loadPixmap ( ib-> pix ));
			b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( ib-> fpressedservice ), ib-> fpressedaction ));
			b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( ib-> fheldservice ), ib-> fheldaction ));
                                        
			d-> m_buttons. append ( b );
		}
	}
	reloadButtonMapping ( );
	
	if ( d-> m_qwsserver )
		QWSServer::setKeyboardFilter ( this );	
}

//#include <linux/h3600_ts.h>  // including kernel headers is evil ...

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


QValueList <OLed> iPAQ::ledList ( ) const
{
	QValueList <OLed> vl;
	vl << Led_Power;

	if ( d-> m_model == Model_iPAQ_H38xx )
		vl << Led_BlueTooth;
	return vl;
}

QValueList <OLedState> iPAQ::ledStateList ( OLed l ) const
{
	QValueList <OLedState> vl;

	if ( l == Led_Power )
		vl << Led_Off << Led_On << Led_BlinkSlow << Led_BlinkFast;
	else if ( l == Led_BlueTooth && d-> m_model == Model_iPAQ_H38xx )
		vl << Led_Off; // << Led_On << ???
		
	return vl;
}

OLedState iPAQ::ledState ( OLed l ) const
{	
	switch ( l ) {
		case Led_Power:
			return m_leds [0];
		case Led_BlueTooth:
			return m_leds [1];
		default:
			return Led_Off;
	}
}

bool iPAQ::setLedState ( OLed l, OLedState st ) 
{
	static int fd = ::open ( "/dev/touchscreen/0", O_RDWR | O_NONBLOCK );

	if ( l == Led_Power ) {
		if ( fd >= 0 ) {
			LED_IN leds;
			::memset ( &leds, 0, sizeof( leds ));
			leds. TotalTime  = 0;
			leds. OnTime     = 0;
			leds. OffTime    = 1;
			leds. OffOnBlink = 2;

			switch ( st ) {
				case Led_Off      : leds. OffOnBlink = 0; break;
				case Led_On       : leds. OffOnBlink = 1; break;
				case Led_BlinkSlow: leds. OnTime = 10; leds. OffTime = 10; break;
				case Led_BlinkFast: leds. OnTime =  5; leds. OffTime =  5; break;
			}

			if ( ::ioctl ( fd, LED_ON, &leds ) >= 0 ) {
				m_leds [0] = st;
				return true;
			}
		}
	}
	return false;
}


bool iPAQ::filter ( int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat )
{
	int newkeycode = keycode;

	switch ( keycode ) {
		// H38xx/H39xx have no "Q" key anymore - this is now the Mail key
		case HardKey_Menu: {
			if (( d-> m_model == Model_iPAQ_H38xx ) ||
			    ( d-> m_model == Model_iPAQ_H39xx )) {			    
				newkeycode = HardKey_Mail;
			}
			break;
		}
				
		// Rotate cursor keys 180�
		case Key_Left : 
		case Key_Right: 
		case Key_Up   : 
		case Key_Down : {
			if (( d-> m_model == Model_iPAQ_H31xx ) ||
			    ( d-> m_model == Model_iPAQ_H38xx )) {			    
				newkeycode = Key_Left + ( keycode - Key_Left + 2 ) % 4;
			}
			break;
		}

		// map Power Button short/long press to F34/F35
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


void iPAQ::alarmSound ( )
{
#ifndef QT_NO_SOUND
	static Sound snd ( "alarm" );
	int fd;
	int vol;
	bool vol_reset = false;

	if (( fd = ::open ( "/dev/sound/mixer", O_RDWR )) >= 0 ) {
		if ( ::ioctl ( fd, MIXER_READ( 0 ), &vol ) >= 0 ) {
			Config cfg ( "qpe" );
			cfg. setGroup ( "Volume" );

 			int volalarm = cfg. readNumEntry ( "AlarmPercent", 50 );
			if ( volalarm < 0 )
				volalarm = 0;
			else if ( volalarm > 100 )
				volalarm = 100;
			volalarm |= ( volalarm << 8 );

			if ( ::ioctl ( fd, MIXER_WRITE( 0 ), &volalarm ) >= 0 )
				vol_reset = true;
		}
	}

	snd. play ( );
	while ( !snd. isFinished ( ))
		qApp-> processEvents ( );

	if ( fd >= 0 ) {
		if ( vol_reset )
			::ioctl ( fd, MIXER_WRITE( 0 ), &vol );
		::close ( fd );
	}
#endif
}


bool iPAQ::setSoftSuspend ( bool soft )
{
	bool res = false;
	int fd;
	
	if (( fd = ::open ( "/proc/sys/ts/suspend_button_mode", O_WRONLY )) >= 0 ) {
		if ( ::write ( fd, soft ? "1" : "0", 1 ) == 1 )
			res = true;
		else
			::perror ( "write to /proc/sys/ts/suspend_button_mode" );
		
		::close ( fd );
	}
	else
		::perror ( "/proc/sys/ts/suspend_button_mode" );
	
	return res;
}


bool iPAQ::setDisplayBrightness ( int bright )
{
	bool res = false;
	int fd;
	
	if ( bright > 255 )
		bright = 255;
	if ( bright < 0 )
		bright = 0;

	if (( fd = ::open ( "/dev/touchscreen/0", O_WRONLY )) >= 0 ) {
		FLITE_IN bl;
		bl. mode = 1;
		bl. pwr = bright ? 1 : 0;
		bl. brightness = ( bright * ( displayBrightnessResolution ( ) - 1 ) + 127 ) / 255;
		res = ( ::ioctl ( fd, FLITE_ON, &bl ) == 0 );
		::close ( fd );
	}
	return res;
}

int iPAQ::displayBrightnessResolution ( ) const
{
	switch ( model ( )) {
		case Model_iPAQ_H31xx:
		case Model_iPAQ_H36xx:
		case Model_iPAQ_H37xx:
			return 128;  	// really 256, but >128 could damage the LCD
			
		case Model_iPAQ_H38xx:
		case Model_iPAQ_H39xx:
			return 64;
		
		default:
			return 2;
	}	
}


bool iPAQ::hasLightSensor ( ) const
{
	return true;
}

int iPAQ::readLightSensor ( )
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

int iPAQ::lightSensorResolution ( ) const
{
	return 256;
}

/**************************************************
 *
 * Zaurus
 *
 **************************************************/



void Zaurus::init ( )
{
	d-> m_vendorstr = "Sharp";
	d-> m_vendor = Vendor_Sharp;

	QFile f ( "/proc/filesystems" );

	if ( f. open ( IO_ReadOnly ) && ( QTextStream ( &f ). read ( ). find ( "\tjffs2\n" ) >= 0 )) {
		d-> m_vendorstr = "OpenZaurus Team";
		d-> m_systemstr = "OpenZaurus";
		d-> m_system = System_OpenZaurus;

		f. close ( );

		f. setName ( "/etc/oz_version" );
		if ( f. open ( IO_ReadOnly )) { 
			QTextStream ts ( &f );
			d-> m_sysverstr = ts. readLine ( );//. mid ( 10 );
			f. close ( );
		}
	}
	else {
		d-> m_systemstr = "Zaurus";
		d-> m_system = System_Zaurus;
	}

	f. setName ( "/proc/deviceinfo/product" );
	if ( f. open ( IO_ReadOnly ) ) {
		QTextStream ts ( &f );
		QString model = ts. readLine ( );
		f. close ( );

		d-> m_modelstr = QString("Zaurus ") + model;
		if ( model == "SL-5500" )
			d-> m_model = Model_Zaurus_SL5500;
		else if ( model == "SL-C700" )
			d-> m_model = Model_Zaurus_SLC700;
		else if ( model == "SL-A300" )
			d-> m_model = Model_Zaurus_SLA300;
		else if ( model == "SL-B600" || model == "SL-5600" )
			d-> m_model = Model_Zaurus_SLB600;
		else
			d-> m_model = Model_Zaurus_SL5000;
	}
	else {
		d-> m_model = Model_Zaurus_SL5000;
		d-> m_modelstr = "Zaurus (model unknown)";
	}

	switch ( d-> m_model ) {
		case Model_Zaurus_SLC700:
			/* note for C700, we must check the display rotation
			 * sensor to set an appropriate value
			 */
		case Model_Zaurus_SLA300:
		case Model_Zaurus_SLB600:
		case Model_Zaurus_SL5500:
		case Model_Zaurus_SL5000:
		default:
			d-> m_rotation = Rot270;
			break;
	}

	for ( uint i = 0; i < ( sizeof( z_buttons ) / sizeof( z_button )); i++ ) {
		z_button *zb = z_buttons + i;	
		ODeviceButton b;
		
		b. setKeycode ( zb-> code );
		b. setUserText ( qApp-> translate ( "Button", zb-> utext ));
		b. setPixmap ( Resource::loadPixmap ( zb-> pix ));
		b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( zb-> fpressedservice ), zb-> fpressedaction ));
		b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( zb-> fheldservice ), zb-> fheldaction ));
                                        
		d-> m_buttons. append ( b );
	}
	reloadButtonMapping ( );

	m_leds [0] = Led_Off;
}

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

//#include <asm/sharp_char.h> // including kernel headers is evil ...

#define SHARP_DEV_IOCTL_COMMAND_START 0x5680

#define	SHARP_BUZZER_IOCTL_START (SHARP_DEV_IOCTL_COMMAND_START)
#define SHARP_BUZZER_MAKESOUND   (SHARP_BUZZER_IOCTL_START)

#define SHARP_BUZ_TOUCHSOUND       1  /* touch panel sound */
#define SHARP_BUZ_KEYSOUND         2  /* key sound */
#define SHARP_BUZ_SCHEDULE_ALARM  11  /* schedule alarm */

/* --- for SHARP_BUZZER device --- */

//#define	SHARP_BUZZER_IOCTL_START (SHARP_DEV_IOCTL_COMMAND_START)
//#define SHARP_BUZZER_MAKESOUND   (SHARP_BUZZER_IOCTL_START)

#define SHARP_BUZZER_SETVOLUME   (SHARP_BUZZER_IOCTL_START+1)
#define SHARP_BUZZER_GETVOLUME   (SHARP_BUZZER_IOCTL_START+2)
#define SHARP_BUZZER_ISSUPPORTED (SHARP_BUZZER_IOCTL_START+3)
#define SHARP_BUZZER_SETMUTE     (SHARP_BUZZER_IOCTL_START+4)
#define SHARP_BUZZER_STOPSOUND   (SHARP_BUZZER_IOCTL_START+5)

//#define SHARP_BUZ_TOUCHSOUND       1  /* touch panel sound */
//#define SHARP_BUZ_KEYSOUND         2  /* key sound */

//#define SHARP_PDA_ILLCLICKSOUND    3  /* illegal click */
//#define SHARP_PDA_WARNSOUND        4  /* warning occurred */
//#define SHARP_PDA_ERRORSOUND       5  /* error occurred */
//#define SHARP_PDA_CRITICALSOUND    6  /* critical error occurred */
//#define SHARP_PDA_SYSSTARTSOUND    7  /* system start */
//#define SHARP_PDA_SYSTEMENDSOUND   8  /* system shutdown */
//#define SHARP_PDA_APPSTART         9  /* application start */
//#define SHARP_PDA_APPQUIT         10  /* application ends */

//#define SHARP_BUZ_SCHEDULE_ALARM  11  /* schedule alarm */
//#define SHARP_BUZ_DAILY_ALARM     12  /* daily alarm */
//#define SHARP_BUZ_GOT_PHONE_CALL  13  /* phone call sound */
//#define SHARP_BUZ_GOT_MAIL        14  /* mail sound */
//

#define	SHARP_LED_IOCTL_START (SHARP_DEV_IOCTL_COMMAND_START)
#define SHARP_LED_SETSTATUS   (SHARP_LED_IOCTL_START+1)

typedef struct sharp_led_status {
  int which;   /* select which LED status is wanted. */
  int status;  /* set new led status if you call SHARP_LED_SETSTATUS */
} sharp_led_status;

#define SHARP_LED_MAIL_EXISTS  9       /* mail status (exists or not) */

#define LED_MAIL_NO_UNREAD_MAIL  0   /* for SHARP_LED_MAIL_EXISTS */
#define LED_MAIL_NEWMAIL_EXISTS  1   /* for SHARP_LED_MAIL_EXISTS */
#define LED_MAIL_UNREAD_MAIL_EX  2   /* for SHARP_LED_MAIL_EXISTS */

// #include <asm/sharp_apm.h> // including kernel headers is evil ...

#define APM_IOCGEVTSRC          OD_IOR( 'A', 203, int )
#define APM_IOCSEVTSRC          OD_IORW( 'A', 204, int )
#define APM_EVT_POWER_BUTTON    (1 << 0)

#define FL_IOCTL_STEP_CONTRAST    100


void Zaurus::buzzer ( int sound )
{
	static int fd = ::open ( "/dev/sharp_buz", O_RDWR|O_NONBLOCK );
  
	if ( fd >= 0 )
		::ioctl ( fd, SHARP_BUZZER_MAKESOUND, sound );
}


void Zaurus::alarmSound ( ) 
{
	buzzer ( SHARP_BUZ_SCHEDULE_ALARM );
}

void Zaurus::touchSound ( ) 
{
	buzzer ( SHARP_BUZ_TOUCHSOUND );
}

void Zaurus::keySound ( ) 
{
	buzzer ( SHARP_BUZ_KEYSOUND );
}


QValueList <OLed> Zaurus::ledList ( ) const
{
	QValueList <OLed> vl;
	vl << Led_Mail;
	return vl;
}

QValueList <OLedState> Zaurus::ledStateList ( OLed l ) const
{
	QValueList <OLedState> vl;
	
	if ( l == Led_Mail )
		vl << Led_Off << Led_On << Led_BlinkSlow;
	return vl;
}

OLedState Zaurus::ledState ( OLed which ) const
{
	if ( which == Led_Mail )
		return m_leds [0];
	else	
		return Led_Off;
}

bool Zaurus::setLedState ( OLed which, OLedState st )
{
	static int fd = ::open ( "/dev/sharp_led", O_RDWR|O_NONBLOCK ); 

	if ( which == Led_Mail ) {
		if ( fd >= 0 ) {
			struct sharp_led_status leds;
			::memset ( &leds, 0, sizeof( leds ));
			leds. which = SHARP_LED_MAIL_EXISTS;
			bool ok = true;
			
			switch ( st ) {
				case Led_Off      : leds. status = LED_MAIL_NO_UNREAD_MAIL; break;
				case Led_On       : leds. status = LED_MAIL_NEWMAIL_EXISTS; break;
				case Led_BlinkSlow: leds. status = LED_MAIL_UNREAD_MAIL_EX; break;
				default            : ok = false;
			}
		
			if ( ok && ( ::ioctl ( fd, SHARP_LED_SETSTATUS, &leds ) >= 0 )) {
				m_leds [0] = st;
				return true;
			}
		}		
	}
	return false;
}

bool Zaurus::setSoftSuspend ( bool soft )
{
	bool res = false;
	int fd;

	if ((( fd = ::open ( "/dev/apm_bios", O_RDWR )) >= 0 ) ||
        (( fd = ::open ( "/dev/misc/apm_bios",O_RDWR )) >= 0 )) {

		int sources = ::ioctl ( fd, APM_IOCGEVTSRC, 0 ); // get current event sources

		if ( sources >= 0 ) {
			if ( soft )
				sources &= ~APM_EVT_POWER_BUTTON;
			else
				sources |= APM_EVT_POWER_BUTTON;

			if ( ::ioctl ( fd, APM_IOCSEVTSRC, sources ) >= 0 ) // set new event sources
				res = true;
			else
				perror ( "APM_IOCGEVTSRC" );
		}
		else
			perror ( "APM_IOCGEVTSRC" );
		
		::close ( fd );
	}
	else
		perror ( "/dev/apm_bios or /dev/misc/apm_bios" );
		
    return res;
}


bool Zaurus::setDisplayBrightness ( int bright )
{
	bool res = false;
	int fd;
	
	if ( bright > 255 )
		bright = 255;
	if ( bright < 0 )
		bright = 0;
	
	if (( fd = ::open ( "/dev/fl", O_WRONLY )) >= 0 ) {
		int bl = ( bright * 4 + 127 ) / 255; // only 4 steps on zaurus
		if ( bright && !bl )
			bl = 1;
		res = ( ::ioctl ( fd, FL_IOCTL_STEP_CONTRAST, bl ) == 0 );
		::close ( fd );
	}
	return res;
}


int Zaurus::displayBrightnessResolution ( ) const 
{
	return 5;
}


