#ifndef VIEWMAIL_H
#define VIEWMAIL_H

#include <qlistview.h>
#include <qmap.h>

#include "viewmailbase.h"
//#include "imapresponse.h"
//#include "mailtable.h"

class AttachItemStore 
{
public:
	void setMimeType(QString mimeType) { _mimeType = mimeType; }
	QString mimeType() { return _mimeType; }
	void setFileName(QString fileName) { _fileName = fileName; }
	QString fileName() { return _fileName; }
	void setDescription(QString description) { _description = description; }
	QString description() { return _description; }
	void setPartNum(QString partNum) { _partNum = partNum; }
	QString partNum() { return _partNum; }

private:
	QString _mimeType, _fileName, _description, _partNum;

};

class AttachItem : public QListViewItem
{
public:
	AttachItem(QListView *parent, AttachItemStore &attachment);
	AttachItem(QListViewItem *parent, AttachItemStore &attachment);

	AttachItemStore attachItemStore() { return _attachItemStore; }

private:
	AttachItemStore _attachItemStore;

};

class ViewMail : public ViewMailBase
{
	Q_OBJECT

public:
	ViewMail(  QWidget *parent = 0, const char *name = 0, WFlags fl = Qt::WType_Modal);
	~ViewMail();

	void hide();
	void exec();
    static QString appName() { return QString::fromLatin1("mail"); }
        void setMailInfo( const QString & from, const QString & to, const QString & subject, const QString & cc, const QString & bcc, const QString & bodytext );

protected:
//	void fillList(IMAPResponseBODYSTRUCTURE &structure);
	QString deHtml(const QString &string);

protected slots:
	void slotReply();
	void slotForward();
        void setText();

//	void slotIMAPUid(IMAPResponse &response);

private:
	bool _inLoop;
//	IMAPResponseFETCH _mail;
//	IMAPHandler *_handler;
	QString _mailHtml;
	bool _gotBody;
        QMap <int,QString>  m_mail;

};

#endif
