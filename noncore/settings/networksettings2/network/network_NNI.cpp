#include <system.h>
#include <netnode.h>
#include "networkedit.h"
#include "network_NNI.h"
#include "network_NN.h"

ANetwork::ANetwork( NetworkNetNode * PNN ) : ANetNodeInstance( PNN ) {
    Data.UseDHCP = 1;
    Data.IPAddress = "";
    Data.NetMask = "";
    Data.Broadcast = "";
    Data.Gateway = "";
    Data.DNS1 = "";
    Data.DNS2 = "";
    Data.SendHostname = 0;
    Data.Hostname = "";
    Data.PreUp_SL.clear();
    Data.PreDown_SL.clear();
    Data.PostUp_SL.clear();
    Data.PostDown_SL.clear();
    GUI = 0;
    RT = 0;
}

void ANetwork::setSpecificAttribute( QString & A, QString & V ) {
    if( A == "usedhcp" ) {
      Data.UseDHCP = (V == "yes");
    } else if( A == "sendhostname" ) {
      Data.SendHostname = (V=="yes");
    } else if( A == "hostname" ) {
      Data.Hostname = V;
    } else if( A == "ipaddress" ) {
      Data.IPAddress = V;
    } else if( A == "netmask" ) {
      Data.NetMask = V;
    } else if( A == "broadcast" ) {
      Data.Broadcast = V;
    } else if( A == "gateway" ) {
      Data.Gateway = V;
    } else if( A == "dns1" ) {
      Data.DNS1 = V;
    } else if( A == "dns2" ) {
      Data.DNS2 = V;
    } else if( A == "preup" ) {
      Data.PreUp_SL.append( V );
    } else if( A == "predown" ) {
      Data.PreDown_SL.append( V );
    } else if( A == "postup" ) {
      Data.PostUp_SL.append( V );
    } else if( A == "postdown" ) {
      Data.PostDown_SL.append( V );
    }
}

void ANetwork::saveSpecificAttribute( QTextStream & TS ) {
    TS << "usedhcp=" << ((Data.UseDHCP) ? "yes" : "no") << endl;
    TS << "sendhostname=" << ((Data.SendHostname) ? "yes" : "no") << endl;
    TS << "hostname=" << Data.Hostname << endl;
    TS << "ipaddress=" << Data.IPAddress << endl;
    TS << "netmask=" << Data.NetMask << endl;
    TS << "broadcast=" << Data.Broadcast << endl;
    TS << "gateway=" << Data.Gateway << endl;
    TS << "dns1=" << Data.DNS1 << endl;
    TS << "dns2=" << Data.DNS2 << endl;
    for ( QStringList::Iterator it = Data.PreUp_SL.begin(); 
          it != Data.PreUp_SL.end(); 
          ++it ) {
      TS << "preup=" << quote(*it) << endl;
    }
    for ( QStringList::Iterator it = Data.PreDown_SL.begin(); 
          it != Data.PreDown_SL.end(); 
          ++it ) {
      TS << "predown=" << quote(*it) << endl;
    }
    for ( QStringList::Iterator it = Data.PostUp_SL.begin(); 
          it != Data.PostUp_SL.end(); 
          ++it ) {
      TS << "postup=" << quote(*it) << endl;
    }
    for ( QStringList::Iterator it = Data.PostDown_SL.begin(); 
          it != Data.PostDown_SL.end(); 
          ++it ) {
      TS << "postdown=" << quote(*it) << endl;
    }
}

QWidget * ANetwork::edit( QWidget * parent ) {
    GUI = new NetworkEdit( parent );
    GUI->showData( Data );
    return GUI;
}

QString ANetwork::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void ANetwork::commit( void ) {
    if( GUI && GUI->commit( Data ) )
      setModified( 1 );
}

bool ANetwork::hasDataForFile( SystemFile & S ) {
      return S.name() == "interfaces";
}

short ANetwork::generateFile( SystemFile &SF,
                               long DevNr 
                             ) {

    short rvl, rvd ;
    QString NIC = runtime()->device()->netNode()->nodeClass()->genNic( DevNr );

    rvl = 1;
    if( SF.name() == "interfaces" ) {
      Log(("Generate Network for %s\n", SF.name().latin1() ));
      // we can safely call from here since device item is deeper
      if( Data.UseDHCP ) {
        SF << "iface " 
           << "A" 
           << networkSetup()->number() 
           << NIC 
           << " inet dhcp" 
           << endl;
        SF << "  up echo \"" 
           << NIC 
           << "\" > /tmp/profile-" 
           << networkSetup()->number() 
           << ".up" 
           << endl;
        if( Data.SendHostname ) {
          SF << "  hostname "
             << Data.Hostname  
             << endl;
        }

        SF << "  down rm -f /tmp/profile-" 
           << networkSetup()->number() 
           << ".up"
           << endl;
      } else {
        SF << "iface " 
           << "A" 
           << networkSetup()->number() 
           << NIC 
           << " inet static" 
           << endl;
        SF << "  up echo \"" 
           << NIC 
           << "\" > /tmp/profile-" 
           << networkSetup()->number() 
           << ".up" 
           << endl;
        SF << "  down rm -f /tmp/profile-" 
           << networkSetup()->number() 
           << ".up" 
           << endl;
        SF << "  address   " 
           << Data.IPAddress 
           << endl;
        SF << "  broadcast " 
           << Data.Broadcast 
           << endl;
        SF << "  netmask   " 
           << Data.NetMask 
           << endl;

        // derive network address = IPAddress & netmask
        { QString NW;
          QStringList ipal = QStringList::split( '.', Data.IPAddress );
          QStringList nmal = QStringList::split( '.', Data.NetMask );

          NW = QString( "%1.%2.%3.%4" ).
              arg( ipal[0].toShort() & nmal[0].toShort() ).
              arg( ipal[1].toShort() & nmal[1].toShort() ).
              arg( ipal[2].toShort() & nmal[2].toShort() ).
              arg( ipal[3].toShort() & nmal[3].toShort() );
          SF << "  network   " 
             << NW 
             << endl;
        }
      }
      for ( QStringList::Iterator it = Data.PreUp_SL.begin(); 
            it != Data.PreUp_SL.end(); 
            ++it ) {
        SF << "  pre-up    " 
           << (*it) 
           << endl;
      }
      for ( QStringList::Iterator it = Data.PostUp_SL.begin(); 
            it != Data.PostUp_SL.end(); 
            ++it ) {
        SF << "  up        " 
           << (*it) 
           << endl;
      }
      for ( QStringList::Iterator it = Data.PreDown_SL.begin(); 
            it != Data.PreDown_SL.end(); 
            ++it ) {
        SF << "  down      " 
           << (*it) 
           << endl;
      }
      for ( QStringList::Iterator it = Data.PostDown_SL.begin(); 
            it != Data.PostDown_SL.end(); 
            ++it ) {
        SF << "  post-down " 
           << (*it) 
           << endl;
      }
      rvl = 0;
    }

    // embed other info in it
    rvd = networkSetup()->getToplevel()->generateFileEmbedded( SF, DevNr );

    return (rvd == 2 || rvl == 2 ) ? 2 :
           (rvd == 0 || rvl == 0 ) ? 0 : 1;
}
