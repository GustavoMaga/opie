#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <qdir.h>

#include "mailwrapper.h"
#include "logindialog.h"
//#include "mail.h"
#include "defines.h"

Attachment::Attachment( DocLnk lnk )
{
    doc = lnk;
    size = QFileInfo( doc.file() ).size();
}

Folder::Folder(const QString&tmp_name, const QString&sep  )
{
    name = tmp_name;
    nameDisplay = name;
    separator = sep;
}

const QString& Folder::Separator()const
{
    return separator;
}

IMAPFolder::IMAPFolder(const QString&name,const QString&sep, bool select,const QString&prefix )
    : Folder( name,sep ),m_MaySelect(select)
{
  // Decode IMAP foldername
  nameDisplay = IMAPFolder::decodeFolderName( name );
  qDebug( "folder " + name + " - displayed as " + nameDisplay );

    if (prefix.length()>0) {
        if (nameDisplay.startsWith(prefix) && nameDisplay.length()>prefix.length()) {
            nameDisplay=nameDisplay.right(nameDisplay.length()-prefix.length());
        }
    }
}

static unsigned char base64chars[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+,";

/**
 * Decodes base64 encoded parts of the imapfolder name
 * Code taken from kde cvs: kdebase/kioslave/imap4/rfcdecoder.cc
 */
QString IMAPFolder::decodeFolderName( const QString &name )
{
  unsigned char c, i, bitcount;
  unsigned long ucs4, utf16, bitbuf;
  unsigned char base64[256], utf8[6];
  unsigned long srcPtr = 0;
  QCString dst;
  QCString src = name.ascii();

  /* initialize modified base64 decoding table */
  memset(base64, UNDEFINED, sizeof(base64));
  for (i = 0; i < sizeof(base64chars); ++i) {
    base64[(int)base64chars[i]] = i;
  }

  /* loop until end of string */
  while (srcPtr < src.length ()) {
    c = src[srcPtr++];
    /* deal with literal characters and &- */
    if (c != '&' || src[srcPtr] == '-') {
      /* encode literally */
      dst += c;
      /* skip over the '-' if this is an &- sequence */
      if (c == '&')
        srcPtr++;
    } else {
      /* convert modified UTF-7 -> UTF-16 -> UCS-4 -> UTF-8 -> HEX */
      bitbuf = 0;
      bitcount = 0;
      ucs4 = 0;
      while ((c = base64[(unsigned char) src[srcPtr]]) != UNDEFINED) {
        ++srcPtr;
        bitbuf = (bitbuf << 6) | c;
        bitcount += 6;
        /* enough bits for a UTF-16 character? */
        if (bitcount >= 16) {
          bitcount -= 16;
          utf16 = (bitcount ? bitbuf >> bitcount : bitbuf) & 0xffff;
          /* convert UTF16 to UCS4 */
          if (utf16 >= UTF16HIGHSTART && utf16 <= UTF16HIGHEND) {
            ucs4 = (utf16 - UTF16HIGHSTART) << UTF16SHIFT;
            continue;
          } else if (utf16 >= UTF16LOSTART && utf16 <= UTF16LOEND) {
            ucs4 += utf16 - UTF16LOSTART + UTF16BASE;
          } else {
            ucs4 = utf16;
          }
          /* convert UTF-16 range of UCS4 to UTF-8 */
          if (ucs4 <= 0x7fUL) {
            utf8[0] = ucs4;
            i = 1;
          } else if (ucs4 <= 0x7ffUL) {
            utf8[0] = 0xc0 | (ucs4 >> 6);
            utf8[1] = 0x80 | (ucs4 & 0x3f);
            i = 2;
          } else if (ucs4 <= 0xffffUL) {
            utf8[0] = 0xe0 | (ucs4 >> 12);
            utf8[1] = 0x80 | ((ucs4 >> 6) & 0x3f);
            utf8[2] = 0x80 | (ucs4 & 0x3f);
            i = 3;
          } else {
            utf8[0] = 0xf0 | (ucs4 >> 18);
            utf8[1] = 0x80 | ((ucs4 >> 12) & 0x3f);
            utf8[2] = 0x80 | ((ucs4 >> 6) & 0x3f);
            utf8[3] = 0x80 | (ucs4 & 0x3f);
            i = 4;
          }
          /* copy it */
          for (c = 0; c < i; ++c) {
            dst += utf8[c];
          }
        }
      }
      /* skip over trailing '-' in modified UTF-7 encoding */
      if (src[srcPtr] == '-')
        ++srcPtr;
    }
  }

  return QString::fromUtf8( dst.data() );
}

MailWrapper::MailWrapper( Settings *s )
    : QObject()
{
    settings = s;
}

QString MailWrapper::mailsmtpError( int errnum )
{
    switch ( errnum ) {
        case MAILSMTP_NO_ERROR:
            return tr( "No error" );
        case MAILSMTP_ERROR_UNEXPECTED_CODE:
            return tr( "Unexpected error code" );
        case MAILSMTP_ERROR_SERVICE_NOT_AVAILABLE:
            return tr( "Service not available" );
        case MAILSMTP_ERROR_STREAM:
            return tr( "Stream error" );
        case MAILSMTP_ERROR_HOSTNAME:
            return tr( "gethostname() failed" );
        case MAILSMTP_ERROR_NOT_IMPLEMENTED:
            return tr( "Not implemented" );
        case MAILSMTP_ERROR_ACTION_NOT_TAKEN:
            return tr( "Error, action not taken" );
        case MAILSMTP_ERROR_EXCEED_STORAGE_ALLOCATION:
            return tr( "Data exceeds storage allocation" );
        case MAILSMTP_ERROR_IN_PROCESSING:
            return tr( "Error in processing" );
  //      case MAILSMTP_ERROR_INSUFFISANT_SYSTEM_STORAGE:
    //        return tr( "Insufficient system storage" );
        case MAILSMTP_ERROR_MAILBOX_UNAVAILABLE:
            return tr( "Mailbox unavailable" );
        case MAILSMTP_ERROR_MAILBOX_NAME_NOT_ALLOWED:
            return tr( "Mailbox name not allowed" );
        case MAILSMTP_ERROR_BAD_SEQUENCE_OF_COMMAND:
            return tr( "Bad command sequence" );
        case MAILSMTP_ERROR_USER_NOT_LOCAL:
            return tr( "User not local" );
        case MAILSMTP_ERROR_TRANSACTION_FAILED:
            return tr( "Transaction failed" );
        case MAILSMTP_ERROR_MEMORY:
            return tr( "Memory error" );
        case MAILSMTP_ERROR_CONNECTION_REFUSED:
            return tr( "Connection refused" );
        default:
            return tr( "Unknown error code" );
    }
}

mailimf_mailbox *MailWrapper::newMailbox(const QString&name, const QString&mail )
{
    return mailimf_mailbox_new( strdup( name.latin1() ),
                                strdup( mail.latin1() ) );
}

mailimf_address_list *MailWrapper::parseAddresses(const QString&addr )
{
    mailimf_address_list *addresses;

    if ( addr.isEmpty() ) return NULL;

    addresses = mailimf_address_list_new_empty();

    QStringList list = QStringList::split( ',', addr );
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); it++ ) {
        char *str = strdup( (*it).latin1() );
        int err = mailimf_address_list_add_parse( addresses, str );
        if ( err != MAILIMF_NO_ERROR ) {
            qDebug( "Error parsing" );
            qDebug( *it );
            free( str );
        } else {
            qDebug( "Parse success! :)" );
        }
    }

    return addresses;
}

mailimf_fields *MailWrapper::createImfFields( Mail *mail )
{
    mailimf_fields *fields;
    mailimf_field *xmailer;
    mailimf_mailbox *sender, *fromBox;
    mailimf_mailbox_list *from;
    mailimf_address_list *to, *cc, *bcc, *reply;
    char *subject = strdup( mail->getSubject().latin1() );
    int err;

    sender = newMailbox( mail->getName(), mail->getMail() );
    if ( sender == NULL ) goto err_free;

    fromBox = newMailbox( mail->getName(), mail->getMail() );
    if ( fromBox == NULL ) goto err_free_sender;

    from = mailimf_mailbox_list_new_empty();
    if ( from == NULL ) goto err_free_fromBox;

    err = mailimf_mailbox_list_add( from, fromBox );
    if ( err != MAILIMF_NO_ERROR ) goto err_free_from;

    to = parseAddresses( mail->getTo() );
    if ( to == NULL ) goto err_free_from;

    cc = parseAddresses( mail->getCC() );
    bcc = parseAddresses( mail->getBCC() );
    reply = parseAddresses( mail->getReply() );

    fields = mailimf_fields_new_with_data( from, sender, reply, to, cc, bcc,
                                           NULL, NULL, subject );
    if ( fields == NULL ) goto err_free_reply;

    xmailer = mailimf_field_new_custom( strdup( "User-Agent" ),
                                        strdup( USER_AGENT ) );
    if ( xmailer == NULL ) goto err_free_fields;

    err = mailimf_fields_add( fields, xmailer );
    if ( err != MAILIMF_NO_ERROR ) goto err_free_xmailer;

    return fields;      // Success :)

err_free_xmailer:
    mailimf_field_free( xmailer );
err_free_fields:
    mailimf_fields_free( fields );
err_free_reply:
    mailimf_address_list_free( reply );
    mailimf_address_list_free( bcc );
    mailimf_address_list_free( cc );
    mailimf_address_list_free( to );
err_free_from:
    mailimf_mailbox_list_free( from );
err_free_fromBox:
    mailimf_mailbox_free( fromBox );
err_free_sender:
    mailimf_mailbox_free( sender );
err_free:
    free( subject );
    qDebug( "createImfFields - error" );

    return NULL;        // Error :(
}

mailmime *MailWrapper::buildTxtPart( QString str )
{
    mailmime *txtPart;
    mailmime_fields *fields;
    mailmime_content *content;
    mailmime_parameter *param;
    char *txt = strdup( str.latin1() );
    int err;

    param = mailmime_parameter_new( strdup( "charset" ),
                                    strdup( "iso-8859-1" ) );
    if ( param == NULL ) goto err_free;

    content = mailmime_content_new_with_str( "text/plain" );
    if ( content == NULL ) goto err_free_param;

    err = clist_append( content->ct_parameters, param );
    if ( err != MAILIMF_NO_ERROR ) goto err_free_content;

    fields = mailmime_fields_new_encoding( MAILMIME_MECHANISM_8BIT );
    if ( fields == NULL ) goto err_free_content;

    txtPart = mailmime_new_empty( content, fields );
    if ( txtPart == NULL ) goto err_free_fields;

    err = mailmime_set_body_text( txtPart, txt, strlen( txt ) );
    if ( err != MAILIMF_NO_ERROR ) goto err_free_txtPart;

    return txtPart;     // Success :)

err_free_txtPart:
    mailmime_free( txtPart );
err_free_fields:
    mailmime_fields_free( fields );
err_free_content:
    mailmime_content_free( content );
err_free_param:
    mailmime_parameter_free( param );
err_free:
    free( txt );
    qDebug( "buildTxtPart - error" );

    return NULL;        // Error :(
}

mailmime *MailWrapper::buildFilePart( QString filename, QString mimetype )
{
    mailmime * filePart;
    mailmime_fields * fields;
    mailmime_content * content;
    mailmime_parameter * param = NULL;
    int err;

    int pos = filename.findRev( '/' );
    QString tmp = filename.right( filename.length() - ( pos + 1 ) );
    char *name = strdup( tmp.latin1() );        // just filename
    char *file = strdup( filename.latin1() );   // full name with path
    char *mime = strdup( mimetype.latin1() );   // mimetype -e.g. text/plain

    fields = mailmime_fields_new_filename(
        MAILMIME_DISPOSITION_TYPE_ATTACHMENT, name,
        MAILMIME_MECHANISM_BASE64 );
    if ( fields == NULL ) goto err_free;

    content = mailmime_content_new_with_str( mime );
    if ( content == NULL ) goto err_free_fields;

    if ( mimetype.compare( "text/plain" ) ==  0 ) {
        param = mailmime_parameter_new( strdup( "charset" ),
                                        strdup( "iso-8859-1" ) );
        if ( param == NULL ) goto err_free_content;

        err = clist_append( content->ct_parameters, param );
        if ( err != MAILIMF_NO_ERROR ) goto err_free_param;
    }

    filePart = mailmime_new_empty( content, fields );
    if ( filePart == NULL ) goto err_free_param;

    err = mailmime_set_body_file( filePart, file );
    if ( err != MAILIMF_NO_ERROR ) goto err_free_filePart;

    return filePart;        // Success :)

err_free_filePart:
    mailmime_free( filePart );
err_free_param:
    if ( param != NULL ) mailmime_parameter_free( param );
err_free_content:
    mailmime_content_free( content );
err_free_fields:
    mailmime_fields_free( fields );
err_free:
    free( name );
    free( mime );
    free( file );
    qDebug( "buildFilePart - error" );

    return NULL;            // Error :(
}

void MailWrapper::addFileParts( mailmime *message, QList<Attachment> files )
{
    Attachment *it;
    for ( it = files.first(); it; it = files.next() ) {
        qDebug( "Adding file" );
        mailmime *filePart;
        int err;

        filePart = buildFilePart( it->getFileName(), it->getMimeType() );
        if ( filePart == NULL ) goto err_free;

        err = mailmime_smart_add_part( message, filePart );
        if ( err != MAILIMF_NO_ERROR ) goto err_free_filePart;

        continue;           // Success :)

    err_free_filePart:
        mailmime_free( filePart );
    err_free:
        qDebug( "addFileParts: error adding file:" );
        qDebug( it->getFileName() );
    }
}

mailmime *MailWrapper::createMimeMail( Mail *mail )
{
    mailmime *message, *txtPart;
    mailimf_fields *fields;
    int err;

    fields = createImfFields( mail );
    if ( fields == NULL ) goto err_free;

    message = mailmime_new_message_data( NULL );
    if ( message == NULL ) goto err_free_fields;

    mailmime_set_imf_fields( message, fields );

    txtPart = buildTxtPart( mail->getMessage() );
    if ( txtPart == NULL ) goto err_free_message;

    err = mailmime_smart_add_part( message, txtPart );
    if ( err != MAILIMF_NO_ERROR ) goto err_free_txtPart;

    addFileParts( message, mail->getAttachments() );

    return message;         // Success :)

err_free_txtPart:
    mailmime_free( txtPart );
err_free_message:
    mailmime_free( message );
err_free_fields:
    mailimf_fields_free( fields );
err_free:
    qDebug( "createMimeMail: error" );

    return NULL;            // Error :(
}

mailimf_field *MailWrapper::getField( mailimf_fields *fields, int type )
{
    mailimf_field *field;
    clistiter *it;

    it = clist_begin( fields->fld_list );
    while ( it ) {
        field = (mailimf_field *) it->data;
        if ( field->fld_type == type ) {
            return field;
        }
        it = it->next;
    }

    return NULL;
}

static void addRcpts( clist *list, mailimf_address_list *addr_list )
{
    clistiter *it, *it2;

    for ( it = clist_begin( addr_list->ad_list ); it; it = it->next ) {
        mailimf_address *addr;
        addr = (mailimf_address *) it->data;

        if ( addr->ad_type == MAILIMF_ADDRESS_MAILBOX ) {
    	    esmtp_address_list_add( list, addr->ad_data.ad_mailbox->mb_addr_spec, 0, NULL );
        } else if ( addr->ad_type == MAILIMF_ADDRESS_GROUP ) {
            clist *l = addr->ad_data.ad_group->grp_mb_list->mb_list;
    	    for ( it2 = clist_begin( l ); it2; it2 = it2->next ) {
                mailimf_mailbox *mbox;
    	        mbox = (mailimf_mailbox *) it2->data;
	            esmtp_address_list_add( list, mbox->mb_addr_spec, 0, NULL );
	        }
	    }
    }
}

clist *MailWrapper::createRcptList( mailimf_fields *fields )
{
    clist *rcptList;
    mailimf_field *field;

    rcptList = esmtp_address_list_new();

    field = getField( fields, MAILIMF_FIELD_TO );
    if ( field && (field->fld_type == MAILIMF_FIELD_TO)
         && field->fld_data.fld_to->to_addr_list ) {
        addRcpts( rcptList, field->fld_data.fld_to->to_addr_list );
    }

    field = getField( fields, MAILIMF_FIELD_CC );
    if ( field && (field->fld_type == MAILIMF_FIELD_CC)
         && field->fld_data.fld_cc->cc_addr_list ) {
        addRcpts( rcptList, field->fld_data.fld_cc->cc_addr_list );
    }

    field = getField( fields, MAILIMF_FIELD_BCC );
    if ( field && (field->fld_type == MAILIMF_FIELD_BCC)
         && field->fld_data.fld_bcc->bcc_addr_list ) {
        addRcpts( rcptList, field->fld_data.fld_bcc->bcc_addr_list );
    }

    return rcptList;
}

char *MailWrapper::getFrom( mailmime *mail )
{
    char *from = NULL;

    mailimf_field *ffrom;
    ffrom = getField( mail->mm_data.mm_message.mm_fields, MAILIMF_FIELD_FROM );
    if ( ffrom && (ffrom->fld_type == MAILIMF_FIELD_FROM)
         && ffrom->fld_data.fld_from->frm_mb_list && ffrom->fld_data.fld_from->frm_mb_list->mb_list ) {
        clist *cl = ffrom->fld_data.fld_from->frm_mb_list->mb_list;
        clistiter *it;
        for ( it = clist_begin( cl ); it; it = it->next ) {
            mailimf_mailbox *mb = (mailimf_mailbox *) it->data;
            from = strdup( mb->mb_addr_spec );
        }
    }

    return from;
}

SMTPaccount *MailWrapper::getAccount( QString from )
{
    SMTPaccount *smtp;

    QList<Account> list = settings->getAccounts();
    Account *it;
    for ( it = list.first(); it; it = list.next() ) {
        if ( it->getType().compare( "SMTP" ) == 0 ) {
            smtp = static_cast<SMTPaccount *>(it);
            if ( smtp->getMail().compare( from ) == 0 ) {
                qDebug( "SMTPaccount found for" );
                qDebug( from );
                return smtp;
            }
        }
    }

    return NULL;
}

QString MailWrapper::getTmpFile() {
    int num = 0;
    QString unique;

    QDir dir( "/tmp" );
    QStringList::Iterator it;

    QStringList list = dir.entryList( "opiemail-tmp-*" );
    do {
        unique.setNum( num++ );
    } while ( list.contains( "opiemail-tmp-" + unique ) > 0 );

    return "/tmp/opiemail-tmp-" + unique;
}

void MailWrapper::writeToFile( QString file, mailmime *mail )
{
    FILE *f;
    int err, col = 0;

    f = fopen( file.latin1(), "w" );
    if ( f == NULL ) {
        qDebug( "writeToFile: error opening file" );
        return;
    }

    err = mailmime_write( f, &col, mail );
    if ( err != MAILIMF_NO_ERROR ) {
        fclose( f );
        qDebug( "writeToFile: error writing mailmime" );
        return;
    }

    fclose( f );
}

void MailWrapper::readFromFile( QString file, char **data, size_t *size )
{
    char *buf;
    struct stat st;
    int fd, count = 0, total = 0;

    fd = open( file.latin1(), O_RDONLY, 0 );
    if ( fd == -1 ) return;

    if ( fstat( fd, &st ) != 0 ) goto err_close;
    if ( !st.st_size )  goto err_close;

    buf = (char *) malloc( st.st_size );
    if ( !buf ) goto err_close;

    while ( ( total < st.st_size ) && ( count >= 0 ) ) {
        count = read( fd, buf + total, st.st_size - total );
        total += count;
    }
    if ( count < 0 ) goto err_free;

    *data = buf;
    *size = st.st_size;

    close( fd );

    return;             // Success :)

err_free:
    free( buf );
err_close:
    close( fd );
}

void progress( size_t current, size_t maximum )
{
    qDebug( "Current: %i of %i", current, maximum );
}

void MailWrapper::smtpSend( mailmime *mail )
{
    mailsmtp *session;
    clist *rcpts;
    char *from, *data, *server, *user = NULL, *pass = NULL;
    size_t size;
    int err;
    bool ssl;
    uint16_t port;


    from = getFrom( mail );
    SMTPaccount *smtp = getAccount( from );
    if ( smtp == NULL ) {
        free(from);
        return;
    }
    server = strdup( smtp->getServer().latin1() );
    ssl = smtp->getSSL();
    port = smtp->getPort().toUInt();
    rcpts = createRcptList( mail->mm_data.mm_message.mm_fields );

    QString file = getTmpFile();
    writeToFile( file, mail );
    readFromFile( file, &data, &size );
    QFile f( file );
    f.remove();

    session = mailsmtp_new( 20, &progress );
    if ( session == NULL ) goto free_mem;

    qDebug( "Servername %s at port %i", server, port );
    if ( ssl ) {
        qDebug( "SSL session" );
        err = mailsmtp_ssl_connect( session, server, port );
    } else {
        qDebug( "No SSL session" );
        err = mailsmtp_socket_connect( session, server, port );
    }
    if ( err != MAILSMTP_NO_ERROR ) goto free_mem_session;

    err = mailsmtp_init( session );
    if ( err != MAILSMTP_NO_ERROR ) goto free_con_session;

    qDebug( "INIT OK" );

    if ( smtp->getLogin() ) {
        if ( smtp->getUser().isEmpty() || smtp->getPassword().isEmpty() ) {
            // get'em
            LoginDialog login( smtp->getUser(), smtp->getPassword(), NULL, 0, true );
            login.show();
            if ( QDialog::Accepted == login.exec() ) {
                // ok
                user = strdup( login.getUser().latin1() );
                pass = strdup( login.getPassword().latin1() );
            } else {
                goto free_con_session;
            }
        } else {
            user = strdup( smtp->getUser().latin1() );
            pass = strdup( smtp->getPassword().latin1() );
        }
        qDebug( "session->auth: %i", session->auth);
        err = mailsmtp_auth( session, user, pass );
        if ( err == MAILSMTP_NO_ERROR ) qDebug("auth ok");
        qDebug( "Done auth!" );
    }

    err = mailsmtp_send( session, from, rcpts, data, size );
    if ( err != MAILSMTP_NO_ERROR ) goto free_con_session;

    qDebug( "Mail sent." );

free_con_session:
    mailsmtp_quit( session );
free_mem_session:
    mailsmtp_free( session );
free_mem:
    smtp_address_list_free( rcpts );
    free( data );
    free( server );
    if ( smtp->getLogin() ) {
        free( user );
        free( pass );
    }
    free( from );
}

void MailWrapper::sendMail( Mail mail )
{
    mailmime *mimeMail;

    mimeMail = createMimeMail( &mail );
    if ( mimeMail == NULL ) {
        qDebug( "sendMail: error creating mime mail" );
    } else {
        smtpSend( mimeMail );
        mailmime_free( mimeMail );
    }
}

Mail::Mail()
    :name(""), mail(""), to(""), cc(""), bcc(""), reply(""), subject(""), message("")
{
}
