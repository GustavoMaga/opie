/***************************************************************************
                          datamgr.h  -  description
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

#ifndef DATAMGR_H
#define DATAMGR_H

#include <map>
using namespace std;

#include "server.h"
#include "destination.h"

#define LOCAL_SERVER			"local"
#define LOCAL_IPKGS				"local IPKG"

/**
  *@author Andy Qua
  */


class DataManager
{
public:
    DataManager();
	~DataManager();

    void setActiveServer( const QString &act )      { activeServer = act; }
    QString &getActiveServer( )      				{ return activeServer; }

    Server *getLocalServer()    					{ return getServer( LOCAL_SERVER ); }
    vector<Server> &getServerList()   				{ return serverList; }
    Server *getServer( const char *name );

    vector<Destination> &getDestinationList()  		{ return destList; }
    Destination *getDestination( const char *name );

    void loadServers();
    void reloadServerData( const char *sn );

    void writeOutIpkgConf();


private:
    QString activeServer;

    vector<Server> serverList;
    vector<Destination> destList;
};

#endif
