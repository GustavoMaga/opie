#ifndef __MAILLISTVIEWITEM_H
#define __MAILLISTVIEWITEM_H

#include <qlistview.h>
#include <libmailwrapper/mailtypes.h>
#include <libmailwrapper/maildefines.h>

class MailListViewItem:public QListViewItem
{
public:
    MailListViewItem(QListView * parent, MailListViewItem * after );
    virtual ~MailListViewItem(){}

    void storeData(const RecMail&data);
    const RecMail&data()const;
    void showEntry();
    MAILLIB::ATYPE wrapperType();

protected:
    RecMail mail_data;
};

#endif
