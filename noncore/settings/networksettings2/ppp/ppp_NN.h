#ifndef PPP_NETNODE_H
#define PPP_NETNODE_H

#include "netnode.h"

class APPP;

class PPPNetNode : public ANetNode{

    Q_OBJECT

public:

    PPPNetNode();
    virtual ~PPPNetNode();

    virtual const QString pixmapName() 
      { return "ppp"; }

    virtual const QString nodeName() 
      { return tr("PPP Connection"); }

    virtual const QString nodeDescription() ;

    virtual ANetNodeInstance * createInstance( void );

    virtual const char ** needs( void );
    virtual const char * provides( void );

    virtual bool generateProperFilesFor( ANetNodeInstance * NNI );
    virtual bool hasDataFor( const QString & S );
    virtual bool generateDataForCommonFile( 
        SystemFile & SF, long DevNr, ANetNodeInstance * NNI );

private:

};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
