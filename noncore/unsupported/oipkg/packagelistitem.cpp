#include "packagelistitem.h"

#include <qpe/resource.h>
#include <qobject.h>

#include "debug.h"

static QPixmap *pm_uninstalled=0;
static QPixmap *pm_installed=0;
static QPixmap *pm_uninstall=0;
static QPixmap *pm_install=0;

PackageListItem::PackageListItem(QListView* lv, Package *pi, PackageManagerSettings *s)
  :	QCheckListItem(lv,pi->name(),CheckBox)
{
  package = pi;
  settings = s;
  setExpandable( true );
	QCheckListItem *item;
#ifndef NEWLAYOUT
	item = new QCheckListItem( this, QObject::tr("Name") );
  item->setText(1,pi->name());
	item = new QCheckListItem( this, QObject::tr("Description") );
  item->setText(1,pi->desc()+"\ntest multi\nline");
	item = new QCheckListItem( this, QObject::tr("Size") );
  item->setText(1,pi->size());
	item = new QCheckListItem( this, QObject::tr("Destination") );
  item->setText(1,pi->getDest());
#endif
#ifdef NEWLAYOUT
	item = new QCheckListItem( this, QObject::tr("Name: ")+pi->name() );
	item = new QCheckListItem( this, QObject::tr("Description: ")+pi->desc() );
	item = new QCheckListItem( this, QObject::tr("Size: ")+pi->size() );
	destItem = new QCheckListItem( this, "" );
	linkItem = new QCheckListItem( this, "" );
  displayDetails();
#endif

  if (!pm_uninstalled) {
    pm_uninstalled = new QPixmap(Resource::loadPixmap("oipkg/uninstalled"));
    pm_installed = new QPixmap(Resource::loadPixmap("oipkg/installed"));
    pm_install = new QPixmap(Resource::loadPixmap("oipkg/install"));
    pm_uninstall = new QPixmap(Resource::loadPixmap("oipkg/uninstall"));
  }
#ifndef NEWLAYOUT
  setText(1, package->shortDesc() );
  setText(2, package->size() );
#endif
}

void PackageListItem::paintCell( QPainter *p,  const QColorGroup & cg,
				 int column, int width, int alignment )
{
  if ( !p )
    return;

  p->fillRect( 0, 0, width, height(),
	       isSelected()? cg.highlight() : cg.base() );

  if ( column != 0 ) {
    // The rest is text
    QListViewItem::paintCell( p, cg, column, width, alignment );
    return;
  }

  QListView *lv = listView();
  if ( !lv )
    return;
  int marg = lv->itemMargin();
  int r = marg;

  QPixmap pm = statePixmap();
  p->drawPixmap(marg,(height()-pm.height())/2,pm);
  r += pm.width()+1;

  p->translate( r, 0 );
  QListViewItem::paintCell( p, cg, column, width - r, alignment );
}


void PackageListItem::paintFocus( QPainter *p, const QColorGroup & cg,
				  const QRect & r )
{
  // Skip QCheckListItem
  // (makes you wonder what we're getting from QCheckListItem)
  QListViewItem::paintFocus(p,cg,r);
}

QPixmap PackageListItem::statePixmap() const
{
  bool installed = package->installed();
  if ( !package->toProcess() ) {
    if ( !installed )
      return *pm_uninstalled;
    else
      return *pm_installed;
  } else {
    if ( !installed )
      return *pm_install;
    else
      return *pm_uninstall;
  }
}

QString PackageListItem::key( int column, bool ascending ) const
{
  if ( column == 2 ) {
    QString t = text(2);
    double bytes=t.toDouble();
    if ( t.contains('M') ) bytes*=1024*1024;
    else if ( t.contains('K') || t.contains('k') ) bytes*=1024;
    if ( !ascending ) bytes=999999999-bytes;
    return QString().sprintf("%09d",(int)bytes);
  } else {
    return QListViewItem::key(column,ascending);
  }
}

void PackageListItem::setOn( bool b )
{
  QCheckListItem::setOn( b );
  package->toggleProcess();
//  if ( b )
//  {
//    if ((package->dest()).isEmpty)
//	  	package->setDest( settings->getDestinationName() );
//  }else{
//    package->setDest( QObject::tr("not installed"));
//  }
  package->setLink( settings->createLinks() );
  displayDetails();
}

void PackageListItem::displayDetails()
{
	linkItem->setText( 0, QObject::tr("Link: ")+QString(package->link()?QObject::tr("Yes"):QObject::tr("No")) );
  destItem->setText( 0, QObject::tr("Destination: ")+package->dest() );
  repaint();
}
