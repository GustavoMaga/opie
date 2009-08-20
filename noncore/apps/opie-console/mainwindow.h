#ifndef OPIE_MAIN_WINDOW_H
#define OPIE_MAIN_WINDOW_H

#include <qmainwindow.h>
#include <qlist.h>
#include <qfile.h>

#include "session.h"

/**
 * this is the MainWindow of the new opie console
 * it's also the dispatcher between the different
 * actions supported by the gui
 */
class QToolBar;
class QToolButton;
class QMenuBar;
class QAction;
class MetaFactory;
class TabWidget;
class ProfileManager;
class Profile;
class FunctionKeyboard;
class FKey;
class DocLnk;


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow( QWidget *parent = 0, const char *name = 0, WFlags fl = 0 );
    ~MainWindow();
    static QString appName() {return QString::fromLatin1("opie-console"); }

    /**
     * our factory to generate IOLayer and so on
     *
     */
    MetaFactory* factory();

    /**
     * A session contains a QWidget*,
     * an IOLayer* and some infos for us
     */
    Session* currentSession();

    /**
     * the session list
     */
    QList<Session> sessions();

    /**
     *
     */
    ProfileManager* manager();
    TabWidget* tabWidget();

private slots:
    void slotNew();
    void slotConnect();
    void slotDisconnect();
    void slotTerminate();
    void slotConfigure();
    void slotClose();
    void slotProfile(int);
    void slotTransfer();
    void slotOpenKeb(bool);
    void slotOpenButtons(bool);
    void slotRecordScript();
    void slotSaveScript();
    void slotRunScript(int);
    void slotFullscreen();
    void slotQuickLaunch();
    void slotWrap();
    void slotSessionChanged( Session* );
    void slotSessionClosed( Session* );
    void slotKeyReceived(FKey, ushort, ushort, bool);
    void slotSaveHistory();
    void slotSaveLog();
    void slotScrollbarSelected(int);

    /* what could these both slot do? */
    void slotCopy();
    void slotPaste();

    /* save the currentSession() to Profiles */
    void slotSaveSession();

private:
    void initUI();
    void populateProfiles();
    void populateScripts();
    void create( const Profile& );
    /**
     * the current session
     */
    Session* m_curSession;

    /**
     * the session list
     */
    QList<Session> m_sessions;
    QList<DocLnk>  m_scriptsData;

    /**
     * the metafactory
     */
    MetaFactory* m_factory;
    ProfileManager* m_manager;

    /*
     * scrollbar
     */

    int sm_none, sm_left, sm_right;

    TabWidget* m_consoleWindow;
    QToolBar* m_tool;
    QToolBar* m_icons;
    QToolBar* m_keyBar;
    QToolBar* m_buttonBar;
    QMenuBar* m_bar;
    QPopupMenu* m_console;
    QPopupMenu* m_sessionsPop;
    QPopupMenu* m_scriptsPop;
    QPopupMenu* m_scripts;
    QPopupMenu* m_scrollbar;
    QAction* m_connect;
    QAction* m_disconnect;
    QAction* m_quickLaunch;
    QAction* m_terminate;
    QAction* m_transfer;
    QAction* m_setProfiles;
    QAction* m_openKeys;
    QAction* m_openButtons;
    QAction* m_recordScript;
    QAction* m_saveScript;
    QAction* m_fullscreen;
    QAction* m_wrap;
    QAction* m_closewindow;
    QAction* m_recordLog;

    FunctionKeyboard *m_kb;
    int m_runScript_id;
    bool m_isFullscreen;
    bool m_isWrapped;
    bool m_recordingLog;
    bool m_closeOnEmpty;

    QWidget* savedParentFullscreen;
};


#endif
