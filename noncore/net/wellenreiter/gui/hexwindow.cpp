/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
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

#include "hexwindow.h"
#include <qmultilineedit.h>

MHexWindow::MHexWindow( QWidget * parent, const char * name, WFlags f )
           :QVBox( parent, name, f )
{
    ledit = new QMultiLineEdit( this );
    
    // FIXME: Set properties( font, read-only, etc...)
    
};

void MHexWindow::log( QString text )
{

    ledit->append( text );

};

void MHexWindow::clear()
{
    ledit->clear();
}

