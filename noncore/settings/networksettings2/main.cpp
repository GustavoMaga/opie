#include "nsdata.h"
#include "activateprofile.h"
#include "activatevpn.h"
#include "networksettings.h"

#include <qpe/qpeapplication.h>

#include <opie2/oapplicationfactory.h>
using namespace Opie::Core;

#ifdef GONE

OPIE_EXPORT_APP( OApplicationFactory<NetworkSettings> )

#else

// just standard GUI
#define ACT_GUI         0
// used by interfaces to request for allow of up/down
#define ACT_REQUEST     1
// regenerate config files
#define ACT_REGEN       2
// used by interfaces to request user prompt 
#define ACT_PROMPT      3
// used by interfaces to trigger VPN
#define ACT_VPN         4

int main( int argc, char * argv[] ) {
        int rv = 0;
        int Action = ACT_GUI;
        // could be overruled by -qws
        QApplication::Type GuiType = QApplication::GuiClient; 

#ifdef _WS_QWS_
        QPEApplication * TheApp;
#else
        QApplication * TheApp;
#endif

        for ( int i = 1; i < argc; i ++ ) {
          int rmv;
          rmv = 0;
          if( strcmp( argv[i], "--regen" ) == 0 ) {
            Action = ACT_REGEN;
            GuiType = QApplication::Tty;
            rmv = 1;
          } else if( strcmp( argv[i], "--prompt" ) == 0 ) {
            Action = ACT_PROMPT;
            rmv = 1;
          } else if( strcmp( argv[i], "--triggervpn" ) == 0 ) {
            Action = ACT_VPN;
            rmv = 1;
          }
          if( rmv ) {
            memmove( argv+i, argv+i+rmv, 
                sizeof( char * ) * (argc-i-rmv) );
            i --;
            argc -= rmv;
          }
        }

        if( strstr( argv[0], "-request" ) ) {
          // called from system to request something
          GuiType = QApplication::Tty;
          Action = ACT_REQUEST;
        }

        // Start Qt
#ifdef _WS_QWS_
        // because QPEApplication does not handle GuiType well
        if( GuiType == QApplication::Tty ) {
          // this cast is NOT correct but we do not use
          // TheApp anymore ...
          TheApp = (QPEApplication *)new QApplication( argc, argv, GuiType );
        } else {
          TheApp = new QPEApplication( argc, argv, GuiType );
        }
#else
        TheApp = new QApplication( argc, argv, GuiType );
#endif

        // init qt with app widget

        switch( Action ) {
          case ACT_REQUEST :
            { NetworkSettingsData NS;
              if( NS.canStart( argv[1] ) ) {
                QString S;
                S.sprintf( QPEApplication::qpeDir()+
                           "/bin/networksettings2" );
                char * MyArgv[4];
                MyArgv[0] = "networksettings2";
                MyArgv[1] = "--prompt";
                MyArgv[2] = argv[1];
                MyArgv[3] = NULL;
                NSResources->system().execAsUser( S, MyArgv );
                // if we come here , failed
                printf( "%s-cNN-disallowed", argv[1] );
              }
            }
            break;
          case ACT_REGEN :
            { NetworkSettingsData NS;
              // regen returns 0 if OK
              rv = (NS.regenerate()) ? 1 : 0;
            }
            break;
          case ACT_PROMPT :
            { ActivateProfile AP(argv[1]);
              if( AP.exec() == QDialog::Accepted ) {
                printf( "%s-c%ld-allowed", argv[1], AP.selectedProfile() );
              } else {
                printf( "%s-cNN-disallowed", argv[1] );
              }
            }
            break;
          case ACT_VPN :
            { ActivateVPN AVPN;
              AVPN.exec();
            }
            break;
          case ACT_GUI :
            { QWidget * W = new NetworkSettings(0);
              TheApp->setMainWidget( W ); 
              W->show();
#ifdef _WS_QWS_
              W->showMaximized();
#else
              W->resize( W->sizeHint() );
#endif
              rv = TheApp->exec();
              delete W;
            }
            break;
        }

        LogClose();

        return rv;
}

#endif


// main.cpp

