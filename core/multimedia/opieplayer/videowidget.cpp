/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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
#include <qpe/resource.h>
#include <qpe/mediaplayerplugininterface.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qslider.h>
#include <qdrawutil.h>
#include "videowidget.h"
#include "mediaplayerstate.h"


#ifdef Q_WS_QWS
# define USE_DIRECT_PAINTER
# include <qdirectpainter_qws.h>
# include <qgfxraster_qws.h>
#endif


extern MediaPlayerState *mediaPlayerState;


static const int xo = 2; // movable x offset
static const int yo = 0; // movable y offset


struct MediaButton {
    int  xPos, yPos;
    bool isToggle, isHeld, isDown;
    int  controlType;
};


// Layout information for the videoButtons (and if it is a toggle button or not)
MediaButton videoButtons[] = {
    { 5+0*32+xo, 200+yo, FALSE, FALSE, FALSE, 4 }, // previous
    { 5+1*32+xo, 200+yo, FALSE, FALSE, FALSE, 1 }, // stop
    { 5+2*32+xo, 200+yo,  TRUE, FALSE, FALSE, 0 }, // play
    { 5+3*32+xo, 200+yo,  TRUE, FALSE, FALSE, 2 }, // pause
    { 5+4*32+xo, 200+yo, FALSE, FALSE, FALSE, 3 }, // next
    { 5+5*32+xo, 200+yo, FALSE, FALSE, FALSE, 8 }, // playlist
    { 5+6*32+xo, 200+yo,  TRUE, FALSE, FALSE, 9 }  // fullscreen
};


static const int numButtons = (sizeof(videoButtons)/sizeof(MediaButton));


VideoWidget::VideoWidget(QWidget* parent, const char* name, WFlags f) :
    QWidget( parent, name, f ), scaledWidth( 0 ), scaledHeight( 0 ) {
    setCaption( tr("OpiePlayer") );
    setBackgroundPixmap( Resource::loadPixmap( "opieplayer/metalFinish" ) );
    pixmaps[0] = new QPixmap( Resource::loadPixmap( "opieplayer/mediaButton0a" ) );
    pixmaps[1] = new QPixmap( Resource::loadPixmap( "opieplayer/mediaButton0b" ) );
    pixmaps[2] = new QPixmap( Resource::loadPixmap( "opieplayer/mediaControls0" ) );
    currentFrame = new QImage( 220 + 2, 160, (QPixmap::defaultDepth() == 16) ? 16 : 32 );

    slider = new QSlider( Qt::Horizontal, this );
    slider->setMinValue( 0 );
    slider->setMaxValue( 1 );
    slider->setBackgroundPixmap( Resource::loadPixmap( "opieplayer/metalFinish" ) );
    slider->setFocusPolicy( QWidget::NoFocus );
    slider->setGeometry( QRect( 7, 250, 220, 20 ) );

    connect( slider,         SIGNAL( sliderPressed() ),      this, SLOT( sliderPressed() ) );
    connect( slider,         SIGNAL( sliderReleased() ),     this, SLOT( sliderReleased() ) );

    connect( mediaPlayerState, SIGNAL( lengthChanged(long) ),  this, SLOT( setLength(long) ) );
    connect( mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
    connect( mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
    connect( mediaPlayerState, SIGNAL( viewChanged(char) ),    this, SLOT( setView(char) ) );
    connect( mediaPlayerState, SIGNAL( pausedToggled(bool) ),  this, SLOT( setPaused(bool) ) );
    connect( mediaPlayerState, SIGNAL( playingToggled(bool) ), this, SLOT( setPlaying(bool) ) );

    // Intialise state
    setLength( mediaPlayerState->length() );
    setPosition( mediaPlayerState->position() );
    setFullscreen( mediaPlayerState->fullscreen() );
    setPaused( mediaPlayerState->paused() );
    setPlaying( mediaPlayerState->playing() );
}


VideoWidget::~VideoWidget() {
    for ( int i = 0; i < 3; i++ )
  delete pixmaps[i];
    delete currentFrame;
}


static bool videoSliderBeingMoved = FALSE;


void VideoWidget::sliderPressed() {
    videoSliderBeingMoved = TRUE;
}


void VideoWidget::sliderReleased() {
    videoSliderBeingMoved = FALSE;
    if ( slider->width() == 0 )
  return;
    long val = long((double)slider->value() * mediaPlayerState->length() / slider->width());
    mediaPlayerState->setPosition( val );
}


void VideoWidget::setPosition( long i ) {
    updateSlider( i, mediaPlayerState->length() );
}


void VideoWidget::setLength( long max ) {
    updateSlider( mediaPlayerState->position(), max );
}


void VideoWidget::setView( char view ) {
    if ( view == 'v' ) {
  makeVisible();
    } else {
  // Effectively blank the view next time we show it so it looks nicer
  scaledWidth = 0;
  scaledHeight = 0;
  hide();
    }
}


void VideoWidget::updateSlider( long i, long max ) {
    // Will flicker too much if we don't do this
    if ( max == 0 )
  return;
    int width = slider->width();
    int val = int((double)i * width / max);
    if ( !mediaPlayerState->fullscreen() && !videoSliderBeingMoved ) {
  if ( slider->value() != val )
      slider->setValue( val );
  if ( slider->maxValue() != width )
      slider->setMaxValue( width );
    }
}


void VideoWidget::setToggleButton( int i, bool down ) {
    if ( down != videoButtons[i].isDown )
  toggleButton( i );
}


void VideoWidget::toggleButton( int i ) {
    videoButtons[i].isDown = !videoButtons[i].isDown;
    QPainter p(this);
    paintButton ( &p, i );
}


void VideoWidget::paintButton( QPainter *p, int i ) {
    int x = videoButtons[i].xPos;
    int y = videoButtons[i].yPos;
    int offset = 10 + videoButtons[i].isDown;
    p->drawPixmap( x, y, *pixmaps[videoButtons[i].isDown] );
    p->drawPixmap( x + 1 + offset, y + offset, *pixmaps[2], 9 * videoButtons[i].controlType, 0, 9, 9 );
}


void VideoWidget::mouseMoveEvent( QMouseEvent *event ) {
    for ( int i = 0; i < numButtons; i++ ) {
        int x = videoButtons[i].xPos;
        int y = videoButtons[i].yPos;
        if ( event->state() == QMouseEvent::LeftButton ) {
              // The test to see if the mouse click is inside the circular button or not
              // (compared with the radius squared to avoid a square-root of our distance)
            int radius = 16;
            QPoint center = QPoint( x + radius, y + radius );
            QPoint dXY = center - event->pos();
            int dist = dXY.x() * dXY.x() + dXY.y() * dXY.y();
            bool isOnButton = dist <= (radius * radius);
            if ( isOnButton != videoButtons[i].isHeld ) {
                videoButtons[i].isHeld = isOnButton;
                toggleButton(i);
            }
        } else {
            if ( videoButtons[i].isHeld ) {
                videoButtons[i].isHeld = FALSE;
                if ( !videoButtons[i].isToggle )
                    setToggleButton( i, FALSE );
            }
        }
        switch (i) {
          case VideoPlay:       mediaPlayerState->setPlaying(videoButtons[i].isDown); return;
          case VideoStop:       mediaPlayerState->setPlaying(FALSE); return;
          case VideoPause:      mediaPlayerState->setPaused(videoButtons[i].isDown); return;
          case VideoNext:       mediaPlayerState->setNext(); return;
          case VideoPrevious:   mediaPlayerState->setPrev(); return;
          case VideoPlayList:   mediaPlayerState->setList(); return;
          case VideoFullscreen: mediaPlayerState->setFullscreen( TRUE ); makeVisible(); return;
        }

    }
}


void VideoWidget::mousePressEvent( QMouseEvent *event ) {
    mouseMoveEvent( event );
}


void VideoWidget::mouseReleaseEvent( QMouseEvent *event ) {
    if ( mediaPlayerState->fullscreen() ) {
  mediaPlayerState->setFullscreen( FALSE );
  makeVisible();

  mouseMoveEvent( event );
    }
}


void VideoWidget::makeVisible() {
    if ( mediaPlayerState->fullscreen() ) {
  setBackgroundMode( QWidget::NoBackground );
  showFullScreen();
  resize( qApp->desktop()->size() );
  slider->hide();
    } else {
  setBackgroundPixmap( Resource::loadPixmap( "opieplayer/metalFinish" ) );
  showNormal();
  showMaximized();
  slider->show();
    }
}


void VideoWidget::paintEvent( QPaintEvent * ) {
    QPainter p( this );

    if ( mediaPlayerState->fullscreen() ) {
  // Clear the background
  p.setBrush( QBrush( Qt::black ) );
  p.drawRect( rect() );

  // Draw the current frame
  //p.drawImage( ); // If using directpainter we won't have a copy except whats on the screen
    } else {
  // draw border
  qDrawShadePanel( &p, 4, 15, 230, 170, colorGroup(), TRUE, 5, NULL );

  // Clear the movie screen first
  p.setBrush( QBrush( Qt::black ) );
  p.drawRect( 9, 20, 220, 160 );

  // draw current frame (centrally positioned from scaling to maintain aspect ratio)
  p.drawImage( 9 + (220 - scaledWidth) / 2, 20 + (160 - scaledHeight) / 2, *currentFrame, 0, 0, scaledWidth, scaledHeight );

  // draw the buttons
  for ( int i = 0; i < numButtons; i++ )
      paintButton( &p, i );

  // draw the slider
  slider->repaint( TRUE );
    }
}


void VideoWidget::closeEvent( QCloseEvent* ) {
    mediaPlayerState->setList();
}


bool VideoWidget::playVideo() {
    bool result = FALSE;

    int stream = 0;

    int sw = mediaPlayerState->curDecoder()->videoWidth( stream );
    int sh = mediaPlayerState->curDecoder()->videoHeight( stream );
    int dd = QPixmap::defaultDepth();
    int w = height();
    int h = width();

    ColorFormat format = (dd == 16) ? RGB565 : BGRA8888;

    if ( mediaPlayerState->fullscreen() ) {
#ifdef USE_DIRECT_PAINTER
  QDirectPainter p(this);

  if ( ( qt_screen->transformOrientation() == 3 ) &&
       ( ( dd == 16 ) || ( dd == 32 ) ) && ( p.numRects() == 1 ) ) {

      w = 320;
      h = 240;

      if ( mediaPlayerState->scaled() ) {
    // maintain aspect ratio
    if ( w * sh > sw * h )
        w = sw * h / sh;
    else
        h = sh * w / sw;
      } else  {
    w = sw;
    h = sh;
      }

      w--; // we can't allow libmpeg to overwrite.
      QPoint roff = qt_screen->mapToDevice( p.offset(), QSize( qt_screen->width(), qt_screen->height() ) );

      int ox = roff.x() - height() + 2 + (height() - w) / 2;
      int oy = roff.y() + (width() - h) / 2;
      int sx = 0, sy = 0;

      uchar* fp = p.frameBuffer() + p.lineStep() * oy;
      fp += dd * ox / 8;
      uchar **jt = new uchar*[h];
      for ( int i = h; i; i-- ) {
    jt[h - i] = fp;
    fp += p.lineStep();
      }

      result = mediaPlayerState->curDecoder()->videoReadScaledFrame( jt, sx, sy, sw, sh, w, h, format, 0) == 0;

      delete [] jt;
  } else {
#endif
      QPainter p(this);

      w = 320;
      h = 240;

      if ( mediaPlayerState->scaled() ) {
    // maintain aspect ratio
    if ( w * sh > sw * h )
        w = sw * h / sh;
    else
        h = sh * w / sw;
      } else  {
    w = sw;
    h = sh;
      }

      int bytes = ( dd == 16 ) ? 2 : 4;
      QImage tempFrame( w, h, bytes << 3 );
      result = mediaPlayerState->curDecoder()->videoReadScaledFrame( tempFrame.jumpTable(),
              0, 0, sw, sh, w, h, format, 0) == 0;
      if ( result && mediaPlayerState->fullscreen() ) {

    int rw = h, rh = w;
    QImage rotatedFrame( rw, rh, bytes << 3 );

    ushort* in  = (ushort*)tempFrame.bits();
    ushort* out = (ushort*)rotatedFrame.bits();
    int spl = rotatedFrame.bytesPerLine() / bytes;
    for (int x=0; x<h; x++) {
        if ( bytes == 2 ) {
      ushort* lout = out++ + (w - 1)*spl;
      for (int y=0; y<w; y++) {
          *lout=*in++;
          lout-=spl;
      }
        } else {
      ulong* lout = ((ulong *)out)++ + (w - 1)*spl;
      for (int y=0; y<w; y++) {
          *lout=*((ulong*)in)++;
          lout-=spl;
      }
        }
    }

    p.drawImage( (240 - rw) / 2, (320 - rh) / 2, rotatedFrame, 0, 0, rw, rh );
      }
#ifdef USE_DIRECT_PAINTER
  }
#endif
    } else {

  w = 220;
  h = 160;

  // maintain aspect ratio
  if ( w * sh > sw * h )
      w = sw * h / sh;
  else
      h = sh * w / sw;

  result = mediaPlayerState->curDecoder()->videoReadScaledFrame( currentFrame->jumpTable(), 0, 0, sw, sh, w, h, format, 0) == 0;

  QPainter p( this );

  // Image changed size, therefore need to blank the possibly unpainted regions first
  if ( scaledWidth != w || scaledHeight != h ) {
      p.setBrush( QBrush( Qt::black ) );
      p.drawRect( 9, 20, 220, 160 );
  }

  scaledWidth = w;
  scaledHeight = h;

  if ( result ) {
      p.drawImage( 9 + (220 - scaledWidth) / 2, 20 + (160 - scaledHeight) / 2, *currentFrame, 0, 0, scaledWidth, scaledHeight );
  }

    }

    return result;
}



void VideoWidget::keyReleaseEvent( QKeyEvent *e)
{
    switch ( e->key() ) {
////////////////////////////// Zaurus keys
      case Key_Home:
          break;
      case Key_F9: //activity
          break;
      case Key_F10: //contacts
//           hide();
          break;
      case Key_F11: //menu
          break;
      case Key_F12: //home
          break;
      case Key_F13: //mail
          break;
      case Key_Space: {
          if(mediaPlayerState->playing()) {
              mediaPlayerState->setPlaying(FALSE);
          } else {
              mediaPlayerState->setPlaying(TRUE);
          }
      }
          break;
      case Key_Down:
//            toggleButton(6);
//            emit lessClicked();
//            emit lessReleased();
//            toggleButton(6);
          break;
      case Key_Up:
//             toggleButton(5);
//             emit moreClicked();
//             emit moreReleased();
//             toggleButton(5);
           break;
      case Key_Right:
          mediaPlayerState->setNext();
          break;
      case Key_Left:
          mediaPlayerState->setPrev();
          break;
      case Key_Escape:
          break;

    };
}
