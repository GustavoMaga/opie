/**********************************************************************
** Copyright (C) 2004 Anton Kachalov mouse@altlinux.ru
**  All rights reserved.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/
#ifndef __MULTIKEYAPPLETIMPL_H__
#define __MULTIKEYAPPLETIMPL_H__

#include <qpe/taskbarappletinterface.h>

class Multikey;

class MultikeyAppletImpl : public TaskbarAppletInterface
{
public:
    MultikeyAppletImpl();
    virtual ~MultikeyAppletImpl();

#ifndef QT_NO_COMPONENT
    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT
#endif

    virtual QWidget *applet( QWidget *parent );
    virtual int position() const;

private:
    Multikey *kbd;
};

#endif /* __MULTIKEYAPPLETIMPL_H__ */
