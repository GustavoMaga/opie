/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef KATECONFIG_H
#define KATECONFIG_H

// ##### could use QSettings with Qt 3.0

#include <qmap.h>
#include <qstringlist.h>
#include <qfont.h>
#include <qcolor.h>

class KateConfigPrivate;
class KateConfig
{
public:
    typedef QMap< QString, QString > KateConfigGroup;
    
    enum Domain { File, User };
    KateConfig( const QString &name, Domain domain=User );
    ~KateConfig();
    
    bool operator == ( const KateConfig & other ) const { return (filename == other.filename); }
    bool operator != ( const KateConfig & other ) const { return (filename != other.filename); }
    
    bool isValid() const;
    bool hasKey( const QString &key ) const;

	// inline for better SharpROM BC
	inline bool hasGroup ( const QString &gname ) const  { return ( groups. find ( gname ) != groups. end ( )); };
	inline QStringList groupList ( ) const { QStringList sl; for ( QMap< QString, KateConfigGroup >::ConstIterator it = groups. begin ( ); it != groups. end ( ); ++it ) { sl << it.key(); } return sl; };
    
    void setGroup( const QString &gname );
    void writeEntry( const QString &key, const char* value );
    void writeEntry( const QString &key, const QString &value );
    void writeEntryCrypt( const QString &key, const QString &value );
    void writeEntry( const QString &key, int num );
    void writeEntry( const QString &key, unsigned int num );
#ifdef Q_HAS_BOOL_TYPE
    void writeEntry( const QString &key, bool b );
#endif
    void writeEntry( const QString &key, const QStringList &lst, const QChar &sep );
    void writeEntry( const QString &key, const QColor & );
    void writeEntry( const QString &key, const QFont & );
    void removeEntry( const QString &key );
    
    QString readEntry( const QString &key, const QString &deflt = QString::null ) const;
    QString readEntryCrypt( const QString &key, const QString &deflt = QString::null ) const;
    QString readEntryDirect( const QString &key, const QString &deflt = QString::null ) const;
    int readNumEntry( const QString &key, int deflt = -1 ) const;
    bool readBoolEntry( const QString &key, bool deflt = FALSE ) const;
    QStringList readListEntry( const QString &key, const QChar &sep ) const;
    QColor readColorEntry( const QString &, const QColor & ) const;
    QFont readFontEntry( const QString &, const QFont & ) const;
    QValueList<int> readIntListEntry( const QString &key ) const;

    // For compatibility, non-const versions.
    QString readEntry( const QString &key, const QString &deflt );
    QString readEntryCrypt( const QString &key, const QString &deflt );
    QString readEntryDirect( const QString &key, const QString &deflt );
    int readNumEntry( const QString &key, int deflt );
    bool readBoolEntry( const QString &key, bool deflt );
    QStringList readListEntry( const QString &key, const QChar &sep );
    
    void clearGroup();
    
    void write( const QString &fn = QString::null );
    
protected:
    void read();
    bool parse( const QString &line );
    
    QMap< QString, KateConfigGroup > groups;
    QMap< QString, KateConfigGroup >::Iterator git;
    QString filename;
    QString lang;
    QString glang;
    bool changed;
    KateConfigPrivate *d;
    static QString configFilename(const QString& name, Domain);

private: // Sharp ROM compatibility
    KateConfig( const QString &name, bool what );
};

inline QString KateConfig::readEntry( const QString &key, const QString &deflt ) const
{ return ((KateConfig*)this)->readEntry(key,deflt); }
inline QString KateConfig::readEntryCrypt( const QString &key, const QString &deflt ) const
{ return ((KateConfig*)this)->readEntryCrypt(key,deflt); }
inline QString KateConfig::readEntryDirect( const QString &key, const QString &deflt ) const
{ return ((KateConfig*)this)->readEntryDirect(key,deflt); }
inline int KateConfig::readNumEntry( const QString &key, int deflt ) const
{ return ((KateConfig*)this)->readNumEntry(key,deflt); }
inline bool KateConfig::readBoolEntry( const QString &key, bool deflt ) const
{ return ((KateConfig*)this)->readBoolEntry(key,deflt); }
inline QStringList KateConfig::readListEntry( const QString &key, const QChar &sep ) const
{ return ((KateConfig*)this)->readListEntry(key,sep); }

#endif
