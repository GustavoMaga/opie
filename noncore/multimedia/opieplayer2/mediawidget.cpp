/*
   Copyright (C) 2002 Simon Hausmann <hausmann@kde.org>
             (C) 2002 Max Reiss <harlekin@handhelds.org>
             (C) 2002 L. Potter <ljp@llornkcor.com>
             (C) 2002 Holger Freyther <zecke@handhelds.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "mediawidget.h"
#include "playlistwidget.h"

MediaWidget::MediaWidget( PlayListWidget &_playList, MediaPlayerState &_mediaPlayerState, QWidget *parent, const char *name )
    : QWidget( parent, name ), mediaPlayerState( _mediaPlayerState ), playList( _playList )
{
    connect( &mediaPlayerState, SIGNAL( displayTypeChanged( MediaPlayerState::DisplayType ) ),
             this, SLOT( setDisplayType( MediaPlayerState::DisplayType ) ) );
    connect( &mediaPlayerState, SIGNAL( lengthChanged( long ) ),
             this, SLOT( setLength( long ) ) );
    connect( &mediaPlayerState, SIGNAL( playingToggled( bool ) ),
             this, SLOT( setPlaying( bool ) ) );
}

MediaWidget::~MediaWidget()
{
}

void MediaWidget::closeEvent( QCloseEvent * )
{
    mediaPlayerState.setList();
}

void MediaWidget::paintEvent( QPaintEvent *pe )
{
    QPainter p( this );

    if ( mediaPlayerState.isFullscreen() ) {
        // Clear the background
        p.setBrush( QBrush( Qt::black ) );
        return;
    } 

    if ( !pe->erased() ) {
        // Combine with background and double buffer
        QPixmap pix( pe->rect().size() );
        QPainter p( &pix );
        p.translate( -pe->rect().topLeft().x(), -pe->rect().topLeft().y() );
        p.drawTiledPixmap( pe->rect(), backgroundPixmap, pe->rect().topLeft() );
        for ( unsigned int i = 0; i < buttons.count(); i++ ) {
            paintButton( p, buttons[ i ] );
        }
        QPainter p2( this );
        p2.drawPixmap( pe->rect().topLeft(), pix );
    } else {
        QPainter p( this );
        for ( unsigned int i = 0; i < buttons.count(); i++ )
            paintButton( p, buttons[ i ] );
    }
}

void MediaWidget::handleCommand( Command command, bool buttonDown )
{
    switch ( command ) {
        case Play:       mediaPlayerState.togglePaused();
        case Stop:       mediaPlayerState.setPlaying(FALSE); return;
        case Next:       if( playList.currentTab() == PlayListWidget::CurrentPlayList ) mediaPlayerState.setNext(); return;
        case Previous:   if( playList.currentTab() == PlayListWidget::CurrentPlayList ) mediaPlayerState.setPrev(); return;
        case Loop:       mediaPlayerState.setLooping( buttonDown ); return;
        case VolumeUp:   emit moreReleased(); return;
        case VolumeDown: emit lessReleased(); return;
        case PlayList:   mediaPlayerState.setList();  return;
        case Forward:    emit forwardReleased(); return;
        case Back:       emit backReleased(); return;
    }
}

bool MediaWidget::isOverButton( const QPoint &position, int buttonId ) const
{
    return ( position.x() > 0 && position.y() > 0 && 
             position.x() < buttonMask.width() && 
             position.y() < buttonMask.height() && 
             buttonMask.pixelIndex( position.x(), position.y() ) == buttonId + 1 );
}

void MediaWidget::paintButton( int buttonId )
{
    QPainter p( this );
    paintButton( p, buttons[ buttonId ] );
}

void MediaWidget::paintButton( QPainter &p, const Button &button )
{
    if ( button.isDown )
        p.drawPixmap( upperLeftOfButtonMask, button.pixDown );
    else
        p.drawPixmap( upperLeftOfButtonMask, button.pixUp );
}

void MediaWidget::toggleButton( int buttonId )
{
    buttons[ buttonId ].isDown = !buttons[ buttonId ].isDown;
    paintButton( buttonId );
}

/* vim: et sw=4 ts=4
 */
