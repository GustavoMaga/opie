/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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
#include "irda.h"
#include "irdaappletimpl.h"


IrdaAppletImpl::IrdaAppletImpl()
    : irda(0)
{
}

IrdaAppletImpl::~IrdaAppletImpl()
{
    delete irda;
}

QWidget *IrdaAppletImpl::applet( QWidget *parent )
{
    if ( !irda )
  irda = new IrdaApplet( parent );
    return irda;
}

int IrdaAppletImpl::position() const
{
    return 6;
}

QRESULT IrdaAppletImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
  *iface = this;
    else if ( uuid == IID_TaskbarApplet )
  *iface = this;
    else
	return QS_FALSE;
	
    if ( *iface )
  (*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( IrdaAppletImpl )
}


