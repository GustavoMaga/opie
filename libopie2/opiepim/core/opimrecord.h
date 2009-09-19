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

#ifndef OPIMRECORD_H
#define OPIMRECORD_H

/* OPIE */
#include <opie2/opimxrefmanager.h>
#include <opie2/opimglobal.h>

/*
 * we need to get customMap which is private...
 */
#define private protected
#include <qpe/palmtoprecord.h>
#undef private

/* QT */
#include <qdatastream.h>
#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>

namespace Opie
{


/**
    * Field code for journal / changelog action field
    */
const int FIELDID_ACTION = 100000;

/**
 * This is the base class for
 * all PIM Records
 *
 */
class OPimRecord : public Qtopia::Record
{
  public:
    enum ChangeAction { ACTION_ADD, ACTION_REMOVE, ACTION_REPLACE };
    
    /**
     * c'tor
     * uid of 0 isEmpty
     * uid of 1 will be assigned a new one
     */
    OPimRecord( UID uid = 0 );
    ~OPimRecord();

    /**
     * copy c'tor
     */
    OPimRecord( const OPimRecord& rec );

    /**
     * copy operator
     */
    OPimRecord &operator=( const OPimRecord& );

    /**
     * category names resolved
     */
    QStringList categoryNames( const QString& appname ) const;

    /**
     * set category names they will be resolved
     */
    void setCategoryNames( const QStringList& catnames, const QString& appname );

    /**
     * addCategoryName adds a name
     * to the internal category list
     */
    void addCategoryName( const QString& category, const QString& appname );

    /**
     * if a Record isEmpty
     * it's empty if it's 0
     */
    virtual bool isEmpty() const;

    /**
     * toRichText summary
     */
    virtual QString toRichText() const = 0;

    /**
     * a small one line summary
     */
    virtual QString toShortText() const = 0;

    /**
     * the name of the Record
     */
    virtual QString type() const = 0;

    /**
     * matches the Records the regular expression?
     */
    virtual bool match( const QString &regexp ) const
    {
        setLastHitField( -1 );
        return Qtopia::Record::match( QRegExp( regexp ) );
    };

    /**
     * if implemented this function returns which item has been
     * last hit by the match() function.
     * or -1 if not implemented or no hit has occured
     */
    int lastHitField() const;

    /**
     * converts the internal structure to a map
     */
    virtual QMap<int, QString> toMap() const = 0;

    /**
     * converts a map to the internal structure
     */
    virtual void fromMap( const QMap<int, QString>& map ) = 0;

    /**
     * key value representation of extra items
     */
    QMap<QString, QString> toExtraMap() const;
    void setExtraMap( const QMap<QString, QString>& );

//@{
    /**
     * the name for a recordField
     */
    virtual QString           recordField( int )    const = 0;
//    virtual QArray<int>       recordAttributes()const = 0;
//    virtual QMap<int,QString> recordAttributesTranslated() const = 0;
//    QString                   recordAttributeTranslated(int field)const;
//@}

    /**
     * returns a reference of the
     * Cross Reference Manager
     * Partner 'One' is THIS PIM RECORD!
     * 'Two' is the Partner where we link to
     */
    OPimXRefManager& xrefmanager();

    /**
     * set the uid
     */
    virtual void setUid( int uid );

    /*
     * used inside the Templates for casting
     * REIMPLEMENT in your ....
     */
    virtual int rtti() const;

    /**
     * some marshalling and de marshalling code
     * saves the OPimRecord
     * to and from a DataStream
     */
    virtual bool loadFromStream( QDataStream& );
    virtual bool saveToStream( QDataStream& stream ) const;

    virtual ChangeAction action() const = 0;
    
  protected:
    // need to be const cause it is called from const methods
    mutable int m_lastHit;
    void setLastHitField( int lastHit ) const;
    Qtopia::UidGen &uidGen();
    //    QString crossToString()const;
    static QString actionToStr( ChangeAction action );
    static ChangeAction strToAction( const QString &str );

  private:
    class OPimRecordPrivate;
    OPimRecordPrivate *d;
    OPimXRefManager m_xrefman;
    static Qtopia::UidGen m_uidGen;

  private:
    void flush( const OPimXRefPartner&, QDataStream& stream ) const;
    OPimXRefPartner partner( QDataStream& );
};

}

#endif
