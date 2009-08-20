/*
 � � � � � � � �             This file is part of the Opie Project
� � � � � � �                Copyright (C) 2004 Andreas 'ar' Richter <ar@oszine.de>
             =.
            .=l.
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This program is free software; you can
.> <`_, � > �. � <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU Library General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
� � .%`+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This program is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful,  but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|`    MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.= � � � = � � � ;      Library General Public License for more
++= � -. � � .` � � .:       details.
�: � � = �...= . :.=-
�-. � .:....=;==+<;          You should have received a copy of the GNU
� -_. . . � )=. �=           Library General Public License along with
� � -- � � � �:-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <opie2/ostorageinfo.h>

using namespace Opie::Core;

OStorageInfo::OStorageInfo( QObject *parent )
        : StorageInfo( parent )
{
}

OStorageInfo::~OStorageInfo()
{
}

QString OStorageInfo::cfPath()const
{
    QString r = "";

    for (QListIterator<FileSystem> i( fileSystems() ); i.current(); ++i)
    {
        const FileSystem *fs = (*i);
        if ( fs->path() == "/media/cf" )
        {
            r = "/media/cf";
            break;
        }
    }

    if ( r == "" )
    {
        for (QListIterator<FileSystem> i( fileSystems() ); i.current(); ++i)
        {
            const FileSystem *fs = (*i);
            if ( fs->disk().left( 8 ) == "/dev/hda"
                    && fs->path().left(6) == "/media" )
            {
                r = (*i)->path();
                break;
            }
        }
    }
    return r;
}

QString OStorageInfo::sdPath()const
{
    QString r = "";

    for (QListIterator<FileSystem> i( fileSystems() ); i.current(); ++i)
    {
        if ( (*i)->disk().left( 9 ) == "/dev/mmcd" ||
             (*i)->disk().left( 14 ) == "/dev/mmc/part1" )
        {
            r = (*i)->path();
            break;
        }
    }
    return r;
}

QString OStorageInfo::mmcPath()const
{
    QString r = "";

    for (QListIterator<FileSystem> i( fileSystems() ); i.current(); ++i)
    {
        if ( (*i)->disk().left( 14 ) == "/dev/mmc/part1"||
             (*i)->disk().left(11) == "/dev/mmcblk" )
        {
            r = (*i)->path();
            break;
        }
    }
    return r;
}

