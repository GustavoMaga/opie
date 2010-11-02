/*
                             This file is part of the Opie Project
              =.             (C) 2009 Team Opie <opie-users@lists.sourceforge.net>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/


#ifndef OSOUNDRECORDER_H
#define OSOUNDRECORDER_H

#include <qobject.h>
#include "owavhandler.h"

namespace Opie {
namespace MM {

class OWavRecorder;

class OWavRecorderCallback
{
    friend class OWavRecorder;

    virtual void recorderCallback(const char *buffer, const int bytes, const int totalbytes, bool &stopflag) = 0;
};


/**
 * @brief A simple sound recording class
 *
 * This class provides an easy interface for recording audio.
 */
class OWavRecorder : public OWavHandler
{
  Q_OBJECT

  public:
    bool setup( const QString &filename, OWavFileParameters fileparams );
    void record( OWavRecorderCallback *callback );
    QString getWriteFileName();
    snd_pcm_format_t getDeviceFormat();

  protected:
    QString m_filename;
    bool setupFile();
    void finalize( int bytesWritten );

  private:
    class Private;
    Private *d;
};

}
}

#endif
