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
#include "xmlsource.h"
#include "../xmlencodeattr.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qdict.h>
#include <qtextstream.h>

/* STD */
#include <stdlib.h>

DBXml::DBXml(DBStore *d) 
{
    dstore = d;
} 

QString DBXml::type()
{
    return "xml";
}

bool DBXml::openSource(QIODevice *inDev)
{
    bool ok;

    DBXmlHandler h(dstore); 

    QTextStream tsIn(inDev);
    QXmlInputSource source(tsIn); 
    QXmlSimpleReader reader; 
    reader.setContentHandler(&h); 
    reader.setErrorHandler(&h); 
    ok = reader.parse(source); 

    return ok;
} 

bool DBXml::saveSource(QIODevice *outDev)
{
    int i;
    DataElem *elem;
    KeyList *k;

    QTextStream outstream(outDev);

    outstream << "<database name=\"" << dstore->getName() << "\">" << endl;
    outstream << "<header>" << endl;
    
    k = dstore->getKeys();
    KeyListIterator it(*k);
    while(it.current()) {
        if (!it.current()->delFlag()) {
            outstream << "<key name=\"KEYID" << it.currentKey() << "\" ";
            outstream << "type=\"" 
                      << TVVariant::typeToName(it.current()->type()) 
                      << "\">";
            outstream << encodeAttr(it.current()->name()) << "</key>" << endl;
        }
        ++it;
    }

    outstream << "</header>" << endl;

    dstore->first();

    do {
        elem = dstore->getCurrentData();
	if (!elem)
	    break;
        outstream << "<record>" << endl;
        it.toFirst();
        while (it.current()) {
            i = it.currentKey();
            if (elem->hasValidValue(i)) {
                outstream << "<KEYID" << i << ">";
                if (dstore->getKeyType(i) == TVVariant::Date) {
                    // dates in files are different from displayed dates
                    QDate date = elem->getField(i).toDate();
                    outstream << date.day() << "/" 
                              << date.month() << "/"
                              << date.year();
                } else {
                    outstream << encodeAttr(elem->toQString(i));
                }
                outstream << "</KEYID" << i << ">" << endl;
            }
            ++it;
        }
        outstream << "</record>" << endl;
    } while(dstore->next());
    
    outstream << "</database>" << endl;
    return TRUE;
}

DBXml::~DBXml() {}

/*!
    \class DBXmlHandler
    \brief An Xml parser for flat tables.

    An xml parser for parsing the files used by the table viewer application.

    The format of the xml files can be found at the front of the file
    dataparser.h
*/

/*! 
    Constructs a new DBXmlHandler, and sets that the table should be
    constructed in the DBStore pointed to by ds.
*/
DBXmlHandler::DBXmlHandler(DBStore *ds)
{
	data_store = ds;
    current_keyrep = 0;
}

/*!
    Destroys the DBXmlHandler
*/
DBXmlHandler::~DBXmlHandler()
{
}

QString DBXmlHandler::errorProtocol()
{
    owarn << "Error reading file" << oendl; 
	return errorProt;
}

bool DBXmlHandler::startDocument()
{
	errorProt = "";
	state = StateInit;
	return TRUE;
}

bool DBXmlHandler::startElement(const QString&, const QString&, 
						const QString& qName, const QXmlAttributes& atts)
{
	if (state == StateInit && qName == "database") {
		// First thing it expects is a <document name="..."> tag
		state = StateDocument;
		data_store->setName(atts.value("name"));
		return TRUE;
	}
    if (state == StateDocument && qName == "header") {
        state = StateHeader;
        if (current_keyrep) delete current_keyrep;
        current_keyrep = new KeyList();
        return TRUE;
    }
    if (state == StateHeader && qName == "key") {
        /* Ok, adding a new key to our KeyList TODO */
        state = StateKey;
        last_key_type = TVVariant::String;
        key = atts.value("name");
        if (key.isEmpty()) {
            owarn << "empty key name" << oendl; 
            return FALSE;
        }
        if(!atts.value("type").isEmpty())
            last_key_type = TVVariant::nameToType(atts.value("type"));
        return TRUE;
    }
	if (state == StateDocument && qName == "record") {
		state = StateRecord;
        current_data = new DataElem(data_store);
		// Now expecting a <record> tag
		return TRUE;
	}
	if (state == StateRecord) {
        state = StateField;
        /* the qName is the name of a key */
        if (!keyIndexList[qName]) {
            /* invalid key, we failed */
            owarn << "Invalid key in record" << oendl; 
            return FALSE;
        }
        keyIndex = *keyIndexList[qName];
		return TRUE;
	}
    owarn << "Unable to determine tag type" << oendl; 
	return FALSE;
}

bool DBXmlHandler::endElement(const QString&, const QString&, 
								const QString& qName)
{
	switch(state) {
	case StateField:
		// TODO checks 'could' be done of the popped value
        state = StateRecord;
		break;
	case StateKey:
		// TODO checks 'could' be done of the popped value
        state = StateHeader;
		break;
    case StateHeader:
        data_store->setKeys(current_keyrep);
        state = StateDocument;
        break;
	case StateRecord:
        data_store->addItem(current_data);
		state = StateDocument;
		break;
	case StateDocument:
		// we are done... 
		break;
	default:
		// should only get a 'endElement' from one of the above states.
        owarn << "Invalid end tag" << oendl; 
		return FALSE;
		break;
	}
	return TRUE;
}

bool DBXmlHandler::characters(const QString& ch)
{
	// this is where the 'between tag' stuff happens.
	// e.g. the stuff between tags.
	QString ch_simplified = ch.simplifyWhiteSpace();

	if (ch_simplified.isEmpty())
		return TRUE;

	if (state == StateKey) {
        int *tmp_val = new int;
        /* We just grabbed the display name of a key */
        *tmp_val = current_keyrep->addKey(ch_simplified, last_key_type);
        keyIndexList.insert(key, tmp_val);
		return TRUE;
	}
	if (state == StateField) {
        /* Ok, need to add data here */
        current_data->setField(keyIndex, ch_simplified);
		return TRUE;
	}
	
    owarn << "Junk characters found... ignored" << oendl; 
    return TRUE;
}

QString DBXmlHandler::errorString()
{
	return "the document is not in the expected file format";
}

bool DBXmlHandler::warning(const QXmlParseException& exception)
{
    errorProt += QString("warning parsing error: %1 in line %2, column %3\n" )
	.arg(exception.message())
	.arg(exception.lineNumber())
	.arg(exception.columnNumber());

    owarn << errorProt << oendl; 
    return QXmlDefaultHandler::fatalError(exception);
}

bool DBXmlHandler::error(const QXmlParseException& exception)
{
    errorProt += QString("error parsing error: %1 in line %2, column %3\n" )
	.arg(exception.message())
	.arg(exception.lineNumber())
	.arg(exception.columnNumber());

    owarn << errorProt << oendl; 
    return QXmlDefaultHandler::fatalError(exception);
}

bool DBXmlHandler::fatalError(const QXmlParseException& exception)
{
    errorProt += QString("fatal parsing error: %1 in line %2, column %3\n" )
	.arg(exception.message())
	.arg(exception.lineNumber())
	.arg(exception.columnNumber());

    owarn << errorProt << oendl; 
    return QXmlDefaultHandler::fatalError(exception);
}
