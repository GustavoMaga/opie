
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include <qwidget.h>
#include <qpixmap.h>
#include <qbutton.h>
#include <qpainter.h>
#include <qframe.h>
#include <qlayout.h>

#include <stdlib.h>
#include <stdio.h>

#include "audiowidget.h"
#include "mediaplayerstate.h"

extern MediaPlayerState *mediaPlayerState;

static const int xo = -2; // movable x offset
static const int yo = 22; // movable y offset


Ticker::Ticker( QWidget* parent=0 ) : QFrame( parent ) {
        setFrameStyle( WinPanel | Sunken );
        setText( "No Song" );
}

Ticker::~Ticker() {
}

void Ticker::setText( const QString& text ) {
    pos = 0; // reset it everytime the text is changed
    scrollText = text;
    pixelLen = fontMetrics().width( scrollText );
    killTimers();
    if ( pixelLen > width() ) {
        startTimer( 50 );
    }
    update();
}


void Ticker::timerEvent( QTimerEvent * ) {
    pos = ( pos + 1 > pixelLen ) ? 0 : pos + 1;
    scroll( -1, 0, contentsRect() );
    repaint( FALSE );
}

void Ticker::drawContents( QPainter *p ) {
  for ( int i = 0; i - pos < width() && (i < 1 || pixelLen > width()); i += pixelLen )
      p->drawText( i - pos, 0, INT_MAX, height(), AlignVCenter, scrollText );
    QPixmap pm( width(), height() );
    pm.fill( colorGroup().base() );
    QPainter pmp( &pm );
    for ( int i = 0; i - pos < width() && (i < 1 || pixelLen > width()); i += pixelLen ) {
        pmp.drawText( i - pos, 0, INT_MAX, height(), AlignVCenter, scrollText );
    }
    p->drawPixmap( 0, 0, pm );
}

struct MediaButton {
   bool isToggle, isHeld, isDown;
};

//Layout information for the audioButtons (and if it is a toggle button or not)
MediaButton audioButtons[] = {
   { TRUE,  FALSE, FALSE }, // play
   { FALSE, FALSE, FALSE }, // stop
   { TRUE,  FALSE, FALSE }, // pause
   { FALSE, FALSE, FALSE }, // next
   { FALSE, FALSE, FALSE }, // previous
   { FALSE, FALSE, FALSE }, // volume up
   { FALSE, FALSE, FALSE }, // volume down
   { TRUE,  FALSE, FALSE }, // repeat/loop
   { FALSE, FALSE, FALSE }, // playlist
   { FALSE, FALSE, FALSE }, // forward
   { FALSE, FALSE, FALSE }  // back 
};

const char *skin_mask_file_names[11] = {
   "play", "stop", "pause", "next", "prev", "up",
   "down", "loop", "playlist", "forward", "back"
};


static void changeTextColor( QWidget *w ) {
   QPalette p = w->palette();
   p.setBrush( QColorGroup::Background, QColor( 167, 212, 167 ) );
   p.setBrush( QColorGroup::Base, QColor( 167, 212, 167 ) );
   w->setPalette( p );
}

static const int numButtons = (sizeof(audioButtons)/sizeof(MediaButton));


AudioWidget::AudioWidget(QWidget* parent, const char* name, WFlags f) :
    QWidget( parent, name, f ), songInfo( this ), slider( Qt::Horizontal, this ),  time( this )
    setCaption( tr("OpiePlayer") );

    Config cfg("OpiePlayer");
    cfg.setGroup("AudioWidget");
    skin = cfg.readEntry("Skin","default");
      //skin = "scaleTest";
// color of background, frame, degree of transparency

    QString skinPath = "opieplayer/skins/" + skin;
    pixBg = new QPixmap( Resource::loadPixmap( QString("%1/background").arg(skinPath) ) );
    imgUp = new QImage( Resource::loadImage( QString("%1/skin_up").arg(skinPath) ) );
    imgDn = new QImage( Resource::loadImage( QString("%1/skin_down").arg(skinPath) ) );

    imgButtonMask = new QImage( imgUp->width(), imgUp->height(), 8, 255 );
    imgButtonMask->fill( 0 );

    for ( int i = 0; i < 11; i++ ) {
        QString filename = QString(getenv("OPIEDIR")) + "/pics/" + skinPath + "/skin_mask_" + skin_mask_file_names[i] + ".png";
        masks[i] = new QBitmap( filename );

        if ( !masks[i]->isNull() ) {
            QImage imgMask = masks[i]->convertToImage();
            uchar **dest = imgButtonMask->jumpTable();
            for ( int y = 0; y < imgUp->height(); y++ ) {
                uchar *line = dest[y];
                for ( int x = 0; x < imgUp->width(); x++ ) 
                    if ( !qRed( imgMask.pixel( x, y ) ) ) 
                        line[x] = i + 1;
            }
        }

    }

    for ( int i = 0; i < 11; i++ ) {
        buttonPixUp[i] = NULL;
        buttonPixDown[i] = NULL;
    }

    setBackgroundPixmap( *pixBg );

    songInfo.setFocusPolicy( QWidget::NoFocus );
    changeTextColor( &songInfo );

    slider.setFixedHeight( 20 );
    slider.setMinValue( 0 );
    slider.setMaxValue( 1 );
    slider.setFocusPolicy( QWidget::NoFocus );
    slider.setBackgroundPixmap( *pixBg );

    time.setFocusPolicy( QWidget::NoFocus );
    time.setAlignment( Qt::AlignCenter );
    time.setFrame(FALSE);
    changeTextColor( &time );

    resizeEvent( NULL );

    connect( &slider,           SIGNAL( sliderPressed() ),      this, SLOT( sliderPressed() ) );
    connect( &slider,           SIGNAL( sliderReleased() ),     this, SLOT( sliderReleased() ) );

    connect( mediaPlayerState, SIGNAL( lengthChanged(long) ),  this, SLOT( setLength(long) ) );
    connect( mediaPlayerState, SIGNAL( viewChanged(char) ),    this, SLOT( setView(char) ) );
    connect( mediaPlayerState, SIGNAL( loopingToggled(bool) ), this, SLOT( setLooping(bool) ) );
    connect( mediaPlayerState, SIGNAL( pausedToggled(bool) ),  this, SLOT( setPaused(bool) ) );
    connect( mediaPlayerState, SIGNAL( playingToggled(bool) ), this, SLOT( setPlaying(bool) ) );

    // Intialise state
    setLength( mediaPlayerState->length() );
    setPosition( mediaPlayerState->position() );
    setLooping( mediaPlayerState->fullscreen() );
    setPaused( mediaPlayerState->paused() );
    setPlaying( mediaPlayerState->playing() );

}

AudioWidget::~AudioWidget() {
    
    for ( int i = 0; i < 11; i++ ) {
        delete buttonPixUp[i];
        delete buttonPixDown[i];
    }
    delete pixBg;
    delete imgUp;
    delete imgDn;
    delete imgButtonMask;
    for ( int i = 0; i < 11; i++ ) {
        delete masks[i];
    }
}

QPixmap *combineImageWithBackground( QImage img, QPixmap bg, QPoint offset ) {
    QPixmap pix( img.width(), img.height() );
    QPainter p( &pix );
    p.drawTiledPixmap( pix.rect(), bg, offset );
    p.drawImage( 0, 0, img );
    return new QPixmap( pix );
}


QPixmap *maskPixToMask( QPixmap pix, QBitmap mask )
{
    QPixmap *pixmap = new QPixmap( pix );
    pixmap->setMask( mask );
    return pixmap;
}



void AudioWidget::resizeEvent( QResizeEvent * ) {
    int h = height();
    int w = width();

    songInfo.setGeometry( QRect( 2, 10, w - 4, 20 ) );
    slider.setFixedWidth( w - 110 );
    slider.setGeometry( QRect( 15, h - 30, w - 90, 20 ) );
    slider.setBackgroundOrigin( QWidget::ParentOrigin );
    time.setGeometry( QRect( w - 85, h - 30, 70, 20 ) );

    xoff = ( w - imgUp->width() ) / 2;
    yoff = (( h - imgUp->height() ) / 2) - 10;
    QPoint p( xoff, yoff );

    QPixmap *pixUp = combineImageWithBackground( *imgUp, *pixBg, p );
    QPixmap *pixDn = combineImageWithBackground( *imgDn, *pixBg, p );

    for ( int i = 0; i < 11; i++ ) {
        if ( !masks[i]->isNull() ) {
            delete buttonPixUp[i];
            delete buttonPixDown[i];
            buttonPixUp[i] = maskPixToMask( *pixUp, *masks[i] );
            buttonPixDown[i] = maskPixToMask( *pixDn, *masks[i] );
        }
    }

    delete pixUp;
    delete pixDn;
}

static bool audioSliderBeingMoved = FALSE;


void AudioWidget::sliderPressed() {
    audioSliderBeingMoved = TRUE;
}


void AudioWidget::sliderReleased() {
    audioSliderBeingMoved = FALSE;
    if ( slider.width() == 0 )
  return;
    long val = long((double)slider.value() * mediaPlayerState->length() / slider.width());
    mediaPlayerState->setPosition( val );
}

void AudioWidget::setPosition( long i ) {
    //    qDebug("set position %d",i);
    updateSlider( i, mediaPlayerState->length() );
}


void AudioWidget::setLength( long max ) {
    updateSlider( mediaPlayerState->position(), max );
}


void AudioWidget::setView( char view ) {
    if (mediaPlayerState->streaming() ) {
        if( !slider.isHidden()) slider.hide();
        disconnect( mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
        disconnect( mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
    } else {
        // this stops the slider from being moved, thus
        // does not stop stream when it reaches the end
        slider->show();
        connect( mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
        connect( mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
    }

    if ( view == 'a' ) {
        startTimer( 150 );
        showMaximized();
    } else {
        killTimers();
        hide();
    }
}


static QString timeAsString( long length ) {
    length /= 44100;
    int minutes = length / 60;
    int seconds = length % 60;
    return QString("%1:%2%3").arg( minutes ).arg( seconds / 10 ).arg( seconds % 10 );
}

void AudioWidget::updateSlider( long i, long max ) {
    time.setText( timeAsString( i ) + " / " + timeAsString( max ) );
    if ( max == 0 ) {
        return;
    }
    // Will flicker too much if we don't do this
    // Scale to something reasonable
    int width = slider->width();
    int val = int((double)i * width / max);
    if ( !audioSliderBeingMoved ) {
        if ( slider->value() != val ) {
            slider->setValue( val );
        }

        if ( slider->maxValue() != width ) {
            slider->setMaxValue( width );
        }
    }
}


void AudioWidget::setToggleButton( int i, bool down ) {
    if ( down != audioButtons[i].isDown ) {
        toggleButton( i );
    }
}


void AudioWidget::toggleButton( int i ) {
    audioButtons[i].isDown = !audioButtons[i].isDown;
    QPainter p(this);
    paintButton ( &p, i );
}


void AudioWidget::paintButton( QPainter *p, int i ) {
    if ( audioButtons[i].isDown )
        p->drawPixmap( xoff, yoff, *buttonPixDown[i] );
    else
        p->drawPixmap( xoff, yoff, *buttonPixUp[i] );
}


void AudioWidget::timerEvent( QTimerEvent * ) {
//     static int frame = 0;
//     if ( !mediaPlayerState->paused() && audioButtons[ AudioPlay ].isDown ) {
//         frame = frame >= 7 ? 0 : frame + 1;
//     }
}


void AudioWidget::mouseMoveEvent( QMouseEvent *event ) {
    for ( int i = 0; i < numButtons; i++ ) {
        if ( event->state() == QMouseEvent::LeftButton ) {

              // The test to see if the mouse click is inside the button or not
            int x = event->pos().x() - xoff;
            int y = event->pos().y() - yoff;

            bool isOnButton = ( x > 0 && y > 0 && x < imgButtonMask->width()
                                && y < imgButtonMask->height() && imgButtonMask->pixelIndex( x, y ) == i + 1 );

            if ( isOnButton && i == AudioVolumeUp ) 
                qDebug("on up");

            if ( isOnButton && !audioButtons[i].isHeld ) {
                audioButtons[i].isHeld = TRUE;
                toggleButton(i);
                switch (i) {
                  case AudioVolumeUp:   
                      qDebug("more clicked");
                      emit moreClicked(); 
                      return;
                  case AudioVolumeDown: emit lessClicked(); return;
                }
            } else if ( !isOnButton && audioButtons[i].isHeld ) {
                audioButtons[i].isHeld = FALSE;
                toggleButton(i);
            }
        } else {
            if ( audioButtons[i].isHeld ) {
                audioButtons[i].isHeld = FALSE;
                if ( !audioButtons[i].isToggle )
                    setToggleButton( i, FALSE );
                switch (i) {
                  case AudioPlay:       mediaPlayerState->setPlaying(audioButtons[i].isDown); return;
                  case AudioStop:       mediaPlayerState->setPlaying(FALSE); return;
                  case AudioPause:      mediaPlayerState->setPaused(audioButtons[i].isDown); return;
                  case AudioNext:       mediaPlayerState->setNext(); return;
                  case AudioPrevious:   mediaPlayerState->setPrev(); return;
                  case AudioLoop:       mediaPlayerState->setLooping(audioButtons[i].isDown); return;
                  case AudioVolumeUp:   emit moreReleased(); return;
                  case AudioVolumeDown: emit lessReleased(); return;
                  case AudioPlayList:   mediaPlayerState->setList();  return;
                }
            }
        }
    }
}


void AudioWidget::mousePressEvent( QMouseEvent *event ) {
    mouseMoveEvent( event );
}


void AudioWidget::mouseReleaseEvent( QMouseEvent *event ) {
    mouseMoveEvent( event );
}


void AudioWidget::showEvent( QShowEvent* ) {
    QMouseEvent event( QEvent::MouseMove, QPoint( 0, 0 ), 0, 0 );
    mouseMoveEvent( &event );
}


void AudioWidget::closeEvent( QCloseEvent* ) {
    mediaPlayerState->setList();
}


void AudioWidget::paintEvent( QPaintEvent * ) {
    if ( !pe->erased() ) {
          // Combine with background and double buffer
        QPixmap pix( pe->rect().size() );
        QPainter p( &pix );
        p.translate( -pe->rect().topLeft().x(), -pe->rect().topLeft().y() );
        p.drawTiledPixmap( pe->rect(), *pixBg, pe->rect().topLeft() );
        for ( int i = 0; i < numButtons; i++ )
            paintButton( &p, i );
        QPainter p2( this );
        p2.drawPixmap( pe->rect().topLeft(), pix );
    } else {
        QPainter p( this );
        for ( int i = 0; i < numButtons; i++ )
            paintButton( &p, i );
    }
}

void AudioWidget::keyReleaseEvent( QKeyEvent *e)
{
    switch ( e->key() ) {
////////////////////////////// Zaurus keys
      case Key_Home:
          break;
      case Key_F9: //activity
           hide();
//           qDebug("Audio F9");
          break;
      case Key_F10: //contacts
          break;
      case Key_F11: //menu
          break;
      case Key_F12: //home
          break;
      case Key_F13: //mail
          break;
      case Key_Space: {
          if(mediaPlayerState->playing()) {
//                toggleButton(1);
              mediaPlayerState->setPlaying(FALSE);
//                toggleButton(1);
          } else {
//                toggleButton(0);
              mediaPlayerState->setPlaying(TRUE);
//                toggleButton(0);
          }
      }
          break;
      case Key_Down:
            toggleButton(6);
          emit lessClicked();
          emit lessReleased();
          toggleButton(6);
          break;
      case Key_Up:
           toggleButton(5);
           emit moreClicked();
           emit moreReleased();
           toggleButton(5);
           break;
      case Key_Right:
//            toggleButton(3);
          mediaPlayerState->setNext();
//            toggleButton(3);
          break;
      case Key_Left:
//            toggleButton(4);
          mediaPlayerState->setPrev();
//            toggleButton(4);
          break;
      case Key_Escape:
          break;

    };
}
