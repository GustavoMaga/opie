/***************************************************************************
 *                                                                         *
 *   DrawPad - a drawing program for Opie Environment                      *
 *                                                                         *
 *   (C) 2002 by S. Prud'homme <prudhomme@laposte.net>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "drawpadcanvas.h"

#include "drawpad.h"
#include "page.h"
#include "tool.h"

#include <qpe/applnk.h>
#include <qpe/filemanager.h>
#include <qpe/mimetype.h>

#include <qbuffer.h>
#include <qimage.h>
#include <qtextcodec.h>
#include <qxml.h>

class DrawPadCanvasXmlHandler: public QXmlDefaultHandler
{
public:
    DrawPadCanvasXmlHandler();
    ~DrawPadCanvasXmlHandler();

    QList<Page> pages();

    bool startElement(const QString& namespaceURI, const QString& localName,
                      const QString& qName, const QXmlAttributes& atts);
    bool endElement(const QString& namespaceURI, const QString& localName,
                    const QString& qName);
    bool characters(const QString& ch);

private:
    enum State {
        Unknown,
        InTitle,
        InDate,
        InData
    };

    State m_state;
    QString m_title;
    QDateTime m_date;
    ulong m_dataLenght;
    QString m_dataFormat;
    QList<Page> m_pages;
};

DrawPadCanvasXmlHandler::DrawPadCanvasXmlHandler()
{
    m_state = Unknown;
}

DrawPadCanvasXmlHandler::~DrawPadCanvasXmlHandler()
{
}

QList<Page> DrawPadCanvasXmlHandler::pages()
{
    return m_pages;
}

bool DrawPadCanvasXmlHandler::startElement(const QString& namespaceURI, const QString& localName,
                                           const QString& qName, const QXmlAttributes& atts)
{
    Q_CONST_UNUSED(namespaceURI)
    Q_CONST_UNUSED(localName)

    if (qName == "image") {
        m_title = QString();
        m_date = QDateTime::currentDateTime();
    } else if (qName == "title") {
        m_state = InTitle;
    } else if (qName == "date") {
        m_state = InDate;
    } else if (qName == "data") {
        m_state = InData;
        m_dataLenght = atts.value("length").toULong();
        m_dataFormat = atts.value("format");
    }

    return true;
}

bool DrawPadCanvasXmlHandler::endElement(const QString& namespaceURI, const QString& localName,
                                         const QString& qName)
{
    Q_CONST_UNUSED(namespaceURI)
    Q_CONST_UNUSED(localName)

    if (qName == "title") {
        m_state = Unknown;
    } else if (qName == "date") {
        m_state = Unknown;
    } else if (qName == "data") {
        m_state = Unknown;
    }

    return true;
}

bool DrawPadCanvasXmlHandler::characters(const QString& ch)
{
    if (m_state == InTitle) {
        m_title = ch;
    } else if (m_state == InDate) {
        m_date = QDateTime(QDate(1970, 1, 1)).addSecs(ch.toInt());
    } else if (m_state == InData) {
        QByteArray byteArray(ch.length() / 2);

        for (int i = 0; i < (int)ch.length() / 2; i++) {
            char h = ch[2 * i].latin1();
            char l = ch[2 * i  + 1].latin1();
            uchar r = 0;

            if (h <= '9') {
                r += h - '0';
            } else {
                r += h - 'a' + 10;
            }

            r = r << 4;

            if (l <= '9') {
                r += l - '0';
            } else {
                r += l - 'a' + 10;
            }

            byteArray[i] = r;
        }

        QImage image;
        image.loadFromData((const uchar*)byteArray.data(), m_dataLenght, m_dataFormat);

        Page* page = new Page(m_title, image.width(), image.height());
        page->setLastModified(m_date);
        page->pixmap()->convertFromImage(image);
        m_pages.append(page);
    }

    return true;
}

DrawPadCanvas::DrawPadCanvas(DrawPad* drawPad, QWidget* parent, const char* name)
    : QScrollView(parent, name)
{
    m_pDrawPad = drawPad;
    m_pages.setAutoDelete(true);

    viewport()->setBackgroundMode(QWidget::NoBackground);
}

DrawPadCanvas::~DrawPadCanvas()
{
}

void DrawPadCanvas::load(QIODevice* ioDevice)
{
    QTextStream textStream(ioDevice);
    textStream.setCodec(QTextCodec::codecForName("UTF-8"));

    QXmlInputSource xmlInputSource(textStream);
    QXmlSimpleReader xmlSimpleReader;
    DrawPadCanvasXmlHandler drawPadCanvasXmlHandler;

    xmlSimpleReader.setContentHandler(&drawPadCanvasXmlHandler);
    xmlSimpleReader.parse(xmlInputSource);

    m_pages = drawPadCanvasXmlHandler.pages();

    if (m_pages.isEmpty()) {
        m_pages.append(new Page("",
		clipper()->width()+(verticalScrollBar()->isVisible()?verticalScrollBar()->width():0),
	        clipper()->height()+(horizontalScrollBar()->isVisible()?horizontalScrollBar()->height():0)));
        m_pages.current()->pixmap()->fill(Qt::white);
    }

    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::initialPage()
{
    m_pages.append(new Page("",
	clipper()->width()+(verticalScrollBar()->isVisible()?verticalScrollBar()->width():0),
	clipper()->height()+(horizontalScrollBar()->isVisible()?horizontalScrollBar()->height():0)));
	//236, 232)); no more fixed sizes

    m_pages.current()->pixmap()->fill(Qt::white);

    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::save(QIODevice* ioDevice)
{
    QTextStream textStream(ioDevice);
    textStream.setCodec(QTextCodec::codecForName("UTF-8"));

    textStream << "<drawpad>" << endl;
    textStream << "    <images>" << endl;

    QListIterator<Page> bufferIterator(m_pages);

    for (bufferIterator.toFirst(); bufferIterator.current() != 0; ++bufferIterator) {
        textStream << "        <image>" << endl;
        textStream << "            <title>" << bufferIterator.current()->title() << "</title>" << endl;

        int intDate = QDateTime(QDate(1970, 1, 1)).secsTo(bufferIterator.current()->lastModified());
        textStream << "            <date>" << intDate << "</date>" << endl;

        QImage image = bufferIterator.current()->pixmap()->convertToImage();
        QByteArray byteArray;
        QBuffer buffer(byteArray);
        QImageIO imageIO(&buffer, "PNG");

        buffer.open(IO_WriteOnly);
        imageIO.setImage(image);
        imageIO.write();
        buffer.close();

        textStream << "            <data length=\"" << byteArray.size() << "\" format=\"PNG\">";

        static const char hexchars[] = "0123456789abcdef";

        for (int i = 0; i < (int)byteArray.size(); i++ ) {
            uchar s = (uchar)byteArray[i];
            textStream << hexchars[s >> 4];
            textStream << hexchars[s & 0x0f];
        }

        textStream << "</data>" << endl;
        textStream << "        </image>" << endl;
    }

    textStream << "    </images>" << endl;
    textStream << "</drawpad>";
}

void DrawPadCanvas::importPage(const QString& fileName)
{
    Page* importedPage = new Page();

    importedPage->pixmap()->load(fileName);
    m_pages.insert(m_pages.at() + 1, importedPage);

    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::importPixmap( const QPixmap& pix ) {
    Page* importedPage = new Page();

    (*importedPage->pixmap()) = pix;
    m_pages.insert(m_pages.at()+1, importedPage );
    resizeContents(m_pages.current()->pixmap()->width(),
                   m_pages.current()->pixmap()->height() );
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::exportPage(uint fromPage, uint toPage, const QString& name,const QString& format)
{
    if (fromPage == toPage) {
        DocLnk docLnk;
        MimeType mimeType(format);

        docLnk.setName(name);
        docLnk.setType(mimeType.id());

        FileManager fileManager;
        QIODevice* ioDevice = fileManager.saveFile(docLnk);
        QImageIO imageIO(ioDevice, format);

        QImage image = m_pages.current()->pixmap()->convertToImage();
        imageIO.setImage(image);
        imageIO.write();
        delete ioDevice;
    } else {
        for (uint i = fromPage; i <= toPage; i++) {
            DocLnk docLnk;
            MimeType mimeType(format);

            docLnk.setName(name + QString::number(i));
            docLnk.setType(mimeType.id());

            FileManager fileManager;
            QIODevice* ioDevice = fileManager.saveFile(docLnk);
            QImageIO imageIO(ioDevice, format);

            QImage image = m_pages.at(i - 1)->pixmap()->convertToImage();
            imageIO.setImage(image);
            imageIO.write();
            delete ioDevice;
        }
    }
}

Page* DrawPadCanvas::currentPage()
{
    return m_pages.current();
}

QList<Page> DrawPadCanvas::pages()
{
    return m_pages;
}

uint DrawPadCanvas::pagePosition()
{
    return (m_pages.at() + 1);
}

uint DrawPadCanvas::pageCount()
{
    return m_pages.count();
}

void DrawPadCanvas::selectPage(Page* page)
{
    m_pages.findRef(page);

    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::backupPage()
{
    m_pages.current()->backup();

    emit pagesChanged();
}

void DrawPadCanvas::selectPage(uint pagePosition)
{
    m_pages.at(pagePosition - 1);

    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::deleteAll()
{
    m_pages.clear();

    m_pages.append(new Page("",
	clipper()->width()+(verticalScrollBar()->isVisible()?verticalScrollBar()->width():0),
        clipper()->height()+(horizontalScrollBar()->isVisible()?horizontalScrollBar()->height():0)));

    m_pages.current()->pixmap()->fill(Qt::white);

    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::newPage(QString title, uint width, uint height, const QColor& color)
{
    m_pages.insert(m_pages.at() + 1, new Page(title, width, height));
    m_pages.current()->pixmap()->fill(color);

    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::clearPage()
{
    m_pages.current()->pixmap()->fill(Qt::white);

    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();
}

void DrawPadCanvas::deletePage()
{
    m_pages.remove(m_pages.current());

    if (m_pages.isEmpty()) {
        m_pages.append(new Page("",
		clipper()->width()+(verticalScrollBar()->isVisible()?verticalScrollBar()->width():0),
	        clipper()->height()+(horizontalScrollBar()->isVisible()?horizontalScrollBar()->height():0)));

        m_pages.current()->pixmap()->fill(Qt::white);
    }

    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::movePageUp()
{
    int index = m_pages.at();
    Page* page = m_pages.take();
    m_pages.insert(index - 1, page);

    emit pagesChanged();
}

void DrawPadCanvas::movePageDown()
{
    int index = m_pages.at();
    Page* page = m_pages.take();
    m_pages.insert(index + 1, page);

    emit pagesChanged();
}

bool DrawPadCanvas::goPreviousPageEnabled()
{
    return (m_pages.current() != m_pages.getFirst());
}

bool DrawPadCanvas::goNextPageEnabled()
{
    return (m_pages.current() != m_pages.getLast());
}

void DrawPadCanvas::undo()
{
    m_pages.current()->undo();

    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::redo()
{
    m_pages.current()->redo();

    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::goFirstPage()
{
    m_pages.first();

    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::goPreviousPage()
{
    m_pages.prev();

    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::goNextPage()
{
    m_pages.next();
    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::goLastPage()
{
    m_pages.last();

    resizeContents(m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    viewport()->update();

    emit pagesChanged();
}

void DrawPadCanvas::contentsMousePressEvent(QMouseEvent* e)
{
    m_pDrawPad->tool()->mousePressEvent(e);
}

void DrawPadCanvas::contentsMouseReleaseEvent(QMouseEvent* e)
{
    m_pDrawPad->tool()->mouseReleaseEvent(e);
}

void DrawPadCanvas::contentsMouseMoveEvent(QMouseEvent* e)
{
    m_pDrawPad->tool()->mouseMoveEvent(e);
}

void DrawPadCanvas::drawContents(QPainter* p, int cx, int cy, int cw, int ch)
{
    QRect clipRect(cx, cy, cw, ch);
    QRect pixmapRect(0, 0, m_pages.current()->pixmap()->width(), m_pages.current()->pixmap()->height());
    QRect drawRect = pixmapRect.intersect(clipRect);

    p->drawPixmap(drawRect.topLeft(), *(m_pages.current()->pixmap()), drawRect);

    if (drawRect.right() < clipRect.right()) {
        p->fillRect(drawRect.right() + 1, cy, cw - drawRect.width(), ch, colorGroup().dark());
    }

    if (drawRect.bottom() < clipRect.bottom()) {
        p->fillRect(cx, drawRect.bottom() + 1, cw, ch - drawRect.height(), colorGroup().dark());
    }
}
