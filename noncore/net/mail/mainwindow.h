#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qlistview.h>
#include <qaction.h>

#include <qtoolbar.h>
#include <qmenubar.h>

#include "accountview.h"
#include "statuswidget.h"

class RecMail;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0, const char *name = 0, WFlags flags = 0 );

public slots:
    void slotAdjustColumns();
    
protected slots:
    virtual void slotShowFolders( bool show );
    virtual void refreshMailView(QList<RecMail>*);
    virtual void displayMail(QListViewItem*);
    virtual void slotDeleteMail();
    void slotAdjustLayout();
    void slotEditSettings();
    
protected:
    QToolBar *toolBar;
    StatusWidget *statusWidget;
    QMenuBar *menuBar;
    QPopupMenu *mailMenu, *settingsMenu;
    QAction *composeMail, *sendQueued, *showFolders, *searchMails, *deleteMails, 
            *editSettings, *editAccounts, *syncFolders;
    AccountView *folderView;
    QListView *mailView;
    QBoxLayout *layout;
};

#endif
