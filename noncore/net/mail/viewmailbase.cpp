#include <qtextbrowser.h>
#include <qlistview.h>
#include <qaction.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qpopupmenu.h>

#include <qpe/qpetoolbar.h>
#include <qmenubar.h>
#include <qpe/resource.h>

#include "viewmailbase.h"
//#include "opendiag.h"

ViewMailBase::ViewMailBase(QWidget *parent, const char *name, WFlags fl)
	: QMainWindow(parent, name, fl)
{
	setCaption(tr("E-Mail by %1"));
	setToolBarsMovable(false);

	toolbar = new QToolBar(this);
  menubar = new QMenuBar( toolbar );
  mailmenu = new QPopupMenu( menubar );
  menubar->insertItem( tr( "Mail" ), mailmenu );

	toolbar->setHorizontalStretchable(true);
	addToolBar(toolbar);

	QLabel *spacer = new QLabel(toolbar);
	spacer->setBackgroundMode(QWidget::PaletteButton);
	toolbar->setStretchableWidget(spacer);

	reply = new QAction(tr("Reply"), QIconSet(Resource::loadPixmap("mail/reply")), 0, 0, this);
	reply->addTo(toolbar);
 	reply->addTo(mailmenu);

	forward = new QAction(tr("Forward"), QIconSet(Resource::loadPixmap("mail/forward")), 0, 0, this);
	forward->addTo(toolbar);
	forward->addTo(mailmenu);

	attachbutton = new QAction(tr("Attachments"), QIconSet(Resource::loadPixmap("mail/attach")), 0, 0, this, 0, true);
	attachbutton->addTo(toolbar);
	attachbutton->addTo(mailmenu);
	connect(attachbutton, SIGNAL(toggled(bool)), SLOT(slotChangeAttachview(bool)));


        showHtml = new QAction( tr( "Show Html" ), QIconSet( Resource::loadPixmap( "mail/attach" ) ), 0, 0, this, 0, true );
        showHtml->addTo( toolbar );
        showHtml->addTo( mailmenu );

	deleteMail = new QAction(tr("Delete Mail"), QIconSet(Resource::loadPixmap("mail/delete")), 0, 0, this);
	deleteMail->addTo(toolbar);
	deleteMail->addTo(mailmenu);

	QVBox *view = new QVBox(this);
	setCentralWidget(view);

	attachments = new QListView(view);
	attachments->setMinimumHeight(90);
	attachments->setMaximumHeight(90);
	attachments->setAllColumnsShowFocus(true);
	attachments->addColumn("Mime Type", 60);
	attachments->addColumn(tr("Description"), 100);
	attachments->addColumn(tr("Filename"), 80);
    attachments->addColumn(tr("Size"), 80);
    attachments->setSorting(-1);
	attachments->hide();

	browser = new QTextBrowser(view);

//	openDiag = new OpenDiag(view);
//	openDiag->hide();

}

void ViewMailBase::slotChangeAttachview(bool state)
{
	if (state) attachments->show();
	else attachments->hide();
}


