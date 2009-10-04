/*
                             This file is part of the Opie Project

                             Copyright (C)2004, 2005 Dan Williams <drw@handhelds.org>
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

#ifndef OPACKAGEMANAGER_H
#define OPACKAGEMANAGER_H

#include "oipkg.h"

#include <qpe/config.h>

#include <qdict.h>
#include <qobject.h>
#include <qstringlist.h>

class OPackageManager : public QObject
{
    Q_OBJECT

public:
    OPackageManager( Config *config = 0x0, QObject *parent = 0x0, const char *name = 0x0 );

    void loadAvailablePackages();
    void loadInstalledPackages();

    enum Status { All, NotInstalled, Installed, Updated, NotDefined };

    OPackageList *packages();
    OPackageList *filterPackages( const QString &name = QString::null,
                                  const QString &server = QString::null,
                                  const QString &destination = QString::null,
                                  Status status = NotDefined,
                                  const QString &category = QString::null );

    const QStringList &categories() { return m_categories; }
    QStringList        servers();
    QStringList        destinations();

    int compareVersions( const QString &version1, const QString &version2 );

    OConfItem *findConfItem( OConfItem::Type type = OConfItem::NotDefined,
                             const QString &name = QString::null );
    OPackage  *findPackage( const QString &name = QString::null );

    bool configureDlg( bool installOptions = false );
    void saveSettings();

    bool executeCommand( OPackage::Command command = OPackage::NotDefined,
                         const QStringList &parameters = QStringList(),
                         const QString &destination = QString::null, const QObject *receiver = 0x0,
                         const char *slotOutput = 0x0, bool rawOutput = true );

private:
    Config         *m_config;     // Pointer to application configuration file
    OIpkg           m_ipkg;       // OIpkg object to retrieve package/configuration information
    QDict<OPackage> m_packages;   // Global list of available packages
    QStringList     m_categories; // List of all categories

    void parseVersion( const QString &verstr, int *epoch, QString *version, QString *revision );
    int verrevcmp( const char *val, const char *ref );

signals:
    void initStatus( int numSteps );
    void statusText( const QString &status );
    void statusBar( int currStep );
    void signalProgress( const QString &msg, int percentage );
};

#endif
