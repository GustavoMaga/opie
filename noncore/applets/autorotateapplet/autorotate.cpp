/*
 * copyright   : (c) 2003 by Greg Gilbert
 * email       : greg@treke.net
 * based on the cardmon applet by Max Reiss
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 *************************************************************************/

#include "autorotate.h"

/* OPIE */
#include <opie2/odevice.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/resource.h>

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qcopchannel_qws.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qtimer.h>
#include <qtextstream.h>

using namespace Opie;

AutoRotate::AutoRotate(QWidget * parent):QWidget(parent)
{
    setFixedWidth( AppLnk::smallIconSize() );
    setFixedHeight( AppLnk::smallIconSize() );

    enabledPm.convertFromImage( Resource::loadImage("autorotate/rotate").smoothScale( height(), width() ) );
    disabledPm.convertFromImage( Resource::loadImage("autorotate/norotate").smoothScale( height(), width() ) );

    repaint(true);
    popupMenu = 0;
    show();
}

AutoRotate::~AutoRotate()
{
    if (popupMenu) {
        delete popupMenu;
    }
}

void AutoRotate::mousePressEvent(QMouseEvent *)
{
    QPopupMenu *menu = new QPopupMenu(this);
    menu->insertItem( isRotateEnabled()? "Disable Rotation" : "Enable Rotation" ,1 );

    QPoint p = mapToGlobal(QPoint(0, 0));
    QSize s = menu->sizeHint();
    int opt = menu->exec(QPoint(p.x() + (width() / 2) - (s.width() / 2), p.y() - s.height()), 0);

    if (opt==1)
    {
        setRotateEnabled( !isRotateEnabled() );
        repaint(true);
    }

    delete menu;
}

void AutoRotate::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap( 0, 0, isRotateEnabled()? enabledPm : disabledPm );
}

void AutoRotate::setRotateEnabled(bool status)
{
    Config cfg( "qpe" );
    cfg.setGroup( "Appearance" );
    cfg.writeEntry( "rotateEnabled", status );

}
bool AutoRotate::isRotateEnabled()
{
    Config cfg( "qpe" );
    cfg.setGroup( "Appearance" );

    bool res = cfg.readBoolEntry( "rotateEnabled" );

    if (res )
        qDebug("Enabled");
    else
        qDebug("Disabled");
    return res;
}

