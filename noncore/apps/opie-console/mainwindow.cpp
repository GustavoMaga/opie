
#include <qaction.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>

#include "configdialog.h"
#include "metafactory.h"
#include "profilemanager.h"
#include "mainwindow.h"
#include "tabwidget.h"

MainWindow::MainWindow() {
    m_factory = new MetaFactory();
    m_sessions.setAutoDelete( TRUE );
    m_curSession = 0;
    m_manager = new ProfileManager( m_factory );
    m_manager->load();

    initUI();
    populateProfiles();
}
void MainWindow::initUI() {
    setToolBarsMovable( FALSE  );

    m_tool = new QToolBar( this );
    m_tool->setHorizontalStretchable( TRUE );

    m_bar = new QMenuBar( m_tool );
    m_console = new QPopupMenu( this );
    m_sessionsPop= new QPopupMenu( this );
    m_settings = new QPopupMenu( this );

    /*
     * new Action for new sessions
     */
    QAction* a = new QAction();
    a->setText( tr("New Connection") );
    a->addTo( m_console );
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

    /*
     * terminate action
     */
    m_terminate = new QAction();
    m_terminate->setText( tr("Terminate") );
    m_terminate->addTo( m_console );
    connect(m_disconnect, SIGNAL(activated() ),
            this, SLOT(slotTerminate() ) );

    a = new QAction();
    a->setText( tr("Close Window") );
    a->addTo( m_console );
    connect(a, SIGNAL(activated() ),
            this, SLOT(slotClose() ) );

    /*
     * the settings action
     */
    m_setProfiles = new QAction();
    m_setProfiles->setText( tr("Configure Profiles") );
    m_setProfiles->addTo( m_settings );
    connect( m_setProfiles, SIGNAL(activated() ),
             this, SLOT(slotConfigure() ) );

    /* insert the submenu */
    m_console->insertItem(tr("New from Profile"), m_sessionsPop,
                          -1, 0);

    /* insert the connection menu */
    m_bar->insertItem( tr("Connection"), m_console );

    /* the settings menu */
    m_bar->insertItem( tr("Settings"), m_settings );

    /*
     * connect to the menu activation
     */
    connect( m_sessionsPop, SIGNAL(activated( int ) ),
             this, SLOT(slotProfile( int ) ) );

    m_consoleWindow = new TabWidget( this, "blah");
    setCentralWidget( m_consoleWindow );

}

ProfileManager* MainWindow::manager() {
    return m_manager;
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
    qWarning("New Connection");
}

void MainWindow::slotConnect() {
    if ( currentSession() )
        currentSession()->layer()->open();
}

void MainWindow::slotDisconnect() {
    if ( currentSession() )
        currentSession()->layer()->close();
}

void MainWindow::slotTerminate() {
    if ( currentSession() )
        currentSession()->layer()->close();
    delete m_curSession;
    m_curSession = 0l;
    /* FIXME move to the next session */
}

void MainWindow::slotConfigure() {
    qWarning("configure");
    ConfigDialog conf( manager()->all() );
    conf.showMaximized();

    int ret = conf.exec();

    if ( QDialog::Accepted == ret ) {
        manager()->setProfiles( conf.list() );
        populateProfiles();
    }
}

void MainWindow::slotClose() {
}

void MainWindow::slotProfile( int ) {


}
