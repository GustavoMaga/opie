#ifndef __MBOX_WRAPPER_H
#define __MBOX_WRAPPER_H

#include "genericwrapper.h"
#include <qstring.h>

class RecMail;
class RecBody;

class MBOXwrapper : public Genericwrapper
{
    Q_OBJECT
public:
    MBOXwrapper(const QString & dir);
    virtual ~MBOXwrapper();
    
    virtual void listMessages(const QString & mailbox, QList<RecMail> &target );
    virtual QList<Folder>* listFolders();

    virtual void deleteMail(const RecMail&mail);
    virtual void answeredMail(const RecMail&mail);
    virtual void createFolder(const QString&aFolder);
    virtual void storeMessage(const char*msg,size_t length, const QString&folder);

    virtual RecBody fetchBody( const RecMail &mail );
    static void mbox_progress( size_t current, size_t maximum );

    virtual void fetchRawBody(const RecMail&mail,char**target,size_t*length);
    virtual void deleteMails(const QString & mailbox,QList<RecMail> &target);

protected:
    QString MBOXPath;
};

#endif
