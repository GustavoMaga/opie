/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Opie Sheet (formerly Sheet/Qt)
 * by Serdar Ozler <sozler@sitebest.com>
 */

#include "mainwindow.h"

#include <qpe/filemanager.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qtranslator.h>
#include <qradiobutton.h>
#include "cellformat.h"
#include "numberdlg.h"
#include "textdlg.h"
#include "sortdlg.h"
#include "finddlg.h"

#define DEFAULT_NUM_ROWS 300
#define DEFAULT_NUM_COLS (26*3)
#define DEFAULT_NUM_SHEETS 3

MainWindow::MainWindow(QWidget *parent, const char* n, WFlags fl)
           :QMainWindow(parent, n, fl)
{
  // initialize variables
  documentModified=FALSE;

  // construct objects
  currentDoc=0;
  fileSelector=new FileSelector("application/sheet-qt", this, QString::null);
  ExcelSelector=new FileSelector("application/excel",this,QString::null,FALSE);
  connect(fileSelector, SIGNAL(closeMe()), this, SLOT(selectorHide()));
  connect(fileSelector, SIGNAL(newSelected(const DocLnk &)), this, SLOT(selectorFileNew(const DocLnk &)));
  connect(fileSelector, SIGNAL(fileSelected(const DocLnk &)), this, SLOT(selectorFileOpen(const DocLnk &)));
  connect(ExcelSelector,SIGNAL(fileSelected(const DocLnk &)),this,SLOT(slotImportExcel(const DocLnk &)));
  connect(ExcelSelector,SIGNAL(closeMe()), this, SLOT(ExcelSelectorHide()));


  listSheets.setAutoDelete(TRUE);

  initActions();
  initMenu();
  initEditToolbar();
  initFunctionsToolbar();
  initStandardToolbar();
  initSheet();

  // set window title
  setCaption(tr("Opie Sheet"));

  // create sheets
  selectorFileNew(DocLnk());
}

MainWindow::~MainWindow()
{
  if (currentDoc) delete currentDoc;
}

void MainWindow::documentSave(DocLnk *lnkDoc)
{
  FileManager fm;
  QByteArray streamBuffer;
  QDataStream stream(streamBuffer, IO_WriteOnly);

  typeSheet *currentSheet=findSheet(sheet->getName());
  if (!currentSheet)
  {
    QMessageBox::critical(this, tr("Error"), tr("Inconsistency error!"));
    return;
  }
  sheet->copySheetData(&currentSheet->data);
  stream.writeRawBytes("SQT100", 6);
  stream << (Q_UINT32)listSheets.count();
  for (typeSheet *tempSheet=listSheets.first(); tempSheet; tempSheet=listSheets.next())
  {
    stream << tempSheet->name << (Q_UINT32)tempSheet->data.count();
    for (typeCellData *tempCell=tempSheet->data.first(); tempCell; tempCell=tempSheet->data.next())
      stream << (Q_UINT32)tempCell->col << (Q_UINT32)tempCell->row << tempCell->borders.right << tempCell->borders.bottom << tempCell->background << (Q_UINT32)tempCell->alignment << tempCell->fontColor << tempCell->font << tempCell->data;
  }

  lnkDoc->setType("application/sheet-qt");
  if (!fm.saveFile(*lnkDoc, streamBuffer))
  {
    QMessageBox::critical(this, tr("Error"), tr("File cannot be saved!"));
    return;
  }
  documentModified=FALSE;
}

void MainWindow::documentOpen(const DocLnk &lnkDoc)
{
  FileManager fm;
  QByteArray streamBuffer;
  if (!lnkDoc.isValid() || !fm.loadFile(lnkDoc, streamBuffer))
  {
    QMessageBox::critical(this, tr("Error"), tr("File cannot be opened!"));
    documentModified=FALSE;
    selectorFileNew(DocLnk());
    return;
  }
  QDataStream stream(streamBuffer, IO_ReadOnly);

  Q_UINT32 countSheet, countCell, i, j, row, col, alignment;
  typeSheet *newSheet;
  typeCellData *newCell;

  char fileFormat[7];
  stream.readRawBytes(fileFormat, 6);
  fileFormat[6]=0;
  if ((QString)fileFormat!="SQT100")
  {
    QMessageBox::critical(this, tr("Error"), tr("Invalid file format!"));
    documentModified=FALSE;
    selectorFileNew(DocLnk());
    return;
  }

  stream >> countSheet;
  for (i=0; i<countSheet; ++i)
  {
    newSheet=new typeSheet;
    newSheet->data.setAutoDelete(TRUE);
    stream >> newSheet->name >> countCell;
    comboSheets->insertItem(newSheet->name);

    for (j=0; j<countCell; ++j)
    {
      newCell=new typeCellData;
      stream >> col >> row >> newCell->borders.right >> newCell->borders.bottom >> newCell->background >> alignment >> newCell->fontColor >> newCell->font >> newCell->data;
      newCell->col=col;
      newCell->row=row;
      newCell->alignment=(Qt::AlignmentFlags)alignment;
      newSheet->data.append(newCell);
    }
    listSheets.append(newSheet);

    if (i==0)
    {
      sheet->setName(newSheet->name);
      sheet->setSheetData(&newSheet->data);
    }
  }
}

int MainWindow::saveCurrentFile(bool ask)
{
  if (ask)
  {
    int result=QMessageBox::information(this, tr("Save File"), tr("Do you want to save the current file?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
    if (result!=QMessageBox::Yes) return result;
  }

  if (!currentDoc->isValid())
  {
    TextDialog dialogText(this);
    if (dialogText.exec(tr("Save File"), tr("&File Name:"), tr("UnnamedFile"))!=QDialog::Accepted || dialogText.getValue().isEmpty()) return QMessageBox::Cancel;

    currentDoc->setName(dialogText.getValue());
    currentDoc->setFile(QString::null);
    currentDoc->setLinkFile(QString::null);
  }

  documentSave(currentDoc);
  return QMessageBox::Yes;
}

void MainWindow::selectorFileNew(const DocLnk &lnkDoc)
{
  selectorHide();

  if (documentModified && saveCurrentFile()==QMessageBox::Cancel) return;
  if (currentDoc) delete currentDoc;
  currentDoc = new DocLnk(lnkDoc);
  editData->clear();
  listSheets.clear();
  comboSheets->clear();

  typeSheet *newSheet=createNewSheet();
  newSheet->data.setAutoDelete(TRUE);
  sheet->setName(newSheet->name);
  sheet->setSheetData(&newSheet->data);
  for (int i=1; i<DEFAULT_NUM_SHEETS; ++i)
    createNewSheet();
  documentModified=FALSE;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
  if (documentModified && saveCurrentFile()==QMessageBox::Cancel) e->ignore();
  else e->accept();
}

void MainWindow::selectorFileOpen(const DocLnk &lnkDoc)
{
  selectorHide();

  if (documentModified && saveCurrentFile()==QMessageBox::Cancel) return;
  if (currentDoc) delete currentDoc;
  currentDoc = new DocLnk();
  listSheets.clear();
  comboSheets->clear();

  documentOpen(lnkDoc);
  documentModified=FALSE;
}

void MainWindow::selectorShow()
{
  sheet->hide();
  setCentralWidget(fileSelector);
  fileSelector->show();
  fileSelector->reread();
}

void MainWindow::selectorHide()
{
  fileSelector->hide();
  setCentralWidget(sheet);
  sheet->show();
}

void MainWindow::slotFileNew()
{
  selectorFileNew(DocLnk());
}

void MainWindow::slotFileOpen()
{
  selectorShow();
}

void MainWindow::slotImportExcelOpen()
{
  sheet->hide();
  setCentralWidget(ExcelSelector);
  ExcelSelector->show();
  ExcelSelector->reread();
}

void MainWindow::ExcelSelectorHide()
{
  ExcelSelector->hide();
  setCentralWidget(sheet);
  sheet->show();
}

void MainWindow::slotFileSave()
{
  saveCurrentFile(FALSE);
}

void MainWindow::setDocument(const QString &applnk_filename)
{
  selectorFileOpen(DocLnk(applnk_filename));
}

void MainWindow::initActions()
{
  fileNew=new QAction(tr("New File"), Resource::loadPixmap( "new" ), tr("&New"), 0, this);
  connect(fileNew, SIGNAL(activated()), this, SLOT(slotFileNew()));
  fileOpen=new QAction(tr("Open File"), Resource::loadPixmap( "fileopen" ), tr("&Open"), 0, this);
  connect(fileOpen, SIGNAL(activated()), this, SLOT(slotFileOpen()));
  fileSave=new QAction(tr("Save File"),Resource::loadPixmap( "save" ), tr("&Save"), 0, this);
  connect(fileSave, SIGNAL(activated()), this, SLOT(slotFileSave()));
  fileSaveAs=new QAction(tr("Save File As"), Resource::loadPixmap( "save" ), tr("Save &As"), 0, this);
  connect(fileSaveAs, SIGNAL(activated()), this, SLOT(slotFileSaveAs()));

  //fileQuit=new QAction(tr("Quit"), tr("&Quit"), 0, this);
  //connect(fileQuit, SIGNAL(activated()), this, SLOT(close()));
  fileExcelImport=new QAction(tr("Import Excel file"),Resource::loadPixmap( "opie-sheet/excel16" ),tr("Import E&xcel file"),0,this);
  connect(fileExcelImport, SIGNAL(activated()), this, SLOT(slotImportExcelOpen()));

  // helpGeneral=new QAction(tr("General Help"), QPixmap(help_general_xpm), tr("&General"), 0, this);
  //connect(helpGeneral, SIGNAL(activated()), this, SLOT(slotHelpGeneral()));
  //helpAbout=new QAction(tr("About Opie Sheet"), tr("&About"), 0, this);
  //connect(helpAbout, SIGNAL(activated()), this, SLOT(slotHelpAbout()));

  editAccept=new QAction(tr("Accept"),Resource::loadPixmap( "enter" ) , tr("&Accept"), 0, this);
  connect(editAccept, SIGNAL(activated()), this, SLOT(slotEditAccept()));
  editCancel=new QAction(tr("Cancel"), Resource::loadPixmap( "close" ), tr("&Cancel"), 0, this);
  connect(editCancel, SIGNAL(activated()), this, SLOT(slotEditCancel()));
  editCellSelect=new QAction(tr("Cell Selector"), Resource::loadPixmap( "opie-sheet/cell-select" ), tr("Cell &Selector"), 0, this);
  editCellSelect->setToggleAction(TRUE);
  connect(editCellSelect, SIGNAL(toggled(bool)), this, SLOT(slotCellSelect(bool)));
  editCut=new QAction(tr("Cut Cells"), tr("Cu&t"), 0, this);
  editCopy=new QAction(tr("Copy Cells"), tr("&Copy"), 0, this);
  editPaste=new QAction(tr("Paste Cells"), tr("&Paste"), 0, this);
  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));
  editPasteContents=new QAction(tr("Paste Contents"), tr("Paste Cont&ents"), 0, this);
  connect(editPasteContents, SIGNAL(activated()), this, SLOT(slotEditPasteContents()));
  editClear=new QAction(tr("Clear Cells"), tr("C&lear"), 0, this);

  insertCells=new QAction(tr("Insert Cells"), tr("C&ells"), 0, this);
  connect(insertCells, SIGNAL(activated()), this, SLOT(slotInsertCells()));
  insertRows=new QAction(tr("Insert Rows"), tr("&Rows"), 0, this);
  connect(insertRows, SIGNAL(activated()), this, SLOT(slotInsertRows()));
  insertCols=new QAction(tr("Insert Columns"), tr("&Columns"), 0, this);
  connect(insertCols, SIGNAL(activated()), this, SLOT(slotInsertCols()));
  insertSheets=new QAction(tr("Add Sheets"), tr("&Sheets"), 0, this);
  connect(insertSheets, SIGNAL(activated()), this, SLOT(slotInsertSheets()));

  formatCells=new QAction(tr("Cells"), tr("&Cells"), 0, this);
  connect(formatCells, SIGNAL(activated()), this, SLOT(slotFormatCells()));

  rowHeight=new QAction(tr("Row Height"), tr("H&eight"), 0, this);
  connect(rowHeight, SIGNAL(activated()), this, SLOT(slotRowHeight()));
  rowAdjust=new QAction(tr("Adjust Row"), tr("&Adjust"), 0, this);
  connect(rowAdjust, SIGNAL(activated()), this, SLOT(slotRowAdjust()));
  rowShow=new QAction(tr("Show Row"), tr("&Show"), 0, this);
  connect(rowShow, SIGNAL(activated()), this, SLOT(slotRowShow()));
  rowHide=new QAction(tr("Hide Row"), tr("&Hide"), 0, this);
  connect(rowHide, SIGNAL(activated()), this, SLOT(slotRowHide()));

  colWidth=new QAction(tr("Column Width"), tr("&Width"), 0, this);
  connect(colWidth, SIGNAL(activated()), this, SLOT(slotColumnWidth()));
  colAdjust=new QAction(tr("Adjust Column"), tr("&Adjust"), 0, this);
  connect(colAdjust, SIGNAL(activated()), this, SLOT(slotColumnAdjust()));
  colShow=new QAction(tr("Show Column"), tr("&Show"), 0, this);
  connect(colShow, SIGNAL(activated()), this, SLOT(slotColumnShow()));
  colHide=new QAction(tr("Hide Column"), tr("&Hide"), 0, this);
  connect(colHide, SIGNAL(activated()), this, SLOT(slotColumnHide()));

  sheetRename=new QAction(tr("Rename Sheet"), tr("&Rename"), 0, this);
  connect(sheetRename, SIGNAL(activated()), this, SLOT(slotSheetRename()));
  sheetRemove=new QAction(tr("Remove Sheet"), tr("R&emove"), 0, this);
  connect(sheetRemove, SIGNAL(activated()), this, SLOT(slotSheetRemove()));

  dataSort=new QAction(tr("Sort Data"), tr("&Sort"), 0, this);
  connect(dataSort, SIGNAL(activated()), this, SLOT(slotDataSort()));
  dataFindReplace=new QAction(tr("Find && Replace"), tr("&Find && Replace"), 0, this);
  connect(dataFindReplace, SIGNAL(activated()), this, SLOT(slotDataFindReplace()));

  funcEqual=new QAction(tr("Equal To"), Resource::loadPixmap( "opie-sheet/func-equal" ), tr("&Equal To"), 0, this);
  funcEqual->setToolTip("=");
  connect(funcEqual, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcPlus=new QAction(tr("Addition"), Resource::loadPixmap( "opie-sheet/func-plus" ), tr("&Addition"), 0, this);
  funcPlus->setToolTip("+");
  connect(funcPlus, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcMinus=new QAction(tr("Subtraction"), Resource::loadPixmap( "opie-sheet/func-minus" ), tr("&Subtraction"), 0, this);
  funcMinus->setToolTip("-");
  connect(funcMinus, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcCross=new QAction(tr("Multiplication"), Resource::loadPixmap ("opie-sheet/func-cross" ), tr("&Multiplication"), 0, this);
  funcCross->setToolTip("*");
  connect(funcCross, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcDivide=new QAction(tr("Division"), Resource::loadPixmap( "opie-sheet/func-divide" ), tr("&Division"), 0, this);
  funcDivide->setToolTip("/");
  connect(funcDivide, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcParanOpen=new QAction(tr("Open ParanthesistempCellData->row+row1, tempCellData->col+col1"), Resource::loadPixmap( "opie-sheet/func-paran-open" ), tr("&Open Paranthesis"), 0, this);
  funcParanOpen->setToolTip("(");
  connect(funcParanOpen, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcParanClose=new QAction(tr("Close Paranthesis"), Resource::loadPixmap( "opie-sheet/func-paran-close" ), tr("&Close Paranthesis"), 0, this);
  funcParanClose->setToolTip(")");
  connect(funcParanClose, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcComma=new QAction(tr("Comma"), Resource::loadPixmap( "opie-sheet/func-comma" ), tr("&Comma"), 0, this);
  funcComma->setToolTip(",");
  connect(funcComma, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
}

void MainWindow::initMenu()
{
  menu=new QMenuBar(this);

  menuFile=new QPopupMenu;
  fileNew->addTo(menuFile);
  fileOpen->addTo(menuFile);
  fileSave->addTo(menuFile);
  fileSaveAs->addTo(menuFile);
//  menuFile->insertSeparator();
//  fileQuit->addTo(menuFile);
  menuFile->insertSeparator();
  fileExcelImport->addTo(menuFile);
  menu->insertItem(tr("&File"), menuFile);

  menuEdit=new QPopupMenu;
  editAccept->addTo(menuEdit);
  editCancel->addTo(menuEdit);
  editCellSelect->addTo(menuEdit);
  menuEdit->insertSeparator();
  editCut->addTo(menuEdit);
  editCopy->addTo(menuEdit);
  editPaste->addTo(menuEdit);
  editPasteContents->addTo(menuEdit);
  editClear->addTo(menuEdit);
  menu->insertItem(tr("&Edit"), menuEdit);

  menuInsert=new QPopupMenu;
  menu->insertItem(tr("&Insert"), menuInsert);

  menuFormat=new QPopupMenu;
  formatCells->addTo(menuFormat);
  menu->insertItem(tr("&Format"), menuFormat);

  menuData=new QPopupMenu;
  dataSort->addTo(menuData);
  dataFindReplace->addTo(menuData);
  menu->insertItem(tr("&Data"), menuData);

//  menuHelp=new QPopupMenu;
// helpGeneral->addTo(menuHelp);
//  helpAbout->addTo(menuHelp);
//  menu->insertItem(tr("&Help"), menuHelp);

  submenuRow=new QPopupMenu;
  rowHeight->addTo(submenuRow);
  rowAdjust->addTo(submenuRow);
  rowShow->addTo(submenuRow);
  rowHide->addTo(submenuRow);
  menuFormat->insertItem(tr("&Row"), submenuRow);

  submenuCol=new QPopupMenu;
  colWidth->addTo(submenuCol);
  colAdjust->addTo(submenuCol);
  colShow->addTo(submenuCol);
  colHide->addTo(submenuCol);
  menuFormat->insertItem(tr("Colum&n"), submenuCol);

  submenuSheet=new QPopupMenu;
  sheetRename->addTo(submenuSheet);
  sheetRemove->addTo(submenuSheet);
  menuFormat->insertItem(tr("&Sheet"), submenuSheet);

  submenuFunc=new QPopupMenu;
  menuInsert->insertItem(tr("&Function"), submenuFunc);

  submenuFuncStd=new QPopupMenu;
  funcPlus->addTo(submenuFuncStd);
  funcMinus->addTo(submenuFuncStd);
  funcCross->addTo(submenuFuncStd);
  funcDivide->addTo(submenuFuncStd);
  submenuFunc->insertItem(tr("&Simple"), submenuFuncStd);



  submenuFuncStandard=new QPopupMenu;
  addFlyAction(tr("ABS(x)"), tr("ABS(x)"), "ABS(", submenuFuncStandard);
  addFlyAction(tr("CEILING(x,acc)"), tr("CEILING(x,acc)"), "CEILING(", submenuFuncStandard);
  addFlyAction(tr("FACT(x)"), tr("FACT(x)"), "FACT(", submenuFuncStandard);
  addFlyAction(tr("FLOOR(x,acc)"), tr("FLOOR(x,acc)"), "FLOOR(", submenuFuncStandard);
  addFlyAction(tr("INT(x)"), tr("INT(x)"), "INT(", submenuFuncStandard);
  addFlyAction(tr("MOD(x,y)"), tr("MOD(x,y)"), "MOD(", submenuFuncStandard);
  addFlyAction(tr("ROUND(x,digits)"), tr("ROUND(x,digits)"), "ROUND(", submenuFuncStandard);
  addFlyAction(tr("SIGN(x)"), tr("SIGN(x)"), "SIGN(", submenuFuncStandard);
  submenuFuncStandard->insertSeparator();
  addFlyAction(tr("EXP(x)"), tr("EXP(x)"), "EXP(", submenuFuncStandard);
  addFlyAction(tr("LN(x)"), tr("LN(x)"), "LN(", submenuFuncStandard);
  addFlyAction(tr("LOG(x,b)"), tr("LOG(x,b)"), "LOG(", submenuFuncStandard);
  addFlyAction(tr("LOG10(x)"), tr("LOG10(x)"), "LOG10(", submenuFuncStandard);
  addFlyAction(tr("POWER(x,y)"), tr("POWER(x,y)"), "POWER(", submenuFuncStandard);
  addFlyAction(tr("SQRT(x)"), tr("SQRT(x)"), "SQRT(", submenuFuncStandard);
  submenuFuncStandard->insertSeparator();
  addFlyAction(tr("DEGREES(x)"), tr("DEGREES(x)"), "DEGREES(", submenuFuncStandard);
  addFlyAction(tr("RADIANS(x)"), tr("RADIANS(x)"), "RADIANS(", submenuFuncStandard);
  addFlyAction(tr("PI()"), tr("PI()"), "PI()", submenuFuncStandard);
  addFlyAction(tr("RAND()"), tr("RAND()"), "RAND(", submenuFuncStandard);
  addFlyAction(tr("RANDBETWEEN(a,b)"), tr("RANDBETWEEN(a,b)"), "RANDBETWEEN(", submenuFuncStandard);
  submenuFunc->insertItem(tr("S&tandard"), submenuFuncStandard);

  submenuFuncLogic=new QPopupMenu;
  addFlyAction(tr("AND(x1,x2)"), tr("AND(x1,x2)"), "AND(", submenuFuncLogic);
  addFlyAction(tr("NOT(x)"), tr("NOT(x)"), "NOT(", submenuFuncLogic);
  addFlyAction(tr("OR(x1,x2)"), tr("OR(x1,x2)"), "OR(", submenuFuncLogic);
  submenuFuncLogic->insertSeparator();
  addFlyAction(tr("IF(compare,val1,val2)"), tr("IF(compare,val1,val2)"), "IF(", submenuFuncLogic);
  addFlyAction(tr("INDEX(range,index)"),tr("INDEX(range,index)"), "INDEX(", submenuFuncLogic);
  addFlyAction(tr("ISBLANK(x)"), tr("ISBLANK(x)"), "ISBLANK(", submenuFuncLogic);
  addFlyAction(tr("ISNUMBER(x)"), tr("ISNUMBER(x)"), "ISNUMBER(", submenuFuncLogic);
  addFlyAction(tr("EVEN(x)"), tr("EVEN(x)"), "EVEN(", submenuFuncLogic);
  addFlyAction(tr("ISEVEN(x)"), tr("ISEVEN(x)"), "ISEVEN(", submenuFuncLogic);
  addFlyAction(tr("ODD(x)"), tr("ODD(x)"), "ODD(", submenuFuncLogic);
  addFlyAction(tr("ISODD(x)"), tr("ISODD(x)"), "ISODD(", submenuFuncLogic);
  submenuFunc->insertItem(tr("Logical-&Information"), submenuFuncLogic);

  submenuFuncTrig=new QPopupMenu;
  addFlyAction(tr("SIN(x)"), tr("SIN(x)"), "SIN(", submenuFuncTrig);
  addFlyAction(tr("COS(x)"), tr("COS(x)"), "COS(", submenuFuncTrig);
  addFlyAction(tr("TAN(x)"), tr("TAN(x)"), "TAN(", submenuFuncTrig);
  addFlyAction(tr("ASIN(x)"), tr("ASIN(x)"), "ASIN(", submenuFuncTrig);
  addFlyAction(tr("ACOS(x)"), tr("ACOS(x)"), "ACOS(", submenuFuncTrig);
  addFlyAction(tr("ATAN(x)"), tr("ATAN(x)"), "ATAN(", submenuFuncTrig);
  addFlyAction(tr("ATAN2(x,y)"), tr("ATAN2(x,y)"), "ATAN2(", submenuFuncTrig);
  submenuFuncTrig->insertSeparator();
  addFlyAction(tr("SINH(x)"), tr("SINH(x)"), "SINH(", submenuFuncTrig);
  addFlyAction(tr("COSH(x)"), tr("COSH(x)"), "COSH(", submenuFuncTrig);
  addFlyAction(tr("TANH(x)"), tr("TANH(x)"), "TANH(", submenuFuncTrig);
  addFlyAction(tr("ACOSH(x)"), tr("ACOSH(x)"), "ACOSH(", submenuFuncTrig);
  addFlyAction(tr("ASINH(x)"), tr("ASINH(x)"), "ASINH(", submenuFuncTrig);
  addFlyAction(tr("ATANH(x)"), tr("ATANH(x)"), "ATANH(", submenuFuncTrig);
  submenuFunc->insertItem(tr("&Trigonometric"), submenuFuncTrig);

  submenuFuncString=new QPopupMenu;
  addFlyAction(tr("LEN(s)"), tr("LEN(s)"), "LEN(",submenuFuncString);
  addFlyAction(tr("LEFT(s,num)"), tr("LEFT(s,num)"), "LEFT(",submenuFuncString);
  addFlyAction(tr("RIGHT(s,num)"), tr("RIGHT(s,num)"), "RIGHT(",submenuFuncString);
  addFlyAction(tr("MID(s,pos,len)"), tr("MID(s,pos,len)"), "MID(",submenuFuncString);
  submenuFuncString->insertSeparator();
  addFlyAction(tr("CONCATENATE(s1,s2..)"), tr("CONCATENATE(s1,s2..)"), "CONCATENATE(",submenuFuncString);
  addFlyAction(tr("EXACT(s1,s2)"), tr("EXACT(s1,s2)"), "EXACT(",submenuFuncString);
  addFlyAction(tr("FIND(what,where,pos)"),
  			 tr("FIND(what,where,pos)"), "FIND(",submenuFuncString);
  addFlyAction(tr("REPLACE(s,pos,len,ns)"), tr("REPLACE(s,pos,len,ns)"), "REPLACE(",submenuFuncString);
  addFlyAction(tr("REPT(s,n)"), tr("REPT(s,n)"), "REPT(",submenuFuncString);
  submenuFuncString->insertSeparator();
  addFlyAction(tr("UPPER(s)"), tr("UPPER(s)"), "UPPER(",submenuFuncString);
  addFlyAction(tr("LOWER(s)"), tr("LOWER(s)"), "LOWER(",submenuFuncString);
  submenuFunc->insertItem(tr("&Strings"), submenuFuncString);

  submenuFuncStat=new QPopupMenu;
  addFlyAction(tr("AVERAGE(range)"), tr("AVERAGE(range)"), "AVERAGE(",submenuFuncStat);
  addFlyAction(tr("COUNT(range)"), tr("COUNT(range)"), "COUNT(",submenuFuncStat);
  addFlyAction(tr("COUNTIF(range,eqls)"), tr("COUNTIF(range,eqls)"), "COUNTIF(",submenuFuncStat);
  addFlyAction(tr("MAX(range)"), tr("MAX(range)"), "MAX(",submenuFuncStat);
  addFlyAction(tr("MIN(range)"), tr("MIN(range)"), "MIN(",submenuFuncStat);
  addFlyAction(tr("SUM(range)"), tr("SUM(range)"), "SUM(",submenuFuncStat);
  addFlyAction(tr("SUMSQ(range)"), tr("SUMSQ(range)"), "SUMSQ(",submenuFuncStat);
  submenuFuncStat->insertSeparator();
  addFlyAction(tr("AVERAGE(range)"), tr("AVERAGE(range)"), "AVERAGE(",submenuFuncStat);
  addFlyAction(tr("VAR(range)"), tr("VAR(range)"), "VAR(",submenuFuncStat);
  addFlyAction(tr("VARP(range)"), tr("VARP(range)"), "VARP(",submenuFuncStat);
  addFlyAction(tr("STDEV(range)"), tr("STDEV(range)"), "STDEV(",submenuFuncStat);
  addFlyAction(tr("STDEVP(range)"), tr("STDEVP(range)"), "STDEVP(",submenuFuncStat);
  addFlyAction(tr("SKEW(range)"), tr("SKEW(range)"), "SKEW(",submenuFuncStat);
  addFlyAction(tr("KURT(range)"), tr("KURT(range)"), "KURT(",submenuFuncStat);
  submenuFunc->insertItem(tr("Sta&tistical"), submenuFuncStat);

  submenuFuncScientific=new QPopupMenu;
  addFlyAction(tr("BESSELI(x,n)"), tr("BESSELI(x,n)"), "BESSELI(",submenuFuncScientific);
  addFlyAction(tr("BESSELJ(x,n)"), tr("BESSELJ(x,n)"), "BESSELJ(",submenuFuncScientific);
  addFlyAction(tr("BESSELK(x,n)"), tr("BESSELK(x,n)"), "BESSELK(",submenuFuncScientific);
  addFlyAction(tr("BESSELY(x,n)"), tr("BESSELY(x,n)"), "BESSELY(",submenuFuncScientific);
  submenuFuncScientific->insertSeparator();
  addFlyAction(tr("BETAI(x,a,b)"), tr("BETAI(x,a,b)"), "BETAI(",submenuFuncScientific);
  addFlyAction(tr("ERF(a,b)"), tr("ERF(a,b)"), "ERF(",submenuFuncScientific);
  addFlyAction(tr("ERFC(a,b)"), tr("ERFC(a,b)"), "ERFC(",submenuFuncScientific);
  addFlyAction(tr("GAMMALN(x)"), tr("GAMMALN(x)"), "GAMMALN(",submenuFuncScientific);
  addFlyAction(tr("GAMMAP(x,a)"), tr("GAMMAP(x,a)"), "GAMMAP(",submenuFuncScientific);
  addFlyAction(tr("GAMMAQ(x,a)"), tr("GAMMAQ(x,a)"), "GAMMAQ(",submenuFuncScientific);
  submenuFunc->insertItem(tr("Scienti&fic"), submenuFuncScientific);

  submenuFuncDistr=new QPopupMenu;
  addFlyAction(tr("BETADIST(z,a,b,Q?)"), tr("BETADIST(z,a,b,Q?)"), "BETADIST(",submenuFuncDistr);
  addFlyAction(tr("CHI2DIST(x,n,Q?)"), tr("CHI2DIST(x,n,Q?)"), "CHI2DIST(",submenuFuncDistr);
  addFlyAction(tr("CHIDIST(x,n,Q?)"), tr("CHIDIST(x,n,Q?)"), "CHIDIST(",submenuFuncDistr);
  addFlyAction(tr("FDIST(z,deg1,deg2,Q?)"), tr("FDIST(z,deg1,deg2,Q?)"), "FDIST(",submenuFuncDistr);
  addFlyAction(tr("GAMMADIST(x,a,b,Q?)"), tr("GAMMADIST(x,a,b,Q?)"), "GAMMADIST(",submenuFuncDistr);
  addFlyAction(tr("NORMALDIST(x,m,s,Q?)"), tr("NORMALDIST(x,m,s,Q?)"), "NORMALDIST(",submenuFuncDistr);
  addFlyAction(tr("PHI(x,Q?)"), tr("PHI(x,Q?)"), "PHI(",submenuFuncDistr);
  addFlyAction(tr("POISSON(x,n,Q?)"), tr("POISSON(x,n,Q?)"), "POISSON(",submenuFuncDistr);
  submenuFunc->insertItem(tr("&Distributions"), submenuFuncDistr);



  menuInsert->insertSeparator();
  insertCells->addTo(menuInsert);
  insertRows->addTo(menuInsert);
  insertCols->addTo(menuInsert);
  insertSheets->addTo(menuInsert);
}

void MainWindow::initStandardToolbar()
{
  toolbarStandard=new QToolBar(this);
  toolbarStandard->setHorizontalStretchable(TRUE);
  moveToolBar(toolbarStandard, Top);

  fileNew->addTo(toolbarStandard);
  fileOpen->addTo(toolbarStandard);
  fileSave->addTo(toolbarStandard);

  comboSheets=new QComboBox(toolbarStandard);
  toolbarStandard->setStretchableWidget(comboSheets);
  connect(comboSheets, SIGNAL(activated(const QString &)), this, SLOT(slotSheetChanged(const QString &)));
}

void MainWindow::initFunctionsToolbar()
{
  toolbarFunctions=new QToolBar(this);
  toolbarFunctions->setHorizontalStretchable(TRUE);
  moveToolBar(toolbarFunctions, Bottom);

  funcEqual->addTo(toolbarFunctions);
  funcPlus->addTo(toolbarFunctions);
  funcMinus->addTo(toolbarFunctions);
  funcCross->addTo(toolbarFunctions);
  funcDivide->addTo(toolbarFunctions);
  funcParanOpen->addTo(toolbarFunctions);
  funcParanClose->addTo(toolbarFunctions);
  funcComma->addTo(toolbarFunctions);

  toolFunction=new QToolButton(toolbarFunctions);
  toolFunction->setPixmap(Resource::loadPixmap( "opie-sheet/func-func" ));
  toolFunction->setTextLabel(tr("Functions"));
  toolFunction->setPopup(submenuFunc);
  toolFunction->setPopupDelay(0);
}

void MainWindow::initEditToolbar()
{
  toolbarEdit=new QToolBar(this);
  toolbarEdit->setHorizontalStretchable(TRUE);
  moveToolBar(toolbarEdit, Bottom);

  editAccept->addTo(toolbarEdit);
  editCancel->addTo(toolbarEdit);

  editData=new QLineEdit(toolbarEdit);
  toolbarEdit->setStretchableWidget(editData);
  connect(editData, SIGNAL(returnPressed()), this, SLOT(slotEditAccept()));

  editCellSelect->addTo(toolbarEdit);
}

void MainWindow::slotHelpAbout()
{
  QDialog dialogAbout(this, 0, TRUE);
  dialogAbout.resize(width()-40, height()-80);
  dialogAbout.setCaption(tr("About Opie Sheet"));

  QLabel label(tr("Opie Sheet\nSpreadsheet Software for Opie\nQWDC Beta Winner (as Sheet/Qt)\n\nDeveloped by: Serdar Ozler\nRelease 1.0.2\nRelease Date: October 08, 2002\n\nThis product is licensed under GPL. It is freely distributable. If you want to get the latest version and also the source code, please visit the web site.\n\nhttp://qtopia.sitebest.com"), &dialogAbout);
  label.setGeometry(dialogAbout.rect());
  label.setAlignment(Qt::AlignCenter | Qt::WordBreak);

  dialogAbout.exec();
}

void MainWindow::initSheet()
{
  sheet=new Sheet(DEFAULT_NUM_ROWS, DEFAULT_NUM_COLS, this);
  setCentralWidget(sheet);

  connect(sheet, SIGNAL(currentDataChanged(const QString &)), editData, SLOT(setText(const QString &)));
  connect(sheet, SIGNAL(cellClicked(const QString &)), this, SLOT(slotCellClicked(const QString &)));
  connect(sheet, SIGNAL(sheetModified()), this, SLOT(slotDocModified()));

  connect(editCut, SIGNAL(activated()), sheet, SLOT(editCut()));
  connect(editCopy, SIGNAL(activated()), sheet, SLOT(editCopy()));
  connect(editClear, SIGNAL(activated()), sheet, SLOT(editClear()));
}

void MainWindow::slotEditAccept()
{
  sheet->setData(editData->text());
}

void MainWindow::slotEditCancel()
{
  editData->setText(sheet->getData());
}

void MainWindow::slotCellSelect(bool lock)
{
  sheet->lockClicks(lock);
}

void MainWindow::addToData(const QString &data)
{
  editData->setText(editData->text().insert(editData->cursorPosition(), data));
}

void MainWindow::slotFuncOutput()
{
  if (sender()->isA("QAction"))
    addToData(((QAction *)sender())->toolTip());
}

void MainWindow::slotInsertRows()
{
  NumberDialog dialogNumber(this);
  if (dialogNumber.exec(tr("Insert Rows"), tr("&Number of rows:"))==QDialog::Accepted)
    sheet->insertRows(dialogNumber.getValue());
}

void MainWindow::slotInsertCols()
{
  NumberDialog dialogNumber(this);
  if (dialogNumber.exec(tr("Insert Columns"), tr("&Number of columns:"))==QDialog::Accepted)
    sheet->insertColumns(dialogNumber.getValue());
}

void MainWindow::slotInsertSheets()
{
  NumberDialog dialogNumber(this);
  if (dialogNumber.exec(tr("Add Sheets"), tr("&Number of sheets:"))==QDialog::Accepted)
    for (int i=dialogNumber.getValue(); i>0; --i) createNewSheet();
}

void MainWindow::slotCellClicked(const QString &cell)
{
  editCellSelect->setOn(FALSE);
  addToData(cell);
}

typeSheet *MainWindow::createNewSheet()
{
  typeSheet *newSheet=new typeSheet;
  int currentNo=1, tempNo=0;
  bool ok;

  for (typeSheet *tempSheet=listSheets.first(); tempSheet; tempSheet=listSheets.next())
    if (tempSheet->name.startsWith(tr("Sheet")) && (tempNo=tempSheet->name.mid(tr("Sheet").length()).toInt(&ok))>=currentNo && ok)
      currentNo=tempNo+1;

  newSheet->name=tr("Sheet")+QString::number(currentNo);
  newSheet->data.setAutoDelete(TRUE);

  comboSheets->insertItem(newSheet->name);
  listSheets.append(newSheet);
  return newSheet;
}

typeSheet *MainWindow::findSheet(const QString &name)
{
  for (typeSheet *tempSheet=listSheets.first(); tempSheet; tempSheet=listSheets.next())
    if (tempSheet->name==name)
      return tempSheet;
  return NULL;
}

void MainWindow::slotSheetChanged(const QString &name)
{
  sheet->copySheetData(&findSheet(sheet->getName())->data);
  sheet->setName(name);
  sheet->setSheetData(&findSheet(name)->data);
  sheet->ReCalc();
}

void MainWindow::addFlyAction(const QString &text, const QString &menuText, const QString &tip, QWidget *w)
{
  QAction *action=new QAction(text, menuText, 0, this);
  action->setToolTip(tip);
  connect(action, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  action->addTo(w);
}

void MainWindow::slotFormatCells()
{
  CellFormat dialogCellFormat(this);
  dialogCellFormat.showMaximized();
  dialogCellFormat.exec(sheet);
}

void MainWindow::slotEditPaste()
{
  sheet->editPaste();
}

void MainWindow::slotEditPasteContents()
{
  sheet->editPaste(TRUE);
}

void MainWindow::slotRowHeight()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  NumberDialog dialogNumber(this);
  if (dialogNumber.exec(tr("Row Height"), tr("&Height of each row:"), sheet->rowHeight(row1))==QDialog::Accepted)
  {
    int newHeight=dialogNumber.getValue(), row;
    for (row=row1; row<=row2; ++row)
      sheet->setRowHeight(row, newHeight);
  }
}

void MainWindow::slotRowAdjust()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  for (int row=row1; row<=row2; ++row)
    sheet->adjustRow(row);
}

void MainWindow::slotRowShow()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  for (int row=row1; row<=row2; ++row)
    sheet->showRow(row);
}

void MainWindow::slotRowHide()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  for (int row=row1; row<=row2; ++row)
    sheet->hideRow(row);
}

void MainWindow::slotColumnWidth()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  NumberDialog dialogNumber(this);
  if (dialogNumber.exec(tr("Column Width"), tr("&Width of each column:"), sheet->columnWidth(col1))==QDialog::Accepted)
  {
    int newWidth=dialogNumber.getValue(), col;
    for (col=col1; col<=col2; ++col)
      sheet->setColumnWidth(col, newWidth);
  }
}

void MainWindow::slotColumnAdjust()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  for (int col=col1; col<=col2; ++col)
    sheet->adjustColumn(col);
}

void MainWindow::slotColumnShow()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  for (int col=col1; col<=col2; ++col)
    sheet->showColumn(col);
}

void MainWindow::slotColumnHide()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  for (int col=col1; col<=col2; ++col)
    sheet->hideColumn(col);
}

void MainWindow::slotFileSaveAs()
{
  TextDialog dialogText(this);
  if (dialogText.exec(tr("Save File As"), tr("&File Name:"), currentDoc->name())!=QDialog::Accepted || dialogText.getValue().isEmpty()) return;

  currentDoc->setName(dialogText.getValue());
  currentDoc->setFile(QString::null);
  currentDoc->setLinkFile(QString::null);
  documentSave(currentDoc);
}

void MainWindow::slotImportExcel(const DocLnk &lnkDoc)
{
  ExcelBook file1;
  file1.ParseBook((char *)lnkDoc.file().ascii());
  int NumOfSheets=file1.Sheets.count();
  printf("OpieSheet::NumberOfSheets:%d\r\n",NumOfSheets);
  if (documentModified && saveCurrentFile()==QMessageBox::Cancel) return;
  if (currentDoc) delete currentDoc;
  currentDoc = new DocLnk();
  listSheets.clear();
  comboSheets->clear();
  int w1,r,c;
  ExcelSheet* sh1;
  typeSheet* newSheet;
  QString* str;
  typeCellData* newCell;
  for(w1=1;w1<=NumOfSheets;w1++)
  {
	sh1=file1.Sheets[w1-1];
	printf("OpieSheet:newSheet:%x,r=%d,c=%d\r\n",sh1,sh1->rows,sh1->cols);
	newSheet=new typeSheet;
	newSheet->data.setAutoDelete(TRUE);
	newSheet->name=sh1->name;
	printf("OpieSheet:Sheetname:%s\r\n",sh1->name.ascii());
	comboSheets->insertItem(newSheet->name);
	for(r=1; r <= sh1->rows; r++)
	{
		for(c=1;c <= sh1->cols; c++)
		{
			str=file1.CellDataString(sh1,r-1,c-1);
			if(str!=NULL && r<DEFAULT_NUM_ROWS && c<DEFAULT_NUM_COLS)
			{
			newCell=new typeCellData;
			newCell->row=r-1;
			newCell->col=c-1;
			if(str!=NULL) newCell->data=QString(*str); else newCell->data=QString("");
			newCell->background=QBrush(Qt::white, Qt::SolidPattern);
  			newCell->alignment=(Qt::AlignmentFlags)(Qt::AlignLeft | Qt::AlignTop);
  			newCell->fontColor=Qt::black;
  			newCell->font=font();
  			newCell->borders.right=QPen(Qt::gray, 1, Qt::SolidLine);
			newCell->borders.bottom=QPen(Qt::gray, 1, Qt::SolidLine);
			newSheet->data.append(newCell);
			//there is no format parsing at the moment or style parsing
			//printf("OpieSheetNumber:row=%d,col=%d,val=%s\r\n",r,c,str->latin1());
			};
		};
	};
	listSheets.append(newSheet);
	if (w1==1)//if i==0 link sheet1 with sheetview
    	{
		sheet->setName(newSheet->name);
		sheet->setSheetData(&newSheet->data);
		sheet->ReCalc();
    	};

  };
  file1.CloseFile();
  printf("Excel FILE read OK\r\n");
  documentModified=TRUE;


}

void MainWindow::slotSheetRename()
{
  TextDialog dialogText(this);
  if (dialogText.exec(tr("Rename Sheet"), tr("&Sheet Name:"), sheet->getName())!=QDialog::Accepted || dialogText.getValue().isEmpty()) return;
  QString newName=dialogText.getValue();

  typeSheet *tempSheet=findSheet(newName);
  if (tempSheet)
  {
    QMessageBox::critical(this, tr("Error"), tr("There is already a sheet named '"+newName+'\''));
    return;
  }

  tempSheet=findSheet(sheet->getName());
  for (int i=0; i<comboSheets->count(); ++i)
    if (comboSheets->text(i)==tempSheet->name)
    {
      comboSheets->changeItem(newName, i);
      break;
    }
  tempSheet->name=newName;
  sheet->setName(newName);
}

void MainWindow::slotSheetRemove()
{
  if (comboSheets->count()<2)
  {
    QMessageBox::warning(this, tr("Error"), tr("There is only one sheet!"));
    return;
  }
  if (QMessageBox::information(this, tr("Remove Sheet"), tr("Are you sure?"), QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
  {
    typeSheet *tempSheet=findSheet(sheet->getName());
    for (int i=0; i<comboSheets->count(); ++i)
      if (comboSheets->text(i)==tempSheet->name)
      {
        comboSheets->removeItem(i);
        break;
      }
    comboSheets->setCurrentItem(0);
    slotSheetChanged(comboSheets->currentText());
    listSheets.remove(tempSheet);
  }
}

void MainWindow::slotDataSort()
{
  SortDialog dialogSort(this);
  dialogSort.showMaximized();
  dialogSort.exec(sheet);
}

void MainWindow::slotDocModified()
{
  documentModified=TRUE;
}

void MainWindow::slotInsertCells()
{
  QDialog dialogInsert(this, 0, TRUE);
  dialogInsert.resize(180, 130);
  dialogInsert.setCaption(tr("Insert Cells"));

  QVButtonGroup *group=new QVButtonGroup(tr("&Type"), &dialogInsert);
  group->setGeometry(10, 10, 160, 110);
  QRadioButton *radio=new QRadioButton(tr("Shift cells &down"), group);
  radio=new QRadioButton(tr("Shift cells &right"), group);
  radio=new QRadioButton(tr("Entire ro&w"), group);
  radio=new QRadioButton(tr("Entire &column"), group);
  group->setButton(0);

  if (dialogInsert.exec()==QDialog::Accepted)
    switch (group->id(group->selected()))
    {
      case 0: sheet->insertRows(1, FALSE); break;
      case 1: sheet->insertColumns(1, FALSE); break;
      case 2: sheet->insertRows(1, TRUE); break;
      case 3: sheet->insertColumns(1, TRUE); break;
    }
}

void MainWindow::slotDataFindReplace()
{
  FindDialog dialogFind(this);
  dialogFind.showMaximized();
  dialogFind.exec(sheet);
}
