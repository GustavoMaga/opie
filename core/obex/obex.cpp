/*
� � � � � � � �=.            This file is part of the OPIE Project
� � � � � � �.=l.            Copyright (c)  2002 Maximilian Reiss <max.reiss@gmx.de>
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This library is free software; you can
.> <, � > �. � <=           redistribute it and/or  modify it under
:=1 )Y*s>-.-- � :            the terms of the GNU Library General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; version 2 of the License.
� � �._= =} � � � :
� � .%+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This library is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful,  but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|     MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>:      PARTICULAR PURPOSE. See the GNU
..}^=.= � � � = � � � ;      Library General Public License for more
++= � -. � � . � � .:        details.
�: � � = �...= . :.=-
�-. � .:....=;==+<;          You should have received a copy of the GNU
� -_. . . � )=. �=           Library General Public License along with
  � -- � � � �:-=            this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/
/*
 * The Infrared OBEX handling class implementation
 */

#include "obex.h"

/* OPIE */
#include <opie2/oprocess.h>
#include <opie2/odebug.h>

/* QT */
#include <qfileinfo.h>



using namespace  OpieObex;

using namespace Opie::Core;
/* TRANSLATOR OpieObex::Obex */

Obex::Obex( QObject *parent, const char* name )
  : ObexBase(parent, name )
{
    m_rec = 0;
    m_send = 0;
};
Obex::~Obex() {
    delete m_rec;
    delete m_send;
}
void Obex::receive()  {
    ObexBase::receive();
    m_rec = new ObexServer(OBEX_TRANS_IRDA);
    // connect to the necessary slots
    connect(m_rec,  SIGNAL(processExited(Opie::Core::OProcess*) ),
            this,  SLOT(slotExited(Opie::Core::OProcess*) ) );

    connect(m_rec,  SIGNAL(receivedStdout(Opie::Core::OProcess*, char*,  int ) ),
            this,  SLOT(slotStdOut(Opie::Core::OProcess*, char*, int) ) );

    if(!m_rec->start(OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        emit done( false );
        delete m_rec;
        m_rec = 0;
    }
}

// if currently receiving stop it send receive
void Obex::send(const QString& fileName, const QString& addr) {
    ObexBase::send(fileName, addr);
    if (m_rec != 0 ) {
        if (m_rec->isRunning() ) {
            emit error(-1 );
            delete m_rec;
            m_rec = 0;
        }else{
            emit error( -1 ); // we did not delete yet but it's not running slotExited is pending
            return;
        }
    }
    sendNow();
}
void Obex::sendNow(){
    if ( m_count >= 25 ) { // could not send
        emit error(-1 );
        emit sent(false);
        return;
    }
    // OProcess inititialisation
    m_send = new OProcess();
    m_send->setWorkingDirectory( QFileInfo(m_file).dirPath(true) );

    *m_send << "irobex_palm3";
    *m_send << QFile::encodeName(QFileInfo(m_file).fileName());

    // connect to slots Exited and and StdOut
    connect(m_send,  SIGNAL(processExited(Opie::Core::OProcess*) ),
            this, SLOT(slotExited(Opie::Core::OProcess*)) );
    connect(m_send,  SIGNAL(receivedStdout(Opie::Core::OProcess*, char*,  int )),
            this, SLOT(slotStdOut(Opie::Core::OProcess*, char*, int) ) );

    // now start it
    if (!m_send->start(/*OProcess::NotifyOnExit,  OProcess::AllOutput*/ ) ) {
        m_count = 25;
        emit error(-1 );
        delete m_send;
        m_send=0;
    }
    // end
    m_count++;
    emit currentTry( m_count );
}

void Obex::slotExited(OProcess* proc ){
    if (proc == m_rec )  // receive process
        received();
    else if ( proc == m_send )
        sendEnd();

}
void Obex::slotStdOut(OProcess* proc, char* buf, int len){
    if ( proc == m_rec ) { // only receive
        QByteArray ar( len  );
        memcpy( ar.data(), buf, len );
        m_outp.append( ar );
    }
}

void Obex::received() {
  if (m_rec->normalExit() ) {
      if ( m_rec->exitStatus() == 0 ) { // we got one
          QString filename = parseOut();
          emit receivedFile( filename );
      }
  }else{
      emit done(false);
  };
  delete m_rec;
  m_rec = 0;
  receive();
}

void Obex::sendEnd() {
  if (m_send->normalExit() ) {
    if ( m_send->exitStatus() == 0 ) {
      delete m_send;
      m_send=0;
      emit sent(true);
    }else if (m_send->exitStatus() == 255 ) { // it failed maybe the other side wasn't ready
      // let's try it again
      delete m_send;
      m_send = 0;
      sendNow();
    }
  }else {
    emit error( -1 );
    delete m_send;
    m_send = 0;
  }
}
QString Obex::parseOut(     ){
  QString path;
  QStringList list = QStringList::split("\n",  m_outp);
  QStringList::Iterator it;
  for (it = list.begin(); it != list.end(); ++it ) {
    if ( (*it).startsWith("Wrote"  ) ) {
        int pos = (*it).findRev('(' );
        if ( pos > 0 ) {

            path = (*it).remove( pos, (*it).length() - pos );
            path = path.mid(6 );
            path = path.stripWhiteSpace();
        }
    }
  }
  return path;
}
/**
 * when sent is done slotError is called we  will start receive again
 */
void Obex::slotError() {
    ObexBase::slotError();
    if ( m_receive )
        receive();
};
void Obex::setReceiveEnabled( bool receive ) {
    ObexBase::setReceiveEnabled(receive);
    if ( !receive ) { //
        m_receive = false;
        shutDownReceive();
    }
}

void Obex::shutDownReceive() {
    if (m_rec != 0 ) {
        if (m_rec->isRunning() ) {
            emit error(-1 );
            delete m_rec;
            m_rec = 0;
        }
    }

}
