//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Patrick S. Vogt <tille@handhelds.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "adresssearch.h"
#include "contactitem.h"

#include <opie2/oresource.h>

//#include <qwhatsthis.h>

using namespace Opie;
AdressSearch::AdressSearch(QListView* parent, QString name):
    SearchGroup(parent, name)
{
    _contacts = 0;
    setPixmap( 0, Opie::Core::OResource::loadPixmap( "addressbook/AddressBook", Opie::Core::OResource::SmallIcon ) );


//    QWhatsThis::add( this, QObject::tr("Search the addressbook") );
/*    QPixmap pix = Resource::loadPixmap( "addressbook/AddressBook" );
    QImage img = pix.convertToImage();
    img.smoothScale( 14, 14 );
    pix.convertFromImage( img );
    setPixmap( 0, pix );*/
}


AdressSearch::~AdressSearch()
{
    delete _contacts;
}

bool AdressSearch::load()
{
    _contacts = new OPimContactAccess("osearch");
    return _contacts->load();
}

int AdressSearch::search()
{
    OPimRecordList<OPimContact> results = _contacts->matchRegexp(m_search);
    for (uint i = 0; i < results.count(); i++) {
        OPimContact *contact = new OPimContact( results[i] );
        if( contact->lastHitField() == -1 )
            contact->match( m_search );
        (void)new ContactItem( this, contact );
    }
    return results.count();
}

void AdressSearch::insertItem( void* )
{

}
