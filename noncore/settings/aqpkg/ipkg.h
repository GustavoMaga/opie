/*
� � � � � � � �              This file is part of the OPIE Project

               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
� � � � � � �.=l.                                Dan Williams <drw@handhelds.org>
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

#ifndef IPKG_H
#define IPKG_H


/**
  *@author Andy Qua
  */

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlist.h>

#define FORCE_DEPENDS                           0x0001
#define FORCE_REMOVE                            0x0002
#define FORCE_REINSTALL                         0x0004
#define FORCE_OVERWRITE                         0x0008
#define MAKE_LINKS                              0x0010
#define VERBOSE_WGET                            0x0020

namespace Opie {namespace Core {class OProcess;}}

class Ipkg : public QObject
{
    Q_OBJECT
public:
    Ipkg();
    ~Ipkg();
    void runIpkg();
    void createSymLinks();

    void setOption( const char *opt )               { option = opt; }
    void setPackage( const char *pkg )              { package = pkg; }
    void setDestination( const char *dest )         { destination = dest; }
    void setDestinationDir( const char *dir )       { destDir = dir; }
    void setFlags( int fl, int il )                 { flags = fl; infoLevel = il; }
    void setRuntimeDirectory( const char *dir )     { runtimeDir = dir; }

signals:
    void outputText( const QString &text );
    void ipkgFinished();

public slots:
    void linkCommandStdout(Opie::Core::OProcess*, char *buffer, int buflen);
    void commandStdout(Opie::Core::OProcess*, char *buffer, int buflen);
    void commandStderr(Opie::Core::OProcess*, char *buffer, int buflen);
    void linkProcessFinished();
    void processFinished();
    void abort();


private:
    bool createLinks;
    bool aborted;
    bool error;
    QString option;
    QString package;
    QString destination;
    QString destDir;
    QString runtimeDir;
    Opie::Core::OProcess *proc;
    int flags;
    int infoLevel;
    bool finished;

    QList<QString> *dependantPackages;

    int executeIpkgLinkCommand( QStringList *cmd );
    int executeIpkgCommand( QStringList &cmd, const QString option );
    void removeStatusEntry();
    void linkPackage( const QString &packFileName, const QString &dest, const QString &destDir );
    QStringList* getList( const QString &packageFilename, const QString &destDir );
    void processFileList( const QStringList *fileList, const QString &destDir );
    void processLinkDir( const QString &file, const QString &baseDir, const QString &destDir );

};

#endif
