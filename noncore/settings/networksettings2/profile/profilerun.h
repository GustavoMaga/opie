#ifndef PROFILERUN_H
#define PROFILERUN_H

#include <asfullsetup.h>
#include "profiledata.h"

class ProfileRun  : public AsFullSetup {

public :

      ProfileRun( ANetNodeInstance * NNI, ProfileData & D ) : 
            AsFullSetup( NNI )
        { Data = &D;  }

      void detectState( NodeCollection * NC );
      bool setState( NodeCollection * NC, Action_t A, bool );
      bool canSetState( State_t Curr, Action_t A );

      bool handlesInterface( const QString & I );

      virtual const QString & description( void )
        { return Data->Description; }

      virtual AsFullSetup * asFullSetup( void ) 
        { return (AsFullSetup *)this; }

      virtual bool triggersVPN( void ) 
        { return Data->TriggerVPN; }

private :

      ProfileData * Data;

};
#endif
