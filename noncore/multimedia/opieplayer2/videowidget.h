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

#ifndef VIDEO_WIDGET_H
#define VIDEO_WIDGET_H

#include <qwidget.h>
#include "xinevideowidget.h"

class QPixmap;
class QSlider;

enum VideoButtons {
    VideoPrevious,
    VideoStop,
    VideoPlay,
    VideoPause,
    VideoNext,
    VideoPlayList,
    VideoFullscreen
};

class VideoWidget : public QWidget {
    Q_OBJECT
public:
    VideoWidget( QWidget* parent=0, const char* name=0, WFlags f=0 );
    ~VideoWidget();

    XineVideoWidget* vidWidget();
public slots:
    void updateSlider( long, long );
    void sliderPressed( );
    void sliderReleased( );
    void setPaused( bool b)      { setToggleButton( VideoPause, b ); }
    void setPlaying( bool b)     { setToggleButton( VideoPlay, b ); }
    void setFullscreen( bool b );
    void makeVisible();
    void setPosition( long );
    void setLength( long );
    void setView( char );

signals:
    void sliderMoved( long );
	void videoResized ( const QSize &s );

protected:
    void paintEvent( QPaintEvent *pe );
    void mouseMoveEvent( QMouseEvent *event );
    void mousePressEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );
    void closeEvent( QCloseEvent *event );
    void keyReleaseEvent( QKeyEvent *e);

private:
    void paintButton( QPainter *p, int i );
    void toggleButton( int );
    void setToggleButton( int, bool );

    QString backgroundPix;
    QSlider *slider;
    QPixmap *pixmaps[3];
    QImage *currentFrame;
    int scaledWidth;
    int scaledHeight;
    XineVideoWidget* videoFrame;
};

#endif // VIDEO_WIDGET_H



