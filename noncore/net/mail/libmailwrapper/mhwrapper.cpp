#include "mhwrapper.h"
#include "mailtypes.h"
#include "mailwrapper.h"
#include <libetpan/libetpan.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <stdlib.h>
#include <qpe/global.h>
#include <opie2/oprocess.h>

using namespace Opie::Core;
MHwrapper::MHwrapper(const QString & mbox_dir,const QString&mbox_name)
    : Genericwrapper(),MHPath(mbox_dir),MHName(mbox_name)
{
    if (MHPath.length()>0) {
        if (MHPath[MHPath.length()-1]=='/') {
            MHPath=MHPath.left(MHPath.length()-1);
        }
        qDebug(MHPath);
        QDir dir(MHPath);
        if (!dir.exists()) {
            dir.mkdir(MHPath);
        }
        init_storage();
    }
}

void MHwrapper::init_storage()
{
    int r;
    QString pre = MHPath;
    if (!m_storage) {
        m_storage = mailstorage_new(NULL);
        r = mh_mailstorage_init(m_storage,(char*)pre.latin1(),0,0,0);
        if (r != MAIL_NO_ERROR) {
            qDebug("error initializing storage");
            mailstorage_free(m_storage);
            m_storage = 0;
            return;
        }
    }
    r = mailstorage_connect(m_storage);
    if (r!=MAIL_NO_ERROR) {
        qDebug("error connecting storage");
        mailstorage_free(m_storage);
        m_storage = 0;
    }
}

void MHwrapper::clean_storage()
{
    if (m_storage) {
        mailstorage_disconnect(m_storage);
        mailstorage_free(m_storage);
        m_storage = 0;
    }
}

MHwrapper::~MHwrapper()
{
    clean_storage();
}

void MHwrapper::listMessages(const QString & mailbox, QValueList<Opie::Core::OSmartPointer<RecMail> > &target )
{
    init_storage();
    if (!m_storage) {
        return;
    }
    QString f = buildPath(mailbox);
    int r = mailsession_select_folder(m_storage->sto_session,(char*)f.latin1());
    if (r!=MAIL_NO_ERROR) {
        qDebug("listMessages: error selecting folder!");
        return;
    }
    parseList(target,m_storage->sto_session,f);
    Global::statusMessage(tr("Mailbox has %1 mail(s)").arg(target.count()));
}

QValueList<Opie::Core::OSmartPointer<Folder> >* MHwrapper::listFolders()
{
    QValueList<Opie::Core::OSmartPointer<Folder> >* folders = new QValueList<Opie::Core::OSmartPointer<Folder> >();
    /* this is needed! */
    if (m_storage) mailstorage_disconnect(m_storage);
    init_storage();
    if (!m_storage) {
        return folders;
    }
    mail_list*flist = 0;
    clistcell*current=0;
    int r = mailsession_list_folders(m_storage->sto_session,NULL,&flist);
    if (r != MAIL_NO_ERROR || !flist) {
        qDebug("error getting folder list");
        return folders;
    }
    for (current=clist_begin(flist->mb_list);current!=0;current=clist_next(current)) {
        QString t = (char*)current->data;
        t.replace(0,MHPath.length(),"");
        folders->append(new MHFolder(t,MHPath));
    }
    mail_list_free(flist);
    return folders;
}

void MHwrapper::deleteMail(const RecMailP&mail)
{
    init_storage();
    if (!m_storage) {
        return;
    }
    int r = mailsession_select_folder(m_storage->sto_session,(char*)mail->getMbox().latin1());
    if (r!=MAIL_NO_ERROR) {
        qDebug("error selecting folder!");
        return;
    }
    r = mailsession_remove_message(m_storage->sto_session,mail->getNumber());
    if (r != MAIL_NO_ERROR) {
        qDebug("error deleting mail");
    }
}

void MHwrapper::answeredMail(const RecMailP&)
{
}

RecBodyP MHwrapper::fetchBody( const RecMailP &mail )
{
    RecBodyP body = new RecBody();
    init_storage();
    if (!m_storage) {
        return body;
    }
    mailmessage * msg;
    char*data=0;

    /* mail should hold the complete path! */
    int r = mailsession_select_folder(m_storage->sto_session,(char*)mail->getMbox().latin1());
    if (r != MAIL_NO_ERROR) {
        return body;
    }
    r = mailsession_get_message(m_storage->sto_session, mail->getNumber(), &msg);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error fetching mail %i",mail->getNumber());
        return body;
    }
    body = parseMail(msg);
    mailmessage_fetch_result_free(msg,data);
    return body;
}

void MHwrapper::mbox_progress( size_t current, size_t maximum )
{
    qDebug("MH %i von %i",current,maximum);
}

QString MHwrapper::buildPath(const QString&p)
{
    QString f="";
    if (p.length()==0||p=="/")
        return MHPath;
    if (!p.startsWith(MHPath)) {
        f+=MHPath;
    }
    if (!p.startsWith("/")) {
        f+="/";
    }
    f+=p;
    return f;
}

int MHwrapper::createMbox(const QString&folder,const FolderP&pfolder,const QString&,bool )
{
    init_storage();
    if (!m_storage) {
        return 0;
    }
    QString f;
    if (!pfolder) {
        // toplevel folder
        f  = buildPath(folder);
    } else {
        f = pfolder->getName();
        f+="/";
        f+=folder;
    }
    qDebug(f);
    int r = mailsession_create_folder(m_storage->sto_session,(char*)f.latin1());
    if (r != MAIL_NO_ERROR) {
        qDebug("error creating folder %i",r);
        return 0;
    }
    qDebug("Folder created");
    return 1;
}

void MHwrapper::storeMessage(const char*msg,size_t length, const QString&Folder)
{
    init_storage();
    if (!m_storage) {
        return;
    }
    QString f = buildPath(Folder);
    int r = mailsession_select_folder(m_storage->sto_session,(char*)f.latin1());
    if (r!=MAIL_NO_ERROR) {
        qDebug("error selecting folder!");
        return;
    }
    r = mailsession_append_message(m_storage->sto_session,(char*)msg,length);
    if (r!=MAIL_NO_ERROR) {
        qDebug("error storing mail");
    }
    return;
}

encodedString* MHwrapper::fetchRawBody(const RecMailP&mail)
{
    encodedString*result = 0;
    init_storage();
    if (!m_storage) {
        return result;
    }
    mailmessage * msg = 0;
    char*data=0;
    size_t size;
    int r = mailsession_select_folder(m_storage->sto_session,(char*)mail->getMbox().latin1());
    if (r!=MAIL_NO_ERROR) {
        qDebug("error selecting folder!");
        return result;
    }
    r = mailsession_get_message(m_storage->sto_session, mail->getNumber(), &msg);
    if (r != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error fetching mail %i").arg(mail->getNumber()));
        return 0;
    }
    r = mailmessage_fetch(msg,&data,&size);
    if (r != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error fetching mail %i").arg(mail->getNumber()));
        if (msg) mailmessage_free(msg);
        return 0;
    }
    result = new encodedString(data,size);
    if (msg) mailmessage_free(msg);
    return result;
}

void MHwrapper::deleteMails(const QString & mailbox,const QValueList<RecMailP> &target)
{
    QString f = buildPath(mailbox);
    int r = mailsession_select_folder(m_storage->sto_session,(char*)f.latin1());
    if (r!=MAIL_NO_ERROR) {
        qDebug("deleteMails: error selecting folder!");
        return;
    }
    QValueList<RecMailP>::ConstIterator it;
    for (it=target.begin(); it!=target.end();++it) {
        r = mailsession_remove_message(m_storage->sto_session,(*it)->getNumber());
        if (r != MAIL_NO_ERROR) {
            qDebug("error deleting mail");
            break;
        }
    }
}

int MHwrapper::deleteAllMail(const FolderP&tfolder)
{
    init_storage();
    if (!m_storage) {
        return 0;
    }
    int res = 1;
    if (!tfolder) return 0;
    int r = mailsession_select_folder(m_storage->sto_session,(char*)tfolder->getName().latin1());
    if (r!=MAIL_NO_ERROR) {
        qDebug("error selecting folder!");
        return 0;
    }
    mailmessage_list*l=0;
    r = mailsession_get_messages_list(m_storage->sto_session,&l);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error message list");
        res = 0;
    }
    unsigned j = 0;
    for(unsigned int i = 0 ; l!= 0 && res==1 && i < carray_count(l->msg_tab) ; ++i) {
        mailmessage * msg;
        msg = (mailmessage*)carray_get(l->msg_tab, i);
        j = msg->msg_index;
        r = mailsession_remove_message(m_storage->sto_session,j);
        if (r != MAIL_NO_ERROR) {
            Global::statusMessage(tr("Error deleting mail %1").arg(i+1));
            res = 0;
            break;
        }
    }
    if (l) mailmessage_list_free(l);
    return res;
}

int MHwrapper::deleteMbox(const FolderP&tfolder)
{
    init_storage();
    if (!m_storage) {
        return 0;
    }
    if (!tfolder) return 0;
    if (tfolder->getName()=="/" || tfolder->getName().isEmpty()) return 0;

    int r = mailsession_delete_folder(m_storage->sto_session,(char*)tfolder->getName().latin1());

    if (r != MAIL_NO_ERROR) {
        qDebug("error deleting mail box");
        return 0;
    }
    QString cmd = "rm -rf "+tfolder->getName();
    QStringList command;
    command << "/bin/sh";
    command << "-c";
    command << cmd.latin1();
    OProcess *process = new OProcess();

    connect(process, SIGNAL(processExited(Opie::Core::OProcess*)),
            this, SLOT( processEnded(Opie::Core::OProcess*)));
    connect(process, SIGNAL( receivedStderr(Opie::Core::OProcess*,char*,int)),
            this, SLOT( oprocessStderr(Opie::Core::OProcess*,char*,int)));

    *process << command;
    removeMboxfailed = false;
    if(!process->start(OProcess::Block, OProcess::All) ) {
        qDebug("could not start process");
        return 0;
    }
    qDebug("mail box deleted");
    return 1;
}

void MHwrapper::processEnded(OProcess *p)
{
    if (p) delete p;
}

void MHwrapper::oprocessStderr(OProcess*, char *buffer, int )
{
    QString lineStr = buffer;
    QMessageBox::warning( 0, tr("Error"), lineStr ,tr("Ok") );
    removeMboxfailed = true;
}

void MHwrapper::statusFolder(folderStat&target_stat,const QString & mailbox)
{
    init_storage();
    if (!m_storage) {
        return;
    }
    target_stat.message_count = 0;
    target_stat.message_unseen = 0;
    target_stat.message_recent = 0;
    QString f = buildPath(mailbox);
    int r = mailsession_status_folder(m_storage->sto_session,(char*)f.latin1(),&target_stat.message_count,
            &target_stat.message_recent,&target_stat.message_unseen);
    if (r != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error retrieving status"));
    }
}

MAILLIB::ATYPE MHwrapper::getType()const
{
    return MAILLIB::A_MH;
}

const QString&MHwrapper::getName()const
{
    return MHName;
}
void MHwrapper::mvcpMail(const RecMailP&mail,const QString&targetFolder,AbstractMail*targetWrapper,bool moveit)
{
    init_storage();
    if (!m_storage) {
        return;
    }
    if (targetWrapper != this) {
        qDebug("Using generic");
        Genericwrapper::mvcpMail(mail,targetFolder,targetWrapper,moveit);
        return;
    }
    qDebug("Using internal routines for move/copy");
    QString tf = buildPath(targetFolder);
    int r = mailsession_select_folder(m_storage->sto_session,(char*)mail->getMbox().latin1());
    if (r != MAIL_NO_ERROR) {
        qDebug("Error selecting source mailbox");
        return;
    }
    if (moveit) {
        r = mailsession_move_message(m_storage->sto_session,mail->getNumber(),(char*)tf.latin1());
    } else {
        r = mailsession_copy_message(m_storage->sto_session,mail->getNumber(),(char*)tf.latin1());
    }
    if (r != MAIL_NO_ERROR) {
        qDebug("Error copy/moving mail internal (%i)",r);
    }
}

void MHwrapper::mvcpAllMails(const FolderP&fromFolder,
    const QString&targetFolder,AbstractMail*targetWrapper,bool moveit)
{
    init_storage();
    if (!m_storage) {
        return;
    }
    if (targetWrapper != this) {
        qDebug("Using generic");
        Genericwrapper::mvcpAllMails(fromFolder,targetFolder,targetWrapper,moveit);
        return;
    }
    if (!fromFolder) return;
    int r = mailsession_select_folder(m_storage->sto_session,(char*)fromFolder->getName().latin1());
    if (r!=MAIL_NO_ERROR) {
        qDebug("error selecting source folder!");
        return;
    }
    QString tf = buildPath(targetFolder);
    mailmessage_list*l=0;
    r = mailsession_get_messages_list(m_storage->sto_session,&l);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error message list");
    }
    unsigned j = 0;
    for(unsigned int i = 0 ; l!= 0 && i < carray_count(l->msg_tab) ; ++i) {
        mailmessage * msg;
        msg = (mailmessage*)carray_get(l->msg_tab, i);
        j = msg->msg_index;
        if (moveit) {
            r = mailsession_move_message(m_storage->sto_session,j,(char*)tf.latin1());
        } else {
            r = mailsession_copy_message(m_storage->sto_session,j,(char*)tf.latin1());
        }
        if (r != MAIL_NO_ERROR) {
            qDebug("Error copy/moving mail internal (%i)",r);
            break;
        }
    }
    if (l) mailmessage_list_free(l);
}
