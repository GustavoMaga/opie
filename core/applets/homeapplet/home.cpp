#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>

#include <qapplication.h>
#include <qiconset.h>
#include <qpopupmenu.h>

#include "home.h"


HomeApplet::HomeApplet ( )
	: QObject ( 0, "HomeApplet" )
{
}

HomeApplet::~HomeApplet ( )
{
}

int HomeApplet::position ( ) const
{
    return 4;
}

QString HomeApplet::name ( ) const
{
	return tr( "Home shortcut" );
}

QString HomeApplet::text ( ) const
{
	return tr( "Desktop" );
}

QString HomeApplet::tr( const char* s ) const
{
    return qApp->translate( "HomeApplet", s, 0 );
}

QString HomeApplet::tr( const char* s, const char* p ) const
{
    return qApp->translate( "HomeApplet", s, p );
}

QIconSet HomeApplet::icon ( ) const
{
	QPixmap pix;
	QImage img = Resource::loadImage ( "home" );
	
	if ( !img. isNull ( ))
		pix. convertFromImage ( img. smoothScale ( 14, 14 ));
	return pix;
}

QPopupMenu *HomeApplet::popup ( QWidget * ) const
{
	return 0;
}

void HomeApplet::activated ( )
{
	// to desktop (home)
	QCopEnvelope ( "QPE/Application/qpe", "raise()" );
}


QRESULT HomeApplet::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
	*iface = 0;
	if ( uuid == IID_QUnknown )
		*iface = this;
	else if ( uuid == IID_MenuApplet )
		*iface = this;
	else
	    return QS_FALSE;

	if ( *iface )
		(*iface)-> addRef ( );
	return QS_OK;
}

Q_EXPORT_INTERFACE( )
{
	Q_CREATE_INSTANCE( HomeApplet )
}


