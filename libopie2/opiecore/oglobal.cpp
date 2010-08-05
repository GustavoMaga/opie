/*
                             This file is part of the Opie Project
                             Copyright (C) 2003 Michael 'Mickey' Lauer <mickey@Vanille.de>
              =.             Copyright (C) 2004 Holger 'zecke' Freyther <zecke@handhelds.org>
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

#include <opie2/oglobal.h>

#include <opie2/odebug.h>

#include <qtextstream.h>
#include <qdir.h>
#include <qpe/mimetype.h>
#include <qpe/qpeapplication.h>
#include <qpe/storage.h>

#include <unistd.h>
#include <sys/types.h>

using namespace Opie::Core;

static const char Base64EncMap[64] =
{
  0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
  0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
  0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
  0x59, 0x5A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
  0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E,
  0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
  0x77, 0x78, 0x79, 0x7A, 0x30, 0x31, 0x32, 0x33,
  0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2B, 0x2F
};

static char Base64DecMap[128] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x3F,
  0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
  0x3C, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
  0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
  0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
  0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
  0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
  0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
  0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00
};


OConfig* OGlobal::_config = 0;
OConfig* OGlobal::_qpe_config = 0;

void OGlobal::clean_up()
{
    qDebug( "Oglobal clean up" );
    delete OGlobal::_config;
    delete OGlobal::_qpe_config;
    OGlobal::_config = 0;
    OGlobal::_qpe_config = 0;
}

OConfig* OGlobal::config()
{
    if ( !OGlobal::_config )
    {
        // odebug classes are reading config, so can't use them here!
        qAddPostRoutine( OGlobal::clean_up );
        qDebug( "OGlobal::creating global configuration instance." );
        OGlobal::_config = new OConfig( "global" );
    }
    return OGlobal::_config;
}


/**
 * Return the internal builtin Global::Command object
 *
 */
Global::Command* OGlobal::builtinCommands() {
    return builtin;
}


/**
 * Return the internal builtin QGuardedPtr<QWidget> object
 */
QGuardedPtr<QWidget>* OGlobal::builtinRunning() {
    return running;
}


/**
 * \brief generate a new UUID as QString
 * Return a new UUID as QString. UUID are global unique
 *
 *
 * @return the UUID or QString::null
 */
QString OGlobal::generateUuid() {
    QFile file( "/proc/sys/kernel/random/uuid" );
    if (!file.open(IO_ReadOnly ) )
        return QString::null;

    QTextStream stream(&file);

    return "{" + stream.read().stripWhiteSpace() + "}";
}


/**
 * \brief Encode a QByteArray in base64
 *
 * An Implementation of the RF1521 base64 encoding.
 *
 * The boolean argument determines if the encoded data is
 * going to be restricted to 76 characters or less per line
 * as specified by RFC 2045.  If @p insertLFs is true, then
 * there will be 76 characters or less per line.
 *
 * If you use this to create a QCString remember that it is null terminated!
 * \code
 * QByteArray ar = OGlobal::encodeBase64(&array);
 * QCString str(ar.data(),ar.size()+1); // the NUL at the end
 *
 * \endcode
 *
 * @param in The QByteArray to encode
 * @param insertLFs Limit number of characters per line
 * @return The argument as base64 encoded or QByteArray() if in.isEmpty()
 * @see QByteArray
 * @see QArray
 * @see QMemArray
 * @see QCString
 */
/*
 * LGPL by Rik Hemsely of the KDE Project. taken from kmdcodec.cpp
 */
QByteArray OGlobal::encodeBase64(const QByteArray& in, bool insertLFs ) {
    if ( in.isEmpty() )
        return QByteArray();

    unsigned int sidx = 0;
    unsigned int didx = 0;
    const char* data = in.data();
    const unsigned int len = in.size();

    unsigned int out_len = ((len+2)/3)*4;

    // Deal with the 76 characters or less per
    // line limit specified in RFC 2045 on a
    // pre request basis.
    insertLFs = (insertLFs && out_len > 76);
    if ( insertLFs )
      out_len += ((out_len-1)/76);

    int count = 0;
    QByteArray out( out_len );

    // 3-byte to 4-byte conversion + 0-63 to ascii printable conversion
    if ( len > 1 )
    {
        while (sidx < len-2)
        {
            if ( insertLFs )
            {
                if ( count && (count%76) == 0 )
                    out[didx++] = '\n';
                count += 4;
            }
            out[didx++] = Base64EncMap[(data[sidx] >> 2) & 077];
            out[didx++] = Base64EncMap[(data[sidx+1] >> 4) & 017 |
                                       (data[sidx] << 4) & 077];
            out[didx++] = Base64EncMap[(data[sidx+2] >> 6) & 003 |
                                       (data[sidx+1] << 2) & 077];
            out[didx++] = Base64EncMap[data[sidx+2] & 077];
            sidx += 3;
        }
    }

    if (sidx < len)
    {
        if ( insertLFs && (count > 0) && (count%76) == 0 )
           out[didx++] = '\n';

        out[didx++] = Base64EncMap[(data[sidx] >> 2) & 077];
        if (sidx < len-1)
        {
            out[didx++] = Base64EncMap[(data[sidx+1] >> 4) & 017 |
                                       (data[sidx] << 4) & 077];
            out[didx++] = Base64EncMap[(data[sidx+1] << 2) & 077];
        }
        else
        {
            out[didx++] = Base64EncMap[(data[sidx] << 4) & 077];
        }
    }

    // Add padding
    while (didx < out.size())
    {
        out[didx] = '=';
        didx++;
    }

    return out;
}

/**
 * Decodes the given data that was encoded with the base64
 * algorithm.
 *
 *
 * @param in   the encoded data to be decoded.
 * @return the decoded QByteArray or QByteArray() in case of failure
 * @see OGlobal::encodeBase64
 */
QByteArray OGlobal::decodeBase64( const QByteArray& in) {
    if ( in.isEmpty() )
        return QByteArray();

    QByteArray out;
    unsigned int count = 0;
    unsigned int len = in.size(), tail = len;
    const char* data = in.data();

    // Deal with possible *nix "BEGIN" marker!!
    while ( count < len && (data[count] == '\n' || data[count] == '\r' ||
            data[count] == '\t' || data[count] == ' ') )
        count++;

    if ( strncasecmp(data+count, "begin", 5) == 0 )
    {
        count += 5;
        while ( count < len && data[count] != '\n' && data[count] != '\r' )
            count++;

        while ( count < len && (data[count] == '\n' || data[count] == '\r') )
            count ++;

        data += count;
        tail = (len -= count);
    }

    // Find the tail end of the actual encoded data even if
    // there is/are trailing CR and/or LF.
    while ( data[tail-1] == '=' || data[tail-1] == '\n' ||
            data[tail-1] == '\r' )
        if ( data[--tail] != '=' ) len = tail;

    unsigned int outIdx = 0;
    out.resize( (count=len) );
    for (unsigned int idx = 0; idx < count; idx++)
    {
        // Adhere to RFC 2045 and ignore characters
        // that are not part of the encoding table.
        unsigned char ch = data[idx];
        if ( (ch > 47 && ch < 58) || (ch > 64 && ch < 91 ) ||
             (ch > 96 && ch < 123)|| ch == '+' || ch == '/' || ch == '=')
        {
            out[outIdx++] = Base64DecMap[ch];
        }
        else
        {
            len--;
            tail--;
        }
    }

    // kdDebug() << "Tail size = " << tail << ", Length size = " << len << endl;

    // 4-byte to 3-byte conversion
    len = (tail>(len/4)) ? tail-(len/4) : 0;
    unsigned int sidx = 0, didx = 0;
    if ( len > 1 )
    {
      while (didx < len-2)
      {
          out[didx] = (((out[sidx] << 2) & 255) | ((out[sidx+1] >> 4) & 003));
          out[didx+1] = (((out[sidx+1] << 4) & 255) | ((out[sidx+2] >> 2) & 017));
          out[didx+2] = (((out[sidx+2] << 6) & 255) | (out[sidx+3] & 077));
          sidx += 4;
          didx += 3;
      }
    }

    if (didx < len)
        out[didx] = (((out[sidx] << 2) & 255) | ((out[sidx+1] >> 4) & 003));

    if (++didx < len )
        out[didx] = (((out[sidx+1] << 4) & 255) | ((out[sidx+2] >> 2) & 017));

    // Resize the output buffer
    if ( len == 0 || len < out.size() )
      out.resize(len);

    return out;
}

bool OGlobal::isAppLnkFileName( const QString& str )
{
    if (str.isEmpty()||str.at(str.length()-1)==QDir::separator()) return false;
    return str.startsWith(MimeType::appsFolderName()+QDir::separator());
}

/* ToDo:
 * This fun should check the document-path value for the mounted media
 * which has to be implemented later. this moment we just check for a
 * mounted media name.
 */
bool OGlobal::isDocumentFileName( const QString& file )
{
    if (file.isEmpty()||file.at(file.length()-1)==QDir::separator()) return false;
    if (file.startsWith(QPEApplication::documentDir()+QDir::separator())) return true;
    StorageInfo si;
    QList< FileSystem > fl =  si.fileSystems();
    FileSystem*fs;
    for (fs = fl.first();fs!=0;fs=fl.next()) {
        if (fs->isRemovable()&&file.startsWith(fs->name()+QDir::separator()))
            return true;
    }
    if (file.startsWith(homeDirPath()+"/Documents/")) return true;
    return false;
}

QString OGlobal::tempDirPath()
{
    static QString defstring="/tmp";
    char * tmpp = 0;
    if ( (tmpp=getenv("TEMP"))) {
        return tmpp;
    }
    return defstring;
}

QString OGlobal::homeDirPath()
{
    char * tmpp = getenv("HOME");
    return (tmpp?tmpp:"/");
}

bool OGlobal::weekStartsOnMonday()
{
    OConfig*conf=OGlobal::qpe_config();
    if (!conf)return false;
    conf->setGroup("Time");
    return conf->readBoolEntry("MONDAY",true);
}

void OGlobal::setWeekStartsOnMonday( bool what)
{
    OConfig*conf=OGlobal::qpe_config();
    if (!conf)return;
    conf->setGroup("Time");
    return conf->writeEntry("MONDAY",what);
}

bool OGlobal::useAMPM()
{
    OConfig*conf=OGlobal::qpe_config();
    if (!conf)return false;
    conf->setGroup("Time");
    return conf->readBoolEntry("AMPM",true);
}

void OGlobal::setUseAMPM( bool what)
{
    OConfig*conf=OGlobal::qpe_config();
    if (!conf)return;
    conf->setGroup("Time");
    return conf->writeEntry("AMPM",what);
}

OConfig* OGlobal::qpe_config()
{
    if ( !OGlobal::_qpe_config ) {
        OGlobal::_qpe_config = new OConfig( "qpe" );
    }
    return OGlobal::_qpe_config;
}

bool OGlobal::truncateFile( QFile &f, off_t size )
{
    /* XXX or should we allow enlarge Files? then remove this
       f.size()< part! - Alwin
    */
    if (!f.exists() || size < 0 || f.size() < unsigned(size))
        return false;

    bool closeit=false;
    if (!f.isOpen()) {
        closeit=true;
        if (!f.open(IO_Raw | IO_ReadWrite | IO_Append))
            return false;
    }
    if (!f.isOpen())
	return false;

    int fileHandle = f.handle();
    if (fileHandle == -1)
	return false;

    int r = ftruncate(fileHandle, size);
    if (closeit)
	f.close();

    return r==0;
}

bool OGlobal::createDirPath( const QString &dirPath )
{
    QDir fullDir(dirPath);
    if (fullDir.exists())
        return true;

    // at this point the directory doesn't exist
    // go through the directory tree and start creating the direcotories
    // that don't exist; if we can't create the directories, return false

    QString dirAbsPath = fullDir.absPath();
    QString dirSeps = "/";
    int dirIndex = dirAbsPath.find(dirSeps);
    QString checkedPath;

    // didn't find any seps; weird, use the cur dir instead
    if (dirIndex == -1) {
        //odebug << "No seperators found in path " << dirAbsPath << "" << oendl;
        checkedPath = QDir::currentDirPath();
    }

    while (checkedPath != dirAbsPath) {
        // no more seperators found, use the local path
        if (dirIndex == -1) {
            checkedPath = dirAbsPath;
        }
        else {
            // the next directory to check
            checkedPath = dirAbsPath.left(dirIndex) + "/";
            // advance the iterator; the next dir seperator
            dirIndex = dirAbsPath.find(dirSeps, dirIndex+1);
        }

        QDir checkDir(checkedPath);
        if (!checkDir.exists()) {
            //odebug << "mkdir making dir " << checkedPath << "" << oendl;

            if (!checkDir.mkdir(checkedPath)) {
                odebug << "Unable to make directory " << checkedPath << "" << oendl;
                return false;
            }
        }

    }
    return true;
}

