/*  This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)
    Copyright (C) 1999 Cristian Tibirna (ctibirna@kde.org)

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

#include <qdialog.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qapplication.h>
#include <kglobalsettings.h>
#include "kcolorbtn.h"

#include <opie2/qcolordialog.h>

KColorButton::KColorButton( QWidget *parent, const char *name )
  : QPushButton( parent, name ), dragFlag(false)
{
  // 2000-10-15 (putzer): fixes broken keyboard usage
  connect (this, SIGNAL(clicked()), this, SLOT(chooseColor()));
}

KColorButton::KColorButton( const QColor &c, QWidget *parent,
          const char *name )
  : QPushButton( parent, name ), col(c), dragFlag(false)
{

  // 2000-10-15 (putzer): fixes broken keyboard usage
  connect (this, SIGNAL(clicked()), this, SLOT(chooseColor()));
}

void KColorButton::setColor( const QColor &c )
{
  col = c;
  repaint( false );
}

void KColorButton::drawButtonLabel( QPainter *painter )
{
  QRect r = QApplication::style().buttonRect( 0, 0, width(), height() );
  int l = r.x();
  int t = r.y();
  int w = r.width();
  int h = r.height();
  int b = 5;

  QColor lnCol = colorGroup().text();
  QColor fillCol = isEnabled() ? col : backgroundColor();

  if ( isDown() ) {
    qDrawPlainRect( painter, l+b+1, t+b+1, w-b*2, h-b*2, lnCol, 1, 0 );
    b++;
    painter->fillRect( l+b+1, t+b+1, w-b*2, h-b*2, fillCol );
  } else {
    qDrawPlainRect( painter, l+b, t+b, w-b*2, h-b*2, lnCol, 1, 0 );
    b++;
    painter->fillRect( l+b, t+b, w-b*2, h-b*2, fillCol );
  }
}

void KColorButton::chooseColor()
{
  QColor newCol=QColorDialog::getColor( col);
  if( newCol != QDialog::Rejected )
  {
  col=newCol;
  repaint( false );
  emit changed( col );
  }
}
