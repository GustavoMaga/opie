/*
� � � � � � � �             This file is part of the Opie Project
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
                             Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.

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
..}^=.= � � � = � � � ;      General Public License for more
++= � -. � � .` � � .:       details.
�: � � = �...= . :.=-
�-. � .:....=;==+<;          You should have received a copy of the GNU
� -_. . . � )=. �=           General Public License along with
� � -- � � � �:-=`           this; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qpe/config.h>

#include <stdlib.h>
#include <stdio.h>

#include "oticker.h"

OTicker::OTicker( QWidget* parent )
        : QLabel( parent ) {
  //        : QFrame( parent ) {
    setTextFormat(Qt::RichText);
    Config cfg("qpe");
    cfg.setGroup("Appearance");
    backgroundcolor = QColor( cfg.readEntry( "Background", "#E5E1D5" ) );
    foregroundcolor= Qt::black;
    updateTimerTime = 50;
    scrollLength = 1;
}

OTicker::~OTicker() {
}

void OTicker::setBackgroundColor(const QColor& backcolor) {
    backgroundcolor = backcolor;
    update();
}

void OTicker::setForegroundColor(const QColor& backcolor) {
    foregroundcolor = backcolor;
    update();
}

void OTicker::setFrame(int frameStyle) {
    setFrameStyle( frameStyle/*WinPanel | Sunken */);
    update();
}

void OTicker::setText( const QString& text ) {
    pos = 0; // reset it everytime the text is changed
    scrollText = text;
qDebug(scrollText);

 int pixelLen = 0;
   bool bigger = false;
   int contWidth = contentsRect().width();
   int contHeight = contentsRect().height();
    int pixelTextLen = fontMetrics().width( text );
    printf("<<<<<<<height %d, width %d, text width %d %d\n", contHeight, contWidth, pixelTextLen, scrollText.length());
  if( pixelTextLen < contWidth)
    {
      pixelLen = contWidth;
    }
  else
    {
        bigger = true;
      pixelLen = pixelTextLen;
    }
    QPixmap pm( pixelLen, contHeight);
//    pm.fill( QColor( 167, 212, 167 ));

    pm.fill(backgroundcolor);
    QPainter pmp( &pm );
    pmp.setPen(foregroundcolor );
    pmp.drawText( 0, 0, pixelTextLen, contHeight, AlignVCenter, scrollText );
    pmp.end();
    scrollTextPixmap = pm;

    killTimers();
    //    qDebug("Scrollupdate %d", updateTimerTime);
    if ( bigger /*pixelTextLen > contWidth*/ )
        startTimer( updateTimerTime);
    update();
}


void OTicker::timerEvent( QTimerEvent * ) {
  pos = ( pos <= 0 ) ? scrollTextPixmap.width() : pos - scrollLength;//1;
    repaint( FALSE );
}

void OTicker::drawContents( QPainter *p ) {
    int pixelLen = scrollTextPixmap.width();
    p->drawPixmap( pos, contentsRect().y(), scrollTextPixmap );
    if ( pixelLen > contentsRect().width() ) // Scrolling
        p->drawPixmap( pos - pixelLen, contentsRect().y(), scrollTextPixmap );
}

void OTicker::mouseReleaseEvent( QMouseEvent * ) {
//    qDebug("<<<<<<<>>>>>>>>>");
    emit mousePressed();
}

void OTicker::setUpdateTime(int time) {
 updateTimerTime=time;
}

void OTicker::setScrollLength(int len) {
scrollLength=len;
}

