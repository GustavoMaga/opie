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

#include <opie/todayplugininterface.h>
#include <opie/todayconfigwidget.h>

#include "mailpluginwidget.h"

class MailPlugin : public TodayPluginObject {

public:
    MailPlugin();
    ~MailPlugin();

    QString pluginName()  const;
    double versionNumber() const;
    QString pixmapNameWidget() const;
    QWidget* widget(QWidget *);
    QString pixmapNameConfig() const;
    TodayConfigWidget* configWidget(QWidget *);
    QString appName() const;
    bool excludeFromRefresh() const;
    void refresh();

 private:
    MailPluginWidget *m_widget;
};

#endif
