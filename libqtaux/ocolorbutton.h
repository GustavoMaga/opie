/*
                             This file is part of the Opie Project
                             Copyright (C) Robert Griebl <sandman@handhelds.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
     .>+-=
_;:,   .>  :=|.         This program is free software; you can
.> <`_,  > .  <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU Library General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This program is distributed in the hope that
  + . -:.    =       it will be useful,  but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=    =    ;      Library General Public License for more
++=  -.   .`   .:       details.
:   = ...= . :.=-
-.  .:....=;==+<;          You should have received a copy of the GNU
 -_. . .  )=. =           Library General Public License along with
  --    :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef OCOLORBUTTON_H
#define OCOLORBUTTON_H

/* QT*/
#include <qpushbutton.h>

class OColorButtonPrivate;
class QColor;

namespace Opie
{

/**
 *
 * @short A Button which will show a OColorPopupMenu
 * @author Robert Griebl ( sandman@handhelds.org )
 * @version 1.0
 * @see QPushButton
 */
class OColorButton : public QPushButton
{
    Q_OBJECT
public:
    OColorButton ( QWidget *parent = 0, const QColor & = Qt::black );
    virtual ~OColorButton ( );

    QColor color ( ) const;

protected:
    void resizeEvent( QResizeEvent* );

signals:
    /**
     * emitted when a color gets selected
     */
    void colorSelected ( const QColor & );

public slots:
    virtual void setColor ( const QColor & );

protected slots:
    /**
     * @internal
     */
    virtual void updateColor ( const QColor & );

private:
    OColorButtonPrivate *d;

    void updateColor ( const QColor &, bool sendSignal );
};

};

#endif

