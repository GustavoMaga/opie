
#include "pop3wrapper.h"
#include "mailtypes.h"
#include <libetpan/mailpop3.h>

POP3wrapper::POP3wrapper( POP3account *a )
{
    account = a;
    m_pop3 = NULL;
}

POP3wrapper::~POP3wrapper()
{
    logout();
}

void POP3wrapper::pop3_progress( size_t current, size_t maximum )
{
    qDebug( "POP3: %i of %i", current, maximum );
}

void POP3wrapper::listMessages( QList<RecMail> &target )
{
    int err = MAILPOP3_NO_ERROR;
    char *header;
    size_t length;
    carray *messages;

    login();
    if (!m_pop3) return;
    mailpop3_list( m_pop3, &messages );

    for ( int i = carray_count( messages ); i > 0; i-- ) {
        mailpop3_msg_info *info = (mailpop3_msg_info *) carray_get( messages, i - 1 );

        err = mailpop3_header( m_pop3, info->msg_index, &header, &length );
        if ( err != MAILPOP3_NO_ERROR ) {
            qDebug( "POP3: error retrieving header msgid: %i", info->msg_index );
            logout();
            return;
        }
        RecMail *mail = parseHeader( header );
        mail->setNumber( info->msg_index );
        target.append( mail );
    }

    logout();
}

RecMail *POP3wrapper::parseHeader( const char *header )
{
    int err = MAILIMF_NO_ERROR;
    size_t curTok;
    RecMail *mail = new RecMail();
    mailimf_fields *fields;
    
    err = mailimf_fields_parse( (char *) header, strlen( header ), &curTok, &fields );
    for ( clistiter *current = clist_begin( fields->fld_list ); current != NULL; current = current->next ) {
        mailimf_field *field = (mailimf_field *) current->data;
        switch ( field->fld_type ) {
            case MAILIMF_FIELD_FROM:
                mail->setFrom( *parseMailboxList( field->fld_data.fld_from->frm_mb_list ) );
                break;
            case MAILIMF_FIELD_TO:
                mail->setTo( *parseAddressList( field->fld_data.fld_to->to_addr_list ) );
                break;
            case MAILIMF_FIELD_CC:
                mail->setCC( *parseAddressList( field->fld_data.fld_cc->cc_addr_list ) );
                break;
            case MAILIMF_FIELD_BCC:
                mail->setBcc( *parseAddressList( field->fld_data.fld_bcc->bcc_addr_list ) );
                break;
            case MAILIMF_FIELD_SUBJECT:
                mail->setSubject( QString( field->fld_data.fld_subject->sbj_value ) );
                break;
            case MAILIMF_FIELD_ORIG_DATE:
                mail->setDate( *parseDateTime( field->fld_data.fld_orig_date->dt_date_time ) );
                break;
            default:
                break;
        }
    }
    
    return mail;
}

QString *POP3wrapper::parseDateTime( mailimf_date_time *date )
{
    char tmp[23];
    
    snprintf( tmp, 23,  "%02i.%02i.%04i %02i:%02i:%02i %+05i", 
        date->dt_day, date->dt_month, date->dt_year, date->dt_hour, date->dt_min, date->dt_sec, date->dt_zone );
    
    QString *result = new QString( tmp );

    return result;
}

QString *POP3wrapper::parseAddressList( mailimf_address_list *list )
{
    QString *result = new QString( "" );

    bool first = true;
    for ( clistiter *current = clist_begin( list->ad_list ); current != NULL; current = current->next ) {
        mailimf_address *addr = (mailimf_address *) current->data;
        
        if ( !first ) {
            result->append( "," );
        } else {
            first = false;
        }

        QString *tmp;
        
        switch ( addr->ad_type ) {
            case MAILIMF_ADDRESS_MAILBOX:
                tmp = parseMailbox( addr->ad_data.ad_mailbox );
                result->append( *tmp );
                delete tmp;
                break;
            case MAILIMF_ADDRESS_GROUP:
                tmp = parseGroup( addr->ad_data.ad_group );
                result->append( *tmp );
                delete tmp;
                break;
            default:
                qDebug( "POP3: unkown mailimf address type" );
                break;
        }
    }
    
    return result;
}

QString *POP3wrapper::parseGroup( mailimf_group *group )
{
    QString *result =  new QString( "" );

    result->append( group->grp_display_name );
    result->append( ": " );

    if ( group->grp_mb_list != NULL ) {
        QString *tmp = parseMailboxList( group->grp_mb_list );
        result->append( *tmp );
        delete tmp;
    }

    result->append( ";" );
    
    return result;
}

QString *POP3wrapper::parseMailbox( mailimf_mailbox *box )
{
    QString *result =  new QString( "" );

    if ( box->mb_display_name == NULL ) {
        result->append( box->mb_addr_spec );
    } else {
        result->append( box->mb_display_name );
        result->append( " <" );
        result->append( box->mb_addr_spec );
        result->append( ">" );
    }
        
    return result;
}

QString *POP3wrapper::parseMailboxList( mailimf_mailbox_list *list )
{
    QString *result =  new QString( "" );

    bool first = true;
    for ( clistiter *current = clist_begin( list->mb_list ); current != NULL; current = current->next ) {
        mailimf_mailbox *box = (mailimf_mailbox *) current->data;

        if ( !first ) {
            result->append( "," );
        } else {
            first = false;
        }
       
        QString *tmp = parseMailbox( box );
        result->append( *tmp );
        delete tmp;
    }
    
    return result;
}

void POP3wrapper::login()
{
    if ( m_pop3 != NULL ) logout();

    const char *server, *user, *pass;
    uint16_t port;
    int err = MAILPOP3_NO_ERROR;

    server = account->getServer().latin1();
    port = account->getPort().toUInt();
    user = account->getUser().latin1();
    pass = account->getPassword().latin1();

    m_pop3 = mailpop3_new( 200, &pop3_progress );

    // connect
    if (account->getSSL()) {
        err = mailpop3_ssl_connect( m_pop3, (char*)server, port );
    } else {
        err = mailpop3_socket_connect( m_pop3, (char*)server, port );
    }

    if ( err != MAILPOP3_NO_ERROR ) {
        qDebug( "pop3: error connecting to %s\n reason: %s", server,
                m_pop3->pop3_response );
        mailpop3_free( m_pop3 );
        m_pop3 = NULL;
        return;
    }
    qDebug( "POP3: connected!" );

    // login
    // TODO: decide if apop or plain login should be used
    err = mailpop3_login( m_pop3, (char *) user, (char *) pass );
    if ( err != MAILPOP3_NO_ERROR ) {
        qDebug( "pop3: error logging in: %s", m_pop3->pop3_response );
        logout();
        return;
    }

    qDebug( "POP3: logged in!" );
}

void POP3wrapper::logout()
{
    int err = MAILPOP3_NO_ERROR;
    if ( m_pop3 == NULL ) return;
    err = mailpop3_quit( m_pop3 );
    mailpop3_free( m_pop3 );
    m_pop3 = NULL;
}


