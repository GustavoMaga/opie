/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef WELLENREITERCONFIGWINDOW_H
#define WELLENREITERCONFIGWINDOW_H

#include "configbase.h"
#include "gps.h"

/* QT */
#include <qmap.h>
#include <qcombobox.h>
#include <qstring.h>

const int DEVTYPE_SELECT = 0;
const int DEVTYPE_CISCO = 1;
const int DEVTYPE_WLAN_NG = 2;
const int DEVTYPE_HOSTAP = 3;
const int DEVTYPE_ORINOCO = 4;
const int DEVTYPE_MANUAL = 5;
const int DEVTYPE_FILE = 6;

class WellenreiterConfigWindow;

class WellenreiterConfigWindow : public WellenreiterConfigBase
{
  Q_OBJECT

  public:
    WellenreiterConfigWindow( QWidget * parent = 0, const char * name = "WellenreiterConfigWindow", WFlags f = 0 );
    ~WellenreiterConfigWindow();

    int driverType() const;
    const QString soundOnNetwork() const { return "";/*netSound->currentText();*/ };
    const QString soundOnBeacon() const { return "";/*beaconSound->currentText();*/ };
    static WellenreiterConfigWindow* instance() { return _instance; };

    int hoppingInterval() const;
    bool usePrismHeader() const;
    bool isChannelChecked( int ) const;

    bool useGPS() const;
    const QString gpsHost() const;
    int gpsPort() const;

    void save();
    void load();

    int hexViewBuffer() const;

  public slots:
    void changedDeviceType(int);
    void changedNetworkAction(int t);
    void changedClientAction(int t);
    void changedStationAction(int t);
    void getCaptureFileNameClicked();
    void performAutodetection();
    void channelAllClicked(int);
    void performAction( const QString&, const QString&, const QString&, bool, int, int /* , const GpsLocation& */ );

  protected slots:
    void synchronizeActionsAndScripts();
    virtual void accept();

  protected:
    QMap<QString, int> _devicetype;
    static WellenreiterConfigWindow* _instance;
    int _guess;

};

#endif
