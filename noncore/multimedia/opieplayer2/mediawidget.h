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

#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

#include <qwidget.h>
#include <qmap.h>

#include "mediaplayerstate.h"
#include "playlistwidget.h"

#include <vector>

class MediaWidget : public QWidget
{
    Q_OBJECT
public:
    enum Command { Play = 0, Stop, Next, Previous, VolumeUp, VolumeDown, Loop, PlayList, Forward, Back, FullScreen, Undefined };
    enum ButtonType { NormalButton, ToggleButton };

    struct Button
    {
        Button() : command( Undefined ), type( NormalButton ), isHeld( false ), isDown( false ) {}

        Command command;

        ButtonType type : 1;
        bool isHeld : 1;
        bool isDown : 1;

        QBitmap mask;
        QPixmap pixUp;
        QPixmap pixDown;
    };
    typedef std::vector<Button> ButtonVector;

    struct SkinButtonInfo
    {
        Command command;
        const char *fileName;
        ButtonType type;
    };

    typedef std::vector<QBitmap> MaskVector;
    typedef std::vector<QPixmap> PixmapVector;

    MediaWidget( PlayListWidget &_playList, MediaPlayerState &_mediaPlayerState, QWidget *parent = 0, const char *name = 0 );
    virtual ~MediaWidget();

public slots:
    virtual void setDisplayType( MediaPlayerState::DisplayType displayType ) = 0;
    virtual void setLength( long length ) = 0;
    virtual void setPlaying( bool playing ) = 0;

signals:
    void moreReleased();
    void lessReleased();
    void forwardReleased();
    void backReleased();
    void forwardClicked();
    void backClicked();
    void moreClicked();
    void lessClicked();

protected:
    virtual void closeEvent( QCloseEvent * );

    virtual void paintEvent( QPaintEvent *pe );

    virtual void mouseMoveEvent( QMouseEvent *event );

    virtual void makeVisible();

    void handleCommand( Command command, bool buttonDown );

    bool isOverButton( const QPoint &position, int buttonId ) const;

    void paintAllButtons( QPainter &p );
    void paintButton( const Button &button );
    void paintButton( QPainter &p, const Button &button );

    void setToggleButton( Button &button, bool down );
    void setToggleButton( Command command, bool down );
    void toggleButton( Button &button );

    MediaPlayerState &mediaPlayerState;
    PlayListWidget &playList;

    ButtonVector buttons;

    QImage buttonMask;

    QPoint upperLeftOfButtonMask;

    QPixmap backgroundPixmap;
};

#endif // MEDIAWIDGET_H
/* vim: et sw=4 ts=4
 */
