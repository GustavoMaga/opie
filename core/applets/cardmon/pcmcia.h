/*
 � � � � � � � �             This file is part of the Opie Project
             =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#ifndef PCMCIA_H
#define PCMCIA_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qpopupmenu.h>

class PcmciaManager : public QWidget
{
    Q_OBJECT
public:
    enum {
	PCMCIA_Socket1,
	PCMCIA_Socket2,
	MMC_Socket
    };

    PcmciaManager( QWidget *parent = 0 );
    ~PcmciaManager();
    bool getStatusPcmcia( int showPopUp = FALSE );
    bool getStatusSd( int showPopUp = FALSE );
    static int position();
private slots:
    void cardMessage( const QCString &msg, const QByteArray & );
    void popupTimeout();

protected:
    void paintEvent( QPaintEvent* );
    void mousePressEvent( QMouseEvent * );

private:
    void execCommand( const QStringList &command );
    int m_commandOrig;
    QPixmap pm;
    // pcmcia socket 0
    bool cardInPcmcia0;
    QString cardInPcmcia0Name;
    QString cardInPcmcia0Type;
    // pcmcia socket 1
    bool cardInPcmcia1;
    QString cardInPcmcia1Name;
    QString cardInPcmcia1Type;
    bool cardInSd;
    QString cardSdName; // the device which is mounted
    void iconShow();
    QPopupMenu *popupMenu;
    void popUp(QString message, QString icon = QString::null );
};

#endif

