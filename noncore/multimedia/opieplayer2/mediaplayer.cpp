#include <qpe/qpeapplication.h>
#include <qpe/qlibrary.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include <qmainwindow.h>
#include <qmessagebox.h>
#include <qwidgetstack.h>
#include <qfile.h>

#include "mediaplayer.h"
#include "playlistwidget.h"
#include "audiowidget.h"

#include "mediaplayerstate.h"


extern AudioWidget *audioUI;
extern PlayListWidget *playList;
extern MediaPlayerState *mediaPlayerState;


MediaPlayer::MediaPlayer( QObject *parent, const char *name )
    : QObject( parent, name ), volumeDirection( 0 ), currentFile( NULL ) {


//    QPEApplication::grabKeyboard(); // EVIL
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
    if ( !play ) {
  mediaPlayerState->setPaused( FALSE );
//  loopControl->stop( FALSE );
  return;
    }

    if ( mediaPlayerState->paused() ) {
  mediaPlayerState->setPaused( FALSE );
  return;
    }

    const DocLnk *playListCurrent = playList->current();
    if ( playListCurrent != NULL ) {
//  loopControl->stop( TRUE );
  currentFile = playListCurrent;
    }

    /*

    if ( currentFile == NULL ) {
  QMessageBox::critical( 0, tr( "No file"), tr( "Error: There is no file selected" ) );
  mediaPlayerState->setPlaying( FALSE );
  return;
    }

    if ( ((currentFile->file()).left(4) != "http") && !QFile::exists( currentFile->file() ) ) {
  QMessageBox::critical( 0, tr( "File not found"), tr( "The following file was not found: <i>" ) + currentFile->file() + "</i>" );
  mediaPlayerState->setPlaying( FALSE );
  return;
    }

    if ( !mediaPlayerState->newDecoder( currentFile->file() ) ) {
  QMessageBox::critical( 0, tr( "No decoder found"), tr( "Sorry, no appropriate decoders found for this file: <i>" ) + currentFile->file() + "</i>" );
  mediaPlayerState->setPlaying( FALSE );
  return;
    }

//    if ( !loopControl->init( currentFile->file() ) ) {
//  QMessageBox::critical( 0, tr( "Error opening file"), tr( "Sorry, an error occured trying to play the file: <i>" ) + currentFile->file() + "</i>" );
//  mediaPlayerState->setPlaying( FALSE );
//  return;
//    }
//   long seconds = loopControl->totalPlaytime();
    long seconds = 120;
    QString time;
   time.sprintf("%li:%02i", seconds/60, (int)seconds%60 );
    QString tickerText;
    if( currentFile->file().left(4) == "http" )
     tickerText= tr( " File: " ) + currentFile->name();
    else
    tickerText = tr( " File: " ) + currentFile->name() + tr(", Length: ") + time;

    QString fileInfo = mediaPlayerState->curDecoder()->fileInfo();
    if ( !fileInfo.isEmpty() )
  tickerText += ", " + fileInfo;
    audioUI->setTickerText( tickerText + "." );


    */ // alles nicht n�tig, xine k�mmert sich drum, man muss nur den return andio oder video gui geben


    //   loopControl->play();

    //  mediaPlayerState->setView( loopControl->hasVideo() ? 'v' : 'a' );
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
    // sollte volumeapplet machen
    //  AudioDevice::decreaseVolume();
}


void MediaPlayer::startIncreasingVolume() {
    volumeDirection = +1;
    startTimer( 100 );
    //  AudioDevice::increaseVolume();
}


void MediaPlayer::stopChangingVolume() {
    killTimers();
}


void MediaPlayer::timerEvent( QTimerEvent * ) {
//    if ( volumeDirection == +1 )
//  AudioDevice::increaseVolume();
//    else if ( volumeDirection == -1 )
        //       AudioDevice::decreaseVolume();
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
