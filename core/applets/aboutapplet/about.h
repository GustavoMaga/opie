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
#ifndef __OPIE_ABOUT_APPLET_H__
#define __OPIE_ABOUT_APPLET_H__

#include <qpe/menuappletinterface.h>
#include <qobject.h>

class AboutApplet : public QObject, public MenuAppletInterface
{
public:
    AboutApplet ( );
    virtual ~AboutApplet ( );

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual int position() const;

    virtual QString name ( ) const;
    virtual QIconSet icon ( ) const;
    virtual QString text ( ) const;
    virtual QString tr( const char* ) const;
    virtual QString tr( const char*, const char* ) const;
    virtual QPopupMenu *popup ( QWidget *parent ) const;

    virtual void activated ( );

};

#endif
