
/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : March 10, 2002
    copyright            : (C) 2002 by llornkcor
    email                : ljp@llornkcor.com
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include <qpe/qpeapplication.h>



#include "advancedfm.h"

#ifdef NOQUICKLAUNCH
int main(int argc, char *argv[])
{
    QPEApplication a(argc, argv);

    AdvancedFm advencedFm;
    a.showMainWidget( &advencedFm);
    return a.exec();
}
#else
#include <opie/oapplicationfactory.h>

OPIE_EXPORT_APP( OApplicationFactory<AdvancedFm> )

#endif
