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
#ifndef _OXYFRAME_H
#define _OXYFRAME_H


#include <qlabel.h>

class OxyFrame : public QLabel
{
	Q_OBJECT
	
    public:
        OxyFrame(  QWidget *parent=0, const char *name=0, QString symbol="" );

        void mousePressEvent( QMouseEvent *);

        QString N;

    signals:
        /*
         * this signal emits the name (the element-number) 
         */
        void num(QString);
};

#endif
