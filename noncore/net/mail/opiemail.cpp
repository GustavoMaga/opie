#include <qmessagebox.h>
#include "opiemail.h"
#include "editaccounts.h"
#include "composemail.h"
#include "smtpwrapper.h"

OpieMail::OpieMail( QWidget *parent, const char *name, WFlags flags )
    : MainWindow( parent, name, flags )
{
    settings = new Settings();

    folderView->populate( settings->getAccounts() );

    connect( composeMail, SIGNAL( activated() ), SLOT( slotComposeMail() ) );
    connect( sendQueued, SIGNAL( activated() ), SLOT( slotSendQueued() ) );
//    connect( searchMails, SIGNAL( activated() ), SLOT( slotSearchMails() ) );
    connect( editSettings, SIGNAL( activated() ), SLOT( slotEditSettings() ) );
    connect( editAccounts, SIGNAL( activated() ), SLOT( slotEditAccounts() ) );
}

void OpieMail::slotComposeMail()
{
    qDebug( "Compose Mail" );
    ComposeMail compose( settings, this, 0 , true );
    compose.showMaximized();
    compose.slotAdjustColumns();
    compose.exec();
}

void OpieMail::slotSendQueued()
{
    qDebug( "Send Queued" );
    SMTPaccount *smtp = 0;

    QList<Account> list = settings->getAccounts();
    Account *it;
//    if (list.count()==1) {
        for ( it = list.first(); it; it = list.next() ) {
            if ( it->getType().compare( "SMTP" ) == 0 ) {
                smtp = static_cast<SMTPaccount *>(it);
                break;
            }
        }
//    }
    if (smtp) {
        SMTPwrapper * wrap = new SMTPwrapper(settings);
        wrap->flushOutbox(smtp);
        QMessageBox::information(0,tr("Info"),tr("Mail queue flushed"));
    }
}

void OpieMail::slotSearchMails()
{
    qDebug( "Search Mails" );
}

void OpieMail::slotEditSettings()
{
    qDebug( "Edit Settings" );
}

void OpieMail::slotEditAccounts()
{
    qDebug( "Edit Accounts" );
    EditAccounts eaDialog( settings, this, 0, true );
    eaDialog.showMaximized();
    eaDialog.slotAdjustColumns();
    eaDialog.exec();
    if ( settings ) delete settings;
    settings = new Settings();

    folderView->populate( settings->getAccounts() );
}

