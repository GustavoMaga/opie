/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef OPIM_XREF_H
#define OPIM_XREF_H

#include <qarray.h>
#include <qvaluelist.h>

#include <opie2/opimxrefpartner.h>

namespace Opie {
/**
 * this is a Cross Referecne between
 * two Cross Reference Partners
 */
class OPimXRef {
public:
    typedef QValueList<OPimXRef> ValueList;
    enum Partners { One, Two };
    OPimXRef( const OPimXRefPartner& ONE, const OPimXRefPartner& );
    OPimXRef();
    OPimXRef( const OPimXRef& );
    ~OPimXRef();

    OPimXRef &operator=( const OPimXRef& );
    bool operator==( const OPimXRef& );

    OPimXRefPartner partner( enum Partners )const;

    void setPartner( enum Partners,  const OPimXRefPartner& );

    bool containsString( const QString& service)const;
    bool containsUid( int uid )const;

private:
    QArray<OPimXRefPartner> m_partners;

    class Private;
    Private *d;
};

}

#endif
