/****************************************************************************
** GPL by Rajko Albrecht
**
**
**
**
**
****************************************************************************/
#include "datebookdayallday.h"

#include <qlayout.h>
#include <qpe/ir.h>
#include <qpopupmenu.h>
#include <qtimer.h>

#include "datebookday.h"

/*
 *  Constructs a DatebookdayAllday which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
DatebookdayAllday::DatebookdayAllday(DateBookDB* db, QWidget* parent,  const char* name, WFlags  )
    : QScrollView( parent, name ),item_count(0),dateBook(db)
{
    if ( !name )
        setName( "DatebookdayAllday" );
    setMinimumSize( QSize( 0, 0 ) );
    setMaximumHeight(3* (QFontMetrics(font()).height()+4) );

    m_MainFrame = new QFrame(viewport());
    m_MainFrame->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
    setFrameStyle(QFrame::NoFrame|QFrame::Plain);
    //setResizePolicy( QScrollView::Default );
    setResizePolicy(QScrollView::AutoOneFit);
    setHScrollBarMode( AlwaysOff );
    addChild(m_MainFrame);

    datebookdayalldayLayout = new QVBoxLayout( m_MainFrame );
    datebookdayalldayLayout->setSpacing( 0 );
    datebookdayalldayLayout->setMargin( 0 );

    lblDesc = new DatebookEventDesc(parent->parentWidget(),"");
    lblDesc->setBackgroundColor(Qt::yellow);
    lblDesc->hide();
    subWidgets.setAutoDelete(true);
}

/*
 *  Destroys the object and frees any allocated resources
 */
DatebookdayAllday::~DatebookdayAllday()
{
    // no need to delete child widgets, Qt does it all for us
}

DatebookAlldayDisp* DatebookdayAllday::addEvent(const EffectiveEvent&ev)
{
    DatebookAlldayDisp * lb;
    lb = new DatebookAlldayDisp(dateBook,ev,m_MainFrame,NULL);
    lb->show();
    datebookdayalldayLayout->addWidget(lb);
    subWidgets.append(lb);

    connect(lb,SIGNAL(displayMe(const Event&)),lblDesc,SLOT(disp_event(const Event&)));
    ++item_count;

    return lb;
}

DatebookAlldayDisp* DatebookdayAllday::addHoliday(const QString&e)
{
    DatebookAlldayDisp * lb;
    lb = new DatebookAlldayDisp(e,m_MainFrame,NULL);
    lb->show();
    datebookdayalldayLayout->addWidget(lb);
    subWidgets.append(lb);

    connect(lb,SIGNAL(displayMe(const Event&)),lblDesc,SLOT(disp_event(const Event&)));
    ++item_count;

    return lb;
}

void DatebookdayAllday::removeAllEvents()
{
    subWidgets.clear();
    item_count = 0;
}

DatebookAlldayDisp::DatebookAlldayDisp(DateBookDB *db,const EffectiveEvent& ev,
                                       QWidget* parent,const char* name,WFlags f)
    : QLabel(parent,name,f),m_Ev(ev),dateBook(db)
{
    QString strDesc = m_Ev.description();
    strDesc = strDesc.replace(QRegExp("<"),"&#60;");
    setBackgroundColor(yellow);
    setText(strDesc);
    setFrameStyle(QFrame::Raised|QFrame::Panel);

    int s = QFontMetrics(font()).height()+4;
    setMaximumHeight( s );
    setMinimumSize( QSize( 0, s ) );
    m_holiday = false;
}

DatebookAlldayDisp::DatebookAlldayDisp(const QString&aholiday,QWidget* parent,const char* name, WFlags fl)
    : QLabel(parent,name,fl),m_Ev(),dateBook(0)
{
    QString strDesc = aholiday;
    strDesc = strDesc.replace(QRegExp("<"),"&#60;");
    Event ev;
    ev.setDescription(strDesc);
    ev.setAllDay(true);
    m_Ev.setEvent(ev);
    setText(strDesc);

    setAlignment(AlignHCenter);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum));

    //setFrameStyle(QFrame::Raised|QFrame::Panel);
    //setBackgroundColor(yellow);

    int s = QFontMetrics(font()).height()+4;
    setMaximumHeight( s );
    setMinimumSize( QSize( 0, s ) );

    m_holiday = true;
}

DatebookAlldayDisp::~DatebookAlldayDisp()
{
}

void DatebookAlldayDisp::beam_single_event()
{
    if (m_holiday) return;
    // create an Event and beam it...
    /*
     * Start with the easy stuff. If start and  end date is the same we can just use
     * the values of effective m_Events
     * If it is a multi day m_Event we need to find the real start and end date...
     */
    if ( m_Ev.event().start().date() == m_Ev.event().end().date() ) {
        Event m_Event( m_Ev.event() );

        QDateTime dt( m_Ev.date(), m_Ev.start() );
        m_Event.setStart( dt );

        dt.setTime( m_Ev.end() );
        m_Event.setEnd( dt );
        emit beamMe( m_Event );
    }else {
        /*
         * at least the the Times are right now
         */
        QDateTime start( m_Ev.event().start() );
        QDateTime end  ( m_Ev.event().end  () );

        /*
         * ok we know the start date or we need to find it
         */
        if ( m_Ev.start() != QTime( 0, 0, 0 ) ) {
            start.setDate( m_Ev.date() );
        }else {
            QDate dt = DateBookDay::findRealStart( m_Ev.event().uid(), m_Ev.date(), dateBook );
            start.setDate( dt );
        }

        /*
         * ok we know now the end date...
         * else
         *   get to know the offset btw the real start and real end
         *   and then add it to the new start date...
         */
        if ( m_Ev.end() != QTime(23, 59, 59 ) ) {
            end.setDate( m_Ev.date() );
        }else{
            int days = m_Ev.event().start().date().daysTo( m_Ev.event().end().date() );
            end.setDate( start.date().addDays( days ) );
        }
        Event m_Event( m_Ev.event() );
        m_Event.setStart( start );
        m_Event.setEnd  ( end   );
        emit beamMe( m_Event );
    }
}

void DatebookAlldayDisp::mousePressEvent(QMouseEvent*e)
{
    Event ev = m_Ev.event();
    QColor b = backgroundColor();
    setBackgroundColor(green);
    update();
    QPopupMenu m;
    if (!m_holiday) {
        m.insertItem( DateBookDayWidget::tr( "Edit" ), 1 );
        m.insertItem( DateBookDayWidget::tr( "Duplicate" ), 4 );
        m.insertItem( DateBookDayWidget::tr( "Delete" ), 2 );
        if(Ir::supported()) m.insertItem( DateBookDayWidget::tr( "Beam" ), 3 );
        if(Ir::supported() && m_Ev.event().doRepeat() ) m.insertItem( DateBookDayWidget::tr( "Beam this occurence"), 5 );
    }
    m.insertItem( tr( "Info"),6);
    int r = m.exec( e->globalPos() );
    setBackgroundColor(b);
    update();
    switch (r) {
    case 1:
        emit editMe( ev );
        break;
    case 2:
        emit deleteMe( ev );
        break;
    case 3:
        emit beamMe( ev );
        break;
    case 4:
        emit duplicateMe( ev );
        break;
    case 5:
        beam_single_event();
        break;
    case 6:
        emit displayMe( ev );
        break;
    default:
        break;
    }
}

DatebookEventDesc::DatebookEventDesc(QWidget*parent,const char*name)
    :QLabel(parent,name)
{
   m_Timer=new QTimer(this);
   connect(m_Timer,SIGNAL(timeout()),this,SLOT(hide()));
   setFrameStyle(QFrame::Sunken|QFrame::Panel);
   setTextFormat(RichText);
}

DatebookEventDesc::~DatebookEventDesc()
{
}

void DatebookEventDesc::mousePressEvent(QMouseEvent*)
{
    hide();
    if (m_Timer->isActive()) m_Timer->stop();
}

void DatebookEventDesc::disp_event(const Event&e)
{
    if (m_Timer->isActive()) m_Timer->stop();
    QString text;
    text = "<b><i>"+e.description()+"</i></b><br>";
    if (e.notes().length()>0) {
        text+="<b>"+e.notes()+"</b><br>";
    }
    if (e.location().length()>0) {
        text+="<i>"+e.location()+"</i><br>";
    }
    text = text.replace(QRegExp("\n"),"<br>");
    setText(text);
    QSize s = sizeHint();
    s+=QSize(10,10);
    resize(s);
    move( QMAX(0,(parentWidget()->width()-width()) / 2),
            (parentWidget()->height()-height())/2 );
    show();
    m_Timer->start(2000,true);
}
