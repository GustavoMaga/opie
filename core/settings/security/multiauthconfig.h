/**
 * \file multiauthconfig.h
 * \brief Configuration GUI for Opie multiauth. framework, login and sync
 * \author Cl�ment S�veillac (clement . seveillac (at) via . ecp . fr)
 */
/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can 
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.        
    .i_,=:_.      -<s.       This library is distributed in the hope that  
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-        
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB. 
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef MULTIAUTHCONFIG_H
#define MULTIAUTHCONFIG_H

#include <opie2/multiauthplugininterface.h>
#include <opie2/multiauthcommon.h>

/* OPIE */
#include <opie2/otabwidget.h>

/* QT */
#include <qdialog.h>
#include <qwidget.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qpe/config.h>
#include <qlistview.h>
#include <qmap.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qtextstream.h>

/* UI */
#include "syncbase.h"
#include "loginbase.h"


/// the "misc" configuration tab, about general Opie Multiauth settings
class MultiauthGeneralConfig : public QWidget
{
    Q_OBJECT
public:
    MultiauthGeneralConfig(QWidget * parent, const char * name);
    ~MultiauthGeneralConfig();
protected:
    QCheckBox *onStart, *onResume, *noProtectConfig, *explanScreens, *allowBypass;
    QSpinBox *nbSuccessMin;
private:
    friend class MultiauthConfig;
private slots:
    void checkBypass();
    void checkScreens();
};

/// the whole configuration dialog
class MultiauthConfig : public QDialog
{
    Q_OBJECT
    
public:
    static QString appName() { return QString::fromLatin1("security"); }
    MultiauthConfig(QWidget *parent, const char* name, WFlags fl);
    virtual ~MultiauthConfig();
    void writeConfig();
    QList<Opie::Security::MultiauthConfigWidget> configWidgetList;
    
protected slots:
    void accept();
    void done(int r); 
    void pluginsChanged();
    void moveSelectedUp();
    void moveSelectedDown();

private slots:
    // Login and Sync stuff
    void setSyncNet(const QString&);
    void changeLoginName(int);
    void toggleAutoLogin(bool);
    void restoreDefaults();
    void insertDefaultRanges();
    void deleteListEntry();

private:
    /// the widget holding all the tabs (or pages)
    Opie::Ui::OTabWidget *m_mainTW;
    /// list of authentication plugins in the "Plugins" page
    QListView *m_pluginListView;
    QStringList m_allPlugins, m_excludePlugins;
    QMap<QString,QCheckListItem*> m_plugins;
    /// plugin list page
    QWidget *m_pluginListWidget;
    /// misc config page
    MultiauthGeneralConfig *m_generalConfig;
    /// login (root / ...) choice page
    LoginBase *m_loginWidget;
    /// synchronization settings page
    SyncBase *m_syncWidget;

    int m_nbSuccessReq;
    bool m_plugins_changed;
    
    void readConfig();
    void loadPlugins();

    // Login and Sync stuff
    void loadUsers();
    bool telnetAvailable() const;
    bool sshAvailable() const;
    void updateGUI();

    static void parseNet(const QString& sn,int& auth_peer,int& auth_peer_bits);
    void selectNet(int auth_peer,int auth_peer_bits,bool update);


    bool autoLogin;
    QString autoLoginName;
};


#endif // MULTIAUTHCONFIG_H

