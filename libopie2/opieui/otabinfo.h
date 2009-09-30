/*
                    This file is part of the Opie Project

                      Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
              =.
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

#ifndef OTABINFO_H
#define OTABINFO_H

/* QT */
#include <qlist.h>
#include <qstring.h>

class QWidget;

namespace Opie{
namespace Ui  {

/**
 * @class OTabInfo
 * @brief The OTabInfo class is used internally by OTabWidget to keep track
 *        of widgets added to the control.
 *
 * OTabInfo provides the following information about a widget added to an
 * OTabWidget control:
 *
 *  ID - integer tab bar ID
 *  Control - QWidget pointer to child widget
 *  Label - QString text label for OTabWidget selection control
 *  Icon - QString name of icon file
 */
class OTabInfo
{
public:
    /**
     * @fn OTabInfo()
     * @brief Object constructor.
     */
    OTabInfo() : i( -1 ), c( 0 ), p( 0 ), l( QString::null ) {}

    /**
     * @fn OTabInfo( int id, QWidget *control, const QString &icon, const QString &label )
     * @brief Object constructor.
     *
     * @param id TabBar identifier for widget.
     * @param control QWidget pointer to widget.
     * @param icon QString name of icon file.
     * @param label QString text label for OTabWidget selection control.
     */
    OTabInfo( int id, QWidget *control, const QString &icon, const QString &label )
            : i( id ), c( control ), p( icon ), l( label ), d( 0 ) {}

    /**
     * @fn id()const
     * @brief Returns TabBar ID.
     */
    int            id()      const { return i; }

    /**
     * @fn setId( int id )
     * @brief Set Id for tab.
     *
     * @param id integer tab identifier.
     */
    void setId( int id ) { i = id; }

    /**
     * @fn label()const
     * @brief Returns text label for widget.
     */
    const QString &label()   const { return l; }

    /**
     * @fn setLabel( const QString &label )
     * @brief Set label for tab.
     *
     * @param label QString text label for OTabWidget selection control.
     */
    void setLabel( const QString &label ) { l = label; }

    /**
     * @fn control()const
     * @brief Returns pointer to widget.
     */
    QWidget       *control() const { return c; }

    /**
     * @fn icon()const
     * @brief Returns name of icon file.
     */
    const QString &icon()    const { return p; }

    /**
     * @fn setIcon( const QString &icon )
     * @brief Set icon for tab.
     *
     * @param icon QString name of icon file.
     */
    void setIcon( const QString &icon ) { p = icon; }

private:
    int      i;
    QWidget *c;
    QString  p;
    QString  l;
    class Private;
    Private *d;
};

/**
 * @class OTabInfoList
 * @brief A list of OTabInfo objects used by OTabWidget.
 */
typedef QList<OTabInfo> OTabInfoList;

}
}

#endif
