
#include "packagelistdoclnk.h"

#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qlist.h>

#include "package.h"
#include "pksettings.h"

PackageListDocLnk::PackageListDocLnk(PackageManagerSettings* s)
	: PackageList(s)
{
	PackageListDocLnk();
}

PackageListDocLnk::PackageListDocLnk()
	: PackageList()
{
	Config cfg( "oipkg", Config::User );
	cfg.setGroup( "Common" );
	docLnkDir = cfg.readEntry( "docLnkDir", "/root/" );
	doclnkset = new DocLnkSet(docLnkDir,"application/ipkg");
}

PackageListDocLnk::~PackageListDocLnk()
{
	Config cfg( "oipkg", Config::User );
	cfg.setGroup( "Common" );
	cfg.writeEntry( "docLnkDir", docLnkDir );
}



void PackageListDocLnk::update()
{
	pvDebug(2,"PackageListDocLnk::update ");	
 	QList<DocLnk> packlist = doclnkset->children();
  for (DocLnk *pack =packlist.first(); pack != 0; pack=packlist.next() )
  {
    insertPackage( new Package(pack->file(), settings) );
  }
}
