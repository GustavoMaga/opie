/***************************************************************************
                          datamgr.cpp  -  description
                             -------------------
    begin                : Thu Aug 29 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <fstream>
#include <iostream>
using namespace std;

#ifdef QWS
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif

#include <stdio.h>

#include "datamgr.h"
#include "global.h"


QString DataManager::availableCategories = "";
DataManager::DataManager()
    : QObject( 0x0, 0x0 )
{
    activeServer = "";
    availableCategories = "#";
    
    serverList.setAutoDelete( TRUE );
    destList.setAutoDelete( TRUE );
}

DataManager::~DataManager()
{
}

Server *DataManager :: getServer( const char *name )
{
    QListIterator<Server> it( serverList );
    while ( it.current() && it.current()->getServerName() != name )
    {
		++it;
	}

	return it.current();
}

Destination *DataManager :: getDestination( const char *name )
{
    QListIterator<Destination> it( destList );
    while ( it.current() && it.current()->getDestinationName() != name )
    {
		++it;
	}

	return it.current();
}

void DataManager :: loadServers()
{
    // First add our local server - not really a server but
    // the local config (which packages are installed)
    serverList.append( new Server( LOCAL_SERVER, "" ) );
    serverList.append( new Server( LOCAL_IPKGS, "" ) );

#ifdef QWS
    Config cfg( "aqpkg" );
    cfg.setGroup( "destinations" );
#endif

    // Read file from /etc/ipkg.conf
    QString ipkg_conf = IPKG_CONF;
    FILE *fp;
    fp = fopen( ipkg_conf, "r" );
    char line[130];
    QString lineStr;
    if ( fp == NULL )
    {
        cout << "Couldn't open " << ipkg_conf << "! err = " << fp << endl;
        return;
    }
    else
    {
        while ( fgets( line, sizeof line, fp) != NULL )
        {
            lineStr = line;
            if ( lineStr.startsWith( "src" ) || lineStr.startsWith( "#src" ) || lineStr.startsWith( "# src" ) )
            {
                char alias[20];
                char url[100];

                // Looks a little wierd but read up to the r of src (throwing it away),
                // then read up to the next space and throw that away, the alias
                // is next.
                // Should Handle #src, # src, src, and combinations of
                sscanf( lineStr, "%*[^r]%*[^ ] %s %s", alias, url );
                Server *s = new Server( alias, url );
                if ( lineStr.startsWith( "src" ) )
                    s->setActive( true );
                else
                    s->setActive( false );

                serverList.append( s );

            }
            else if ( lineStr.startsWith( "dest" ) )
            {
                char alias[20];
                char path[50];
                sscanf( lineStr, "%*[^ ] %s %s", alias, path );
                Destination *d = new Destination( alias, path );
                bool linkToRoot = true;
#ifdef QWS
                QString key = alias;
                key += "_linkToRoot";
                linkToRoot = cfg.readBoolEntry( key, true );
#endif
                d->linkToRoot( linkToRoot );
                
                destList.append( d );
            }
            else if ( lineStr.startsWith( "option" ) || lineStr.startsWith( "#option" ) )
            {
                char type[20];
                char val[100];
                sscanf( lineStr, "%*[^ ] %s %s", type, val );
                if ( stricmp( type, "http_proxy" ) == 0 )
                {
                    httpProxy = val;
                    if ( lineStr.startsWith( "#" ) )
                        httpProxyEnabled = false;
                    else
                        httpProxyEnabled = true;
                }
                if ( stricmp( type, "ftp_proxy" ) == 0 )
                {
                    ftpProxy = val;
                    if ( lineStr.startsWith( "#" ) )
                        ftpProxyEnabled = false;
                    else
                        ftpProxyEnabled = true;
                }
                if ( stricmp( type, "proxy_username" ) == 0 )
                    proxyUsername = val;
                if ( stricmp( type, "proxy_password" ) == 0 )
                    proxyPassword = val;
            }
        }
    }
    fclose( fp );

    reloadServerData( );
}

void DataManager :: reloadServerData( )
{
    emit progressSetSteps( serverList.count() );
    emit progressSetMessage( tr( "Reading configuration..." ) );

    QString serverName;
    int i = 0;
    
    Server *server;
    QListIterator<Server> it( serverList );
    for ( ; it.current(); ++it )
    {
        server = it.current();
        serverName = server->getServerName();
        i++;
        emit progressUpdate( i );
        qApp->processEvents();
        
    	// Now we've read the config file in we need to read the servers
    	// The local server is a special case. This holds the contents of the
    	// status files the number of which depends on how many destinations
    	// we've set up
    	// The other servers files hold the contents of the server package list
    	if ( serverName == LOCAL_SERVER )
        	server->readStatusFile( destList );
    	else if ( serverName == LOCAL_IPKGS )
            server->readLocalIpks( getServer( LOCAL_SERVER ) );
    	else
            server->readPackageFile( getServer( LOCAL_SERVER ) );     
	}
}

void DataManager :: writeOutIpkgConf()
{
    QString ipkg_conf = IPKG_CONF;
    ofstream out( ipkg_conf );

    out << "# Written by AQPkg" << endl;
    out << "# Must have one or more source entries of the form:" << endl;
    out << "#" << endl;
    out << "#   src <src-name> <source-url>" << endl;
    out << "#" << endl;
    out << "# and one or more destination entries of the form:" << endl;
    out << "#" << endl;
    out << "#   dest <dest-name> <target-path>" << endl;
    out << "#" << endl;
    out << "# where <src-name> and <dest-names> are identifiers that" << endl;
    out << "# should match [a-zA-Z0-9._-]+, <source-url> should be a" << endl;
    out << "# URL that points to a directory containing a Familiar" << endl;
    out << "# Packages file, and <target-path> should be a directory" << endl;
    out << "# that exists on the target system." << endl << endl;
    
    // Write out servers
    Server *server;
    QListIterator<Server> it( serverList );
    while ( it.current() )
    {
        server = it.current();
        QString alias = server->getServerName();
        // Don't write out local as its a dummy
        if ( alias != LOCAL_SERVER && alias != LOCAL_IPKGS )
        {
            QString url = server->getServerUrl();;

            if ( !server->isServerActive() )
                out << "#";
            out << "src " << alias << " " << url << endl;
        }

        ++it;
    }

    out << endl;

    // Write out destinations
    QListIterator<Destination> it2( destList );
    while ( it2.current() )
    {
        out << "dest " << it2.current()->getDestinationName() << " " << it2.current()->getDestinationPath() << endl;
        ++it2;
    }

    out << endl;
    out << "# Proxy Support" << endl;

    if ( !httpProxyEnabled && httpProxy == "" )
        out << "#option http_proxy http://proxy.tld:3128" << endl;
    else
    {
        if ( !httpProxyEnabled )
            out << "#";
        out << "option http_proxy " << httpProxy << endl;
    }

    if ( !ftpProxyEnabled && ftpProxy == "" )
        out << "#option ftp_proxy http://proxy.tld:3128" << endl;
    else
    {
        if ( !ftpProxyEnabled )
            out << "#";
        out << "option ftp_proxy " << ftpProxy << endl;
    }
    if ( proxyUsername == "" || (!httpProxyEnabled && !ftpProxyEnabled) )
        out << "#option proxy_username <username>" << endl;
    else
        out << "option proxy_username " << proxyUsername << endl;
    if ( proxyPassword == "" || (!httpProxyEnabled && !ftpProxyEnabled) )
        out << "#option proxy_password <password>" << endl << endl;
    else
        out << "option proxy_password " << proxyPassword << endl<< endl;

    out << "# Offline mode (for use in constructing flash images offline)" << endl;
    out << "#option offline_root target" << endl;

    
    out.close();
}


void DataManager :: setAvailableCategories( QString section )
{
    section = section.lower();
    if ( availableCategories.find( "#" + section + "#" ) == -1 )
        availableCategories += section + "#";
}
