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


#include <qtimer.h>
#include "xinecontrol.h"
#include "mediaplayerstate.h"
#include "videowidget.h"

extern MediaPlayerState *mediaPlayerState;
extern VideoWidget *videoUI;
XineControl::XineControl( QObject *parent, const char *name )
    : QObject( parent, name ) {
    libXine = new XINE::Lib(videoUI->vidWidget() );

	connect ( videoUI, SIGNAL( videoResized ( const QSize & )), this, SLOT( videoResized ( const QSize & )));
    connect( mediaPlayerState, SIGNAL( pausedToggled(bool) ),  this, SLOT( pause(bool) ) );
    connect( this, SIGNAL( positionChanged( long ) ), mediaPlayerState, SLOT( updatePosition( long ) ) );
    connect( this, SIGNAL( positionChanged( long ) ), mediaPlayerState, SLOT( setPosition( long ) ) );
    connect( mediaPlayerState, SIGNAL( playingToggled( bool ) ), this, SLOT( stop( bool ) ) );
    connect( mediaPlayerState, SIGNAL( fullscreenToggled( bool ) ), this, SLOT( setFullscreen( bool ) ) );
    connect( mediaPlayerState, SIGNAL( positionChanged( long ) ),  this,  SLOT( seekTo( long ) ) );

}

XineControl::~XineControl() {
    delete libXine;
}

void XineControl::play( const QString& fileName ) {
    libXine->play( fileName );
    mediaPlayerState->setPlaying( true );
    // default to audio view until we know how to handle video
    //   MediaDetect mdetect;
    char whichGui = mdetect.videoOrAudio( fileName );
    if (whichGui == 'f') {
        qDebug("Nicht erkannter Dateityp");
        return;
    }

    if (whichGui == 'a') {
        libXine->setShowVideo( false );
    } else {
        libXine->setShowVideo( true );
    }

    // determine if slider is shown
    //    mediaPlayerState->setIsStreaming( mdetect.isStreaming( fileName ) );
    mediaPlayerState->setIsStreaming( libXine->isSeekable() );
    // which gui (video / audio)
    mediaPlayerState->setView( whichGui );
    length();
    position();
}

void XineControl::stop( bool isSet ) {
    if ( !isSet) {
        libXine->stop();
        mediaPlayerState->setList();
        //mediaPlayerState->setPlaying( false );
    } else {
        // play again
    }
}

void XineControl::pause( bool isSet) {
    libXine->pause();
}

long XineControl::currentTime() {
    // todo: jede sekunde �berpr�fen
    m_currentTime =  libXine->currentTime();
    return m_currentTime;
    QTimer::singleShot( 1000, this, SLOT( currentTime() ) );
}

void  XineControl::length() {
    m_length = libXine->length();
    mediaPlayerState->setLength( m_length );
}

long XineControl::position() {
    m_position = ( currentTime()  );
    mediaPlayerState->setPosition( m_position  );
    long emitPos = (long)m_position;
    emit positionChanged( emitPos );
    if(mediaPlayerState->isPlaying)
    // needs to be stopped the media is stopped
    QTimer::singleShot( 1000, this, SLOT( position() ) );
//    qDebug("POSITION : %d", m_position);
    return m_position;
}

void XineControl::setFullscreen( bool isSet ) {
    libXine->showVideoFullScreen( isSet);
}

void XineControl::seekTo( long second ) {
    //  libXine->
}


void XineControl::videoResized ( const QSize &s )
{
	libXine-> resize ( s );
}
