/*
 * cardmon.h
 *
 * ---------------------
 *
 * copyright   : (c) 2002 by Maximilian Reiss
 * email       : max.reiss@gmx.de
 * based on two apps by Devin Butterfield
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CARDMON_H
#define CARDMON_H

#include <qwidget.h>
#include <qpixmap.h>

class CardMonitor : public QWidget {
    Q_OBJECT
public:
    CardMonitor( QWidget *parent = 0 );
    ~CardMonitor();
    bool getStatusPcmcia( int showPopUp = FALSE );
    bool getStatusSd( int showPopUp = FALSE );

private slots:
    void cardMessage( const QCString &msg, const QByteArray & );

protected:
    void paintEvent( QPaintEvent* );
    void mousePressEvent( QMouseEvent * );
private:
    QPixmap pm;
    // pcmcia socket 0
    bool cardInPcmcia0;
    QString cardInPcmcia0Name;
    // pcmcia socket 1
    bool cardInPcmcia1;
    QString cardInPcmcia1Name;
    bool cardInSd;
    void iconShow();

};

#endif

