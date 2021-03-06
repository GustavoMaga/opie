/*
                             This file is part of the Opie Project

                             Copyright (C)2002-2005 The Opie Team <opie-devel@lists.sourceforge.net>
              =.
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

#include "odevice_yopy.h"

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>

/* OPIE */
#include <qpe/config.h>
#include <qpe/sound.h>
#include <qpe/qcopenvelope_qws.h>

#include <opie2/oresource.h>

/* STD */
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#ifndef QT_NO_SOUND
#include <linux/soundcard.h>
#endif

using namespace Opie::Core;
using namespace Opie::Core::Internal;

void Yopy::init(const QString&)
{
    d->m_vendorstr = "G.Mate";
    d->m_vendor = Vendor_GMate;
    d->m_modelstr = "Yopy3700";
    d->m_model = Model_Yopy_3700;
    d->m_rotation = Rot0;
    d->m_systemstr = "Linupy";
    d->m_system = System_Linupy;
    // Distribution detection code now in the base class
}


bool Yopy::suspend()
{
    /* Opie for Yopy does not implement its own power management at the
        moment.  The public version runs parallel to X, and relies on the
        existing power management features. */
    return false;
}


bool Yopy::setDisplayBrightness( int bright )
{
    /* The code here works, but is disabled as the current version runs
        parallel to X, and relies on the existing backlight demon. */
#if 0
    if ( QFile::exists( "/proc/sys/pm/light" ) )
    {
        int fd = ::open( "/proc/sys/pm/light", O_WRONLY );
        if ( fd >= 0 )
        {
            if ( bright )
                ::write( fd, "1\n", 2 );
            else
                ::write( fd, "0\n", 2 );
            ::close( fd );
            return true;
        }
    }
#else
    Q_UNUSED( bright )
#endif
    return false;
}


int Yopy::displayBrightnessResolution() const
{
    return 2;
}

