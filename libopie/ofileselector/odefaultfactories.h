#ifndef OPIE_DEFAULT_FACTORIES_H
#define OPIE_DEFAULT_FACTORIES_H

#include "ofilefactory.h"

extern "C" {

    OFileView* newFileIconView( OFileSelector*, QWidget* parent );
    OFileView* newFileListView( OFileSelector*, QWidget* parent );

    OLister* newLocalLister(OFileSelector* );


};


#endif
