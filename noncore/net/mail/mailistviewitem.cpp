#include "mailistviewitem.h"
#include <libmailwrapper/abstractmail.h>

/* OPIE */
#include <opie2/oresource.h>
#include <qpe/timestring.h>

/* QT */
#include <qtextstream.h>

MailListViewItem::MailListViewItem(QListView * parent, MailListViewItem * item )
        :QListViewItem(parent,item),mail_data()
{
}

void MailListViewItem::showEntry()
{
    if ( mail_data->getFlags().testBit( FLAG_ANSWERED ) == true) {
        setPixmap( 0, Opie::Core::OResource::loadPixmap( "mail/kmmsgreplied", Opie::Core::OResource::SmallIcon ) );
    } else if ( mail_data->getFlags().testBit( FLAG_SEEN ) == true )  {
        /* I think it looks nicer if there are not such a lot of icons but only on mails
           replied or new - Alwin*/
        //setPixmap( 0, Opie::Core::OResource::loadPixmap( "mail/kmmsgunseen", Opie::Core::OResource::SmallIcon ) );
    } else  {
        setPixmap( 0, Opie::Core::OResource::loadPixmap( "mail/kmmsgnew", Opie::Core::OResource::SmallIcon ) );
    }
    double s = mail_data->Msgsize();
    int w;
    w=0;

    while (s>1024) {
        s/=1024;
        ++w;
        if (w>=2) break;
    }

    QString q="";
    QString fsize="";
    switch(w) {
    case 1:
        q="k";
        break;
    case 2:
        q="M";
        break;
    default:
        break;
    }

    {
        QTextOStream o(&fsize);
        if (w>0) o.precision(2); else o.precision(0);
        o.setf(QTextStream::fixed);
        o << s << " " << q << "Byte";
    }
    setText(1,mail_data->getSubject());
    setText(2,mail_data->getFrom());
    setText(3,fsize);
    setText(4,mail_data->getStringDate());
}

QString MailListViewItem::key(int col,bool) const
{
    QString temp;
    if (col == 4) {
        temp.sprintf( "%08d",QDateTime().secsTo(mail_data->getDate()));
        return temp;
    }
    if (col == 3) {
        temp.sprintf( "%020d",mail_data->Msgsize());
        return temp;
    }
    return text(col);
}

void MailListViewItem::storeData(const RecMailP&data)
{
    mail_data = data;
}

const RecMailP& MailListViewItem::data()const
{
    return mail_data;
}

MAILLIB::ATYPE MailListViewItem::wrapperType()
{
    if (!mail_data->Wrapper()) return MAILLIB::A_UNDEFINED;
    return mail_data->Wrapper()->getType();
}
