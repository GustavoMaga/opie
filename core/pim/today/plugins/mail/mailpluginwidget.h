/*
 * mailpluginwidget.h
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


#ifndef MAIL_PLUGIN_WIDGET_H
#define MAIL_PLUGIN_WIDGET_H

#include <qstring.h>
#include <qwidget.h>
#include <qlayout.h>

#include <opie/tododb.h>
#include <opie/oclickablelabel.h>

class MailPluginWidget : public QWidget {

    Q_OBJECT


public:
    MailPluginWidget( QWidget *parent,  const char *name );
    ~MailPluginWidget();
    
    void refresh();

protected slots:
    void startMail();

private:
    OClickableLabel* m_mailLabel;
    QHBoxLayout* m_layout;
    void readConfig();
    void getInfo();
};

#endif
