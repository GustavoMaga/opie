
#include "ocontactfields.h"

#include <qstringlist.h>
#include <qobject.h>

// We should use our own enum in the future ..
#include <qpe/recordfields.h>
#include <qpe/config.h>
#include <opie/ocontact.h>

/*!
  \internal
  Returns a  list of details field names for a contact.
*/
QStringList OContactFields::untrdetailsfields( bool sorted )
{
    QStringList list;

    list.append( "Office" );
    list.append( "Profession" );
    list.append( "Assistant" );
    list.append( "Manager" );

    list.append( "Spouse" );
    list.append( "Gender" );
    list.append( "Birthday" );
    list.append( "Anniversary" );
    list.append( "Nickname" );
    list.append( "Children" );

    if (sorted) list.sort();
    return list;
}

/*!
  \internal
  Returns a translated list of details field names for a contact.
*/
QStringList OContactFields::trdetailsfields( bool sorted )
{
    QStringList list;

    list.append( QObject::tr( "Office" ) );
    list.append( QObject::tr( "Profession" ) );
    list.append( QObject::tr( "Assistant" ) );
    list.append( QObject::tr( "Manager" ) );

    list.append( QObject::tr( "Spouse" ) );
    list.append( QObject::tr( "Gender" ) );
    list.append( QObject::tr( "Birthday" ) );
    list.append( QObject::tr( "Anniversary" ) );
    list.append( QObject::tr( "Nickname" ) );
    list.append( QObject::tr( "Children" ) );

    if (sorted) list.sort();
    return list;
}


/*!
  \internal
  Returns a translated list of phone field names for a contact.
*/
QStringList OContactFields::trphonefields( bool sorted )
{
    QStringList list;
    list.append( QObject::tr( "Business Phone" ) );
    list.append( QObject::tr( "Business Fax" ) );
    list.append( QObject::tr( "Business Mobile" ) );

    list.append( QObject::tr( "Default Email" ) );
    list.append( QObject::tr( "Emails" ) );

    list.append( QObject::tr( "Home Phone" ) );
    list.append( QObject::tr( "Home Fax" ) );
    list.append( QObject::tr( "Home Mobile" ) );

    if (sorted) list.sort();

    return list;
}


/*!
  \internal
  Returns a list of phone field names for a contact.
*/
QStringList OContactFields::untrphonefields( bool sorted )
{
    QStringList list;

    list.append( "Business Phone" );
    list.append( "Business Fax" );
    list.append( "Business Mobile" );

    list.append( "Default Email" );
    list.append( "Emails" );

    list.append( "Home Phone" );
    list.append( "Home Fax" );
    list.append( "Home Mobile" );

    if (sorted) list.sort();

    return list;
}


/*!
  \internal
  Returns a translated list of field names for a contact.
*/
QStringList OContactFields::trfields( bool sorted )
{
    QStringList list;

    list.append( QObject::tr( "Name Title") );
    list.append( QObject::tr( "First Name" ) );
    list.append( QObject::tr( "Middle Name" ) );
    list.append( QObject::tr( "Last Name" ) );
    list.append( QObject::tr( "Suffix" ) );
    list.append( QObject::tr( "File As" ) );

    list.append( QObject::tr( "Job Title" ) );
    list.append( QObject::tr( "Department" ) );
    list.append( QObject::tr( "Company" ) );

    list += trphonefields( sorted );

    list.append( QObject::tr( "Business Street" ) );
    list.append( QObject::tr( "Business City" ) );
    list.append( QObject::tr( "Business State" ) );
    list.append( QObject::tr( "Business Zip" ) );
    list.append( QObject::tr( "Business Country" ) );
    list.append( QObject::tr( "Business Pager" ) );
    list.append( QObject::tr( "Business WebPage" ) );

    list.append( QObject::tr( "Home Street" ) );
    list.append( QObject::tr( "Home City" ) );
    list.append( QObject::tr( "Home State" ) );
    list.append( QObject::tr( "Home Zip" ) );
    list.append( QObject::tr( "Home Country" ) );
    list.append( QObject::tr( "Home Web Page" ) );

    list += trdetailsfields( sorted );

    list.append( QObject::tr( "Notes" ) );
    list.append( QObject::tr( "Groups" ) );

    if (sorted) list.sort();

    return list;
}

/*!
  \internal
  Returns an untranslated list of field names for a contact.
*/
QStringList OContactFields::untrfields( bool sorted )
{
    QStringList list;

    list.append( "Name Title" );
    list.append( "First Name" );
    list.append( "Middle Name" );
    list.append( "Last Name" );
    list.append( "Suffix" );
    list.append( "File As" );

    list.append( "Job Title" );
    list.append( "Department" );
    list.append( "Company" );

    list += untrphonefields( sorted );

    list.append( "Business Street" );
    list.append( "Business City" );
    list.append( "Business State" );
    list.append( "Business Zip" );
    list.append( "Business Country" );
    list.append( "Business Pager" );
    list.append( "Business WebPage" );

    list.append( "Home Street" );
    list.append( "Home City" );
    list.append( "Home State" );
    list.append( "Home Zip" );
    list.append( "Home Country" );
    list.append( "Home Web Page" );

    list += untrdetailsfields( sorted );

    list.append( "Notes" );
    list.append( "Groups" );

    if (sorted) list.sort();

    return list;
}
QMap<int, QString> OContactFields::idToTrFields()
{
	QMap<int, QString> ret_map;

	ret_map.insert( Qtopia::Title, QObject::tr( "Name Title") );
	ret_map.insert( Qtopia::FirstName, QObject::tr( "First Name" ) );
	ret_map.insert( Qtopia::MiddleName, QObject::tr( "Middle Name" ) );
	ret_map.insert( Qtopia::LastName, QObject::tr( "Last Name" ) );
	ret_map.insert( Qtopia::Suffix,  QObject::tr( "Suffix" ));
	ret_map.insert( Qtopia::FileAs, QObject::tr( "File As" ) );

	ret_map.insert( Qtopia::JobTitle, QObject::tr( "Job Title" ) );
	ret_map.insert( Qtopia::Department, QObject::tr( "Department" ) );
	ret_map.insert( Qtopia::Company, QObject::tr( "Company" ) );
	ret_map.insert( Qtopia::BusinessPhone, QObject::tr( "Business Phone" ) );
	ret_map.insert( Qtopia::BusinessFax, QObject::tr( "Business Fax" ) );
	ret_map.insert( Qtopia::BusinessMobile,  QObject::tr( "Business Mobile" ));

	// email
	ret_map.insert( Qtopia::DefaultEmail, QObject::tr( "Default Email" ) );
	ret_map.insert( Qtopia::Emails, QObject::tr( "Emails" ) );

	ret_map.insert( Qtopia::HomePhone, QObject::tr( "Home Phone" ) );
	ret_map.insert( Qtopia::HomeFax, QObject::tr( "Home Fax" ) );
	ret_map.insert( Qtopia::HomeMobile, QObject::tr( "Home Mobile" ) );

	// business
	ret_map.insert( Qtopia::BusinessStreet, QObject::tr( "Business Street" ) );
	ret_map.insert( Qtopia::BusinessCity, QObject::tr( "Business City" ) );
	ret_map.insert( Qtopia::BusinessState, QObject::tr( "Business State" ) );
	ret_map.insert( Qtopia::BusinessZip, QObject::tr( "Business Zip" ) );
	ret_map.insert( Qtopia::BusinessCountry, QObject::tr( "Business Country" ) );
	ret_map.insert( Qtopia::BusinessPager, QObject::tr( "Business Pager" ) );
	ret_map.insert( Qtopia::BusinessWebPage, QObject::tr( "Business WebPage" ) );

	ret_map.insert( Qtopia::Office, QObject::tr( "Office" ) );
	ret_map.insert( Qtopia::Profession, QObject::tr( "Profession" ) );
	ret_map.insert( Qtopia::Assistant, QObject::tr( "Assistant" ) );
	ret_map.insert( Qtopia::Manager, QObject::tr( "Manager" ) );

	// home
	ret_map.insert( Qtopia::HomeStreet, QObject::tr( "Home Street" ) );
	ret_map.insert( Qtopia::HomeCity, QObject::tr( "Home City" ) );
	ret_map.insert( Qtopia::HomeState, QObject::tr( "Home State" ) );
	ret_map.insert( Qtopia::HomeZip, QObject::tr( "Home Zip" ) );
	ret_map.insert( Qtopia::HomeCountry, QObject::tr( "Home Country" ) );
	ret_map.insert( Qtopia::HomeWebPage, QObject::tr( "Home Web Page" ) );

	//personal
	ret_map.insert( Qtopia::Spouse, QObject::tr( "Spouse" ) );
	ret_map.insert( Qtopia::Gender, QObject::tr( "Gender" ) );
	ret_map.insert( Qtopia::Birthday, QObject::tr( "Birthday" ) );
	ret_map.insert( Qtopia::Anniversary, QObject::tr( "Anniversary" ) );
	ret_map.insert( Qtopia::Nickname, QObject::tr( "Nickname" ) );
	ret_map.insert( Qtopia::Children, QObject::tr( "Children" ) );

	// other
	ret_map.insert( Qtopia::Notes, QObject::tr( "Notes" ) );


	return ret_map;
}

QMap<QString, int> OContactFields::trFieldsToId()
{
	QMap<int, QString> idtostr = idToTrFields();
	QMap<QString, int> ret_map;


        QMap<int, QString>::Iterator it;
        for( it = idtostr.begin(); it != idtostr.end(); ++it )
		ret_map.insert( *it, it.key() );


	return ret_map;
}

OContactFields::OContactFields():
	fieldOrder( DEFAULT_FIELD_ORDER ),
	changedFieldOrder( false )
{
	// Get the global field order from the config file and
	// use it as a start pattern
	Config cfg ( "AddressBook" );
	cfg.setGroup( "ContactFieldOrder" );
	globalFieldOrder = cfg.readEntry( "General", DEFAULT_FIELD_ORDER );
}

OContactFields::~OContactFields(){

	// We will store the fieldorder into the config file
	// to reuse it for the future.. 
	if ( changedFieldOrder ){
		Config cfg ( "AddressBook" );
		cfg.setGroup( "ContactFieldOrder" );
		cfg.writeEntry( "General", globalFieldOrder );
	}
}



void OContactFields::saveToRecord( OContact &cnt ){

	qDebug("ocontactfields saveToRecord: >%s<",fieldOrder.latin1());

	// Store fieldorder into this contact.
	cnt.setCustomField( CONTACT_FIELD_ORDER_NAME, fieldOrder );

	globalFieldOrder = fieldOrder;
	changedFieldOrder = true;

}

void OContactFields::loadFromRecord( const OContact &cnt ){
	qDebug("ocontactfields loadFromRecord");
	qDebug("loading >%s<",cnt.fullName().latin1());

	// Get fieldorder for this contact. If none is defined,
	// we will use the global one from the config file..

	fieldOrder = cnt.customField( CONTACT_FIELD_ORDER_NAME );

	qDebug("fieldOrder from contact>%s<",fieldOrder.latin1());

	if (fieldOrder.isEmpty()){
		fieldOrder = globalFieldOrder;
	}


	qDebug("effective fieldOrder in loadFromRecord >%s<",fieldOrder.latin1());
}

void OContactFields::setFieldOrder( int num, int index ){
	qDebug("qcontactfields setfieldorder pos %i -> %i",num,index);

	fieldOrder[num] = QString::number( index )[0];

	// We will store this new fieldorder globally to 
	// remember it for contacts which have none
	globalFieldOrder = fieldOrder;
	changedFieldOrder = true;

	qDebug("fieldOrder >%s<",fieldOrder.latin1());
}

int OContactFields::getFieldOrder( int num, int defIndex ){
	qDebug("ocontactfields getFieldOrder");
	qDebug("fieldOrder >%s<",fieldOrder.latin1());

	// Get index of combo as char..
	QChar poschar = fieldOrder[num];

	bool ok;
	int ret = 0;
	// Convert char to number..
	if ( !( poschar == QChar::null ) )
		ret = QString( poschar ).toInt(&ok, 10);
	else
		ok = false;

	// Return default value if index for
	// num was not set or if anything else happened..
	if ( !ok ) ret = defIndex;

	qDebug("returning >%i<",ret);

	return ret;

}
