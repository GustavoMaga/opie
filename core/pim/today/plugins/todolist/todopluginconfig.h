/*
 * todopluginconfig.h
 *
 * copyright   : (c) 2002, 2003 by Maximilian Rei�
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

#ifndef TODOLIST_PLUGIN_CONFIG_H
#define TODOLIST_PLUGIN_CONFIG_H

//#include <qwidget.h>
#include <qspinbox.h>

#include <opie/todayconfigwidget.h>

class TodolistPluginConfig : public TodayConfigWidget {


public:

    TodolistPluginConfig( QWidget *parent,  const char *name );
    ~TodolistPluginConfig();

private:
    /**
     * if changed then save
     */
    bool changed();
    void readConfig();
    void writeConfig();

    QSpinBox* SpinBox2;
    QSpinBox* SpinBoxClip;

    // how many lines should be showed in the todolist section
    int m_max_lines_task;
    // clip the lines after X chars
    int m_maxCharClip;



};





#endif
