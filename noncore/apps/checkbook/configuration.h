/*
� � � � � � � �              This file is part of the OPIE Project
               =.
� � � � � � �.=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <qdialog.h>
#include "cfg.h"

class QCheckBox;
class QLineEdit;
class QString;
class QTabWidget;
class ListEdit;

class Configuration : public QDialog
{
	Q_OBJECT

	public:
        // Constructor
		Configuration( QWidget *, Cfg &cfg);
		~Configuration();

		QLineEdit *symbolEdit;
		QCheckBox *lockCB;
		QCheckBox *balCB;
        QCheckBox *openLastBookCB;
        QCheckBox *lastTabCB;
        QCheckBox *savePayees;
        QTabWidget *_mainWidget;
        ListEdit *_listEditTypes;
        ListEdit *_listEditCategories;
        ListEdit *_listEditPayees;

        // saves settings in config struct
        void saveConfig(Cfg &cfg);

   protected:
        // creates settings tap from configuration
        QWidget *initSettings(Cfg &cfg);
};

#endif
