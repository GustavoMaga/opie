/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
              =.             
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

#include "virtualfs.h"

#include <opie2/odatebookaccessbackend_xml.h>
#include <opie2/ocontactaccessbackend_xml.h>
#include <opie2/otodoaccessxml.h>
#include <opie2/ocontactaccess.h>
#include <opie2/odatebookaccess.h>
#include <opie2/otodoaccess.h>

using namespace Opie;

///////////////////////////////////////////////////////////////////////////////////////////

VirtualFile::VirtualFile( const QString &path, VirtualReader *rd, VirtualWriter *wr )
    : m_reader( rd ), m_writer( wr ), m_filePath( path )
{
}

VirtualFile::~VirtualFile() {
    if( m_reader )
        delete m_reader;
    if( m_writer )
        delete m_writer;
}

const QString & VirtualFile::filePath() const
{
    return m_filePath;
}

VirtualReader * VirtualFile::reader() const
{
    return m_reader;
}

VirtualWriter * VirtualFile::writer() const
{
    return m_writer;
}

///////////////////////////////////////////////////////////////////////////////////////////

VirtualFS::VirtualFS()
{
    m_files.setAutoDelete( true );
}

void VirtualFS::init( const QString &basedir )
{
    m_files.clear();
    m_files.append( new VirtualFile( basedir + "/Applications/datebook/datebook.xml", 
                                     new VirtualDateBookReader(), 
                                     new VirtualDateBookWriter() ) );
    m_files.append( new VirtualFile( basedir + "/Applications/addressbook/addressbook.xml", 
                                     new VirtualContactReader(), 
                                     new VirtualContactWriter() ) );
    m_files.append( new VirtualFile( basedir + "/Applications/todolist/todolist.xml", 
                                     new VirtualTodoListReader(), 
                                     new VirtualTodoListWriter() ) );
}

void VirtualFS::fileListing( const QString &path, QTextStream &stream )
{
    QString name;
    QDateTime lastModified = QDateTime::currentDateTime();
    QStringList fileEntries;
    QStringList dirEntries;
    
    for ( QListIterator<VirtualFile> it( m_files ); it.current(); ++it ) {
        QString filePath = it.current()->filePath();
        if( filePath.startsWith( path ) ) {
            filePath = filePath.mid( path.length() );
            QStringList fl = QStringList::split( '/', filePath );
            if( fl.count() > 1 ) {
                if ( !dirEntries.contains( fl[0] ) )
                    dirEntries += fl[0];
            }
            else
                if ( !fileEntries.contains( fl[0] ) )
                    fileEntries += fl[0];
        }
    }    

    for ( QStringList::Iterator it = dirEntries.begin(); it != dirEntries.end(); ++it ) {
        stream << fileListingEntry( (*it), true, lastModified ) << oendl;
    }
    for ( QStringList::Iterator it = fileEntries.begin(); it != fileEntries.end(); ++it ) {
        stream << fileListingEntry( (*it), false, lastModified ) << oendl;
    }
}

QString VirtualFS::fileListingEntry( const QString &name, bool dir, const QDateTime &lastModified )
{
    QString s;

    // type char
    if ( dir )
        s += "drwxr-xr-x";
    else
        s += "-rw-r--r--";

    s += ' ';
    
    // number of hardlinks
    int subdirs = 1;

    if ( dir )
        subdirs = 2;
    
    s += QString::number( subdirs ).rightJustify( 3, ' ', TRUE ) + " ";

    // owner
    QString o = "nobody";
    s += o.leftJustify( 8, ' ', TRUE ) + " ";

    // group
    QString g = "nogroup";
    s += g.leftJustify( 8, ' ', TRUE ) + " ";

    // file size in bytes
    s += QString::number( 1024 ).rightJustify( 9, ' ', TRUE ) + " ";

    // last modified date
    QDate date = lastModified.date();
    QTime time = lastModified.time();
    s += date.monthName( date.month() ) + " "
        + QString::number( date.day() ).rightJustify( 2, ' ', TRUE ) + " "
        + QString::number( time.hour() ).rightJustify( 2, '0', TRUE ) + ":"
        + QString::number( time.minute() ).rightJustify( 2,'0', TRUE ) + " ";

    // file name
    s += name;

    return s;
}

bool VirtualFS::canRead( const QString &file )
{
    for ( QListIterator<VirtualFile> it( m_files ); it.current(); ++it ) {
        if( it.current()->filePath() == file && it.current()->reader() )
            return true;
    }
    return false;
}

bool VirtualFS::canWrite( const QString &file )
{
    for ( QListIterator<VirtualFile> it( m_files ); it.current(); ++it ) {
        if( it.current()->filePath() == file && it.current()->writer() )
            return true;
    }
    return false;
}

bool VirtualFS::isVirtual( const QString &file )
{
    for ( QListIterator<VirtualFile> it( m_files ); it.current(); ++it ) {
        if( it.current()->filePath() == file )
            return true;
    }
    return false;
}

bool VirtualFS::deleteFile( const QString &file )
{
    // FIXME implement
    return false;
}

VirtualReader *VirtualFS::readFile( const QString &file )
{
    for ( QListIterator<VirtualFile> it( m_files ); it.current(); ++it ) {
        if( it.current()->filePath() == file ) {
            VirtualReader *reader = it.current()->reader();
            return reader;
        }            
    }
    return NULL;
}

VirtualWriter *VirtualFS::writeFile( const QString &file ) 
{
    for ( QListIterator<VirtualFile> it( m_files ); it.current(); ++it ) {
        if( it.current()->filePath() == file ) {
            VirtualWriter *writer = it.current()->writer();
            return writer;
        }            
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////

void VirtualDateBookReader::read( QTextStream &stream )
{
    ODateBookAccess *sourceDB = new ODateBookAccess();
    sourceDB->load();
    ODateBookAccessBackend_XML dest( QString::null );
    
    // Copy items from source to destination
    QArray<int> uidList = sourceDB->records();
    odebug << "Try to move data for datebook.. (" << uidList.count() << " items) " << oendl;
    for ( uint i = 0; i < uidList.count(); ++i ) {
        odebug << "Adding uid: " << uidList[i] << "" << oendl;
        OPimRecord* rec = sourceDB->record( uidList[i] );
        dest.add( *OPimEvent::safeCast( rec ) );
        delete rec;
    }

    delete sourceDB;
    
    // Write out the stream
    OTextStreamWriter wr( &stream );
    dest.write( wr );
}

///////////////////////////////////////////////////////////////////////////////////////////

void VirtualDateBookWriter::write( OPimXmlReader &reader )
{
    ODateBookAccess *destDB = new ODateBookAccess();
    destDB->clear();
    ODateBookAccessBackend_XML source( QString::null );
    
    // FIXME return code
    source.read( reader );
    
    // Copy items from source to destination
    QArray<int> uidList = source.allRecords();
    odebug << "Try to move data for datebook.. (" << uidList.count() << " items) " << oendl;
    for ( uint i = 0; i < uidList.count(); ++i ) {
        odebug << "Adding uid: " << uidList[i] << "" << oendl;
        OPimEvent ev = source.find( uidList[i] );
        destDB->add( ev );
    }

    // Write the changes
    destDB->save();
    delete destDB;
}

///////////////////////////////////////////////////////////////////////////////////////////

void VirtualContactReader::read( QTextStream &stream )
{
    OPimContactAccess *sourceDB = new OPimContactAccess( "sync" );
    sourceDB->load();
    OPimContactAccessBackend_XML dest( QString::null );
    
    // Copy items from source to destination
    QArray<int> uidList = sourceDB->records();
    odebug << "Try to move data for contacts.. (" << uidList.count() << " items) " << oendl;
    for ( uint i = 0; i < uidList.count(); ++i ) {
        odebug << "Adding uid: " << uidList[i] << "" << oendl;
        OPimRecord* rec = sourceDB->record( uidList[i] );
        dest.add( *OPimContact::safeCast( rec ) );
        delete rec;
    }
    
    delete sourceDB;

    // Write out the stream
    OTextStreamWriter wr( &stream );
    dest.write( wr );
}

///////////////////////////////////////////////////////////////////////////////////////////

void VirtualContactWriter::write( OPimXmlReader &reader )
{
    // FIXME need a proper way of disabling the journal here
    OPimContactAccess *destDB = new OPimContactAccess( "sync" );
    destDB->clear();
    OPimContactAccessBackend_XML source( QString::null );
    
    // FIXME return code
    source.read( reader );
    
    // Copy items from source to destination
    QArray<int> uidList = source.allRecords();
    odebug << "Try to move data for contacts.. (" << uidList.count() << " items) " << oendl;
    for ( uint i = 0; i < uidList.count(); ++i ) {
        odebug << "Adding uid: " << uidList[i] << "" << oendl;
        OPimContact contact = source.find( uidList[i] );
        destDB->add( contact );
    }

    // Write the changes
    destDB->save();
    delete destDB;
}

///////////////////////////////////////////////////////////////////////////////////////////

void VirtualTodoListReader::read( QTextStream &stream )
{
    OPimTodoAccess *sourceDB = new OPimTodoAccess();
    sourceDB->load();
    OPimTodoAccessXML dest( QString::null );
    
    // Copy items from source to destination
    QArray<int> uidList = sourceDB->records();
    odebug << "Try to move data for todolist.. (" << uidList.count() << " items) " << oendl;
    for ( uint i = 0; i < uidList.count(); ++i ) {
        odebug << "Adding uid: " << uidList[i] << "" << oendl;
        OPimRecord* rec = sourceDB->record( uidList[i] );
        dest.add( *OPimTodo::safeCast( rec ) );
        delete rec;
    }
    
    delete sourceDB;

    // Write out the stream
    OTextStreamWriter wr( &stream );
    dest.write( wr );
}

///////////////////////////////////////////////////////////////////////////////////////////

void VirtualTodoListWriter::write( OPimXmlReader &reader )
{
    OPimTodoAccess *destDB = new OPimTodoAccess();
    destDB->load(); // need to call this or internal opened flag won't be set
    destDB->clear();
    OPimTodoAccessXML source( QString::null );
    
    // FIXME return code
    source.read( reader );
    
    // Copy items from source to destination
    QArray<int> uidList = source.allRecords();
    odebug << "Try to move data for contacts.. (" << uidList.count() << " items) " << oendl;
    for ( uint i = 0; i < uidList.count(); ++i ) {
        odebug << "Adding uid: " << uidList[i] << "" << oendl;
        OPimTodo todo = source.find( uidList[i] );
        destDB->add( todo );
    }

    // Write the changes
    destDB->save();
    delete destDB;
}
