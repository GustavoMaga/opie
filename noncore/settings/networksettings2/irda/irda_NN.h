#ifndef IRDA_NETNODE_H
#define IRDA_NETNODE_H

#include "netnode.h"

class AIRDA;

class IRDANetNode : public ANetNode {

      Q_OBJECT

public:

      IRDANetNode();
      virtual ~IRDANetNode();

      virtual const QString pixmapName()
        { return "Devices/irda"; }

      virtual const QString nodeDescription() ;
      virtual ANetNodeInstance * createInstance( void );
      virtual const char ** needs( void );
      virtual const char ** provides( void );

private:

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );
};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
