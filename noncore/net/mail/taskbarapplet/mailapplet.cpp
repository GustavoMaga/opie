#include <qpainter.h>
#include <qtimer.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <opie2/odebug.h>
#include <opie2/odevice.h>

#include <libmailwrapper/settings.h>

#include "mailapplet.h"

/* UNIX */
#include <signal.h>

using namespace Opie::Core;

MailApplet::MailApplet( QWidget *parent )
: QWidget( parent ) {

    m_config = new Config( "mail" );
    m_config->setGroup( "Applet" );

    setFixedWidth( AppLnk::smallIconSize() );
    setFixedHeight( AppLnk::smallIconSize() );

    hide();

    m_newMails = 0;
    m_statusMail = 0l;

    /* for networking we must block SIGPIPE and Co. */
    struct sigaction blocking_action,temp_action;
    blocking_action.sa_handler = SIG_IGN;
    sigemptyset(&(blocking_action.sa_mask));
    blocking_action.sa_flags = 0;
    sigaction(SIGPIPE,&blocking_action,&temp_action);

    if ( !m_config->readBoolEntry( "Disabled", false ) ) {
        // delay 5 sec until the whole mail backend gets started .-)
        QTimer::singleShot( 5000, this, SLOT( startup() ) );
    }
    repaint( true );
}


MailApplet::~MailApplet() {
    if ( m_statusMail )
        delete m_statusMail;
    if ( m_config )
        delete m_config;
}

void MailApplet::paintEvent( QPaintEvent* ) {
    QPainter p( this );
    p.drawPixmap( 0, 0, Resource::loadPixmap( "mail/inbox" ) );
    QFont f( "vera", AppLnk::smallIconSize() );
    QFontMetrics fm( f );
    p.setFont( f );
    p.setPen( Qt::blue );
    p.drawText( AppLnk::smallIconSize()/3, AppLnk::smallIconSize() - 2, QString::number( m_newMails ) );
    return;

}

void MailApplet::mouseReleaseEvent( QMouseEvent* e ) {
    slotClicked();
}

void MailApplet::slotClicked() {
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << QString( "opiemail" );

    ODevice *device = ODevice::inst();
    if ( !device-> ledList().isEmpty() ) {
        OLed led = ( device->ledList().contains( Led_Mail ) ) ? Led_Mail : device->ledList()[0];

        device->setLedState( led, Led_Off );
    }
    if (m_statusMail)
        m_statusMail->reset_status();
    hide();
}

void MailApplet::startup()
{
    Settings *settings = new Settings();
    QList<Account> ma = settings->getAccounts();
    m_statusMail = new StatusMail( ma );
    delete settings;

    //m_intervalMs = m_config->readNumEntry( "CheckEvery", 5 ) * 60000;
    m_intervalMs = 100;
    m_intervalTimer = new QTimer();
    m_intervalTimer->start( m_intervalMs );
    connect( m_intervalTimer, SIGNAL( timeout() ), this, SLOT( slotCheck() ) );
}

void MailApplet::slotCheck() {
    // Check wether the check interval has been changed.
    odebug << "MailApplet::slotCheck()" << oendl;
    int newIntervalMs = m_config->readNumEntry( "CheckEvery", 5 ) * 60000;
    if ( newIntervalMs != m_intervalMs ) {
        m_intervalTimer->changeInterval( newIntervalMs );
        m_intervalMs = newIntervalMs;
    }

    if (m_statusMail == 0) {
        return;
    }

    folderStat stat;
    m_statusMail->check_current_stat( stat );
    int newMailsOld = m_newMails;
    m_newMails = stat.message_unseen;
    odebug << QString( "test %1" ).arg( m_newMails ) << oendl;
    if ( m_newMails > 0) {
        if (isHidden())
            show();
        if (newMailsOld != m_newMails) {
            ODevice *device = ODevice::inst();
            if ( m_config->readBoolEntry( "BlinkLed", true ) ) {
                if ( !device->ledList().isEmpty() ) {
                    OLed led = ( device->ledList().contains( Led_Mail ) ) ? Led_Mail : device->ledList()[0];
                    device->setLedState( led, device->ledStateList( led ).contains( Led_BlinkSlow ) ? Led_BlinkSlow : Led_On );
                }
            }
            if ( m_config->readBoolEntry( "PlaySound", false ) )
                device->playAlarmSound();
        }
        Config cfg( "mail" );
        cfg.setGroup( "Status" );
        cfg.writeEntry( "newMails", m_newMails );
        {
            odebug << "QCop abschicken" << oendl;
            QCopEnvelope env( "QPE/Pim", "newMails(int)" );
            env <<  m_newMails;
        }
        odebug << "QCop abschicken done" << oendl;
        repaint( true );
    } else {
        ODevice *device = ODevice::inst();
        if ( !isHidden() )
            hide();
        if ( !device->ledList().isEmpty() ) {
            OLed led = ( device->ledList().contains( Led_Mail ) ) ? Led_Mail : device->ledList()[0];
            device->setLedState( led, Led_Off );
        }

        if ( newMailsOld != m_newMails ) {
            Config cfg( "mail" );
            cfg.setGroup( "Status" );
            cfg.writeEntry( "newMails", m_newMails );
            QCopEnvelope env( "QPE/Pim", "newMails(int)" );
            env <<  m_newMails;
        }
    }
}
