#include <assert.h>

#include <qaction.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>

#include <qpe/resource.h>

#include <opie/ofiledialog.h>

#include "keytrans.h"
#include "profileeditordialog.h"
#include "configdialog.h"
#include "default.h"
#include "metafactory.h"
#include "profile.h"
#include "profilemanager.h"
#include "mainwindow.h"
#include "tabwidget.h"
#include "transferdialog.h"
#include "function_keyboard.h"
#include "emulation_handler.h"
#include "script.h"
#include "quick_button.h"



MainWindow::MainWindow(QWidget *parent, const char *name, WFlags) : QMainWindow(parent, name, WStyle_ContextHelp)  {
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

    initUI();
    populateProfiles();
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
    //m_settings = new QPopupMenu( this );

    /* add a toolbar for icons */
    m_icons = new QToolBar(this);




    /*
     * the settings action
     */
    m_setProfiles = new QAction(tr("Configure Profiles"),
                             Resource::loadPixmap( "SettingsIcon" ),
                             QString::null, 0, this, 0);
    //  m_setProfiles->addTo( m_settings );
    m_setProfiles->addTo( m_icons );
    m_setProfiles->addTo( m_console );
    connect( m_setProfiles, SIGNAL(activated() ),
             this, SLOT(slotConfigure() ) );

    m_console->insertSeparator();
    /*
     * new Action for new sessions
     */
    QAction* a = new QAction(tr("New Connection"),
                             Resource::loadPixmap( "new" ),
                             QString::null, 0, this, 0);
    a->addTo( m_console );
    a->addTo( m_icons );
    connect(a, SIGNAL(activated() ),
            this, SLOT(slotNew() ) );

    /*
     * connect action
     */
    m_connect = new QAction();
    m_connect->setText( tr("Connect") );
    m_connect->addTo( m_console );
    connect(m_connect, SIGNAL(activated() ),
            this, SLOT(slotConnect() ) );

    /*
     * disconnect action
     */
    m_disconnect = new QAction();
    m_disconnect->setText( tr("Disconnect") );
    m_disconnect->addTo( m_console );
    connect(m_disconnect, SIGNAL(activated() ),
            this, SLOT(slotDisconnect() ) );

    m_console->insertSeparator();

    m_transfer = new QAction();
    m_transfer->setText( tr("Transfer file...") );
    m_transfer->addTo( m_console );
    connect(m_transfer, SIGNAL(activated() ),
            this, SLOT(slotTransfer() ) );


    /*
     * fullscreen
     */
    m_isFullscreen = false;

    m_fullscreen = new QAction( tr("Full screen"), Resource::loadPixmap( "fullscreen" )
                           , QString::null, 0, this, 0);
    m_fullscreen->addTo( m_console );
    m_fullscreen->addTo( m_icons );
    connect( m_fullscreen, SIGNAL( activated() ),
             this,  SLOT( slotFullscreen() ) );

    m_console->insertSeparator();
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
    m_recordScript = new QAction(tr("Record Script"), QString::null, 0, this, 0);
    m_recordScript->addTo(m_scripts);
    connect(m_recordScript, SIGNAL(activated()), this, SLOT(slotRecordScript()));

    m_saveScript = new QAction(tr("Save Script"), QString::null, 0, this, 0);
    m_saveScript->addTo(m_scripts);
    connect(m_saveScript, SIGNAL(activated()), this, SLOT(slotSaveScript()));

    m_runScript = new QAction(tr("Run Script"), QString::null, 0, this, 0);
    m_runScript->addTo(m_scripts);
    connect(m_runScript, SIGNAL(activated()), this, SLOT(slotRunScript()));

    /*
     * action that open/closes the keyboard
     */
    m_openKeys = new QAction (tr("Open Keyboard..."),
                             Resource::loadPixmap( "console/keyboard_icon.png" ),
                             QString::null, 0, this, 0);

    m_openKeys->setToggleAction(true);

    connect (m_openKeys, SIGNAL(toggled(bool)),
             this, SLOT(slotOpenKeb(bool)));
    m_openKeys->addTo(m_icons);


     /*
     * action that open/closes the keyboard
     */
    m_openButtons = new QAction ( tr( "Open Buttons..." ),
                             Resource::loadPixmap( "down" ),
                             QString::null, 0, this, 0 );

    m_openButtons->setToggleAction( true );

    connect ( m_openButtons, SIGNAL( toggled( bool ) ),
             this, SLOT( slotOpenButtons( bool ) ) );
    m_openButtons->addTo( m_icons );


    /* insert the submenu */
    m_console->insertItem(tr("New from Profile"), m_sessionsPop,
                          -1, 0);

    /* insert the connection menu */
    m_bar->insertItem( tr("Connection"), m_console );

    /* the scripts menu */
    m_bar->insertItem( tr("Scripts"), m_scripts );

    /* the settings menu */
    // m_bar->insertItem( tr("Settings"), m_settings );

    /* and the keyboard */
    m_keyBar = new QToolBar(this);
    addToolBar( m_keyBar,  "Keyboard", QMainWindow::Top, TRUE );
    m_keyBar->setHorizontalStretchable( TRUE );
    m_keyBar->hide();

    m_kb = new FunctionKeyboard(m_keyBar);
    connect(m_kb, SIGNAL(keyPressed(ushort, ushort, bool, bool, bool, ushort, ushort)),
            this, SLOT(slotKeyReceived(ushort, ushort, bool, bool, bool, ushort, ushort)));

    m_buttonBar = new QToolBar( this );
    addToolBar( m_buttonBar, "Buttons",  QMainWindow::Top, TRUE );
    m_buttonBar->setHorizontalStretchable( TRUE );
    m_buttonBar->hide();

    m_qb = new QuickButton( m_buttonBar );
    connect( m_qb, SIGNAL( keyPressed( ushort, ushort, bool, bool, bool) ),
            this, SLOT( slotKeyReceived( ushort, ushort, bool, bool, bool) ) );


    m_connect->setEnabled( false );
    m_disconnect->setEnabled( false );
    m_terminate->setEnabled( false );
    m_transfer->setEnabled( false );
    m_recordScript->setEnabled( false );
    m_saveScript->setEnabled( false );
    m_runScript->setEnabled( false );
    m_fullscreen->setEnabled( false );
    m_closewindow->setEnabled( false );

    /*
     * connect to the menu activation
     */
    connect( m_sessionsPop, SIGNAL(activated( int ) ),
             this, SLOT(slotProfile( int ) ) );

    m_consoleWindow = new TabWidget( this, "blah");
    connect(m_consoleWindow, SIGNAL(activated(Session*) ),
            this, SLOT(slotSessionChanged(Session*) ) );
    setCentralWidget( m_consoleWindow );

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
MainWindow::~MainWindow() {
    delete m_factory;
    manager()->save();
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
    dlg.showMaximized();
    int ret = dlg.exec();

    if ( ret == QDialog::Accepted ) {
        create( dlg.profile() );
    }
}

void MainWindow::slotRecordScript() {
    if (currentSession()) {
        currentSession()->emulationHandler()->startRecording();
    }
}

void MainWindow::slotSaveScript() {
    if (currentSession() && currentSession()->emulationHandler()->isRecording()) {
        MimeTypes types;
        QStringList script;
        script << "text/plain";
        script << "text/all";
        script << "application/octet-stream";
        types.insert("Script", script);
        QString filename = OFileDialog::getSaveFileName(2, "/", QString::null, types);
        if (!filename.isEmpty()) {
            currentSession()->emulationHandler()->script()->saveTo(filename);
            currentSession()->emulationHandler()->clearScript();
        }
    }
}

void MainWindow::slotRunScript() {
    if (currentSession()) {
        MimeTypes types;
        QStringList script;
        script << "text/plain";
        script << "text/all";
        script << "application/octet-stream";
        types.insert("Script", script);
        QString filename = OFileDialog::getOpenFileName(2, "/", QString::null, types);
        if (!filename.isEmpty()) {
            Script script(DocLnk(filename).file());
            currentSession()->emulationHandler()->runScript(&script);
        }
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
        }
    }
}

void MainWindow::slotDisconnect() {
    if ( currentSession() ) {
        currentSession()->layer()->close();
        m_connect->setEnabled( true );
        m_disconnect->setEnabled( false );
    }
}

void MainWindow::slotTerminate() {
    if ( currentSession() )
        currentSession()->layer()->close();

    slotClose();
    /* FIXME move to the next session */
}

void MainWindow::slotConfigure() {
    ConfigDialog conf( manager()->all(), factory() );
    conf.showMaximized();

    int ret = conf.exec();

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
    qWarning("removing! currentSession %s", currentSession()->name().latin1() );
	/* set to NULL to be safe, if its needed slotSessionChanged resets it automatically */
	m_curSession = NULL;
    tabWidget()->remove( /*currentSession()*/ses );
    /*it's autodelete */
    m_sessions.remove( ses );
    qWarning("after remove!!");

    if (!currentSession() ) {
        m_connect->setEnabled( false );
        m_disconnect->setEnabled( false );
        m_terminate->setEnabled( false );
        m_transfer->setEnabled( false );
        m_recordScript->setEnabled( false );
        m_saveScript->setEnabled( false );
        m_runScript->setEnabled( false );
        m_fullscreen->setEnabled( false );
        m_closewindow->setEnabled( false );
    }
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
    m_curSession = ses;

    // dicide if its a local term ( then no connction and no tranfer), maybe make a wrapper method out of it
    m_connect->setEnabled( true );
    m_disconnect->setEnabled( false );
    m_terminate->setEnabled( true );
    m_transfer->setEnabled( true );
    m_recordScript->setEnabled( true );
    m_saveScript->setEnabled( true );
    m_runScript->setEnabled( true );
    m_fullscreen->setEnabled( true );
    m_closewindow->setEnabled( true );


    // is io_layer wants direct connection, then autoconnect
    //if ( ( m_curSession->layer() )->supports()[0] == 1 ) {
    if (prof.autoConnect()) {
        slotConnect();
    }

    QWidget *w = currentSession()->widget();
    if(w) w->setFocus();
}

void MainWindow::slotTransfer()
{
    if ( currentSession() ) {
	TransferDialog dlg(currentSession()->widgetStack(), this);
	dlg.showMaximized();
	//currentSession()->widgetStack()->add(dlg);
	dlg.exec();
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
    qWarning("changed!");
    if ( ses ) {
        m_curSession = ses;
        qDebug(QString("is connected : %1").arg(  m_curSession->layer()->isConnected() ) );
        if ( m_curSession->layer()->isConnected() ) {
            m_connect->setEnabled( false );
            m_disconnect->setEnabled( true );
        } else {
            m_connect->setEnabled( true );
            m_disconnect->setEnabled( false );
        }

        QWidget *w = m_curSession->widget();
        if(w) w->setFocus();
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


void MainWindow::slotKeyReceived(ushort u, ushort q, bool, bool pressed, bool, ushort, ushort) {

    //qWarning("unicode: %x, qkey: %x, %s", u, q, pressed ? "pressed" : "released");

    if ( m_curSession ) {

        QEvent::Type state;

        if (pressed) state = QEvent::KeyPress;
        else state = QEvent::KeyRelease;

        QKeyEvent ke(state, q, u, 0, QString(QChar(u)));

        // where should i send this event? doesnt work sending it here
        QApplication::sendEvent((QObject *)m_curSession->widget(), &ke);
        ke.ignore();
    }
}
