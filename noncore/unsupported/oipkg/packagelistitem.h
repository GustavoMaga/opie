#ifndef PK_LISTITEM_H
#define PK_LISTITEM_H

#include "package.h"

#include <qstring.h>
#include <qlistview.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qdict.h>

class PackageListItem
: public QCheckListItem
{
public:
  PackageListItem(QListView*, Package* );
  void paintCell( QPainter*, const QColorGroup&, int, int, int );
  void paintFocus( QPainter*, const QColorGroup&, const QRect&  );
  QPixmap statePixmap() const;
  QString key( int, bool ) const;
  Package* getPackage() { return package; } ;
  QString getName() { return package->name(); } ;
  bool isInstalled(){ return package->installed(); };
  virtual void setOn ( bool );

private:
  Package *package;
};


#endif
