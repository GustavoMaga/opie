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

#ifndef __fifteenapplet_h__
#define __fifteenapplet_h__

#include <qmainwindow.h>
#include <qtableview.h>
#include <qarray.h>
#include <qpointarray.h>
#include <qpixmap.h>
#include <qvector.h>

class QPopupMenu;
class FifteenConfigDialog;

class PiecesTable : public QTableView
{
  Q_OBJECT

 public:
  PiecesTable(QWidget* parent = 0, const char* name = 0);
  ~PiecesTable();

 protected slots:
  void slotConfigure();
  void slotCustomImage(const QString &str, bool upd = false);
  void slotRandomize();
  void slotReset();

 protected:
  void resizeEvent(QResizeEvent*);
  void mousePressEvent(QMouseEvent*);

  void paintCell(QPainter *, int row, int col);

  void initImage();
  void initMap();
  void initColors();
  void randomizeMap();
  void checkwin();
  void readConfig();
  void writeConfig();

  void initPolygon(int w, int h, int x_of, int y_of );
 private:
  void clear();
  QString        _image;
  QArray<int>    _map;
  QArray<QColor> _colors;
  QArray<QPixmap*> _pixmap;

  QPopupMenu     *_menu;
  bool           _randomized;
  QPointArray	light_border;
  QPointArray	dark_border;
  FifteenConfigDialog *_dialog;

  enum MenuOp { mRandomize = 1, mReset = 2 };
};

class FifteenWidget : public QWidget
{
    Q_OBJECT

public:
    FifteenWidget(QWidget *parent = 0, const char *name = 0);

private:
    PiecesTable *_table;
};


class FifteenMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static QString appName() {
        return QString::fromLatin1("fifteen");
    }
    FifteenMainWindow(QWidget *parent=0, const char* name=0, WFlags fl=0);
};

#endif
