/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// (c) 2002 Patrick S. Vogt <tille@handhelds.org>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qdialog.h>
#include <qaction.h>
#include <qlist.h>
#include <qtimer.h>
#include <qpopupmenu.h>

class QPEToolBar;
class QVBoxLayout;
class QHBoxLayout;
class QTextView;
class QFrame;
class QListViewItem;
class OListView;
class OListViewItem;
class QHButtonGroup;

class SearchGroup;

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
  MainWindow( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
  ~MainWindow();


public slots:
  void setCurrent(QListViewItem*);
  void showPopup();
  void stopTimer( QListViewItem* );
  void setSearch( const QString& );

protected slots:
  void slotAction(int);

private:
  OListView *resultsList;
  QTextView *richEdit;
  OListViewItem *_currentItem;
  QVBoxLayout *mainLayout;
  QHBoxLayout *buttonLayout;
  QFrame *detailsFrame;
//  OListViewItem *_item;
  QTimer *popupTimer;

  QList<SearchGroup> searches;
  QHButtonGroup *buttonGroupActions;
  uint _buttonCount;
  void makeMenu();
};

#endif

