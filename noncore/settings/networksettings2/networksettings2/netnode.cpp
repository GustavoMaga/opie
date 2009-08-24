#include <time.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>

#include <qpainter.h>
#include <qbitmap.h>
#include <qtextstream.h>
#include <qpixmap.h>

#include "resources.h"
#include "netnode.h"

static char * ActionName[] = {
    "Disable",
    "Enable",
    "Activate",
    "Deactivate",
    "Up",
    "Down"
};

static char * StateName[] = {
    "Unchecked",
    "Unknown",
    "Unavailable",
    "Disabled",
    "Off",
    "Available",
    "IsUp"
};

QString & deQuote( QString & X ) {
    if( X[0] == '"' ) {
      // remove end and trailing "" and \x -> x
      QChar R;
      long idx;
      idx = X.length()-1;
      X = X.mid( 1, idx );

      idx = 0;
      while( ( idx = X.find( '\\', idx ) ) >= 0 ) {
        R = X.at( idx + 1 );
        X.replace( idx, 2, &R, 1 );
      }
      X = X.left( X.length()-1 );
    }
    return X;
}

QString quote( QString X ) {
    if( X.find( QRegExp( "[ \n\"\\\t]" ) ) >= 0 ) {
      // need to quote this
      QString OutString = "\"";

      X.replace( QRegExp("\""), "\\\"" );
      X.replace( QRegExp("\\"), "\\\\" );
      X.replace( QRegExp(" "), "\\ " );

      OutString += X;
      OutString += "\"";
      X = OutString;
    }
    return X;
}


//
//
// ANETNODE
//
//

void ANetNode::saveAttributes( QTextStream & TS ) {
    saveSpecificAttribute( TS );
}

void ANetNode::setAttribute( QString & Attr, QString & Value ){
    setSpecificAttribute( Attr, Value );
}

bool ANetNode::isToplevel( void ) {
    const char ** P = provides();
    while( *P ) {
      if( strcmp( *P, "fullsetup") == 0 )
        return 1;
      P ++;
    }
    return 0;
}

bool ANetNode::openFile( SystemFile & SF,
                         ANetNodeInstance * NNI,
                         QStringList & SL ) {
    return (NNI ) ? NNI->openFile( SF, SL ) : 0 ;
}

//
//
// ANETNODEINSTANCE
//
//

long ANetNodeInstance::InstanceCounter = -1;

void ANetNodeInstance::initialize( void ) {
    if( InstanceCounter == -1 )
      InstanceCounter = time(0);
    // set name
    QString N;
    N.sprintf( "-%ld", InstanceCounter++ );
    N.prepend( NodeType->name() );
    setName( N.latin1() );
}

void ANetNodeInstance::setAttribute( QString & Attr, QString & Value ){
    if( Attr == "__name" ) {
      setName( Value.latin1() );
    } else {
      setSpecificAttribute( Attr, Value );
    }
}

void ANetNodeInstance::saveAttributes( QTextStream & TS ) {
    TS << "__name=" << name() << endl;
    saveSpecificAttribute( TS );
}

ANetNodeInstance * ANetNodeInstance::nextNode( void ) {
    return networkSetup()->findNext( this );
}

//
//
// NODECOLLECTION
//
//

NetworkSetup::NetworkSetup( void ) : QList<ANetNodeInstance>() {
    IsModified = 0;
    Index = -1;
    Name="";
    IsNew = 1;
    CurrentState = Unchecked;
    AssignedInterface = 0;
    Number = -1;
    Done = 0;
}

NetworkSetup::NetworkSetup( QTextStream & TS, bool & Dangling ) :
      QList<ANetNodeInstance>() {
    long idx;
    QString S, A, N;

    Number = -1;
    Done = 0;
    IsModified = 0;
    Index = -1;
    Name="";
    IsNew = 0;
    AssignedInterface = 0;
    CurrentState = Unchecked;

    Dangling = 0; // by default node collection is ok

    do {
      S = TS.readLine();
      if( S.isEmpty() ) {
        // empty line
        break;
      }

      idx = S.find('=');
      S.stripWhiteSpace();
      A = S.left( idx );
      A.lower();
      N = S.mid( idx+1, S.length() );
      N.stripWhiteSpace();
      N = deQuote( N );

      if( A == "name" ) {
        Name = N;
      } else if( A == "number" ) {
        setNumber( N.toLong() );
      } else if( A == "node" ) {
        ANetNodeInstance * NNI = NSResources->findNodeInstance( N );
        Log(( "Find node %s : %p\n", N.latin1(), NNI ));
        if( NNI ) {
          append( NNI );
        } else {
          // could not find a node type -> collection invalid
          Log(( "Node %s missing -> NetworkSetup dangling\n",
                      N.latin1() ));
          // create placeholder for this dangling NNI
          NNI = new ErrorNNI( N );
          Dangling = 1;
        }
      }
    } while( 1 );

    Log(( "Profile number %s : %d nodes\n",
          Name.latin1(), count() ));
}

NetworkSetup::~NetworkSetup( void ) {
}

const QString & NetworkSetup::description( void ) {
    ANetNodeInstance * NNI = getToplevel();
    return (NNI) ? NNI->runtime()->description() : Name;
}

void NetworkSetup::append( ANetNodeInstance * NNI ) {
    NNI->setNetworkSetup( this );
    QList<ANetNodeInstance>::append( NNI );
}

void NetworkSetup::save( QTextStream & TS ) {

    TS << "name=" << quote( Name ) << endl;
    TS << "number=" << number() << endl;
    ANetNodeInstance * NNI;
    for( QListIterator<ANetNodeInstance> it(*this);
         it.current();
         ++it ) {
      NNI = it.current();
      TS << "node=" << NNI->name() << endl;
    }
    TS << endl;
    IsNew = 0;
}

ANetNodeInstance * NetworkSetup::getToplevel( void ) {
    ANetNodeInstance * NNI = 0;
    for( QListIterator<ANetNodeInstance> it(*this);
         it.current();
         ++it ) {
      NNI = it.current();
      if( NNI->nodeClass()->isToplevel() ) {
        return NNI;
      }
    }
    return 0;
}

ANetNodeInstance * NetworkSetup::findByName( const QString & S ) {
    ANetNodeInstance * NNI = 0;
    for( QListIterator<ANetNodeInstance> it(*this);
         it.current();
         ++it ) {
      NNI = it.current();
      if( NNI->name() == S ) {
        return NNI;
      }
    }
    return 0;
}

ANetNodeInstance * NetworkSetup::findNext( ANetNodeInstance * NNI ) {
    ANetNodeInstance * NNNI;

    if( ! NNI )
      getToplevel();

    for( QListIterator<ANetNodeInstance> it(*this);
         it.current();
         ++it ) {
      NNNI = it.current();
      if( NNNI == NNI ) {
        ++it;
        return it.current();
      }
    }
    return 0; // no more next
}

int NetworkSetup::compareItems( QCollection::Item I1,
                                  QCollection::Item I2 ) {
    ANetNodeInstance * NNI1, * NNI2;
    NNI1 = (ANetNodeInstance *)I1;
    NNI2 = (ANetNodeInstance *)I2;
    return strcmp( NNI1->name(), NNI2->name() );
}

static char * State2PixmapTbl[] = {
    "NULL", // Unchecked : no pixmap
    "check", // Unknown
    "delete", // unavailable
    "disabled", // disabled
    "off", // off
    "disconnected", // available
    "connected" // up
};

QPixmap NetworkSetup::devicePixmap( void ) {
    QPixmap pm = NSResources->getPixmap(
            getToplevel()->nextNode()->pixmapName()+"-large");

    QPixmap Mini = NSResources->getPixmap(
            device()->netNode()->pixmapName() );

    if( pm.isNull() || Mini.isNull() )
	return Resource::loadPixmap("Unknown");

    QPainter painter( &pm );
    painter.drawPixmap( pm.width()-Mini.width(),
                        pm.height()-Mini.height(),
                        Mini );
    pm.setMask( pm.createHeuristicMask( TRUE ) );
    return pm;
}

QPixmap NetworkSetup::statePixmap( State_t S) {
    return NSResources->getPixmap( State2PixmapTbl[S] );
}

QString NetworkSetup::stateName( State_t S) {
    switch( S ) {
      case Unknown :
        return qApp->translate( "networksettings2", "Unknown");
      case Unavailable :
        return qApp->translate( "networksettings2", "Unavailable");
      case Disabled :
        return qApp->translate( "networksettings2", "Disabled");
      case Off :
        return qApp->translate( "networksettings2", "Inactive");
      case Available :
        return qApp->translate( "networksettings2", "Available");
      case IsUp :
        return qApp->translate( "networksettings2", "Up");
      case Unchecked : /* FT */
      default :
        break;
    }
    return QString("");
}

void NetworkSetup::reassign( void ) {
    for( QListIterator<ANetNodeInstance> it(*this);
         it.current();
         ++it ) {
      it.current()->setNetworkSetup( this );
    }
}

const QStringList & NetworkSetup::triggers() {
    return getToplevel()->runtime()->triggers();
}

bool NetworkSetup::hasDataForFile( SystemFile & S ) {
    return ( firstWithDataForFile( S ) != 0 );
}

ANetNodeInstance * NetworkSetup::firstWithDataForFile( SystemFile & S ) {
    for( QListIterator<ANetNodeInstance> it(*this);
         it.current();
         ++it ) {
      if( it.current()->hasDataForFile( S ) ) {
        return it.current();
      }
    }
    return 0;
}

State_t NetworkSetup::state( bool Update ) {
    State_t NodeState;

    if( CurrentState == Unchecked || Update ) {
      // collect states of all nodes until with get the 'higest'
      // state possible

      Log(( "NetworkSetup %s state %s\n",
                Name.latin1(), StateName[CurrentState] ));

      CurrentState = Unknown;
      for( QListIterator<ANetNodeInstance> it(*this);
           it.current();
           ++it ) {
        Log(( "-> Detect %s\n", it.current()->name() ));
        NodeState = it.current()->runtime()->detectState();
        Log(( "   state %s\n", StateName[NodeState] ));

        if( NodeState == Disabled ||
            NodeState == IsUp ) {
          // max
          CurrentState = NodeState;
          break;
        }

        if( NodeState > CurrentState ) {
          // higher
          CurrentState = NodeState;
        }
      }
    }

    return CurrentState;
}

QString NetworkSetup::setState( Action_t A, bool Force ) {

    QString msg;
    Action_t Actions[10];
    int NoOfActions = 0;

    // get current state
    state( Force );

    switch( A ) {
      case Disable :
        /*
        if( CurrentState < Disabled ) {
          // disabled
          CurrentState = Disabled;
          return QString();
        }
        */

        if( CurrentState == IsUp ) {
          Actions[NoOfActions++] = Down;
          Actions[NoOfActions++] = Deactivate;
        } else if( CurrentState == Available ) {
          Actions[NoOfActions++] = Deactivate;
        }
        Actions[NoOfActions++] = Disable;
        break;
      case Enable :
        // always possible -> detected state is new state
        Actions[NoOfActions++] = Enable;
        break;
      case Activate :
        if( ! Force ) {
          if( CurrentState >= Available ) {
            // already available
            return QString();
          }

          if( CurrentState != Off ) {
            return qApp->translate( "System",
                                    "State should be off" );
          }
        }

        Actions[NoOfActions++] = Activate;
        break;
      case Deactivate :
        if( ! Force ) {
          if( CurrentState < Off ) {
            // already inactive
            return QString();
          }
        }

        if( CurrentState == IsUp ) {
          Actions[NoOfActions++] = Down;
        }
        Actions[NoOfActions++] = Deactivate;
        break;
      case Up :
        if( ! Force ) {
          if( CurrentState == IsUp ) {
            return QString();
          }
          if( CurrentState < Off ) {
            return qApp->translate( "System",
                                    "State should at least be off" );
          }
        }
        if( CurrentState == Off ) {
          Actions[NoOfActions++] = Activate;
        }
        Actions[NoOfActions++] = Up;
        break;
      case Down :
        if( ! Force ) {
          if( CurrentState < Available ) {
            // OK
            return QString();
          }
        }
        Actions[NoOfActions++] = Down;
        break;
    }

    // send actions to all nodes
    Log(( "Action %s requires %d steps\n",
          ActionName[A], NoOfActions ));

    for( int i = 0 ; i < NoOfActions; i ++ ) {
      // setState recurses through the tree depth first
      msg = getToplevel()->runtime()->setState( this, Actions[i], Force );
      if( ! msg.isEmpty() ) {
        return msg;
      }
    }
    return QString();
}

void NetworkSetup::copyFrom( const NetworkSetup & N ) {
    Number = N.Number;
    CurrentState = N.CurrentState;
    Name = N.Name;
    IsNew = N.IsNew;
    Index = N.Index;
    AssignedInterface = N.AssignedInterface;
}

//
//
// RuntimeInfo
//
//

QString RuntimeInfo::setState( NetworkSetup * NC,
                               Action_t A,
                               bool Force ) {
    QString M;
    RuntimeInfo * Deeper = nextNode();

    if( Deeper ) {
      // first go deeper
      M = Deeper->setState( NC, A, Force );
      if( ! M.isEmpty() )
        return M;
    }

    // set my own state
    Log (( "-> Act upon %s\n", netNode()->name() ));
    M = setMyState( NC, A, Force );
    Log (( "   result %s\n", M.latin1() ));
    return M;
}
