#include <qapplication.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qspinbox.h>

#include <qpe/timestring.h>

#include <opie2/osortweekdaybuttons.h>

#include "opimrecurrencewidget.h"


using namespace Opie;
// Global Templates for use in setting up the repeat label...
// the problem is these strings get initialized before QPEApplication can install the translator -zecke
namespace {
QString strDayTemplate;
QString strYearTemplate;
QString strMonthDateTemplate;
QString strMonthDayTemplate;
QString strWeekTemplate;
QString dayLabel[7];
}

/*
 * static linkage to not polute the symbol table...
 * The problem is that const and static linkage are resolved prior to installing a translator
 * leading to that the above strings are translted but to the original we delay the init of these strings...
 * -zecke
 */
static void fillStrings() {
    strDayTemplate = QObject::tr("Every");
    strYearTemplate = QObject::tr("%1 %2 every ");
    strMonthDateTemplate = QObject::tr("The %1 every ");
    strMonthDayTemplate = QObject::tr("The %1 %2 of every");
    strWeekTemplate = QObject::tr("Every ");
    dayLabel[0] = QObject::tr("Monday");
    dayLabel[1] = QObject::tr("Tuesday");
    dayLabel[2] = QObject::tr("Wednesday");
    dayLabel[3] = QObject::tr("Thursday");
    dayLabel[4] = QObject::tr("Friday");
    dayLabel[5] = QObject::tr("Saturday");
    dayLabel[6] = QObject::tr("Sunday");
}

static QString numberPlacing( int x );	// return the proper word format for
                                        // x (1st, 2nd, etc)
static int week( const QDate &dt );    // what week in the month is dt?

/**
 * Constructs the Widget
 * @param startOnMonday Does the week start on monday
 * @param newStart The start date of the recurrence
 * @param parent The parent widget
 * @param name the name of object
 * @param modal if the dialog should be modal
 * @param fl Additional window flags
 */
OPimRecurrenceWidget::OPimRecurrenceWidget( bool startOnMonday,
                                      const QDate& newStart,
                                      QWidget* parent,
                                      const char* name,
                                      bool modal,
                                      WFlags fl )
    : OPimRecurrenceBase( parent, name, modal, fl ),
      start( newStart ),
      currInterval( None ),
      startWeekOnMonday( startOnMonday )
{
     if (strDayTemplate.isEmpty() )
        fillStrings();

    init();
    fraType->setButton( currInterval );
    chkNoEnd->setChecked( TRUE );
    setupNone();
}

/**
 * Different constructor
 * @param startOnMonday Does the week start on monday?
 * @param rp Already set OPimRecurrence object
 * @param startDate The start date
 * @param parent The parent widget
 * @param name The name of the object
 * @param modal
 * @param fl The flags for window
 */
OPimRecurrenceWidget::OPimRecurrenceWidget( bool startOnMonday,
                                            const Opie::OPimRecurrence& rp, const QDate& startDate,
                                      QWidget* parent, const char* name,
                                      bool modal, WFlags fl)
    : OPimRecurrenceBase( parent, name, modal, fl ),
      start( startDate ),
      end( rp.endDate() ),
      startWeekOnMonday( startOnMonday )
{
     if (strDayTemplate.isEmpty() )
        fillStrings();
    // do some stuff with the repeat pattern
    init();
    setRecurrence( rp );
}

OPimRecurrenceWidget::~OPimRecurrenceWidget() {
}

/**
 * set the start date
 * @param date the new start date
 */
void OPimRecurrenceWidget::setStartDate( const QDate& date ) {
    setRecurrence( recurrence(), date );
}
/**
 * set the recurrence
 * @param rp  The OPimRecurrence object with the new recurrence rules
 */
void OPimRecurrenceWidget::setRecurrence( const Opie::OPimRecurrence& rp ) {
    setRecurrence( rp, start );
}

/**
 * overloaded method taking OPimRecurrence and a new start date
 * @param rp Recurrence rule
 * @param date The new start date
 */
void OPimRecurrenceWidget::setRecurrence( const Opie::OPimRecurrence& rp, const QDate& date ) {
    start = date;
    end = rp.endDate();
    switch ( rp.type() ) {
    default:
    case OPimRecurrence::NoRepeat:
        currInterval = None;
        setupNone();
        break;
    case OPimRecurrence::Daily:
        currInterval = Day;
        setupDaily();
        break;
    case OPimRecurrence::Weekly:
        currInterval = Week;
        setupWeekly();
        sortWeekdayButtons( rp.days(), startWeekOnMonday, fraExtra );
        slotWeekLabel();
        break;
    case OPimRecurrence::MonthlyDay:
        currInterval = Month;
        setupMonthly();
        fraExtra->setButton( 0 );
        slotMonthLabel( 0 );
        break;
    case OPimRecurrence::MonthlyDate:
        currInterval = Month;
        setupMonthly();
        fraExtra->setButton( 1 );
        slotMonthLabel( 1 );
        break;
    case OPimRecurrence::Yearly:
        currInterval = Year;
        setupYearly();
        break;
    }
    fraType->setButton( currInterval );
    spinFreq->setValue( rp.frequency() );
    if ( !rp.hasEndDate() ) {
	cmdEnd->setText( tr("No End Date") );
	chkNoEnd->setChecked( TRUE );
    } else
	cmdEnd->setText( TimeString::shortDate( end ) );
}

/**
 * the user selected recurrence rule.
 * @return The recurrence rule.
 */
OPimRecurrence OPimRecurrenceWidget::recurrence()const {
    QListIterator<QToolButton> it( listRTypeButtons );
    QListIterator<QToolButton> itExtra( listExtra );
    OPimRecurrence rpTmp;
    int i;
    for ( i = 0; *it; ++it, i++ ) {
	if ( (*it)->isOn() ) {
	    switch ( i ) {
            case None:
                rpTmp.setType( OPimRecurrence::NoRepeat );
                break;
            case Day:
                rpTmp.setType( OPimRecurrence::Daily );
                break;
            case Week:{
                rpTmp.setType( OPimRecurrence::Weekly );
                int day;
                int day2 = 0;
                for ( day = 1; *itExtra; ++itExtra, day = day << 1 ) {
                    if ( (*itExtra)->isOn() ) {
                        if ( startWeekOnMonday )
                            day2 |= day;
                        else {
                            if ( day == 1 )
                                day2 |= Event::SUN;
                            else
                                day2 |= day >> 1;
                        }
                    }
                }
                rpTmp.setDays( day2 );
            }
                break;
            case Month:
                if ( cmdExtra1->isOn() )
                    rpTmp.setType( OPimRecurrence::MonthlyDay );
                else if ( cmdExtra2->isOn() )
                    rpTmp.setType( OPimRecurrence::MonthlyDate );
                // figure out the montly day...
                rpTmp.setPosition(  week( start ) );
                break;
            case Year:
                rpTmp.setType( OPimRecurrence::Yearly );
                break;
	    }
	    break;  // no need to keep looking!
	}
    }
    rpTmp.setFrequency(spinFreq->value() );
    rpTmp.setHasEndDate( !chkNoEnd->isChecked() );
    if ( rpTmp.hasEndDate() ) {
	rpTmp.setEndDate( end );
    }
    // timestamp it...
//    rpTmp.setCreateTime(  ); current DateTime is already set -zecke
    return rpTmp;
}

/**
 * Return the end date of the recurrence. This is only
 * valid if the recurrence rule does contain an enddate
 */
QDate OPimRecurrenceWidget::endDate()const {
    return end;
}
void OPimRecurrenceWidget::slotSetRType(int rtype) {
   // now call the right function based on the type...
    currInterval = static_cast<repeatButtons>(rtype);
    switch ( currInterval ) {
    case None:
        setupNone();
        break;
    case Day:
        setupDaily();
        break;
    case Week:
        setupWeekly();
        slotWeekLabel();
        break;
    case Month:
        setupMonthly();
        cmdExtra2->setOn( TRUE );
        slotMonthLabel( 1 );
        break;
    case Year:
        setupYearly();
        break;
    }
}
void OPimRecurrenceWidget::endDateChanged(int y, int m, int d) {
    end.setYMD( y, m, d );
    if ( end < start )
	end = start;
    cmdEnd->setText(  TimeString::shortDate( end ) );
    repeatPicker->setDate( end.year(), end.month(), end.day() );
}
void OPimRecurrenceWidget::slotNoEnd( bool unused) {
    // if the item was toggled, then go ahead and set it to the maximum date
    if ( unused ) {
	end.setYMD( 3000, 12, 31 );
	cmdEnd->setText( tr("No End Date") );
    } else {
	end = start;
	cmdEnd->setText( TimeString::shortDate(end) );
    }
}
void OPimRecurrenceWidget::setupRepeatLabel( const QString& s) {
    lblVar1->setText( s );
}
void OPimRecurrenceWidget::setupRepeatLabel( int x) {
    // change the spelling based on the value of x
    QString strVar2;

    if ( x > 1 )
	lblVar1->show();
    else
	lblVar1->hide();

    switch ( currInterval ) {
	case None:
	    break;
	case Day:
	    if ( x > 1 )
		strVar2 = tr( "days" );
	    else
		strVar2 = tr( "day" );
	    break;
	case Week:
	    if ( x > 1 )
		strVar2 = tr( "weeks" );
	    else
		strVar2 = tr( "week" );
	    break;
	case Month:
	    if ( x > 1 )
		strVar2 = tr( "months" );
	    else
		strVar2 = tr( "month" );
	    break;
	case Year:
	    if ( x > 1 )
		strVar2 = tr( "years" );
	    else
		strVar2 = tr( "year" );
	    break;
    }
    if ( !strVar2.isNull() )
	lblVar2->setText( strVar2 );
}
void OPimRecurrenceWidget::slotWeekLabel() {
    QString str;
    QListIterator<QToolButton> it( listExtra );
    unsigned int i;
    unsigned int keepMe;
    bool bNeedCarriage = FALSE;
    // don't do something we'll regret!!!
    if ( currInterval != Week )
	return;

    if ( startWeekOnMonday )
	keepMe = start.dayOfWeek() - 1;
    else
	keepMe = start.dayOfWeek() % 7;

    QStringList list;
    for ( i = 0; *it; ++it, i++ ) {
	// a crazy check, if you are repeating weekly, the current day
	// must be selected!!!
	if ( i == keepMe && !( (*it)->isOn() ) )
	    (*it)->setOn( TRUE );
	if ( (*it)->isOn() ) {
	    if ( startWeekOnMonday )
		list.append( dayLabel[i] );
	    else {
		if ( i == 0 )
		    list.append( dayLabel[6] );
		else
		    list.append( dayLabel[i - 1] );
	    }
	}
    }
    QStringList::Iterator itStr;
    for ( i = 0, itStr = list.begin(); itStr != list.end(); ++itStr, i++ ) {
	if ( i == 3 )
	    bNeedCarriage = TRUE;
	else
	    bNeedCarriage = FALSE;
	if ( str.isNull() )
	    str = *itStr;
	else if ( i == list.count() - 1 ) {
	    if ( i < 2 )
		str += tr(" and ") + *itStr;
	    else {
		if ( bNeedCarriage )
		    str += tr( ",\nand " ) + *itStr;
		else
		    str += tr( ", and " ) + *itStr;
	    }
	} else {
	    if ( bNeedCarriage )
		str += ",\n" + *itStr;
	    else
		str += ", " + *itStr;
	}
    }
    str = str.prepend( tr("on ") );

    lblWeekVar->setText( str );
}
void OPimRecurrenceWidget::slotMonthLabel(int type) {
    QString str;
    if ( currInterval != Month || type > 1 )
	return;
    if ( type == 1 )
		str = strMonthDateTemplate.arg( numberPlacing(start.day()) );
    else
		str = strMonthDayTemplate.arg( numberPlacing(week(start)))
			  .arg( dayLabel[start.dayOfWeek() - 1] );
    lblRepeat->setText( str );
}
void OPimRecurrenceWidget::slotChangeStartOfWeek( bool onMonday ) {
 startWeekOnMonday = onMonday;
    // we need to make this as unintrusive as possible...
    int saveSpin = spinFreq->value();
    char days = 0;
    int day;
    QListIterator<QToolButton> itExtra( listExtra );
    for ( day = 1; *itExtra; ++itExtra, day = day << 1 ) {
	if ( (*itExtra)->isOn() ) {
	    if ( !startWeekOnMonday )
		days |= day;
	    else {
		if ( day == 1 )
		    days |= OPimRecurrence::SUN;
		else
		    days |= day >> 1;
	    }
	}
    }
    setupWeekly();
    spinFreq->setValue( saveSpin );
    sortWeekdayButtons( days , startWeekOnMonday, fraExtra );
    slotWeekLabel();
}
void OPimRecurrenceWidget::setupNone() {
   lblRepeat->setText( tr("No Repeat") );
    lblVar1->hide();
    lblVar2->hide();
    hideExtras();
    cmdEnd->hide();
    lblFreq->hide();
    lblEvery->hide();
    lblFreq->hide();
    spinFreq->hide();
    lblEnd->hide();
    lblWeekVar->hide();
}
void OPimRecurrenceWidget::setupDaily() {
  hideExtras();
    lblWeekVar->hide();
    spinFreq->setValue( 1 );
    lblFreq->setText( tr("day(s)") );
    lblVar2->show();
    showRepeatStuff();
    lblRepeat->setText( strDayTemplate );
    setupRepeatLabel( 1 );
}
void OPimRecurrenceWidget::setupWeekly() {
// reshow the buttons...
    fraExtra->setTitle( tr("Repeat On") );
    fraExtra->setExclusive( FALSE );
    fraExtra->show();
    if ( startWeekOnMonday ) {
	cmdExtra1->setText( tr("Mon") );
	cmdExtra2->setText( tr("Tue") );
	cmdExtra3->setText( tr("Wed") );
	cmdExtra4->setText( tr("Thu") );
	cmdExtra5->setText( tr("Fri") );
	cmdExtra6->setText( tr("Sat") );
	cmdExtra7->setText( tr("Sun") );
    } else {
	cmdExtra1->setText( tr("Sun") );
	cmdExtra2->setText( tr("Mon") );
	cmdExtra3->setText( tr("Tue") );
	cmdExtra4->setText( tr("Wed") );
	cmdExtra5->setText( tr("Thu") );
	cmdExtra6->setText( tr("Fri") );
	cmdExtra7->setText( tr("Sat") );
    }
    // I hope clustering these improve performance....
    cmdExtra1->setOn( FALSE );
    cmdExtra2->setOn( FALSE );
    cmdExtra3->setOn( FALSE );
    cmdExtra4->setOn( FALSE );
    cmdExtra5->setOn( FALSE );
    cmdExtra6->setOn( FALSE );
    cmdExtra7->setOn( FALSE );

    cmdExtra1->show();
    cmdExtra2->show();
    cmdExtra3->show();
    cmdExtra4->show();
    cmdExtra5->show();
    cmdExtra6->show();
    cmdExtra7->show();

    lblWeekVar->show();
    spinFreq->setValue( 1 );
    // might as well set the day too...
    if ( startWeekOnMonday ) {
	fraExtra->setButton( start.dayOfWeek() - 1 );
    } else {
	fraExtra->setButton( start.dayOfWeek() % 7 );
    }
    lblFreq->setText( tr("week(s)") );
    lblVar2->show();
    showRepeatStuff();
    setupRepeatLabel( 1 );
}
void OPimRecurrenceWidget::setupMonthly() {
    hideExtras();
    lblWeekVar->hide();
    fraExtra->setTitle( tr("Repeat By") );
    fraExtra->setExclusive( TRUE );
    fraExtra->show();
    cmdExtra1->setText( tr("Day") );
    cmdExtra1->show();
    cmdExtra2->setText( tr("Date") );
    cmdExtra2->show();
    spinFreq->setValue( 1 );
    lblFreq->setText( tr("month(s)") );
    lblVar2->show();
    showRepeatStuff();
    setupRepeatLabel( 1 );
}
void OPimRecurrenceWidget::setupYearly() {
hideExtras();
    lblWeekVar->hide();
    spinFreq->setValue( 1 );
    lblFreq->setText( tr("year(s)") );
    lblFreq->show();
    lblFreq->show();
    showRepeatStuff();
    lblVar2->show();
    QString strEvery = strYearTemplate.arg( start.monthName(start.month()) ).arg( numberPlacing(start.day()) );
    lblRepeat->setText( strEvery );
    setupRepeatLabel( 1 );

}
void OPimRecurrenceWidget::init() {
 QPopupMenu *m1 = new QPopupMenu( this );
    repeatPicker = new DateBookMonth( m1, 0, TRUE );
    m1->insertItem( repeatPicker );
    cmdEnd->setPopup( m1 );
    cmdEnd->setPopupDelay( 0 );

    QObject::connect( repeatPicker, SIGNAL(dateClicked(int,int,int)),
                      this, SLOT(endDateChanged(int,int,int)) );
    QObject::connect( qApp, SIGNAL(weekChanged(bool)),
		      this, SLOT(slotChangeStartOfWeek(bool)) );

    listRTypeButtons.setAutoDelete( TRUE );
    listRTypeButtons.append( cmdNone );
    listRTypeButtons.append( cmdDay );
    listRTypeButtons.append( cmdWeek );
    listRTypeButtons.append( cmdMonth );
    listRTypeButtons.append( cmdYear );

    listExtra.setAutoDelete( TRUE );
    listExtra.append( cmdExtra1 );
    listExtra.append( cmdExtra2 );
    listExtra.append( cmdExtra3 );
    listExtra.append( cmdExtra4 );
    listExtra.append( cmdExtra5 );
    listExtra.append( cmdExtra6 );
    listExtra.append( cmdExtra7 );
}
void OPimRecurrenceWidget::hideExtras() {
      // hide the extra buttons...
    fraExtra->hide();
    chkNoEnd->hide();
    QListIterator<QToolButton> it( listExtra );
    for ( ; *it; ++it ) {
	(*it)->hide();
	(*it)->setOn( FALSE );
    }
}
void OPimRecurrenceWidget::showRepeatStuff() {
    cmdEnd->show();
    chkNoEnd->show();
    lblFreq->show();
    lblEvery->show();
    lblFreq->show();
    spinFreq->show();
    lblEnd->show();
    lblRepeat->setText( tr("Every") );
}


static int week( const QDate &start )
{
    // figure out the week...
    int stop = start.day(),
        sentinel = start.dayOfWeek(),
        dayOfWeek = QDate( start.year(), start.month(), 1 ).dayOfWeek(),
        week = 1,
	i;
    for ( i = 1; i < stop; i++ ) {
	if ( dayOfWeek++ == sentinel )
	    week++;
	if ( dayOfWeek > 7 )
	    dayOfWeek = 0;
    }
    return week;
}

static QString numberPlacing( int x )
{
    // I hope this works in other languages besides english...
    QString str = QString::number( x );
    switch ( x % 10 ) {
	case 1:
	    str += QWidget::tr( "st" );
	    break;
	case 2:
	    str += QWidget::tr( "nd" );
	    break;
	case 3:
	    str += QWidget::tr( "rd" );
	    break;
	default:
	    str += QWidget::tr( "th" );
	    break;
    }
    return str;
}
