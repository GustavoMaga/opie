#include "vpndata.h"
#include "vpnGUI.h"

class VPNEdit  : public VPNGUI {

public :

    VPNEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( VPNData & Data );
    void showData( VPNData & Data );
};
