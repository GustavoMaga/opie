#include "backuprestore.h"
#include "errordialog.h"


/* OPIE */
#include <opie2/odebug.h>
#include <opie2/ostorageinfo.h>
using namespace Opie::Core;

#include <opie2/ofiledialog.h>
using namespace Opie::Ui;

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/config.h>

/* QT */
#include <qapplication.h>
#include <qmultilineedit.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qlist.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qtextview.h>
#include <qlineedit.h>
#include <qstringlist.h>

/* STD */
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#define HEADER_NAME 0
#define HEADER_BACKUP 1
#define BACKUP_LOCATION 2

#define EXTENSION ".bck"

const QString tempFileName = "/tmp/backup.err";

BackupAndRestore::BackupAndRestore( QWidget* parent, const char* name,  WFlags fl)
        : BackupAndRestoreBase(parent, name,  fl)
{
    backupList->header()->hide();
    restoreList->header()->hide();
    locationList->header()->hide();
    connect( backupButton, SIGNAL( clicked() ), this, SLOT( backup() ) );
    connect( restoreButton, SIGNAL( clicked() ), this, SLOT( restore() ) );
    connect( backupList, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( selectItem( QListViewItem* ) ) );
    connect( restoreSource, SIGNAL( activated( int ) ), this, SLOT( sourceDirChanged( int ) ) );
    connect( addLocationButton, SIGNAL( clicked() ), this, SLOT( addLocation() ) );
    connect( removeLocationButton, SIGNAL( clicked() ), this, SLOT( removeLocation() ) );
    connect( saveLocationsButton, SIGNAL( clicked() ), this, SLOT( saveLocations() ) );
    connect( selectLocationButton, SIGNAL( clicked() ), this, SLOT( selectLocation() ) );

    //add directorys for backing up
    applicationSettings = new QListViewItem(backupList, "Application Settings", "", "Settings/");
    selectItem(applicationSettings);
    applicationSettings = new QListViewItem(backupList, "Application Data", "", "Applications/");
    selectItem(applicationSettings);
    documents= new QListViewItem(backupList, "Documents", "", "Documents/");
    selectItem(documents);

    scanForApplicationSettings();
    refreshLocations();
    refreshBackupLocations();

    // Read the list of items to ignore.
    QList<QString> dontBackupList;
    dontBackupList.setAutoDelete(true);
    Config config("BackupAndRestore");
    config.setGroup("DontBackup");
    int total = config.readNumEntry("Total", 0);
    for(int i = 0; i < total; i++)
    {
        dontBackupList.append(new QString(config.readEntry(QString("%1").arg(i), "")));
    }

    QList<QListViewItem> list;
    getAllItems(backupList->firstChild(), list);

    for(uint i = 0; i < list.count(); i++)
    {
        QString text = list.at(i)->text(HEADER_NAME);
        for(uint i2 = 0;  i2 < dontBackupList.count(); i2++)
        {
            if(*dontBackupList.at(i2) == text)
            {
                selectItem(list.at(i));
                break;
            }
        }
    }
    QPEApplication::showWidget( this );
}

BackupAndRestore::~BackupAndRestore()
{
    QList<QListViewItem> list;
    getAllItems(backupList->firstChild(), list);

    Config config("BackupAndRestore");
    config.setGroup("DontBackup");
    config.clearGroup();

    int count = 0;
    for(uint i = 0; i < list.count(); i++)
    {
        if(list.at(i)->text(HEADER_BACKUP) == "")
        {
            config.writeEntry(QString("%1").arg(count), list.at(i)->text(HEADER_NAME));
            count++;
        }
    }
    config.writeEntry("Total", count);

    // Remove Temp File
    if ( QFile::exists( tempFileName ) )
        QFile::remove( tempFileName );
}

void BackupAndRestore::refreshBackupLocations()
{
    backupLocations.clear();
    // Add cards
    Opie::Core::OStorageInfo storage;
    backupLocations.insert( "Documents", QDir::homeDirPath() + "/Documents" );
    if ( storage.hasCf() )
    {
        backupLocations.insert( "CF", storage.cfPath() );
        odebug << "Cf Path: " + storage.cfPath() << oendl;
    }
    if ( storage.hasSd() )
    {
        backupLocations.insert( "SD", storage.sdPath() );
        odebug << " Sd Path: " + storage.sdPath() << oendl;
    }
    if ( storage.hasMmc() )
    {
        backupLocations.insert( "MMC", storage.mmcPath() );
        odebug << "Mmc Path: " + storage.mmcPath() << oendl;
    }

    for ( QListViewItemIterator it( locationList ); it.current(); ++it )
    {
        backupLocations.insert( it.current()->text( 0 ), it.current()->text( 0 ) );
    }

    //update QComboBox
    storeToLocation->clear();
    restoreSource->clear();

    //read last locations
    Config config("BackupAndRestore");
    config.setGroup("LastLocation");
    QString lastStoreLocation   = config.readEntry( "LastStoreLocation", "" );
    QString lastRestoreLocation = config.readEntry( "LastRestoreLocation", "" );
    int locationIndex    = 0;

    //fill QComboBox
    QMap<QString, QString>::Iterator it;
    for( it = backupLocations.begin(); it != backupLocations.end(); ++it )
    {
        storeToLocation->insertItem(it.key());
        restoreSource->insertItem(it.key());

        //check for last locations
        if ( it.key() == lastStoreLocation )
            storeToLocation->setCurrentItem( locationIndex );
        if ( it.key() == lastRestoreLocation )
            restoreSource->setCurrentItem( locationIndex );
        locationIndex++;
    }
}

QList<QListViewItem> BackupAndRestore::getAllItems(QListViewItem *item, QList<QListViewItem> &list)
{
    while(item)
    {
        if(item->childCount() > 0)
            getAllItems(item->firstChild(), list);
        list.append(item);
        item = item->nextSibling();
    }
    return list;
}

/**
 * Selects and unselects the item by setting the HEADER_BACKUP to B or !.
 * and changing the icon to match
 * @param currentItem the item to swich the selection choice.
 */
void BackupAndRestore::selectItem(QListViewItem *currentItem)
{
    if(!currentItem)
        return;

    if(currentItem->text(HEADER_BACKUP) == "B")
    {
        currentItem->setPixmap(HEADER_NAME, Resource::loadPixmap("backup/null"));
        currentItem->setText(HEADER_BACKUP, "");
    }
    else
    {
        currentItem->setPixmap(HEADER_NAME, Resource::loadPixmap("backup/check"));
        currentItem->setText(HEADER_BACKUP, "B");
    }
}

void BackupAndRestore::scanForApplicationSettings()
{
    QDir d( QDir::homeDirPath() + "/" + QString( applicationSettings->text(BACKUP_LOCATION) ) );
    d.setFilter( QDir::Dirs | QDir::Files | QDir::NoSymLinks );
    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *fi;
    while ( (fi=it.current()) )
    {
        //odebug << (d.path()+"/"+fi->fileName()).latin1() << oendl;
        if ( ( fi->fileName() != "." ) && ( fi->fileName() != ".." ) )
        {
            QListViewItem *newItem = new QListViewItem(applicationSettings, fi->fileName());
            selectItem(newItem);
        }
        ++it;
    }
}

/**
 * The "Backup" button has been pressed.  Get a list of all of the files that
 * should be backed up.  If there are no files, emit and error and exit.
 * Determine the file name to store the backup in.  Backup the file(s) using
 * tar and gzip  --best.  Report failure or success
 */
void BackupAndRestore::backup()
{
    QString backupFiles;
    if(getBackupFiles(backupFiles, NULL) == 0)
    {
        QMessageBox::critical(this, "Message",
                              "No items selected.",QString("Ok") );
        return;
    }

    setCaption(tr("Backup and Restore... working..."));
    QString outputFile = backupLocations[storeToLocation->currentText()];

    QDateTime datetime = QDateTime::currentDateTime();
    QString dateString = QString::number( datetime.date().year() ) + QString::number( datetime.date().month() ).rightJustify(2, '0') +
                         QString::number( datetime.date().day() ).rightJustify(2, '0');

    outputFile += "/" + dateString;

    QString t = outputFile;
    int c = 1;
    while(QFile::exists(outputFile + EXTENSION))
    {
        outputFile = t + QString("%1").arg(c);
        c++;
    }

    // We execute tar and compressing its output with gzip..
    // The error output will be written into a temp-file which could be provided
    // for debugging..
    odebug << "Storing file: " << outputFile.latin1() << "" << oendl;
    outputFile += EXTENSION;

    QString commandLine = QString( "cd %1 && (tar -X %1 -cz %2 Applications/backup/exclude -f %3 ) 2> %4" ).arg( QDir::homeDirPath() )
                          .arg( getExcludeFile() )
                          .arg( backupFiles )
                          .arg( outputFile.latin1() )
                          .arg( tempFileName.latin1() );

    odebug << commandLine << oendl;

    int r = system( commandLine );

    if(r != 0)
    {
        perror("Error: ");
        QString errorMsg= tr( "Error from System:\n" ) + (QString)strerror( errno );

        switch( QMessageBox::critical(this, tr( "Message" ), tr( "Backup Failed!" ) + "\n"
                                      + errorMsg, QString( tr( "Ok" ) ), QString( tr( "Details" ) ) ) )
        {

        case 1:
            owarn << "Details pressed !" << oendl;
            ErrorDialog* pErrDialog = new ErrorDialog( this, NULL, true );
            QFile errorFile( tempFileName );
            if ( errorFile.open(IO_ReadOnly) )
            {
                QTextStream t( &errorFile );
                QString s;
                while ( !t.eof() )
                {        // until end of file...
                    s += t.readLine();       // line of text excluding '\n'
                }
                errorFile.close();

                pErrDialog->m_textarea->setText( s );
            }
            else
            {
                pErrDialog->m_textarea->setText( "Unable to open File: /tmp/backup.er" );
            }
            QPEApplication::execDialog( pErrDialog );
            delete pErrDialog;
            break;
        }
        setCaption(tr("Backup and Restore.. Failed !!"));
        return;
    }
    else
    {
        QMessageBox::information(this, tr( "Message" ), tr( "Backup Successful." ), QString(tr( "Ok" ) ) );
    }

    //write store-location
    Config config( "BackupAndRestore" );
    config.setGroup( "LastLocation" );
    config.writeEntry( "LastStoreLocation", storeToLocation->currentText() );

    setCaption(tr("Backup and Restore"));
}

/***
 * Get a list of all of the files to backup.
 */
int BackupAndRestore::getBackupFiles(QString &backupFiles, QListViewItem *parent)
{
    QListViewItem * currentItem;
    QString currentHome;
    if(!parent)
        currentItem = backupList->firstChild();
    else
    {
        currentItem = parent->firstChild();
        currentHome = parent->text(BACKUP_LOCATION);
    }

    uint count = 0;
    while( currentItem != 0 )
    {
        if(currentItem->text(HEADER_BACKUP) == "B" )
        {
            if(currentItem->childCount() == 0 )
            {
                if(parent == NULL)
                    backupFiles += currentItem->text(BACKUP_LOCATION);
                else
                    backupFiles += currentHome + currentItem->text(HEADER_NAME);
                backupFiles += " ";
                count++;
            }
            else
            {
                count += getBackupFiles(backupFiles, currentItem);
            }
        }
        currentItem = currentItem->nextSibling();
    }
    return count;
}

void BackupAndRestore::sourceDirChanged(int selection)
{
    restoreList->clear();
    rescanFolder(backupLocations[restoreSource->text(selection)]);
}

void BackupAndRestore::fileListUpdate()
{
    owarn << "void BackupAndRestore::fileListUpdate()" << oendl;
    restoreList->clear();
    rescanFolder( backupLocations[restoreSource->currentText()] );
}

/**
 * Scans directory for any backup files.  Will recursivly go down,
 * but will not follow symlinks.
 * @param directory - the directory to look in.
 */
void BackupAndRestore::rescanFolder(QString directory)
{
    //odebug << QString("rescanFolder: ") + directory.latin1() << oendl;
    QDir d(directory);
    if(!d.exists())
        return;

    d.setFilter( QDir::Files | QDir::Hidden | QDir::Dirs);
    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *file;
    while ( (file=it.current()) )
    {  // for each file...
        // If it is a dir and not .. or . then add it as a tab and go down.
        if(file->isDir())
        {
            if(file->fileName() != ".." && file->fileName() != ".")
            {
                rescanFolder(directory + "/" + file->fileName());
            }
        }
        else
        {
            // If it is a backup file add to list.
            if(file->fileName().contains(EXTENSION))
                (void)new QListViewItem(restoreList, file->fileName());
        }
        ++it;
    }
}

/**
 * Restore a backup file.
 * Report errors or success
 */
void BackupAndRestore::restore()
{
    QListViewItem *restoreItem = restoreList->currentItem();
    if(!restoreItem)
    {
        QMessageBox::critical(this, tr( "Message" ),
                              tr( "Please select something to restore." ),QString( tr( "Ok") ) );
        return;
    }
    setCaption(tr("Backup and Restore... working..."));

    QString restoreFile = backupLocations[restoreSource->currentText()];

    restoreFile += "/" + restoreItem->text(0);

    odebug << restoreFile << oendl;

    //check if backup file come from opie 1.0.x

    QString commandLine = QString( "tar -tzf %1 | grep Applications/backup/exclude" ).arg( restoreFile.latin1() );

    int r = system( commandLine );

    QString startDir;

    if( r != 0 ) //Applications/backup/exclude not found - old backup file
    {
        startDir = QString( "/" );
    } else
    {
        startDir = QDir::homeDirPath();
    }

    //unpack backup file
    commandLine = QString( "cd %1 && tar -zxf %2 2> %3" ).arg( startDir )
                  .arg( restoreFile.latin1() )
                  .arg( tempFileName.latin1() );

    odebug << commandLine << oendl;

    r = system( commandLine );

    //error handling
    if(r != 0)
    {
        QString errorMsg= tr( "Error from System:\n" ) + (QString)strerror( errno );
        switch( QMessageBox::critical(this, tr( "Message" ), tr( "Restore Failed." ) + "\n"
                                      + errorMsg, QString( tr( "Ok") ), QString( tr( "Details" ) ) ) )
        {
        case 1:
            owarn << "Details pressed !" << oendl;
            ErrorDialog* pErrDialog = new ErrorDialog( this, NULL, true );
            QFile errorFile( tempFileName );
            if ( errorFile.open(IO_ReadOnly) )
            {
                QTextStream t( &errorFile );
                QString s;
                while ( !t.eof() )
                {        // until end of file...
                    s += t.readLine();       // line of text excluding '\n'
                }
                errorFile.close();

                pErrDialog->m_textarea->setText( s );
            }
            else
            {
                pErrDialog->m_textarea->setText( tr( "Unable to open File: %1" ).arg( "/tmp/backup.er" ) );
            }
            QPEApplication::execDialog( pErrDialog );
            delete pErrDialog;

            setCaption(tr("Backup and Restore.. Failed !!"));
            return;

            break;

        }
    }
    else
    {
        QMessageBox::information(this, tr( "Message" ), tr( "Restore Successful." ), QString( tr( "Ok") ) );
    }

    //write restore-location
    Config config( "BackupAndRestore" );
    config.setGroup( "LastLocation" );
    config.writeEntry( "LastRestoreLocation", restoreSource->currentText() );

    setCaption(tr("Backup and Restore"));
}

/**
 * Check for exclude in Applications/backup
 * If it does not exist, the function will create the file with *.bck as content
 * The exclude_files is read by tar and will provide to exclude special files out from backup.
 * e.g. alle *.bck files (backup-files) will not be backed up by default
 */

QString BackupAndRestore::getExcludeFile()
{
    QString excludeFileName = Global::applicationFileName( "backup", "exclude" );
    if ( !QFile::exists( excludeFileName ) )
    {
        QFile excludeFile( excludeFileName);
        if ( excludeFile.open( IO_WriteOnly ) == true )
        {
            QTextStream writeStream( &excludeFile );
            writeStream << "*.bck" << "\n";
            excludeFile.close();
        }
        else
        {
            return QString::null;
        }
    }

    return excludeFileName;
}

void BackupAndRestore::refreshLocations()
{
    locationList->clear();

    //todo: implement add locations
    Config config( "BackupAndRestore" );
    config.setGroup( "Locations" );

    QStringList locations( config.readListEntry( "locations", '|' ) );

    for ( QStringList::Iterator it = locations.begin(); it != locations.end(); ++it ) {
         (void) new QListViewItem( locationList, *it );
    }
}

void BackupAndRestore::addLocation()
{
    if ( ( !locationEdit->text().isEmpty() ) &&
         ( QDir( locationEdit->text() ).exists() ) )
    {
        (void) new QListViewItem( locationList, locationEdit->text() );
        locationEdit->setText( "" );
    }
}

void BackupAndRestore::removeLocation()
{
    if ( locationList->selectedItem() )
    {
        delete( locationList->selectedItem() );
    }
}

void BackupAndRestore::saveLocations()
{
    Config config("BackupAndRestore");
    config.setGroup("Locations");

    QStringList locations;
    for ( QListViewItemIterator it( locationList ); it.current(); ++it )
    {
        locations.append( it.current()->text( 0 ) );
    }
    config.writeEntry( "locations", locations, '|' );

    refreshBackupLocations();
}

void BackupAndRestore::selectLocation()
{
    QString location = OFileDialog::getDirectory( OFileSelector::DIRECTORYSELECTOR );
    if ( !location.isEmpty() )
    {
        locationEdit->setText( location );
    }
}

// backuprestore.cpp

