/* this is a straight copy of datemonthview. We can not make child of
 * it 'cause the origin view isn't virtual in any form.
 */
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
#ifndef ODATEBOOKMONTH
#define ODATEBOOKMONTH

#include <qtopia/private/event.h>
#include <qpe/datebookmonth.h>

#include <qvbox.h>
#include <qhbox.h>
#include <qdatetime.h>
#include <qvaluelist.h>
#include <qtable.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>

#include <qpe/calendar.h>
#include <qpe/timestring.h>

class QToolButton;
class QComboBox;
class QSpinBox;
class Event;
class DateBookDB;
class DateBookDBHoliday;

class ODateBookMonthTablePrivate;
class ODateBookMonthTable : public QTable
{
    Q_OBJECT

public:
    ODateBookMonthTable( QWidget *parent = 0, const char *name = 0,
                        DateBookDBHoliday *newDb = 0 );
    virtual ~ODateBookMonthTable();
    void setDate( int y, int m, int d );
    void redraw();

    QSize minimumSizeHint() const { return sizeHint(); }
    QSize minimumSize() const { return sizeHint(); }
    void  getDate( int& y, int &m, int &d ) const {y=selYear;m=selMonth;d=selDay;}
    void setWeekStart( bool onMonday );
signals:
    void dateClicked( int year, int month, int day );

protected:
    virtual void viewportMouseReleaseEvent( QMouseEvent * );

protected slots:

    virtual void keyPressEvent(QKeyEvent *e ) {
    e->ignore();
    }

private slots:
    void dayClicked( int row, int col );
    void dragDay( int row, int col );

private:
    void setupTable();
    void setupLabels();

    void findDay( int day, int &row, int &col );
    bool findDate( QDate date, int &row, int &col );
    void getEvents();
    void changeDaySelection( int row, int col );
    QDate getDateAt( int row, int col );

    int year, month, day;
    int selYear, selMonth, selDay;
    QValueList<Event> monthsEvents; // not used anymore...
    DateBookDBHoliday *db;
    ODateBookMonthTablePrivate *d;
};

class ODateBookMonthPrivate;
class ODateBookMonth : public QVBox
{
    Q_OBJECT

public:
    /* ac = Auto Close */
    ODateBookMonth( QWidget *parent = 0, const char *name = 0, bool ac = FALSE,
                   DateBookDBHoliday *data = 0 );
    virtual ~ODateBookMonth();
    QDate  selectedDate() const;

signals:
    /* ### FIXME add a signal with QDate -zecke */
    void dateClicked( int year, int month, int day );

public slots:
    void setDate( int y, int m );
    void setDate( int y, int m, int d );
    void setDate( QDate );
    void redraw();
    void slotWeekChange( bool );

protected slots:
    virtual void keyPressEvent(QKeyEvent *e);

private slots:
    void forwardDateClicked( int y, int m, int d ) { emit dateClicked(  y, m, d  ); }
    void finalDate(int, int, int);

private:
    DateBookMonthHeader *header;
    ODateBookMonthTable *table;
    int year, month, day;
    bool autoClose;
    class ODateBookMonthPrivate *d;
};

#endif
