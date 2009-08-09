/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
/*
 * VCard Backend for the OPIE-Contact Database.
 */


#include <opie2/private/vobject_p.h>

/* OPIE */
#include <opie2/ocontactaccessbackend_vcard.h>
#include <opie2/odebug.h>

#include <qpe/timeconversion.h>

//FIXME: Hack to allow direct access to FILE* fh. Rewrite this!
#define protected public
#include <qfile.h>
#undef protected

namespace Opie {

OPimContactAccessBackend_VCard::OPimContactAccessBackend_VCard ( const QString& , const QString& filename ):
    m_dirty( false ),
    m_file( filename ),
    version_major( 1 ),
    version_minor( 0 )
{

}


bool OPimContactAccessBackend_VCard::load ()
{
    m_map.clear();
    m_dirty = false;

    VObject* obj = 0l;

    if ( QFile::exists(m_file) ) {
        obj = Parse_MIME_FromFileName( QFile::encodeName(m_file).data() );
        if ( !obj )
            return false;
    }
    else {
        odebug << "File \"" << m_file << "\" not found !" << oendl;
        return false;
    }

    while ( obj ) {
        OPimContact con = parseVObject( obj );
        /*
         * if uid is 0 assign a new one
         * this at least happens on
         * Nokia6210
         */
        if ( con.uid() == 0 ) {
            con.setUid( 1 );
            owarn << "assigned new uid " << con.uid() << "" << oendl;
        }

        m_map.insert( con.uid(), con );

        VObject *t = obj;
        obj = nextVObjectInList(obj);
        cleanVObject( t );
    }

    return true;

}

bool OPimContactAccessBackend_VCard::reload()
{
    return load();
}

bool OPimContactAccessBackend_VCard::save()
{
    if (!m_dirty )
        return true;

    QFile file( m_file );
    if (!file.open(IO_WriteOnly ) )
        return false;

    VObject *obj;
    obj = newVObject( VCCalProp );
    addPropValue( obj, VCVersionProp, "1.0" );

    VObject *vo;
    for(QMap<int, OPimContact>::ConstIterator it=m_map.begin(); it !=m_map.end(); ++it ){
        vo = createVObject( *it );
        writeVObject( file.fh, vo ); //FIXME: HACK!!!
        cleanVObject( vo );
    }
    cleanStrTbl();
    deleteVObject( obj );

    m_dirty = false;
    return true;


}

void OPimContactAccessBackend_VCard::clear ()
{
    m_map.clear();
    m_dirty = true; // ??? sure ? (se)
}

bool OPimContactAccessBackend_VCard::add ( const OPimContact& newcontact )
{
    m_map.insert( newcontact.uid(), newcontact );
    m_dirty = true;
    return true;
}

bool OPimContactAccessBackend_VCard::remove ( int uid )
{
    m_map.remove( uid );
    m_dirty = true;
    return true;
}

bool OPimContactAccessBackend_VCard::replace ( const OPimContact &contact )
{
    m_map.replace( contact.uid(), contact );
    m_dirty = true;
    return true;
}

OPimContact OPimContactAccessBackend_VCard::find ( int uid ) const
{
    return m_map[uid];
}

UIDArray OPimContactAccessBackend_VCard::allRecords() const
{
    UIDArray ar( m_map.count() );
    QMap<int, OPimContact>::ConstIterator it;
    int i = 0;
    for ( it = m_map.begin(); it != m_map.end(); ++it ) {
        ar[i] = it.key();
        i++;
    }
    return ar;
}

bool OPimContactAccessBackend_VCard::wasChangedExternally()
{
    return false; // Don't expect concurrent access
}

// *** Private stuff ***
OPimContact OPimContactAccessBackend_VCard::parseVObject( VObject *obj )
{
    OPimContact c;
    VObjectIterator it;
    initPropIterator( &it, obj );
    while( moreIteration( &it ) ) {
        VObject *o = nextVObject( &it );
        QCString name = vObjectName( o );
        QString value = QString::fromUtf8( vObjectStringZValue( o ) );
        odebug << "(1)Read: " << name << " " << QString( value ).latin1() << oendl;
        if ( name == VCVersionProp ) {

            odebug << "Version: " << value << oendl;
            QStringList version = QStringList::split( ".", value );
            version_major = version[0].toUInt();
            version_minor = version[1].toUInt();
            odebug << "Major: "<< version_major << " Minor: " << version_minor << oendl;

        }
        else if ( name == VCNameProp ) {
            VObjectIterator nit;
            initPropIterator( &nit, o );
            while( moreIteration( &nit ) ) {
                VObject *o = nextVObject( &nit );
                QCString name = vObjectTypeInfo( o );
                QString value = QString::fromUtf8( vObjectStringZValue( o ) );
                odebug << "Nametype is: "<< name << " Value: " << value.latin1() << oendl;
                if ( name == VCNamePrefixesProp )
                    c.setTitle( value );
                else if ( name == VCNameSuffixesProp )
                    c.setSuffix( value );
                else if ( name == VCFamilyNameProp )
                    c.setLastName( value );
                else if ( name == VCGivenNameProp )
                    c.setFirstName( value );
                else if ( name == VCAdditionalNamesProp )
                    c.setMiddleName( value );
            }
        }
        else if ( name == VCAdrProp ) {
            bool work = TRUE; // default address is work address
            QString street;
            QString city;
            QString region;
            QString postal;
            QString country;
    
            VObjectIterator nit;
            initPropIterator( &nit, o );
            while( moreIteration( &nit ) ) {
                VObject *o = nextVObject( &nit );
                QCString name = vObjectTypeInfo( o );
                QString value = QString::fromUtf8( vObjectStringZValue( o ) );
                odebug << "AddressType is: "<< name << " Value: " << value.latin1() << oendl;
                if ( name == VCHomeProp )
                    work = FALSE;
                else if ( name == VCWorkProp )
                    work = TRUE;
                else if ( name == VCStreetAddressProp )
                    street = value;
                else if ( name == VCCityProp )
                    city = value;
                else if ( name == VCRegionProp )
                    region = value;
                else if ( name == VCPostalCodeProp )
                    postal = value;
                else if ( name == VCCountryNameProp )
                    country = value;
            }
            if ( work ) {
                c.setBusinessStreet( street );
                c.setBusinessCity( city );
                c.setBusinessCountry( country );
                c.setBusinessZip( postal );
                c.setBusinessState( region );
            } else {
                c.setHomeStreet( street );
                c.setHomeCity( city );
                c.setHomeCountry( country );
                c.setHomeZip( postal );
                c.setHomeState( region );
            }
        }
        else if ( name == VCTelephoneProp ) {
            enum {
                HOME = 0x01,
                WORK = 0x02,
                VOICE = 0x04,
                CELL = 0x08,
                FAX = 0x10,
                PAGER = 0x20,
                UNKNOWN = 0x80
            };
            int type = 0;
    
            VObjectIterator nit;
            initPropIterator( &nit, o );
            while( moreIteration( &nit ) ) {
                VObject *o = nextVObject( &nit );
                QCString name = vObjectTypeInfo( o );
                odebug << "Telephonetype is: "<< name << " Value: " << value.latin1() << oendl;
                if ( name == VCHomeProp )
                    type |= HOME;
                else if ( name == VCWorkProp )
                    type |= WORK;
                else if ( name == VCVoiceProp )
                    type |= VOICE;
                else if ( name == VCCellularProp )
                    type |= CELL;
                else if ( name == VCFaxProp )
                    type |= FAX;
                else if ( name == VCPagerProp )
                    type |= PAGER;
                else  if ( name == VCPreferredProp )
                    ;
                else  if ( name.left( 2 ) == "X-" || name.left( 2 ) == "x-" )
                    ; // Ignore
                else
                    type |= UNKNOWN;
            }
            if ( (type & UNKNOWN) != UNKNOWN ) {
                if ( ( type & (HOME|WORK) ) == 0 ) // default
                    type |= HOME;
                if ( ( type & (VOICE|CELL|FAX|PAGER) ) == 0 ) // default
                    type |= VOICE;
    
                odebug << "value %s %d" << value.data() << type << oendl;

                if ( (type & (VOICE|HOME) ) == (VOICE|HOME) && (type & (CELL|HOME) ) != (CELL|HOME) )
                    c.setHomePhone( value );
                if ( ( type & (FAX|HOME) ) == (FAX|HOME) )
                    c.setHomeFax( value );
                if ( ( type & (CELL|HOME) ) == (CELL|HOME) )
                    c.setHomeMobile( value );
                if ( ( type & (VOICE|WORK) ) == (VOICE|WORK) && (type & (CELL|WORK) ) != (CELL|WORK) )
                    c.setBusinessPhone( value );
                if ( ( type & (FAX|WORK) ) == (FAX|WORK) )
                    c.setBusinessFax( value );
                if ( ( type & (CELL|WORK) ) == (CELL|WORK) )
                    c.setBusinessMobile( value );
                if ( ( type & (PAGER|WORK) ) == (PAGER|WORK) )
                    c.setBusinessPager( value );
            }
        }
        else if ( name == VCEmailAddressProp ) {
            QString email = QString::fromUtf8( vObjectStringZValue( o ) );
            bool valid = TRUE;
            VObjectIterator nit;
            initPropIterator( &nit, o );
            while( moreIteration( &nit ) ) {
                VObject *o = nextVObject( &nit );
                QCString name = vObjectTypeInfo( o );
                odebug << "Emailtype is: "<< name << " Value: " << value.latin1() << oendl;
                if ( name != VCInternetProp && name != VCHomeProp &&
                 name != VCWorkProp &&
                 name != VCPreferredProp &&
                 name.left( 2 ) != "X-" && name.left( 2 ) != "x-" ){
                    // ### preffered should map to default email
                    valid = FALSE;
                    odebug << "Email was detected as invalid!" << oendl;
                }
            }
            if ( valid ) {
                c.insertEmail( email );
            }
        }
        else if ( name == VCURLProp ) {
            VObjectIterator nit;
            initPropIterator( &nit, o );
            while( moreIteration( &nit ) ) {
                VObject *o = nextVObject( &nit );
                QCString name = vObjectTypeInfo( o );
                if ( name == VCHomeProp )
                    c.setHomeWebpage( value );
                else if ( name == VCWorkProp )
                    c.setBusinessWebpage( value );
            }
        }
        else if ( name == VCOrgProp ) {
            VObjectIterator nit;
            initPropIterator( &nit, o );
            while( moreIteration( &nit ) ) {
                VObject *o = nextVObject( &nit );
                QCString name = vObjectName( o );
                QString value = QString::fromUtf8( vObjectStringZValue( o ) );
                if ( name == VCOrgNameProp )
                    c.setCompany( value );
                else if ( name == VCOrgUnitProp )
                    c.setDepartment( value );
                else if ( name == VCOrgUnit2Prop )
                    c.setOffice( value );
            }
        }
        else if ( name == VCTitleProp ) {
            c.setJobTitle( value );
        }
        else if ( name == "X-Qtopia-Profession" ) {
            c.setProfession( value );
        }
        else if ( name == "X-Qtopia-Manager" ) {
            c.setManager( value );
        }
        else if ( name == "X-Qtopia-Assistant" ) {
            c.setAssistant( value );
        }
        else if ( name == "X-Qtopia-Spouse" ) {
            c.setSpouse( value );
        }
        else if ( name == "X-Qtopia-Gender" ) {
            c.setGender( value );
        }
        else if ( name == "X-Qtopia-Anniversary" ) {
            c.setAnniversary( convVCardDateToDate( value ) );
        }
        else if ( name == "X-Qtopia-Nickname" ) {
            c.setNickname( value );
        }
        else if ( name == "X-Qtopia-Children" ) {
            c.setChildren( value );
        }
        else if ( name == VCBirthDateProp ) {
            // Reading Birthdate regarding RFC 2425 (5.8.4)
            c.setBirthday( convVCardDateToDate( value ) );
        }
        else if ( name == VCCommentProp ) {
            c.setNotes( value );
        }
#if 0
        else {
            printf("Name: %s, value=%s\n", name.data(), QString::fromUtf8( vObjectStringZValue( o ) ) );
            VObjectIterator nit;
            initPropIterator( &nit, o );
            while( moreIteration( &nit ) ) {
                VObject *o = nextVObject( &nit );
                QCString name = vObjectName( o );
                QString value = QString::fromUtf8( vObjectStringZValue( o ) );
                printf(" subprop: %s = %s\n", name.data(), value.latin1() );
            }
        }
        else {
            printf("Name: %s, value=%s\n", name.data(), vObjectStringZValue( o ) );
            VObjectIterator nit;
            initPropIterator( &nit, o );
            while( moreIteration( &nit ) ) {
                VObject *o = nextVObject( &nit );
                QCString name = vObjectName( o );
                QString value = vObjectStringZValue( o );
                printf(" subprop: %s = %s\n", name.data(), value.latin1() );
            }
        }
#endif
    }
    c.setFileAs();
    return c;
}


VObject* OPimContactAccessBackend_VCard::createVObject( const OPimContact &c )
{
    VObject *vcard = newVObject( VCCardProp );
    safeAddPropValue( vcard, VCVersionProp, "2.1" );
    safeAddPropValue( vcard, VCLastRevisedProp, TimeConversion::toISO8601( QDateTime::currentDateTime() ) );
    safeAddPropValue( vcard, VCUniqueStringProp, QString::number(c.uid()) );

    // full name
    safeAddPropValue( vcard, VCFullNameProp, c.fullName() );

    // name properties
    VObject *name = safeAddProp( vcard, VCNameProp );
    safeAddPropValue( name, VCFamilyNameProp, c.lastName() );
    safeAddPropValue( name, VCGivenNameProp, c.firstName() );
    safeAddPropValue( name, VCAdditionalNamesProp, c.middleName() );
    safeAddPropValue( name, VCNamePrefixesProp, c.title() );
    safeAddPropValue( name, VCNameSuffixesProp, c.suffix() );

    // home properties
    if ( !( c.homeStreet().isEmpty() 
       && c.homeCity().isEmpty()
       && c.homeState().isEmpty()
       && c.homeZip().isEmpty()
       && c.homeCountry().isEmpty() ) ){
        VObject *home_adr= safeAddProp( vcard, VCAdrProp );
        safeAddProp( home_adr, VCHomeProp );
        safeAddPropValue( home_adr, VCStreetAddressProp, c.homeStreet() );
        safeAddPropValue( home_adr, VCCityProp, c.homeCity() );
        safeAddPropValue( home_adr, VCRegionProp, c.homeState() );
        safeAddPropValue( home_adr, VCPostalCodeProp, c.homeZip() );
        safeAddPropValue( home_adr, VCCountryNameProp, c.homeCountry() );
    }

    VObject *home_phone = safeAddPropValue( vcard, VCTelephoneProp, c.homePhone() );
    safeAddProp( home_phone, VCHomeProp );
    home_phone = safeAddPropValue( vcard, VCTelephoneProp, c.homeMobile() );
    safeAddProp( home_phone, VCCellularProp );
    safeAddProp( home_phone, VCHomeProp );
    home_phone = safeAddPropValue( vcard, VCTelephoneProp, c.homeFax() );
    safeAddProp( home_phone, VCFaxProp );
    safeAddProp( home_phone, VCHomeProp );

    VObject *url = safeAddPropValue( vcard, VCURLProp, c.homeWebpage() );
    safeAddProp( url, VCHomeProp );

    // work properties
    if ( !( c.businessStreet().isEmpty() 
       && c.businessCity().isEmpty()
       && c.businessState().isEmpty()
       && c.businessZip().isEmpty()
       && c.businessCountry().isEmpty() ) ){
        VObject *work_adr= safeAddProp( vcard, VCAdrProp );
        safeAddProp( work_adr, VCWorkProp );
        safeAddPropValue( work_adr, VCStreetAddressProp, c.businessStreet() );
        safeAddPropValue( work_adr, VCCityProp, c.businessCity() );
        safeAddPropValue( work_adr, VCRegionProp, c.businessState() );
        safeAddPropValue( work_adr, VCPostalCodeProp, c.businessZip() );
        safeAddPropValue( work_adr, VCCountryNameProp, c.businessCountry() );
    }

    VObject *work_phone = safeAddPropValue( vcard, VCTelephoneProp, c.businessPhone() );
    safeAddProp( work_phone, VCWorkProp );
    work_phone = safeAddPropValue( vcard, VCTelephoneProp, c.businessMobile() );
    safeAddProp( work_phone, VCCellularProp );
    safeAddProp( work_phone, VCWorkProp );
    work_phone = safeAddPropValue( vcard, VCTelephoneProp, c.businessFax() );
    safeAddProp( work_phone, VCFaxProp );
    safeAddProp( work_phone, VCWorkProp );
    work_phone = safeAddPropValue( vcard, VCTelephoneProp, c.businessPager() );
    safeAddProp( work_phone, VCPagerProp );
    safeAddProp( work_phone, VCWorkProp );

    url = safeAddPropValue( vcard, VCURLProp, c.businessWebpage() );
    safeAddProp( url, VCWorkProp );

    VObject *title = safeAddPropValue( vcard, VCTitleProp, c.jobTitle() );
    safeAddProp( title, VCWorkProp );


    QStringList emails = c.emailList();
    // emails.prepend( c.defaultEmail() ); Fix for bugreport #1045
    for( QStringList::Iterator it = emails.begin(); it != emails.end(); ++it ) {
        VObject *email = safeAddPropValue( vcard, VCEmailAddressProp, *it );
        safeAddProp( email, VCInternetProp );
    }

    safeAddPropValue( vcard, VCNoteProp, c.notes() );

    // Exporting Birthday regarding RFC 2425 (5.8.4)
    if ( c.birthday().isValid() ){
        safeAddPropValue( vcard, VCBirthDateProp, convDateToVCardDate( c.birthday() ) );
    }

    if ( !c.company().isEmpty() || !c.department().isEmpty() || !c.office().isEmpty() ) {
        VObject *org = safeAddProp( vcard, VCOrgProp );
        safeAddPropValue( org, VCOrgNameProp, c.company() );
        safeAddPropValue( org, VCOrgUnitProp, c.department() );
        safeAddPropValue( org, VCOrgUnit2Prop, c.office() );
    }

    // some values we have to export as custom fields
    safeAddPropValue( vcard, "X-Qtopia-Profession", c.profession() );
    safeAddPropValue( vcard, "X-Qtopia-Manager", c.manager() );
    safeAddPropValue( vcard, "X-Qtopia-Assistant", c.assistant() );

    safeAddPropValue( vcard, "X-Qtopia-Spouse", c.spouse() );
    safeAddPropValue( vcard, "X-Qtopia-Gender", c.gender() );
    if ( c.anniversary().isValid() ){
        safeAddPropValue( vcard, "X-Qtopia-Anniversary", convDateToVCardDate( c.anniversary() ) );
    }
    safeAddPropValue( vcard, "X-Qtopia-Nickname", c.nickname() );
    safeAddPropValue( vcard, "X-Qtopia-Children", c.children() );

    return vcard;
}

QString OPimContactAccessBackend_VCard::convDateToVCardDate( const QDate& d ) const
{
        QString str_rfc2425 = QString("%1-%2-%3")
            .arg( d.year() )
            .arg( d.month(), 2 )
            .arg( d.day(), 2 );
        // Now replace spaces with "0"...
        int pos = 0;
        while ( ( pos = str_rfc2425.find (' ')  ) > 0 )
            str_rfc2425.replace( pos, 1, "0" );

        return str_rfc2425;
}

QDate OPimContactAccessBackend_VCard::convVCardDateToDate( const QString& datestr )
{
    int monthPos = datestr.find('-');
    int dayPos = datestr.find('-', monthPos+1 );
    int sep_ignore = 1;
    if ( monthPos == -1 || dayPos == -1 ) {
        odebug << "fromString didn't find - in str = " << datestr << "; mpos = " << monthPos << " ypos = " << dayPos << "" << oendl;
        // Ok.. No "-" found, therefore we will try to read other format ( YYYYMMDD )
        if ( datestr.length() == 8 ){
            monthPos   = 4;
            dayPos     = 6;
            sep_ignore = 0;
            odebug << "Try with following positions str = " << datestr << "; mpos = " << monthPos << " ypos = " << dayPos << "" << oendl;
        }
        else {
            return QDate();
        }
    }
    int y = datestr.left( monthPos ).toInt();
    int m = datestr.mid( monthPos + sep_ignore, dayPos - monthPos - sep_ignore ).toInt();
    int d = datestr.mid( dayPos + sep_ignore ).toInt();
    odebug << "TimeConversion::fromString ymd = " << datestr << " => " << y << " " << m << " " << d << "; mpos = " << monthPos << " ypos = " << dayPos << "" << oendl;
    QDate date ( y,m,d );
    return date;
}

VObject* OPimContactAccessBackend_VCard::safeAddPropValue( VObject *o, const char *prop, const QString &value )
{
    VObject *ret = 0;
    if ( o && !value.isEmpty() )
        ret = addPropValue( o, prop, value.utf8() );
    return ret;
}

VObject* OPimContactAccessBackend_VCard::safeAddProp( VObject *o, const char *prop)
{
    VObject *ret = 0;
    if ( o )
        ret = addProp( o, prop );
    return ret;
}



}
