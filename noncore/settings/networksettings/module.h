#ifndef NETCONF_MODULE_H
#define NETCONF_MODULE_H

#include <qobject.h>
#include <qlist.h>
#include <qmap.h>
#include "interface.h"

class QWidget;
class QTabWidget;

class Module : QObject{

signals:
  void updateInterface(Interface *i);
	
public:
  Module(){};
  
  virtual bool isOwner(Interface *){ return false; };
  virtual QWidget *configure(QTabWidget **tabWidget){ return NULL; } ;
  virtual QWidget *information(QTabWidget **tabWidget){ return NULL; };
  virtual QList<Interface> getInterfaces() = 0;
  virtual QMap<QString, QString> possibleNewInterfaces() = 0;
  virtual Interface *addNewInterface(QString name) = 0;
  virtual bool remove(Interface* i) = 0;
  virtual QString getPixmapName(Interface* i) = 0;

};

#endif

// module.h

