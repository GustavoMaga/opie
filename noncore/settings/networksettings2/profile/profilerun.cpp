#include <resources.h>

#include "profilerun.h"

State_t ProfileRun::detectState( void ) { 

    Log(( "Profile %sabled\n", (Data->Disabled) ? "dis" : "en" ));

    if( Data->Disabled ) {
      return Disabled;
    } 
    return Unknown;
}

QString ProfileRun::setMyState( NetworkSetup * NC, Action_t A, bool ) { 
    odebug << "Profile " << Data->Disabled << oendl;
    if( A == Disable ) {
      if( ! Data->Disabled ) {
        Data->Disabled = 1;
        NC->setModified( 1 );
      }
    } else if( A == Enable ) {
      if( Data->Disabled ) { 
        Data->Disabled = 0;
        NC->setModified( 1 );
      }
    }

    return QString();
}
