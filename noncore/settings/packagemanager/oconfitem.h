/*
                    This file is part of the Opie Project

                      Copyright (c)  2003 Dan Williams <drw@handhelds.org>
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

#ifndef OCONFITEM_H
#define OCONFITEM_H

#include <qlist.h>
#include <qstring.h>

class OConfItem
{
public:
    enum Type { Source=0x01, 
                GzSource=0x02,
                Destination=0x04, 
                Option=0x08,
                Arch=0x10,
                NotDefined=0x20 };

    OConfItem( Type type = NotDefined, const QString &name = QString::null,
               const QString &value = QString::null, const QString &features = QString::null,
               bool active = true );

    Type           type()     { return m_type; }
    const QString &name()     { return m_name; }
    const QString &value()    { return m_value; }
    const QString &features() { return m_features; }
    bool           active()   { return m_active; }

    void setType( Type type )                   { m_type = type; }
    void setName( const QString &name )         { m_name = name; }
    void setValue( const QString &value )       { m_value = value; }
    void setFeatures( const QString &features ) { m_features = features; }
    void setActive( bool active )               { m_active = active; }

private:
    Type    m_type;     // Type of configuration item
    QString m_name;     // Name of item
    QString m_value;    // Value of item
    QString m_features; // Comma-deliminated list of features this item supports
    bool    m_active;   // Indicates whether item is currently active
};

class OConfItemList : public QList<OConfItem>
{
private:

    int compareItems( QCollection::Item item1, QCollection::Item item2 )
    {
        // Sort by OConfItem location then by type
        OConfItem::Type type1 = reinterpret_cast<OConfItem*>(item1)->type();
        OConfItem::Type type2 = reinterpret_cast<OConfItem*>(item2)->type();
        if ( type1 < type2 )
            return -1;
        else if ( type1 == type2 )
        {
            QString name1 = reinterpret_cast<OConfItem*>(item1)->name();
            QString name2 = reinterpret_cast<OConfItem*>(item2)->name();
            if ( name1 < name2 )
                return -1;
            else if ( name1 == name2 )
                return 0;
            else /*if ( name1 > name2 )*/
                return 1;
        }
        else /*if ( type1 > type2 )*/
            return 1;
    }
};

typedef QListIterator<OConfItem> OConfItemListIterator;

#endif
