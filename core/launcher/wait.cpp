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

#include "wait.h"

#include <opie2/owait.h>

#include <qtopia/config.h>
#include <qtopia/applnk.h>

Wait *lastWaitObject = NULL;


using namespace Opie::Ui;
Wait::Wait( QWidget *parent ) : QWidget( parent ),
                                waiting( FALSE )
{

    QSize size( AppLnk::smallIconSize(), AppLnk::smallIconSize() );
    setFixedSize( size );

    QImage img = Resource::loadImage( "wait" );
    img = img.smoothScale( size.width(), size.height() );
    pm.convertFromImage( img );

    lastWaitObject = this;
    m_centralWait = new OWait( 0l );
    m_centralWait->hide();
    hide();
}


Wait *Wait::getWaitObject()
{
    return lastWaitObject;
}


void Wait::setWaiting( bool w )
{
    Config cfg ( "Launcher" );
    cfg.setGroup("GUI");


    waiting = w;
    if ( w ) {
        if ( cfg. readBoolEntry( "BigBusy" ) )
            m_centralWait->show();
        else
            show();
    }else{
        m_centralWait->hide();
	hide();
    }
}


void Wait::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    p.drawPixmap( 0, 0, pm );
}


