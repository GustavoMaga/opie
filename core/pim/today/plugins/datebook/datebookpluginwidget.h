/*
 * datebookpluginwidget.h
 *
 * copyright   : (c) 2002,2003 by Maximilian Rei�
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

#ifndef DATEBOOK_PLUGIN_WIDGET_H
#define DATEBOOK_PLUGIN_WIDGET_H

#include <qlayout.h>
#include <qguardedptr.h>
#include <qlist.h>

#include <qpe/datebookdb.h>
#include <opie/oclickablelabel.h>

#include "datebookevent.h"

class DatebookPluginWidget :  public QWidget {

    Q_OBJECT

public:
    DatebookPluginWidget( QWidget *parent,  const char *name );
    ~DatebookPluginWidget();
    void refresh();

private:
    DateBookDB* db;
    QGuardedPtr<QVBoxLayout> m_layoutDates;
    QList<QWidget> m_eventsList;
    void readConfig();
    void getDates();

    
    // how many lines should be showed in the datebook section
    int m_max_lines_meet;
    // If location is to be showed too, 1 to activate it.
    bool m_show_location;
    // if notes should be shown
    bool m_show_notes;
    // should only later appointments be shown or all for the current day.
    bool m_onlyLater;
    int m_moreDays;

};

#endif
