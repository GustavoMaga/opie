/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef DATEBOOKSETTINGS_H
#define DATEBOOKSETTINGS_H
#include "datebooksettingsbase.h"
#include <qpe/categoryselect.h>
#include <qvaluelist.h>

namespace Opie {
namespace Core {
    class OPluginManager;
    class OGenericPluginLoader;
    class OPluginLoader;
}
namespace Ui {
    class OPluginConfigWidget;
}
namespace Datebook {
    class HolidayPluginConfigWidget;
}
}

class DateBookSettings : public DateBookSettingsBase
{
    Q_OBJECT
public:
    DateBookSettings( bool whichClock, QWidget *parent = 0,
                      const char *name = 0, bool modal = TRUE, WFlags = 0 );
    ~DateBookSettings();
    void setStartTime( int newStartViewTime );
    int startTime() const;
    void setAlarmPreset( bool bAlarm, int presetTime );
    bool alarmPreset() const;
    int presetTime() const;
    void setAlarmType( int alarmType );
    int alarmType() const;

    void setJumpToCurTime( bool bJump );
    bool jumpToCurTime() const;
    void setRowStyle( int style );
    int rowStyle() const;

    void setPluginList(Opie::Core::OPluginManager*,Opie::Core::OPluginLoader*);
    void savePlugins();
private slots:
    void slot12Hour( int );
    void slotChangeClock( bool );
protected slots:
    virtual void pluginItemClicked(QListViewItem *);

protected:
    void init();
    bool ampm;
    int oldtime;
    Opie::Core::OPluginManager*m_manager;
    Opie::Core::OPluginLoader*m_loader;
    QValueList<Opie::Datebook::HolidayPluginConfigWidget*> m_cfgWidgets;
};
#endif
