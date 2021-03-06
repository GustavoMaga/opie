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
#ifndef OPIE_LIQUIDDECO_H__
#define OPIE_LIQUIDDECO_H__

#include <qpe/qpedecoration_qws.h>


class LiquidDecoration : public QPEDecoration
{
public:
    LiquidDecoration();
    virtual ~LiquidDecoration();

    virtual void paint(QPainter *, const QWidget *);
    virtual void paintButton(QPainter *, const QWidget *, Region, int state);

protected:
    virtual int getTitleHeight(const QWidget *);
};



#endif // OPIE_LIQUIDDECO_H__
