/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "settings.h"

#include <qpe/global.h>
#include <qpe/fontmanager.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/qpedialog.h>
#include <qpe/qpeapplication.h>
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif

#include <qlabel.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qtabwidget.h>
#include <qslider.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdatastream.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qdir.h>
#if QT_VERSION >= 300
#include <qstylefactory.h>
#endif

#include <stdlib.h>


LanguageSettings::LanguageSettings( QWidget* parent, const char* name, WFlags fl )
		: LanguageSettingsBase( parent, name, TRUE, fl )
{
	if ( FontManager::hasUnicodeFont() )
		languages->setFont(FontManager::unicodeFont(FontManager::Proportional));


	QString tfn = QPEApplication::qpeDir() + "i18n/";
	QDir langDir = tfn;
	QStringList list = langDir.entryList("*", QDir::Dirs );

	QStringList::Iterator it;

	for ( it = list.begin(); it != list.end(); ++it ) {
		QString name = (*it);
		QFileInfo desktopFile( tfn + "/" + name + "/.directory" );
		if ( desktopFile.exists() ) {
			langAvail.append(name);
			Config conf( desktopFile.filePath(), Config::File );
			QString langName = conf.readEntry( "Name" );
			QString ownName = conf.readEntryDirect( "Name[" + name + "]" );
			if ( ownName.isEmpty() )
				ownName = conf.readEntryDirect( "Name" );
			if ( !ownName.isEmpty() && ownName != langName )
				langName = langName + " [" + ownName + "]";
			languages->insertItem( langName );

		}
	}
	if ( langAvail. find ( "en" ) == -1 ) {
		langAvail. prepend ( "" ); // no tr
		languages-> insertItem ( QString ( "English [%1] (%2)" /* no tr (!) */ ). arg ( tr ( "English" )). arg ( tr( "default" )), 0 );
	}

	dl = new QPEDialogListener(this);
	reset();
}

LanguageSettings::~LanguageSettings()
{}

void LanguageSettings::accept()
{
    Config c( "qpe" );
    c.setGroup( "Startup" );
    if ( ( c.readNumEntry( "FirstUse", 42 ) == 0 ) &&
       ( QMessageBox::warning( this, tr("Language"), tr("<qt>Attention, all windows will be closed by changing the language\n"
                                                 "without saving the Data.<br><br>Go on?</qt>"), 1, 2) ) 
	 == QMessageBox::Cancel )
                                                 return;
    applyLanguage();
    QDialog::accept();
}

void LanguageSettings::applyLanguage()
{
	setLanguage ( langAvail. at ( languages-> currentItem ( )));
}


void LanguageSettings::reject()
{
	reset();
	QDialog::reject();
}

void LanguageSettings::reset()
{
	QString l = getenv("LANG");
	Config config("locale");
	config.setGroup("Language");
	l = config.readEntry( "Language", l );
	actualLanguage = l;
	if (l.isEmpty())
		l = "en";

	int n = langAvail.find( l );
	languages->setCurrentItem( n );
}

QString LanguageSettings::actualLanguage;

void LanguageSettings::setLanguage(const QString& lang)
{
	if ( lang != actualLanguage ) {
		Config config("locale");
		config.setGroup( "Language" );
		if ( lang. isEmpty ( ))
			config. removeEntry ( "Language" );
		else
			config.writeEntry( "Language", lang );
		config.write();

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)

		QCopEnvelope e("QPE/System", "language(QString)");
		e << lang;
#endif

	}
}

void LanguageSettings::done(int r)
{
	QDialog::done(r);
	close();
}
