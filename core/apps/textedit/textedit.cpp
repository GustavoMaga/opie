/**********************************************************************
// textedit.cpp
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
**********************************************************************/
// changes added by L. J. Potter Sun 02-17-2002 21:31:31
#include "textedit.h"
#include "filePermissions.h"


#include <opie/ofileselector.h>
#include <opie/ofiledialog.h>
#include <opie/ofontselector.h>

#include <qpe/fontdatabase.h>
#include <qpe/global.h>
#include <qpe/fileselector.h>
#include <qpe/applnk.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qcopenvelope_qws.h>

#include <qpoint.h>
#include <qtextstream.h>
#include <qdatetime.h>
#include <qclipboard.h>
#include <qstringlist.h>
#include <qaction.h>
#include <qcolordialog.h>
#include <qfileinfo.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qobjectlist.h>
#include <qpopupmenu.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qwidgetstack.h>
#include <qcheckbox.h>
#include <qcombo.h>
#include <qlayout.h>
#include <qapplication.h>
#include <qtimer.h>
#include <qdir.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h> //getenv

#include "resource.h"

#if QT_VERSION < 300

class QpeEditor : public QMultiLineEdit
{

public:
    QpeEditor( QWidget *parent, const char * name = 0 )
            : QMultiLineEdit( parent, name ) {
        clearTableFlags();
        setTableFlags( Tbl_vScrollBar | Tbl_autoHScrollBar );
}

    void find( const QString &txt, bool caseSensitive,
                            bool backwards );
protected:
    bool markIt;
    int line1, line2, col1, col2;
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );

//public slots:
      /*
signals:
    void notFound();
    void searchWrapped();
    */

private:

};

void QpeEditor::mousePressEvent( QMouseEvent *e ) {
 switch(e->button()) {
   case RightButton:
   { //rediculous workaround for qt popup menu
     //and the hold right click mechanism
       this->setSelection( line1, col1, line2, col2);
       QMultiLineEdit::mousePressEvent( e );
       markIt = false;
   }
   break;
   default:
   {
       if(!markIt) {
           int line, col;
           this->getCursorPosition(&line, &col);
           line1=line2=line;
           col1=col2=col;
       }
       QMultiLineEdit::mousePressEvent( e );
   }
       break;
 };
}

void QpeEditor::mouseReleaseEvent( QMouseEvent * ) {
    if(this->hasMarkedText()) {
        markIt = true;
        this->getMarkedRegion( &line1, &col1, &line2, & col2 );
    } else {
       markIt = false;
    }
}

void QpeEditor::find ( const QString &txt, bool caseSensitive,
           bool backwards )
{
    static bool wrap = false;
    int line, col;
    if ( wrap ) {
        if ( !backwards )
            line = col = 0;
        wrap = false;
          //  emit searchWrapped();
    } else {
        getCursorPosition( &line, &col );
    }
      //ignore backwards for now....
    if ( !backwards ) {
        for ( ; ; ) {
            if ( line >= numLines() ) {
                wrap = true;
                  //emit notFound();
                break;
            }
            int findCol = getString( line )->find( txt, col, caseSensitive );
            if ( findCol >= 0 ) {
                setCursorPosition( line, findCol, false );
                col = findCol + txt.length();
                setCursorPosition( line, col, true );

                  //found = true;
                break;
            }
            line++;
            col = 0;
        }
    }
}


#else

#error "Must make a QpeEditor that inherits QTextEdit"

#endif


static const int nfontsizes = 6;
static const int fontsize[nfontsizes] = {8,10,12,14,18,24};

TextEdit::TextEdit( QWidget *parent, const char *name, WFlags f )
    : QMainWindow( parent, name, f ), bFromDocView( false )
{
    doc = 0;
    edited=false;
    fromSetDocument=false;

    setToolBarsMovable( false );
    connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );

    channel = new QCopChannel( "QPE/Application/textedit", this );
    connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
        this, SLOT(receive(const QCString&, const QByteArray&)) );

    setIcon( Resource::loadPixmap( "TextEditor" ) );

    QPEToolBar *bar = new QPEToolBar( this );
    bar->setHorizontalStretchable( true );
    menu = bar;

    QPEMenuBar *mb = new QPEMenuBar( bar );
    QPopupMenu *file = new QPopupMenu( this );
    QPopupMenu *edit = new QPopupMenu( this );
    QPopupMenu *advancedMenu = new QPopupMenu(this);
    
    font = new QPopupMenu( this );

    bar = new QPEToolBar( this );
    editBar = bar;

    QAction *a = new QAction( tr( "New" ), Resource::loadPixmap( "new" ),
                              QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileNew() ) );
//    a->addTo( bar );
    a->addTo( file );

    a = new QAction( tr( "Open" ), Resource::loadPixmap( "fileopen" ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    a->addTo( bar );
    a->addTo( file );

    a = new QAction( tr( "Save" ), QPixmap(( const char** ) filesave_xpm  ) ,
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( save() ) );
    file->insertSeparator();
    a->addTo( bar );
    a->addTo( file );

    a = new QAction( tr( "Save As" ), QPixmap(( const char** ) filesave_xpm  ) ,
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( saveAs() ) );
    a->addTo( file );

    a = new QAction( tr( "Cut" ), Resource::loadPixmap( "cut" ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editCut() ) );
    a->addTo( editBar );
    a->addTo( edit );

    a = new QAction( tr( "Copy" ), Resource::loadPixmap( "copy" ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    a->addTo( editBar );
    a->addTo( edit );

    a = new QAction( tr( "Paste" ), Resource::loadPixmap( "paste" ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    a->addTo( editBar );
    a->addTo( edit );


#ifndef QT_NO_CLIPBOARD
    a = new QAction( tr( "Insert Time and Date" ), Resource::loadPixmap( "paste" ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editPasteTimeDate() ) );
    a->addTo( edit );
#endif
    
    a = new QAction( tr( "Find..." ), Resource::loadPixmap( "find" ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editFind() ) );
    edit->insertSeparator();
    a->addTo( bar );
    a->addTo( edit );

       a = new QAction( tr( "Goto Line..." ), Resource::loadPixmap( "find" ),
                    QString::null, 0, this, 0 );
   connect( a, SIGNAL( activated() ), this, SLOT( gotoLine() ) );
   a->addTo( edit );


    zin = new QAction( tr("Zoom in"), QString::null, 0, this, 0 );
    connect( zin, SIGNAL( activated() ), this, SLOT( zoomIn() ) );
    zin->addTo( font );

    zout = new QAction( tr("Zoom out"), QString::null, 0, this, 0 );
    connect( zout, SIGNAL( activated() ), this, SLOT( zoomOut() ) );
    zout->addTo( font );

    font->insertSeparator();

    font->insertItem(tr("Font"), this, SLOT(changeFont()) );

    font->insertSeparator();
    font->insertItem(tr("Advanced Features"), advancedMenu);
    
    QAction *wa = new QAction( tr("Wrap lines"),
                               QString::null, 0, this, 0 );
    connect( wa, SIGNAL( toggled(bool) ),
             this, SLOT( setWordWrap(bool) ) );
    wa->setToggleAction(true);
    wa->addTo( advancedMenu);

    nStart = new QAction( tr("Start with new file"),
                          QString::null, 0, this, 0 );
    connect( nStart, SIGNAL( toggled(bool) ),
             this, SLOT( changeStartConfig(bool) ) );
    nStart->setToggleAction(true);
    nStart->addTo( advancedMenu );
    nStart->setEnabled(false);

    nAdvanced = new QAction( tr("Prompt on Exit"),
                             QString::null, 0, this, 0 );
    connect( nAdvanced, SIGNAL( toggled(bool) ),
             this, SLOT( doPrompt(bool) ) );
    nAdvanced->setToggleAction(true);
    nAdvanced->addTo( advancedMenu );

    desktopAction = new QAction( tr("Always open linked file"),
                                 QString::null, 0, this, 0 );
    connect( desktopAction, SIGNAL( toggled(bool) ),
             this, SLOT( doDesktop(bool) ) );
    desktopAction->setToggleAction(true);
    desktopAction->addTo( advancedMenu);

    filePermAction = new QAction( tr("File Permissions"),
                                  QString::null, 0, this, 0 );
    connect( filePermAction, SIGNAL( toggled(bool) ),
             this, SLOT( doFilePerms(bool) ) );
    filePermAction->setToggleAction(true);
    filePermAction->addTo( advancedMenu);

    searchBarAction = new QAction( tr("Search Bar Open"),
                                   QString::null, 0, this, 0 );
    connect( searchBarAction, SIGNAL( toggled(bool) ),
             this, SLOT( setSearchBar(bool) ) );
    searchBarAction->setToggleAction(true);
    searchBarAction->addTo( advancedMenu);

    nAutoSave = new QAction( tr("Auto Save 5 min."),
                                   QString::null, 0, this, 0 );
    connect( nAutoSave, SIGNAL( toggled(bool) ),
             this, SLOT( doTimer(bool) ) );
    nAutoSave->setToggleAction(true);
    nAutoSave->addTo( advancedMenu);
     

    font->insertSeparator();

    font->insertItem(tr("About"), this, SLOT( doAbout()) );

    mb->insertItem( tr( "File" ), file );
    mb->insertItem( tr( "Edit" ), edit );
    mb->insertItem( tr( "View" ), font );

    searchBar = new QPEToolBar(this);
    addToolBar( searchBar,  "Search", QMainWindow::Top, true );

    searchBar->setHorizontalStretchable( true );

    searchEdit = new QLineEdit( searchBar, "searchEdit" );
    searchBar->setStretchableWidget( searchEdit );
    connect( searchEdit, SIGNAL( textChanged( const QString & ) ),
       this, SLOT( search() ) );

    a = new QAction( tr( "Find Next" ), Resource::loadPixmap( "next" ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( findNext() ) );
    a->addTo( searchBar );
    a->addTo( edit );

    a = new QAction( tr( "Close Find" ), Resource::loadPixmap( "close" ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( findClose() ) );
    a->addTo( searchBar );

    edit->insertSeparator();
    a = new QAction( tr( "Delete" ), Resource::loadPixmap( "close" ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editDelete() ) );
    a->addTo( edit );

    searchBar->hide();

    editor = new QpeEditor( this );
    setCentralWidget( editor );
    editor->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    connect( editor, SIGNAL( textChanged() ),
             this, SLOT( editorChanged() ) );

    QPEApplication::setStylusOperation( editor, QPEApplication::RightOnHold);

    Config cfg("TextEdit");
    cfg. setGroup ( "Font" );

    QFont defaultFont = editor-> font ( );

    QString family = cfg. readEntry     ( "Family", defaultFont. family ( ));
    int size       = cfg. readNumEntry  ( "Size",   defaultFont. pointSize ( ));
    int weight     = cfg. readNumEntry  ( "Weight", defaultFont. weight ( ));
    bool italic    = cfg. readBoolEntry ( "Italic", defaultFont. italic ( ));

    defaultFont = QFont ( family, size, weight, italic );
    editor-> setFont ( defaultFont );

//    updateCaption();

    cfg.setGroup ( "View" );

    promptExit = cfg.readBoolEntry ( "PromptExit", false );
    openDesktop = cfg.readBoolEntry ( "OpenDesktop", true );
    filePerms = cfg.readBoolEntry ( "FilePermissions", false );
    useSearchBar = cfg.readBoolEntry ( "SearchBar", false );
    startWithNew =  cfg.readBoolEntry ( "startNew", true);
    featureAutoSave = cfg.readBoolEntry( "autosave", false);
    
    if(useSearchBar) searchBarAction->setOn(true);
    if(promptExit)  nAdvanced->setOn( true );
    if(openDesktop) desktopAction->setOn( true );
    if(filePerms) filePermAction->setOn( true );
    if(startWithNew) nStart->setOn( true );
    if(featureAutoSave) nAutoSave->setOn(true);

//       {
//           doTimer(true);
//       }
        
    bool wrap = cfg. readBoolEntry ( "Wrap", true );
    wa-> setOn ( wrap );
    setWordWrap ( wrap );

/////////////////
    if( qApp->argc() > 1) {
        currentFileName=qApp->argv()[1];
        
        QFileInfo fi(currentFileName);

        if(fi.baseName().left(1) == "") {
            openDotFile(currentFileName);
        } else {
            openFile(currentFileName);
        }
    } else {
    edited1=false;
            openDotFile("");
    }

    viewSelection = cfg.readNumEntry( "FileView", 0 );
}

TextEdit::~TextEdit() {
    qWarning("textedit d'tor");
    delete editor;
}

void TextEdit::closeEvent(QCloseEvent *) {
    if( edited1 && promptExit)
      {
          switch( savePrompt() )
            {
              case 1:
              {
                  saveAs();
                  qApp->quit();
              }
              break;

              case 2: 
              {
                  qApp->quit();
              }
              break;

              case -1: 
                  break;
            };
      }
    else
        qApp->quit();
        
}

void TextEdit::cleanUp() {
  
    Config cfg ( "TextEdit" );
    cfg. setGroup ( "Font" );
    QFont f = editor->font();
    cfg.writeEntry ( "Family", f. family ( ));
    cfg.writeEntry ( "Size",   f. pointSize ( ));
    cfg.writeEntry ( "Weight", f. weight ( ));
    cfg.writeEntry ( "Italic", f. italic ( ));
    
    cfg.setGroup ( "View" );
    cfg.writeEntry ( "Wrap", editor->wordWrap() == QMultiLineEdit::WidgetWidth );
    cfg.writeEntry ( "FileView", viewSelection );

    cfg.writeEntry ( "PromptExit", promptExit );
    cfg.writeEntry ( "OpenDesktop", openDesktop );
    cfg.writeEntry ( "FilePermissions", filePerms );
    cfg.writeEntry ( "SearchBar", useSearchBar );
    cfg.writeEntry ( "startNew", startWithNew );

}


void TextEdit::accept() {
        if( edited1)
            saveAs();
     qApp->quit();
}

void TextEdit::zoomIn() {
    setFontSize(editor->font().pointSize()+1,false);
}

void TextEdit::zoomOut() {
    setFontSize(editor->font().pointSize()-1,true);
}


void TextEdit::setFontSize(int sz, bool round_down_not_up) {
    int s=10;
    for (int i=0; i<nfontsizes; i++) {
  if ( fontsize[i] == sz ) {
      s = sz;
      break;
  } else if ( round_down_not_up ) {
      if ( fontsize[i] < sz )
    s = fontsize[i];
  } else {
      if ( fontsize[i] > sz ) {
    s = fontsize[i];
    break;
      }
  }
    }

    QFont f = editor->font();
    f.setPointSize(s);
    editor->setFont(f);

    zin->setEnabled(s != fontsize[nfontsizes-1]);
    zout->setEnabled(s != fontsize[0]);
}

void TextEdit::setBold(bool y) {
    QFont f = editor->font();
    f.setBold(y);
    editor->setFont(f);
}

void TextEdit::setItalic(bool y) {
    QFont f = editor->font();
    f.setItalic(y);
    editor->setFont(f);
}

void TextEdit::setWordWrap(bool y) {
    bool state = editor->edited();
    QString captionStr = caption();
    bool b1 = edited1;
    bool b2 = edited;

    editor->setWordWrap(y ? QMultiLineEdit::WidgetWidth : QMultiLineEdit::NoWrap );
    editor->setEdited( state );
    edited1=b1;
    edited=b2;
    setCaption(captionStr);
}

void TextEdit::setSearchBar(bool b) {
    useSearchBar=b;
    Config cfg("TextEdit");
    cfg.setGroup("View");
    cfg.writeEntry ( "SearchBar", b );
    searchBarAction->setOn(b);
    if(b)
        searchBar->show();
    else
        searchBar->hide();
    editor->setFocus();            
}

void TextEdit::fileNew() {
//     if( !bFromDocView  ) {
//         saveAs();
//     }
    newFile(DocLnk());
}

void TextEdit::fileOpen() {
    Config cfg("TextEdit");
    cfg. setGroup ( "View" );
    QString dir = cfg.readEntry("LastOpenDirectory", QPEApplication::documentDir());
    QMap<QString, QStringList> map;
    map.insert(tr("All"), QStringList() );
    QStringList text;
    text << "text/*";
    map.insert(tr("Text"), text );
    text << "*";
    map.insert(tr("All"), text );
    QString str = OFileDialog::getOpenFileName( 2,
                                                dir ,
                                                QString::null, map);
    if( !str.isEmpty() && QFile(str).exists() && !QFileInfo(str).isDir() )
    {
       cfg.writeEntry("LastOpenDirectory",  QFileInfo(str).dirPath(false));
       openFile( str );
    }
    else
        updateCaption();
}

void TextEdit::doSearchBar() {
    if(!useSearchBar)
        searchBar->hide();
    else
        searchBar->show();
}

#if 0
void TextEdit::slotFind() {
    FindDialog frmFind( tr("Text Editor"), this );
    connect( &frmFind, SIGNAL(signalFindClicked(const QString &, bool, bool, int)),
       editor, SLOT(slotDoFind( const QString&,bool,bool)));

    //case sensitive, backwards, [category]

    connect( editor, SIGNAL(notFound()),
       &frmFind, SLOT(slotNotFound()) );
    connect( editor, SIGNAL(searchWrapped()),
       &frmFind, SLOT(slotWrapAround()) );

    frmFind.exec();


}
#endif

void TextEdit::fileRevert() {
    clear();
    fileOpen();
}

void TextEdit::editCut() {
#ifndef QT_NO_CLIPBOARD
    editor->cut();
#endif
}

void TextEdit::editCopy() {
#ifndef QT_NO_CLIPBOARD
    editor->copy();
#endif
}

void TextEdit::editPaste() {
#ifndef QT_NO_CLIPBOARD
    editor->paste();
#endif
}

void TextEdit::editFind() {
    searchBar->show();
    searchEdit->setFocus();
}

void TextEdit::findNext() {
    editor->find( searchEdit->text(), false, false );

}

void TextEdit::findClose() {
    searchBar->hide();
}

void TextEdit::search() {
    editor->find( searchEdit->text(), false, false );
}

void TextEdit::newFile( const DocLnk &f ) {
    DocLnk nf = f;
    nf.setType("text/plain");
    clear();
    setWState (WState_Reserved1 );
    editor->setFocus();
    doc = new DocLnk(nf);
    currentFileName = "Unnamed";
    qDebug("newFile "+currentFileName);
    updateCaption( currentFileName);
//    editor->setEdited( false);
}

void TextEdit::openDotFile( const QString &f ) {
    if(!currentFileName.isEmpty()) {
    currentFileName=f;
    
    qDebug("openFile dotfile " + currentFileName);
    QString txt;
    QFile file(f);
    file.open(IO_ReadWrite);
    QTextStream t(&file);
    while ( !t.atEnd()) {
        txt+=t.readLine()+"\n";
    }
    editor->setText(txt);
    editor->setEdited( false);
    edited1=false;
    edited=false;


    }
    updateCaption( currentFileName);
}

void TextEdit::openFile( const QString &f ) {
    qDebug("filename is "+ f);
    QString filer;
    QFileInfo fi( f);
//    bFromDocView = true;
    if(f.find(".desktop",0,true) != -1 && !openDesktop ) {
        switch ( QMessageBox::warning(this,tr("Text Editor"),
                                      tr("Text Editor has detected<BR>you selected a <B>.desktop</B>
file.<BR>Open <B>.desktop</B> file or <B>linked</B> file?"),
                                      tr(".desktop File"),tr("Linked Document"),0,1,1) ) {
          case 0:
              filer = f;
              break;
          case 1:
              DocLnk sf(f);
              filer = sf.file();
              break;
        }
    } else if(fi.baseName().left(1) == "") {
        currentFileName=f;
        openDotFile(currentFileName);
    } else {
        DocLnk sf(f);
        filer = sf.file();
        if(filer.right(1) == "/")
            filer = f;
        
    DocLnk nf;
    nf.setType("text/plain");
    nf.setFile(filer);
    currentFileName=filer;

        nf.setName(fi.baseName());
        openFile(nf);

        qDebug("openFile string "+currentFileName);

    }
    showEditTools();
      // Show filename in caption
    QString name = filer;
    int sep = name.findRev( '/' );
    if ( sep > 0 )
        name = name.mid( sep+1 );
    updateCaption( name );
}

void TextEdit::openFile( const DocLnk &f ) {
//    clear();
//    bFromDocView = true;
    FileManager fm;
    QString txt;
    currentFileName=f.file();
    qDebug("openFile doclnk " + currentFileName);
    if ( !fm.loadFile( f, txt ) ) {
          // ####### could be a new file
        qDebug( "Cannot open file" );
    }
//    fileNew();
    if ( doc )
        delete doc;
    doc = new DocLnk(f);
    editor->setText(txt);
    editor->setEdited( false);
    edited1=false;
    edited=false;

    doc->setName(currentFileName);
    updateCaption();
    setTimer();
}

void TextEdit::showEditTools() {
    menu->show();
    editBar->show();
    if(!useSearchBar)
        searchBar->hide();
    else
        searchBar->show();
    setWState (WState_Reserved1 );
}

/*!
  unprompted save */
bool TextEdit::save() {
    qDebug("saveAsFile " + currentFileName);
    if(currentFileName.isEmpty()) {
        saveAs();
        return false;
    }

    QString file = doc->file();
    qDebug("saver file "+file);
    QString name= doc->name();
    qDebug("File named "+name);
    QString rt = editor->text();
    if( !rt.isEmpty() ) {
        if(name.isEmpty()) {
            saveAs();
        } else {
            currentFileName= name ;
            qDebug("saveFile "+currentFileName);

            struct stat buf;
            mode_t mode;
            stat(file.latin1(), &buf);
            mode = buf.st_mode;

            if(!fileIs) {
                doc->setName( name);
                FileManager fm;
                if ( !fm.saveFile( *doc, rt ) ) {
                    return false;
                }
            } else {
                qDebug("regular save file");
                QFile f(file);
                 if( f.open(IO_WriteOnly)) {
                     QCString crt = rt.utf8();
                     f.writeBlock(crt,crt.length());
                 } else {
                     QMessageBox::message(tr("Text Edit"),tr("Write Failed"));
                 return false;
                 }
                     
            }
            editor->setEdited( false);
            edited1=false;
            edited=false;
            if(caption().left(1)=="*")
            setCaption(caption().right(caption().length()-1));


            chmod( file.latin1(), mode);
        }
        return true;
    }
    return false;
}

/*!
  prompted save */
bool TextEdit::saveAs() {

    if(caption() == tr("Text Editor"))
        return false;
    qDebug("saveAsFile " + currentFileName);
      // case of nothing to save... 
//     if ( !doc && !currentFileName.isEmpty()) {
// //|| !bFromDocView)
//         qDebug("no doc");
//         return true;
//     }
//     if ( !editor->edited() ) {
//         delete doc;
//         doc = 0;
//         return true;
//     }

    QString rt = editor->text();
    qDebug(currentFileName);
    
    if( currentFileName.isEmpty()
        || currentFileName == tr("Unnamed")
        || currentFileName == tr("Text Editor")) {
        qDebug("do silly TT filename thing");
//        if ( doc && doc->name().isEmpty() ) {
        QString pt = rt.simplifyWhiteSpace();
        int i = pt.find( ' ' );
        QString docname = pt;
        if ( i > 0 )
            docname = pt.left( i );
          // remove "." at the beginning
        while( docname.startsWith( "." ) )
            docname = docname.mid( 1 );
        docname.replace( QRegExp("/"), "_" );
          // cut the length. filenames longer than that
          //don't make sense and something goes wrong when they get too long.
        if ( docname.length() > 40 )
            docname = docname.left(40);
        if ( docname.isEmpty() )
            docname = tr("Unnamed");
        if(doc) doc->setName(docname);
        currentFileName=docname;
//         }
//         else
//             qDebug("hmmmmmm");
    }


    QMap<QString, QStringList> map;
    map.insert(tr("All"), QStringList() );
    QStringList text;
    text << "text/*";
    map.insert(tr("Text"), text );
    text << "*";
    map.insert(tr("All"), text );

    QFileInfo cuFi( currentFileName);
    QString filee = cuFi.fileName();
    QString dire = cuFi.dirPath();
    if(dire==".")
        dire = QPEApplication::documentDir();
    QString str;
    if( !featureAutoSave)
      {
          str = OFileDialog::getSaveFileName( 2,
                                              dire,
                                              filee, map);
      }
    else
        str=currentFileName;
    if(!str.isEmpty()) {
        QString fileNm=str;

        qDebug("saving filename "+fileNm);
        QFileInfo fi(fileNm);
        currentFileName=fi.fileName();
        if(doc) 
//        QString file = doc->file();
//        doc->removeFiles();
            delete doc;
            DocLnk nf;
            nf.setType("text/plain");
            nf.setFile( fileNm);
            doc = new DocLnk(nf);
//        editor->setText(rt);
            qDebug("Saving file as "+currentFileName);
            doc->setName( currentFileName);
            updateCaption( currentFileName);

            FileManager fm;
            if ( !fm.saveFile( *doc, rt ) ) {
                return false;
            }

            if( filePerms ) {
                filePermissions *filePerm;
                filePerm = new filePermissions(this,
                                               tr("Permissions"),true,
                                               0,(const QString &)fileNm);
                filePerm->showMaximized();
                filePerm->exec();

                if( filePerm)
                    delete  filePerm;
            }
//        }
        editor->setEdited( false);
        edited1 = false;
        edited = false;
        if(caption().left(1)=="*")
            setCaption(caption().right(caption().length()-1));
        
        return true;
    }
    qDebug("returning false");
    return false;
} //end saveAs

void TextEdit::clear() {
    delete doc;
    doc = 0;
    editor->clear();
}

void TextEdit::updateCaption( const QString &name ) {
    
    if ( name.isEmpty() )
        setCaption( tr("Text Editor") );
    else {
        QString s = name;
        if ( s.isNull() )
            s = doc->name();
        if ( s.isEmpty()  ) {
            s = tr( "Unnamed" );
            currentFileName=s;
        }
//         if(s.left(1) == "/")
//             s = s.right(s.length()-1);
        setCaption( s + " - " + tr("Text Editor") );
    }
}

void TextEdit::setDocument(const QString& fileref) {
    if(fileref != "Unnamed") {
        currentFileName=fileref;
        qDebug("setDocument");
         QFileInfo fi(currentFileName);
         qDebug("basename:"+fi.baseName()+": current filenmame "+currentFileName);
         if( (fi.baseName().left(1)).isEmpty() ) {
        openDotFile(currentFileName);

         } else {
             qDebug("setDoc open");
             bFromDocView = true;
             openFile(fileref);
             editor->setEdited(true);
             edited1=false;
             edited=true;
               //   fromSetDocument=false;
 //    doSearchBar();
        }
    }
    updateCaption( currentFileName);
}

void TextEdit::changeFont() {
  QDialog *d = new QDialog ( this, "FontDialog", true );
  d-> setCaption ( tr( "Choose font" ));
  QBoxLayout *lay = new QVBoxLayout ( d );
  OFontSelector *ofs = new OFontSelector ( true, d );
  lay-> addWidget ( ofs );
  ofs-> setSelectedFont ( editor-> font ( ));

  d-> showMaximized ( );
    if ( d-> exec ( ) == QDialog::Accepted )
      editor-> setFont ( ofs-> selectedFont ( ));
    delete d;

}

void TextEdit::editDelete() {
    switch ( QMessageBox::warning(this,tr("Text Editor"),
                                  tr("Do you really want<BR>to <B>delete</B> "
                                     "the current file\nfrom the disk?<BR>This is "
                                     "<B>irreversable!!</B>"),
                                  tr("Yes"),tr("No"),0,0,1) ) {
      case 0:
          if(doc) {
              doc->removeFiles();
              clear();
              setCaption( tr("Text Editor") );
          }
          break;
      case 1: 
            // exit
          break;
    };
}

void TextEdit::changeStartConfig( bool b ) {
    startWithNew=b;
    Config cfg("TextEdit");
    cfg.setGroup("View");
    cfg.writeEntry("startNew",b);
    update();
}

void TextEdit::editorChanged() {
//    qDebug("editor changed");
    if( /*editor->edited() &&*/ /*edited && */!edited1) {
        setCaption( "*"+caption());
        edited1=true;
    }
    edited=true;
}

void TextEdit::receive(const QCString&msg, const QByteArray &) {
    qDebug("QCop "+msg);
  if ( msg == "setDocument(QString)" ) {
      qDebug("bugger all");
          
  }

}

void TextEdit::doAbout() {
    QMessageBox::about(0,tr("Text Edit"),tr("Text Edit is copyright<BR>"
                         "2000 Trolltech AS, and<BR>"
                         "2002 by <B>L. J. Potter <BR>llornkcor@handhelds.org</B><BR>"
                         "and is licensed under the GPL"));
}

void TextEdit::doPrompt(bool b) {
    promptExit=b;
    Config cfg("TextEdit");
    cfg.setGroup ( "View" );
    cfg.writeEntry ( "PromptExit", b); 
}

void TextEdit::doDesktop(bool b) {
    openDesktop=b;
    Config cfg("TextEdit");
    cfg.setGroup ( "View" );
    cfg.writeEntry ( "OpenDesktop", b); 
}

void TextEdit::doFilePerms(bool b) {
    filePerms=b;
    Config cfg("TextEdit");
    cfg.setGroup ( "View" );
    cfg.writeEntry ( "FilePermissions", b); 
}

void TextEdit::editPasteTimeDate() {
#ifndef QT_NO_CLIPBOARD
  QClipboard *cb = QApplication::clipboard();
  QDateTime dt = QDateTime::currentDateTime();
  cb->setText( dt.toString());
  editor->paste();
#endif
}

int TextEdit::savePrompt()
{
    switch( QMessageBox::information( 0, (tr("Textedit")),
                                      (tr("Textedit detected\n"
                                          "you have unsaved changes\n"
                                          "Go ahead and save?\n")),
                                      (tr("Save")), (tr("Don't Save")), (tr("&Cancel")), 2, 2 ) )
      {
        case 0:
        {
            return 1;
        }
        break;

        case 1: 
        {
            return 2;
        }
        break;

        case 2:
        {
            return -1;
        }
        break;
      };

    return 0;
}

void TextEdit::timerCrank()
{
    if(featureAutoSave && edited1)
      {
          if(currentFileName.isEmpty())
            {
                currentFileName = QDir::homeDirPath()+"/textedit.tmp";
                saveAs();
            }
          else
            {
//                qDebug("autosave");
                save();
            }
          setTimer();
      }
}

void TextEdit::doTimer(bool b)
{
    Config cfg("TextEdit");
    cfg.setGroup ( "View" );
    cfg.writeEntry ( "autosave", b); 
    featureAutoSave = b; 
    nAutoSave->setOn(b);
    if(b)
      {
//          qDebug("doTimer true");
          setTimer();      
      }
//     else
//         qDebug("doTimer false");
}

void TextEdit::setTimer()
{
if(featureAutoSave)
  {
//      qDebug("setting autosave");
      QTimer *timer = new QTimer(this );
      connect( timer, SIGNAL(timeout()), this, SLOT(timerCrank()) );
      timer->start( 300000, true); //5 minutes
    }
}

void TextEdit::gotoLine() {

   QWidget *d = QApplication::desktop();
   gotoEdit = new QLineEdit( 0, "Goto line");
   
   gotoEdit->move( (d->width()/2) - ( gotoEdit->width()/2) , (d->height()/2) - (gotoEdit->height()/2));
   gotoEdit->setFrame(true);
   gotoEdit->show();
   connect (gotoEdit,SIGNAL(returnPressed()), this, SLOT(doGoto()));
}

void TextEdit::doGoto() {
   QString number = gotoEdit->text();
   gotoEdit->hide();
   if(gotoEdit) {
      delete gotoEdit;
      gotoEdit = 0;
   }
   
   bool ok;
   int lineNumber = number.toInt(&ok, 10);
   if(editor->numLines() < lineNumber)
      QMessageBox::message(tr("Text Edit"),tr("Not enough lines"));
   else
   {
      editor->setCursorPosition(lineNumber, 0, false);
   }
}
