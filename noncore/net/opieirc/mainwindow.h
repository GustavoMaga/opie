/*
    OpieIRC - An embedded IRC client
    Copyright (C) 2002 Wenzel Jakob

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <qmainwindow.h>
#include <qaction.h>
#include <qlist.h>
#include "mainwindow.h"
#include "ircmisc.h"
#include "irctab.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0, const char *name = 0, WFlags f = 0);
//    IRCTabWidget getTabWidget();
    void addTab(IRCTab *tab);
    void killTab(IRCTab *tab);
signals:
    void updateScroll();
protected slots:
    void newConnection();
    void settings();
    void selected(QWidget *);
    void changeEvent(IRCTab *);
protected:
    void loadSettings();
protected:
    IRCTabWidget *m_tabWidget;
    QList<IRCTab> m_tabs;
};

#endif /* __MAINWINDOW_H */
