#include "interfacesetupimp.h"
#include "interface.h"

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qlabel.h>

#include <qmessagebox.h>

#define DNSSCRIPT "changedns"

/**
 * Constuctor.  Set up the connection. A profile must be set.
 */ 
InterfaceSetupImp::InterfaceSetupImp(QWidget* parent, const char* name, Interface *i, Interfaces *j, WFlags fl) : InterfaceSetup(parent, name, fl), interface(i), interfaces(j), delInterfaces(false){
  if (j == 0) {
    delInterfaces = true;
    interfaces = new Interfaces;
  }
}

/**
 * Destructor
 */ 
InterfaceSetupImp::~InterfaceSetupImp(){
  if(delInterfaces) {
    delete interfaces;
  }
}

/**
 * Save the current settings, then write out the interfaces file and close. 
 */
bool InterfaceSetupImp::saveChanges(){
  if(!saveSettings())
    return false;
  interfaces->write();
  return true;
}

/**
 * Save the settings for the current Interface.
 * @return bool true if successfull, false otherwise
 */ 
bool InterfaceSetupImp::saveSettings(){
  // eh can't really do anything about it other then return. :-D
  if(!interfaces->isInterfaceSet())
    return true;
  
  bool error = false;
  // Loopback case
  if(interfaces->getInterfaceMethod(error) == INTERFACES_LOOPBACK){
    interfaces->setAuto(interface->getInterfaceName(), autoStart->isChecked());
    return true;
  }
  
  if(!dhcpCheckBox->isChecked() && (ipAddressEdit->text().isEmpty() || subnetMaskEdit->text().isEmpty())){
   QMessageBox::information(this, "Not Saved.", "Please fill in the IP address and\n subnet entries.", QMessageBox::Ok);
   return false;
  }	
  //interfaces.removeAllInterfaceOptions();
  
  // DHCP
  if(dhcpCheckBox->isChecked())
    interfaces->setInterfaceMethod(INTERFACES_METHOD_DHCP);
  else{
    interfaces->setInterfaceMethod("static");
    interfaces->setInterfaceOption("address", ipAddressEdit->text());
    interfaces->setInterfaceOption("netmask", subnetMaskEdit->text());
    interfaces->setInterfaceOption("gateway", gatewayEdit->text());
    if(!firstDNSLineEdit->text().isEmpty() || !secondDNSLineEdit->text().isEmpty()){
      QString dns = firstDNSLineEdit->text() + " " + secondDNSLineEdit->text();
      interfaces->setInterfaceOption("up "DNSSCRIPT" -a ", dns);
      interfaces->setInterfaceOption("down "DNSSCRIPT" -r ", dns);
    }
  }
  
  // IP Information
  interfaces->setAuto(interface->getInterfaceName(), autoStart->isChecked());
  return true;
}

/**
 * The Profile has changed.
 * @param QString profile the new profile.
 */ 
void InterfaceSetupImp::setProfile(const QString &profile){
   /*
  bool error = false;
  if(interfaces->getInterfaceMethod(error) == INTERFACES_LOOPBACK){
    staticGroupBox->hide();
    dhcpCheckBox->hide();
    leaseTime->hide();
    leaseHoursLabel->hide();
  }
  */
  
  QString newInterfaceName = interface->getInterfaceName();
  if(profile.length() > 0)
    newInterfaceName += "_" + profile;
  // See if we have to make a interface.
  if(!interfaces->setInterface(newInterfaceName)){
    // Add making for this new interface if need too
    if(profile != ""){
      interfaces->copyInterface(interface->getInterfaceName(), newInterfaceName);
      if(!interfaces->setMapping(interface->getInterfaceName())){
        interfaces->addMapping(interface->getInterfaceName());
        if(!interfaces->setMapping(interface->getInterfaceName())){
	  qDebug("InterfaceSetupImp: Added Mapping, but still can't setInterface.");
          return;
	}
      }
      interfaces->setMap("map", newInterfaceName);
      interfaces->setScript("getprofile.sh");
    }
    else{
      interfaces->addInterface(newInterfaceName, INTERFACES_FAMILY_INET, INTERFACES_METHOD_DHCP);
      if(!interfaces->setInterface(newInterfaceName)){
        qDebug("InterfaceSetupImp: Added interface, but still can't setInterface.");
        return;	      
      }
    }
  }
  
  // We must have a valid interface to get this far so read some settings.
  
  // DHCP
  bool error = false;
  if(interfaces->getInterfaceMethod(error) == INTERFACES_METHOD_DHCP)
    dhcpCheckBox->setChecked(true);
  else
    dhcpCheckBox->setChecked(false);

  // IP Information
  autoStart->setChecked(interfaces->isAuto(interface->getInterfaceName()));
  QString dns = interfaces->getInterfaceOption("up "DNSSCRIPT" -a", error);
  if(dns.contains(" ")){
    firstDNSLineEdit->setText(dns.mid(0, dns.find(" ")));
    secondDNSLineEdit->setText(dns.mid(dns.find(" ")+1, dns.length())); 
  }
  ipAddressEdit->setText(interfaces->getInterfaceOption("address", error));
  subnetMaskEdit->setText(interfaces->getInterfaceOption("netmask", error));
  gatewayEdit->setText(interfaces->getInterfaceOption("gateway", error)); 
}
  
// interfacesetup.cpp

