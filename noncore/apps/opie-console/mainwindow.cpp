#include "TEmulation.h"
#include "profileeditordialog.h"
#include "configdialog.h"
#include "default.h"
#include "profilemanager.h"
#include "mainwindow.h"
#include "tabwidget.h"
#include "transferdialog.h"
#include "function_keyboard.h"
#include "emulation_handler.h"
#include "script.h"
#include "fixit.h"

/* OPIE */
#include <opie2/ofiledialog.h>
#include <opie2/oresource.h>
#include <qpe/filemanager.h>
using namespace Opie::Ui;

/* QT */
#include <qaction.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qmessagebox.h>
#include <qwhatsthis.h>
#include <qfileinfo.h>

/* STD */
#include <assert.h>

#include <opie2/oconfig.h>

MainWindow::MainWindow(QWidget *parent, const char *name, WFlags) : QMainWindow(parent, name, WStyle_ContextHelp)  {

#ifdef FSCKED_DISTRI
   FixIt fix;
   fix.fixIt();
#endif

    setCaption(QObject::tr("Opie Console") );
    KeyTrans::loadAll();
    for (int i = 0; i < KeyTrans::count(); i++ ) {
        KeyTrans* s = KeyTrans::find(i );
        assert( s );
    }
    m_factory = new MetaFactory();
    Default def(m_factory);
    m_sessions.setAutoDelete( TRUE );
    m_curSession = 0;
    m_manager = new ProfileManager( m_factory );
    m_manager->load();
    m_scriptsData.setAutoDelete(TRUE);

    initUI();
    populateProfiles();
    populateScripts();
}

void MainWindow::initUI() {

    setToolBarsMovable( FALSE  );

    /* tool bar for the menu */
    m_tool = new QToolBar( this );
    m_tool->setHorizontalStretchable( TRUE );

    m_bar = new QMenuBar( m_tool );
    m_console = new QPopupMenu( this );
    m_scripts = new QPopupMenu( this );
    m_sessionsPop= new QPopupMenu( this );
    m_scriptsPop = new QPopupMenu( this );
    m_scrollbar = new QPopupMenu( this );

    /* add a toolbar for icons */
    m_icons = new QToolBar(this);

    /*
     * the settings action
     */
    m_setProfiles = new QAction(tr("Configure Profiles"),
                                Opie::Core::OResource::loadPixmap( "SettingsIcon", Opie::Core::OResource::SmallIcon ),
                                QString::null, 0, this, 0);
    m_setProfiles->addTo( m_console );
    connect( m_setProfiles, SIGNAL(activated() ),
             this, SLOT(slotConfigure() ) );

    m_console->insertSeparator();
    /*
     * new Action for new sessions
     */
    QAction* newCon = new QAction(tr("New Profile"),
                                  Opie::Core::OResource::loadPixmap( "new", Opie::Core::OResource::SmallIcon ),
                                  QString::null, 0, this, 0);
    newCon->addTo( m_console );
    connect( newCon, SIGNAL(activated() ),
            this, SLOT(slotNew() ) );

    m_console->insertSeparator();

    QAction *saveCon = new QAction( tr("Save Profile" ),
                                    Opie::Core::OResource::loadPixmap( "save", Opie::Core::OResource::SmallIcon ), QString::null,
                                    0, this, 0 );
    saveCon->addTo( m_console );
    connect( saveCon, SIGNAL(activated() ),
            this, SLOT(slotSaveSession() ) );
    m_console->insertSeparator();

    /*
     * connect action
     */
    m_connect = new QAction( tr("Connect"), Opie::Core::OResource::loadPixmap("console/connected",
                             Opie::Core::OResource::SmallIcon ), QString::null, 0, this, 0 );
    m_connect->addTo( m_console );
    connect(m_connect, SIGNAL(activated() ),
            this, SLOT(slotConnect() ) );

    /*
     * disconnect action
     */
    m_disconnect = new QAction( tr("Disconnect"), Opie::Core::OResource::loadPixmap("console/notconnected",
                                Opie::Core::OResource::SmallIcon ), QString::null, 0, this, 0 );
    m_disconnect->addTo( m_console );
    connect(m_disconnect, SIGNAL(activated() ),
            this, SLOT(slotDisconnect() ) );

    m_console->insertSeparator();

#ifndef EAST
    m_quickLaunch = new QAction( tr("QuickLaunch"),
                                 Opie::Core::OResource::loadPixmap("console/konsole_mini", Opie::Core::OResource::SmallIcon ),
                                 QString::null,  0,  this, 0 );
    m_quickLaunch->addTo( m_icons );
    connect( m_quickLaunch,  SIGNAL( activated() ),
             this,  SLOT( slotQuickLaunch() ) );
#endif

    QWhatsThis::add( m_icons, tr( "The shell button launches the \"default\" profile. If there is none default values are taken" ) );

    m_transfer = new QAction( tr("Transfer file..."), Opie::Core::OResource::loadPixmap("pass", Opie::Core::OResource::SmallIcon ),
                              QString::null, 0, this, 0  );
    m_transfer->addTo( m_console );
    connect(m_transfer, SIGNAL(activated() ),
            this, SLOT(slotTransfer() ) );



    /*
     * immediate change of line wrap policy
     */
    m_isWrapped = true;
    m_wrap = new QAction( tr("Line wrap"), Opie::Core::OResource::loadPixmap( "linewrap", Opie::Core::OResource::SmallIcon ),
                          QString::null, 0, this, 0, true );
    m_wrap->addTo( m_console );
    m_wrap->setOn( true );
    connect( m_wrap, SIGNAL( activated() ), SLOT( slotWrap() ) );

    /*
     * fullscreen
     */
    m_isFullscreen = false;

    m_fullscreen = new QAction( tr("Full screen"), Opie::Core::OResource::loadPixmap( "fullscreen",
                                Opie::Core::OResource::SmallIcon ), QString::null, 0, this, 0 );
    m_fullscreen->addTo( m_console );
    connect( m_fullscreen, SIGNAL( activated() ),
             this,  SLOT( slotFullscreen() ) );

	/*
	 * scrollbar
	 */
    sm_none = m_scrollbar->insertItem(tr( "None" ));
    sm_left = m_scrollbar->insertItem(tr( "Left" ));
    sm_right = m_scrollbar->insertItem(tr( "Right" ));

    m_console->insertItem(tr("Scrollbar"), m_scrollbar, -1, 0);
    connect( m_scrollbar, SIGNAL(activated(int)),
			 this, SLOT(slotScrollbarSelected(int)));

    m_console->insertSeparator();

	m_recordLog = new QAction();
	m_recordLog->setText( tr("Start log") );
	m_recordLog->addTo( m_console );
	connect(m_recordLog, SIGNAL(activated() ), 
			this, SLOT( slotSaveLog() ) );
 	m_recordingLog = false;

    QAction *a = new QAction();
    a->setText( tr("Save history") );
    a->addTo( m_console );
    connect(a, SIGNAL(activated() ),
            this, SLOT(slotSaveHistory() ) );
    /*
     * terminate action
     */
    m_terminate = new QAction();
    m_terminate->setText( tr("Terminate") );
    m_terminate->addTo( m_console );
    connect(m_terminate, SIGNAL(activated() ),
            this, SLOT(slotTerminate() ) );

    m_closewindow = new QAction();
    m_closewindow->setText( tr("Close Window") );
    m_closewindow->addTo( m_console );
    connect( m_closewindow, SIGNAL(activated() ),
            this, SLOT(slotClose() ) );


    /*
     * script actions
     */
    m_runScript_id = m_scripts->insertItem(tr("Run Script"), m_scriptsPop, -1, 0);
    connect(m_scriptsPop, SIGNAL(activated(int)), this, SLOT(slotRunScript(int)));

    m_recordScript = new QAction(tr("Record Script"), QString::null, 0, this, 0);
    m_recordScript->addTo(m_scripts);
    connect(m_recordScript, SIGNAL(activated()), this, SLOT(slotRecordScript()));

    m_saveScript = new QAction(tr("Save Script"), QString::null, 0, this, 0);
    m_saveScript->addTo(m_scripts);
    connect(m_saveScript, SIGNAL(activated()), this, SLOT(slotSaveScript()));
   
	 
	
	
    /*
     * action that open/closes the keyboard
     */
    m_openKeys = new QAction (tr("Open Keyboard..."),
                              Opie::Core::OResource::loadPixmap( "console/keys/keyboard_icon", Opie::Core::OResource::SmallIcon ),
                             QString::null, 0, this, 0);
    m_openKeys->setToggleAction(true);
    connect (m_openKeys, SIGNAL(toggled(bool)), this, SLOT(slotOpenKeb(bool)));

    /* insert the submenu */
    m_console->insertItem(tr("New from Profile"), m_sessionsPop,
                          -1, 0);

    /* insert the connection menu */
    m_bar->insertItem( tr("Connection"), m_console );

    /* the scripts menu */
#ifdef EAST
    Opie::Core::OConfig cfg("opie-console");
	cfg.setGroup("10east");
   	if( !cfg.readEntry("scripthide",0) ) {
    	m_bar->insertItem( tr("Scripts"), m_scripts );
    }
#endif

    /* and the keyboard */
    m_keyBar = new QToolBar(this);
    addToolBar( m_keyBar,  "Keyboard", QMainWindow::Top, TRUE );
    m_keyBar->setHorizontalStretchable( TRUE );
    m_keyBar->hide();

    m_kb = new FunctionKeyboard(m_keyBar);
    connect(m_kb, SIGNAL(keyPressed(FKey,ushort,ushort,bool)),
            this, SLOT(slotKeyReceived(FKey,ushort,ushort,bool)));


    a = new QAction(tr("Copy"),
                    Opie::Core::OResource::loadPixmap("copy", Opie::Core::OResource::SmallIcon ), QString::null,
                    0, this, 0 );
    //a->addTo( m_icons );
    connect( a, SIGNAL(activated() ),
             this, SLOT(slotCopy() ) );

    QAction *paste = new QAction(tr("Paste"),
                                 Opie::Core::OResource::loadPixmap("paste", Opie::Core::OResource::SmallIcon ), QString::null,
                    0, this, 0 );
    connect( paste, SIGNAL(activated() ),
             this, SLOT(slotPaste() ) );


    newCon->addTo( m_icons );
    //m_setProfiles->addTo( m_icons );
    paste->addTo( m_icons );
    m_openKeys->addTo(m_icons);
    m_fullscreen->addTo( m_icons );

    m_connect->setEnabled( false );
    m_disconnect->setEnabled( false );
    m_terminate->setEnabled( false );
    m_transfer->setEnabled( false );
    m_scripts->setItemEnabled(m_runScript_id, false);
    m_recordScript->setEnabled( false );
    m_saveScript->setEnabled( false );
    m_fullscreen->setEnabled( false );
    m_closewindow->setEnabled( false );
    m_wrap->setEnabled( false );

    /*
     * connect to the menu activation
     */
    connect( m_sessionsPop, SIGNAL(activated(int) ),
             this, SLOT(slotProfile(int) ) );

    m_consoleWindow = new TabWidget( this, "blah");
    connect(m_consoleWindow, SIGNAL(activated(Session*) ),
            this, SLOT(slotSessionChanged(Session*) ) );
    setCentralWidget( m_consoleWindow );

    slotQuickLaunch();
}

ProfileManager* MainWindow::manager() {
    return m_manager;
}
TabWidget* MainWindow::tabWidget() {
    return m_consoleWindow;
}
void MainWindow::populateProfiles() {
    m_sessionsPop->clear();
    Profile::ValueList list = manager()->all();
    for (Profile::ValueList::Iterator it = list.begin(); it != list.end(); ++it ) {
        m_sessionsPop->insertItem( (*it).name() );
    }

}

void MainWindow::populateScripts() {
    m_scriptsPop->clear();
    m_scriptsData.clear();
    DocLnkSet files(QPEApplication::documentDir(), "text/plain");
    QListIterator<DocLnk> dit(files.children());
    for (; dit.current(); ++dit) {
        if (*dit && (*dit)->name().length()>0) {
            QFileInfo info((*dit)->file());
            if (info.extension(false) == "script") {
                m_scriptsData.append(new DocLnk(**dit));
                m_scriptsPop->insertItem((*dit)->name());
            }
        }
    }

}

MainWindow::~MainWindow() {
    delete m_factory;
    manager()->save();
#ifdef FSCKED_DISTRI
    FixIt fix;
    fix.breakIt();
#endif
}

MetaFactory* MainWindow::factory() {
    return m_factory;
}

Session* MainWindow::currentSession() {
    return m_curSession;
}

QList<Session> MainWindow::sessions() {
    return m_sessions;
}

void MainWindow::slotNew() {
    ProfileEditorDialog dlg(factory() );
    dlg.setCaption( tr("New Connection") );
    int ret = QPEApplication::execDialog( &dlg );

    if ( ret == QDialog::Accepted ) {
        create( dlg.profile() );
    }
}

void MainWindow::slotRecordScript() {
    if (currentSession()) {
        currentSession()->emulationHandler()->startRecording();
        m_saveScript->setEnabled(true);
        m_recordScript->setEnabled(false);
    }
}

void MainWindow::slotSaveScript() {
    if (currentSession() && currentSession()->emulationHandler()->isRecording()) {
        QMap<QString, QStringList> map;
        QStringList text;
        text << "text/plain";
        map.insert(tr("Script"), text );
        QString filename = OFileDialog::getSaveFileName(2, QPEApplication::documentDir(), QString::null, map);
        if (!filename.isEmpty()) {
            QFileInfo info(filename);
            if (info.extension(FALSE) != "script")
                filename += ".script";
            DocLnk nf;
            nf.setType("text/plain");
            nf.setFile(filename);
            nf.setName(info.fileName());
            FileManager fm;
            fm.saveFile(nf, currentSession()->emulationHandler()->script()->script());
            currentSession()->emulationHandler()->clearScript();
            m_saveScript->setEnabled(false);
            m_recordScript->setEnabled(true);
            populateScripts();
        }
    }
}

void MainWindow::slotRunScript(int id) {
    if (currentSession()) {
        int index = m_scriptsPop->indexOf(id);
        DocLnk *lnk = m_scriptsData.at(index);
        QString filePath = lnk->file();
        Script script(filePath);
        currentSession()->emulationHandler()->runScript(&script);
    }
}

void MainWindow::slotConnect() {
    if ( currentSession() ) {
        bool ret = currentSession()->layer()->open();
        if(!ret) QMessageBox::warning(currentSession()->widgetStack(),
                                      QObject::tr("Failed"),
                                      QObject::tr("Connecting failed for this session."));
        else {
            m_connect->setEnabled( false );
            m_disconnect->setEnabled( true );

            // if it does not support file transfer, disable the menu entry
            if ( ( m_curSession->layer() )->supports()[1] == 0 ) {
                m_transfer->setEnabled( false );
            } else {
                m_transfer->setEnabled( true );
            }

            m_recordScript->setEnabled( true );
            m_scripts->setItemEnabled(m_runScript_id, true);
        }
    }
}

void MainWindow::slotDisconnect() {
    if ( currentSession() ) {
        currentSession()->layer()->close();
        m_connect->setEnabled( true );
        m_disconnect->setEnabled( false );
        m_transfer->setEnabled( false );
        m_recordScript->setEnabled( false);
        m_saveScript->setEnabled( false );
        m_scripts->setItemEnabled(m_runScript_id, false);
    }
}

void MainWindow::slotTerminate() {
    if ( currentSession() )
        currentSession()->layer()->close();

    slotClose();
    /* FIXME move to the next session */
}



void MainWindow::slotQuickLaunch()  {

    Profile prof = manager()->profile(  "default" );
    if ( prof.name() == "default"  )  {
        create( prof );
    } else {
    #ifndef EAST
        Profile newProf =  Profile( "default",  "console", "default" ,  0, 3,  0 );
        newProf.setAutoConnect( true );
        create( newProf );
        slotSaveSession();
    #endif
    }
}

void MainWindow::slotConfigure() {
    ConfigDialog conf( manager()->all(), factory() );

    int ret = QPEApplication::execDialog( &conf );

    if ( QDialog::Accepted == ret ) {
        manager()->setProfiles( conf.list() );
        manager()->save();
        populateProfiles();
    }
}
/*
 * we will remove
 * this window from the tabwidget
 * remove it from the list
 * delete it
 * and set the currentSession()
 */
void MainWindow::slotClose() {
    if (!currentSession() )
        return;

    Session* ses = currentSession();
    /* set to NULL to be safe, if its needed slotSessionChanged resets it automatically */
    m_curSession = NULL;
    tabWidget()->remove( /*currentSession()*/ses );
    /*it's autodelete */
    m_sessions.remove( ses );

    if (!currentSession() ) {
        m_connect->setEnabled( false );
        m_disconnect->setEnabled( false );
        m_terminate->setEnabled( false );
        m_transfer->setEnabled( false );
        m_recordScript->setEnabled( false );
        m_saveScript->setEnabled( false );
        m_scripts->setItemEnabled(m_runScript_id, false);
        m_fullscreen->setEnabled( false );
        m_wrap->setEnabled( false );
        m_closewindow->setEnabled( false );
    }

    m_kb->loadDefaults();
}

/*
 * We will get the name
 * Then the profile
 * and then we will make a profile
 */
void MainWindow::slotProfile( int id) {
    Profile prof = manager()->profile( m_sessionsPop->text( id)  );
    create( prof );
}



void MainWindow::create( const Profile& prof ) {
    char *homeDir = getenv("HOME");

    if ( homeDir )
        ::chdir( homeDir );
    
    if(m_curSession)
        if(m_curSession->transferDialog()) m_curSession->transferDialog()->hide();

    Session *ses = manager()->fromProfile( prof, tabWidget() );

    if((!ses) || (!ses->layer()) || (!ses->widgetStack()))
    {
        QMessageBox::warning(this,
            QObject::tr("Session failed"),
            QObject::tr("<qt>Cannot open session: Not all components were found.</qt>"));
        //if(ses) delete ses;
        return;
    }

    m_sessions.append( ses );
    tabWidget()->add( ses );
    tabWidget()->repaint();
    m_curSession = ses;

    // dicide if its a local term ( then no connction and no tranfer), maybe make a wrapper method out of it
    m_connect->setEnabled( true );
    m_disconnect->setEnabled( false );
    m_terminate->setEnabled( true );
    m_fullscreen->setEnabled( true );
    m_wrap->setEnabled( true );
    m_closewindow->setEnabled( true );
    m_transfer->setEnabled( false );
    m_recordScript->setEnabled( false );
    m_saveScript->setEnabled( false );
    m_scripts->setItemEnabled(m_runScript_id, false);

    // is io_layer wants direct connection, then autoconnect
    //if ( ( m_curSession->layer() )->supports()[0] == 1 ) {
    if (prof.autoConnect()) {
        slotConnect();
    }

    QWidget *w = currentSession()->widget();
    if(w) w->setFocus();

    if(currentSession()->profile().readNumEntry("Wrap", 80)){
        m_isWrapped = true;
    } else {
        m_isWrapped = false;
    }

    m_kb->load(currentSession()->profile());
}

void MainWindow::slotTransfer()
{
    if ( currentSession() ) {
    Session *mysession = currentSession();
    TransferDialog dlg(/*mysession->widgetStack()*/this, this);
    mysession->setTransferDialog(&dlg);
    //dlg.reparent(mysession->widgetStack(), QPoint(0, 0));
    //dlg.showMaximized();
    currentSession()->widgetStack()->addWidget(&dlg, -1);
    dlg.show();
    //dlg.exec();
    while(dlg.isRunning()) qApp->processEvents();
    mysession->setTransferDialog(0l);
    }
}


void MainWindow::slotOpenKeb(bool state) {

    if (state) m_keyBar->show();
    else m_keyBar->hide();

}


void MainWindow::slotOpenButtons( bool state ) {

    if ( state ) {
        m_buttonBar->show();
    } else {
        m_buttonBar->hide();
    }
}



void MainWindow::slotSessionChanged( Session* ses ) {
    if(m_curSession)
        if(m_curSession->transferDialog()) m_curSession->transferDialog()->hide();
    if(ses)
        if(ses->transferDialog()) ses->transferDialog()->show();

    if ( ses ) {
        m_curSession = ses;
        if ( m_curSession->layer()->isConnected() ) {
            m_connect->setEnabled( false );
            m_disconnect->setEnabled( true );
            m_recordScript->setEnabled(!m_curSession->emulationHandler()->isRecording());
            m_saveScript->setEnabled(m_curSession->emulationHandler()->isRecording());
            m_scripts->setItemEnabled(m_runScript_id, true);
        } else {
            m_connect->setEnabled( true );
            m_disconnect->setEnabled( false );
            m_recordScript->setEnabled( false );
            m_saveScript->setEnabled( false );
            m_scripts->setItemEnabled(m_runScript_id, false);
        }

		if ( ( currentSession()->emulationHandler()->isLogging() ) ) {
			 m_recordLog->setText( tr("Stop log") );
		} else {
			 m_recordLog->setText( tr("Start log") );
		}

        if ( ( m_curSession->layer() )->supports()[1] == 0 ) {
            m_transfer->setEnabled( false );
        } else {
            m_transfer->setEnabled( true );
        }

        QWidget *w = m_curSession->widget();
        if(w) w->setFocus();

        if(currentSession()->profile().readNumEntry("Wrap", 80)){
            m_isWrapped = true;
        } else {
            m_isWrapped = false;
        }

        m_kb->load(currentSession()->profile());
    }
}

void MainWindow::slotWrap()
{
    if(m_curSession)
    {
        EmulationHandler *e = m_curSession->emulationHandler();
        if(e)
        {
            e->setWrap( m_isWrapped ? 80:0 );
            m_isWrapped = !m_isWrapped;
        }
    }
}

void MainWindow::slotFullscreen() {



    if ( m_isFullscreen ) {
        ( m_curSession->widgetStack() )->reparent(  savedParentFullscreen, 0, QPoint(0,0), true );
        ( m_curSession->widgetStack() )->resize( savedParentFullscreen->width(), savedParentFullscreen->height() );
        ( m_curSession->emulationHandler() )->cornerButton()->hide();
        disconnect( ( m_curSession->emulationHandler() )->cornerButton(), SIGNAL( pressed() ), this, SLOT( slotFullscreen() ) );

    } else {
        savedParentFullscreen = ( m_curSession->widgetStack() )->parentWidget();
        ( m_curSession->widgetStack() )->setFrameStyle( QFrame::NoFrame );
        ( m_curSession->widgetStack() )->reparent( 0, WStyle_Tool | WStyle_Customize | WStyle_StaysOnTop
                                                   , QPoint(0,0), false );
        ( m_curSession->widgetStack() )->resize( qApp->desktop()->width(), qApp->desktop()->height() );
        ( m_curSession->widgetStack() )->setFocus();
        ( m_curSession->widgetStack() )->show();

        ( ( m_curSession->emulationHandler() )->cornerButton() )->show();

        connect( ( m_curSession->emulationHandler() )->cornerButton(), SIGNAL( pressed() ), this, SLOT( slotFullscreen() ) );
    }

    m_isFullscreen = !m_isFullscreen;
}

void MainWindow::slotScrollbarSelected(int index)
{
	int loc;

    Config cfg( "Konsole" );
    cfg.setGroup("ScrollBar");
    if(index == sm_none)
    {
		loc = 0;
    }
    else if(index == sm_left)
    {
		loc = 1;
    }
    else if(index == sm_right)
    {
		loc = 2;
    }
	
	cfg.writeEntry("Position", loc);

    if (currentSession()) {
        currentSession()->emulationHandler()->setScrollbarLocation(loc);
    }

    m_scrollbar->setItemChecked(sm_none, index == sm_none);
    m_scrollbar->setItemChecked(sm_left, index == sm_left);
    m_scrollbar->setItemChecked(sm_right, index == sm_right);
}

void MainWindow::slotKeyReceived(FKey k, ushort, ushort, bool pressed) {

    if ( m_curSession ) {

        QEvent::Type state;

        if (pressed) state = QEvent::KeyPress;
        else state = QEvent::KeyRelease;

        QKeyEvent ke(state, k.qcode, k.unicode, 0, QString(QChar(k.unicode)));

        // is this the best way to do this? cant figure out any other way to work
        QApplication::sendEvent((QObject *)m_curSession->widget(), &ke);
        ke.ignore();
    }
}
void MainWindow::slotCopy() {
    if (!currentSession() ) return;
    currentSession()->emulationHandler()->copy();
}
void MainWindow::slotPaste() {
    if (!currentSession() ) return;
    currentSession()->emulationHandler()->paste();
}

/*
 * Save the session
 */

void MainWindow::slotSaveSession() {
    if (!currentSession() ) {
      QMessageBox::information(this, tr("Save Connection"),
                                          tr("<qt>There is no Connection.</qt>"), 1 );
      return;
    }
    manager()->add( currentSession()->profile() );
    manager()->save();
    populateProfiles();
}



void MainWindow::slotSaveLog() {

    if( currentSession()->emulationHandler()->isLogging() ) {
      DocLnk nf;
      QString m_logName = currentSession()->emulationHandler()->logFileName();
      QFileInfo info(m_logName);
      nf.setType("text/plain");
      nf.setFile(m_logName);
      nf.setName(info.fileName());
      nf.writeLink();
      m_recordLog->setText( tr("Start log") );
      m_recordingLog = false;
	  currentSession()->emulationHandler()->clearLog(); 
    } else {
    QMap<QString, QStringList> map;
    QStringList text;
    text << "text/plain";
    map.insert(tr("Log"), text );
    Opie::Core::OConfig cfg("opie-console");
    cfg.setGroup("defaults");
    QString startDir = cfg.readEntry("defaultlogdir", QPEApplication::documentDir() );
    QString m_logName = OFileDialog::getSaveFileName(2, startDir, QString::null, map, 0, startDir);
    if (m_logName.isEmpty() ) return;

    m_recordLog->setText( tr("Stop log") ); 
    m_recordingLog = true;
    currentSession()->emulationHandler()->startLogging(m_logName); 
    }
}

void MainWindow::slotSaveHistory() {
    QMap<QString, QStringList> map;
    QStringList text;
    text << "text/plain";
    map.insert(tr("History"), text );
    QString filename = OFileDialog::getSaveFileName(2, QPEApplication::documentDir(), QString::null, map);
    if (filename.isEmpty() ) return;

    QFileInfo info(filename);

    DocLnk nf;
    nf.setType("text/plain");
    nf.setFile(filename);
    nf.setName(info.fileName());


    QFile file(filename);
    if ( !file.open(IO_WriteOnly ) ) return;

    QTextStream str(&file );
    if ( currentSession() )
        currentSession()->emulationHandler()->emulation()->streamHistory(&str);

    file.close();
    nf.writeLink();
}
