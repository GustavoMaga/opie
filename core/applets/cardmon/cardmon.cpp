/*
* cardmon.cpp
*
* ---------------------
*
* copyright   : (c) 2002 by Maximilian Reiss
* email       : max.reiss@gmx.de
* based on two apps by Devin Butterfield
*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cardmon.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/odevice.h>
#include <opie2/otaskbarapplet.h>
#include <opie2/oprocess.h>

#include <qpe/applnk.h>
#include <qpe/resource.h>
using namespace Opie::Core;
using namespace Opie::Ui;

/* QT */
#include <qcopchannel_qws.h>
#include <qpainter.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qsound.h>
#include <qtimer.h>

/* STD */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <sys/vfs.h>
#include <mntent.h>
#endif

CardMonitor::CardMonitor( QWidget * parent ) : QWidget( parent ),
pm( Resource::loadPixmap( "cardmon/pcmcia" ) ) {

    QCopChannel * pcmciaChannel = new QCopChannel( "QPE/Card", this );
    connect( pcmciaChannel,
             SIGNAL( received(const QCString&,const QByteArray&) ), this,
             SLOT( cardMessage(const QCString&,const QByteArray&) ) );

    QCopChannel *sdChannel = new QCopChannel( "QPE/Card", this );
    connect( sdChannel,
             SIGNAL( received(const QCString&,const QByteArray&) ), this,
             SLOT( cardMessage(const QCString&,const QByteArray&) ) );

    cardInPcmcia0 = FALSE;
    cardInPcmcia1 = FALSE;
    cardInSd = FALSE;
    m_process = 0;

    setFocusPolicy( NoFocus );

    setFixedWidth ( AppLnk::smallIconSize() );
    setFixedHeight ( AppLnk::smallIconSize() );

    getStatusPcmcia( TRUE );
    getStatusSd( TRUE );
    repaint( FALSE );
    popupMenu = 0;
}

CardMonitor::~CardMonitor() {
    delete popupMenu;
}

void CardMonitor::popUp( QString message, QString icon ) {
    if ( !popupMenu ) {
        popupMenu = new QPopupMenu( this );
    }

    popupMenu->clear();
    if ( icon.isEmpty() ) {
        popupMenu->insertItem( message, 0 );
    } else {
        popupMenu->insertItem( QIconSet( Resource::loadPixmap( icon ) ),
                               message, 0 );
    }

    QPoint p = mapToGlobal( QPoint( 0, 0 ) );
    QSize s = popupMenu->sizeHint();
    popupMenu->popup( QPoint( p.x() + ( width() / 2 ) - ( s.width() / 2 ),
                              p.y() - s.height() ), 0 );

    QTimer::singleShot( 2000, this, SLOT( popupTimeout() ) );
}

void CardMonitor::popupTimeout() {
    popupMenu->hide();
}

void CardMonitor::mousePressEvent( QMouseEvent * ) {
    QPopupMenu * menu = new QPopupMenu( this );
    QString cmd;

    if ( cardInSd ) {
        menu->insertItem( QIconSet( Resource::loadPixmap( "cardmon/ide" ) ),
                          tr( "Eject SD/MMC card" ), 0 );
    }

    if ( cardInPcmcia0 ) {
        menu->
        insertItem( QIconSet
                    ( Resource::loadPixmap( "cardmon/" + cardInPcmcia0Type ) ),
                    tr( "Eject card 0: %1" ).arg( cardInPcmcia0Name ), 1 );
    }

    if ( cardInPcmcia1 ) {
        menu->
        insertItem( QIconSet
                    ( Resource::loadPixmap( "cardmon/" + cardInPcmcia1Type ) ),
                    tr( "Eject card 1: %1" ).arg( cardInPcmcia1Name ), 2 );
    }

    QPoint p = mapToGlobal( QPoint( 0, 0 ) );
    QSize s = menu->sizeHint();
    int opt = menu->exec( QPoint( p.x() + ( width() / 2 ) - ( s.width() / 2 ),
                                  p.y() - s.height() ), 0 );

    if ( opt == 1 ) {
        m_commandOrig = 1;
        execCommand("/sbin/cardctl eject 0");
    } else if ( opt == 0 ) {
        if ( ODevice::inst() ->system() == System_Familiar ) {
            cmd = "umount /dev/mmc/part1";
        } else {
            cmd = "umount /dev/mmcda1";
        }
        m_commandOrig = 2;
        execCommand( cmd );
    } else if ( opt == 2 ) {
        m_commandOrig = 3;
        execCommand( "/sbin/cardctl eject 1" );
    }
    delete menu;
}


void CardMonitor::cardMessage( const QCString & msg, const QByteArray & ) {
    if ( msg == "stabChanged()" ) {
        // odebug << "Pcmcia: stabchanged" << oendl;
        getStatusPcmcia();
    } else if ( msg == "mtabChanged()" ) {
        // odebug << "CARDMONAPPLET: mtabchanged" << oendl;
        getStatusSd();
    }
}

bool CardMonitor::getStatusPcmcia( int showPopUp ) {

    bool cardWas0 = cardInPcmcia0;    // remember last state
    bool cardWas1 = cardInPcmcia1;

    QString fileName;

    // one of these 3 files should exist
    if ( QFile::exists( "/var/run/stab" ) ) {
        fileName = "/var/run/stab";
    } else if ( QFile::exists( "/var/state/pcmcia/stab" ) ) {
        fileName = "/var/state/pcmcia/stab";
    } else {
        fileName = "/var/lib/pcmcia/stab";
    }

    QFile f( fileName );

    if ( f.open( IO_ReadOnly ) ) {
        QStringList list;
        QTextStream stream( &f );
        QString streamIn;
        streamIn = stream.read();
        list = QStringList::split( "\n", streamIn );
        for ( QStringList::Iterator line = list.begin(); line != list.end();
                line++ ) {
            if ( ( *line ).startsWith( "Socket 0:" ) ) {
                if ( ( *line ).startsWith( "Socket 0: empty" ) && cardInPcmcia0 ) {
                    cardInPcmcia0 = FALSE;
                } else if ( !( *line ).startsWith( "Socket 0: empty" )
                            && !cardInPcmcia0 ) {
                    cardInPcmcia0Name =
                        ( *line ).mid( ( ( *line ).find( ':' ) + 1 ),
                                       ( *line ).length() - 9 );
                    cardInPcmcia0Name.stripWhiteSpace();
                    cardInPcmcia0 = TRUE;
                    show();
                    line++;
                    int pos = ( *line ).find( '\t' ) + 1;
                    cardInPcmcia0Type =
                        ( *line ).mid( pos, ( *line ).find( "\t", pos ) - pos );
                }
            } else if ( ( *line ).startsWith( "Socket 1:" ) ) {
                if ( ( *line ).startsWith( "Socket 1: empty" ) && cardInPcmcia1 ) {
                    cardInPcmcia1 = FALSE;
                } else if ( !( *line ).startsWith( "Socket 1: empty" )
                            && !cardInPcmcia1 ) {
                    cardInPcmcia1Name =
                        ( *line ).mid( ( ( *line ).find( ':' ) + 1 ),
                                       ( *line ).length() - 9 );
                    cardInPcmcia1Name.stripWhiteSpace();
                    cardInPcmcia1 = TRUE;
                    show();
                    line++;
                    int pos = ( *line ).find( '\t' ) + 1;
                    cardInPcmcia1Type =
                        ( *line ).mid( pos, ( *line ).find( "\t", pos ) - pos );
                }
            }
        }
        f.close();

        if ( !showPopUp
                && ( cardWas0 != cardInPcmcia0 || cardWas1 != cardInPcmcia1 ) ) {
            QString text = QString::null;
            QString what = QString::null;
            if ( cardWas0 != cardInPcmcia0 ) {
                if ( cardInPcmcia0 ) {
                    text += tr( "New card: " );
                    what = "on";
                } else {
                    text += tr( "Ejected: " );
                    what = "off";
                }
                text += cardInPcmcia0Name;
                popUp( text, "cardmon/" + cardInPcmcia0Type );
            }

            if ( cardWas1 != cardInPcmcia1 ) {
                if ( cardInPcmcia1 ) {
                    text += tr( "New card: " );
                    what = "on";
                } else {
                    text += tr( "Ejected: " );
                    what = "off";
                }
                text += cardInPcmcia1Name;
                popUp( text, "cardmon/" + cardInPcmcia1Type );
            }
            #ifndef QT_NO_SOUND
            QSound::play( Resource::findSound( "cardmon/card" + what ) );
            #endif

        }
    } else {
        // no file found
        odebug << "no file found" << oendl;
        cardInPcmcia0 = FALSE;
        cardInPcmcia1 = FALSE;
    }
    repaint( FALSE );
    return ( ( cardWas0 == cardInPcmcia0
               && cardWas1 == cardInPcmcia1 ) ? FALSE : TRUE );

}


bool CardMonitor::getStatusSd( int showPopUp ) {

    bool cardWas = cardInSd;    // remember last state
    cardInSd = FALSE;

    #if defined(_OS_LINUX_) || defined(Q_OS_LINUX)

    struct mntent *me;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );

    if ( mntfp ) {
        while ( ( me = getmntent( mntfp ) ) != 0 ) {
            QString fs = me->mnt_fsname;
            //odebug << fs << oendl;
            if ( fs.left( 14 ) == "/dev/mmc/part1" || fs.left( 7 ) == "/dev/sd"
                    || fs.left( 9 ) == "/dev/mmcd" ) {
                cardInSd = TRUE;
                show();
            }
            //            else {
            //                 cardInSd = FALSE;
            //             }
        }
        endmntent( mntfp );
    }

    if ( !showPopUp && cardWas != cardInSd ) {
        QString text = QString::null;
        QString what = QString::null;
        if ( cardInSd ) {
            text += "New card: SD/MMC";
            what = "on";
        } else {
            text += "Ejected: SD/MMC";
            what = "off";
        }
        //odebug << "TEXT: " + text << oendl;
        #ifndef QT_NO_SOUND
        QSound::play( Resource::findSound( "cardmon/card" + what ) );
        #endif

        popUp( text, "cardmon/ide" );    // XX add SD pic
    }
    #else
    #error "Not on Linux"
    #endif
    repaint( FALSE );
    return ( ( cardWas == cardInSd ) ? FALSE : TRUE );
}

void CardMonitor::paintEvent( QPaintEvent * ) {

    QPainter p( this );

    if ( cardInPcmcia0 || cardInPcmcia1 || cardInSd ) {
        p.drawPixmap( 0, 0, pm );
        show();
    } else {
        //p.eraseRect(rect());
        hide();
    }
}

int CardMonitor::position() {
    return 7;
}

void CardMonitor::execCommand( const QString &command ) {

    if ( m_process == 0 ) {
        m_process = new OProcess();
	QStringList strList = QStringList::split( " ", command );

   	for ( QStringList::Iterator it = strList.begin(); it != strList.end(); ++it ) {
        	*m_process << *it;
    	}

        connect(m_process, SIGNAL( processExited(Opie::Core::OProcess*) ),
                this, SLOT( slotExited(Opie::Core::OProcess* ) ) );

        if(!m_process->start(OProcess::NotifyOnExit, OProcess::AllOutput ) ) {
            delete m_process;
            m_process = 0;
        }
    }
}

void CardMonitor::slotExited( OProcess* proc ) {

   if( m_process->normalExit() ) {
        int ret = m_process->exitStatus();
        if( ret != 0 ) {
            if ( m_commandOrig == 1 ) {
                popUp( tr( "CF/PCMCIA card eject failed!" ) );
            } else if ( m_commandOrig == 2 ) {
                popUp( tr( "SD/MMC card eject failed!" ) );
            } else if ( m_commandOrig == 3 ) {
                popUp( tr( "CF/PCMCIA card eject failed!" ) );
            }
        }
    }

    delete m_process;
    m_process = 0;
}

EXPORT_OPIE_APPLET_v1( CardMonitor )

