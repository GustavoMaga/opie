#ifndef COMPOSEMAIL_H
#define COMPOSEMAIL_H

#include <qlineedit.h>
#include <qlistview.h>

#include "composemailui.h"
#include "addresspickerui.h"
#include <libmailwrapper/settings.h>
#include <libmailwrapper/mailwrapper.h>

class RecMail;

#include <opie2/osmartpointer.h>

class AddressPicker : public AddressPickerUI
{
    Q_OBJECT

public:
    AddressPicker( QWidget *parent = 0, const char *name = 0, bool modal = false, WFlags flags = 0 );
    static QString getNames();

protected:
    QString selectedNames;
    void accept();

};

class RecMail;

class ComposeMail : public ComposeMailUI
{
    Q_OBJECT

public:
    ComposeMail( Settings *s, QWidget *parent = 0, const char *name = 0, bool modal = false, WFlags flags = 0 );
    virtual ~ComposeMail();

    void reEditMail(const Opie::Core::OSmartPointer<RecMail>&current);

public slots:
    void slotAdjustColumns();

    void setTo( const QString & to );
    void setSubject( const QString & subject );
    void setInReplyTo( const QString & messageId );
    void setMessage( const QString & text );

protected slots:
    void accept();
    void reject();

private slots:
    void fillValues( int current );
    void pickAddress( QLineEdit *line );
    void pickAddressTo();
    void pickAddressCC();
    void pickAddressBCC();
    void pickAddressReply();
    void addAttachment();
    void removeAttachment();

protected:
    Settings *settings;
    QList<SMTPaccount> smtpAccounts;
    QString m_replyid;
};

class AttachViewItem : public QListViewItem
{
public:
    AttachViewItem( QListView *parent, Attachment *att );
    Attachment *getAttachment() { return attachment; }

private:
    Attachment *attachment;

};

#endif
