/***************************************************************************
   application:             : Oxygen

   begin                    : September 2002
   copyright                : ( C ) 2002 by Carsten Niehaus
   email                    : cniehaus@handhelds.org
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

#include "oxyframe.h"

OxyFrame::OxyFrame(QWidget *parent, const char *name, QString symbol ) 
  : QLabel(parent,name)
{
    N = name;
    this->setFrameStyle( QFrame::Box );
    this->setLineWidth( 0 );
    this->setMidLineWidth( 1 );
    this->setFrameShadow( QFrame::Sunken );
    setMinimumSize(6,6);
    setScaledContents( true );
    setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    QFont font("helvetica");
    font.setWeight(QFont::Light);
    font.setPixelSize(3);
    setFont( font );
    setText( symbol );
}

void OxyFrame::mousePressEvent (  QMouseEvent* /*e*/ ){
    emit num( N );
};
