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
 * =====================================================================
 * ToDo: Define enum for query settings
 * =====================================================================
 */
#ifndef _OCONTACTACCESS_H
#define _OCONTACTACCESS_H

#include <qobject.h>

#include <qpe/qcopenvelope_qws.h>

#include <qvaluelist.h>
#include <qfileinfo.h>

#include <opie2/opimcontact.h>
#include <opie2/ocontactaccessbackend.h>
#include <opie2/opimaccesstemplate.h>

namespace Opie {
/**
 * Class to access the contacts database.
 * This is just a frontend for the real database handling which is
 * done by the backend.
 * This class is used to access the Contacts on a system. This class as any OPIE PIM
 * class is backend independent.
 * @author Stefan Eilers, Holger Freyther
 * @see OPimAccessTemplate
 */
class OPimContactAccess: public QObject, public OPimAccessTemplate<OPimContact>
{
    Q_OBJECT

 public:
    enum SortFilter {
        DoNotShowNoneChildren      = FilterCustom<<1,
        DoNotShowNoneAnniversary   = FilterCustom<<2,
        DoNotShowNoneBirthday      = FilterCustom<<3,
        DoNotShowNoHomeAddress     = FilterCustom<<4,
        DoNotShowNoBusinessAddress = FilterCustom<<5
    };

    enum SortOrder {
        SortTitle = SortCustom,
        SortFirstName,
        SortMiddleName,
        SortSuffix,
        SortEmail,
        SortNickname,
        SortAnniversary,
        SortBirthday,
        SortGender
    };

	/**
         * Create Database with contacts (addressbook).
	 * @param appname Name of application which wants access to the database
	 * (i.e. "todolist")
	 * @param filename The name of the database file. If not set, the default one
	 * is used.
	 * @param backend Pointer to an alternative Backend. If not set, we will use
	 * the default backend.
	 * @param handlesync If <b>true</b> the database stores the current state
	 * automatically if it receives the signals <i>flush()</i> and <i>reload()</i>
	 * which are used before and after synchronisation. If the application wants
	 * to react itself, it should be disabled by setting it to <b>false</b>
	 * @see OPimContactAccessBackend
	 */
	OPimContactAccess (const QString appname, const QString filename = 0l,
                           OPimContactAccessBackend* backend = 0l, bool handlesync = true);
        ~OPimContactAccess ();


	/** Return all possible settings.
	 *  @return All settings provided by the current backend
	 * (i.e.: query_WildCards & query_IgnoreCase)
	 */
	const uint querySettings();

	/** Check whether settings are correct.
	 * @return <i>true</i> if the given settings are correct and possible.
	 */
	bool hasQuerySettings ( int querySettings ) const;

	/**
	 * if the resource was changed externally.
	 * You should use the signal instead of polling possible changes !
	 */
	bool wasChangedExternally()const;


	/** Save contacts database.
	 * Save is more a "commit". After calling this function, all changes are public available.
	 * @return true if successful
	 */
        bool save();

	/**
	 * Return identification of used records
	 */
	int rtti() const;

 signals:
	/* Signal is emitted if the database was changed. Therefore
	 * we may need to reload to stay consistent.
	 * @param which Pointer to the database who created this event. This pointer
	 * is useful if an application has to handle multiple databases at the same time.
	 * @see reload()
	 */
	void signalChanged ( const OPimContactAccess *which );


 private:
        OPimContactAccessBackend *m_backEnd;
        bool m_loading:1;

 private slots:
	void copMessage( const QCString &msg, const QByteArray &data );

 private:
        class Private;
	Private *d;

};

}

#endif
