/***************************************************************************
                          mainwin.h  -  description
                             -------------------
    begin                : Mon Aug 26 13:32:30 BST 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MAINWIN_H
#define MAINWIN_H

#include <qmainwindow.h>
#include <qpixmap.h>
//#include <qwidgetstack.h>

class QWidgetStack;
class QPEToolBar;
class QAction;
class ProgressWidget;
class NetworkPackageManager;
class DataManager;

class MainWindow :public  QMainWindow
{
	Q_OBJECT
public:

	MainWindow();
	~MainWindow();

private:
    DataManager *mgr;

    QMenuBar *menu;
    QPopupMenu *help;
    QPopupMenu *settings;
    QPopupMenu *edit;
    QPopupMenu *filter;
    QWidgetStack *stack;
    QAction *actionUpgrade;
    QAction *actionDownload;
    QPixmap iconDownload;
    QPixmap iconRemove;
    
    NetworkPackageManager *networkPkgWindow;
    ProgressWidget *progressWindow;

    int mnuShowUninstalledPkgsId;
    int mnuShowInstalledPkgsId;
    int mnuShowUpgradedPkgsId;
    int mnuFilterByCategory;
    int mnuSetFilterCategory;

public slots:
    void setDocument( const QString &doc );
    void displayHelp();
    void searchForPackage();
    void repeatSearchForPackage();
    void displayAbout();
    void displaySettings();
    void filterUninstalledPackages();
    void filterInstalledPackages();
    void filterUpgradedPackages();
    void filterCategory();
    void setFilterCategory();
    void raiseMainWidget();
    void raiseProgressWidget();
    void enableUpgrade( bool );
    void enableDownload( bool );

private slots:
    void init();
};
#endif
