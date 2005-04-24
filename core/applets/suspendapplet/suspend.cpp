#include "suspend.h"

#include <opie2/oresource.h>

#include <qpe/applnk.h>
#include <qpe/qcopenvelope_qws.h>

#include <qapplication.h>

SuspendApplet::SuspendApplet ( )
	: QObject ( 0, "SuspendApplet" )
{
}

SuspendApplet::~SuspendApplet ( )
{
}

int SuspendApplet::position ( ) const
{
    return 2;
}

QString SuspendApplet::name ( ) const
{
	return tr( "Suspend shortcut" );
}

QString SuspendApplet::text ( ) const
{
	return tr( "Suspend" );
}

QString SuspendApplet::tr( const char* s ) const
{
    return qApp->translate( "SuspendApplet", s, 0 );
}

QString SuspendApplet::tr( const char* s, const char* p ) const
{
    return qApp->translate( "SuspendApplet", s, p );
}

QIconSet SuspendApplet::icon ( ) const
{
    QPixmap pix = Opie::Core::OResource::loadPixmap( "suspend", Opie::Core::OResource::SmallIcon );
	return pix;
}

QPopupMenu *SuspendApplet::popup ( QWidget * ) const
{
	return 0;
}

void SuspendApplet::activated ( )
{
	// suspend
	QCopEnvelope ( "QPE/System", "suspend()" );
}


QRESULT SuspendApplet::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
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
	Q_CREATE_INSTANCE( SuspendApplet )
}


