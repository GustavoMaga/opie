
/*
 * datebookplugin.cpp
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



#include "datebookevent.h"
#include "datebookplugin.h"
#include "datebookpluginwidget.h"
#include "datebookpluginconfig.h"


DatebookPlugin::DatebookPlugin()  {
}

DatebookPlugin::~DatebookPlugin() {
}

QString DatebookPlugin::pluginName() const {
    return QObject::tr( "Datebook plugin");
}

double DatebookPlugin::versionNumber() const {
    return 1.0;
}

QString DatebookPlugin::pixmapNameWidget() const {
    return "DateBook";
}

QWidget* DatebookPlugin::widget( QWidget* wid ) {
    return new DatebookPluginWidget( wid,  "Datebook" );
}

QString DatebookPlugin::pixmapNameConfig() const {
    return "DateBook";
}

TodayConfigWidget* DatebookPlugin::configWidget( QWidget* wid ) {
    return new DatebookPluginConfig( wid , "Datebook" );
}

QString DatebookPlugin::appName()  const {
    return "datebook";
}

