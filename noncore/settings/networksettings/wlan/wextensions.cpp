#include "wextensions.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qfile.h>
#include <qtextstream.h>

/* STD */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <math.h>

#define PROCNETWIRELESS "/proc/net/wireless"
#define IW_LOWER 0
#define IW_UPPER 256

#warning This is duplicated code. Use libopienet2!

/**
 * Constructor.  Sets hasWirelessExtensions
 */ 
WExtensions::WExtensions(QString interfaceName): hasWirelessExtensions(false), interface(interfaceName) {
  fd = socket( AF_INET, SOCK_DGRAM, 0 );
  if(fd == -1) 
    return;

  const char* buffer[200];
  memset( &iwr, 0, sizeof( iwr ) );
  iwr.u.essid.pointer = (caddr_t) buffer;
  iwr.u.essid.length = IW_ESSID_MAX_SIZE;
  iwr.u.essid.flags = 0;
  
  // check if it is an IEEE 802.11 standard conform
  // wireless device by sending SIOCGIWESSID
  // which also gives back the Extended Service Set ID
  // (see IEEE 802.11 for more information)

  const char* iname = interface.latin1();
  strcpy( iwr.ifr_ifrn.ifrn_name,  (const char *)iname );
  if ( 0 == ioctl( fd, SIOCGIWESSID, &iwr ) )
    hasWirelessExtensions = true;
}

/**
 * @return QString the station name of the access point.
 */ 
QString WExtensions::station(){
  if(!hasWirelessExtensions)
    return QString();
  const char* buffer[200];
  iwr.u.data.pointer = (caddr_t) buffer;
  iwr.u.data.length = IW_ESSID_MAX_SIZE;
  iwr.u.data.flags = 0;
  if ( 0 == ioctl( fd, SIOCGIWNICKN, &iwr )){
    buffer[(unsigned int) iwr.u.data.length-1] = '\0';
    return (const char*) buffer;
  }
  return QString::null;
}

/**
 * @return QString the essid of the host 802.11 access point.
 */ 
QString WExtensions::essid(){
  if(!hasWirelessExtensions)
    return QString();
  const char* buffer[200];
  memset(buffer,0x00,200);
  iwr.u.data.pointer = (caddr_t) buffer;
  iwr.u.data.length = IW_ESSID_MAX_SIZE;
  iwr.u.data.flags = 0;
  if ( 0 == ioctl( fd, SIOCGIWESSID, &iwr )){
    if (iwr.u.essid.length > IW_ESSID_MAX_SIZE)
      iwr.u.essid.length = IW_ESSID_MAX_SIZE;
    buffer[(unsigned int) iwr.u.essid.length] = '\0';
    return (const char*) buffer;
  }
  return QString::null;
}

/**
 * @return QString the mode of interface
 */ 
QString WExtensions::mode(){
  if(!hasWirelessExtensions)
    return QString();
  if ( 0 == ioctl( fd, SIOCGIWMODE, &iwr ) )
    return QString("%1").arg(iwr.u.mode == IW_MODE_ADHOC ? "Ad-Hoc" : "Managed");
  return QString();
}

/**
 * Get the frequency that the interface is running at.
 * @return int the frequency that the interfacae is running at.
 */ 
double WExtensions::frequency(){
  if(!hasWirelessExtensions)
    return 0;
  if ( 0 == ioctl( fd, SIOCGIWFREQ, &iwr ))
    return (double( iwr.u.freq.m ) * pow( 10, iwr.u.freq.e ) / 1000000000);
  return 0;
}

/**
 * Get the channel that the interface is running at.
 * @return int the channel that the interfacae is running at.
 */ 
int WExtensions::channel(){
  if(!hasWirelessExtensions)
    return 0;
  if ( 0 != ioctl( fd, SIOCGIWFREQ, &iwr ))
    return 0;
  
  // http://www.elanix.com/pdf/an137e.pdf

  double num = (double( iwr.u.freq.m ) * pow( 10, iwr.u.freq.e ) / 1000000000);
  double left = 2.401;
  double right = 2.416;
  for(int channel = 1;  channel<= 15; channel++){
    if( num >= left && num <= right )
      return channel;
    left += 0.005;
    right += 0.005;
  }
  odebug << QString("Unknown frequency: %1, returning -1 for the channel.").arg(num).latin1() << oendl; 
  return -1;
}

/***
 * Get the current rate that the card is transmiting at.
 * @return double the rate, 0 if error.
 */ 
double WExtensions::rate(){
  if(!hasWirelessExtensions)
    return 0;
  if(0 == ioctl(fd, SIOCGIWRATE, &iwr)){
    return ((double)iwr.u.bitrate.value)/1000000;
  }
  return 0;
}


/**
 * @return QString the AccessPoint that the interface is connected to.
 */ 
QString WExtensions::ap(){
  if(!hasWirelessExtensions)
    return QString();
  if ( 0 == ioctl( fd, SIOCGIWAP, &iwr )){
    QString ap;
    ap = ap.sprintf( "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
      iwr.u.ap_addr.sa_data[0]&0xff,
      iwr.u.ap_addr.sa_data[1]&0xff,
      iwr.u.ap_addr.sa_data[2]&0xff,
      iwr.u.ap_addr.sa_data[3]&0xff,
      iwr.u.ap_addr.sa_data[4]&0xff,
      iwr.u.ap_addr.sa_data[5]&0xff );
    return ap;
  }
  else return QString();
}

/**
 * Get the stats for interfaces
 * @param signal the signal strength of interface
 * @param noise the noise level of the interface
 * @param quality the quality level of the interface
 * @return bool true if successful
 */ 
bool WExtensions::stats(int &signal, int &noise, int &quality){
  // gather link quality from /proc/net/wireless
  if(!QFile::exists(PROCNETWIRELESS))
    return false;

  char c;
  QString status;
  QString name;
  
  QFile wfile( PROCNETWIRELESS );
  if(!wfile.open( IO_ReadOnly ))
    return false;
  
  QTextStream wstream( &wfile );
  wstream.readLine();  // skip the first two lines
  wstream.readLine();  // because they only contain headers
  while(!wstream.atEnd()){
    wstream >> name >> status >> quality >> c >> signal >> c >> noise;
    if(name == QString("%1:").arg(interface)){
      if ( quality > 92 )
        odebug << "WIFIAPPLET: D'oh! Quality " << quality << " > estimated max!\n" << oendl; 
      if ( ( signal > IW_UPPER ) || ( signal < IW_LOWER ) )
        odebug << "WIFIAPPLET: Doh! Strength " << signal << " > estimated max!\n" << oendl; 
      if ( ( noise > IW_UPPER ) || ( noise < IW_LOWER ) )
        odebug << "WIFIAPPLET: Doh! Noise " << noise << " > estimated max!\n" << oendl; 
      //odebug << QString("q:%1, s:%2, n:%3").arg(quality).arg(signal).arg(noise).latin1() << oendl; 
      signal = ( ( signal-IW_LOWER ) * 100 ) / IW_UPPER;
      noise = ( ( noise-IW_LOWER ) * 100 ) / IW_UPPER;
      quality = ( quality*100 ) / 92;
      return true;
    }  
  }

  odebug << "WExtensions::statsCard no longer present." << oendl; 
  quality = -1;
  signal = IW_LOWER;
  noise = IW_LOWER;
  return false;
}

// wextensions.cpp
