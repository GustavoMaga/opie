/*
 * Class to manage Backends.
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation;
 *      either version 2 of the License, or (at your option) any later
 *      version.
 * =====================================================================
 * ToDo: Use plugins 
 * =====================================================================
 * Version: $Id: obackendfactory.h,v 1.2 2002-10-08 09:27:36 eilers Exp $
 * =====================================================================
 * History:
 * $Log: obackendfactory.h,v $
 * Revision 1.2  2002-10-08 09:27:36  eilers
 * Fixed libopie.pro to include the new pim-API.
 * The SQL-Stuff is currently deactivated. Otherwise everyone who wants to
 * compile itself would need to install libsqlite, libopiesql...
 * Therefore, the backend currently uses XML only..
 *
 * Revision 1.1  2002/10/07 17:35:01  eilers
 * added OBackendFactory for advanced backend access
 *
 *
 * =====================================================================
 */
#ifndef __OPIE_BACKENDFACTORY_H_
#define __OPIE_BACKENDFACTORY_H_

#include <qstring.h>
#include <qasciidict.h>
#include <qpe/config.h>

#include "otodoaccessxml.h"
#include "ocontactaccessbackend_xml.h"

#ifdef __USE_SQL
#include "otodoaccesssql.h"
#endif

template<class T>
class OBackendFactory
{
 public:
	OBackendFactory() {};

	enum BACKENDS {
		TODO,
		CONTACT,
		DATE
	};
	
	static T* Default( const QString backendName, const QString& appName ){
	

		Config config( "pimaccess" );
		config.setGroup ( backendName );
		QString backend = config.readEntry( "usebackend" );

		QAsciiDict<int> dict ( 3 );
		dict.setAutoDelete ( TRUE );

		dict.insert( "todo", new int (TODO) );
		dict.insert( "contact", new int (CONTACT) );

		qWarning ("TODO is: %d", TODO);
		qWarning ("CONTACT is: %d", CONTACT);
	
		switch ( *dict.take( backendName ) ){
		case TODO:
#ifdef __USE_SQL
			if ( backend == "sql" ) 
				return (T*) new OTodoAccessBackendSQL("");
#else
			if ( backend == "sql" ) 
				qWarning ("OBackendFactory:: sql Backend not implemented! Using XML instead!");
#endif

			return (T*) new OTodoAccessXML( appName );
		case CONTACT:
			if ( backend == "sql" ) 
				qWarning ("OBackendFactory:: sql Backend not implemented! Using XML instead!");

			return (T*) new OContactAccessBackend_XML( appName );
		case DATE:
			qWarning ("OBackendFactory:: DATE-Backend not implemented!");
			return NULL;
		default:
			return NULL;
		}
	
	
	}
};


#endif
