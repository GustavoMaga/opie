/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#include "documentlist.h"
#include "serverinterface.h"
#include "mediadlg.h"

/* OPIE */
#include <opie2/oglobal.h>
#include <opie2/odebug.h>
#include <qtopia/config.h>
#include <qtopia/mimetype.h>
#include <qtopia/resource.h>
#include <qtopia/private/categories.h>
#include <qtopia/qpeapplication.h>
#include <qtopia/applnk.h>
#include <qtopia/storage.h>
#ifdef Q_WS_QWS
#include <qtopia/qcopenvelope_qws.h>
#endif
using namespace Opie::Core;

/* QT */
#include <qtimer.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qdir.h>
#include <qpainter.h>
#include <qimage.h>
#include <qcopchannel_qws.h>
#include <qlistview.h>
#include <qlist.h>
#include <qpixmap.h>


AppLnkSet *DocumentList::appLnkSet = 0;

static const int MAX_SEARCH_DEPTH = 10;


class DocumentListPrivate : public QObject {
    Q_OBJECT
public:
    DocumentListPrivate( ServerInterface *gui );
    ~DocumentListPrivate();

    void initialize();

    const QString nextFile();
    const DocLnk *iterate();
    bool store( DocLnk* dl );
    void estimatedPercentScanned();
    void appendDocpath(FileSystem*);


    DocLnkSet dls;
    QDict<void> reference;
    QDictIterator<void> *dit;
    enum { Find, RemoveKnownFiles, MakeUnknownFiles, Done } state;

    QStringList docPaths;
    unsigned int docPathsSearched;

    int searchDepth;
    QDir *listDirs[MAX_SEARCH_DEPTH];
    const QFileInfoList *lists[MAX_SEARCH_DEPTH];
    unsigned int listPositions[MAX_SEARCH_DEPTH];

    StorageInfo *storage;

    int tid;

    ServerInterface *serverGui;

    bool needToSendAllDocLinks;
    bool sendAppLnks;
    bool sendDocLnks;
    bool scanDocs;
};


/*
 * scandocs will be read from Config
 */
DocumentList::DocumentList( ServerInterface *serverGui, bool /*scanDocs*/,
			    QObject *parent, const char *name )
 : QObject( parent, name )
{
    appLnkSet = new AppLnkSet( MimeType::appsFolderName() );
    d = new DocumentListPrivate( serverGui );
    d->needToSendAllDocLinks = false;

    Config cfg( "Launcher" );
    cfg.setGroup( "DocTab" );
    d->scanDocs = cfg.readBoolEntry( "Enable", true );
    odebug << "DocumentList::DocumentList() : scanDocs = " << d->scanDocs << "" << oendl;

    QTimer::singleShot( 10, this, SLOT( startInitialScan() ) );
}

void DocumentList::startInitialScan()
{
    reloadAppLnks();
    reloadDocLnks();
}

DocumentList::~DocumentList()
{
    delete appLnkSet;
    delete d;
}


void DocumentList::add( const DocLnk& doc )
{
    if ( d->serverGui && QFile::exists( doc.file() ) )
	d->serverGui->documentAdded( doc );
}


void DocumentList::start()
{
    resume();
}


void DocumentList::pause()
{
    //odebug << "pause " << d->tid << "" << oendl;
    killTimer( d->tid );
    d->tid = 0;
}


void DocumentList::resume()
{
    if ( d->tid == 0 ) {
	d->tid = startTimer( 20 );
    //odebug << "resumed " << d->tid << "" << oendl;
    }
}

/*
void DocumentList::resend()
{
    // Re-emits all the added items to the list (firstly letting everyone know to
    // clear what they have as it is being sent again)
    pause();
    emit allRemoved();
    QTimer::singleShot( 5, this, SLOT( resendWorker() ) );
}


void DocumentList::resendWorker()
{
    const QList<DocLnk> &list = d->dls.children();
    for ( QListIterator<DocLnk> it( list ); it.current(); ++it )
	add( *(*it) );
    resume();
}
*/

void DocumentList::rescan()
{
    //odebug << "rescan" << oendl;
    pause();
    d->initialize();
    resume();
}


void DocumentList::timerEvent( QTimerEvent *te )
{
    if ( te->timerId() == d->tid ) {
	// Do 3 at a time
        if ( d->serverGui )
            d->serverGui->aboutToAddBegin();
	for (int i = 0; i < 3; i++ ) {
	    const DocLnk *lnk = d->iterate();
	    if ( lnk ) {
		add( *lnk );
	    } else {
		// stop when done
		pause();
		if ( d->serverGui )
		    d->serverGui->documentScanningProgress( 100 );
		if ( d->needToSendAllDocLinks )
		    sendAllDocLinks();
		break;
	    }
        }
        if ( d->serverGui )
            d->serverGui->aboutToAddEnd();
    }
}


void DocumentList::reloadAppLnks()
{
    if ( d->sendAppLnks && d->serverGui ) {
	d->serverGui->applicationScanningProgress( 0 );
	d->serverGui->allApplicationsRemoved();
    }

    delete appLnkSet;
    appLnkSet = new AppLnkSet( MimeType::appsFolderName() );

    if ( d->sendAppLnks && d->serverGui ) {
	static QStringList prevTypeList;
	QStringList types = appLnkSet->types();
	for ( QStringList::Iterator ittypes=types.begin(); ittypes!=types.end(); ++ittypes) {
	    if ( !(*ittypes).isEmpty() ) {
		if ( !prevTypeList.contains(*ittypes) ) {
		    QString name = appLnkSet->typeName(*ittypes);
		    QPixmap pm = appLnkSet->typePixmap(*ittypes);
		    QPixmap bgPm = appLnkSet->typeBigPixmap(*ittypes);

		    if (pm.isNull()) {
			QImage img( Resource::loadImage( "UnknownDocument" ) );
			pm = img.smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() );
			bgPm = img.smoothScale( AppLnk::bigIconSize(), AppLnk::bigIconSize() );
		    }

            //odebug << "adding type " << (*ittypes) << "" << oendl;

		    // ### our current launcher expects docs tab to be last
		    d->serverGui->typeAdded( *ittypes, name.isNull() ? (*ittypes) : name, pm, bgPm );
		}
		prevTypeList.remove(*ittypes);
	    }
	}
	for ( QStringList::Iterator ittypes=prevTypeList.begin(); ittypes!=prevTypeList.end(); ++ittypes) {
        //odebug << "removing type " << (*ittypes) << "" << oendl;
	    d->serverGui->typeRemoved(*ittypes);
	}
	prevTypeList = types;
    }

    QListIterator<AppLnk> itapp( appLnkSet->children() );
    AppLnk* l;
    while ( (l=itapp.current()) ) {
	++itapp;
	if ( d->sendAppLnks && d->serverGui )
	    d->serverGui->applicationAdded( l->type(), *l );
    }

    if ( d->sendAppLnks && d->serverGui )
	d->serverGui->applicationScanningProgress( 100 );
}

void DocumentList::reloadDocLnks()
{
    if ( !d->scanDocs )
	return;

    if ( d->sendDocLnks && d->serverGui ) {
	d->serverGui->documentScanningProgress( 0 );
	d->serverGui->allDocumentsRemoved();
    }

    rescan();
}

void DocumentList::linkChanged( QString arg )
{
    //odebug << "linkchanged( " << arg << " )" << oendl;

    if ( arg.isNull() || OGlobal::isAppLnkFileName( arg ) ) {
	reloadAppLnks();
    } else {

	const QList<DocLnk> &list = d->dls.children();
	QListIterator<DocLnk> it( list );
	while ( it.current() ) {
	    DocLnk *doc = it.current();
	    ++it;
	    if ( ( doc->linkFileKnown() && doc->linkFile() == arg )
		|| ( doc->fileKnown() && doc->file() == arg ) ) {
        //odebug << "found old link" << oendl;
		DocLnk* dl = new DocLnk( arg );
		// add new one if it exists and matches the mimetype
		if ( d->store( dl ) ) {
		    // Existing link has been changed, send old link ref and a ref
		    // to the new link
            //odebug << "change case" << oendl;
		    if ( d->serverGui )
			d->serverGui->documentChanged( *doc, *dl );

		} else {
		    // Link has been removed or doesn't match the mimetypes any more
		    // so we aren't interested in it, so take it away from the list
            //odebug << "removal case" << oendl;
		    if ( d->serverGui )
			d->serverGui->documentRemoved( *doc );

		}
		d->dls.remove( doc ); // remove old link from docLnkSet
		delete doc;
		return;
	    }
	}
	// Didn't find existing link, must be new
	DocLnk* dl = new DocLnk( arg );
	if ( d->store( dl ) ) {
	    // Add if it's a link we are interested in
        //odebug << "add case" << oendl;
	    add( *dl );
	}

    }
}

void DocumentList::restoreDone()
{
    reloadAppLnks();
    reloadDocLnks();
}

void DocumentList::DiffAppLnks()
{
    static AppLnkSet *appLnkSet2;

    appLnkSet2 = new AppLnkSet( MimeType::appsFolderName() );

    if ( d->sendAppLnks && d->serverGui ) {
	static QStringList prevTypeList = appLnkSet->types();
	QStringList types = appLnkSet2->types();
	for ( QStringList::Iterator ittypes=types.begin(); ittypes!=types.end(); ++ittypes) {
	    if ( !(*ittypes).isEmpty() ) {
		if ( !prevTypeList.contains(*ittypes) ) {
		    QString name = appLnkSet2->typeName(*ittypes);
		    QPixmap pm = appLnkSet2->typePixmap(*ittypes);
		    QPixmap bgPm = appLnkSet2->typeBigPixmap(*ittypes);

		    if (pm.isNull()) {
			QImage img( Resource::loadImage( "UnknownDocument" ) );
			pm = img.smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() );
			bgPm = img.smoothScale( AppLnk::bigIconSize(), AppLnk::bigIconSize() );
		    }

            odebug << "adding type " << (*ittypes) << "" << oendl;

		    // ### our current launcher expects docs tab to be last
		    d->serverGui->typeAdded( *ittypes, name.isNull() ? (*ittypes) : name, pm, bgPm );
		}
		prevTypeList.remove(*ittypes);
	    }
	}
	for ( QStringList::Iterator ittypes=prevTypeList.begin(); ittypes!=prevTypeList.end(); ++ittypes) {
        odebug << "removing type " << (*ittypes) << "" << oendl;
	    d->serverGui->typeRemoved(*ittypes);
	}
	prevTypeList = types;
    }


    QListIterator<AppLnk> it1( appLnkSet->children() );
    QListIterator<AppLnk> it2( appLnkSet2->children() );

    AppLnk *i;
    AppLnk *j;
    bool found;

    while ( (j=it2.current()) ) {
	it1 = appLnkSet->children();
	found = false;
	while ( (i=it1.current()) ){
	    if (strcmp(i->name().ascii(),j->name().ascii()) == 0)
		found = true;
	    ++it1;
	}
	if (!found) {
	    odebug << "Item " << j->name().ascii() << " needs to be added" << oendl;
	    d->serverGui->applicationAdded( j->type(), *j );
	}
	    ++it2;
    }

    it1 = appLnkSet->children();
     while ( (i=it1.current()) ) {
	it2 = appLnkSet2->children();
	found = false;
	while ( (j=it2.current()) ){
	    if (strcmp(i->name().ascii(),j->name().ascii()) == 0)
		found = true;
	    ++it2;
	}
	if (!found) {
	    odebug << "Item " << i->name().ascii() << " needs to be removed" << oendl;
	    d->serverGui->applicationRemoved( i->type(), *i );
	}

	    ++it1;
    }

    delete appLnkSet;
    appLnkSet = appLnkSet2;

}
void DocumentList::storageChanged()
{
    QTime t;
    // ### can implement better

    t.start();
    DiffAppLnks();
//    reloadAppLnks();
    odebug << "Reload App links took " << t.elapsed() << " ms" << oendl;
    reloadDocLnks();
//    odebug << "Reload links took " << t.elapsed() << " ms " << oendl;
    odebug << "Reload All links took " << t.elapsed() << " ms" << oendl;
// ### Optimization opportunity
    // Could be a bit more intelligent and somehow work out which
    // mtab entry has changed and then only scan that and add and remove
    // links appropriately.
//    rescan();
}

void DocumentList::sendAllDocLinks()
{
    if ( d->tid != 0 ) {
	// We are in the middle of scanning, set a flag so
	// we do this when we finish our scanning
	d->needToSendAllDocLinks = true;
	return;
    }

    QString contents;
    Categories cats;
    for ( QListIterator<DocLnk> it( d->dls.children() ); it.current(); ++it ) {
	DocLnk *doc = it.current();
	QFileInfo fi( doc->file() );
	if ( !fi.exists() )
	    continue;

	bool fake = !doc->linkFileKnown();
	if ( !fake ) {
	    QFile f( doc->linkFile() );
	    if ( f.open( IO_ReadOnly ) ) {
		QTextStream ts( &f );
		ts.setEncoding( QTextStream::UnicodeUTF8 );
		QString docLnk = ts.read();
		// Strip out the (stale) LinkFile entry
		int start = docLnk.find( "\nLinkFile = " ) + 1;
		if ( start > 0 ) {
		    int end = docLnk.find( "\n", start + 1 ) + 1;
		    contents += docLnk.left(start);
		    contents += docLnk.mid(end);
		} else {
		    contents += docLnk;
		}
		contents += "LinkFile = " + doc->linkFile() + "\n";

		f.close();
	    } else
		fake = TRUE;
	}
	if (fake) {
	    contents += "[Desktop Entry]\n"; // No tr
	    contents += "Categories = " + // No tr
		cats.labels("Document View",doc->categories()).join(";") + "\n"; // No tr
	    contents += "Name = "+doc->name()+"\n"; // No tr
	    contents += "Type = "+doc->type()+"\n"; // No tr
	}
	contents += "File = "+doc->file()+"\n"; // No tr // (resolves path)
	contents += QString("Size = %1\n").arg( fi.size() ); // No tr
    }

    //odebug << "sending length " << contents.length() << "" << oendl;
#ifndef QT_NO_COP
    QCopEnvelope e( "QPE/Desktop", "docLinks(QString)" );
    e << contents;
#endif
    //odebug << "================ \n\n" << contents << "\n\n===============" << oendl;

    d->needToSendAllDocLinks = false;
}
























DocumentListPrivate::DocumentListPrivate( ServerInterface *gui )
{
    storage = new StorageInfo( this );
    serverGui = gui;
    if ( serverGui ) {
	sendAppLnks = serverGui->requiresApplications();
	sendDocLnks = serverGui->requiresDocuments();
    } else {
	sendAppLnks = false;
	sendDocLnks = false;
    }
    for ( int i = 0; i < MAX_SEARCH_DEPTH; i++ ) {
	listDirs[i] = 0;
	lists[i] = 0;
	listPositions[i] = 0;
    }
    initialize();
    tid = 0;
}

void DocumentListPrivate::appendDocpath(FileSystem*fs)
{
    QDir defPath(fs->path()+"/Documents");
    QFileInfo f(fs->path()+"/.opiestorage.cf");
    if (!f.exists()) {
        Mediadlg dlg(fs);
        if (QDialog::Accepted != QPEApplication::execDialog( &dlg )) {
            return;
        }
    }
    Config conf(f.filePath(), Config::File );
    conf.setGroup("main");
    if (!conf.readBoolEntry("check",false)) {
        return;
    }
    conf.setGroup("subdirs");
    bool read_all = conf.readBoolEntry("wholemedia",true);
    if (read_all) {
        docPaths+=fs->path();
        return;
    }
    QStringList subDirs = conf.readListEntry("subdirs",':');
    if (subDirs.isEmpty()) {
        if (defPath.exists()) {
            docPaths+=defPath.path();
        }
        return;
    }
    for (unsigned c = 0; c < subDirs.count();++c) {
        QDir docDir(QString(fs->path()+"/"+subDirs[c]));
        if (docDir.exists()) {
            docPaths+=docDir.path();
        }
    }
}

void DocumentListPrivate::initialize()
{
    // Reset
    dls.clear();
    docPaths.clear();
    reference.clear();

    QDir docDir( QPEApplication::documentDir() );
    if ( docDir.exists() )
	docPaths += QPEApplication::documentDir();
    int i = 1;
    const QList<FileSystem> &fs = storage->fileSystems();
    QListIterator<FileSystem> it( fs );
    for ( ; it.current(); ++it ) {
        if ( (*it)->isRemovable() ) {
            appendDocpath((*it));
            ++i;
        }
    }

    for ( int i = 0; i < MAX_SEARCH_DEPTH; ++i ) {
	if ( listDirs[i] ) {
	    delete listDirs[i];
	    listDirs[i] = 0;
	}
	lists[i] = 0;
	listPositions[i] = 0;
    }

    docPathsSearched = 0;
    searchDepth = -1;
    state = Find;
    dit = 0;
}


DocumentListPrivate::~DocumentListPrivate()
{
    for ( int i = 0; i < MAX_SEARCH_DEPTH; i++ )
	if ( listDirs[i] )
	    delete listDirs[i];
    delete dit;
}


void DocumentListPrivate::estimatedPercentScanned()
{
    double overallProgress = 0.0;
    double levelWeight = 75.0;

    int topCount = docPaths.count();
    if ( topCount > 1 ) {
	levelWeight = levelWeight / topCount;
	overallProgress += (docPathsSearched - 1) * levelWeight;
    }

    for ( int d = 0; d <= searchDepth; d++ ) {
	if ( listDirs[d] ) {
	    int items = lists[d]->count();
	    if ( items > 1 ) {
		levelWeight = levelWeight / items;
		// Take in to account "." and ".."
		overallProgress += (listPositions[d] - 3) * levelWeight;
	    }
	} else {
	    break;
	}
    }

    // odebug << "overallProgress: " << overallProgress << "" << oendl;

    if ( serverGui )
	serverGui->documentScanningProgress( (int)overallProgress );
}


const QString DocumentListPrivate::nextFile()
{
    while ( TRUE ) {
	while ( searchDepth < 0 ) {
	    // go to next base path
	    if ( docPathsSearched >= docPaths.count() ) {
		// end of base paths
		return QString::null;
	    } else {
		QDir dir( docPaths[docPathsSearched] );
        // odebug << "now using base path: " << docPaths[docPathsSearched] << "" << oendl;
		docPathsSearched++;
		if ( !dir.exists( ".Qtopia-ignore" ) ) {
		    listDirs[0] = new QDir( dir );
		    lists[0] = listDirs[0]->entryInfoList();
		    listPositions[0] = 0;
		    searchDepth = 0;
		}
	    }
	}

	const QFileInfoList *fil = lists[searchDepth];
    if (!fil) {
        return QString::null;
    }
	QFileInfoList *fl = (QFileInfoList *)fil;
	unsigned int pos = listPositions[searchDepth];

	if ( pos >= fl->count() ) {
	    // go up a depth
	    delete listDirs[searchDepth];
	    listDirs[searchDepth] = 0;
	    lists[searchDepth] = 0;
	    listPositions[searchDepth] = 0;
	    searchDepth--;
	} else {
	    const QFileInfo *fi = fl->at(pos);
	    listPositions[searchDepth]++;
	    QString bn = fi->fileName();
	    if ( bn[0] != '.' ) {
		if ( fi->isDir()  ) {
		    if ( bn != "CVS" && bn != "Qtopia" && bn != "QtPalmtop" ) {
			// go down a depth
			QDir dir( fi->filePath() );
            // odebug << "now going in to path: " << bn << "" << oendl;
			if ( !dir.exists( ".Qtopia-ignore" ) ) {
			    if ( searchDepth < MAX_SEARCH_DEPTH - 1) {
				searchDepth++;
				listDirs[searchDepth] = new QDir( dir );
				lists[searchDepth] = listDirs[searchDepth]->entryInfoList();
				listPositions[searchDepth] = 0;
			    }
			}
		    }
		} else {
		    estimatedPercentScanned();
		    return fl->at(pos)->filePath();
		}
	    }
	}
    }

    return QString::null;
}


bool DocumentListPrivate::store( DocLnk* dl )
{
    // if ( dl->fileKnown() && !dl->file().isEmpty() ) {
    if ( dl && dl->fileKnown() ) {
	dls.add( dl ); // store
	return TRUE;
    }

    // don't store - delete
    delete dl;
    return FALSE;
}


#define MAGIC_NUMBER	((void*)2)

const DocLnk *DocumentListPrivate::iterate()
{
    if ( state == Find ) {
    //odebug << "state Find" << oendl;
	QString file = nextFile();
	while ( !file.isNull() ) {
	    if ( file.right(8) == ".desktop" ) { // No tr
		DocLnk* dl = new DocLnk( file );
		if ( store(dl) )
		    return dl;
	    } else {
		reference.insert( file, MAGIC_NUMBER );
	    }
	    file = nextFile();
	}
	state = RemoveKnownFiles;

	if ( serverGui )
	    serverGui->documentScanningProgress( 75 );
    }

    static int iterationI;
    static int iterationCount;

    if ( state == RemoveKnownFiles ) {
    //odebug << "state RemoveKnownFiles" << oendl;
	const QList<DocLnk> &list = dls.children();
	for ( QListIterator<DocLnk> it( list ); it.current(); ++it ) {
	    reference.remove( (*it)->file() );
	    // ### does this need to be deleted?
	}
	dit = new QDictIterator<void>(reference);
	state = MakeUnknownFiles;

	iterationI = 0;
	iterationCount = dit->count();
    }

    if ( state == MakeUnknownFiles ) {
    //odebug << "state MakeUnknownFiles" << oendl;
	for (void* c; (c=dit->current()); ++(*dit) ) {
	    if ( c == MAGIC_NUMBER ) {
		DocLnk* dl = new DocLnk;
		QFileInfo fi( dit->currentKey() );
		dl->setFile( fi.filePath() );
		dl->setName( fi.baseName() );
		if ( store(dl) ) {
		    ++*dit;
		    iterationI++;
		    if ( serverGui )
			serverGui->documentScanningProgress( 75 + (25*iterationI)/iterationCount );
		    return dl;
		}
	    }
	    iterationI++;
	}

	delete dit;
	dit = 0;
	state = Done;
    }

    //odebug << "state Done" << oendl;
    return NULL;
}


#include "documentlist.moc"



