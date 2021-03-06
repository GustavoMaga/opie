#include "obex.h"
#include "btobex.h"
#include "obexbase.h"
#include "receiver.h"
using namespace OpieObex;

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/filemanager.h>
using namespace Opie::Core;

/* QT */
#include <qfileinfo.h>
#include <qlabel.h>
#include <qtextview.h>
#include <qpushbutton.h>

/* STD */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h> // int system
#include <unistd.h>
#include <fcntl.h>

/* TRANSLATOR OpieObex::Receiver */

Receiver::Receiver(RecType type)
{
    if (type == REC_IRDA)
        m_obex = new Obex(this, "Receiver");
#ifdef BLUETOOTH
    else
        m_obex = new BtObex(this, "Receiver");
#endif
    connect(m_obex, SIGNAL(receivedFile(const QString&) ),
            this, SLOT(slotReceived(const QString&) ) );
    m_obex->receive();
}

Receiver::~Receiver()
{
    m_obex->setReceiveEnabled( false );
    delete m_obex;
}

void Receiver::slotReceived( const QString& _file )
{
    QString file = _file;
    int check = checkFile(file);
    if ( check == AddressBook )
        handleAddr( file );
    else if ( check == Datebook )
        handleDateTodo( file );
    else
        handleOther( file );
}

void Receiver::handleAddr( const QString& str )
{
    QCopEnvelope e("QPE/Application/addressbook", "setDocument(QString)" );
    e << str;
}

/* we can not say for sure if it's a VEevent ot VTodo */
void Receiver::handleDateTodo( const QString& str )
{
    QCopEnvelope e0("QPE/Application/todolist", "setDocument(QString)");
    e0 << str;
    QCopEnvelope e1("QPE/Application/datebook", "setDocument(QString)" );
    e1 << str;
}

/*
 * Handle other asks if it should accept the
 * beamed object and creates a DocLnk
 */
void Receiver::handleOther( const QString& other )
{
    OtherHandler* hand =  new OtherHandler();
    hand->handle( other );
}

void Receiver::tidyUp( QString& _file, const QString& ending)
{
    /* libversit fails on BASE64 encoding we try to sed it away */
    QString file = _file;
    char foo[24]; // big enough
    (void)::strcpy(foo, "/tmp/opie-XXXXXX");

    int fd = ::mkstemp(foo);

    if ( fd == -1 )
        return;

    (void)::strncat( foo, QFile::encodeName(ending), 4 );
    _file = QString::fromLocal8Bit( foo );
    QString cmd = QString("sed -e \"s/^\\(X-MICROSOFT-BODYINK\\)\\;/\\1:/;\" < %2 > %2 ").arg( Global::shellQuote(file)).arg( Global::shellQuote(_file) );
    (void)::system( QFile::encodeName(cmd) );

    cmd = QString("rm %1").arg( Global::shellQuote(file) );
    (void)::system( QFile::encodeName(cmd) );
}

int Receiver::checkFile( QString& file )
{
    int ret;
    QString ending;

    if (file.right(4).lower() == ".vcs" ) {
        ret = Datebook;
        ending = QString::fromLatin1(".vcs");
    }
    else if ( file.right(4).lower() == ".vcf") {
        ret = AddressBook;
        ending = QString::fromLatin1(".vcf");
    }
    else
        ret = Other;


    if (ending.isEmpty() )
        return ret;

    /**
     * currently the parser is broken in regard of BASE64 encoding
     * and M$ likes to send that. So we will executed a small
     * tidy up system sed script
     * At this point we can also remove umlaute from the filename
     */
    tidyUp( file, ending );

    return ret;
}

/* TRANSLATOR OpieObex::OtherHandler */

OtherHandler::OtherHandler()
    : QVBox()
{
    QHBox* box = new QHBox(this);
    QLabel* lbl = new QLabel(box);
    lbl->setText(tr("<qt><b>Received:</b></qt>"));
    m_na = new QLabel(box);

    QFrame* frame = new QFrame(this);
    frame->setFrameShape( QFrame::HLine );
    frame->setFrameShadow( QFrame::Sunken );

    m_view = new QTextView(this);

    box = new QHBox(this);
    QPushButton *but = new QPushButton(box);
    but->setText(tr("Accept") );
    connect(but, SIGNAL(clicked() ),
            this, SLOT(accept()) );

    but = new QPushButton(box);
    but->setText(tr("Deny") );
    connect(but, SIGNAL(clicked() ),
            this, SLOT(deny() ) );

    raise();
    showMaximized();
}

OtherHandler::~OtherHandler()
{

}

void OtherHandler::handle( const QString& file )
{
    m_file = file;
    m_na->setText(file);
    DocLnk lnk(file);

    QString str = tr("<p>You received a file of type %1 (<img src=\"%2\"> )What do you want to do?").arg(lnk.type() ).arg(lnk.icon() );
    m_view->setText( str );
}

/*
 * hehe evil evil mmap ahead :)
 * we quickly copy the file and then we'll create a DocLnk for it
 */
void OtherHandler::accept()
{
    QString na = targetName( m_file );
    copy(m_file, na );
    DocLnk lnk(na);
    lnk.writeLink();
    QFile::remove(m_file);
    delete this;
}

void OtherHandler::deny()
{
    QFile::remove( m_file );
    delete this;
}

QString OtherHandler::targetName( const QString& file )
{
    QFileInfo info( file );

    /* $HOME needs to be set!!!! */
    Global::createDocDir();

    QString newFile = QPEApplication::documentDir()+ "/"+ info.baseName();
    QString newFileBase = newFile;

    int trie = 0;
    while (QFile::exists(newFile + "."+info.extension() ) ) {
        newFile = newFileBase + "_"+QString::number(trie) ;
        trie++;
    }
    newFile += "." + info.extension();

    return newFile;
}

/* fast cpy */
void OtherHandler::copy(const QString& src, const QString& file) {
    FileManager *fm;
    if(!fm->copyFile(src,file))
        owarn << "Copy failed" << oendl;
}
