/*
 * bluebase.cpp *
 * ---------------------
 *
 * copyright   : (c) 2002 by Maximilian Rei�
 * email       : max.reiss@gmx.de
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

#include "bluebase.h"
#include "scandialog.h"
#include "hciconfwrapper.h"

#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qtabwidget.h>
#include <qscrollview.h>
#include <qvbox.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qdir.h>
#include <qpopupmenu.h>
#include <qtimer.h>

#include <qpe/resource.h>
#include <qpe/config.h>

#include <remotedevice.h>
#include <services.h>

#include "btconnectionitem.h"

using namespace OpieTooth;

    BlueBase::BlueBase( QWidget* parent,  const char* name, WFlags fl )
        : BluetoothBase( parent, name, fl ) {

        m_localDevice = new Manager( "hci0" );

        connect( PushButton2,  SIGNAL( clicked() ), this, SLOT(startScan() ) );
        connect( configApplyButton, SIGNAL(clicked() ), this, SLOT(applyConfigChanges() ) );
        // not good since lib is async
        //       connect( ListView2, SIGNAL( expanded ( QListViewItem* ) ),
        //                  this, SLOT( addServicesToDevice( QListViewItem * ) ) );
        connect( ListView2, SIGNAL( clicked( QListViewItem* )),
                          this, SLOT( startServiceActionClicked( QListViewItem* ) ) );
        connect( ListView2, SIGNAL( rightButtonClicked( QListViewItem *, const QPoint &, int ) ),
                 this,  SLOT(startServiceActionHold( QListViewItem *, const QPoint &, int) ) );
        connect( m_localDevice , SIGNAL( foundServices( const QString& , Services::ValueList ) ),
                 this, SLOT( addServicesToDevice( const QString& , Services::ValueList ) ) );
        connect( m_localDevice, SIGNAL( available( const QString&, bool ) ),
                 this, SLOT( deviceActive( const QString& , bool ) ) );
        connect( m_localDevice, SIGNAL( connections( Connection::ValueList ) ),
                 this, SLOT( addConnectedDevices( Connection::ValueList ) ) );

        //Load all icons needed
        m_offPix = Resource::loadPixmap( "editdelete" );
        m_onPix = Resource::loadPixmap( "installed" );

        QPalette pal = this->palette();
        QColor col = pal.color( QPalette::Active, QColorGroup::Background );
        pal.setColor( QPalette::Active, QColorGroup::Button, col );
        pal.setColor( QPalette::Inactive, QColorGroup::Button, col );
        pal.setColor( QPalette::Normal, QColorGroup::Button, col );
        pal.setColor( QPalette::Disabled, QColorGroup::Button, col );
        this->setPalette( pal );

        setCaption( tr( "Bluetooth Manager" ) );

        readConfig();
        initGui();

        //TESTING
        ListView2->setRootIsDecorated(true);

        BTDeviceItem *topLV2 = new BTDeviceItem( ListView2, RemoteDevice("xx:", "Siemens S45" ) );
        topLV2->setPixmap( 1, m_onPix );
	Services s1;
	s1.setServiceName( "Serial" );
        (void) new BTServiceItem( topLV2, s1 );
	s1.setServiceName( "BlueNic" );
        (void) new BTServiceItem( topLV2, s1 );

        writeToHciConfig();
        // search conncetions
        addConnectedDevices();
        m_iconLoader = new BTIconLoader();
      }

    /**
     * Reads all options from the config file
     */
    void BlueBase::readConfig() {

        Config cfg( "bluetoothmanager" );
        cfg.setGroup( "bluezsettings" );

        m_deviceName = cfg.readEntry( "name" , "No name" ); // name the device should identify with
        m_defaultPasskey = cfg.readEntryCrypt( "passkey" , "" ); // <- hmm, look up how good the trolls did that, maybe too weak
        m_useEncryption = cfg.readBoolEntry( "useEncryption" , TRUE );
        m_enableAuthentification = cfg.readBoolEntry( "enableAuthentification" , TRUE );
        m_enablePagescan = cfg.readBoolEntry( "enablePagescan" , TRUE );
        m_enableInquiryscan = cfg.readBoolEntry( "enableInquiryscan" , TRUE );
    }

    /**
     * Writes all options to the config file
     */
    void BlueBase::writeConfig() {

        Config cfg( "bluetoothmanager" );
        cfg.setGroup( "bluezsettings" );

        cfg.writeEntry( "name" , m_deviceName );
        cfg.writeEntryCrypt( "passkey" , m_defaultPasskey );
        cfg.writeEntry( "useEncryption" , m_useEncryption );
        cfg.writeEntry( "enableAuthentification" , m_enableAuthentification );
        cfg.writeEntry( "enablePagescan" , m_enablePagescan );
        cfg.writeEntry( "enableInquiryscan" , m_enableInquiryscan );

        writeToHciConfig();
    }

    void BlueBase::writeToHciConfig() {

        HciConfWrapper hciconf ( "/tmp/hcid.conf" );
        hciconf.setPinHelper( "/bin/QtPalmtop/bin/blue-pin" );


        //    hciconf->setPinHelper( "/bin/QtPalmtop/bin/blue-pin" );

        hciconf.setName( m_deviceName );
        hciconf.setEncrypt( m_useEncryption );
	hciconf.setAuth( m_enableAuthentification );
        hciconf.setPscan( m_enablePagescan );
        hciconf.setIscan( m_enableInquiryscan );
    }


    /**
     * Read the list of allready known devices
     *
     */
    void BlueBase::readSavedDevices() {

        QValueList<RemoteDevice> loadedDevices;

        QDir deviceListSave( QDir::homeDirPath() + "/Settings/bluetooth/");
        // list of .conf files
        QStringList devicesFileList = deviceListSave.entryList();

        // cut .conf of to get the mac and also read the name entry in it.

        for ( QStringList::Iterator it = devicesFileList.begin(); it != devicesFileList.end(); ++it ) {

            QString name;
            QString mac;
            qDebug((*it).latin1() );
            Config conf((*it));
            conf.setGroup("Info");
            name = conf.readEntry("name", "Error");
            qDebug("MAC: " + mac);
            qDebug("NAME: " + name);
            RemoteDevice currentDevice( mac , name  );
            loadedDevices.append( currentDevice );
        }
        addSearchedDevices( loadedDevices );
    }

    /**
     * Write the list of allready known devices
     *
     */
    void BlueBase::writeSavedDevices() {
        QListViewItemIterator it( ListView2 );
	BTListItem* item;
	BTDeviceItem* device;

        for ( ; it.current(); ++it ) {
	    item = (BTListItem*)it.current();
	    if(item->typeId() != BTListItem::Device )
		continue;
	    device = (BTDeviceItem*)item;
            // seperate config file for each device, to store more information in future.
            qDebug( "/Settings/bluetooth/" + device->mac() + ".conf");
            Config conf( QDir::homeDirPath() + "/Settings/bluetooth/" + device->mac() + ".conf", Config::File );
            conf.setGroup( "Info" );
            conf.writeEntry( "name", device->name() );
        }
    }


    /**
     * Set up the gui
     */
    void BlueBase::initGui() {
        StatusLabel->setText( status() ); // maybe move it to getStatus()
        cryptCheckBox->setChecked( m_useEncryption );
        authCheckBox->setChecked( m_enableAuthentification );
        pagescanCheckBox->setChecked( m_enablePagescan );
        inquiryscanCheckBox->setChecked( m_enableInquiryscan );
        deviceNameLine->setText( m_deviceName );
        passkeyLine->setText( m_defaultPasskey );
        // set info tab
        setInfo();
    }


    /**
     * Get the status informations and returns it
     * @return QString the status informations gathered
     */
    QString BlueBase::status()const{
        QString infoString = tr( "<b>Device name : </b> Ipaq" );
        infoString += QString( "<br><b>" + tr( "MAC adress: " ) +"</b> No idea" );
        infoString += QString( "<br><b>" + tr( "Class" ) + "</b> PDA" );

        return (infoString);
    }


    /**
     * Read the current values from the gui and invoke writeConfig()
     */
    void BlueBase::applyConfigChanges() {
        m_deviceName =  deviceNameLine->text();
        m_defaultPasskey = passkeyLine->text();
        m_useEncryption = cryptCheckBox->isChecked();
        m_enableAuthentification = authCheckBox->isChecked();
        m_enablePagescan = pagescanCheckBox->isChecked();
        m_enableInquiryscan = inquiryscanCheckBox->isChecked();

        writeConfig();

        QMessageBox::information( this, tr("Test") , tr("Changes were applied.") );
   }

    /**
     * Add fresh found devices from scan dialog to the listing
     *
     */
    void BlueBase::addSearchedDevices( const QValueList<RemoteDevice> &newDevices ) {
        BTDeviceItem * deviceItem;
        QValueList<RemoteDevice>::ConstIterator it;

        for( it = newDevices.begin(); it != newDevices.end() ; ++it ) {

            deviceItem = new BTDeviceItem( ListView2 , (*it) );
            deviceItem->setExpandable ( true );

            // look if device is avail. atm, async
            deviceActive( (*it) );

            // ggf auch hier?
            addServicesToDevice( deviceItem );
        }
    }


    /**
     * Action that is toggled on entrys on click
     */
void BlueBase::startServiceActionClicked( QListViewItem */*item*/ ) {
}

    /**
     * Action that are toggled on hold (mostly QPopups i guess)
     */
void BlueBase::startServiceActionHold( QListViewItem * item, const QPoint & point, int /*column*/ ) {

    QPopupMenu *menu = new QPopupMenu();
    int ret=0;

    if ( ((BTListItem*)item)->type() == "device") {

        QPopupMenu *groups = new QPopupMenu();

        menu->insertItem( tr("rescan sevices:"),  0);
        menu->insertItem( tr("to group"), groups ,  1);
        menu->insertItem( tr("bound device"), 2);
        menu->insertItem( tr("delete"),  3);

        ret = menu->exec( point  , 0);

        switch(ret) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            // make connection
            break;
        case 3:
            // delete childs too
            delete item;
            break;
        }
        delete groups;

    } else if ( ((BTListItem*)item)->type() == "service") {

        menu->insertItem( tr("Test1:"),  0);
        menu->insertItem( tr("connect"), 1);
        menu->insertItem( tr("delete"),  2);

        ret = menu->exec( point  , 0);

        switch(ret) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            // delete childs too
            delete item;
            break;
        }
    }
    delete menu;
}

    /**
     * Search and display avail. services for a device (on expand from device listing)
     *
     */
    void BlueBase::addServicesToDevice( BTDeviceItem * item ) {
        qDebug("addServicesToDevice");
        // row of mac adress text(3)
        RemoteDevice device = item->remoteDevice();
        m_deviceList.insert( item->mac() ,  item );
        // and some time later I get a signal foundServices( const QString& device, Services::ValueList ); back
        m_localDevice->searchServices( device );
    }


    /**
     * Overloaded. This one it the one that is connected to the foundServices signal
     * @param device the mac address of the remote device
     * @param servicesList the list with the service the device has.
     */
    void BlueBase::addServicesToDevice( const QString& device, Services::ValueList servicesList ) {
        qDebug("fill services list");

        QMap<QString,BTDeviceItem*>::Iterator it;
        BTDeviceItem* deviceItem = 0;

        // get the right devices which requested the search
	it = m_deviceList.find( device );
	if( it == m_deviceList.end() )
	    return;
        deviceItem = it.data();

        QValueList<OpieTooth::Services>::Iterator it2;
        BTServiceItem * serviceItem;

        if (!servicesList.isEmpty() ) {
            // add services
            QMap<int, QString> list;
            QMap<int, QString>::Iterator classIt;
            for( it2 = servicesList.begin(); it2 != servicesList.end(); ++it2 ) {
                serviceItem = new BTServiceItem( deviceItem  , (*it2)  );
                list = (*it2).classIdList();
                classIt = list.begin();
                int classId;
                if ( classIt != list.end() )
                    classId = classIt.key();
                serviceItem->setPixmap( 0, m_iconLoader->serviceIcon( classId ) );
            }
        } else {
    	    Services s1;
	    s1.setServiceName( tr("no serives found") );
            serviceItem = new BTServiceItem( deviceItem, s1 );
        }
	// now remove them from the  list
	m_deviceList.remove( it );
    }


    /**
     * Add the existing connections (pairs) to the connections tab.
     * This one triggers the search
     */
    void BlueBase::addConnectedDevices() {
        m_localDevice->searchConnections();
    }


    void BlueBase::addConnectedDevices( ConnectionState::ValueList connectionList ) {
        QValueList<OpieTooth::ConnectionState>::Iterator it;
        BTConnectionItem * connectionItem;

        if ( !connectionList.isEmpty() ) {

            for (it = connectionList.begin(); it != connectionList.end(); ++it) {
                connectionItem = new BTConnectionItem( ListView4 , (*it) );
            }
        } else {
	    ConnectionState con;
	    con.setMac( tr("No connections found") );
            connectionItem = new BTConnectionItem( ListView4 , con );
        }

        //  recall connection search after some time
        QTimer::singleShot( 20000, this, SLOT( addConnectedDevices() ) );
    }

    /**
     * Find out if a device can  currently be reached
     */
    void BlueBase::deviceActive( const RemoteDevice &device ) {
        // search by mac, async, gets a signal back
	// We should have a BTDeviceItem there or where does it get added to the map -zecke
        m_localDevice->isAvailable( device.mac() );
    }

    /**
     * The signal catcher. Set the avail. status on device.
     * @param device - the mac address
     * @param connected - if it is avail. or not
     */
    void BlueBase::deviceActive( const QString& device, bool connected  ) {
        qDebug("deviceActive slot");

        QMap<QString,BTDeviceItem*>::Iterator it;

	it = m_deviceList.find( device );
	if( it == m_deviceList.end() )
	    return;

        BTDeviceItem* deviceItem = it.data();


        if ( connected ) {
            deviceItem->setPixmap( 1, m_onPix );
        } else {
            deviceItem->setPixmap( 1, m_offPix );
        }
	m_deviceList.remove( it );
    }

    /**
     * Open the "scan for devices"  dialog
     */
    void BlueBase::startScan() {
        ScanDialog *scan = new ScanDialog( this, "ScanDialog",
                                           true, WDestructiveClose );
        QObject::connect( scan, SIGNAL( selectedDevices( const QValueList<RemoteDevice>& ) ),
                         this, SLOT( addSearchedDevices( const QValueList<RemoteDevice>& ) ) );

        scan->showMaximized();
    }


    /**
     * Set the informations about the local device in information Tab
     */
    void BlueBase::setInfo() {
        StatusLabel->setText( status() );
    }

    /**
     * Decontructor
     */
    BlueBase::~BlueBase() {
        writeSavedDevices();
        delete m_iconLoader;
    }

