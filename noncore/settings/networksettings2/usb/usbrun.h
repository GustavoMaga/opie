#ifndef USBRUN_H
#define USBRUN_H

#include <asdevice.h>
#include <qregexp.h>
#include "usbdata.h"

class USBRun  : public AsDevice {

public :

      USBRun( ANetNodeInstance * NNI, 
              USBData & Data ) : 
                AsDevice( NNI ),
                Pat( "usb[0-9abcdef]" )
        { }

      virtual AsDevice * device( void ) 
        { return (AsDevice *)this; }

      virtual AsDevice * asDevice( void ) 
        { return (AsDevice *)this; }
protected :

      void detectState( NodeCollection * );
      bool setState( NodeCollection * , Action_t A, bool );
      bool canSetState( State_t , Action_t A );

      bool handlesInterface( const QString & I );

private :

      InterfaceInfo * getInterface( void );
      QRegExp Pat;

};
#endif
