
#ifndef DATEBOOK_PLUGIN_CONFIG_H
#define DATEBOOK_PLUGIN_CONFIG_H

#include <qwidget.h>
#include <qcheckbox.h>
#include <qspinbox.h>

#include "../../configwidget.h"

class DatebookPluginConfig : public ConfigWidget {

    Q_OBJECT

public:
    DatebookPluginConfig( QWidget *parent,  const char *name );
    ~DatebookPluginConfig();

    void writeConfig();
private:
    /**
     * if changed then save
     */
    bool changed();
    void readConfig();

    QCheckBox* CheckBox2;
    QCheckBox* CheckBox1;
    QCheckBox* CheckBox3;
    QSpinBox* SpinBox1;

    // how many lines should be showed in the datebook section
    int m_max_lines_meet;
    // If location is to be showed too, 1 to activate it.
    int m_show_location;
    // if notes should be shown
    int m_show_notes;
    // should only later appointments be shown or all for the current day.
    int m_only_later;


};





#endif
