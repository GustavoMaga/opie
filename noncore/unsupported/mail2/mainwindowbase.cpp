#include <qpopupmenu.h>
#include <qaction.h>
#include <qheader.h>
#include <qlabel.h>
#include <qvbox.h>

#include <qtoolbar.h>
#include <qmenubar.h>
#include <qpe/resource.h>

#include "mainwindowbase.h"
#include "mailstatusbar.h"
#include "folderwidget.h"
#include "mailtable.h"

MainWindowBase::MainWindowBase(QWidget *parent, const char *name, WFlags fl)
	: QMainWindow(parent, name, fl)
{
	setCaption(tr("E-Mail"));
	setToolBarsMovable(false);

	toolbar = new QToolBar(this);
	menubar = new QMenuBar( toolbar );
	mailmenu = new QPopupMenu( menubar );
	servermenu = new QPopupMenu( menubar );
	menubar->insertItem( tr( "Mail" ), mailmenu );
	menubar->insertItem( tr( "Servers" ), servermenu );

	addToolBar(toolbar);
	toolbar->setHorizontalStretchable(true);

	QLabel *spacer = new QLabel(toolbar);
	spacer->setBackgroundMode(QWidget::PaletteButton);
	toolbar->setStretchableWidget(spacer);

	compose = new QAction(tr("Compose new mail"), QIconSet(Resource::loadPixmap("mail/newmail")), 0, 0, this);
	compose->addTo(toolbar);
	compose->addTo(mailmenu);

 	sendQueue = new QAction(tr("Send queued mails"), QIconSet(Resource::loadPixmap("mail/sendqueue")), 0, 0, this);
	sendQueue->addTo(toolbar);
	sendQueue->addTo(mailmenu);

	folders = new QAction(tr("Show/hide folders"), QIconSet(Resource::loadPixmap("mail/folder")), 0, 0, this, 0, true);
	folders->addTo(toolbar);
	folders->addTo(servermenu);
	connect(folders, SIGNAL(toggled(bool)), SLOT(slotFoldersToggled(bool)));

	findmails = new QAction(tr("Search mails"), QIconSet(Resource::loadPixmap("mail/find")), 0, 0, this); 
	findmails->addTo(toolbar);
	findmails->addTo(mailmenu);

	configure = new QAction(tr("Configuration"), QIconSet(Resource::loadPixmap("mail/configure")), 0, 0, this);
	configure->addTo(servermenu);

	QVBox *view = new QVBox(this);
	setCentralWidget(view);

	folderView = new FolderWidget(view);
	folderView->setMinimumHeight(90);
	folderView->setMaximumHeight(90);
	folderView->hide();

	mailView = new MailTable(view);
	mailView->setMinimumHeight(50);

	status = new MailStatusBar(view);
}

void MainWindowBase::slotFoldersToggled(bool toggled)
{
	if (folderView->isHidden() && toggled) folderView->show();
	if (!folderView->isHidden() && !toggled) folderView->hide();
}

