/*
� � � � � � � �=.            This file is part of the OPIE Project
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
#ifndef __TABS_SETTINGS_H__
#define __TABS_SETTINGS_H__

#include <qwidget.h>
#include <qmap.h>

#include "tabconfig.h"

class QListBox;
class QCheckBox;
class Config;


class TabsSettings : public QWidget {
	Q_OBJECT

public:
	TabsSettings ( QWidget *parent = 0, const char *name = 0 );

	void accept ( );

protected slots:
	void newClicked ( );
	void deleteClicked ( );
	void editClicked ( );

protected:
	void init ( );
	void readTabSettings ( Config & );

private:
	QListBox *m_list;
//	QString currentTab;
	QStringList m_ids;
	QMap <QString, TabConfig> m_tabs;
	QCheckBox *m_busyblink;
};




#endif
