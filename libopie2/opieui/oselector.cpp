/* This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)

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

/* QT */

#include <qimage.h>
#include <qpainter.h>
#include <qdrawutil.h>

/* OPIE */

#include <opie2/oimageeffect.h>
#include <opie2/oselector.h>

#define STORE_W 8
#define STORE_W2 STORE_W * 2

//-----------------------------------------------------------------------------
/*
 * 2D value selector.
 * The contents of the selector are drawn by derived class.
 */

OXYSelector::OXYSelector( QWidget *parent, const char *name )
	: QWidget( parent, name )
{
	xPos = 0;
	yPos = 0;
	minX = 0;
	minY = 0;
	maxX = 100;
	maxY = 100;
	store.setOptimization( QPixmap::BestOptim );
	store.resize( STORE_W2, STORE_W2 );
}


OXYSelector::~OXYSelector()
{}


void OXYSelector::setRange( int _minX, int _minY, int _maxX, int _maxY )
{
	px = 2;
	py = 2;
	minX = _minX;
	minY = _minY;
	maxX = _maxX;
	maxY = _maxY;
}

void OXYSelector::setValues( int _xPos, int _yPos )
{
	xPos = _xPos;
	yPos = _yPos;

	if ( xPos > maxX )
		xPos = maxX;
	else if ( xPos < minX )
		xPos = minX;
	
	if ( yPos > maxY )
		yPos = maxY;
	else if ( yPos < minY )
		yPos = minY;

	int xp = 2 + (width() - 4) * xPos / (maxX - minX);
	int yp = height() - 2 - (height() - 4) * yPos / (maxY - minY);

	setPosition( xp, yp );
}

QRect OXYSelector::contentsRect() const
{
	return QRect( 2, 2, width()-4, height()-4 );
}

void OXYSelector::paintEvent( QPaintEvent *ev )
{
	QRect cursorRect( px - STORE_W, py - STORE_W, STORE_W2, STORE_W2);
	QRect paintRect = ev->rect();

	QPainter painter;
	painter.begin( this );

	QBrush brush;
	qDrawShadePanel( &painter, 0, 0, width(), height(), colorGroup(),
			TRUE, 2, &brush );

	drawContents( &painter );
	if (paintRect.contains(cursorRect))
	{
	   bitBlt( &store, 0, 0, this, px - STORE_W, py - STORE_W,
		STORE_W2, STORE_W2, CopyROP );
	   drawCursor( &painter, px, py );
        }
        else if (paintRect.intersects(cursorRect))
        {
           repaint( cursorRect, false);
        }

	painter.end();
}

void OXYSelector::mousePressEvent( QMouseEvent *e )
{
    int xVal, yVal;
    valuesFromPosition( e->pos().x() - 2, e->pos().y() - 2, xVal, yVal );
    setValues( xVal, yVal );

    emit valueChanged( xPos, yPos );
}

void OXYSelector::mouseMoveEvent( QMouseEvent *e )
{
    int xVal, yVal;
    valuesFromPosition( e->pos().x() - 2, e->pos().y() - 2, xVal, yVal );
    setValues( xVal, yVal );

    emit valueChanged( xPos, yPos );
}

void OXYSelector::wheelEvent( QWheelEvent *e )
{
    #if QT_VERSION > 290
    if ( e->orientation() == Qt::Horizontal )
        setValues( xValue() + e->delta()/120, yValue() );
    else
        setValues( xValue(), yValue() + e->delta()/120 );

    emit valueChanged( xPos, yPos );
    #endif
}

void OXYSelector::valuesFromPosition( int x, int y, int &xVal, int &yVal ) const
{
    xVal = ( (maxX-minX) * (x-2) ) / ( width()-4 );
    yVal = maxY - ( ( (maxY-minY) * (y-2) ) / ( height()-4 ) );

    if ( xVal > maxX )
        xVal = maxX;
    else if ( xVal < minX )
        xVal = minX;

    if ( yVal > maxY )
        yVal = maxY;
    else if ( yVal < minY )
        yVal = minY;
}

void OXYSelector::setPosition( int xp, int yp )
{
    if ( xp < 2 )
        xp = 2;
    else if ( xp > width() - 2 )
        xp = width() - 2;

    if ( yp < 2 )
        yp = 2;
    else if ( yp > height() - 2 )
        yp = height() - 2;

    QPainter painter;
    painter.begin( this );

    bitBlt( this, px - STORE_W, py - STORE_W, &store, 0, 0,
            STORE_W2, STORE_W2, CopyROP );
    bitBlt( &store, 0, 0, this, xp - STORE_W, yp - STORE_W,
            STORE_W2, STORE_W2, CopyROP );
    drawCursor( &painter, xp, yp );
    px = xp;
    py = yp;

    painter.end();
}

void OXYSelector::drawContents( QPainter * )
{}


void OXYSelector::drawCursor( QPainter *p, int xp, int yp )
{
    p->setPen( QPen( white ) );

    p->drawLine( xp - 6, yp - 6, xp - 2, yp - 2 );
    p->drawLine( xp - 6, yp + 6, xp - 2, yp + 2 );
    p->drawLine( xp + 6, yp - 6, xp + 2, yp - 2 );
    p->drawLine( xp + 6, yp + 6, xp + 2, yp + 2 );
}

//-----------------------------------------------------------------------------
/*
 * 1D value selector with contents drawn by derived class.
 * See OColorDialog for example.
 */


OSelector::OSelector( QWidget *parent, const char *name )
	: QWidget( parent, name ), QRangeControl()
{
	_orientation = Horizontal;
	_indent = TRUE;
}

OSelector::OSelector( Orientation o, QWidget *parent, const char *name )
	: QWidget( parent, name ), QRangeControl()
{
	_orientation = o;
	_indent = TRUE;
}


OSelector::~OSelector()
{}


QRect OSelector::contentsRect() const
{
	if ( orientation() == Vertical )
		return QRect( 2, 5, width()-9, height()-10 );
	else
		return QRect( 5, 2, width()-10, height()-9 );
}

void OSelector::paintEvent( QPaintEvent * )
{
	QPainter painter;

	painter.begin( this );

	drawContents( &painter );

	QBrush brush;

	if ( indent() )
	{
		if ( orientation() == Vertical )
			qDrawShadePanel( &painter, 0, 3, width()-5, height()-6,
				colorGroup(), TRUE, 2, &brush );
		else
			qDrawShadePanel( &painter, 3, 0, width()-6, height()-5,
				colorGroup(), TRUE, 2, &brush );
	}

	QPoint pos = calcArrowPos( value() );
	drawArrow( &painter, TRUE, pos );   

	painter.end();
}

void OSelector::mousePressEvent( QMouseEvent *e )
{
	moveArrow( e->pos() );
}

void OSelector::mouseMoveEvent( QMouseEvent *e )
{
	moveArrow( e->pos() );
}

void OSelector::wheelEvent( QWheelEvent *e )
{
	int val = value() + e->delta()/120;
	emit valueChanged( val );
	setValue( val );
}

void OSelector::valueChange()
{
	QPainter painter;
	QPoint pos;

	painter.begin( this );

	pos = calcArrowPos( prevValue() );
	drawArrow( &painter, FALSE, pos );   

	pos = calcArrowPos( value() );
	drawArrow( &painter, TRUE, pos );   

	painter.end();
}

void OSelector::moveArrow( const QPoint &pos )
{
	int val;

	if ( orientation() == Vertical )
		val = ( maxValue() - minValue() ) * (height()-pos.y()-3)
				/ (height()-10) + minValue();
	else
		val = ( maxValue() - minValue() ) * (width()-pos.x()-3)
				/ (width()-10) + minValue();

	if ( val > maxValue() )
		val = maxValue();
	if ( val < minValue() )
		val = minValue();

	emit valueChanged( val );
	setValue( val );
}

QPoint OSelector::calcArrowPos( int val )
{
	QPoint p;

	if ( orientation() == Vertical )
	{
		p.setY( height() - ( (height()-10) * val
				/ ( maxValue() - minValue() ) + 5 ) );
		p.setX( width() - 5 );
	}
	else
	{
		p.setX( width() - ( (width()-10) * val
				/ ( maxValue() - minValue() ) + 5 ) );
		p.setY( height() - 5 );
	}

	return p;
}

void OSelector::drawContents( QPainter * )
{}

void OSelector::drawArrow( QPainter *painter, bool show, const QPoint &pos )
{
  if ( show )
  {
    QPointArray array(3);

    painter->setPen( QPen() );
    painter->setBrush( QBrush( colorGroup().buttonText() ) );
    if ( orientation() == Vertical )
    {
      array.setPoint( 0, pos.x()+0, pos.y()+0 );
      array.setPoint( 1, pos.x()+5, pos.y()+5 );
      array.setPoint( 2, pos.x()+5, pos.y()-5 );
    }
    else
    {
      array.setPoint( 0, pos.x()+0, pos.y()+0 );
      array.setPoint( 1, pos.x()+5, pos.y()+5 );
      array.setPoint( 2, pos.x()-5, pos.y()+5 );
    }

    painter->drawPolygon( array );
  } 
  else 
  {
    if ( orientation() == Vertical )
    {
       repaint(pos.x(), pos.y()-5, 6, 11, true);
    }
    else
    {
       repaint(pos.x()-5, pos.y(), 11, 6, true);
    }
  }
}

//----------------------------------------------------------------------------

OGradientSelector::OGradientSelector( QWidget *parent, const char *name )
    : OSelector( parent, name )
{
    init();
}


OGradientSelector::OGradientSelector( Orientation o, QWidget *parent,
		const char *name )
	: OSelector( o, parent, name )
{
    init();
}


OGradientSelector::~OGradientSelector()
{}


void OGradientSelector::init()
{
    color1.setRgb( 0, 0, 0 );
    color2.setRgb( 255, 255, 255 );
    
    text1 = text2 = "";
}


void OGradientSelector::drawContents( QPainter *painter )
{
	QImage image( contentsRect().width(), contentsRect().height(), 32 );

	QColor col;
	float scale;

	int redDiff   = color2.red() - color1.red();
	int greenDiff = color2.green() - color1.green();
	int blueDiff  = color2.blue() - color1.blue();

	if ( orientation() == Vertical )
	{
		for ( int y = 0; y < image.height(); y++ )
		{
			scale = 1.0 * y / image.height();
			col.setRgb( color1.red() + int(redDiff*scale),
						color1.green() + int(greenDiff*scale),
						color1.blue() + int(blueDiff*scale) );

			unsigned int *p = (uint *) image.scanLine( y );
			for ( int x = 0; x < image.width(); x++ )
				*p++ = col.rgb();
		}
	}
	else
	{
		unsigned int *p = (uint *) image.scanLine( 0 );

		for ( int x = 0; x < image.width(); x++ )
		{
			scale = 1.0 * x / image.width();
			col.setRgb( color1.red() + int(redDiff*scale),
						color1.green() + int(greenDiff*scale),
						color1.blue() + int(blueDiff*scale) );
			*p++ = col.rgb();
		}

		for ( int y = 1; y < image.height(); y++ )
			memcpy( image.scanLine( y ), image.scanLine( y - 1),
				 sizeof( unsigned int ) * image.width() );
	}

	QColor ditherPalette[8];

	for ( int s = 0; s < 8; s++ )
		ditherPalette[s].setRgb( color1.red() + redDiff * s / 8,
								color1.green() + greenDiff * s / 8,
								color1.blue() + blueDiff * s / 8 );

	OImageEffect::dither( image, ditherPalette, 8 );

	QPixmap p;
	p.convertFromImage( image );

	painter->drawPixmap( contentsRect().x(), contentsRect().y(), p );

	if ( orientation() == Vertical )
	{
		int yPos = contentsRect().top() + painter->fontMetrics().ascent() + 2;
		int xPos = contentsRect().left() + (contentsRect().width() -
			 painter->fontMetrics().width( text2 )) / 2;
		QPen pen( color2 );
		painter->setPen( pen );
		painter->drawText( xPos, yPos, text2 );

		yPos = contentsRect().bottom() - painter->fontMetrics().descent() - 2;
		xPos = contentsRect().left() + (contentsRect().width() - 
			painter->fontMetrics().width( text1 )) / 2;
		pen.setColor( color1 );
		painter->setPen( pen );
		painter->drawText( xPos, yPos, text1 );
	}
	else
	{
		int yPos = contentsRect().bottom()-painter->fontMetrics().descent()-2;

		QPen pen( color2 );
		painter->setPen( pen );
		painter->drawText( contentsRect().left() + 2, yPos, text1 );

		pen.setColor( color1 );
		painter->setPen( pen );
		painter->drawText( contentsRect().right() -
			 painter->fontMetrics().width( text2 ) - 2, yPos, text2 );
	}
}

//-----------------------------------------------------------------------------

static QColor *standardPalette = 0;

#define STANDARD_PAL_SIZE 17

OColor::OColor()
: QColor()
{
  r = 0; g = 0; b = 0; h = 0; s = 0; v = 0;
};

OColor::OColor( const OColor &col)
: QColor( col )
{
  h = col.h; s = col.s; v = col.v;
  r = col.r; g = col.g; b = col.b;
};

OColor::OColor( const QColor &col)
: QColor( col )
{
  QColor::rgb(&r, &g, &b);
  QColor::hsv(&h, &s, &v);
};

bool OColor::operator==(const OColor& col) const
{
  return (h == col.h) && (s == col.s) && (v == col.v) &&
         (r == col.r) && (g == col.g) && (b == col.b);
}

OColor& OColor::operator=(const OColor& col)
{
  *(QColor *)this = col;
  h = col.h; s = col.s; v = col.v;
  r = col.r; g = col.g; b = col.b;
  return *this;
}

void
OColor::setHsv(int _h, int _s, int _v)
{
  h = _h; s = _s; v = _v;
  QColor::setHsv(h, s, v);
  QColor::rgb(&r, &g, &b);
};

void
OColor::setRgb(int _r, int _g, int _b)
{
  r = _r; g = _g; b = _b;
  QColor::setRgb(r, g, b);
  QColor::hsv(&h, &s, &v);
}

void
OColor::rgb(int *_r, int *_g, int *_b) const
{
  *_r = r; *_g = g; *_b = b;
}

void
OColor::hsv(int *_h, int *_s, int *_v) const
{
  *_h = h; *_s = s; *_v = v;
}

static void createStandardPalette()
{
    if ( standardPalette )
	return;

    standardPalette = new QColor[STANDARD_PAL_SIZE];

    int i = 0;

    standardPalette[i++] = Qt::red;
    standardPalette[i++] = Qt::green;
    standardPalette[i++] = Qt::blue;
    standardPalette[i++] = Qt::cyan;
    standardPalette[i++] = Qt::magenta;
    standardPalette[i++] = Qt::yellow;
    standardPalette[i++] = Qt::darkRed;
    standardPalette[i++] = Qt::darkGreen;
    standardPalette[i++] = Qt::darkBlue;
    standardPalette[i++] = Qt::darkCyan;
    standardPalette[i++] = Qt::darkMagenta;
    standardPalette[i++] = Qt::darkYellow;
    standardPalette[i++] = Qt::white;
    standardPalette[i++] = Qt::lightGray;
    standardPalette[i++] = Qt::gray;
    standardPalette[i++] = Qt::darkGray;
    standardPalette[i++] = Qt::black;
}


OHSSelector::OHSSelector( QWidget *parent, const char *name )
	: OXYSelector( parent, name )
{
	setRange( 0, 0, 359, 255 );
}

void OHSSelector::updateContents()
{
	drawPalette(&pixmap);
}

void OHSSelector::resizeEvent( QResizeEvent * )
{
	updateContents();
}

void OHSSelector::drawContents( QPainter *painter )
{
	painter->drawPixmap( contentsRect().x(), contentsRect().y(), pixmap );
}

void OHSSelector::drawPalette( QPixmap *pixmap )
{
	int xSize = contentsRect().width(), ySize = contentsRect().height();
	QImage image( xSize, ySize, 32 );
	QColor col;
	int h, s;
	uint *p;

	for ( s = ySize-1; s >= 0; s-- )
	{
		p = (uint *) image.scanLine( ySize - s - 1 );
		for( h = 0; h < xSize; h++ )
		{
			col.setHsv( 359*h/(xSize-1), 255*s/(ySize-1), 192 );
			*p = col.rgb();
			p++;
		}
	}

	if ( QColor::numBitPlanes() <= 8 )
	{
		createStandardPalette();
		OImageEffect::dither( image, standardPalette, STANDARD_PAL_SIZE );
	}
	pixmap->convertFromImage( image );
}


//-----------------------------------------------------------------------------

OValueSelector::OValueSelector( QWidget *parent, const char *name )
	: OSelector( OSelector::Vertical, parent, name ), _hue(0), _sat(0)
{
	setRange( 0, 255 );
	pixmap.setOptimization( QPixmap::BestOptim );
}

OValueSelector::OValueSelector(Orientation o, QWidget *parent, const char *name
 )
	: OSelector( o, parent, name), _hue(0), _sat(0)
{
	setRange( 0, 255 );
	pixmap.setOptimization( QPixmap::BestOptim );
}

void OValueSelector::updateContents()
{
	drawPalette(&pixmap);
}

void OValueSelector::resizeEvent( QResizeEvent * )
{
	updateContents();
}

void OValueSelector::drawContents( QPainter *painter )
{
	painter->drawPixmap( contentsRect().x(), contentsRect().y(), pixmap );
}

void OValueSelector::drawPalette( QPixmap *pixmap )
{
	int xSize = contentsRect().width(), ySize = contentsRect().height();
	QImage image( xSize, ySize, 32 );
	QColor col;
	uint *p;
	QRgb rgb;

	if ( orientation() == OSelector::Horizontal )
	{
		for ( int v = 0; v < ySize; v++ )
		{
			p = (uint *) image.scanLine( ySize - v - 1 );

			for( int x = 0; x < xSize; x++ )
			{
				col.setHsv( _hue, _sat, 255*x/(xSize-1) );
				rgb = col.rgb();
				*p++ = rgb;
			}
		}
	}

	if( orientation() == OSelector::Vertical )
	{
		for ( int v = 0; v < ySize; v++ )
		{
			p = (uint *) image.scanLine( ySize - v - 1 );
			col.setHsv( _hue, _sat, 255*v/(ySize-1) );
			rgb = col.rgb();
			for ( int i = 0; i < xSize; i++ )
				*p++ = rgb;
		}
	}

	if ( QColor::numBitPlanes() <= 8 )
	{
		createStandardPalette();
		OImageEffect::dither( image, standardPalette, STANDARD_PAL_SIZE );
	}
	pixmap->convertFromImage( image );
}
