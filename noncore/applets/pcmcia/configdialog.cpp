/*
 � � � � � � � �             This file is part of the Opie Project
             =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#include "configdialog.h"

/* OPIE */
#include <opie2/oconfig.h>
#include <opie2/odebug.h>
#include <opie2/opcmciasystem.h>
using namespace Opie::Core;

/* QT */
#include <qcombobox.h>
#include <qdir.h>
#include <qfile.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qtextstream.h>

ConfigDialog::ConfigDialog( const OPcmciaSocket* card, QWidget* parent )
             :ConfigDialogBase( parent, "pcmcia config dialog", true )
{
    gbDetails->setTitle( QString( "Details for '%1'" ).arg( card->identity() ) );
    txtCardName->setText( card->productIdentity().join( " " ) );
    txtManfid->setText( card->manufacturerIdentity() );
    txtFunction->setText( card->function() );

    OConfig cfg( "PCMCIA" );
    cfg.setGroup( "Global" );
    int nCards = cfg.readNumEntry( "nCards", 0 );
    QString insert;

    for ( int i = 0; i < nCards; ++i )
    {
        QString cardSection = QString( "Card_%1" ).arg( i );
        cfg.setGroup( cardSection );
        QString name = cfg.readEntry( "name" );
        odebug << "comparing card '" << card->name() << "' with known card '" << name << "'" << oendl;
        if ( card->name() == name )
        {
            insert = cfg.readEntry( "insert" );
            break;
        }
    }
    odebug << "preferred action for card '" << card->name() << "' seems to be '" << insert << "'" << oendl;

    if ( !insert.isEmpty() )
    {
        for ( int i; i < cbAction->count(); ++i )
            if ( cbAction->text( i ) == insert ) cbAction->setCurrentItem( i );
    }

    if ( !card->isUnsupported() )
    {
        odebug << "card is recognized - hiding bindings" << oendl;
        textBindTo->hide();
        cbBindTo->hide();
        return;
    }
    else
    {
        odebug << "card is unsupported yet - showing possible bindings" << oendl;
        textBindTo->show();
        cbBindTo->show();
    }

    // parse possible bind entries out of /etc/pcmcia/*.conf
    typedef QMap<QString,QString> StringMap;
    StringMap bindEntries;

    QDir pcmciaconfdir( "/etc/pcmcia", "*.conf" );

    for ( int i = 0; i < pcmciaconfdir.count(); ++i )
    {
        odebug << "processing conf file '" << pcmciaconfdir[i] << "'" << oendl;
        QString conffilename = QString( "%1/%2" ).arg( pcmciaconfdir.absPath() ).arg( pcmciaconfdir[i] );
        QFile conffile( conffilename );
        if ( conffile.open( IO_ReadOnly ) )
        {
            QTextStream ts( &conffile );
            while ( !ts.atEnd() )
            {
                QString word;
                ts >> word;
                if ( word == "bind" )
                {
                    word = ts.readLine();
                    bindEntries[ word.stripWhiteSpace() ] = conffilename;
                    continue;
                }
                ts.readLine();
            }
        }
        else
        {
            owarn << "couldn't open '" << conffile.name() << "' for reading" << oendl;
            continue;
        }
    }

    for ( StringMap::Iterator it = bindEntries.begin(); it != bindEntries.end(); ++it )
    {
        odebug << "found binding '" << it.key() << "' defined in '" << it.data().latin1() << "'" << oendl;
        cbBindTo->insertItem( it.key() );
    }
}

ConfigDialog::~ConfigDialog()
{
}
