/*
Opie-uBrowser.  a very small web browser, using on QTextBrowser for html display/parsing
Copyright (C) 2002 Thomas Stephens

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#include "mainview.h"

MainView::MainView(QWidget *parent, const char *name, WFlags fl) : QMainWindow(parent, name, fl)
{
	setIcon( Resource::loadPixmap( "remote" ) );
	setCaption(tr("uBrowser"));
	
	setToolBarsMovable( false );

	QPEToolBar *toolbar = new QPEToolBar(this, "toolbar");
	back = new QToolButton(Resource::loadPixmap("ubrowser/back"), 0, 0, 0, 0, toolbar, "back");
	forward = new QToolButton(Resource::loadPixmap("ubrowser/forward"), 0, 0, 0, 0, toolbar, "forward");
	home = new QToolButton(Resource::loadPixmap("ubrowser/home"), 0, 0, 0, 0, toolbar, "home");
	location = new QComboBox(true, toolbar, "location");
	go = new QToolButton(Resource::loadPixmap("ubrowser/go"), 0, 0, 0, 0, toolbar, "go");

	toolbar->setStretchableWidget(location);
	toolbar->setHorizontalStretchable(true);
	location->setAutoCompletion( true );

	addToolBar(toolbar);

	browser = new QTextBrowser(this, "browser");
	setCentralWidget(browser);

//make the button take you to the location
	connect(go, SIGNAL(clicked()), this, SLOT(goClicked()) );
	connect(location->lineEdit(), SIGNAL(returnPressed()), this, SLOT(goClicked()) );

//make back, forward and home do their thing (isnt QTextBrowser great?)
	connect(back, SIGNAL(clicked()), browser, SLOT(backward()) );
	connect(forward, SIGNAL(clicked()), browser, SLOT(forward()) );
	connect(home, SIGNAL(clicked()), browser, SLOT(home()) );
	
//make back and forward buttons be enabled, only when you can go back or forward (again, i love QTextBrowser)
//this doesnt seem to work, but doesnt break anything either...
	connect(browser, SIGNAL(backwardAvailable(bool)), back, SLOT(setOn(bool)) );
	connect(browser, SIGNAL(forwardAvailable(bool)), forward, SLOT(setOn(bool)) );

//notify me when the text of the browser has changed (like when the user clicks a link)
	connect(browser, SIGNAL(textChanged()), this, SLOT(textChanged()) );

	http = new HttpFactory(browser);
	
	if( qApp->argc() > 1 )
	{
		char **argv = qApp->argv();
		int i = 0;
		QString *openfile = new QString( argv[0] );
		while( openfile->contains( "ubrowser" ) == 0 && i < qApp->argc() )
		{
			i++;
			*openfile = argv[i];
		}
		*openfile = argv[i+1];
		if( !openfile->startsWith( "http://" ) && !openfile->startsWith( "/" ) )
		{
			openfile->insert( 0, QDir::currentDirPath()+"/" );
		}
		location->setEditText( *openfile );
		goClicked();
	}
}

void MainView::goClicked()
{
	location->insertItem( location->currentText() );
	
	if(location->currentText().startsWith("http://") )
	{
		location->setEditText(location->currentText().lower());
		browser->setMimeSourceFactory(http);
		printf("MainView::goClicked: using http source factory\n");
	}
	else
	{
		browser->setMimeSourceFactory(QMimeSourceFactory::defaultFactory());
		printf("MainView::goClicked: using default source factory\n");
	}

	browser->setSource(location->currentText());
}

void MainView::textChanged()
{
	if(browser->documentTitle().isNull())
	{
		setCaption( tr("%1 - uBrowser").arg( browser->source() ) );
	}
	else
	{
		setCaption(tr(" - uBrowser").arg( browser->documentTitle() ));
	}

	location->setEditText(browser->source());
}

void MainView::setDocument( const QString& applnk_filename )
{
	DocLnk *file = new DocLnk( applnk_filename );
	
	location->setEditText( file->file() );
	goClicked();
}
