#include "profile_NN.h"
#include "profile_NNI.h"

static const char * ProfileNeeds[] = 
    { "connection", 
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
ProfileNetNode::ProfileNetNode() : ANetNode() {
}

/**
 * Delete any interfaces that we own.
 */
ProfileNetNode::~ProfileNetNode(){
}

const QString ProfileNetNode::nodeDescription(){
      return tr("\
<p>Define use of an IP connection.</p>\
<p>Configure if and when this connection needs to be established</p>\
"
);
}

ANetNodeInstance * ProfileNetNode::createInstance( void ) {
      return new AProfile( this );
}

const char ** ProfileNetNode::needs( void ) {
      return ProfileNeeds;
}

const char * ProfileNetNode::provides( void ) {
      return "fullsetup";
}

bool ProfileNetNode::generateProperFilesFor( 
            ANetNodeInstance * ) {
      return 1;
}

bool ProfileNetNode::hasDataFor( const QString &, bool  ) {
      return 0;
}

bool ProfileNetNode::generateDataForCommonFile( 
                                SystemFile & , 
                                long,
                                ANetNodeInstance * ) {
      return 1;
}

bool ProfileNetNode::generateDeviceDataForCommonFile( 
                                SystemFile & , 
                                long ,
                                ANetNodeInstance * ) {
      return 1;
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new ProfileNetNode() );
}
}
