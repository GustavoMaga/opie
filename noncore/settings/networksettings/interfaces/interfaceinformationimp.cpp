
#include "interfaceinformationimp.h"
#include "interfaceadvanced.h"

/* OPIE */
#include <qpe/config.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qmessagebox.h>


#ifdef QWS
#else
 #define showMaximized show
#endif

/**
 * Constructor for the InterfaceInformationImp class.  This class pretty much
 * just display's information about the interface that is passed to it.
 */
InterfaceInformationImp::InterfaceInformationImp(QWidget *parent, const char *name, Interface *i, WFlags f):InterfaceInformation(parent, name, f), interface(i)
{
    connect(i, SIGNAL(updateInterface(Interface *)), this, SLOT(updateInterface(Interface *)));
    connect(i, SIGNAL(updateMessage(const QString &)), this, SLOT(showMessage(const QString &)));
    updateInterface(interface);
    connect(startButton, SIGNAL(clicked()), interface, SLOT(start()));
    connect(stopButton, SIGNAL(clicked()), interface, SLOT(stop()));
    connect(restartButton, SIGNAL(clicked()), interface, SLOT(restart()));
    connect(refreshButton, SIGNAL(clicked()), interface, SLOT(refresh()));
    connect(advancedButton, SIGNAL(clicked()), this, SLOT(advanced()));
    Config cfg("networksettings", Config::User);
    cfg.setGroup("interface");
    CheckBoxSilent->setChecked( cfg.readBoolEntry("silent", false) );
}

InterfaceInformationImp::~InterfaceInformationImp()
{
    Config cfg("networksettings", Config::User);
    cfg.setGroup("interface");
    cfg.writeEntry("silent", CheckBoxSilent->isChecked() );
}

/**
 * Update the interface information and buttons.
 * @param Intarface *i the interface to update (should be the one we already
 *                     know about).
 */
void InterfaceInformationImp::updateInterface(Interface *)
{
    if(interface->getStatus())
    {
        startButton->setEnabled(false);
        stopButton->setEnabled(true);
        restartButton->setEnabled(true);
    }
    else
    {
        startButton->setEnabled(true);
        stopButton->setEnabled(false);
        restartButton->setEnabled(false);
    }
    macAddressLabel->setText(interface->getMacAddress());
    ipAddressLabel->setText(interface->getIp());
    subnetMaskLabel->setText(interface->getSubnetMask());
    broadcastLabel->setText(interface->getBroadcast());
}

/**
 * Create the advanced widget. Fill it with the current interface's information.
 * Display it.
 */
void InterfaceInformationImp::advanced()
{
    InterfaceAdvanced *a = new InterfaceAdvanced(this, "InterfaceAdvanced", Qt::WType_Modal | Qt::WDestructiveClose | Qt::WStyle_Dialog);
    a->interfaceName->setText(interface->getInterfaceName());
    a->macAddressLabel->setText(interface->getMacAddress());
    a->ipAddressLabel->setText(interface->getIp());
    a->subnetMaskLabel->setText(interface->getSubnetMask());
    a->broadcastLabel->setText(interface->getBroadcast());
    a->dhcpServerLabel->setText(interface->getDhcpServerIp());
    a->leaseObtainedLabel->setText(interface->getLeaseObtained());
    a->leaseExpiresLabel->setText(interface->getLeaseExpires());
    a->dhcpInformation->setEnabled(interface->isDhcp());
    QPEApplication::showWidget( a );
}

/**
 * Messages from the interface if start/stop went as planned.
 * Purly for user feedback.
 * @param message the message to display.
 */
void InterfaceInformationImp::showMessage(const QString &message)
{
    if (CheckBoxSilent->isChecked()) return;
    QMessageBox::information(this, "Message", message, QMessageBox::Ok);
}

// infoimp.cpp

