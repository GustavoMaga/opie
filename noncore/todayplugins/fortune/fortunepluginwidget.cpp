/*
 * fortunepluginwidget.cpp
 *
 * copyright   : (c) 2002 by Maximilian Rei�
 * email       : harlekin@handhelds.org
 *
 */
/***************************************************************************
 *   *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.     *
 *   *
 ***************************************************************************/

#include "fortunepluginwidget.h"

#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>

#include <qvaluelist.h>
#include <qtl.h>
#include <qstring.h>
#include <qscrollview.h>
#include <qobject.h>
#include <qlayout.h>

using namespace Opie::Core;
using namespace Opie::Ui;
FortunePluginWidget::FortunePluginWidget( QWidget *parent,  const char* name )
	: QWidget( parent, name )
{

	fortune = NULL;
	getFortune();
}

FortunePluginWidget::~FortunePluginWidget() {
	if( fortuneProcess ){
		delete fortuneProcess;
	}
}

/**
 * Get the fortunes
 */
void FortunePluginWidget::getFortune() {

	QVBoxLayout* layoutFortune = new QVBoxLayout( this );

	if ( fortune ) {
		delete fortune;
	}

	fortune = new OTicker( this );
//	fortune->setReadOnly( TRUE );
//	fortune->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );

	fortune->setText( QString("Obtaining fortune...") );
	layoutFortune->addWidget( fortune );

	fortuneProcess = new OProcess();
	*fortuneProcess << "fortune";

	connect(fortuneProcess,  SIGNAL(receivedStdout(Opie::Core::OProcess*,char*,int) ),
		this,  SLOT(slotStdOut(Opie::Core::OProcess*,char*,int) ) );

	if(!fortuneProcess->start(OProcess::NotifyOnExit, OProcess::AllOutput) ) {
		qWarning("could not start :(");
		fortune->setText( QString("Failed to obtain fortune.") );
		delete fortuneProcess;
		fortuneProcess = 0;
	}

}

void FortunePluginWidget::slotStdOut( OProcess* /*proc*/, char* buf, int len )
{
	QCString s( buf, len );
	s.replace( QRegExp("\n"), "" );
	fortune->setText( s );
}
