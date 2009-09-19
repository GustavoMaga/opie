/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 Paul Eggleton <bluelightning@bluelightning.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "opimchangelog_sql.h"

#include <opie2/osqldriver.h>
#include <opie2/osqlresult.h>
#include <opie2/osqlquery.h>
#include <opie2/odebug.h>
#include <opie2/opimtimezone.h>

using namespace Opie;
using namespace Opie::DB;

OPimChangeLog_SQL::OPimChangeLog_SQL( Opie::DB::OSQLDriver* driver, const QString &logTable, const QString &peerTable )
    : m_driver( driver ), m_logTable( logTable ), m_peerTable( peerTable ), m_peerLastSyncLogId( 0 )
    , m_slowSync( true ), m_enabled( true ), m_peersSynced( false )
{
}

void OPimChangeLog_SQL::init()
{
    if ( !m_driver->open() ) {
        owarn << "OPimChangeLog_SQL::init: driver open failed" << oendl;
        return;
    }

    // Create tables and indexes
    QString qu;
    qu = "CREATE TABLE IF NOT EXISTS " + m_logTable;
    qu += " ( logid INTEGER PRIMARY KEY ASC ";
    qu += ",uid INTEGER";
    qu += ",chgtype CHAR(1)";
    qu += " );";
    OSQLRawQuery qry( qu );
    m_driver->query( &qry );

    qu = "CREATE INDEX IF NOT EXISTS " + m_logTable + "_idx1";
    qu += " on " + m_logTable + " ( logid, uid )";
    OSQLRawQuery qry2( qu );
    m_driver->query( &qry2 );

    qu = "CREATE TABLE IF NOT EXISTS " + m_peerTable;
    qu += " ( peerid VARCHAR(10)";
    qu += ",peername VARCHAR(10)";
    qu += ",lastsynclogid INTEGER";
    qu += ",lastsyncdate VARCHAR(10)";
    qu += " );";
    OSQLRawQuery qry3( qu );
    m_driver->query( &qry3 );

    // No point logging anything if no peer has synced yet
    // (since first sync is always a "slow sync")
    m_peersSynced = peersSynced();
}

void OPimChangeLog_SQL::setEnabled( bool enabled )
{
    m_enabled = enabled;
}

uint OPimChangeLog_SQL::peerLastSyncLogId()
{
    return m_peerLastSyncLogId;
}

bool OPimChangeLog_SQL::slowSync()
{
    return m_slowSync;
}

void OPimChangeLog_SQL::addAddEntry( int uid )
{
    if( m_enabled && m_peersSynced ) {
        QString qu;
        qu = "INSERT INTO " + m_logTable + "( uid, chgtype )";
        qu += " VALUES ( " + QString::number( uid ) + ", \"A\" )";
        OSQLRawQuery qry( qu );
        m_driver->query( &qry );
    }
}

void OPimChangeLog_SQL::addUpdateEntry( int uid )
{
    if( m_enabled && m_peersSynced ) {
        if( !entryExists( uid, false ) ) {
            // OK, there's no existing entry, so add one
            QString qu;
            qu = "INSERT INTO " + m_logTable + "( uid, chgtype )";
            qu += " VALUES ( " + QString::number( uid ) + ", \"U\" )";
            OSQLRawQuery qry( qu );
            m_driver->query( &qry );
        }
    }
}

void OPimChangeLog_SQL::addDeleteEntry( int uid )
{
    if( m_enabled && m_peersSynced ) {
        QString qu;

        // Find out if there is an ADD entry for this record since
        // the last sync
        bool newRecord = entryExists( uid, true );

        // Any entries for this record since the last sync are no longer useful
        qu = "DELETE FROM " + m_logTable;
        qu += " WHERE uid = \"" + QString::number( uid ) + "\"";
        qu += " AND logid > (SELECT COALESCE(MAX(lastsynclogid),0) ";
        qu += "    FROM " + m_peerTable + ")";
        
        if( !newRecord ) {
            // This record was added prior to the last sync, thus we need
            // to record the fact that it was deleted for the next sync
            qu += "; INSERT INTO " + m_logTable + " ( uid, chgtype )";
            qu += " VALUES ( " + QString::number( uid ) + ", \"D\" )";
        }
        OSQLRawQuery qry( qu );
        m_driver->query( &qry );
    }
}

void OPimChangeLog_SQL::purge()
{
    OSQLRawQuery qry( "DELETE FROM " + m_logTable );
    m_driver->query( &qry );

    OSQLRawQuery qry2( "DELETE FROM " + m_peerTable );
    m_driver->query( &qry2 );
}

QList<OPimSyncPeer> OPimChangeLog_SQL::peerList()
{
    OSQLRawQuery qry( "SELECT * FROM " + m_peerTable );
    OSQLResult result = m_driver->query( &qry );

    QList<OPimSyncPeer> list;
    list.setAutoDelete( true );
    
    if( result.state() == OSQLResult::Success ) {
        OSQLResultItem::ValueList items = result.results();
        for ( OSQLResultItem::ValueList::Iterator it = items.begin(); it != items.end(); ++it ) {
            QMap<QString,QString> values = (*it).tableString();

            OPimTimeZone to_zone( OPimTimeZone::current() );
            QDateTime dt = to_zone.toDateTime( (time_t) values["lastsyncdate"].toLong() );

            list.append( new OPimSyncPeer( values["peerid"], values["peername"], dt ) );
        }
    }
    else
        odebug << "peerList: Query failed" << oendl;

    return list;
}

void OPimChangeLog_SQL::removePeer( const QString &peerId )
{
    OSQLRawQuery qry( "DELETE FROM " + m_peerTable + " WHERE peerid = \"" + peerId + "\"" );
    m_driver->query( &qry );

    purgeOldData();
}

bool OPimChangeLog_SQL::startSync( const OPimSyncPeer &peer, bool slowSync )
{
    m_peer = peer;
    m_peerLastSyncLogId = 0;
    m_slowSync = true;

    if( !slowSync ) {
        QString qu = "SELECT * FROM " + m_peerTable;
        qu += " WHERE peerid = \"" + m_peer.peerId() + "\"";
        OSQLRawQuery qry( qu );
        OSQLResult result = m_driver->query( &qry );

        if( result.state() == OSQLResult::Success ) {
            if( result.results().count() > 0 ) {
                bool ok = true;
                m_peerLastSyncLogId = result.first().data("lastsynclogid", &ok).toInt();
                m_slowSync = false;
            }
            else
                odebug << "startSync: no record for specified peer " << m_peer.peerId() << oendl;
        }
        else
            odebug << "startSync: Query failed" << oendl;
    }
    
    return !m_slowSync;
}

void OPimChangeLog_SQL::syncDone()
{
    OSQLRawQuery qry( "DELETE FROM " + m_peerTable + " WHERE peerid = \"" + m_peer.peerId() + "\"" );
    m_driver->query( &qry );

    QString qu;
    qu = "INSERT INTO " + m_peerTable;
    qu += " (peerid, peername, lastsynclogid, lastsyncdate )";
    time_t t = time( NULL );
    qu += " VALUES ( \"" + m_peer.peerId() + "\"";
    qu += ", \"" + m_peer.peerName() + "\"";
    qu += ", (SELECT COALESCE(MAX(logid) FROM " + m_logTable + "), 0)";
    qu += ", " + QString::number(t) + " )";
    OSQLRawQuery qry2( qu );
    m_driver->query( &qry2 );

    m_peersSynced = true;
    
    purgeOldData();
}

void OPimChangeLog_SQL::purgeOldData()
{
    QString qu;
    qu = "DELETE FROM " + m_logTable;
    qu += " WHERE logid <= (SELECT MIN(lastsynclogid) FROM " + m_peerTable + ")";
    OSQLRawQuery qry( qu );
    m_driver->query( &qry );
}

bool OPimChangeLog_SQL::entryExists( int uid, bool addedonly )
{
    // Check if there's not already an ADD or UPDATE entry for this uid
    // since the last sync
    QString qu;
    qu = "SELECT COUNT(*) FROM " + m_logTable;
    qu += " WHERE uid = \"" + QString::number( uid ) + "\"";

    if( addedonly )
        qu += " AND chgtype = \"A\"";
    else
        qu += " AND chgtype in (\"U\", \"A\")";

    qu += " AND logid > (SELECT COALESCE(MAX(lastsynclogid),0) ";
    qu += "    FROM " + m_peerTable + ")";
    OSQLRawQuery qry( qu );
    OSQLResult result = m_driver->query( &qry );
    if( result.state() == OSQLResult::Success ) {
        OSQLResultItem item = result.first();
        if( item.tableInt()[0].toInt() > 0 )
            return true;
    }
    else
        odebug << "entryExists: query failed!" << oendl;

    return false;
}

bool OPimChangeLog_SQL::peersSynced()
{
    OSQLRawQuery qry( "SELECT COUNT(*) FROM " + m_peerTable );
    OSQLResult result = m_driver->query( &qry );
    if( result.state() == OSQLResult::Success ) {
        OSQLResultItem item = result.first();
        if( item.tableInt()[0].toInt() > 0 )
            return true;
    }

    return false;
}
