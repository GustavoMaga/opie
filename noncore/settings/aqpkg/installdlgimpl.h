/***************************************************************************
                          installdlgimpl.h  -  description
                             -------------------
    begin                : Mon Aug 26 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef INSTALLDLGIMPL_H
#define INSTALLDLGIMPL_H

#include <vector>
using namespace std;

#include <qwidget.h>
#include <qstring.h>

class QComboBox;
class QLabel;
class QMultiLineEdit;
class QPushButton;

class DataManager;
class Destination;
class Ipkg;

class InstallData
{
public:
    QString option;            // I - install, D - delete, R- reinstall U - upgrade
    QString packageName;
    Destination *destination;
    bool recreateLinks;
};

class InstallDlgImpl : public QWidget
{
    Q_OBJECT
public:
    InstallDlgImpl( vector<InstallData> &packageList, DataManager *dataManager, const char *title = 0 );
    InstallDlgImpl( Ipkg *ipkg, QString initialText, const char *title = 0 );
    ~InstallDlgImpl();

	bool upgradeServer( QString &server );

protected:

private:
    DataManager *dataMgr;
	vector<InstallData> installList;
	vector<InstallData> removeList;
	vector<InstallData> updateList;
    int flags;
    Ipkg *pIpkg;
    bool upgradePackages;
    
    QComboBox      *destination;
    QPushButton    *btnInstall;
    QPushButton    *btnOptions;
    QMultiLineEdit *output;
    QLabel         *txtAvailableSpace;

    void init( bool );
    
    bool runIpkg( QString &option, const QString& package, const QString& dest, int flags );

signals:
    void reloadData( InstallDlgImpl * );
        
public slots:
    void optionsSelected();
    void installSelected();
    void displayText(const QString &text );
    void displayAvailableSpace( const QString &text);
};

#endif
