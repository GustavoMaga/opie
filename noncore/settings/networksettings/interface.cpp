#include "interface.h"
#include <qdatetime.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>

#define IFCONFIG "/sbin/ifconfig"
#define HDCP_INFO_DIR "/etc/dhcpc"

#include <stdio.h>
#include <stdlib.h>

Interface::Interface(QString name, bool newSatus): status(newSatus), attached(false), interfaceName(name), hardareName("Unknown"), moduleOwner("Default"), macAddress(""), ip("0.0.0.0"), broadcast(""), subnetMask("0.0.0.0"), dhcp(false){
  refresh();
}

/**
 * Try to start the interface.
 * @return bool true if successfull.
 */ 
bool Interface::start(){
  // check to see if we are already running.	
  if(status)
    return false;

  int ret = system(QString("%1 %2 up").arg(IFCONFIG).arg(interfaceName).latin1());
  if(ret != 0)
    return false;
  
  status = true;
  refresh();
  return true;
}

/**
 * Try to stop the interface.
 * @return bool true if successfull.
 */
bool Interface::stop(){
  // check to see if we are already stopped.	
  if(status == false)
    return false;
	  
  int ret = system(QString("%1 %2 down").arg(IFCONFIG).arg(interfaceName).latin1());
  if(ret != 0)
    return false;

  status = true;
  refresh();
  return true;
}
/**
 * Try to restart the interface.
 * @return bool true if successfull.
 */ 
bool Interface::restart(){
  return (stop() && start());
}

/**
 * Try to refresh the information about the interface.
 * First call ifconfig, then check the dhcp-info file
 * @return bool true if successfull.
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
    return true;
  }
	
  QString fileName = QString("/tmp/%1_ifconfig_info").arg(interfaceName);
  int ret = system(QString("%1 %2 > %3").arg(IFCONFIG).arg(interfaceName).arg(fileName).latin1());
  if(ret != 0){
    qDebug(QString("Interface: Ifconfig return value: %1, is not 0").arg(ret).latin1());
    return false;
  }
  
  QFile file(fileName);
  if (!file.open(IO_ReadOnly)){
    qDebug(QString("Interface: Can't open file: %1").arg(fileName).latin1());
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
    
  // See if we have 
  QString dhcpFile(QString(HDCP_INFO_DIR "/dhcpcd-%1.info").arg(interfaceName));
  // If there is no DHCP information then exit now with no errors.
  if(!QFile::exists(dhcpFile)){
    return true;
  }
  
  file.setName(dhcpFile);
  if (!file.open(IO_ReadOnly)){
    qDebug(QString("Interface: Can't open file: %1").arg(dhcpFile).latin1());
    return false;
  }
  
  // leaseTime and renewalTime and used if pid and deamon exe can be accessed.
  int leaseTime = 0;
  int renewalTime = 0;
  
  stream.setDevice( &file );
  while ( !stream.eof() ) {
    line = stream.readLine();
    if(line.contains("DHCPSID="))
      dhcpServerIp = line.mid(8, line.length());
    if(line.contains("LEASETIME="))
      leaseTime = line.mid(10, line.length()).toInt();
    if(line.contains("RENEWALTIME="))
      renewalTime = line.mid(12, line.length()).toInt();
  }
  file.close();
  //qDebug(QString("Interface: leaseTime: %1").arg(leaseTime).latin1());
  //qDebug(QString("Interface: renewalTime: %1").arg(renewalTime).latin1());
  
  // Get the pid of the deamond
  dhcpFile = (QString(HDCP_INFO_DIR "/dhcpcd-%1.pid").arg(interfaceName));
  file.setName(dhcpFile);
  if (!file.open(IO_ReadOnly)){
    qDebug(QString("Interface: Can't open file: %1").arg(dhcpFile).latin1());
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
    qDebug("Interface: Could not get pid of dhcpc deamon.");
    return false;
  }

  // Get the start running time of the deamon 
  fileName = (QString("/proc/%1/stat").arg(pid));
  file.setName(fileName);
  stream.setDevice( &file );
  if (!file.open(IO_ReadOnly)){
    qDebug(QString("Interface: Can't open file: %1").arg(fileName).latin1());
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
    qDebug("Interface: Can't open /proc/uptime to retrive uptime.");
    return false;
  }
  
  datetime = datetime.addSecs(time);
  //qDebug(QString("Interface: %1 %2").arg(datetime.toString()).arg(pid).latin1());
   
  // Calculate the start and renew times 
  leaseObtained	= datetime.toString();
  
  // Calculate the start and renew times 
  datetime = datetime.addSecs(leaseTime);
  leaseExpires = datetime.toString();
  
  dhcp = true;
  return true;
}

// interface.cpp

