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
#include <qlayout.h>
#include <qslider.h>
#include <qspinbox.h>

#include <opie/odevice.h>

using namespace Opie;

#include "calibration.h"
#include "sensor.h"

Sensor::Sensor ( QStringList &params, QWidget *parent, const char *name )
	: SensorBase ( parent, name, true, WStyle_ContextHelp ), m_params ( params )
{
	int steps = 12; 
	int inter = 2;
	
	int smin = 40;
	int smax = 215;
	int lmin = 1;
	int lmax = 255;
	
	switch ( params. count ( )) {
		case 6: lmax = params [5]. toInt ( );
		case 5: lmin = params [4]. toInt ( );
		case 4: smax = params [3]. toInt ( );
		case 3: smin = params [2]. toInt ( );
		case 2: steps = params [1]. toInt ( );
		case 1: inter = params [0]. toInt ( ) / 1000;
	}

	int xscale = ODevice::inst ( )-> lightSensorResolution ( );
	int yscale = ODevice::inst ( )-> displayBrightnessResolution ( );
	
	QVBoxLayout *lay = new QVBoxLayout ( frame );
	lay-> setMargin ( 2 );
	m_calib = new Calibration ( frame );
	lay-> add ( m_calib );	

	m_calib-> setScale ( QSize ( xscale, yscale ));
	m_calib-> setLineSteps ( steps );
	m_calib-> setInterval ( inter );
	m_calib-> setStartPoint ( QPoint ( smin * xscale / 256, lmax * yscale / 256 ));
	m_calib-> setEndPoint ( QPoint ( smax * xscale / 256, lmin * yscale / 256 ));
	
	interval-> setValue ( inter );
	linesteps-> setValue ( steps );
	
	connect ( interval, SIGNAL( valueChanged(int)), m_calib, SLOT( setInterval(int)));
	connect ( linesteps, SIGNAL( valueChanged(int)), m_calib, SLOT( setLineSteps(int)));
	
	connect ( m_calib, SIGNAL( startPointChanged(const QPoint&)), this, SLOT( pointDrag(const QPoint&)));
	connect ( m_calib, SIGNAL( endPointChanged(const QPoint&)), this, SLOT( pointDrag(const QPoint&)));
}                

void Sensor::accept ( )
{
	int xscale = ODevice::inst ( )-> lightSensorResolution ( );
	int yscale = ODevice::inst ( )-> displayBrightnessResolution ( );

	m_params. clear ( );
	m_params << QString::number ( m_calib-> interval ( ) * 1000 )
	         << QString::number ( m_calib-> lineSteps ( ))
	         << QString::number ( m_calib-> startPoint ( ). x ( ) * 256 / xscale )
	         << QString::number ( m_calib-> endPoint ( ). x ( ) * 256 / xscale )
	         << QString::number ( m_calib-> endPoint ( ). y ( ) * 256 / yscale )
	         << QString::number ( m_calib-> startPoint ( ). y ( ) * 256 / yscale );

	QDialog::accept ( );
}

void Sensor::pointDrag ( const QPoint &p )
{
	emit viewBacklight ( p. y ( ));
}
