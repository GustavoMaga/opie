#include "wlan_NN.h"
#include "wlan_NNI.h"

static const char * WLanNeeds[] = 
    { 0
    };

/**
 * Constructor, find all of the possible interfaces
 */
WLanNetNode::WLanNetNode() : ANetNode(tr("WLan Device")) {
    InstanceCount = 2;
}

/**
 * Delete any interfaces that we own.
 */
WLanNetNode::~WLanNetNode(){
}

const QString WLanNetNode::nodeDescription(){
      return tr("\
<p>Configure Wi/Fi or WLan network cards.</p>\
<p>Defines Wireless options for those cards</p>\
"
);
}

ANetNodeInstance * WLanNetNode::createInstance( void ) {
      return new AWLan( this );
}

const char ** WLanNetNode::needs( void ) {
      return WLanNeeds;
}

const char * WLanNetNode::provides( void ) {
      return "device";
}

QString WLanNetNode::genNic( long nr ) { 
      QString S; 
      return S.sprintf( "wlan%ld", nr );
}

void WLanNetNode::setSpecificAttribute( QString & A, QString & V ) {
      if( A == "interfacecount" ) {
        InstanceCount = V.toLong();
      }
}

void WLanNetNode::saveSpecificAttribute( QTextStream & TS ) {
      TS << "interfacecount="
         << InstanceCount
         << endl;
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new WLanNetNode() );
}
}
