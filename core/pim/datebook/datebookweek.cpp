/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
:     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "datebookweek.h"
#include "datebookweekheaderimpl.h"
#include "datebooktypes.h"

#include <qpe/datebookdb.h>
#include <qpe/qpeapplication.h>
#include <qpe/calendar.h>

#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>

//-----------------------------------------------------------------


DateBookWeekItem::DateBookWeekItem( const EffectiveEvent e )
    : ev( e )
{
    // with the current implementation change the color for all day events
    if ( ev.event().type() == Event::AllDay && !ev.event().hasAlarm() ) {
        c = Qt::green;
    } else {
        c = ev.event().hasAlarm() ? Qt::red : Qt::blue;
    }
}

void DateBookWeekItem::setGeometry( int x, int y, int w, int h )
{
    r.setRect( x, y, w, h );
}


//------------------=---------------------------------------------

DateBookWeekView::DateBookWeekView( bool ap, bool startOnMonday,
                    QWidget *parent, const char *name )
    : QScrollView( parent, name ), ampm( ap ), bOnMonday( startOnMonday ),
      showingEvent( false )
{
    items.setAutoDelete( true );

    viewport()->setBackgroundMode( PaletteBase );

    header = new QHeader( this );
    header->addLabel( "" );

    header->setMovingEnabled( false );
    header->setResizeEnabled( false );
    header->setClickEnabled( false, 0 );
    initNames();


    connect( header, SIGNAL(clicked(int)), this, SIGNAL(showDay(int)) );

    QObject::connect(qApp, SIGNAL(clockChanged(bool)),
        this, SLOT(slotChangeClock(bool)));

    QFontMetrics fm( font() );
    rowHeight = fm.height()+2;

    resizeContents( width(), 24*rowHeight );
}

void DateBookWeekView::initNames()
{

    static bool bFirst = true;
    if ( bFirst ) {
    if ( bOnMonday ) {
                    for ( int i = 1; i<=7; i++  )  {
                         header->addLabel( Calendar::nameOfDay( i ) );
                    }

    } else {
                    header->addLabel( Calendar::nameOfDay( 7 ) );
                    for ( int i = 1; i<7; i++  )  {
                         header->addLabel( Calendar::nameOfDay( i ) );
                    }
                }
    bFirst = false;
    } else {
    // we are change things...
    if ( bOnMonday ) {
                    for ( int i = 1; i<=7; i++  )  {
                        header->setLabel( i, Calendar::nameOfDay( i ) );
                    }

    } else {
                    header->setLabel( 1, Calendar::nameOfDay( 7 ) );
                    for ( int i = 1; i<7; i++  )  {
                        header->setLabel( i+1, Calendar::nameOfDay( i ) );
                    }

    }
    }
}



void DateBookWeekView::showEvents( QValueList<EffectiveEvent> &ev )
{
    items.clear();
    QValueListIterator<EffectiveEvent> it;
    for ( it = ev.begin(); it != ev.end(); ++it ) {
        DateBookWeekItem *i = new DateBookWeekItem( *it );
        if(!((i->event().end().hour()==0) && (i->event().end().minute()==0) && (i->event().startDate()!=i->event().date()))) {  // Skip events ending at 00:00 starting at another day.
            positionItem( i );
            items.append( i );
        }
    }
    viewport()->update();
}

void DateBookWeekView::moveToHour( int h )
{
    int offset = h*rowHeight;
    setContentsPos( 0, offset );
}

void DateBookWeekView::keyPressEvent( QKeyEvent *e )
{
    e->ignore();
}

void DateBookWeekView::slotChangeClock( bool c )
{
    ampm = c;
    viewport()->update();
}

static inline int db_round30min( int m )
{
    if ( m < 15 )
    m = 0;
    else if ( m < 45 )
    m = 1;
    else
    m = 2;

    return m;
}

void DateBookWeekView::alterDay( int day )
{
    if ( !bOnMonday ) {
        day--;
    }
    emit showDay( day );
}

void DateBookWeekView::positionItem( DateBookWeekItem *i )
{
    const int Width = 8;
    const EffectiveEvent ev = i->event();

    // 30 minute intervals
    int y = ev.start().hour() * 2;
    y += db_round30min( ev.start().minute() );
    int y2 = ev.end().hour() * 2;
    y2 += db_round30min( ev.end().minute() );
    if ( y > 47 ) y = 47;
    if ( y2 > 48 ) y2 = 48;
    y = (y * rowHeight) / 2;
    y2 = (y2 * rowHeight) / 2;

    int h;
    if ( ev.event().type() == Event::AllDay ) {
        h = (48 * rowHeight) / 2;
        y = 0;
    } else {
        h=y2-y;
        if ( h < (1*rowHeight)/2 ) h = (1*rowHeight)/2;
    }

    int dow = ev.date().dayOfWeek();
    if ( !bOnMonday ) {
    if ( dow == 7 )
        dow = 1;
    else
        dow++;
    }
    int x = header->sectionPos( dow ) - 1;
    int xlim = header->sectionPos( dow ) + header->sectionSize( dow );
    DateBookWeekItem *isect = 0;
    do {
    i->setGeometry( x, y, Width, h );
    isect = intersects( i );
    x += Width - 1;
    } while ( isect && x < xlim );
}

DateBookWeekItem *DateBookWeekView::intersects( const DateBookWeekItem *item )
{
    QRect geom = item->geometry();

    // We allow the edges to overlap
    geom.moveBy( 1, 1 );
    geom.setSize( geom.size()-QSize(2,2) );

    QListIterator<DateBookWeekItem> it(items);
    for ( ; it.current(); ++it ) {
    DateBookWeekItem *i = it.current();
    if ( i != item ) {
        if ( i->geometry().intersects( geom ) ) {
        return i;
        }
    }
    }

    return 0;
}

void DateBookWeekView::contentsMousePressEvent( QMouseEvent *e )
{
    QListIterator<DateBookWeekItem> it(items);
    for ( ; it.current(); ++it ) {
    DateBookWeekItem *i = it.current();
    if ( i->geometry().contains( e->pos() ) ) {
        showingEvent = true;
        emit signalShowEvent( i->event() );
        break;
    }
    }
}

void DateBookWeekView::contentsMouseReleaseEvent( QMouseEvent *e )
{
    if ( showingEvent ) {
    showingEvent = false;
    emit signalHideEvent();
    } else {
    int d = header->sectionAt( e->pos().x() );
    if ( d > 0 ) {
//      if ( !bOnMonday )
//      d--;
        emit showDay( d );
    }
    }
}

void DateBookWeekView::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    QRect ur( cx, cy, cw, ch );
    p->setPen( lightGray );
    for ( int i = 1; i <= 7; i++ )
    p->drawLine( header->sectionPos(i)-2, cy, header->sectionPos(i)-2, cy+ch );

    p->setPen( black );
    for ( int t = 0; t < 24; t++ ) {
    int y = t*rowHeight;
    if ( QRect( 1, y, 20, rowHeight ).intersects( ur ) ) {
        QString s;
        if ( ampm ) {
        if ( t == 0 )
            s = QString::number( 12 );
        else if ( t == 12 )
            s = QString::number(12) + tr( "p" );
        else if ( t > 12 ) {
            if ( t - 12 < 10 )
            s = " ";
            else
            s = "";
            s += QString::number( t - 12 ) + tr("p");
        } else {
            if ( 12 - t < 3 )
            s = "";
            else
            s = " ";
            s += QString::number( t );
        }
        } else {
        s = QString::number( t );
        if ( s.length() == 1 )
            s.prepend( "0" );
        }
        p->drawText( 1, y+p->fontMetrics().ascent()+1, s );
    }
    }

    QListIterator<DateBookWeekItem> it(items);
    for ( ; it.current(); ++it ) {
    DateBookWeekItem *i = it.current();
    if ( i->geometry().intersects( ur ) ) {
        p->setBrush( i->color() );
        p->drawRect( i->geometry() );
    }
    }
}

void DateBookWeekView::resizeEvent( QResizeEvent *e )
{
    const int hourWidth = 20;
    QScrollView::resizeEvent( e );


    //HEAD
    /*
    int avail = visibleWidth();
    header->setGeometry( leftMargin()+frameWidth()+frameRect().left() , frameWidth(),
              visibleWidth(), header->sizeHint().height() );
    setMargins( 0, header->sizeHint().height(), 0, 0 );
    */
    //BRANCH_1_0
    int avail = width()-qApp->style().scrollBarExtent().width()-1;
    header->setGeometry( 0, 0, avail, header->sizeHint().height() );
    setMargins( 0, header->height(), 0, 0 );


    header->resizeSection( 0, hourWidth );
    int sw = (avail - hourWidth) / 7;
    for ( int i = 1; i < 7; i++ )
    header->resizeSection( i, sw );
    header->resizeSection( 7, avail - hourWidth - sw*6 );
}

void DateBookWeekView::setStartOfWeek( bool bStartOnMonday )
{
    bOnMonday = bStartOnMonday;
    initNames();
}

//-------------------------------------------------------------------

DateBookWeek::DateBookWeek( bool ap, bool startOnMonday, DateBookDBHoliday *newDB,
                QWidget *parent, const char *name )
    : QWidget( parent, name ),
      db( newDB ),
      startTime( 0 ),
      ampm( ap ),
      bStartOnMonday( startOnMonday )
{
    setFocusPolicy(StrongFocus);
    QVBoxLayout *vb = new QVBoxLayout( this );
    header = new DateBookWeekHeader( bStartOnMonday, this );
    view = new DateBookWeekView( ampm, startOnMonday, this );
    vb->addWidget( header );
    vb->addWidget( view );

    lblDesc = new QLabel( this, "event label" );
    lblDesc->setFrameStyle( QFrame::Plain | QFrame::Box );
    lblDesc->setBackgroundColor( yellow );
    lblDesc->hide();

    tHide = new QTimer( this );

    connect( view, SIGNAL( showDay(int) ), this, SLOT( showDay(int) ) );
    connect( view, SIGNAL(signalShowEvent(const EffectiveEvent&)), this, SLOT(slotShowEvent(const EffectiveEvent&)) );
    connect( view, SIGNAL(signalHideEvent()), this, SLOT(slotHideEvent()) );
    connect( header, SIGNAL( dateChanged(QDate&) ), this, SLOT( dateChanged(QDate&) ) );
    connect( tHide, SIGNAL( timeout() ), lblDesc, SLOT( hide() ) );
    connect( qApp, SIGNAL(weekChanged(bool)), this, SLOT(slotWeekChanged(bool)) );
    connect( qApp, SIGNAL(clockChanged(bool)), this, SLOT(slotClockChanged(bool)));
    setDate(QDate::currentDate());
}

void DateBookWeek::keyPressEvent(QKeyEvent *e)
{
    switch(e->key()) {
    case Key_Up:
        view->scrollBy(0, -20);
        break;
    case Key_Down:
        view->scrollBy(0, 20);
        break;
    case Key_Left:
        setDate(date().addDays(-7));
        break;
    case Key_Right:
        setDate(date().addDays(7));
        break;
    default:
        e->ignore();
    }
}

void DateBookWeek::showDay( int day )
{
    QDate d=bdate;

    // Calculate offset to first day of week.
    int dayoffset=d.dayOfWeek() % 7;

    if(bStartOnMonday) dayoffset--;

    day--;
    d=d.addDays(day-dayoffset);
    emit showDate( d.year(), d.month(), d.day() );
}

void DateBookWeek::setDate( int y, int m, int d )
{
    setDate(QDate(y, m, d));
}

void DateBookWeek::setDate(QDate newdate)
{
    bdate=newdate;
    dow = newdate.dayOfWeek();
    header->setDate( newdate );
}

void DateBookWeek::dateChanged( QDate &newdate )
{
    bdate=newdate;
    getEvents();
}

QDate DateBookWeek::date() const
{
    return bdate;
}

void DateBookWeek::getEvents()
{
    QDate startWeek = weekDate();

    QDate endWeek = startWeek.addDays( 6 );
    QValueList<EffectiveEvent> eventList = db->getEffectiveEvents(startWeek, endWeek);
    view->showEvents( eventList );
    view->moveToHour( startTime );
}

void DateBookWeek::generateAllDayTooltext( QString& text ) {
    text += "<b>" + tr("This is an all day event.") + "</b><br>";
}

void DateBookWeek::generateNormalTooltext( QString& str, const EffectiveEvent &ev ) {
    str += "<b>" + QObject::tr("Start") + "</b>: ";
    str += TimeString::timeString( ev.event().start().time(), ampm, FALSE );
    if( ev.startDate()!=ev.endDate() ) {
        str += " <i>" + TimeString::longDateString( ev.startDate() )+"</i>";
    }
    str += "<br>";
    str += "<b>" + QObject::tr("End") + "</b>: ";
    str += TimeString::timeString( ev.event().end().time(), ampm, FALSE );
    if( ev.startDate()!=ev.endDate() ) {
        str += " <i>" + TimeString::longDateString( ev.endDate() ) + "</i>";
    }
}

void DateBookWeek::slotShowEvent( const EffectiveEvent &ev )
{
    if ( tHide->isActive() )
        tHide->stop();

    // why would someone use "<"?  Oh well, fix it up...
    // I wonder what other things may be messed up...
    QString strDesc = ev.description();
    int where = strDesc.find( "<" );
    while ( where != -1 ) {
    strDesc.remove( where, 1 );
    strDesc.insert( where, "&#60;" );
    where = strDesc.find( "<", where );
    }

    QString strCat;
    // ### FIX later...
//     QString strCat = ev.category();
//     where = strCat.find( "<" );
//     while ( where != -1 ) {
//  strCat.remove( where, 1 );
//  strCat.insert( where, "&#60;" );
//  where = strCat.find( "<", where );
//     }

    QString strLocation = ev.location();
    while ( where != -1 ) {
    strLocation.remove( where, 1 );
    strLocation.insert( where, "&#60;" );
    where = strLocation.find( "<", where );
    }

    QString strNote = ev.notes();
    where = strNote.find( "<" );
    while ( where != -1 ) {
    strNote.remove( where, 1 );
    strNote.insert( where, "&#60;" );
    where = strNote.find( "<", where );
    }

    QString str = "<b>" + strDesc + "</b><br>"
                  + strLocation + "<br>"
                  + "<i>" + strCat + "</i>"
                  + "<br>" + TimeString::longDateString( ev.date() )
              + "<br>";

    if (ev.event().type() == Event::Normal )
        generateNormalTooltext( str, ev );
    else
        generateAllDayTooltext( str );

    str += "<br><br>" + strNote;

    lblDesc->setText( str );
    lblDesc->resize( lblDesc->sizeHint() );
    // move the label so it is "centerd" horizontally...
    lblDesc->move( QMAX(0,(width() - lblDesc->width()) / 2), 0 );
    lblDesc->show();
}

void DateBookWeek::slotHideEvent()
{
    tHide->start( 2000, true );
}

void DateBookWeek::setStartViewTime( int startHere )
{
    startTime = startHere;
    view->moveToHour( startTime );
}

int DateBookWeek::startViewTime() const
{
    return startTime;
}

void DateBookWeek::redraw()
{
    getEvents();
}

void DateBookWeek::slotYearChanged( int y )
{
    int totWeek;
    QDate d( y, 12, 31 );
    int throwAway;
    calcWeek( d, totWeek, throwAway, bStartOnMonday );
    while ( totWeek == 1 ) {
        d = d.addDays( -1 );
        calcWeek( d, totWeek, throwAway, bStartOnMonday );
    }
}

void DateBookWeek::slotWeekChanged( bool onMonday )
{
    bStartOnMonday = onMonday;
    view->setStartOfWeek( bStartOnMonday );
    header->setStartOfWeek( bStartOnMonday );
    redraw();
}

void DateBookWeek::slotClockChanged( bool ap )
{
    ampm = ap;
}

// return the date at the beginning of the week...
QDate DateBookWeek::weekDate() const
{
    QDate d=bdate;

    // Calculate offset to first day of week.
    int dayoffset=d.dayOfWeek();
    if(bStartOnMonday) dayoffset--;
    else if( dayoffset == 7 )
        dayoffset = 0;

    return d.addDays(-dayoffset);
}

// This used to only be needed by datebook.cpp, but now we need it
// inside week view since we need to be able to figure out our total
// number of weeks on the fly. This is probably the best place to put
// it.

// For Weeks that start on Monday... (EASY!)
// At the moment we will use the ISO 8601 method for computing the week.
// Granted, other countries use other methods, but we aren't doing any
// Locale stuff at the moment. Without locale info it is too hard to
// guess what method should be used. So, this should pass. This
// algorithim is public domain and available at:
// http://personal.ecu.edu/mccartyr/ISOwdALG.txt
// the week number is returned, and the year number is returned in year
// for Instance 2001/12/31 is actually the first week in 2002.
// There is a more mathematical definition, it should be implemented.

// For Weeks that start on Sunday... (ahh... home rolled)
// okay, if Jan 1 is on Friday or Saturday, it will go to the previous
// week.

/*!
 * The week number and year number of a date may actually be different
 * then the date itself as specified by the ISO 8601 standard. For
 * example if Jan 1 year falls on a Friday, the week number will
 * be the last week number for the previous year (52 of year - 1). Another
 * example is that the date 2001-12-31 falls on a Monday, its week number
 * is 1 and its year is 2002. This function provides stripped down
 * version of the alogrithm described in
 * http://personal.ecu.edu/mccartyr/ISOwdALG.txt to provide the correct
 * week number and year number for a given date.
 *
 * \param d The date that you want to know the week number for.
 * \param week A reference to the variable you want to store the week
 *        number in.
 * \param year A reference to the variable you want to store the year
 *        number in.
 * \param startOnMonday Inform the function whether weeks start on
 *        Sunday or Monday. Set to true if your weeks start on Monday.
 * \return false if the supplied date is invalid, true otherwise.
 */
bool calcWeek( const QDate &d, int &week, int &year,
           bool startOnMonday )
{
    int weekNumber = -1;
    int yearNumber = -1;

    int jan1WeekDay = QDate(d.year(), 1, 1).dayOfWeek();
    int dayOfWeek = d.dayOfWeek();

    if ( !d.isValid() )
        return false;

    if ( startOnMonday ) {
        // find the Jan1Weekday;
        if ( d.dayOfYear() <= ( 8 - jan1WeekDay) && jan1WeekDay > 4 ) {
            yearNumber = d.year() - 1;
            if ( jan1WeekDay == 5
                 || ( jan1WeekDay == 6 && QDate::leapYear(yearNumber) ) )
                weekNumber = 53;
            else
                weekNumber = 52;
        } else
            yearNumber = d.year();

        if ( yearNumber == d.year() ) {
            int totalDays = 365;
            if ( QDate::leapYear(yearNumber) )
                totalDays++;
            if ( ((totalDays - d.dayOfYear()) < (4 - dayOfWeek) )
                 || (jan1WeekDay == 7) && (totalDays - d.dayOfYear()) < 3)
            {
                yearNumber++;
                weekNumber = 1;
            }
        }
        if ( yearNumber == d.year() ) {
            int j = d.dayOfYear() + (7 - dayOfWeek) + ( jan1WeekDay - 1 );
            weekNumber = j / 7;
            if ( jan1WeekDay > 4 )
                weekNumber--;
        }
    } else {
        // it's better to keep these cases separate...
        if ( d.dayOfYear() <= (7 - jan1WeekDay) && jan1WeekDay > 4
             && jan1WeekDay != 7 )
        {
            yearNumber = d.year() - 1;
            if ( jan1WeekDay == 6
                 || (jan1WeekDay == 7 && QDate::leapYear(yearNumber) ) )
            {
                weekNumber = 53;
            } else
                weekNumber = 52;
        } else
            yearNumber = d.year();

        if ( yearNumber == d.year() ) {
            int totalDays = 365;
            if ( QDate::leapYear( yearNumber ) )
                totalDays++;
            if ( ((totalDays - d.dayOfYear()) < (4 - dayOfWeek % 7)) ) {
                yearNumber++;
                weekNumber = 1;
            }
        }
        if ( yearNumber == d.year() ) {
            int j = d.dayOfYear() + (7 - dayOfWeek % 7) + ( jan1WeekDay - 1 );
            weekNumber = j / 7;

            if ( jan1WeekDay > 4 )
                weekNumber--;
        }
    }
    year = yearNumber;
    week = weekNumber;
    return true;
}

