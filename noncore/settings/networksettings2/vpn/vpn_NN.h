#ifndef VPN_NETNODE_H
#define VPN_NETNODE_H

#include "netnode.h"

class AVPN;

class VPNNetNode : public ANetNode{

    Q_OBJECT

public:

    VPNNetNode();
    virtual ~VPNNetNode();

    virtual const QString pixmapName() 
      { return "Devices/vpn"; }

    virtual const QString nodeDescription() ;
    virtual ANetNodeInstance * createInstance( void );
    virtual const char ** needs( void );
    virtual const char * provides( void );

private:

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );
};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
