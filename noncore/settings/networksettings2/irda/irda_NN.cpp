#include "irda_NN.h"
#include "irda_NNI.h"

#include "netnodeinterface.h"

static const char * IRDANeeds[] =
    { 0
    };

static const char * IRDAProvides[] =
    { "line",
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
IRDANetNode::IRDANetNode() : ANetNode(tr("Infrared link")) {
}

/**
 * Delete any interfaces that we own.
 */
IRDANetNode::~IRDANetNode(){
}

const QString IRDANetNode::nodeDescription(){
      return tr("\
<p>Sets up a infra red serial link.</p>\
"
);
}

ANetNodeInstance * IRDANetNode::createInstance( void ) {
      return new AIRDA( this );
}

const char ** IRDANetNode::needs( void ) {
      return IRDANeeds;
}

const char ** IRDANetNode::provides( void ) {
      return IRDAProvides;
}

void IRDANetNode::setSpecificAttribute( QString & , QString & ) {
}

void IRDANetNode::saveSpecificAttribute( QTextStream & ) {
}

OPIE_NS2_PLUGIN(  NetNodeInterface_T<IRDANetNode> )
