#include "datebookweeklstevent.h"
#include "datebooktypes.h"

#include <opie2/odebug.h>

#include <qpe/ir.h>

#include <qstring.h>
#include <qpopupmenu.h>

DateBookWeekLstEvent::DateBookWeekLstEvent(bool ap, const EffectiveEvent &ev,
                        int weeklistviewconfig,
                        QWidget* parent,
                        const char* name,
                        WFlags fl ) : OClickableLabel(parent,name,fl), event(ev), ampm(ap)
{
    // old values... lastday = "__|__", middle="   |---", Firstday="00:00",
    QString s,start,middle,end,day;

    odebug << "weeklistviewconfig=" << weeklistviewconfig << oendl;
    if(weeklistviewconfig==NORMAL) { // "Normal", only display start time.
        if ( ampm ) {
            int shour = ev.start().hour();
            int smin = ev.start().minute();
            if ( shour >= 12 ) {
                if ( shour > 12 ) {
                    shour -= 12;
                }
                start.sprintf( "%.2d:%.2d PM", shour, smin );
                day.sprintf("%.2d:%.2d PM",shour,smin);
            } else {
                if ( shour == 0 ) {
                    shour = 12;
                }
             start.sprintf( "%.2d:%.2d AM", shour, smin );
             day.sprintf("%.2d:%.2d AM",shour,smin);
            }
        } else {
        start.sprintf("%.2d:%.2d",ev.start().hour(),ev.start().minute());
            day.sprintf("%.2d:%.2d",ev.start().hour(),ev.start().minute());
        }
        middle.sprintf("   |---");
        end.sprintf("__|__");
    } else if(weeklistviewconfig==EXTENDED) { // Extended mode, display start and end times.
        start.sprintf("%.2d:%.2d-",ev.start().hour(),ev.start().minute());
        middle.sprintf("<--->");
        end.sprintf("-%.2d:%.2d",ev.end().hour(),ev.end().minute());
        day.sprintf("%.2d:%.2d-%.2d:%.2d",ev.start().hour(),ev.start().minute(),ev.end().hour(),ev.end().minute());
    }

    if(ev.event().type() == Event::Normal) {
        if(ev.startDate()==ev.date() && ev.endDate()==ev.date()) {  // day event.
            s=day;
        } else if(ev.startDate()==ev.date()) {  // start event.
            s=start;
        } else if(ev.endDate()==ev.date()) { // end event.
            s=end;
        } else {    // middle day.
            s=middle;
        }
    } else {
        s="";
    }
    setText(QString(s) + " " + ev.description());
//  connect(this, SIGNAL(clicked()), this, SLOT(editMe()));
    setAlignment( int( QLabel::WordBreak | QLabel::AlignLeft ) );
}
void DateBookWeekLstEvent::editMe() {
    emit editEvent(event.event());
}
void DateBookWeekLstEvent::duplicateMe()
{
       emit duplicateEvent(event.event());
}
void DateBookWeekLstEvent::deleteMe()
{
       emit removeEvent(event.event());
       emit redraw();
}
void DateBookWeekLstEvent::beamMe()
{
    emit beamEvent( event.event() );
}
void DateBookWeekLstEvent::mousePressEvent( QMouseEvent *e )
{
    if (!event.event().isValidUid()) {
        // this is just such a holiday event.
        return;
    }
    popmenue = new QPopupMenu;

    popmenue->insertItem( tr( "Edit" ), this, SLOT(editMe()));
    popmenue->insertItem( tr( "Duplicate" ), this, SLOT(duplicateMe()));
    popmenue->insertItem( tr( "Delete" ), this, SLOT(deleteMe()));
    if(Ir::supported())
        popmenue->insertItem( tr( "Beam" ), this, SLOT(beamMe()));
    popmenue->popup( mapToGlobal( e->pos() ));
}
