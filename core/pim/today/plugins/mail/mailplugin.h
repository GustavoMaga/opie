/*
 * mailplugin.h
 *
 * copyright   : (c) 2002 by Maximilian Rei�
 * email       : harlekin@handhelds.org
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef MAIL_PLUGIN_H
#define MAIL_PLUGIN_H

#include <qstring.h>
#include <qwidget.h>

#include <opie/tododb.h>
#include <opie/oclickablelabel.h>

#include "../../todayplugininterface.h"

class MailPlugin : public TodayPluginObject {

public:
    MailPlugin();
    ~MailPlugin();

    QString pluginName()  const;
    double versionNumber() const;
    QString pixmapNameWidget() const;
    QWidget* widget(QWidget *);
    QString pixmapNameConfig() const;
    QWidget* configWidget(QWidget *);
    QString appName() const;
    virtual int minHeight()  const;
    virtual int maxHeight()  const;

};

#endif
