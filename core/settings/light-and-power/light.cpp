/*
                             This file is part of the OPIE Project
� � � � � � � �=.            Copyright (c)  2002 Maximilian Reiss <harlekin@handhelds.org>
� � � � � � �.=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This file is free software; you can
.> <`_, � > �. � <=          redistribute it and/or modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
� � .%`+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This file is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful, but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|`    MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.= � � � = � � � ;      Public License for more details.
++= � -. � � .` � � .:       
�: � � = �...= . :.=-        You should have received a copy of the GNU
�-. � .:....=;==+<;          General Public License along with this file;
� -_. . . � )=. �=           see the file COPYING. If not, write to the
� � -- � � � �:-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "light.h"

#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <qpe/power.h>
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif

#include <qcheckbox.h>
#include <qtabwidget.h>
#include <qslider.h>
#include <qtimer.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qgroupbox.h>

#include <opie/odevice.h>

#include "sensor.h"

using namespace Opie;

LightSettings::LightSettings( QWidget* parent,  const char* name, WFlags )
	: LightSettingsBase( parent, name, false, WStyle_ContextHelp )
{
	m_bres = ODevice::inst ( )-> displayBrightnessResolution ( );
	m_cres = ODevice::inst ( )-> displayContrastResolution ( );

	if ( !ODevice::inst ( )-> hasLightSensor ( )) {
		auto_brightness-> hide ( );
		CalibrateLightSensor-> hide ( );
		auto_brightness_ac-> hide ( );
		CalibrateLightSensor_ac-> hide ( );
	}
	if (m_cres) {
		GroupLight->setTitle(tr("Backlight & Contrast"));
	} else {
		contrast->hide();
		contrast_ac->hide();
	}

	Config config ( "apm" );
	config. setGroup ( "Battery" );

	// battery spinboxes
	interval_dim->      setValue ( config. readNumEntry ( "Dim", 30 ));
	interval_lightoff-> setValue ( config. readNumEntry ( "LightOff", 20 ));
	interval_suspend->  setValue ( config. readNumEntry ( "Suspend", 60 ));

	// battery check and slider
	LcdOffOnly-> setChecked ( config. readBoolEntry ( "LcdOffOnly", false ));

	int bright = config. readNumEntry ( "Brightness", 127 );
	int contr  = config. readNumEntry ( "Contrast", 127 );
	brightness-> setMaxValue ( m_bres - 1 );
	brightness-> setTickInterval ( QMAX( 1, m_bres / 16 ));
	brightness-> setLineStep ( QMAX( 1, m_bres / 16 ));
	brightness-> setPageStep ( QMAX( 1, m_bres / 16 ));
	brightness-> setValue (( bright * ( m_bres - 1 ) + 127 ) / 255 );

	if (m_cres) {
		contrast-> setMaxValue ( m_cres - 1 );
		contrast-> setTickInterval ( QMAX( 1, m_cres / 16 ));
		contrast-> setLineStep ( QMAX( 1, m_cres / 16 ));
		contrast-> setPageStep ( QMAX( 1, m_cres / 16 ));
		contrast-> setValue (( contr * ( m_cres - 1 ) + 127 ) / 255 );
	}

	// light sensor
	auto_brightness-> setChecked ( config. readBoolEntry ( "LightSensor", false ));
	m_sensordata = config. readListEntry ( "LightSensorData", ';' );

	config. setGroup ( "AC" );
	
	// ac spinboxes
	interval_dim_ac->      setValue ( config. readNumEntry ( "Dim", 60 ));
	interval_lightoff_ac-> setValue ( config. readNumEntry ( "LightOff", 120 ));
	interval_suspend_ac->  setValue ( config. readNumEntry ( "Suspend", 0 ));

	// ac check and slider
	LcdOffOnly_ac-> setChecked ( config. readBoolEntry ( "LcdOffOnly", false ));

	bright = config. readNumEntry ( "Brightness", 255 );
	brightness_ac-> setMaxValue ( m_bres - 1 );
	brightness_ac-> setTickInterval ( QMAX( 1, m_bres / 16 ));
	brightness_ac-> setLineStep ( QMAX( 1, m_bres / 16 ));
	brightness_ac-> setPageStep ( QMAX( 1, m_bres / 16 ));
	brightness_ac-> setValue (( bright * ( m_bres - 1 ) + 127 ) / 255 );

	if (m_cres) {
		contr = config. readNumEntry ( "Contrast", 127);
		contrast_ac-> setMaxValue ( m_cres - 1 );
		contrast_ac-> setTickInterval ( QMAX( 1, m_cres / 16 ));
		contrast_ac-> setLineStep ( QMAX( 1, m_cres / 16 ));
		contrast_ac-> setPageStep ( QMAX( 1, m_cres / 16 ));
		contrast_ac-> setValue (( contr * ( m_cres - 1 ) + 127 ) / 255 );
	}

	// light sensor
	auto_brightness_ac-> setChecked ( config. readBoolEntry ( "LightSensor", false ));
	m_sensordata_ac = config. readListEntry ( "LightSensorData", ';' );
	
	// advanced settings
	config. setGroup ( "Warnings" );
	warnintervalBox-> setValue ( config. readNumEntry ( "checkinterval", 10000 ) / 1000 );
	lowSpinBox->      setValue ( config. readNumEntry ( "powerverylow", 10 ) );
	criticalSpinBox-> setValue ( config. readNumEntry ( "powercritical", 5 ) );

	m_resettimer = new QTimer ( this );
	connect ( m_resettimer, SIGNAL( timeout ( )), this, SLOT( resetBacklight ( )));

	if ( PowerStatusManager::readStatus ( ). acStatus ( ) != PowerStatus::Online ) {
		tabs-> setCurrentPage ( 0 );		
	}
	else {
		tabs-> setCurrentPage ( 1 );
	}
	
	connect ( brightness, SIGNAL( valueChanged ( int )), this, SLOT( setBacklight ( int )));
	connect ( brightness_ac, SIGNAL( valueChanged ( int )), this, SLOT( setBacklight ( int )));
	if (m_havecontrast) {
		connect ( contrast,    SIGNAL( valueChanged ( int )), this, SLOT( setContrast ( int )));
		connect ( contrast_ac, SIGNAL( valueChanged ( int )), this, SLOT( setContrast ( int )));
	}
}

LightSettings::~LightSettings ( ) 
{
}

void LightSettings::calibrateSensor ( )
{
	Sensor *s = new Sensor ( m_sensordata, this );
	connect ( s, SIGNAL( viewBacklight ( int )), this, SLOT( setBacklight ( int )));
	s-> showMaximized ( );
	s-> exec ( );
	delete s;
}

void LightSettings::calibrateSensorAC ( )
{
	Sensor *s = new Sensor ( m_sensordata_ac, this );
	connect ( s, SIGNAL( viewBacklight ( int )), this, SLOT( setBacklight ( int )));
	s-> showMaximized ( );
	s-> exec ( );
	delete s;
}

void LightSettings::setBacklight ( int bright )
{
	if ( bright >= 0 )
		bright = bright * 255 / ( m_bres - 1 );
	
	QCopEnvelope e ( "QPE/System", "setBacklight(int)" );
	e << bright;
	
	if ( bright != -1 ) {
		m_resettimer-> stop ( );
		m_resettimer-> start ( 2000, true );
	}	
}

void LightSettings::setContrast ( int contr )
{
	if ( contr >= 0 )
		contr = contr * 255 / ( m_cres - 1 );
	
	QCopEnvelope e ( "QPE/System", "setContrast(int)" );
	e << contr;
	
	if ( contr != -1 ) {
		m_resettimer-> stop ( );
		m_resettimer-> start ( 2000, true );
	}	
}

void LightSettings::resetBacklight ( )
{
	setBacklight ( -1 );
	setContrast ( -1 );
}

void LightSettings::accept ( )
{
	Config config ( "apm" );

	// bat
	config. setGroup ( "Battery" );
	config. writeEntry ( "LcdOffOnly", LcdOffOnly-> isChecked ( ));
	config. writeEntry ( "Dim",        interval_dim-> value ( ));
	config. writeEntry ( "LightOff",   interval_lightoff-> value ( ));
	config. writeEntry ( "Suspend",    interval_suspend-> value ( ));
	config. writeEntry ( "Brightness", brightness-> value ( ) * 255 / ( m_bres - 1 ) );
	if (m_cres)
	config. writeEntry ( "Contrast",   contrast-> value ( ) * 255 / ( m_cres - 1 ) );

	// ac
	config. setGroup ( "AC" );
	config. writeEntry ( "LcdOffOnly", LcdOffOnly_ac-> isChecked ( ));
	config. writeEntry ( "Dim",        interval_dim_ac-> value ( ));
	config. writeEntry ( "LightOff",   interval_lightoff_ac-> value ( ));
	config. writeEntry ( "Suspend",    interval_suspend_ac-> value ( ));
	config. writeEntry ( "Brightness", brightness_ac-> value ( ) * 255 / ( m_bres - 1 ));
	if (m_cres)
	config. writeEntry ( "Contrast",   contrast_ac-> value ( ) * 255 / ( m_cres - 1 ));

	// only make light sensor stuff appear if the unit has a sensor	
	if ( ODevice::inst ( )-> hasLightSensor ( )) {
		config. setGroup ( "Battery" );
		config. writeEntry ( "LightSensor", auto_brightness->isChecked() );
		config. writeEntry ( "LightSensorData", m_sensordata, ';' );
		config. setGroup ( "AC" );
		config. writeEntry ( "LightSensor", auto_brightness_ac->isChecked() );
		config. writeEntry ( "LightSensorData", m_sensordata_ac, ';' );
	}

	// advanced
	config. setGroup ( "Warnings" );
	config. writeEntry ( "check_interval", warnintervalBox-> value ( ) * 1000 );
	config. writeEntry ( "power_verylow",  lowSpinBox-> value ( ));
	config. writeEntry ( "power_critical", criticalSpinBox-> value ( ));
	config. write ( );

	// notify the launcher
	{
		QCopEnvelope e ( "QPE/System", "reloadPowerWarnSettings()" );
	}
	{
		QCopEnvelope e ( "QPE/System", "setScreenSaverInterval(int)" );
		e << -1;
	}
	LightSettingsBase::accept ( );
}

void LightSettings::done ( int r )
{
	m_resettimer-> stop ( );
	resetBacklight ( );
	
	LightSettingsBase::done ( r );
	close ( );
}
