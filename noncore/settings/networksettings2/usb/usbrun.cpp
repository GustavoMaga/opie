#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <resources.h>
#include "usbrun.h"

State_t USBRun::detectState( void ) {

    // unavailable : no card found
    // available : card found and assigned to us or free
    // up : card found and assigned to us and up

    NetworkSetup * NC = networkSetup();
    QString S = QString( "/tmp/profile-%1.up" ).arg(NC->number());
    System & Sys = NSResources->system();
    InterfaceInfo * Run;
    QFile F( S );
    Log(("Detecting for %s\n", NC->name().latin1() ));

    if( F.open( IO_ReadOnly ) ) {
      // could open file -> read interface and assign
      QString X;
      QTextStream TS(&F);
      X = TS.readLine();
      Log(("%s exists\n", S.latin1() ));
      // find interface
      if( handlesInterface( X ) ) {
        for( QDictIterator<InterfaceInfo> It(Sys.interfaces());
             It.current();
             ++It ) {
          Run = It.current();
          if( X == Run->Name ) {
            NC->assignInterface( Run );
            return (Run->IsUp) ? IsUp : Available;
          }
        }
      }
    } 

    Log(("Assigned %p\n", NC->assignedInterface() ));

    if( ( Run = NC->assignedInterface() ) ) {
      // we already have an interface assigned -> still present ?
      if( ! Run->IsUp ) {
        // usb is still free -> keep assignment
        return Available;
      } // else interface is up but NOT us -> some other profile
    }

    // nothing (valid) assigned to us
    NC->assignInterface( 0 );

    // find possible interface
    if( getInterface() ) {
      // proper type, and Not UP -> free
      // usb cables are currently always available when requested
      // until we can detect if we are plugged in
      return Available;
    }

    return Unavailable;
}

QString USBRun::setMyState( NetworkSetup * NC, Action_t, bool ) {

    InterfaceInfo * I = getInterface();

    if( ! I ) {
      return QString("No usb device available");
    }

    Log(( "Grabbed USB interface %s\n", I->Name.latin1() ));
    // grab this interface
    NC->assignInterface( I );

    return QString();
}

// get interface that is free or assigned to us
InterfaceInfo * USBRun::getInterface( void ) {

    System & S = NSResources->system();
    InterfaceInfo * best = 0, * Run;

    for( QDictIterator<InterfaceInfo> It(S.interfaces());
         It.current();
         ++It ) {
      Run = It.current();

      Log(("%s %d %d=%d %d\n",
          Run->Name.latin1(),
          handlesInterface( Run->Name ),
              Run->CardType, ARPHRD_ETHER,
              ! Run->IsUp ));

      if( handlesInterface( Run->Name ) &&
          Run->CardType == ARPHRD_ETHER
        ) {
        // this is a USB card
        if( Run->assignedToNetworkSetup() == netNode()->networkSetup() ) {
          // assigned to us
          return Run;
        } else if( ! Run->IsUp &&
                   Run->assignedToNetworkSetup() == 0 ) {
          // free
          best = Run;
        } // UP or not assigned to us
      }
    }
    return best; // can be 0
}

bool USBRun::handlesInterface( const QString & S ) {
    return Pat.match( S ) >= 0;
}

bool USBRun::handlesInterface( InterfaceInfo * I ) {
    return handlesInterface( I->Name );
}

