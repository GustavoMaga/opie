/**
 * \file noticeplugin.h
 * \brief Standard Opie multiauth plugin definition
 * \author Cl�ment S�veillac (clement . seveillac (at) via . ecp . fr)
 */
/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@lists.sourceforge.net>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
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

#ifndef NOTICE_PLUGIN_H
#define NOTICE_PLUGIN_H

#include <qstring.h>
#include <qpe/config.h>
#include <opie2/multiauthplugininterface.h>
#include "noticeConfigWidget.h"

/// Multi-authentication plugin, having the user accept a (legal, etc.) notice text.
/**
 * The plugin itself, implementing the main authenticate() function.
 */
class NoticePlugin : public QObject, public Opie::Security::MultiauthPluginObject {

    Q_OBJECT;

public:
    NoticePlugin();
    virtual ~NoticePlugin();
    int authenticate();
    Opie::Security::MultiauthConfigWidget * configWidget(QWidget * parent);
    QString pixmapNameConfig() const;
    QString pixmapNameWidget() const;
    QString pluginName() const;
private:
    NoticeConfigWidget * m_noticeW;
    QString getNoticeText();
};

#endif
