#include "interfaceinformationppp.h"
#include "connect.h"
#include "conwindow.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qpushbutton.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qabstractlayout.h>

#ifdef QWS
#else
 #define showMaximized show
#endif

/**
 * Constructor for the InterfaceInformationImp class.  This class pretty much
 * just display's information about the interface that is passed to it.
 */
InterfaceInformationPPP::InterfaceInformationPPP(QWidget *parent, const char *name, Interface *i, WFlags )
    :InterfaceInformationImp(parent, name, i, Qt::WStyle_ContextHelp)
{
    odebug << "InterfaceInformationPPP::InterfaceInformationPPP " << name << "" << oendl;
    con = new ConnectWidget( (InterfacePPP*)i, this, "con" );
    con->setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding,
                                    QSizePolicy::Fixed) );

    macAddressLabel->hide();
    subnetMaskLabel->hide();
    broadcastLabel->hide();
    TextLabel23->hide();
    TextLabel21->hide();
    TextLabel24->hide();

     InterfaceInformationLayout->addWidget( con, 1, 0 );
     connect(i, SIGNAL(begin_connect()),con, SLOT(preinit()));
     connect(i, SIGNAL(hangup_now() ), con, SLOT(cancelbutton() ) );
}



