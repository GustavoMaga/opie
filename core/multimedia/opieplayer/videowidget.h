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
#ifndef VIDEO_WIDGET_H
#define VIDEO_WIDGET_H


#include <qwidget.h>

class QPixmap;
class QSlider;


enum VideoButtons {
    VideoStop,
    VideoPlay,
//    VideoPause,
    VideoPrevious,
    VideoNext,
    VideoVolUp,
    VideoVolDown,
//    VideoPlayList,
    VideoFullscreen
};

class VideoWidget : public QWidget {
    Q_OBJECT
public:
    VideoWidget( QWidget* parent=0, const char* name=0, WFlags f=0 );
    ~VideoWidget();

    bool playVideo();

public slots:
    void updateSlider( long, long );
    void sliderPressed( );
    void sliderReleased( );
//    void setPaused( bool b)      { setToggleButton( VideoPause, b ); }
    void setPlaying( bool b)     { setToggleButton( VideoPlay, b ); }
    void setFullscreen( bool b ) { setToggleButton( VideoFullscreen, b ); }
    void makeVisible();
    void setPosition( long );
    void setLength( long );
    void setView( char );

signals:
    void moreClicked();
    void lessClicked();
    void moreReleased();
    void lessReleased();
    void sliderMoved( long );

protected:
    void resizeEvent( QResizeEvent * );
    void paintEvent( QPaintEvent *pe );
    void mouseMoveEvent( QMouseEvent *event );
    void mousePressEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );
    void closeEvent( QCloseEvent *event );
    void keyReleaseEvent( QKeyEvent *e);

private:
    QPixmap *pixBg;
    QImage  *imgUp;
    QImage  *imgDn;
    QImage  *imgButtonMask;
    QBitmap *masks[7];
    QString backgroundPix;
    QPixmap *buttonPixUp[7];
    QPixmap *buttonPixDown[7];
    QString skin;
    QSlider *slider;
    QPixmap *pixmaps[3];
    QImage *currentFrame;
    int xoff, yoff;
    int scaledWidth;
    int scaledHeight;

  void paintButton( QPainter *p, int i );
    void toggleButton( int );
    void setToggleButton( int, bool );

};


#endif // VIDEO_WIDGET_H



