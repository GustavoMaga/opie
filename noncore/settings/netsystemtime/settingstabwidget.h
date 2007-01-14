/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
:     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef SETTINGSTABWIDGET_H
#define SETTINGSTABWIDGET_H

#include <qwidget.h>

class QCheckBox;
class QComboBox;
class QSpinBox;

class SettingsTabWidget : public QWidget
{
	Q_OBJECT

public:
	SettingsTabWidget( QWidget * = 0x0 );
	~SettingsTabWidget();

	void saveSettings();
	QString ntpServer();

private:
	QComboBox *cbTimeServer;
	QSpinBox  *sbNtpDelay;
	QSpinBox  *sbPredictDelay;
	QCheckBox *chShowOnRestart;
	QCheckBox *chNtpTab;
	QCheckBox *chPredictTab;

signals:
	void ntpDelayChanged( int );
	void displayNTPTab( bool );
	void displayPredictTab( bool );
};

#endif
