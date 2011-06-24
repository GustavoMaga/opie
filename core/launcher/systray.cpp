/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "systray.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qtopia/qpeapplication.h>
#include <qtopia/qlibrary.h>
#include <qtopia/config.h>
using namespace Opie::Core;

/* QT */
#include <qlayout.h>
#include <qdir.h>

/* STD */
#include <stdlib.h>

SysTray::SysTray( QWidget *parent ) : QFrame( parent ), layout(0)
{
    //setFrameStyle( QFrame::Panel | QFrame::Sunken );
    loadApplets();
}

SysTray::~SysTray()
{
    clearApplets();
}

static int compareAppletPositions(const void *a, const void *b)
{
    const TaskbarApplet* aa = *(const TaskbarApplet**)a;
    const TaskbarApplet* ab = *(const TaskbarApplet**)b;
    int d = ab->iface->position() - aa->iface->position();
    if ( d )
        return d;
    return QString::compare(ab->name,aa->name);
}

void SysTray::loadApplets()
{
    hide();
    clearApplets();
    addApplets();
}

void SysTray::clearApplets()
{
#ifndef QT_NO_COMPONENTS

    /*
     * Note on clearing. SOme applets delete their
     * applets themselves some don't do it
     * and on restart this can crash. If we delete it
     * here we might end up in a double deletion. We could
     * use QGuardedPtr but that would be one QOBject
     * for every applet more but only useful for restart
     */
    QValueList<TaskbarApplet>::Iterator mit;
    for ( mit = appletList.begin(); mit != appletList.end(); ++mit ) {
        (*mit).iface->release();
        (*mit).library->unload();
        delete (*mit).library;
    }

#endif
    appletList.clear();
    if ( layout )
        delete layout;
    layout = new QHBoxLayout( this, 0, 1 );
    layout->setAutoAdd(TRUE);
}

void SysTray::addApplets()
{
    hide();
#ifndef QT_NO_COMPONENTS
    Config cfg( "Taskbar" );
    cfg.setGroup( "Applets" );
    QStringList exclude = cfg.readListEntry( "ExcludeApplets", ',' );

    QString lang = getenv( "LANG" );
    QString path = QPEApplication::qpeDir() + "plugins/applets";
#ifdef Q_OS_MACX
    QDir dir( path, "lib*.dylib" );
#else
    QDir dir( path, "lib*.so" );
#endif /* Q_OS_MACX */
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    int napplets=0;
    TaskbarApplet* *applets = new TaskbarApplet*[list.count()];
    for ( it = list.begin(); it != list.end(); ++it ) {
        if ( exclude.find( *it ) != exclude.end() )
            continue;
        owarn << "Found Applet: " << (*it) << "" << oendl;
        TaskbarAppletInterface *iface = 0;
        QLibrary *lib = new QLibrary( path + "/" + *it );
        if (( lib->queryInterface( IID_TaskbarApplet, (QUnknownInterface**)&iface ) == QS_OK ) && iface ) {
            TaskbarApplet *applet = new TaskbarApplet;
            applets[napplets++] = applet;
            applet->library = lib;
            applet->iface = iface;

            QTranslator *trans = new QTranslator(qApp);
            QString type = (*it).left( (*it).find(".") );
            QString tfn = QPEApplication::qpeDir()+"i18n/"+lang+"/"+type+".qm";
            if ( trans->load( tfn ))
                qApp->installTranslator( trans );
            else
                delete trans;
        }
        else {
            exclude += *it;
            delete lib;
        }
    }
    cfg.writeEntry( "ExcludeApplets", exclude, ',' );
    qsort(applets,napplets,sizeof(applets[0]),compareAppletPositions);
    while (napplets--) {
        TaskbarApplet *applet = applets[napplets];
        applet->applet = applet->iface->applet( this );
        appletList.append(*applet);
    }
    delete [] applets;
#else /* ## FIXME single app */
    TaskbarApplet * const applet = new TaskbarApplet();
    applet->iface = new ClockAppletImpl();
    applet->applet = applet->iface->applet( this );
    appletList.append( applet );
#endif
    show();
}

