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

#include "drawpad.h"

#include "drawpadcanvas.h"
#include "ellipsetool.h"
#include "erasetool.h"
#include "exportdialog.h"
#include "filledellipsetool.h"
#include "filledrectangletool.h"
#include "filltool.h"
#include "importdialog.h"
#include "linetool.h"
#include "newpagedialog.h"
#include "page.h"
#include "pageinformationdialog.h"
#include "pointtool.h"
#include "rectangletool.h"
#include "texttool.h"
#include "thumbnailview.h"

#include <opie/colordialog.h>
#include <opie/colorpopupmenu.h>

#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/global.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>

#include <qaction.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

DrawPad::DrawPad(QWidget* parent, const char* name)
    : QMainWindow(parent, name, WStyle_ContextHelp)
{
    // init members

    m_pDrawPadCanvas = new DrawPadCanvas(this, this);

    connect(m_pDrawPadCanvas, SIGNAL(pagesChanged()), this, SLOT(updateView()));

    setCentralWidget(m_pDrawPadCanvas);

    // init menu

    setToolBarsMovable(false);

    QPEToolBar* menuToolBar = new QPEToolBar(this);
    QPEMenuBar* menuBar = new QPEMenuBar(menuToolBar);

    QPopupMenu* toolsPopupMenu = new QPopupMenu(menuBar);

    QAction* deleteAllAction = new QAction(tr("Delete All"), QString::null, 0, this);
    connect(deleteAllAction, SIGNAL(activated()), this, SLOT(deleteAll()));
    deleteAllAction->addTo(toolsPopupMenu);

    toolsPopupMenu->insertSeparator();

    QAction* importPageAction = new QAction(tr("Import"), tr("Import..."), 0, this);
    connect(importPageAction, SIGNAL(activated()), this, SLOT(importPage()));
    importPageAction->addTo(toolsPopupMenu);

    QAction* exportPageAction = new QAction(tr("Export"), tr("Export..."), 0, this);
    connect(exportPageAction, SIGNAL(activated()), this, SLOT(exportPage()));
    exportPageAction->addTo(toolsPopupMenu);

    toolsPopupMenu->insertSeparator();

    QAction* thumbnailViewAction = new QAction(tr("Thumbnail View"), tr("Thumbnail View..."), 0, this);
    connect(thumbnailViewAction, SIGNAL(activated()), this, SLOT(thumbnailView()));
    thumbnailViewAction->addTo(toolsPopupMenu);

    QAction* pageInformationAction = new QAction(tr("Page Information"), tr("Page Information..."), 0, this);
    connect(pageInformationAction, SIGNAL(activated()), this, SLOT(pageInformation()));
    pageInformationAction->addTo(toolsPopupMenu);

    toolsPopupMenu->insertSeparator();

    m_pAntiAliasingAction = new QAction(tr("Anti-Aliasing"), QString::null, 0, this);
    m_pAntiAliasingAction->setToggleAction(true);
    m_pAntiAliasingAction->addTo(toolsPopupMenu);

    menuBar->insertItem(tr("Tools"), toolsPopupMenu);

    // init page toolbar

    QPEToolBar* pageToolBar = new QPEToolBar(this);

    QAction* newPageAction = new QAction(tr("New Page"), Resource::loadPixmap("new"), QString::null, 0, this);
    connect(newPageAction, SIGNAL(activated()), this, SLOT(newPage()));
    newPageAction->addTo(pageToolBar);
    newPageAction->setWhatsThis( tr( "Click here to add a new sheet." ) );

    QAction* clearPageAction = new QAction(tr("Clear Page"), Resource::loadPixmap("drawpad/clear"), QString::null, 0, this);
    connect(clearPageAction, SIGNAL(activated()), this, SLOT(clearPage()));
    clearPageAction->addTo(pageToolBar);
    clearPageAction->setWhatsThis( tr( "Click here to erase the current sheet." ) );

    QAction* deletePageAction = new QAction(tr("Delete Page"), Resource::loadPixmap("trash"), QString::null, 0, this);
    connect(deletePageAction, SIGNAL(activated()), this, SLOT(deletePage()));
    deletePageAction->addTo(pageToolBar);
    deletePageAction->setWhatsThis( tr( "Click here to remove the current sheet." ) );

    QPEToolBar* emptyToolBar = new QPEToolBar(this);
    emptyToolBar->setHorizontalStretchable(true);

    // init navigation toolbar

    QPEToolBar* navigationToolBar = new QPEToolBar(this);

    m_pUndoAction = new QAction(tr("Undo"), Resource::loadIconSet("undo"), QString::null, 0, this);
    connect(m_pUndoAction, SIGNAL(activated()), m_pDrawPadCanvas, SLOT(undo()));
    m_pUndoAction->addTo(navigationToolBar);
    m_pUndoAction->setWhatsThis( tr( "Click here to undo the last action." ) );

    m_pRedoAction = new QAction(tr("Redo"), Resource::loadIconSet("redo"), QString::null, 0, this);
    connect(m_pRedoAction, SIGNAL(activated()), m_pDrawPadCanvas, SLOT(redo()));
    m_pRedoAction->addTo(navigationToolBar);
    m_pRedoAction->setWhatsThis( tr( "Click here to re-perform the last action." ) );

    m_pFirstPageAction = new QAction(tr("First Page"), Resource::loadIconSet("fastback"), QString::null, 0, this);
    connect(m_pFirstPageAction, SIGNAL(activated()), m_pDrawPadCanvas, SLOT(goFirstPage()));
    m_pFirstPageAction->addTo(navigationToolBar);
    m_pFirstPageAction->setWhatsThis( tr( "Click here to view the first page." ) );

    m_pPreviousPageAction = new QAction(tr("Previous Page"), Resource::loadIconSet("back"), QString::null, 0, this);
    connect(m_pPreviousPageAction, SIGNAL(activated()), m_pDrawPadCanvas, SLOT(goPreviousPage()));
    m_pPreviousPageAction->addTo(navigationToolBar);
    m_pPreviousPageAction->setWhatsThis( tr( "Click here to view the previous page." ) );

    m_pNextPageAction = new QAction(tr("Next Page"), Resource::loadIconSet("forward"), QString::null, 0, this);
    connect(m_pNextPageAction, SIGNAL(activated()), m_pDrawPadCanvas, SLOT(goNextPage()));
    m_pNextPageAction->addTo(navigationToolBar);
    m_pNextPageAction->setWhatsThis( tr( "Click here to view the next page." ) );

    m_pLastPageAction = new QAction(tr("Last Page"), Resource::loadIconSet("fastforward"), QString::null, 0, this);
    connect(m_pLastPageAction, SIGNAL(activated()), m_pDrawPadCanvas, SLOT(goLastPage()));
    m_pLastPageAction->addTo(navigationToolBar);
    m_pLastPageAction->setWhatsThis( tr( "Click here to view the last page." ) );

    // init draw mode toolbar

    QPEToolBar* drawModeToolBar = new QPEToolBar(this);

    m_pLineToolButton = new QToolButton(drawModeToolBar);
    m_pLineToolButton->setToggleButton(true);
    QWhatsThis::add( m_pLineToolButton, tr( "Click here to select one of the available tools to draw lines." ) );


    QPopupMenu* linePopupMenu = new QPopupMenu(m_pLineToolButton);

    m_pPointToolAction = new QAction(tr("Draw Point"), Resource::loadPixmap("drawpad/point"), "", 0, this);
    connect(m_pPointToolAction, SIGNAL(activated()), this, SLOT(setPointTool()));
    m_pPointToolAction->addTo(linePopupMenu);

    m_pLineToolAction = new QAction(tr("Draw Line"), Resource::loadPixmap("drawpad/line"), "", 0, this);
    connect(m_pLineToolAction, SIGNAL(activated()), this, SLOT(setLineTool()));
    m_pLineToolAction->addTo(linePopupMenu);

    m_pLineToolButton->setPopup(linePopupMenu);
    m_pLineToolButton->setPopupDelay(0);

    m_pRectangleToolButton = new QToolButton(drawModeToolBar);
    m_pRectangleToolButton->setToggleButton(true);
    QWhatsThis::add( m_pRectangleToolButton, tr( "Click here to select one of the available tools to draw rectangles." ) );

    QPopupMenu* rectanglePopupMenu = new QPopupMenu(m_pRectangleToolButton);

    m_pRectangleToolAction = new QAction(tr("Draw Rectangle"), Resource::loadPixmap("drawpad/rectangle"), "", 0, this);
    connect(m_pRectangleToolAction, SIGNAL(activated()), this, SLOT(setRectangleTool()));
    m_pRectangleToolAction->addTo(rectanglePopupMenu);

    m_pFilledRectangleToolAction = new QAction(tr("Draw Filled Rectangle"), Resource::loadPixmap("drawpad/filledrectangle"), "", 0, this);
    connect(m_pFilledRectangleToolAction, SIGNAL(activated()), this, SLOT(setFilledRectangleTool()));
    m_pFilledRectangleToolAction->addTo(rectanglePopupMenu);

    m_pRectangleToolButton->setPopup(rectanglePopupMenu);
    m_pRectangleToolButton->setPopupDelay(0);

    m_pEllipseToolButton = new QToolButton(drawModeToolBar);
    m_pEllipseToolButton->setToggleButton(true);
    QWhatsThis::add( m_pEllipseToolButton, tr( "Click here to select one of the available tools to draw ellipses." ) );

    QPopupMenu* ellipsePopupMenu = new QPopupMenu(m_pEllipseToolButton);

    m_pEllipseToolAction = new QAction(tr("Draw Ellipse"), Resource::loadPixmap("drawpad/ellipse"), "", 0, this);
    connect(m_pEllipseToolAction, SIGNAL(activated()), this, SLOT(setEllipseTool()));
    m_pEllipseToolAction->addTo(ellipsePopupMenu);

    m_pFilledEllipseToolAction = new QAction(tr("Draw Filled Ellipse"), Resource::loadPixmap("drawpad/filledellipse"), "", 0, this);
    connect(m_pFilledEllipseToolAction, SIGNAL(activated()), this, SLOT(setFilledEllipseTool()));
    m_pFilledEllipseToolAction->addTo(ellipsePopupMenu);

    m_pEllipseToolButton->setPopup(ellipsePopupMenu);
    m_pEllipseToolButton->setPopupDelay(0);

    m_pTextToolAction = new QAction(tr("Insert Text"), Resource::loadPixmap("drawpad/text"), QString::null, 0, this);
    m_pTextToolAction->setToggleAction(true);
    connect(m_pTextToolAction, SIGNAL(activated()), this, SLOT(setTextTool()));
    m_pTextToolAction->addTo(drawModeToolBar);
    m_pTextToolAction->setWhatsThis( tr( "Click here to select the text drawing tool." ) );

    m_pFillToolAction = new QAction(tr("Fill Region"), Resource::loadPixmap("drawpad/fill"), QString::null, 0, this);
    m_pFillToolAction->setToggleAction(true);
    connect(m_pFillToolAction, SIGNAL(activated()), this, SLOT(setFillTool()));
    m_pFillToolAction->addTo(drawModeToolBar);
    m_pFillToolAction->setWhatsThis( tr( "Click here to select the fill tool." ) );

    m_pEraseToolAction = new QAction(tr("Erase Point"), Resource::loadPixmap("drawpad/erase"), QString::null, 0, this);
    m_pEraseToolAction->setToggleAction(true);
    connect(m_pEraseToolAction, SIGNAL(activated()), this, SLOT(setEraseTool()));
    m_pEraseToolAction->addTo(drawModeToolBar);
    m_pEraseToolAction->setWhatsThis( tr( "Click here to select the eraser tool." ) );

    m_pTool = 0;
    setRectangleTool();
    setEllipseTool();
    setPointTool();

    emptyToolBar = new QPEToolBar(this);
    emptyToolBar->setHorizontalStretchable(true);
    emptyToolBar->addSeparator();

    // init draw parameters toolbar

    QPEToolBar* drawParametersToolBar = new QPEToolBar(this);

    m_pPenWidthSpinBox = new QSpinBox(1, 9, 1, drawParametersToolBar);
    connect(m_pPenWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changePenWidth(int)));

    QToolTip::add(m_pPenWidthSpinBox, tr("Pen Width"));
    m_pPenWidthSpinBox->setValue(1);
    m_pPenWidthSpinBox->setFocusPolicy(QWidget::NoFocus);
    QWhatsThis::add( m_pPenWidthSpinBox, tr( "Click here to select the width of the drawing pen." ) );

    m_pPenColorToolButton = new QToolButton(drawParametersToolBar);
    m_pPenColorToolButton->setPixmap(Resource::loadPixmap("drawpad/pencolor"));
    QWhatsThis::add( m_pPenColorToolButton, tr( "Click here to select the color used when drawing." ) );

    OColorPopupMenu* penColorPopupMenu = new OColorPopupMenu(Qt::black, m_pPenColorToolButton);
    connect(penColorPopupMenu, SIGNAL(colorSelected(const QColor&)), this, SLOT(changePenColor(const QColor&)));

    QToolTip::add(m_pPenColorToolButton, tr("Pen Color"));
    m_pPenColorToolButton->setPopup(penColorPopupMenu);
    m_pPenColorToolButton->setPopupDelay(0);

    changePenColor(Qt::black);

    m_pBrushColorToolButton = new QToolButton(drawParametersToolBar);
    m_pBrushColorToolButton->setPixmap(Resource::loadPixmap("drawpad/brushcolor"));
    QWhatsThis::add( m_pBrushColorToolButton, tr( "Click here to select the color used when filling in areas." ) );

    OColorPopupMenu* brushColorPopupMenu = new OColorPopupMenu(Qt::white, m_pBrushColorToolButton);
    connect(brushColorPopupMenu, SIGNAL(colorSelected(const QColor&)), this, SLOT(changeBrushColor(const QColor&)));

    QToolTip::add(m_pBrushColorToolButton, tr("Fill Color"));
    m_pBrushColorToolButton->setPopup(brushColorPopupMenu);
    m_pBrushColorToolButton->setPopupDelay(0);

    changeBrushColor(Qt::white);

    // init pages

    QFile file(Global::applicationFileName("drawpad", "drawpad.xml"));

    if (file.open(IO_ReadOnly)) {
        m_pDrawPadCanvas->load(&file);
        file.close();
    } else {
        m_pDrawPadCanvas->initialPage();
    }

    loadConfig();
}

DrawPad::~DrawPad()
{
    saveConfig();

    QFile file(Global::applicationFileName("drawpad", "drawpad.xml"));

    if (file.open(IO_WriteOnly)) {
        m_pDrawPadCanvas->save(&file);
        file.close();
    }
}

bool DrawPad::antiAliasing()
{
    return (m_pAntiAliasingAction->isOn());
}

void DrawPad::newPage()
{
    QRect rect = m_pDrawPadCanvas->contentsRect();

    NewPageDialog newPageDialog(rect.width(), rect.height(), m_pen.color(), m_brush.color(), this);

    if (newPageDialog.exec() == QDialog::Accepted) {
        m_pDrawPadCanvas->newPage(newPageDialog.selectedTitle(), newPageDialog.selectedWidth(),
                                  newPageDialog.selectedHeight(), newPageDialog.selectedColor());
    }
}

void DrawPad::clearPage()
{
    QMessageBox messageBox(tr("Clear Page"), tr("Do you want to clear\nthe current page?"),
                           QMessageBox::Information, QMessageBox::Yes,
                           QMessageBox::No | QMessageBox::Escape | QMessageBox::Default,
                           QMessageBox::NoButton, this);

    messageBox.setButtonText(QMessageBox::Yes, tr("Yes"));
    messageBox.setButtonText(QMessageBox::No, tr("No"));

    if (messageBox.exec() == QMessageBox::Yes) {
        m_pDrawPadCanvas->clearPage();
    }
}

void DrawPad::deletePage()
{
    QMessageBox messageBox(tr("Delete Page"), tr("Do you want to delete\nthe current page?"),
                           QMessageBox::Information, QMessageBox::Yes,
                           QMessageBox::No | QMessageBox::Escape | QMessageBox::Default,
                           QMessageBox::NoButton, this);

    messageBox.setButtonText(QMessageBox::Yes, tr("Yes"));
    messageBox.setButtonText(QMessageBox::No, tr("No"));

    if (messageBox.exec() == QMessageBox::Yes) {
        m_pDrawPadCanvas->deletePage();
    }
}

void DrawPad::setPointTool()
{
    if (m_pTool) {
        delete m_pTool;
    }

    m_pTool = new PointTool(this, m_pDrawPadCanvas);

    m_pLineToolButton->setIconSet(m_pPointToolAction->iconSet());
    QToolTip::add(m_pLineToolButton, m_pPointToolAction->text());

    disconnect(m_pLineToolButton, SIGNAL(clicked()), 0, 0);
    connect(m_pLineToolButton, SIGNAL(clicked()), m_pPointToolAction, SIGNAL(activated()));

    m_pLineToolButton->setOn(true);
    m_pRectangleToolButton->setOn(false);
    m_pEllipseToolButton->setOn(false);
    m_pTextToolAction->setOn(false);
    m_pFillToolAction->setOn(false);
    m_pEraseToolAction->setOn(false);
}

void DrawPad::setLineTool()
{
    if (m_pTool) {
        delete m_pTool;
    }

    m_pTool = new LineTool(this, m_pDrawPadCanvas);

    m_pLineToolButton->setIconSet(m_pLineToolAction->iconSet());
    QToolTip::add(m_pLineToolButton, m_pLineToolAction->text());

    disconnect(m_pLineToolButton, SIGNAL(clicked()), 0, 0);
    connect(m_pLineToolButton, SIGNAL(clicked()), m_pLineToolAction, SIGNAL(activated()));

    m_pLineToolButton->setOn(true);
    m_pRectangleToolButton->setOn(false);
    m_pEllipseToolButton->setOn(false);
    m_pTextToolAction->setOn(false);
    m_pFillToolAction->setOn(false);
    m_pEraseToolAction->setOn(false);
}

void DrawPad::setRectangleTool()
{
    if (m_pTool) {
        delete m_pTool;
    }

    m_pTool = new RectangleTool(this, m_pDrawPadCanvas);

    m_pRectangleToolButton->setIconSet(m_pRectangleToolAction->iconSet());
    QToolTip::add(m_pRectangleToolButton, m_pRectangleToolAction->text());

    disconnect(m_pRectangleToolButton, SIGNAL(clicked()), 0, 0);
    connect(m_pRectangleToolButton, SIGNAL(clicked()), m_pRectangleToolAction, SIGNAL(activated()));

    m_pLineToolButton->setOn(false);
    m_pRectangleToolButton->setOn(true);
    m_pEllipseToolButton->setOn(false);
    m_pTextToolAction->setOn(false);
    m_pFillToolAction->setOn(false);
    m_pEraseToolAction->setOn(false);
}

void DrawPad::setFilledRectangleTool()
{
    if (m_pTool) {
        delete m_pTool;
    }

    m_pTool = new FilledRectangleTool(this, m_pDrawPadCanvas);

    m_pRectangleToolButton->setIconSet(m_pFilledRectangleToolAction->iconSet());
    QToolTip::add(m_pRectangleToolButton, m_pFilledRectangleToolAction->text());

    disconnect(m_pRectangleToolButton, SIGNAL(clicked()), 0, 0);
    connect(m_pRectangleToolButton, SIGNAL(clicked()), m_pFilledRectangleToolAction, SIGNAL(activated()));

    m_pLineToolButton->setOn(false);
    m_pRectangleToolButton->setOn(true);
    m_pEllipseToolButton->setOn(false);
    m_pTextToolAction->setOn(false);
    m_pFillToolAction->setOn(false);
    m_pEraseToolAction->setOn(false);
}

void DrawPad::setEllipseTool()
{
    if (m_pTool) {
        delete m_pTool;
    }

    m_pTool = new EllipseTool(this, m_pDrawPadCanvas);

    m_pEllipseToolButton->setIconSet(m_pEllipseToolAction->iconSet());
    QToolTip::add(m_pEllipseToolButton, m_pEllipseToolAction->text());

    disconnect(m_pEllipseToolButton, SIGNAL(clicked()), 0, 0);
    connect(m_pEllipseToolButton, SIGNAL(clicked()), m_pEllipseToolAction, SIGNAL(activated()));

    m_pLineToolButton->setOn(false);
    m_pRectangleToolButton->setOn(false);
    m_pEllipseToolButton->setOn(true);
    m_pTextToolAction->setOn(false);
    m_pFillToolAction->setOn(false);
    m_pEraseToolAction->setOn(false);
}

void DrawPad::setFilledEllipseTool()
{
    if (m_pTool) {
        delete m_pTool;
    }

    m_pTool = new FilledEllipseTool(this, m_pDrawPadCanvas);

    m_pEllipseToolButton->setIconSet(m_pFilledEllipseToolAction->iconSet());
    QToolTip::add(m_pEllipseToolButton, m_pFilledEllipseToolAction->text());

    disconnect(m_pEllipseToolButton, SIGNAL(clicked()), 0, 0);
    connect(m_pEllipseToolButton, SIGNAL(clicked()), m_pFilledEllipseToolAction, SIGNAL(activated()));

    m_pLineToolButton->setOn(false);
    m_pRectangleToolButton->setOn(false);
    m_pEllipseToolButton->setOn(true);
    m_pTextToolAction->setOn(false);
    m_pFillToolAction->setOn(false);
    m_pEraseToolAction->setOn(false);
}

void DrawPad::setTextTool()
{
    if (m_pTool) {
        delete m_pTool;
    }

    m_pTool = new TextTool(this, m_pDrawPadCanvas);

    m_pLineToolButton->setOn(false);
    m_pRectangleToolButton->setOn(false);
    m_pEllipseToolButton->setOn(false);
    m_pTextToolAction->setOn(true);
    m_pFillToolAction->setOn(false);
    m_pEraseToolAction->setOn(false);
}

void DrawPad::setFillTool()
{
    if (m_pTool) {
        delete m_pTool;
    }

    m_pTool = new FillTool(this, m_pDrawPadCanvas);

    m_pLineToolButton->setOn(false);
    m_pRectangleToolButton->setOn(false);
    m_pEllipseToolButton->setOn(false);
    m_pTextToolAction->setOn(false);
    m_pFillToolAction->setOn(true);
    m_pEraseToolAction->setOn(false);
}

void DrawPad::setEraseTool()
{
    if (m_pTool) {
        delete m_pTool;
    }

    m_pTool = new EraseTool(this, m_pDrawPadCanvas);

    m_pLineToolButton->setOn(false);
    m_pRectangleToolButton->setOn(false);
    m_pEllipseToolButton->setOn(false);
    m_pTextToolAction->setOn(false);
    m_pFillToolAction->setOn(false);
    m_pEraseToolAction->setOn(true);
}

void DrawPad::changePenWidth(int value)
{
    m_pen.setWidth(value);
}

void DrawPad::changePenColor(const QColor& color)
{
    m_pen.setColor(color);

    QPainter painter;
    painter.begin(m_pPenColorToolButton->pixmap());
    painter.fillRect(QRect(0, 12, 14, 2), m_pen.color());
    painter.end();

    m_pPenColorToolButton->popup()->hide();
}

void DrawPad::changeBrushColor(const QColor& color)
{
    m_brush = QBrush(color);

    QPainter painter;
    painter.begin(m_pBrushColorToolButton->pixmap());
    painter.fillRect(QRect(0, 12, 14, 2), m_brush.color());
    painter.end();

    m_pBrushColorToolButton->popup()->hide();
}

void DrawPad::updateView()
{
    uint pagePosition = m_pDrawPadCanvas->pagePosition();
    uint pageCount = m_pDrawPadCanvas->pageCount();

    setCaption( tr( "DrawPad - Page %1/%2" ).arg( pagePosition ).arg( pageCount ) );

    m_pUndoAction->setEnabled(m_pDrawPadCanvas->currentPage()->undoEnabled());
    m_pRedoAction->setEnabled(m_pDrawPadCanvas->currentPage()->redoEnabled());

    m_pFirstPageAction->setEnabled(m_pDrawPadCanvas->goPreviousPageEnabled());
    m_pPreviousPageAction->setEnabled(m_pDrawPadCanvas->goPreviousPageEnabled());
    m_pNextPageAction->setEnabled(m_pDrawPadCanvas->goNextPageEnabled());
    m_pLastPageAction->setEnabled(m_pDrawPadCanvas->goNextPageEnabled());
}

void DrawPad::deleteAll()
{
    QMessageBox messageBox(tr("Delete All"), tr("Do you want to delete\nall the pages?"),
                           QMessageBox::Information, QMessageBox::Yes,
                           QMessageBox::No | QMessageBox::Escape | QMessageBox::Default,
                           QMessageBox::NoButton, this);

    messageBox.setButtonText(QMessageBox::Yes, tr("Yes"));
    messageBox.setButtonText(QMessageBox::No, tr("No"));

    if (messageBox.exec() == QMessageBox::Yes) {
        m_pDrawPadCanvas->deleteAll();
    }
}

void DrawPad::importPage()
{
    ImportDialog importDialog(this);

    importDialog.showMaximized();

    if (importDialog.exec() == QDialog::Accepted) {
        const DocLnk* docLnk = importDialog.selected();

        if (docLnk) {
            m_pDrawPadCanvas->importPage(docLnk->file());
            delete docLnk;
        }
    }
}

void DrawPad::exportPage()
{
    ExportDialog exportDialog(m_pDrawPadCanvas->pagePosition(), m_pDrawPadCanvas->pageCount(), this);

    exportDialog.showMaximized();

    if (exportDialog.exec() == QDialog::Accepted) {
        m_pDrawPadCanvas->exportPage(exportDialog.selectedFromPage(), exportDialog.selectedToPage(),
                                     exportDialog.selectedName(), exportDialog.selectedFormat());
    }
}

void DrawPad::thumbnailView()
{
    ThumbnailView thumbnailView(this, m_pDrawPadCanvas, this);

    thumbnailView.showMaximized();
    thumbnailView.exec();
}

void DrawPad::pageInformation()
{
    PageInformationDialog pageInformationDialog(m_pDrawPadCanvas->currentPage());

    if (pageInformationDialog.exec() == QDialog::Accepted) {
        m_pDrawPadCanvas->currentPage()->setTitle(pageInformationDialog.selectedTitle());
    }
}

void DrawPad::loadConfig()
{
    Config config("drawpad");
    config.setGroup("General");

    m_pAntiAliasingAction->setOn(config.readBoolEntry("AntiAliasing", false));
    m_pPenWidthSpinBox->setValue(config.readNumEntry("PenWidth", 1));
    changePenColor(QColor(config.readEntry("PenColor", Qt::black.name())));
    changeBrushColor(QColor(config.readEntry("BrushColor", Qt::white.name())));
    m_pDrawPadCanvas->selectPage(config.readNumEntry("PagePosition", 1));
}

void DrawPad::saveConfig()
{
    Config config("drawpad");
    config.setGroup("General");

    config.writeEntry("PagePosition", (int)m_pDrawPadCanvas->pagePosition());
    config.writeEntry("AntiAliasing", antiAliasing());
    config.writeEntry("PenWidth", (int)m_pen.width());
    config.writeEntry("PenColor", m_pen.color().name());
    config.writeEntry("BrushColor", m_brush.color().name());
}
