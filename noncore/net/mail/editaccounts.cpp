
#include "defines.h"
#include "editaccounts.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qt.h>
#include <qstringlist.h>

#include <libmailwrapper/nntpwrapper.h>

using namespace Opie::Core;

AccountListItem::AccountListItem( QListView *parent, Account *a)
        : QListViewItem( parent )
{
    account = a;
    setText( 0, account->getAccountName() );
    QString ttext = "";
    switch (account->getType()) {
        case MAILLIB::A_NNTP:
            ttext="NNTP";
            break;
        case MAILLIB::A_POP3:
            ttext = "POP3";
            break;
        case MAILLIB::A_IMAP:
            ttext = "IMAP";
            break;
        case MAILLIB::A_SMTP:
            ttext = "SMTP";
            break;
        default:
            ttext = "UNKNOWN";
            break;
    }
    setText( 1, ttext);
}

EditAccounts::EditAccounts( Settings *s, QWidget *parent, const char *name, bool modal, WFlags flags )
        : EditAccountsUI( parent, name, modal, flags )
{
    odebug << "New Account Configuration Widget" << oendl; 
    settings = s;

    mailList->addColumn( tr( "Account" ) );
    mailList->addColumn( tr( "Type" ) );

    newsList->addColumn( tr( "Account" ) );

    connect( newMail, SIGNAL( clicked() ), SLOT( slotNewMail() ) );
    connect( editMail, SIGNAL( clicked() ), SLOT( slotEditMail() ) );
    connect( deleteMail, SIGNAL( clicked() ), SLOT( slotDeleteMail() ) );
    connect( newNews, SIGNAL( clicked() ), SLOT( slotNewNews() ) );
    connect( editNews, SIGNAL( clicked() ), SLOT( slotEditNews() ) );
    connect( deleteNews, SIGNAL( clicked() ), SLOT( slotDeleteNews() ) );

    slotFillLists();
}

void EditAccounts::slotFillLists()
{
    mailList->clear();
    newsList->clear();

    QList<Account> accounts = settings->getAccounts();
    Account *it;
    for ( it = accounts.first(); it; it = accounts.next() )
    {
        if ( it->getType()==MAILLIB::A_NNTP )
        {
            (void) new AccountListItem( newsList, it );
        }
        else
        {
            (void) new AccountListItem( mailList, it );
        }
    }
}

void EditAccounts::slotNewMail()
{
    odebug << "New Mail Account" << oendl; 
    QString *selection = new QString();
    SelectMailType selType( selection, this, 0, true );
    selType.show();
    if ( QDialog::Accepted == selType.exec() )
    {
        slotNewAccount( *selection );
    }
}

void EditAccounts::slotNewAccount( const QString &type )
{
    if ( type.compare( "IMAP" ) == 0 )
    {
        odebug << "-> config IMAP" << oendl; 
        IMAPaccount *account = new IMAPaccount();
        IMAPconfig imap( account, this, 0, true );
        if ( QDialog::Accepted == QPEApplication::execDialog( &imap ) )
        {
            settings->addAccount( account );
            account->save();
            slotFillLists();
        }
        else
        {
            account->remove();
        }
    }
    else if ( type.compare( "POP3" ) == 0 )
    {
        odebug << "-> config POP3" << oendl; 
        POP3account *account = new POP3account();
        POP3config pop3( account, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == QPEApplication::execDialog( &pop3 ) )
        {
            settings->addAccount( account );
            account->save();
            slotFillLists();
        }
        else
        {
            account->remove();
        }
    }
    else if ( type.compare( "SMTP" ) == 0 )
    {
        odebug << "-> config SMTP" << oendl; 
        SMTPaccount *account = new SMTPaccount();
        SMTPconfig smtp( account, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == QPEApplication::execDialog( &smtp ) )
        {
            settings->addAccount( account );
            account->save();
            slotFillLists();

        }
        else
        {
            account->remove();
        }
    }
    else if ( type.compare( "NNTP" ) == 0 )
    {
        odebug << "-> config NNTP" << oendl; 
        NNTPaccount *account = new NNTPaccount();
        NNTPconfig nntp( account, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == QPEApplication::execDialog( &nntp ) )
        {
            settings->addAccount( account );
            account->save();
            slotFillLists();
        }
        else
        {
            account->remove();
        }
    }
}

void EditAccounts::slotEditAccount( Account *account )
{
    if ( account->getType() == MAILLIB::A_IMAP )
    {
        IMAPaccount *imapAcc = static_cast<IMAPaccount *>(account);
        IMAPconfig imap( imapAcc, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == QPEApplication::execDialog( &imap ) )
        {
            slotFillLists();
        }
    }
    else if ( account->getType()==MAILLIB::A_POP3 )
    {
        POP3account *pop3Acc = static_cast<POP3account *>(account);
        POP3config pop3( pop3Acc, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == QPEApplication::execDialog( &pop3 ) )
        {
            slotFillLists();
        }
    }
    else if ( account->getType()==MAILLIB::A_SMTP )
    {
        SMTPaccount *smtpAcc = static_cast<SMTPaccount *>(account);
        SMTPconfig smtp( smtpAcc, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == QPEApplication::execDialog( &smtp ) )
        {
            slotFillLists();
        }
    }
    else if ( account->getType()==MAILLIB::A_NNTP)
    {
        NNTPaccount *nntpAcc = static_cast<NNTPaccount *>(account);
        NNTPconfig nntp( nntpAcc, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == QPEApplication::execDialog( &nntp ) )
        {
            slotFillLists();
        }
    }
}

void EditAccounts::slotDeleteAccount( Account *account )
{
    if ( QMessageBox::information( this, tr( "Question" ),
                                   tr( "<p>Do you really want to delete the selected Account?</p>" ),
                                   tr( "Yes" ), tr( "No" ) ) == 0 )
    {
        settings->delAccount( account );
        slotFillLists();
    }
}

void EditAccounts::slotEditMail()
{
    odebug << "Edit Mail Account" << oendl; 
    if ( !mailList->currentItem() )
    {
        QMessageBox::information( this, tr( "Error" ),
                                  tr( "<p>Please select an account.</p>" ),
                                  tr( "Ok" ) );
        return;
    }

    Account *a = ((AccountListItem *) mailList->currentItem())->getAccount();
    slotEditAccount( a );
}

void EditAccounts::slotDeleteMail()
{
    if ( !mailList->currentItem() )
    {
        QMessageBox::information( this, tr( "Error" ),
                                  tr( "<p>Please select an account.</p>" ),
                                  tr( "Ok" ) );
        return;
    }

    Account *a = ((AccountListItem *) mailList->currentItem())->getAccount();
    slotDeleteAccount( a );
}

void EditAccounts::slotNewNews()
{
    odebug << "New News Account" << oendl; 
    slotNewAccount( "NNTP" );
}

void EditAccounts::slotEditNews()
{
    odebug << "Edit News Account" << oendl; 
    if ( !newsList->currentItem() )
    {
        QMessageBox::information( this, tr( "Error" ),
                                  tr( "<p>Please select an account.</p>" ),
                                  tr( "Ok" ) );
        return;
    }

    Account *a = ((AccountListItem *) newsList->currentItem())->getAccount();
    slotEditAccount( a );
}

void EditAccounts::slotDeleteNews()
{
    odebug << "Delete News Account" << oendl; 
    if ( !newsList->currentItem() )
    {
        QMessageBox::information( this, tr( "Error" ),
                                  tr( "<p>Please select an account.</p>" ),
                                  tr( "Ok" ) );
        return;
    }

    Account *a = ((AccountListItem *) newsList->currentItem())->getAccount();
    slotDeleteAccount( a );
}

void EditAccounts::slotAdjustColumns()
{
    int currPage = configTab->currentPageIndex();

    configTab->showPage( mailTab );
    mailList->setColumnWidth( 0, mailList->visibleWidth() - 50 );
    mailList->setColumnWidth( 1, 50 );

    configTab->showPage( newsTab );
    newsList->setColumnWidth( 0, newsList->visibleWidth() );

    configTab->setCurrentPage( currPage );
}

void EditAccounts::accept()
{
    settings->saveAccounts();

    QDialog::accept();
}

/**
 * SelectMailType
 */

SelectMailType::SelectMailType( QString *selection, QWidget *parent, const char *name, bool modal, WFlags flags )
        : SelectMailTypeUI( parent, name, modal, flags )
{
    selected = selection;
    selected->replace( 0, selected->length(), typeBox->currentText() );
    connect( typeBox, SIGNAL( activated(const QString&) ), SLOT( slotSelection(const QString&) ) );
}

void SelectMailType::slotSelection( const QString &sel )
{
    selected->replace( 0, selected->length(), sel );
}

/**
 * IMAPconfig
 */

IMAPconfig::IMAPconfig( IMAPaccount *account, QWidget *parent, const char *name, bool modal, WFlags flags )
        : IMAPconfigUI( parent, name, modal, flags )
{
    data = account;

    fillValues();

    connect( ComboBox1, SIGNAL( activated(int) ), SLOT( slotConnectionToggle(int) ) );
    ComboBox1->insertItem( "Only if available", 0 );
    ComboBox1->insertItem( "Always, Negotiated", 1 );
    ComboBox1->insertItem( "Connect on secure port", 2 );
    ComboBox1->insertItem( "Run command instead", 3 );
    CommandEdit->hide();
    ComboBox1->setCurrentItem( data->ConnectionType() );
}

void IMAPconfig::slotConnectionToggle( int index )
{
    if ( index == 2 )
    {
        portLine->setText( IMAP_SSL_PORT );
    }
    else if (  index == 3 )
    {
        portLine->setText( IMAP_PORT );
        CommandEdit->show();
    }
    else
    {
        portLine->setText( IMAP_PORT );
    }
}

void IMAPconfig::fillValues()
{
    accountLine->setText( data->getAccountName() );
    serverLine->setText( data->getServer() );
    portLine->setText( data->getPort() );
    ComboBox1->setCurrentItem( data->ConnectionType() );
    userLine->setText( data->getUser() );
    passLine->setText( data->getPassword() );
    prefixLine->setText(data->getPrefix());
}

void IMAPconfig::accept()
{
    data->setAccountName( accountLine->text() );
    data->setServer( serverLine->text() );
    data->setPort( portLine->text() );
    data->setConnectionType( ComboBox1->currentItem() );
    data->setUser( userLine->text() );
    data->setPassword( passLine->text() );
    data->setPrefix(prefixLine->text());

    QDialog::accept();
}

/**
 * POP3config
 */

POP3config::POP3config( POP3account *account, QWidget *parent, const char *name, bool modal, WFlags flags )
        : POP3configUI( parent, name, modal, flags )
{
    data = account;
    fillValues();

    connect( ComboBox1, SIGNAL( activated(int) ), SLOT( slotConnectionToggle(int) ) );
    ComboBox1->insertItem( "Only if available", 0 );
    ComboBox1->insertItem( "Always, Negotiated", 1 );
    ComboBox1->insertItem( "Connect on secure port", 2 );
    ComboBox1->insertItem( "Run command instead", 3 );
    CommandEdit->hide();
    ComboBox1->setCurrentItem( data->ConnectionType() );
}

void POP3config::slotConnectionToggle( int index )
{
    // 2 is ssl connection
    if ( index == 2 )
    {
        portLine->setText( POP3_SSL_PORT );
    }
    else if (  index == 3 )
    {
        portLine->setText( POP3_PORT );
        CommandEdit->show();
    }
    else
    {
        portLine->setText( POP3_PORT );
    }
}

void POP3config::fillValues()
{
    accountLine->setText( data->getAccountName() );
    serverLine->setText( data->getServer() );
    portLine->setText( data->getPort() );
    ComboBox1->setCurrentItem( data->ConnectionType() );
    userLine->setText( data->getUser() );
    passLine->setText( data->getPassword() );
}

void POP3config::accept()
{
    data->setAccountName( accountLine->text() );
    data->setServer( serverLine->text() );
    data->setPort( portLine->text() );
    data->setConnectionType( ComboBox1->currentItem() );
    data->setUser( userLine->text() );
    data->setPassword( passLine->text() );

    QDialog::accept();
}

/**
 * SMTPconfig
 */

SMTPconfig::SMTPconfig( SMTPaccount *account, QWidget *parent, const char *name, bool modal, WFlags flags )
        : SMTPconfigUI( parent, name, modal, flags )
{
    data = account;

    connect( loginBox, SIGNAL( toggled(bool) ), userLine, SLOT( setEnabled(bool) ) );
    connect( loginBox, SIGNAL( toggled(bool) ), passLine, SLOT( setEnabled(bool) ) );

    fillValues();

    connect( ComboBox1, SIGNAL( activated(int) ), SLOT( slotConnectionToggle(int) ) );
    ComboBox1->insertItem( "Only if available", 0 );
    ComboBox1->insertItem( "Always, Negotiated", 1 );
    ComboBox1->insertItem( "Connect on secure port", 2 );
    ComboBox1->insertItem( "Run command instead", 3 );
    CommandEdit->hide();
    ComboBox1->setCurrentItem( data->ConnectionType() );
}

void SMTPconfig::slotConnectionToggle( int index )
{
    // 2 is ssl connection
    if ( index == 2 )
    {
        portLine->setText( SMTP_SSL_PORT );
    }
    else if (  index == 3 )
    {
        portLine->setText( SMTP_PORT );
        CommandEdit->show();
    }
    else
    {
        portLine->setText( SMTP_PORT );
    }
}

void SMTPconfig::fillValues()
{
    accountLine->setText( data->getAccountName() );
    serverLine->setText( data->getServer() );
    portLine->setText( data->getPort() );
    ComboBox1->setCurrentItem( data->ConnectionType() );
    loginBox->setChecked( data->getLogin() );
    userLine->setText( data->getUser() );
    passLine->setText( data->getPassword() );
}

void SMTPconfig::accept()
{
    data->setAccountName( accountLine->text() );
    data->setServer( serverLine->text() );
    data->setPort( portLine->text() );
    data->setConnectionType( ComboBox1->currentItem() );
    data->setLogin( loginBox->isChecked() );
    data->setUser( userLine->text() );
    data->setPassword( passLine->text() );

    QDialog::accept();
}

/**
 * NNTPconfig
 */

NNTPconfig::NNTPconfig( NNTPaccount *account, QWidget *parent, const char *name, bool modal, WFlags flags )
        : NNTPconfigUI( parent, name, modal, flags )
{
    data = account;

    connect( loginBox, SIGNAL( toggled(bool) ), userLine, SLOT( setEnabled(bool) ) );
    connect( loginBox, SIGNAL( toggled(bool) ), passLine, SLOT( setEnabled(bool) ) );
    connect( GetNGButton,  SIGNAL( clicked() ), this, SLOT( slotGetNG() ) );
    fillValues();

    connect( sslBox, SIGNAL( toggled(bool) ), SLOT( slotSSL(bool) ) );
}

void NNTPconfig::slotGetNG() {
    save();
    data->save();
    NNTPwrapper* tmp = new NNTPwrapper( data );
    QStringList list =  tmp->listAllNewsgroups();
    
    ListViewGroups->clear();

    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
         QCheckListItem *item;
         item = new QCheckListItem( ListViewGroups, (*it), QCheckListItem::CheckBox );
         if ( subscribedGroups.contains( (*it) ) >= 1 ) {
             item->setOn( true );
         }
    }
}

void NNTPconfig::slotSSL( bool enabled )
{
    if ( enabled )
    {
        portLine->setText( NNTP_SSL_PORT );
    }
    else
    {
        portLine->setText( NNTP_PORT );
    }
}

void NNTPconfig::fillValues()
{
    accountLine->setText( data->getAccountName() );
    serverLine->setText( data->getServer() );
    portLine->setText( data->getPort() );
    sslBox->setChecked( data->getSSL() );
    loginBox->setChecked( data->getLogin() );
    userLine->setText( data->getUser() );
    passLine->setText( data->getPassword() );
    subscribedGroups = data->getGroups();
    /* don't forget that - you will overwrite values if user clicks cancel! */
    for ( QStringList::Iterator it = subscribedGroups.begin(); it != subscribedGroups.end(); ++it ) {
         QCheckListItem *item;
         item = new QCheckListItem( ListViewGroups, (*it), QCheckListItem::CheckBox );
         item->setOn( true );
    }
}

void NNTPconfig::save()
{
    data->setAccountName( accountLine->text() );
    data->setServer( serverLine->text() );
    data->setPort( portLine->text() );
    data->setSSL( sslBox->isChecked() );
    data->setLogin( loginBox->isChecked() );
    data->setUser( userLine->text() );
    data->setPassword( passLine->text() );

   QListViewItemIterator list_it( ListViewGroups );

       QStringList groupList;
   for ( ; list_it.current(); ++list_it ) {

       if ( ( (QCheckListItem*)list_it.current() )->isOn() ) {
          odebug << list_it.current()->text(0) << oendl; 
          groupList.append(  list_it.current()->text(0) );
      }

  }
      data->setGroups( groupList );
}

void NNTPconfig::accept()
{
    save();
    QDialog::accept();
}

