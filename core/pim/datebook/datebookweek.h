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
#ifndef DATEBOOKWEEK
#define DATEBOOKWEEK

#include <qpe/event.h>

#include <qlist.h>
#include <qvaluelist.h>
#include <qvector.h>
#include <qscrollview.h>
#include <qstring.h>
#include <qvaluelist.h>
#include "layoutmanager.h"

class DateBookDB;
class DateBookWeekHeader;
class QDate;
class QLabel;
class QResizeEvent;
class QSpinBox;
class QTimer;
class QHeader;

class DateBookWeekView : public QScrollView
{
    Q_OBJECT
public:
    DateBookWeekView( bool ampm, bool weekOnMonday, QWidget *parent = 0,
		      const char *name = 0 );

    bool whichClock() const;
    void showEvents( QValueList<EffectiveEvent> &ev );
    void moveToHour( int h );
    void setStartOfWeek( bool bOnMonday );

signals:
    void showDay( int d );
    void signalShowEvent( QValueList<EffectiveEvent> & );
    void signalHideEvent();

protected slots:
    void keyPressEvent(QKeyEvent *);

private slots:
    void slotChangeClock( bool );
    void alterDay( int );

private:
    void positionItem( LayoutItem *i );
    LayoutItem *intersects( const LayoutItem * );
    void drawContents( QPainter *p, int cx, int cy, int cw, int ch );
    void drawFrame( QPainter *p);
    void contentsMousePressEvent( QMouseEvent * );
    void contentsMouseReleaseEvent( QMouseEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void resizeEvent( QResizeEvent * );
    void initNames();

private:
    bool ampm;
    bool bOnMonday;
    QHeader *header;
    QVector<LayoutManager> items;
    QList<LayoutItem> dayItems;
    int rowHeight;
    bool showingEvent;
};

class DateBookWeek : public QWidget
{
    Q_OBJECT

public:
    DateBookWeek( bool ampm, bool weekOnMonday, DateBookDB *newDB,
		  QWidget *parent = 0, const char *name = 0 );
    void setDate( int y, int m, int d );
    void setDate( QDate d );
    QDate date() const;
    DateBookWeekView *weekView() const { return view; }
    void setStartViewTime( int startHere );
    int startViewTime() const;
    int week() const { return _week; };
    void setTotalWeeks( int totalWeeks );
    int totalWeeks() const;
    QDate weekDate() const;

public slots:
    void redraw();
    void slotWeekChanged( bool bStartOnMonday );
    void slotClockChanged( bool a );

signals:
    void showDate( int y, int m, int d );

protected slots:
    void keyPressEvent(QKeyEvent *);

private slots:
    void showDay( int day );
    void dateChanged( int y, int w );
    void slotShowEvent( QValueList<EffectiveEvent> & );
    void slotHideEvent();
    void slotYearChanged( int );

private:
    void getEvents();
    int year;
    int _week;
    int dow;
    DateBookWeekHeader *header;
    DateBookWeekView *view;
    DateBookDB *db;
    QLabel *lblDesc;
    QTimer *tHide;
    int startTime;
    bool ampm;
    bool bStartOnMonday;
};


bool calcWeek( const QDate &d, int &week, int &year,
	       bool startOnMonday = false );
#endif
