#include <rohfeedback.h>


#include <stdio.h>
#include <qpeapplication.h>
#include <qevent.h>
#include <resource.h>
#include <qpixmap.h>
#include <qbitmap.h>

#define SPEED 600
#define DELAY 500

namespace Opie {
namespace Internal {
/*

    RightOnHold feedback

*/

QPixmap * RoHFeedback::Imgs[NOOFICONS] = { 0, 0, 0, 0, 0 };
QBitmap * RoHFeedback::Masks[NOOFICONS];
int RoHFeedback::IconWidth;
int RoHFeedback::IconHeight;

RoHFeedback::RoHFeedback() :
                        QLabel( 0, 0, Qt::WType_Popup ), Timer() {

    Receiver = 0l;
    connect( &Timer, SIGNAL( timeout() ), this, SLOT( iconShow() ) );

    if( Imgs[0] == 0 ) {
      QString S;


      for( int i = 0; i < NOOFICONS ; i ++ ) {
        Imgs[i] =  new QPixmap( Resource::loadPixmap("RoH/star/"+
						     QString::number(i+1) +
						     ".png" ));
        Masks[i] = new QBitmap();
        (*Masks[i]) = Resource::loadPixmap("RoH/star/"+QString::number(i+1) +
                                        ".png" );
      }
    }

    IconWidth = Imgs[0]->size().width();
    IconHeight = Imgs[0]->size().height();

    resize( IconWidth, IconHeight );
}

int RoHFeedback::delay( void ) {
    return DELAY+SPEED+50;
}

RoHFeedback::~RoHFeedback() {
    for ( int i = 0; i < NOOFICONS; ++i ) {
        delete Imgs [i];
        delete Masks[i];
    }
}

void RoHFeedback::init( const QPoint & P, QWidget* wid ) {
    if( ! IconWidth )
      return;

    Receiver =  wid;
    IconNr = -1;
    move( P.x()-IconWidth/2, P.y() - IconHeight/2 );
    // to initialize
    Timer.start( DELAY - SPEED/NOOFICONS );
}

void RoHFeedback::stop( void ) {
    IconNr = -2; // stop
    hide();
    Timer.stop();
}

bool RoHFeedback::event( QEvent * E ) {

    if( E->type() >= QEvent::MouseButtonPress &&
        E->type() <= QEvent::MouseMove ) {
      // pass the event to the receiver with translated coord
      QMouseEvent QME( ((QMouseEvent *)E)->type(),
                       Receiver->mapFromGlobal(
                          ((QMouseEvent *)E)->globalPos() ),
                       ((QMouseEvent *)E)->globalPos(),
                       ((QMouseEvent *)E)->button(),
                       ((QMouseEvent *)E)->state()
                   );
      return QPEApplication::sendEvent( Receiver, &QME );
    }

    // first let the label treat the event
    return QLabel::event( E );
}

void RoHFeedback::iconShow( void ) {
    switch( IconNr ) {
      case FeedbackTimerStart:
        IconNr = 0;
        Timer.start( SPEED/NOOFICONS );
        break;
      case FeedbackStopped:
        // stopped
        IconNr = FeedbackTimerStart;
        hide();
        break;
      case FeedbackShow: // first
        show();
        // FT
      default :
        // show

        setPixmap( *(Imgs[IconNr]) );
        setMask( *(Masks[IconNr]) );
        IconNr = (IconNr+1)%NOOFICONS; // rotate
        break;
    }
}

}
}