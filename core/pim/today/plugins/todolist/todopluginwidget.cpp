/*
 * todopluginwidget.cpp
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

#include "todopluginwidget.h"

#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>

TodolistPluginWidget::TodolistPluginWidget( QWidget *parent,  const char* name )
    : QWidget( parent, name ) {

    todo = 0l;
    layoutTodo = 0l;
    todoLabel = 0l;

    if ( todo ) {
        delete todo;
    }
    todo = new OTodoAccess();
    todo->load();

    if ( layoutTodo ) {
        delete layoutTodo;
    }
    layoutTodo = new QVBoxLayout( this );
    layoutTodo->setAutoAdd( true );

    if ( todoLabel )  {
        delete todoLabel;
    }
    todoLabel = new OClickableLabel( this );

    connect( todoLabel, SIGNAL( clicked() ), this, SLOT( startTodolist() ) );

    readConfig();
    getTodo();
}

TodolistPluginWidget::~TodolistPluginWidget() {
    delete todo;
    delete todoLabel;
    delete layoutTodo;
}


void TodolistPluginWidget::readConfig() {
    Config cfg( "todaytodoplugin" );
    cfg.setGroup( "config" );
    m_maxLinesTask = cfg.readNumEntry( "maxlinestask", 5 );
    m_maxCharClip =  cfg.readNumEntry( "maxcharclip", 38 );
}

void TodolistPluginWidget:: refresh()  {
    getTodo();
}

void TodolistPluginWidget::reinitialize()  {
    readConfig();
    getTodo();
}

/**
 * Get the todos
 */
void TodolistPluginWidget::getTodo() {


    QString output;
    QString tmpout;
    int count = 0;
    int ammount = 0;

    // get overdue todos first
    m_list = todo->overDue();

    for ( m_it = m_list.begin(); m_it != m_list.end(); ++m_it ) {
        if (!(*m_it).isCompleted() && ( ammount < m_maxLinesTask ) ) {
            QString desc = (*m_it).summary();
            if( desc.isEmpty() ) {
                desc = (*m_it).description();
            }
            tmpout += "<font color=#e00000><b>-" + desc.mid( 0, m_maxCharClip ) + "</b></font><br>";
            ammount++ ;
        }
    }

    // get total number of still open todos
    m_list = todo->sorted( true, 1, 4, 1);

    for ( m_it = m_list.begin(); m_it != m_list.end(); ++m_it ) {
        count +=1;
        // not the overdues, we allready got them, and not if we are
        // over the maxlines
        if ( !(*m_it).isOverdue() && ( ammount < m_maxLinesTask ) ) {
            QString desc = (*m_it).summary();
            if( desc.isEmpty() ) {
                desc = (*m_it).description();
            }
            tmpout += "<b>-</b>" + desc.mid( 0, m_maxCharClip ) + "<br>";
            ammount++;
        }
    }

    if ( count > 0 ) {
        if( count == 1 ) {
            output += QObject::tr( "There is <b> 1</b> active task:  <br>" );
        } else {
            output += QObject::tr( "There are <b> %1</b> active tasks: <br>" ).arg( count );
        }
        output += tmpout;
    } else {
        output = QObject::tr( "No active tasks" );
    }
    todoLabel->setText( output );
}

/**
 * start the todolist
 */
void TodolistPluginWidget::startTodolist() {
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << QString( "todolist" );
}
