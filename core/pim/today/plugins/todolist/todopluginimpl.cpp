/*
 * todopluginimpl.cpp
 *
 * copyright   : (c) 2002, 2003, 2004 by Maximilian Rei�
 * email       : harlekin@handhelds.org
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "todoplugin.h"
#include "todopluginimpl.h"

TodolistPluginImpl::TodolistPluginImpl() {
    todolistPlugin  = new TodolistPlugin();
}

TodolistPluginImpl::~TodolistPluginImpl() {
    delete todolistPlugin;
}


TodayPluginObject* TodolistPluginImpl::guiPart() {
    return todolistPlugin;
}

QRESULT TodolistPluginImpl::queryInterface( const QUuid & uuid,  QUnknownInterface **iface ) {
    *iface = 0;
    if ( ( uuid == IID_QUnknown ) || ( uuid == IID_TodayPluginInterface ) )  {
        *iface = this, (*iface)->addRef();
    }else
	return QS_FALSE;

    return QS_OK;

}

Q_EXPORT_INTERFACE() {
    Q_CREATE_INSTANCE( TodolistPluginImpl );
}
