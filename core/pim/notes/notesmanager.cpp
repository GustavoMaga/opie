/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
            .=l.             
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <opie2/odebug.h>

#include <qpe/categoryselect.h>

#include "notesmanager.h"

using namespace Opie;
using namespace Opie::Notes;

NotesManager::NotesManager( QObject *obj )
    : QObject( obj )
{
    m_db = 0l;
    m_cat.load( categoryFileName() );
}

NotesManager::~NotesManager()
{
    delete m_db;
}

OPimMemo NotesManager::memo(int uid )
{
    return m_db->find( uid );
}

void NotesManager::updateList()
{
    m_list = m_db->allRecords();
}

OPimMemoAccess::List NotesManager::list() const
{
    return m_list;
}

OPimMemoAccess::List NotesManager::sorted( bool asc, int so, int f, int cat )
{
    return m_db->sorted( asc, so, f, cat );
}

OPimMemoAccess::List::Iterator NotesManager::query( const OPimMemo& ev, int query )
{
    m_list = m_db->queryByExample( ev, query );
    m_it = m_list.begin();
    return m_it;
}

void NotesManager::add( const OPimMemo& ev )
{
    m_db->add( ev );
}

void NotesManager::update( int, const OPimMemo& ev )
{
    m_db->replace( ev );
}

bool NotesManager::remove( int uid )
{
    return m_db->remove( uid );
}

void NotesManager::save()
{
    m_db->save();
}

bool NotesManager::saveAll()
{
    return m_db->save();
}

void NotesManager::reload()
{
    m_db->reload();
}

QStringList NotesManager::categories()
{
    m_cat.load( categoryFileName() );
    return m_cat.labels( "Notes");
}

/*
 * we rely on load being called from populateCategories
 */
int NotesManager::catId( const QString& cats )
{
    return m_cat.id( "Notes", cats );
}

bool NotesManager::isLoaded()const
{
    return ( m_db == 0 );
}

void NotesManager::load()
{
    if ( !m_db ) {
        m_db = new OPimMemoAccess();
        m_db->load();
    }
}
