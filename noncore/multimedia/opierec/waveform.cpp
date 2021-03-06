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
#include "waveform.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qpainter.h>

Waveform::Waveform( QWidget *parent, const char *name, WFlags fl )
   : QWidget( parent, name, fl )
{
    pixmap = 0;
    windowSize = 100;
    samplesPerPixel = 8000 / (5 * windowSize);
    currentValue = 0;
    numSamples = 0;
    windowPosn = 0;
    window = 0;
}

void Waveform::changeSettings( int frequency, int channels, snd_pcm_format_t format )
{
    makePixmap();
//   owarn << "change waveform " << frequency << ", " << channels << "" << oendl;
    samplesPerPixel = frequency * channels / (5 * windowSize);
//   owarn << "Waveform::changeSettings " << samplesPerPixel << "" << oendl;
    if ( !samplesPerPixel )
        samplesPerPixel = 1;
    currentValue = 0;
    numSamples = 0;
    windowPosn = 0;
    sndFormat = format;
    draw();
}

Waveform::~Waveform()
{
    if ( window )
        delete[] window;
    if ( pixmap )
        delete pixmap;
}

void Waveform::reset()
{
    makePixmap();
    currentValue = 0;
    numSamples = 0;
    windowPosn = 0;
    draw();
}

void Waveform::newSamples( const char *buf, int len )
{
    // Cache the object values in local variables.
    int samplesPerPixel = this->samplesPerPixel;
    int currentValue = this->currentValue;
    int numSamples = this->numSamples;
    short *window = this->window;
    int windowPosn = this->windowPosn;
    int windowSize = this->windowSize;

    // Average the incoming samples to scale them to the window.
    while ( len > 0 ) {
        if( sndFormat == SND_PCM_FORMAT_U8 ) {
            currentValue += (32768 - ((*buf++) * 256));
            --len;
        }
        else {
            currentValue += *((short *)buf) * 8;
            buf+=2;
            len-=2;
        }

        if ( ++numSamples >= samplesPerPixel ) {
            window[windowPosn++] = (short)(currentValue / numSamples);
            if ( windowPosn >= windowSize ) {
                this->windowPosn = windowPosn;
                draw();
                windowPosn = 0;
            }
            numSamples = 0;
            currentValue = 0;
        }
    }

    // Copy the final state back to the object.
//owarn << "" << currentValue << ", " << numSamples << ", " << windowPosn << "" << oendl;
    this->currentValue = currentValue;
    this->numSamples = numSamples;
    this->windowPosn = windowPosn;
}

void Waveform::makePixmap()
{
    if ( !pixmap ) {
        pixmap = new QPixmap( size() );
        windowSize = pixmap->width();
        window = new short [windowSize];
    }
}

void Waveform::draw()
{
    pixmap->fill( Qt::black );
    QPainter painter;
    painter.begin( pixmap );
    painter.setPen( Qt::green );

    int middle = pixmap->height() / 2;
    int mag;
    short *window = this->window;
    int posn;
    int size = windowPosn;
    for( posn = 0; posn < size; ++posn )
    {
        mag = (window[posn] * middle / 32768);
        painter.drawLine(posn, middle - mag, posn, middle + mag);
    }
    if ( windowPosn < windowSize )
    {
        painter.drawLine(windowPosn, middle, windowSize, middle);
    }

    painter.end();

    paintEvent( 0 );
}

void Waveform::paintEvent( QPaintEvent * )
{
    QPainter painter;
    painter.begin( this );

    if ( pixmap ) {
        painter.drawPixmap( 0, 0, *pixmap );
    } else {
        painter.setPen( Qt::green );
        QSize sz = size();
        painter.drawLine(0, sz.height() / 2, sz.width(), sz.height() / 2);
    }

    painter.end();
}
