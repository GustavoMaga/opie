/*
 * addresspluginconfig.h
 *
 * copyright   : (c) 2003 by Stefan Eilers
 * email       : eilers.stefan@epost.de
 *
 * This implementation was derived from the todolist plugin implementation
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

#ifndef ADDRESSBOOK_PLUGIN_CONFIG_H
#define ADDRESSBOOK_PLUGIN_CONFIG_H

#include <opie2/todayconfigwidget.h>
#include <opie2/ocolorbutton.h>

#include <qwidget.h>
#include <qspinbox.h>
#include <qcheckbox.h>

class AddressBookPluginConfig : public TodayConfigWidget {
    Q_OBJECT

public:

    AddressBookPluginConfig( QWidget *parent,  const char *name );
    ~AddressBookPluginConfig();

private:
    /**
     * if changed then save
     */
    bool changed();
    void readConfig();
    void writeConfig();

    QSpinBox* SpinBox2;
    QSpinBox* SpinBoxClip;
    QSpinBox* SpinDaysClip;
    QSpinBox* SpinUrgentClip;
    Opie::OColorButton* entryColor;
    Opie::OColorButton* headlineColor;
    Opie::OColorButton* urgentColor;
    QCheckBox* m_showBirthdayButton;
    QCheckBox* m_showAnniversaryButton;

    // how many lines should be showed in the AddressBook section
    int m_max_lines_task;
    // clip the lines after X chars
    int m_maxCharClip;
    // How many days look ahead
    int m_daysLookAhead;
    // Days until urgent color is used
    int m_urgentDays;
    // Entry Color
    QString m_entryColor;
    // Headline Color
    QString m_headlineColor;
    // Urgent Color
    QString m_urgentColor;
};





#endif
