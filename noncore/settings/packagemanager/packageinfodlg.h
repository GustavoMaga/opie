/*
                    This file is part of the OPIE Project

               =.            Copyright (c)  2003 Dan Williams <drw@handhelds.org>
      .=l.
     .>+-=
_;:,   .>  :=|.         This file is free software; you can
.> <`_,  > .  <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This file is distributed in the hope that
  + . -:.    =       it will be useful, but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=    =    ;      Public License for more details.
++=  -.   .`   .:
:   = ...= . :.=-        You should have received a copy of the GNU
-.  .:....=;==+<;          General Public License along with this file;
 -_. . .  )=. =           see the file COPYING. If not, write to the
  --    :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef PACKAGEINFODLG_H
#define PACKAGEINFODLG_H

#include "opackage.h"

#include <qmultilineedit.h>
#include <qwidget.h>

class QPushButton;

class OPackage;
class OPackageManager;

class PackageInfoDlg : public QWidget
{
    Q_OBJECT

public:
    PackageInfoDlg( QWidget *parent = 0l, OPackageManager *pm = 0l, const QString &package = QString::null );
    ~PackageInfoDlg();

private:
    OPackageManager *m_packman;     // Pointer to application instance of package manager
    OPackage        *m_package;     // Pointer to package to display information for

    // UI controls
    QMultiLineEdit   m_information;   // Multi-line edit to display package information
    QMultiLineEdit   m_files;         // Multi-line edit to display package file list
    QPushButton     *m_retrieveFiles; // Push button to retrieve file list

private slots:
    void slotBtnFileScan();
    void slotInfo( char *info );
    void slotFiles( char *filelist );
};

#endif
