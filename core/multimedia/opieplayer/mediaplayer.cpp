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


#include <qmessagebox.h>

#include "mediaplayer.h"
#include "playlistwidget.h"
#include "audiowidget.h"
#include "loopcontrol.h"
#include "audiodevice.h"

#include "mediaplayerstate.h"


extern AudioWidget *audioUI;
extern PlayListWidget *playList;
extern LoopControl *loopControl;
extern MediaPlayerState *mediaPlayerState;


MediaPlayer::MediaPlayer( QObject *parent, const char *name )
    : QObject( parent, name ), volumeDirection( 0 ), currentFile( NULL ) {

//    QPEApplication::grabKeyboard();
    connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );

    connect( mediaPlayerState, SIGNAL( playingToggled( bool ) ), this, SLOT( setPlaying( bool ) ) );
    connect( mediaPlayerState, SIGNAL( pausedToggled( bool ) ),  this, SLOT( pauseCheck( bool ) ) );
    connect( mediaPlayerState, SIGNAL( next() ),     this, SLOT( next() ) );
    connect( mediaPlayerState, SIGNAL( prev() ),     this, SLOT( prev() ) );

    connect( audioUI,  SIGNAL( moreClicked() ),         this, SLOT( startIncreasingVolume() ) );
    connect( audioUI,  SIGNAL( lessClicked() ),         this, SLOT( startDecreasingVolume() ) );
    connect( audioUI,  SIGNAL( moreReleased() ),        this, SLOT( stopChangingVolume() ) );
    connect( audioUI,  SIGNAL( lessReleased() ),        this, SLOT( stopChangingVolume() ) );
}


MediaPlayer::~MediaPlayer() {

}


void MediaPlayer::pauseCheck( bool b ) {
    // Only pause if playing
    if ( b && !mediaPlayerState->playing() )
  mediaPlayerState->setPaused( FALSE );
}


void MediaPlayer::play() {
    mediaPlayerState->setPlaying( FALSE );
    mediaPlayerState->setPlaying( TRUE );
}


void MediaPlayer::setPlaying( bool play ) {
    qDebug("MediaPlayer setPlaying");
    if ( !play ) {
        mediaPlayerState->setPaused( FALSE );
        loopControl->stop( FALSE );
        return;
    }

    if ( mediaPlayerState->paused() ) {
        mediaPlayerState->setPaused( FALSE );
        return;
    }
    qDebug("about to ctrash");
    const DocLnk *playListCurrent = playList->current();
    
    if ( playListCurrent != NULL ) {
        loopControl->stop( TRUE );
        currentFile = playListCurrent;
    }
    if ( currentFile == NULL ) {
        QMessageBox::critical( 0, tr( "No file"), tr( "Error: There is no file selected" ) );
        mediaPlayerState->setPlaying( FALSE );
        return;
    }

    if ( ((currentFile->file()).left(4) != "http") && !QFile::exists( currentFile->file() ) ) {
        QMessageBox::critical( 0, tr( "File not found"),
                               tr( "The following file was not found: <i>" )
                               + currentFile->file() + "</i>" );
        mediaPlayerState->setPlaying( FALSE );
        return;
    }

    if ( !mediaPlayerState->newDecoder( currentFile->file() ) ) {
        QMessageBox::critical( 0, tr( "No decoder found"),
                               tr( "Sorry, no appropriate decoders found for this file: <i>" )
                               + currentFile->file() + "</i>" );
        mediaPlayerState->setPlaying( FALSE );
        return;
    }

    if ( !loopControl->init( currentFile->file() ) ) {
        QMessageBox::critical( 0, tr( "Error opening file"),
                               tr( "Sorry, an error occured trying to play the file: <i>" ) + currentFile->file() + "</i>" );
        mediaPlayerState->setPlaying( FALSE );
        return;
    }
    long seconds = loopControl->totalPlaytime();
    QString time; time.sprintf("%li:%02i", seconds/60, (int)seconds%60 );
    QString tickerText;
    if( currentFile->file().left(4) == "http" )
        tickerText= tr( " File: " ) + currentFile->name();
    else
        tickerText = tr( " File: " ) + currentFile->name() + tr(", Length: ") + time;

    QString fileInfo = mediaPlayerState->curDecoder()->fileInfo();
    if ( !fileInfo.isEmpty() )
        tickerText += ", " + fileInfo;
    audioUI->setTickerText( tickerText + "." );

    loopControl->play();

    mediaPlayerState->setView( loopControl->hasVideo() ? 'v' : 'a' );
}


void MediaPlayer::prev() {
    if ( playList->prev() )
  play();
    else if ( mediaPlayerState->looping() ) {
        if ( playList->last() )
      play();
    } else
  mediaPlayerState->setList();
}


void MediaPlayer::next() {
    if ( playList->next() )
  play();
    else if ( mediaPlayerState->looping() ) {
        if ( playList->first() )
      play();
    } else
  mediaPlayerState->setList();
}


void MediaPlayer::startDecreasingVolume() {
    volumeDirection = -1;
    startTimer( 100 );
    AudioDevice::decreaseVolume();
}


void MediaPlayer::startIncreasingVolume() {
    volumeDirection = +1;
    startTimer( 100 );
    AudioDevice::increaseVolume();
    
}

bool drawnOnScreenDisplay = FALSE;
unsigned int onScreenDisplayVolume = 0;
const int yoff = 110;

void MediaPlayer::stopChangingVolume() {
    killTimers();

      // Get rid of the on-screen display stuff
    drawnOnScreenDisplay = FALSE;
    onScreenDisplayVolume = 0;
    int w = audioUI->width();
    int h = audioUI->height();
    audioUI->repaint( (w - 200) / 2, h - yoff, 200 + 9, 70, FALSE );
}


void MediaPlayer::timerEvent( QTimerEvent * ) {
//    qDebug("timer");
    if ( volumeDirection == +1 )
        AudioDevice::increaseVolume();
    else if ( volumeDirection == -1 )
        AudioDevice::decreaseVolume();

    // Display an on-screen display volume
    unsigned int l, r, v; bool m;
    AudioDevice::getVolume( l, r, m );
    v = ((l + r) * 11) / (2*0xFFFF);

    if ( drawnOnScreenDisplay && onScreenDisplayVolume == v ) {
//        qDebug("returning %d, %d, %d, %d", v, l, r, m);
        return;
    }

    int w = audioUI->width();
    int h = audioUI->height();

    if ( drawnOnScreenDisplay ) {
        if ( onScreenDisplayVolume > v )
            audioUI->repaint( (w - 200) / 2 + v * 20 + 0, h - yoff + 40,
                              (onScreenDisplayVolume - v) * 20 + 9, 30, FALSE );
    }

    drawnOnScreenDisplay = TRUE;
    onScreenDisplayVolume = v;

    QPainter p( audioUI );
    p.setPen( QColor( 0x10, 0xD0, 0x10 ) );
    p.setBrush( QColor( 0x10, 0xD0, 0x10 ) );

    QFont f;
    f.setPixelSize( 20 );
    f.setBold( TRUE );
    p.setFont( f );
    p.drawText( (w - 200) / 2, h - yoff + 20, tr("Volume") );

    for ( unsigned int i = 0; i < 10; i++ ) {
        if ( v > i ) 
            p.drawRect( (w - 200) / 2 + i * 20 + 0, h - yoff + 40, 9, 30 );
        else 
            p.drawRect( (w - 200) / 2 + i * 20 + 3, h - yoff + 50, 3, 10 );
    }
}

void MediaPlayer::keyReleaseEvent( QKeyEvent *e) {
    switch ( e->key() ) {
////////////////////////////// Zaurus keys
      case Key_Home:
          break;
      case Key_F9: //activity
          break;
      case Key_F10: //contacts
          break;
      case Key_F11: //menu
          break;
      case Key_F12: //home
          qDebug("Blank here");
          break;
      case Key_F13: //mail
          break;
    }
}

void MediaPlayer::doBlank() {

}

void MediaPlayer::doUnblank() {

}

void MediaPlayer::cleanUp() {
//     QPEApplication::grabKeyboard();
//     QPEApplication::ungrabKeyboard();

}
