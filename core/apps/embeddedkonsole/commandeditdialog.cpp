//comandeditdialog.cpp

#include "commandeditdialog.h"
#include "playlistselection.h"

#include <opie2/oresource.h>

#include <qpe/config.h>

#include <qapplication.h>
#include <qtoolbutton.h>
#include <qlineedit.h>
#include <qheader.h>
#include <qlabel.h>
#include "smallcommandeditdialogbase.h"

CommandEditDialog::CommandEditDialog(QWidget *parent, const char* name, WFlags fl )
    : CommandEditDialogBase(parent, name, TRUE, fl)

{
  m_SuggestedCommandList->addColumn( tr("Command Selection") );
  m_SuggestedCommandList->header()->hide();
  m_SuggestedCommandList->setSorting(-1,FALSE);
  m_SuggestedCommandList->clearSelection();
  m_SuggestedCommandList->setSorting(0,TRUE);
  new QListViewItem( m_SuggestedCommandList,"export ");
  new QListViewItem( m_SuggestedCommandList,"ifconfig ");
  new QListViewItem( m_SuggestedCommandList,"ipkg ");
  new QListViewItem( m_SuggestedCommandList,"gzip ");
  new QListViewItem( m_SuggestedCommandList,"gunzip ");
  new QListViewItem( m_SuggestedCommandList,"chgrp ");
  new QListViewItem( m_SuggestedCommandList,"chown ");
  new QListViewItem( m_SuggestedCommandList,"date ");
  new QListViewItem( m_SuggestedCommandList,"dd ");
  new QListViewItem( m_SuggestedCommandList,"dmesg ");
  new QListViewItem( m_SuggestedCommandList,"fuser ");
  new QListViewItem( m_SuggestedCommandList,"hostname ");
  new QListViewItem( m_SuggestedCommandList,"kill ");
  new QListViewItem( m_SuggestedCommandList,"killall ");
  new QListViewItem( m_SuggestedCommandList,"ln ");
  new QListViewItem( m_SuggestedCommandList,"ln -s ");
  new QListViewItem( m_SuggestedCommandList,"lsmod");
  new QListViewItem( m_SuggestedCommandList,"depmod -a");
  new QListViewItem( m_SuggestedCommandList,"modprobe ");
  new QListViewItem( m_SuggestedCommandList,"mount ");
  new QListViewItem( m_SuggestedCommandList,"more ");
  new QListViewItem( m_SuggestedCommandList,"sort ");
  new QListViewItem( m_SuggestedCommandList,"touch ");
  new QListViewItem( m_SuggestedCommandList,"umount ");
  new QListViewItem( m_SuggestedCommandList,"mknod ");
  new QListViewItem( m_SuggestedCommandList,"netstat ");
  new QListViewItem( m_SuggestedCommandList,"route ");
  QListViewItem *item = new QListViewItem( m_SuggestedCommandList,"cardctl eject ");

  m_SuggestedCommandList->setSelected(m_SuggestedCommandList->firstChild(),TRUE);
  m_SuggestedCommandList->sort();

  connect( m_SuggestedCommandList, SIGNAL( clicked(QListViewItem*) ), m_PlayListSelection, SLOT( addToSelection(QListViewItem*) ) );

  bool bigPixmaps = qApp->desktop()->size().width()>330;

  ToolButton1->setTextLabel("new");
  ToolButton1->setUsesBigPixmap( bigPixmaps );
  ToolButton1->setPixmap( Opie::Core::OResource::loadPixmap( "new", Opie::Core::OResource::SmallIcon ) );
  ToolButton1->setAutoRaise(TRUE);
  ToolButton1->setFocusPolicy(QWidget::NoFocus);
  connect(ToolButton1,SIGNAL(clicked()),this,SLOT(showAddDialog()));

  ToolButton2->setTextLabel("edit");
  ToolButton2->setUsesBigPixmap( bigPixmaps );
  ToolButton2->setPixmap( Opie::Core::OResource::loadPixmap( "edit", Opie::Core::OResource::SmallIcon ) );
  ToolButton2->setAutoRaise(TRUE);
  ToolButton2->setFocusPolicy(QWidget::NoFocus);
  connect(ToolButton2,SIGNAL(clicked()),this,SLOT(showEditDialog()));

  ToolButton3->setTextLabel("delete");
  ToolButton3->setUsesBigPixmap( bigPixmaps );
  ToolButton3->setPixmap( Opie::Core::OResource::loadPixmap( "editdelete", Opie::Core::OResource::SmallIcon ) );
  ToolButton3->setAutoRaise(TRUE);
  ToolButton3->setFocusPolicy(QWidget::NoFocus);
  connect(ToolButton3,SIGNAL(clicked()),m_PlayListSelection,SLOT(removeSelected()));

  ToolButton4->setTextLabel("up");
  ToolButton4->setUsesBigPixmap( bigPixmaps );
  ToolButton4->setPixmap( Opie::Core::OResource::loadPixmap( "up", Opie::Core::OResource::SmallIcon ) );
  ToolButton4->setAutoRaise(TRUE);
  ToolButton4->setFocusPolicy(QWidget::NoFocus);
  connect(ToolButton4,SIGNAL(clicked()),m_PlayListSelection,SLOT(moveSelectedUp()));

  ToolButton5->setTextLabel("down");
  ToolButton5->setUsesBigPixmap( bigPixmaps );
  ToolButton5->setPixmap( Opie::Core::OResource::loadPixmap( "down", Opie::Core::OResource::SmallIcon ) );
  ToolButton5->setAutoRaise(TRUE);
  ToolButton5->setFocusPolicy(QWidget::NoFocus);

  connect(ToolButton5,SIGNAL(clicked()),m_PlayListSelection,SLOT(moveSelectedDown()));

  QListViewItem *current = m_SuggestedCommandList->selectedItem();
  if ( current )
    item->moveItem( current );

  m_SuggestedCommandList->setSelected( item, TRUE );
  m_SuggestedCommandList->ensureItemVisible( m_SuggestedCommandList->selectedItem() );
  Config cfg( "Konsole" );
  cfg.setGroup("Commands");
  if (cfg.readEntry("Commands Set","FALSE") == "TRUE") {
    for (int i = 0; i < 100; i++) {
      QString tmp;
      tmp = cfg.readEntry( QString::number(i),"");
      if (!tmp.isEmpty())
        m_PlayListSelection->addStringToSelection(tmp);
    }
  } else {
    m_PlayListSelection->addStringToSelection("ls ");
    m_PlayListSelection->addStringToSelection("cardctl eject");
    m_PlayListSelection->addStringToSelection("cat ");
    m_PlayListSelection->addStringToSelection("cd ");
    m_PlayListSelection->addStringToSelection("chmod ");
    m_PlayListSelection->addStringToSelection("cp ");
    m_PlayListSelection->addStringToSelection("dc ");
    m_PlayListSelection->addStringToSelection("df ");
    m_PlayListSelection->addStringToSelection("dmesg");
    m_PlayListSelection->addStringToSelection("echo ");
    m_PlayListSelection->addStringToSelection("env");
    m_PlayListSelection->addStringToSelection("find ");
    m_PlayListSelection->addStringToSelection("free");
    m_PlayListSelection->addStringToSelection("grep ");
    m_PlayListSelection->addStringToSelection("ifconfig ");
    m_PlayListSelection->addStringToSelection("ipkg ");
    m_PlayListSelection->addStringToSelection("mkdir ");
    m_PlayListSelection->addStringToSelection("mv ");
    m_PlayListSelection->addStringToSelection("nc localhost 7776");
    m_PlayListSelection->addStringToSelection("nc localhost 7777");
    m_PlayListSelection->addStringToSelection("nslookup ");
    m_PlayListSelection->addStringToSelection("ping ");
    m_PlayListSelection->addStringToSelection("ps aux");
    m_PlayListSelection->addStringToSelection("pwd ");
    m_PlayListSelection->addStringToSelection("rm ");
    m_PlayListSelection->addStringToSelection("rmdir ");
    m_PlayListSelection->addStringToSelection("route ");
    m_PlayListSelection->addStringToSelection("set ");
    m_PlayListSelection->addStringToSelection("traceroute");
  }
}

CommandEditDialog::~CommandEditDialog()
{
}

void CommandEditDialog::accept()
{
  int i = 0;
  Config *cfg = new Config("Konsole");
  cfg->setGroup("Commands");
  cfg->clearGroup();

    QListViewItemIterator it( m_PlayListSelection );

    for ( ; it.current(); ++it ) {
//    odebug << it.current()->text(0) << oendl;
    cfg->writeEntry(QString::number(i),it.current()->text(0));
    i++;

    }
    cfg->writeEntry("Commands Set","TRUE");
//    odebug << "CommandEditDialog::accept() - written" << oendl;
    delete cfg;
    emit commandsEdited();
    close();





}

void CommandEditDialog::showEditDialog()
{
editCommandBase *d = new editCommandBase(this,"smalleditdialog", TRUE);
d->setCaption("Edit command");
d->TextLabel->setText("Edit command:");
d->commandEdit->setText(m_PlayListSelection->currentItem()->text(0));
int i = d->exec();
if ((i==1) && (!(d->commandEdit->text()).isEmpty()))
  m_PlayListSelection->currentItem()->setText(0,(d->commandEdit->text()));
}

void CommandEditDialog::showAddDialog()
{

editCommandBase *d = new editCommandBase(this,"smalleditdialog", TRUE);
int i = d->exec();
if ((i==1) && (!(d->commandEdit->text()).isEmpty()))
m_PlayListSelection->addStringToSelection(d->commandEdit->text());

}

