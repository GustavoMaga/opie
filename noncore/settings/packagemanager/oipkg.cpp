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

#include "oipkg.h"

#include <opie2/odebug.h>

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

#include <stdlib.h>
#include <unistd.h>

extern "C" {
#include <libipkg.h>
};
args_t m_ipkgArgs; // libipkg configuration arguments

const QString IPKG_CONF        = "/etc/ipkg.conf";      // Fully-qualified name of Ipkg primary configuration file
const QString IPKG_CONF_DIR    = "/etc/ipkg";           // Directory of secondary Ipkg configuration files
const QString IPKG_PKG_PATH    = "/usr/lib/ipkg/lists"; // Directory containing server package lists
const QString IPKG_STATUS_PATH = "usr/lib/ipkg/status"; // Destination status file location
const QString IPKG_INFO_PATH   = "usr/lib/ipkg/info";   // Package file lists location

OIpkg *oipkg;

// Ipkg callback functions

int fsignalIpkgMessage( ipkg_conf_t *conf, message_level_t level, char *msg )
{
    // Display message only if it is below the message level threshold
    if ( conf && ( conf->verbosity < level ) )
        return 0;
    else
        oipkg->ipkgMessage( msg );

    return 0;
}

char *fIpkgResponse( char */*question*/ )
{
    return 0l;
}

int fIpkgStatus( char */*name*/, int /*status*/, char *desc, void */*userdata*/ )
{
    oipkg->ipkgStatus( desc );
    return 0;
}

int fIpkgFiles( char */*name*/, char *desc, char */*version*/, pkg_state_status_t /*status*/,
                void */*userdata*/ )
{
    oipkg->ipkgList( desc );
    return 0;
}

OIpkg::OIpkg( Config *config, QObject *parent, const char *name )
    : QObject( parent, name )
    , m_config( config )
    , m_confInfo( NULL )
    , m_ipkgExecOptions( 0 )
    , m_ipkgExecVerbosity( 1 )
{
    // Keep pointer to self for the Ipkg callback functions
    oipkg = this;

    // Initialize libipkg
    ipkg_init( &fsignalIpkgMessage, &fIpkgResponse, &m_ipkgArgs );

    // Default ipkg run-time arguments
    m_ipkgArgs.noaction = false;
    m_ipkgArgs.force_defaults = true;
}

OIpkg::~OIpkg()
{
    // Upon destruction, ensure that items in config list are deleted with list
    if ( m_confInfo )
        m_confInfo->setAutoDelete( true );

    // Free up libipkg resources
    ipkg_deinit( &m_ipkgArgs );
}

OConfItemList *OIpkg::configItems()
{
    // Retrieve all configuration items
    return filterConfItems();
}

OConfItemList *OIpkg::servers()
{
    // Retrieve only servers
    return filterConfItems( OConfItem::Source );
}

OConfItemList *OIpkg::destinations()
{
    // Retrieve only destinations
    return filterConfItems( OConfItem::Destination );
}

OConfItemList *OIpkg::options()
{
    // Retrieve only destinations
    return filterConfItems( OConfItem::Option );
}

void OIpkg::defaultConfItemFile( OConfItem *item )
{
    if( item->file().isNull() )
    {
        switch ( item->type() )
        {
            case OConfItem::Source :
            case OConfItem::Destination :
                item->setFile( IPKG_CONF_DIR + "/" + item->name() + ".conf" );
                break;
            case OConfItem::Arch :
                item->setFile( IPKG_CONF_DIR + "/arch.conf" );
                break;
            default :
                item->setFile( IPKG_CONF );
                break;
        };
    }
}

void OIpkg::setConfigItems( OConfItemList *configList )
{
    if ( m_confInfo )
        delete m_confInfo;

    m_confInfo = configList;

    // Write out new config files
    QString lastFile = "";
    QFile *confFile = NULL;
    QTextStream *confStream = NULL;
    OConfItemListIterator it( *m_confInfo );
    for ( ; it.current(); ++it )
    {
        OConfItem *item = it.current();

        // Only write out valid conf items
        if ( item->type() != OConfItem::NotDefined )
        {
            if ( lastFile != item->file() ) {
                if ( confFile ) {
                    confFile->close();
                    delete confStream;
                    delete confFile;
                }
                odebug << "Opening " << item->file() << oendl;
                confFile = new QFile( item->file() );
                if ( ! confFile->open( IO_WriteOnly ) ) {
                    owarn << "Failed to open " << item->file() << oendl;
                    delete confFile;
                    confFile = NULL;
                    break;
                }
                lastFile = item->file();

                confStream = new QTextStream( confFile );
//                (*confStream) << "# Generated by Opie Package Manager\n\n";
            }

            QString confLine;
            QString name = item->name();
            if ( !item->active() )
                confLine = "#";

            switch ( item->type() )
            {
                case OConfItem::Source :
                {
                    if ( item->features().contains( "Compressed" ) )
                        confLine.append( "src/gz" );
                    else
                        confLine.append( "src" );
                }
                break;
                case OConfItem::Destination : confLine.append( "dest" ); break;
                case OConfItem::Option : confLine.append( "option" ); break;
                case OConfItem::Arch : confLine.append( "arch" ); break;
                case OConfItem::Other :
                {
                    // For options w/type = Other, the mapping is as follows:
                    //    name = typeStr (e.g. "lists_dir")
                    //    value = value
                    //    features = name (from configuration file)
                    confLine.append( item->name() );
                    name = item->features();
                }
                break;
                default : break;
            };

            (*confStream) << confLine << " " << name << " " << item->value() << "\n";
        }

    }

    if ( confFile ) {
        confFile->close();
        delete confStream;
        delete confFile;
    }

    // Reinitialize libipkg to pick up new configuration
    ipkg_deinit( &m_ipkgArgs );
    ipkg_init( &fsignalIpkgMessage, &fIpkgResponse, &m_ipkgArgs );
    m_ipkgArgs.noaction = false;
    m_ipkgArgs.force_defaults = true;
}

void OIpkg::saveSettings()
{
    // Save Ipkg execution options to application configuration file
    if ( m_config )
    {
        m_config->setGroup( "Ipkg" );
        m_config->writeEntry( "ExecOptions", m_ipkgExecOptions );
        m_config->writeEntry( "Verbosity", m_ipkgExecVerbosity );
    }
}

OPackageList *OIpkg::availablePackages( const QString &server )
{
    // Load Ipkg configuration info if not already cached
    if ( !m_confInfo )
        loadConfiguration();

    // Build new server list (caller is responsible for deleting)
    OPackageList *pl = new OPackageList;

    // Get directory where server lists are located
    QString listsDir;
    OConfItem *confItem = findConfItem( OConfItem::Other, "lists_dir" );
    if ( confItem )
        listsDir = confItem->value();
    else
        listsDir = IPKG_PKG_PATH;

    // Open package list file
    QFile f( listsDir + "/" + server );
    if ( !f.open( IO_ReadOnly ) )
        return NULL;
    QTextStream t( &f );

    // Process all information in package list file
    OPackage *package = NULL;
    QString line = t.readLine();
    while ( !t.eof() )
    {
        // Determine key/value pair
        int pos = line.find( ':', 0 );
        QString key;
        if ( pos > -1 )
            key = line.mid( 0, pos );
        else
            key = QString::null;
        QString value = line.mid( pos+2, line.length()-pos );

        // Allocate new package and insert into list
        if ( package == NULL  && !key.isEmpty() )
        {
            package = new OPackage( value );
            package->setSource( server );
            pl->append( package );
        }

        // Update package data
        if ( key == "Package" )
                package->setName( value );
        else if ( key == "Version" )
            package->setVersion( value );
        else if ( key == "Section" )
            package->setCategory( value );
            //DataManager::setAvailableCategories( value );
        else if ( key.isEmpty() && value.isEmpty() )
            package = NULL;

        // Skip past all description lines
        if ( key == "Description" )
        {
            line = t.readLine();
            while ( !line.isEmpty() && line.find( ':', 0 ) == -1 && !t.eof() )
                line = t.readLine();
        }
        else
            line = t.readLine();
    }

    f.close();

    return pl;
}

OPackageList *OIpkg::installedPackages( const QString &destName, const QString &destPath )
{
    // Load Ipkg configuration info if not already cached
    if ( !m_confInfo )
        loadConfiguration();

    // Build new server list (caller is responsible for deleting)
    OPackageList *pl = new OPackageList;

    // Open status file
    QString path = destPath;
    if ( path.right( 1 ) != "/" )
        path.append( "/" );
    path.append( IPKG_STATUS_PATH );

    QFile f( path );
    if ( !f.open( IO_ReadOnly ) )
        return NULL;
    QTextStream t( &f );

    // Process all information in status file
    bool newPackage = false;
    QString line = t.readLine();
    QString name;
    QString version;
    QString status;

    while ( !t.eof() )
    {
        // Determine key/value pair
        int pos = line.find( ':', 0 );
        QString key;
        if ( pos > -1 )
            key = line.mid( 0, pos );
        else
            key = QString::null;
        QString value = line.mid( pos+2, line.length()-pos );

        // Allocate new package and insert into list
        if ( newPackage  && !key.isEmpty() )
        {
            // Add to list only if it has a valid name and is installed
            if ( !name.isNull() && status.contains( " installed" ) )
            {
                pl->append( new OPackage( name, QString::null, version, QString::null, destName ) );
                name = QString::null;
                version = QString::null;
                status = QString::null;

                newPackage = false;
            }
        }

        // Update package data
        if ( key == "Package" )
            name = value;
        else if ( key == "Version" )
            version = value;
        else if ( key == "Status" )
            status = value;
        else if ( key.isEmpty() && value.isEmpty() )
            newPackage = true;

        // Skip past all description lines
        if ( key == "Description" )
        {
            line = t.readLine();
            while ( !line.isEmpty() && line.find( ':', 0 ) == -1 && !t.eof() )
                line = t.readLine();
        }
        else
            line = t.readLine();
    }

    f.close();

    // Make sure to add to list last entry
    if ( !name.isNull() && status.contains( " installed" ) )
        pl->append( new OPackage( name, QString::null, version, QString::null, destName ) );

    return pl;
}

OConfItem *OIpkg::findConfItem( OConfItem::Type type, const QString &name )
{
    // Find configuration item in list
    OConfItemListIterator configIt( *m_confInfo );
    OConfItem *config = 0l;
    for ( ; configIt.current(); ++configIt )
    {
        config = configIt.current();
        if ( config->type() == type && config->name() == name )
            break;
    }

    if ( config && config->type() == type && config->name() == name )
        return config;

    return 0l;
}

bool OIpkg::executeCommand( OPackage::Command command, const QStringList &parameters, const QString &destination,
                            const QObject *receiver, const char *slotOutput, bool rawOutput )
{
    if ( command == OPackage::NotDefined )
        return false;

    // Set ipkg run-time options/arguments
    m_ipkgArgs.force_depends = ( m_ipkgExecOptions & FORCE_DEPENDS );
    m_ipkgArgs.force_reinstall = ( m_ipkgExecOptions & FORCE_REINSTALL );
    // TODO m_ipkgArgs.force_remove = ( m_ipkgExecOptions & FORCE_REMOVE );
    m_ipkgArgs.force_overwrite = ( m_ipkgExecOptions & FORCE_OVERWRITE );
    m_ipkgArgs.force_removal_of_dependent_packages = ( m_ipkgExecOptions & FORCE_RECURSIVE );
    m_ipkgArgs.verbose_wget = ( m_ipkgExecOptions & FORCE_VERBOSE_WGET );
    m_ipkgArgs.verbosity = m_ipkgExecVerbosity;
    if ( m_ipkgArgs.dest )
        free( m_ipkgArgs.dest );
    if ( !destination.isNull() )
    {
        int len = destination.length() + 1;
        m_ipkgArgs.dest = (char *)malloc( len );
        strncpy( m_ipkgArgs.dest, destination, destination.length() );
        m_ipkgArgs.dest[ len - 1 ] = '\0';
    }
    else
        m_ipkgArgs.dest = 0l;

    // Connect output signal to widget

    if ( !rawOutput )
    {
        // TODO - connect to local slot and parse output before emitting signalIpkgMessage
    }

    switch( command )
    {
        case OPackage::Update : {
                connect( this, SIGNAL(signalIpkgMessage(const QString &)), receiver, slotOutput );
                ipkg_lists_update( &m_ipkgArgs );
                disconnect( this, SIGNAL(signalIpkgMessage(const QString &)), 0, 0 );
            };
            break;
        case OPackage::Upgrade : {
                connect( this, SIGNAL(signalIpkgMessage(const QString &)), receiver, slotOutput );
                ipkg_packages_upgrade( &m_ipkgArgs );

                // Re-link non-root destinations to make sure everything is in sync
                OConfItemList *destList = destinations();
                OConfItemListIterator it( *destList );
                for ( ; it.current(); ++it )
                {
                    OConfItem *dest = it.current();
                    if ( dest->name() != "root" )
                        linkPackageDir( dest->name() );
                }
                delete destList;
                disconnect( this, SIGNAL(signalIpkgMessage(const QString &)), 0, 0 );
            };
            break;
        case OPackage::Install : {
                connect( this, SIGNAL(signalIpkgMessage(const QString &)), receiver, slotOutput );
                for ( QStringList::ConstIterator it = parameters.begin(); it != parameters.end(); ++it )
                {
                    ipkg_packages_install( &m_ipkgArgs, (*it) );
                }
                if ( destination != "root" )
                    linkPackageDir( destination );
                disconnect( this, SIGNAL(signalIpkgMessage(const QString &)), 0, 0 );
            };
            break;
        case OPackage::Remove : {
                connect( this, SIGNAL(signalIpkgMessage(const QString &)), receiver, slotOutput );

                // Get list of destinations for unlinking of packages not installed to root
                OConfItemList *destList = destinations();

                for ( QStringList::ConstIterator it = parameters.begin(); it != parameters.end(); ++it )
                {
                    unlinkPackage( (*it), destList );
                    ipkg_packages_remove( &m_ipkgArgs, (*it), true );
                }

                delete destList;
                disconnect( this, SIGNAL(signalIpkgMessage(const QString &)), 0, 0 );
            };
            break;
        case OPackage::Download : {
                connect( this, SIGNAL(signalIpkgMessage(const QString &)), receiver, slotOutput );
                for ( QStringList::ConstIterator it = parameters.begin(); it != parameters.end(); ++it )
                {
                    ipkg_packages_download( &m_ipkgArgs, (*it) );
                }
                disconnect( this, SIGNAL(signalIpkgMessage(const QString &)), 0, 0 );
            };
            break;
        case OPackage::Info : {
                connect( this, SIGNAL(signalIpkgStatus(const QString &)), receiver, slotOutput );
                ipkg_packages_info( &m_ipkgArgs, (*parameters.begin()), &fIpkgStatus, 0l );
                disconnect( this, SIGNAL(signalIpkgStatus(const QString &)), 0, 0 );
            };
            break;
        case OPackage::Files : {
                connect( this, SIGNAL(signalIpkgList(const QString &)), receiver, slotOutput );
                ipkg_package_files( &m_ipkgArgs, (*parameters.begin()), &fIpkgFiles, 0l );
                disconnect( this, SIGNAL(signalIpkgList(const QString &)), 0, 0 );
            };
            break;
        default : break;
    };

    return true;
}

void OIpkg::ipkgMessage( char *msg )
{
    emit signalIpkgMessage( msg );
}

void OIpkg::ipkgStatus( char *status )
{
    emit signalIpkgStatus( status );
}

void OIpkg::ipkgList( char *filelist )
{
    emit signalIpkgList( filelist );
}

void OIpkg::loadConfiguration()
{
    if ( m_confInfo )
        delete m_confInfo;

    // Load configuration item list
    m_confInfo = new OConfItemList();

    QStringList confFiles;
    QDir confDir( IPKG_CONF_DIR );
    if ( confDir.exists() )
    {
        confDir.setNameFilter( "*.conf" );
        confDir.setFilter( QDir::Files );
        confFiles = confDir.entryList( "*.conf", QDir::Files );
    }
    confFiles << IPKG_CONF;

    QStringList::Iterator lastFile = confFiles.end();
    for ( QStringList::Iterator it = confFiles.begin(); it != lastFile; ++it )
    {
        // Create absolute file path if necessary
        QString absFile = (*it);
        if ( !absFile.startsWith( "/" ) )
            absFile.prepend( QString( IPKG_CONF_DIR ) + "/" );

        // Read in file
        QFile f( absFile );
        if ( f.open( IO_ReadOnly ) )
        {
            QTextStream s( &f );
            while ( !s.eof() )
            {

                QString line = s.readLine().simplifyWhiteSpace();

                // Parse line and save info to the conf options list
                if ( !line.isEmpty() )
                {
                    // Strip leading comment marker if exists
                    bool comment = false;
                    if ( line.startsWith( "#" ) )
                    {
                        line.remove( 0, 1 );
                        line = line.simplifyWhiteSpace();
                        comment = true;
                    }

                    bool recognizedOption = true;
                    int pos = line.find( ' ', 1 )  + 1;
                    int endpos = line.find( ' ', pos );

                    // Name
                    QString name = line.mid( pos, endpos - pos );

                    // Value
                    QString value = "";
                    if ( endpos > -1 )
                        value = line.right( line.length() - endpos - 1 );

                    // Active
                    bool active = !comment;

                    // Type
                    // For options w/type = Other, the mapping is as follows:
                    //    name = typeStr (e.g. "lists_dir")
                    //    value = value
                    //    features = name (from configuration file)

                    QString typeStr = line.left( pos - 1 );
                    OConfItem::Type type;
                    QString features;
                    if ( typeStr == "src" )
                        type = OConfItem::Source;
                    else if ( typeStr == "src/gz" )
                    {
                        type = OConfItem::Source;
                        features = "Compressed";
                    }
                    else if ( typeStr == "dest" )
                        type = OConfItem::Destination;
                    else if ( typeStr == "option" )
                        type = OConfItem::Option;
                    else if ( typeStr == "arch" )
                        type = OConfItem::Arch;
                    else if ( typeStr == "lists_dir" )
                    {
                        type = OConfItem::Other;
                        features = name;
                        name = typeStr;

                        // Default value when not defined
                        if ( value == QString::null || value == "" )
                            value = IPKG_PKG_PATH;
                    }
                    else
                        recognizedOption = false;

                    // Add to list
                    if ( recognizedOption )
                        m_confInfo->append( new OConfItem( type, name, value, features, absFile, active ) );
                }
            }

            f.close();
        }
    }

    // Load Ipkg execution options from application configuration file
    if ( m_config )
    {
        m_config->setGroup( "Ipkg" );
        m_ipkgExecOptions = m_config->readNumEntry( "ExecOptions", m_ipkgExecOptions );
        m_ipkgExecVerbosity = m_config->readNumEntry( "Verbosity", m_ipkgExecVerbosity );
    }
}

OConfItemList *OIpkg::filterConfItems( OConfItem::Type typefilter )
{
    // Load Ipkg configuration info if not already cached
    if ( !m_confInfo )
        loadConfiguration();

    // Build new server list (caller is responsible for deleting)
    OConfItemList *sl = new OConfItemList;

    // If typefilter is empty, retrieve all items
    bool retrieveAll = ( typefilter == OConfItem::NotDefined );

    // Parse configuration info for servers
    OConfItemListIterator it( *m_confInfo );
    for ( ; it.current(); ++it )
    {
        OConfItem *item = it.current();
        if ( retrieveAll || item->type() == typefilter )
        {
            sl->append( item );
        }
    }

    return sl;
}

const QString &OIpkg::rootPath()
{
    if ( m_rootPath.isEmpty() )
    {
        OConfItem *rootDest = findConfItem( OConfItem::Destination, "root" );
        rootDest ? m_rootPath = rootDest->value()
                 : m_rootPath = '/';
        if ( m_rootPath.right( 1 ) == '/' )
            m_rootPath.truncate( m_rootPath.length() - 1 );
    }
    return m_rootPath;
}

void OIpkg::linkPackageDir( const QString &dest )
{
    if ( !dest.isNull() )
    {
        OConfItem *destConfItem = findConfItem( OConfItem::Destination, dest );

        emit signalIpkgMessage( tr( "Linking packages installed in: %1" ).arg( dest ) );

        // Set package destination directory
        QString destDir = destConfItem->value();
        QString destInfoDir = destDir;
        if ( destInfoDir.right( 1 ) != '/' )
            destInfoDir.append( '/' );
        destInfoDir.append( IPKG_INFO_PATH );

        // Get list of installed packages in destination
        QDir packageDir( destInfoDir );
        QStringList packageFiles;
        if ( packageDir.exists() )
        {
            packageDir.setNameFilter( "*.list" );
            packageDir.setFilter( QDir::Files );
            packageFiles = packageDir.entryList( "*.list", QDir::Files );
        }

        // Link all files for every package installed in desination
        QStringList::Iterator lastFile = packageFiles.end();
        for ( QStringList::Iterator it = packageFiles.begin(); it != lastFile; ++it )
        {
            //emit signalIpkgMessage( QString( "Processing: %1/%2" ).arg( destInfoDir ).arg (*it) );
            QString packageFileName = destInfoDir;
            packageFileName.append( '/' );
            packageFileName.append( (*it) );
            QFile packageFile( packageFileName );
            if ( packageFile.open( IO_ReadOnly ) )
            {
                QTextStream t( &packageFile );
                QString linkFile;
                while ( !t.eof() )
                {
                    // Get the name of the file to link and build the sym link filename
                    linkFile = t.readLine();
                    QString linkDest( linkFile.right( linkFile.length() - destDir.length() ) );
                    linkDest.prepend( rootPath() );

                    // If file installed file is actually symbolic link, use actual file for linking
                    QFileInfo fileInfo( linkFile );
                    if ( fileInfo.isSymLink() && !fileInfo.readLink().isEmpty() )
                        linkFile = fileInfo.readLink();

                    // See if directory exists in 'root', if not, create
                    fileInfo.setFile( linkDest );
                    QString linkDestDirName = fileInfo.dirPath( true );
                    QDir linkDestDir( linkDestDirName );
                    if ( !linkDestDir.exists() )
                    {
                        linkDestDir.mkdir( linkDestDirName );
                    }
                    else
                    {
                        // Remove any previous link to make sure we will be pointing to the current version
                        if ( QFile::exists( linkDest ) )
                            QFile::remove( linkDest );
                    }

                    // Link the file
                    //emit signalIpkgMessage( QString( "Linking '%1' to '%2'" ).arg( linkFile ).arg( linkDest ) );
                    if ( symlink( linkFile, linkDest ) == -1 )
                        emit signalIpkgMessage( tr( "Error linkling '%1' to '%2'" )
                                                .arg( linkFile )
                                                .arg( linkDest ) );
                }
                packageFile.close();
            }
        }
    }
}

void OIpkg::unlinkPackage( const QString &package, OConfItemList *destList )
{
    if ( !package.isNull() )
    {
        // Find destination package is installed in
        if ( destList )
        {
            OConfItemListIterator it( *destList );
            for ( ; it.current(); ++it )
            {
                OConfItem *dest = it.current();
                QString destInfoFileName = QString( "%1/%2/%3.list" ).arg( dest->value() )
                                                                     .arg( IPKG_INFO_PATH )
                                                                     .arg( package );
                //emit signalIpkgMessage( QString( "Looking for '%1'" ).arg ( destInfoFileName ) );

                // If found and destination is not 'root', remove symbolic links
                if ( QFile::exists( destInfoFileName ) && dest->name() != "root" )
                {
                    QFile destInfoFile( destInfoFileName );
                    if ( destInfoFile.open( IO_ReadOnly ) )
                    {
                        QTextStream t( &destInfoFile );
                        QString linkFile;
                        while ( !t.eof() )
                        {
                            // Get the name of the file to link and build the sym link filename
                            linkFile = t.readLine();
                            QString linkDest( linkFile.right( linkFile.length() -
                                                              dest->value().length() ) );
                            linkDest.prepend( rootPath() );

                            //emit signalIpkgMessage( QString( "Deleting: '%1'" ).arg( linkDest ) );
                            QFile::remove( linkDest );
                        }
                        destInfoFile.close();
                    }

                    emit signalIpkgMessage( tr( "Links removed for: %1" ).arg( package ) );
                    return;
                }
            }
        }
    }
}
