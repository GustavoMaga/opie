/*
    OpieIRC - An embedded IRC client
    Copyright (C) 2002 Wenzel Jakob

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef __IRCSETTINGS_H
#define __IRCSETTINGS_H

#include <qpe/config.h>
#include <qdialog.h>
#include <qlineedit.h>

namespace Opie {
class OColorButton;
namespace Ui {
class OKeyConfigWidget;
}
}

class QCheckBox;
class IRCSettings : public QDialog {
    Q_OBJECT
public:
    IRCSettings(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags f = 0);
    ~IRCSettings();
protected slots:
    void accept();
protected:
    Config       *m_config;
    Opie::OColorButton *m_background;
    Opie::OColorButton *m_text;
    Opie::OColorButton *m_error;
    Opie::OColorButton *m_self;
    Opie::OColorButton *m_server;
    Opie::OColorButton *m_other;
    Opie::OColorButton *m_notification;
    Opie::Ui::OKeyConfigWidget *m_keyConf;
    QLineEdit    *m_lines;
    QCheckBox *m_displayTime;
};

#endif /* __IRCSETTINGS_H */
