/*
� � � � � � � �             This file is part of the Opie Project

� � � � � � �                Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
                             Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
              =.
            .=l.
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This program is free software; you can
.> <`_, � > �. � <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
� � .%`+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This program is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful,  but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|`    MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.= � � � = � � � ;      Library General Public License for more
++= � -. � � .` � � .:       details.
�: � � = �...= . :.=-
�-. � .:....=;==+<;          You should have received a copy of the GNU
� -_. . . � )=. �=           Library General Public License along with
� � -- � � � �:-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/


#include "audiowidget.h"

#include <qpe/qpeapplication.h>

using namespace Opie::Ui;
namespace
{

const int xo = -2; // movable x offset
const int yo = 22; // movable y offset

const MediaWidget::SkinButtonInfo skinInfo[] =
{
    { MediaWidget::Play, "play", MediaWidget::ToggleButton },
    { MediaWidget::Stop, "stop", MediaWidget::NormalButton },
    { MediaWidget::Next, "next", MediaWidget::NormalButton },
    { MediaWidget::Previous, "prev", MediaWidget::NormalButton },
    { MediaWidget::VolumeUp, "up", MediaWidget::NormalButton },
    { MediaWidget::VolumeDown, "down", MediaWidget::NormalButton },
    { MediaWidget::Loop, "loop", MediaWidget::ToggleButton },
    { MediaWidget::PlayList, "playlist", MediaWidget::NormalButton },
    { MediaWidget::Forward, "forward", MediaWidget::NormalButton },
    { MediaWidget::Back, "back", MediaWidget::NormalButton }
};

const uint buttonCount = sizeof( skinInfo ) / sizeof( skinInfo[ 0 ] );

void changeTextColor( QWidget * w) {
   QPalette p = w->palette();
   p.setBrush( QColorGroup::Background, QColor( 167, 212, 167 ) );
   p.setBrush( QColorGroup::Base, QColor( 167, 212, 167 ) );
   w->setPalette( p );
}

}

AudioWidget::AudioWidget( PlayListWidget &playList, MediaPlayerState &mediaPlayerState, QWidget* parent, const char* name) :

    MediaWidget( playList, mediaPlayerState, parent, name ), songInfo( this ), slider( Qt::Horizontal, this ),  time( this ),
    audioSliderBeingMoved( false )
{
    setCaption( tr("OpiePlayer") );

    loadSkin();

    connect( &mediaPlayerState, SIGNAL( loopingToggled(bool) ), this, SLOT( setLooping(bool) ) );
    connect( &mediaPlayerState, SIGNAL( isSeekableToggled(bool) ), this, SLOT( setSeekable(bool) ) );

    connect( this,  SIGNAL( forwardClicked() ), this, SLOT( skipFor() ) );
    connect( this,  SIGNAL( backClicked() ),  this, SLOT( skipBack() ) );
    connect( this,  SIGNAL( forwardReleased() ), this, SLOT( stopSkip() ) );
    connect( this,  SIGNAL( backReleased() ), this, SLOT( stopSkip() ) );

    // Intialise state
    setLength( mediaPlayerState.length() );
    setPosition( mediaPlayerState.position() );
    setLooping( mediaPlayerState.isFullscreen() );
    //    setPaused( mediaPlayerState->paused() );
    setPlaying( mediaPlayerState.isPlaying() );
}

AudioWidget::~AudioWidget() {

//    mediaPlayerState->setPlaying(false);
}

MediaWidget::GUIInfo AudioWidget::guiInfo()
{
    return GUIInfo( QString::null /* infix */, ::skinInfo, ::buttonCount );
}

void AudioWidget::resizeEvent( QResizeEvent *e ) {
    int h = height();
    int w = width();

    songInfo.setGeometry( QRect( 2, 2, w - 4, 20 ) );
    slider.setFixedWidth( w - 110 );
    slider.setGeometry( QRect( 15, h - 22, w - 90, 20 ) );
    slider.setBackgroundOrigin( QWidget::ParentOrigin );
    time.setGeometry( QRect( w - 85, h - 30, 70, 20 ) );

    upperLeftOfButtonMask.rx() = ( w - buttonUpImage.width() ) / 2;
    upperLeftOfButtonMask.ry() = (( h - buttonUpImage.height() ) / 2) - 10;

    MediaWidget::resizeEvent( e );
}

void AudioWidget::sliderPressed() {
    audioSliderBeingMoved = TRUE;
}


void AudioWidget::sliderReleased() {
    audioSliderBeingMoved = FALSE;
    if ( slider.width() == 0 )
  return;
    long val = long((double)slider.value() * mediaPlayerState.length() / slider.width());
    mediaPlayerState.setPosition( val );
}

void AudioWidget::setPosition( long i ) {
    //    odebug << "<<<<<<<<<<<<<<<<<<<<<<<<set position " << i << "" << oendl; 
    updateSlider( i, mediaPlayerState.length() );
}


void AudioWidget::setLength( long max ) {
    updateSlider( mediaPlayerState.position(), max );
}


void AudioWidget::setDisplayType( MediaPlayerState::DisplayType mediaType ) {
    if ( mediaType == MediaPlayerState::Audio ) {
        // startTimer( 150 );
        QPEApplication::showWidget( this );
        return;
    }

    killTimers();
    hide();
}

void AudioWidget::loadSkin()
{
    loadDefaultSkin( guiInfo() );

    songInfo.setFocusPolicy( QWidget::NoFocus );
//    changeTextColor( &songInfo );
//    songInfo.setBackgroundColor( QColor( 167, 212, 167 ));
//    songInfo.setFrameStyle( QFrame::NoFrame);
    songInfo.setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
//    songInfo.setForegroundColor(Qt::white);

    slider.setFixedHeight( 20 );
    slider.setMinValue( 0 );
    slider.setMaxValue( 1 );
    slider.setFocusPolicy( QWidget::NoFocus );
    slider.setBackgroundPixmap( backgroundPixmap );

//     Config cofg("qpe");
//     cofg.setGroup("Appearance");
//     QColor backgroundcolor = QColor( cofg.readEntry( "Background", "#E5E1D5" ) );

    time.setFocusPolicy( QWidget::NoFocus );
    time.setAlignment( Qt::AlignCenter );

//    time.setFrame(FALSE);
//    changeTextColor( &time );

    resizeEvent( 0 );
}

void AudioWidget::setSeekable( bool isSeekable ) {

    if ( !isSeekable ) {
        odebug << "<<<<<<<<<<<<<<file is STREAMING>>>>>>>>>>>>>>>>>>>" << oendl; 
        if( !slider.isHidden()) {
            slider.hide();
        }
        disconnect( &mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
        disconnect( &mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
        disconnect( &slider, SIGNAL( sliderPressed() ), this, SLOT( sliderPressed() ) );
        disconnect( &slider, SIGNAL( sliderReleased() ), this, SLOT( sliderReleased() ) );
    } else {
        // this stops the slider from being moved, thus
        // does not stop stream when it reaches the end
        slider.show();
        odebug << " CONNECT SET POSTION " << oendl; 
        connect( &mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
        connect( &mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
        connect( &slider, SIGNAL( sliderPressed() ), this, SLOT( sliderPressed() ) );
        connect( &slider, SIGNAL( sliderReleased() ), this, SLOT( sliderReleased() ) );
    }
}


static QString timeAsString( long length ) {
    int minutes = length / 60;
    int seconds = length % 60;
    return QString("%1:%2%3").arg( minutes ).arg( seconds / 10 ).arg( seconds % 10 );
}

void AudioWidget::updateSlider( long i, long max ) {

    time.setText( timeAsString( i ) + " / " + timeAsString( max ) );
//    qDebug( timeAsString( i ) + " / " + timeAsString( max ) ) ;

    if ( max == 0 ) {
        return;
    }
    // Will flicker too much if we don't do this
    // Scale to something reasonable
    int width = slider.width();
    int val = int((double)i * width / max);
    if ( !audioSliderBeingMoved ) {
        if ( slider.value() != val ) {
            slider.setValue( val );
        }

        if ( slider.maxValue() != width ) {
            slider.setMaxValue( width );
        }
    }
}

void AudioWidget::skipFor() {
    skipDirection = +1;
    startTimer( 50 );
    mediaPlayerState.setPosition( mediaPlayerState.position() + 2 );
}

void AudioWidget::skipBack() {
    skipDirection = -1;
    startTimer( 50 );
    mediaPlayerState.setPosition( mediaPlayerState.position() - 2 );
}



void AudioWidget::stopSkip() {
    killTimers();
}


void AudioWidget::timerEvent( QTimerEvent * ) {
    if ( skipDirection == +1 ) {
        mediaPlayerState.setPosition( mediaPlayerState.position() + 2 );
    }  else if ( skipDirection == -1 ) {
        mediaPlayerState.setPosition( mediaPlayerState.position() - 2 );
    }
}

void AudioWidget::keyReleaseEvent( QKeyEvent *e) {
    switch ( e->key() ) {
        ////////////////////////////// Zaurus keys
      case Key_Home:
          break;
      case Key_F9: //activity
           hide();
           //           odebug << "Audio F9" << oendl; 
           e->accept();
          break;
      case Key_F10: //contacts
          break;
      case Key_F11: //menu
              mediaPlayerState.toggleBlank();
              e->accept();
          break;
      case Key_F12: //home
          break;
      case Key_F13: //mail
             mediaPlayerState.toggleBlank();
             e->accept();
          break;
      case Key_Space: {
          e->accept();
          mediaPlayerState.togglePaused();
      }
          break;
      case Key_Down:
          //              toggleButton(6);
          emit lessClicked();
          emit lessReleased();
          //          toggleButton(6);
          e->accept();
          break;
      case Key_Up:
          //             toggleButton(5);
           emit moreClicked();
           emit moreReleased();
           //             toggleButton(5);
           e->accept();
           break;
      case Key_Right:
          //            toggleButton(3);
          mediaPlayerState.setNext();
          //            toggleButton(3);
           e->accept();
          break;
      case Key_Left:
          //            toggleButton(4);
          mediaPlayerState.setPrev();
          //            toggleButton(4);
          e->accept();
          break;
      case Key_Escape: {
      }
          break;

    };
}
