/**
 * $Author: zecke $
 * $Date: 2004-12-20 22:29:08 $
 */

#include "interface.h"

#include <opie2/odebug.h>
#include <opie2/oprocess.h>
#include <opie2/owait.h>

#include <qpe/global.h>

#include <qapplication.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qtextstream.h>

#define IFCONFIG "/sbin/ifconfig"
#define IF_UP   "/sbin/ifup"
#define IF_DOWN "/sbin/ifdown"
#define DHCP_INFO_DIR "/etc/dhcpc"

#include <stdio.h>
#include <stdlib.h>

Interface::Interface(QObject * parent, const char * name, bool newSatus): QObject(parent, name), hardwareName("Unknown"), moduleOwner(NULL), status(newSatus), attached(false), dhcp(false), macAddress(""), ip("0.0.0.0"), broadcast(""), subnetMask("0.0.0.0"){
  refresh();
}

/**
 * Set status
 * @param newStatus - the new status
 * emit updateInterface
 */
void Interface::setStatus(bool newStatus){
  if(status != newStatus){
    status = newStatus;
    refresh();
  }
};

/**
 * Set if attached or not (802.11 card pulled out for example)
 * @param isAttached - if attached
 * emit updateInterface
 */
void Interface::setAttached(bool isAttached){
  attached = isAttached;
  emit(updateInterface(this));
};

/**
 * Set Hardware name
 * @param name - the new name
 * emit updateInterface
 */
void Interface::setHardwareName(const QString &name){
  hardwareName = name;
  emit(updateInterface(this));
};

/**
 * Set Module owner
 * @param owner - the new owner
 * emit updateInterface
 */
void Interface::setModuleOwner(Module *owner){
  moduleOwner = owner;
  emit(updateInterface(this));
};


bool Interface::callProcess( const QStringList& names ) {
    Opie::Ui::OWait *owait = new Opie::Ui::OWait();
    Global::statusMessage( tr( "Restarting interface" ) );

    owait->show();
    qApp->processEvents();

    Opie::Core::OProcess restart;
    restart << names;
    if ( !restart.start(Opie::Core::OProcess::Block,
                        Opie::Core::OProcess::NoCommunication ) ) {
        owarn << "unable to spawn command" << names << oendl;
        return false;
    }
    owait->hide();
    delete owait;

    if ( restart.normalExit() || restart.exitStatus() != 0 )
        return false;

    return true;
}

/**
 * Try to start the interface.
 */
void Interface::start(){
  // check to see if we are already running.
  if(true == status){
    emit (updateMessage("Unable to start interface,\n already started"));
    return;
  }

  /* prepare command and call it */
  QStringList lst;
  lst << IF_UP;
  lst << name();
  if ( !callProcess(lst) ) {
    emit (updateMessage("Starting interface failed"));
    return;
  }

  status = true;
  refresh();
  emit (updateMessage("Start successful"));
}

/**
 * Try to stop the interface.
 */
void Interface::stop(){
  // check to see if we are already stopped.
  if(false == status){
    emit (updateMessage("Unable to stop interface,\n already stopped"));
    return;
  }

  QStringList lst;
  lst << IF_DOWN;
  lst << name();

  /* prepare command and call it */
  if( !callProcess( lst ) ){
    emit (updateMessage("Stopping interface failed"));
    return;
  }

  status = false;
  refresh();
  emit (updateMessage("Stop successful"));
}

/**
 * Try to restart the interface.
 */
void Interface::restart(){
  stop();
  start();
}

/**
 * Try to refresh the information about the interface.
 * First call ifconfig, then check the dhcp-info file
 * @return bool true if successful.
 */
bool Interface::refresh(){
  // See if we are up.
  if(status == false){
    macAddress = "";
    ip = "0.0.0.0";
    subnetMask = "0.0.0.0";
    broadcast = "";
    dhcp = false;
    dhcpServerIp = "";
    leaseObtained = "";
    leaseExpires = "";
    emit(updateInterface(this));
    return true;
  }

  QString fileName = QString("/tmp/%1_ifconfig_info").arg(this->name());
  int ret = system(QString("LANG=C %1 %2 > %3").arg(IFCONFIG).arg(this->name()).arg(fileName).latin1());
  if(ret != 0){
    odebug << QString("Interface: Ifconfig return value: %1, is not 0").arg(ret).latin1() << oendl; 
    return false;
  }

  QFile file(fileName);
  if (!file.open(IO_ReadOnly)){
    odebug << QString("Interface: Can't open file: %1").arg(fileName).latin1() << oendl; 
    return false;
  }

  // Set to the defaults
  macAddress = "";
  ip = "0.0.0.0";
  subnetMask = "0.0.0.0";
  broadcast = "";

  QTextStream stream( &file );
  QString line;
  while ( !stream.eof() ) {
    line = stream.readLine();
    if(line.contains("HWaddr")){
      int mac = line.find("HWaddr");
      macAddress = line.mid(mac+7, line.length());
    }
    if(line.contains("inet addr")){
      int ipl = line.find("inet addr");
      int space = line.find(" ", ipl+10);
      ip = line.mid(ipl+10, space-ipl-10);
    }
    if(line.contains("Mask")){
      int mask = line.find("Mask");
      subnetMask = line.mid(mask+5, line.length());
    }
    if(line.contains("Bcast")){
      int mask = line.find("Bcast");
      int space = line.find(" ", mask+6);
      broadcast = line.mid(mask+6, space-mask-6);
    }
  }
  file.close();
  QFile::remove(fileName);

  // DHCP TESTING
  // reset DHCP info
  dhcpServerIp = "";
  leaseObtained = "";
  leaseExpires = "";
  dhcp = false;

  QString dhcpDirectory(DHCP_INFO_DIR);
  QDir d(dhcpDirectory);
  if(!d.exists(dhcpDirectory))
    dhcpDirectory = "/var/run";

  // See if we have
  QString dhcpFile(QString(dhcpDirectory+"/dhcpcd-%1.info").arg(this->name()));
  // If there is no DHCP information then exit now with no errors.
  if(!QFile::exists(dhcpFile)){
    emit(updateInterface(this));
    return true;
  }

  file.setName(dhcpFile);
  if (!file.open(IO_ReadOnly)){
    odebug << QString("Interface: Can't open file: %1").arg(dhcpFile).latin1() << oendl; 
    return false;
  }

  // leaseTime and renewalTime and used if pid and deamon exe can be accessed.
  int leaseTime = 0;
  int renewalTime = 0;

  stream.setDevice( &file );
  while ( !stream.eof() ) {
    line = stream.readLine();
    if(line.contains("DHCPSIADDR="))
      dhcpServerIp = line.mid(11, line.length());
    if(line.contains("LEASETIME="))
      leaseTime = line.mid(10, line.length()).toInt();
    if(line.contains("RENEWALTIME="))
      renewalTime = line.mid(12, line.length()).toInt();
  }
  file.close();
  //odebug << QString("Interface: leaseTime: %1").arg(leaseTime).latin1() << oendl; 
  //odebug << QString("Interface: renewalTime: %1").arg(renewalTime).latin1() << oendl; 

  // Get the pid of the deamond
  dhcpFile = (QString(dhcpDirectory+"/dhcpcd-%1.pid").arg(this->name()));
  file.setName(dhcpFile);
  if (!file.open(IO_ReadOnly)){
    odebug << QString("Interface: Can't open file: %1").arg(dhcpFile).latin1() << oendl; 
    return false;
  }

  int pid = -1;
  stream.setDevice( &file );
  while ( !stream.eof() ) {
    line = stream.readLine();
    pid = line.toInt();
  }
  file.close();

  if( pid == -1){
    odebug << "Interface: Could not get pid of dhcpc deamon." << oendl; 
    return false;
  }

  // Get the start running time of the deamon
  fileName = (QString("/proc/%1/stat").arg(pid));
  file.setName(fileName);
  stream.setDevice( &file );
  if (!file.open(IO_ReadOnly)){
    odebug << QString("Interface: Can't open file: %1").arg(fileName).latin1() << oendl; 
    return false;
  }
  while ( !stream.eof() ) {
    line = stream.readLine();
  }
  file.close();
  long time = 0;
  // Grab the start time
  //                     pid com state ppid pgrp session tty_nr tpgid flags
  sscanf(line.latin1(), "%*d %*s %*c   %*d  %*d  %*d     %*d    %*d   %*u "
  //   minflt cminflt majflt cmajflt utime stime cutime cstime priority
      "%*u    %*u    %*u    %*u     %*u   %*u   %*d   %*d   %*d "
  //   nice 0   itrealvalue starttime
      "%*d  %*d %*d %lu", (long*) &time);
  time = time/100;

  QDateTime datetime(QDateTime::currentDateTime());

  // Get the uptime of the computer.
  QFile f("/proc/uptime");
  if ( f.open(IO_ReadOnly) ) {    // file opened successfully
    QTextStream t( &f );        // use a text stream
    int sec = 0;
    t >> sec;
    datetime = datetime.addSecs((-1*sec));
    f.close();
  }
  else{
    odebug << "Interface: Can't open /proc/uptime to retrive uptime." << oendl; 
    return false;
  }

  datetime = datetime.addSecs(time);
  //odebug << QString("Interface: %1 %2").arg(datetime.toString()).arg(pid).latin1() << oendl; 

  // Calculate the start and renew times
  leaseObtained	= datetime.toString();

  // Calculate the start and renew times
  datetime = datetime.addSecs(leaseTime);
  leaseExpires = datetime.toString();

  dhcp = true;

  emit(updateInterface(this));
  return true;
}

// interface.cpp

