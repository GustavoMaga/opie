
#include <qapplication.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qpe/qcopenvelope_qws.h>
#include <opie/oprocess.h>
#include "obex.h"

using namespace  OpieObex;

Obex::Obex( QObject *parent, const char* name )
  : QObject(parent, name )
{
    m_rec = 0;
    m_send=0;
    m_count = 0;
    m_receive = false;
    connect( this, SIGNAL(error(int) ), // for recovering to receive
             SLOT(slotError() ) );
    connect( this, SIGNAL(sent(bool) ),
             SLOT(slotError() ) );
};
Obex::~Obex() {
    delete m_rec;
    delete m_send;
}
void Obex::receive()  {
    m_receive = true;
    m_outp = QString::null;
    qWarning("Receive" );
    m_rec = new OProcess();
    *m_rec << "irobex_palm3";
    // connect to the necessary slots
    connect(m_rec,  SIGNAL(processExited(OProcess*) ),
            this,  SLOT(slotExited(OProcess*) ) );

    connect(m_rec,  SIGNAL(receivedStdout(OProcess*, char*,  int ) ),
            this,  SLOT(slotStdOut(OProcess*, char*, int) ) );

    if(!m_rec->start(OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        qWarning("could not start :(");
        emit done( false );
        delete m_rec;
        m_rec = 0;
    }
//    emit currentTry(m_count );

}
void Obex::send( const QString& fileName) { // if currently receiving stop it send receive
    m_count = 0;
    m_file = fileName;
    qWarning("send %s", fileName.latin1() );
    if (m_rec != 0 ) {
        qWarning("running");
        if (m_rec->isRunning() ) {
            emit error(-1 );
            qWarning("is running");
            delete m_rec;
            m_rec = 0;

        }else{
            qWarning("is not running");
            emit error( -1 ); // we did not delete yet but it's not running slotExited is pending
            return;
        }
    }
    sendNow();
}
void Obex::sendNow(){
    qWarning("sendNow");
    if ( m_count >= 25 ) { // could not send
        emit error(-1 );
        emit sent(false);
        return;
    }
    // OProcess inititialisation
    m_send = new OProcess();
    *m_send << "irobex_palm3";
    *m_send << QFile::encodeName(m_file);

    // connect to slots Exited and and StdOut
    connect(m_send,  SIGNAL(processExited(OProcess*) ),
            this, SLOT(slotExited(OProcess*)) );
    connect(m_send,  SIGNAL(receivedStdout(OProcess*, char*,  int )),
            this, SLOT(slotStdOut(OProcess*, char*, int) ) );

    // now start it
    if (!m_send->start(/*OProcess::NotifyOnExit,  OProcess::AllOutput*/ ) ) {
        qWarning("could not send" );
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
    if (proc == m_rec ) { // receive process
        received();
    }else if ( proc == m_send ) {
        sendEnd();
    }
}
void Obex::slotStdOut(OProcess* proc, char* buf, int len){
    if ( proc == m_rec ) { // only receive
        QString str = QString::fromUtf8( buf,  len );
        m_outp.append( str );
    }
}

void Obex::received() {
  if (m_rec->normalExit() ) {
      if ( m_rec->exitStatus() == 0 ) { // we got one
          QString filename = parseOut();
          qWarning("ACHTUNG %s", filename.latin1() );
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
      qWarning("done" );
      emit sent(true);
    }else if (m_send->exitStatus() == 255 ) { // it failed maybe the other side wasn't ready
      // let's try it again
      delete m_send;
      m_send = 0;
      qWarning("try sending again" );
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
            qWarning( "%d %s", pos,  (*it).mid(6 ).latin1() ) ;
            qWarning("%d %d",  (*it).length(),  (*it).length()-pos );

            path = (*it).remove( pos, (*it).length() - pos );
            qWarning("%s",  path.latin1() );
            path = path.mid(6 );
            path = path.stripWhiteSpace();
            qWarning("path %s", path.latin1() );
        }
    }
  }
  return path;
}
/**
 * when sent is done slotError is called we  will start receive again
 */
void Obex::slotError() {
    qWarning("slotError");
    if ( m_receive )
        receive();
};
void Obex::setReceiveEnabled( bool receive ) {
    if ( !receive ) { //
        m_receive = false;
        shutDownReceive();
    }
}

void Obex::shutDownReceive() {
    if (m_rec != 0 ) {
        qWarning("running");
        if (m_rec->isRunning() ) {
            emit error(-1 );
            qWarning("is running");
            delete m_rec;
            m_rec = 0;
        }
    }

}
