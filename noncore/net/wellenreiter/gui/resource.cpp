/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
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

#include "resource.h"

#define PIXMAPPATH "/usr/local/share"

#include <qiconset.h>

namespace Resource
{

QPixmap loadPixmap( const QString& pix )
{
    QString filename;
    filename.sprintf( "%s/%s.png", PIXMAPPATH, pix.local8Bit() );
    QPixmap pixmap( filename );
    if ( pixmap.isNull() )
    {
        odebug << "Wellenreiter::Resource: can't find pixmap " + filename << oendl;
    }
    return pixmap;
};

QIconSet loadIconSet( const QString& pix )
{
    QString filename;
    filename.sprintf( "%s/%s.png", PIXMAPPATH, pix.local8Bit() );
    QPixmap pixmap( filename );
    if ( pixmap.isNull() )
    {
        odebug << "Wellenreiter::Resource: can't find pixmap " + filename << oendl;
    }
    return QIconSet( pixmap );
};

};
