/*
                     This file is part of the OPIE Project
               =.
      .=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
     .>+-=
_;:,   .>  :=|.         This file is free software; you can
.> <`_,  > .  <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This file is distributed in the hope that
  + . -:.    =       it will be useful, but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=    =    ;      Public License for more details.
++=  -.   .`   .:
:   = ...= . :.=-        You should have received a copy of the GNU
-.  .:....=;==+<;          General Public License along with this file;
 -_. . .  )=. =           see the file COPYING. If not, write to the
  --    :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "checkbook.h"
#include "cbinfo.h"
#include "transaction.h"
#include "traninfo.h"
#include "graph.h"
#include "graphinfo.h"
#include "password.h"
#include "cfg.h"

#include <opie2/oresource.h>

#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpemessagebox.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include <qpopupmenu.h>

#define COL_ID          0
#define COL_SORTDATE    1
#define COL_NUM         2
#define COL_DATE        3
#define COL_DESC        4
#define COL_AMOUNT      5
#define COL_BAL         6

// --- Checkbook --------------------------------------------------------------
using namespace Opie::Ui;
Checkbook::Checkbook( QWidget *parent, CBInfo *i, Cfg *cfg )
    : QDialog( parent, 0, TRUE, WStyle_ContextHelp )
{
    info = i;
    _pCfg=cfg;

    // Title bar
    if ( info->name() != "" )
    {
        QString tempstr = info->name();
        tempstr.append( " - " );
        tempstr.append( tr( "Checkbook" ) );
        setCaption( tempstr );
    }
    else
    {
        setCaption( tr( "New checkbook" ) );
    }


    // Setup layout to make everything pretty
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin( 2 );
    layout->setSpacing( 4 );

    // Setup tabs for all info
    mainWidget = new OTabWidget( this );
    layout->addWidget( mainWidget );
    mainWidget->addTab( initInfo(), "checkbook/infotab", tr( "Info" ) );
    mainWidget->addTab( initTransactions(), "checkbook/trantab", tr( "Transactions" ) );
    mainWidget->addTab( initCharts(), "checkbook/charttab", tr( "Charts" ) );
    if( _pCfg->isShowLastTab() )
        mainWidget->setCurrentTab( info->getLastTab() );
    else
        mainWidget->setCurrentTab( tr( "Info" ) );
    connect( mainWidget, SIGNAL( currentChanged(QWidget*) ), this, SLOT( slotTab(QWidget*) ) );

    // Load checkbook information
    loadCheckbook();
}

Checkbook::~Checkbook()
{
}

// --- initInfo ---------------------------------------------------------------
QWidget *Checkbook::initInfo()
{
    QWidget *control = new QWidget( mainWidget, tr("Info") );

    QVBoxLayout *vb = new QVBoxLayout( control );

    QScrollView *sv = new QScrollView( control );
    vb->addWidget( sv, 0, 0 );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );

    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );

    QGridLayout *layout = new QGridLayout( container );
    layout->setSpacing( 2 );
    layout->setMargin( 4 );

    // Password protection
    passwordCB = new QCheckBox( tr( "Password protect" ), container );
    QWhatsThis::add( passwordCB, tr( "Click here to enable/disable password protection of this checkbook." ) );
    connect( passwordCB, SIGNAL( clicked() ), this, SLOT( slotPasswordClicked() ) );
    layout->addMultiCellWidget( passwordCB, 0, 0, 0, 1 );

    // Account name
    QLabel *label = new QLabel( tr( "Name:" ), container );
    QWhatsThis::add( label, tr( "Enter name of checkbook here." ) );
    layout->addWidget( label, 1, 0 );
    nameEdit = new QLineEdit( container );
    QWhatsThis::add( nameEdit, tr( "Enter name of checkbook here." ) );
    connect( nameEdit, SIGNAL( textChanged(const QString&) ),
             this, SLOT( slotNameChanged(const QString&) ) );
    layout->addWidget( nameEdit, 1, 1 );

    // Type of account
    label = new QLabel( tr( "Type:" ), container );
    QWhatsThis::add( label, tr( "Select type of checkbook here." ) );
    layout->addWidget( label, 2, 0 );
    typeList = new QComboBox( container );
    QWhatsThis::add( typeList, tr( "Select type of checkbook here." ) );
    typeList->insertStringList( _pCfg->getAccountTypes() );
    layout->addWidget( typeList, 2, 1 );

    // Bank/institution name
    label = new QLabel( tr( "Bank:" ), container );
    QWhatsThis::add( label, tr( "Enter name of the bank for this checkbook here." ) );
    layout->addWidget( label, 3, 0 );
    bankEdit = new QLineEdit( container );
    QWhatsThis::add( bankEdit, tr( "Enter name of the bank for this checkbook here." ) );
    layout->addWidget( bankEdit, 3, 1 );

    // Account number
    label = new QLabel( tr( "Account number:" ), container );
    QWhatsThis::add( label, tr( "Enter account number for this checkbook here." ) );
    layout->addWidget( label, 4, 0 );
    acctNumEdit = new QLineEdit( container );
    QWhatsThis::add( acctNumEdit, tr( "Enter account number for this checkbook here." ) );
    layout->addWidget( acctNumEdit, 4, 1 );

    // PIN number
    label = new QLabel( tr( "PIN number:" ), container );
    QWhatsThis::add( label, tr( "Enter PIN number for this checkbook here." ) );
    layout->addWidget( label, 5, 0 );
    pinNumEdit = new QLineEdit( container );
    QWhatsThis::add( pinNumEdit, tr( "Enter PIN number for this checkbook here." ) );
    layout->addWidget( pinNumEdit, 5, 1 );

    // Starting balance
    label = new QLabel( tr( "Starting balance:" ), container );
    QWhatsThis::add( label, tr( "Enter the initial balance for this checkbook here." ) );
    layout->addWidget( label, 6, 0 );
    balanceEdit = new QLineEdit( container );
    QWhatsThis::add( balanceEdit, tr( "Enter the initial balance for this checkbook here." ) );
    connect( balanceEdit, SIGNAL( textChanged(const QString&) ),
             this, SLOT( slotStartingBalanceChanged(const QString&) ) );
    layout->addWidget( balanceEdit, 6, 1 );

    // Notes
    label = new QLabel( tr( "Notes:" ), container );
    QWhatsThis::add( label, tr( "Enter any additional information for this checkbook here." ) );
    layout->addWidget( label, 7, 0 );
    notesEdit = new QMultiLineEdit( container );
    QWhatsThis::add( notesEdit, tr( "Enter any additional information for this checkbook here." ) );
    notesEdit->setMinimumHeight( 25 );
    notesEdit->setMaximumHeight( 65 );
    layout->addMultiCellWidget( notesEdit, 8, 8, 0, 1 );

    return control;
}


// --- initTransactions -------------------------------------------------------
QWidget *Checkbook::initTransactions()
{
    QWidget *control = new QWidget( mainWidget, tr("Transactions") );

    QGridLayout *layout = new QGridLayout( control );
    layout->setSpacing( 2 );
    layout->setMargin( 4 );

    // Sort selector
    QLabel *label = new QLabel( tr( "Sort by:" ), control );
    QWhatsThis::add( label, tr( "Select checkbook sorting here." ) );
    layout->addMultiCellWidget( label, 0, 0, 0, 1 );
    _cbSortType=new QComboBox( control );
    _cbSortType->insertItem( tr("Entry Order") );
    _cbSortType->insertItem( tr("Date") );
    _cbSortType->insertItem( tr("Number") );
    layout->addMultiCellWidget( _cbSortType, 0, 0, 1, 2 );
    connect( _cbSortType, SIGNAL( activated(const QString&) ), this, SLOT( slotSortChanged(const QString&) ) );

    // Table
    tranTable = new QListView( control );
    QFont fnt(QPEApplication::font());
    if( _pCfg->getUseSmallFont() )
	{
		fnt.setPointSize( fnt.pointSize()-1 );
	}
    tranTable->setFont( fnt );
    QWhatsThis::add( tranTable, tr( "This is a listing of all transactions entered for this checkbook.\n\nTo sort entries by a specific field, click on the column name." ) );
    tranTable->addColumn( tr( "Id" ) );
    tranTable->setColumnWidthMode( COL_ID, QListView::Manual );
    tranTable->setColumnWidth( COL_ID, 0);
    tranTable->addColumn( tr( "SortDate" ) );
    tranTable->setColumnWidthMode( COL_SORTDATE, QListView::Manual );
    tranTable->setColumnWidth( COL_SORTDATE, 0);
    tranTable->addColumn( tr( "Num" ) );
    tranTable->addColumn( tr( "Date" ) );
    //tranTable->addColumn( tr( "Cleared" ) );
    tranTable->addColumn( tr( "Description" ) );
    int column = tranTable->addColumn( tr( "Amount" ) );
    tranTable->setColumnAlignment( column, Qt::AlignRight );
    column=tranTable->addColumn( tr("Balance") );
    tranTable->setColumnAlignment( column, Qt::AlignRight );
    tranTable->setAllColumnsShowFocus( TRUE );
    tranTable->setSorting( -1 );
    layout->addMultiCellWidget( tranTable, 1, 1, 0, 2 );
    QPEApplication::setStylusOperation( tranTable->viewport(), QPEApplication::RightOnHold );
    connect( tranTable, SIGNAL( rightButtonPressed(QListViewItem*,const QPoint&,int) ),
          this, SLOT( slotMenuTran(QListViewItem*,const QPoint&) ) );
    connect( tranTable, SIGNAL( doubleClicked(QListViewItem*) ),
          this, SLOT( slotEditTran() ) );
    _sortCol=COL_ID;

    // Buttons
    QPushButton *btn = new QPushButton( Opie::Core::OResource::loadPixmap( "new", Opie::Core::OResource::SmallIcon ),
                                        tr( "New" ), control );
    btn->setFixedHeight( AppLnk::smallIconSize()+4 );
    QWhatsThis::add( btn, tr( "Click here to add a new transaction." ) );
    connect( btn, SIGNAL( clicked() ), this, SLOT( slotNewTran() ) );
    layout->addWidget( btn, 2, 0 );

    btn = new QPushButton( Opie::Core::OResource::loadPixmap( "edit", Opie::Core::OResource::SmallIcon ),
                           tr( "Edit" ), control );
    btn->setFixedHeight( AppLnk::smallIconSize()+4 );
    QWhatsThis::add( btn, tr( "Select a transaction and then click here to edit it." ) );
    connect( btn, SIGNAL( clicked() ), this, SLOT( slotEditTran() ) );
    layout->addWidget( btn, 2, 1 );

    btn = new QPushButton( Opie::Core::OResource::loadPixmap( "trash", Opie::Core::OResource::SmallIcon ),
                           tr( "Delete" ), control );
    btn->setFixedHeight( AppLnk::smallIconSize()+4 );
    QWhatsThis::add( btn, tr( "Select a checkbook and then click here to delete it." ) );
    connect( btn, SIGNAL( clicked() ), this, SLOT( slotDeleteTran() ) );
    layout->addWidget( btn, 2, 2 );

    return( control );
}


// --- initCharts -------------------------------------------------------------
QWidget *Checkbook::initCharts()
{
    graphInfo = 0x0;

    QWidget *control = new QWidget( mainWidget, tr("Charts") );

    QGridLayout *layout = new QGridLayout( control );
    layout->setSpacing( 2 );
    layout->setMargin( 4 );

    graphWidget = new Graph( control );
    QWhatsThis::add( graphWidget, tr( "Select the desired chart below and then click on the Draw button." ) );
    layout->addMultiCellWidget( graphWidget, 0, 0, 0, 2 );

    graphList = new QComboBox( control );
    QWhatsThis::add( graphList, tr( "Click here to select the desired chart type." ) );
    graphList->insertItem( tr( "Account balance" ) );
    graphList->insertItem( tr( "Withdrawals by category" ) );
    graphList->insertItem( tr( "Deposits by category" ) );

    layout->addMultiCellWidget( graphList, 1, 1, 0, 1 );

    QPushButton *btn = new QPushButton( Opie::Core::OResource::loadPixmap( "checkbook/drawbtn", Opie::Core::OResource::SmallIcon ),
                                        tr( "Draw" ), control );
    btn->setFixedHeight( AppLnk::smallIconSize()+4 );
    QWhatsThis::add( btn, tr( "Click here to draw the selected chart." ) );
    connect( btn, SIGNAL( clicked() ), this, SLOT( slotDrawGraph() ) );
    layout->addWidget( btn, 1, 2 );

    return control;
}

// --- loadCheckbook ----------------------------------------------------------
void Checkbook::loadCheckbook()
{
    if ( !info )
    {
        return;
    }

    tranList = info->transactions();

    passwordCB->setChecked( !info->password().isNull() );
    nameEdit->setText( info->name() );
    QString temptext = info->type();
    int i = typeList->count();
    while ( i > 0 )
    {
        i--;
        typeList->setCurrentItem( i );
        if ( typeList->currentText() == temptext )
        {
            break;
        }
    }
    if( i<=0 ) {
        typeList->insertItem( temptext, 0 );
        typeList->setCurrentItem(0);
    }
    bankEdit->setText( info->bank() );
    acctNumEdit->setText( info->account() );
    pinNumEdit->setText( info->pin() );
    temptext.setNum( info->startingBalance(), 'f', 2 );
    balanceEdit->setText( temptext );
    notesEdit->setText( info->notes() );

    // Load transactions
    float amount;
    QString stramount;
    QString symbol = _pCfg->getCurrencySymbol();
    for ( TranInfo *tran = tranList->first(); tran; tran = tranList->next() )
    {
        amount = tran->amount();
        if ( tran->withdrawal() )
        {
            amount *= -1;
        }
        stramount.sprintf( "%.2f", amount );
	stramount.prepend( symbol );
        ( void ) new CBListItem( tran, tranTable, tran->getIdStr(), tran->datestr(false), tran->number(), tran->datestr(true), tran->desc(), stramount );
    }

    // set sort order
    bool bOk=false;
    for(int i=0; i<_cbSortType->count(); i++) {
        if( _cbSortType->text(i)==info->getSortOrder() ) {
            _cbSortType->setCurrentItem(i);
            slotSortChanged( info->getSortOrder() );
            bOk=true;
            break;
        }
    }
    if( !bOk ) {
        _cbSortType->setCurrentItem(0);
        slotSortChanged( _cbSortType->currentText() );
    }

    // calc running balance
    adjustBalance();
}


// --- adjustBalance ----------------------------------------------------------
void Checkbook::adjustBalance()
{
    // update running balance in register
    QString sRunning;
    QString symbol = _pCfg->getCurrencySymbol();
    float bal=info->startingBalance();

    for(CBListItem *item=(CBListItem *)tranTable->firstChild(); item; item=(CBListItem *)item->nextSibling() ) {
        TranInfo *tran=item->getTranInfo();
        bal=bal + (tran->withdrawal() ? -1 : 1)*tran->amount() - tran->fee();
        sRunning.sprintf( "%.2f", bal );
	sRunning.prepend(symbol);
        item->setText( COL_BAL, sRunning);
    }
}

// --- resort -----------------------------------------------------------------
void Checkbook::resort()
{
    tranTable->setSorting(_sortCol);
    tranTable->sort();
    tranTable->setSorting(-1);
}


// --- accept -----------------------------------------------------------------
void Checkbook::accept()
{
    info->setName( nameEdit->text() );
    info->setType( typeList->currentText() );
    info->setBank( bankEdit->text() );
    info->setAccount( acctNumEdit->text() );
    info->setPin( pinNumEdit->text() );
    bool ok;
    info->setStartingBalance( balanceEdit->text().toFloat( &ok ) );
    info->setNotes( notesEdit->text() );

    QDialog::accept();
}

// --- slotPasswordClicked ----------------------------------------------------
void Checkbook::slotPasswordClicked()
{
    if ( info->password().isNull() && passwordCB->isChecked() )
    {
        Password *pw = new Password( this, tr( "Enter password" ), tr( "Please enter your password:" ) );
        if ( pw->exec() != QDialog::Accepted  )
        {
            passwordCB->setChecked( FALSE );
            delete pw;
            return;
        }
        info->setPassword( pw->password );
        delete pw;

        pw = new Password( this, tr( "Confirm password" ), tr( "Please confirm your password:" ) );
        if ( pw->exec() != QDialog::Accepted || pw->password != info->password() )
        {
            passwordCB->setChecked( FALSE );
            info->setPassword( QString::null );
        }

        delete pw;
    }
    else if ( !info->password().isNull() && !passwordCB->isChecked() )
    {
        Password *pw = new Password( this, tr( "Enter password" ),
                    tr( "Please enter your password to confirm removal of password protection:" ) );
        if ( pw->exec() == QDialog::Accepted && pw->password == info->password() )
        {
            info->setPassword( QString::null );
            delete pw;
            return;
        }
        else
        {
            passwordCB->setChecked( TRUE );
        }

        delete pw;
    }
}

void Checkbook::slotNameChanged( const QString &newname )
{
    info->setName( newname );

    // TODO - need filedir
//    QString namestr = filedir;
//    namestr.append( newname );
//    namestr.append( ".qcb" );
//    info->setFilename( namestr );

    QString namestr = newname;
    namestr.append( " - " );
    namestr.append( tr( "Checkbook" ) );
    setCaption( namestr );
}


// ---slotStartingBalanceChanged ----------------------------------------------
void Checkbook::slotStartingBalanceChanged( const QString &newbalance )
{
    bool ok;
    info->setStartingBalance( newbalance.toFloat( &ok ) );
    adjustBalance();
}


// --- slotNewTran ------------------------------------------------------------
void Checkbook::slotNewTran()
{
    TranInfo *traninfo = new TranInfo( info->getNextNumber() );
    if( !_dLastNew.isNull() )
        traninfo->setDate(_dLastNew);

    Transaction *currtran = new Transaction( this, true, info->name(),
                                             traninfo,
                                             _pCfg );
    QString symbol = _pCfg->getCurrencySymbol();
    if ( QPEApplication::execDialog( currtran ) == QDialog::Accepted )
    {
        // Add to transaction list
        info->addTransaction( traninfo );

        // Add to transaction table
        float amount;
        QString stramount;
        amount = (traninfo->withdrawal() ? -1 : 1)*traninfo->amount();
        stramount.sprintf( "%.2f", amount );
	stramount.prepend(symbol);
        ( void ) new CBListItem( traninfo, tranTable, traninfo->getIdStr(), traninfo->datestr(false),
                                 traninfo->number(), traninfo->datestr(true), traninfo->desc(),
                                 stramount );
        resort();
        adjustBalance();

        // save last date
        _dLastNew = traninfo->date();

        // save description in list of payees, if not in there
        QStringList *pLst=&_pCfg->getPayees();
        if( _pCfg->getSavePayees() && pLst->contains(traninfo->desc())==0 ) {
            pLst->append( traninfo->desc() );
            pLst->sort();
            _pCfg->setDirty(true);
        }
    }
    else
    {
        delete traninfo;
    }
}


// --- slotEditTran -----------------------------------------------------------
void Checkbook::slotEditTran()
{
    QListViewItem *curritem = tranTable->currentItem();
    if ( !curritem )
        return;

    TranInfo *traninfo=info->findTransaction( curritem->text(COL_ID) );

    Transaction *currtran = new Transaction( this, false, info->name(),
                                             traninfo,
                                             _pCfg );
    if ( QPEApplication::execDialog( currtran ) == QDialog::Accepted )
    {
        curritem->setText( COL_NUM, traninfo->number() );
        curritem->setText( COL_SORTDATE, traninfo->datestr(false) );
        curritem->setText( COL_DATE, traninfo->datestr(true) );
        curritem->setText( COL_DESC, traninfo->desc() );

        float amount = traninfo->amount();
        if ( traninfo->withdrawal() )
        {
            amount *= -1;
        }
        QString stramount;
        stramount.sprintf( "%.2f", amount );
	stramount.prepend( _pCfg->getCurrencySymbol() );
        curritem->setText( COL_AMOUNT, stramount );
        resort();
        adjustBalance();

        // save description in list of payees, if not in there
        QStringList *pLst=&_pCfg->getPayees();
        if( _pCfg->getSavePayees() && pLst->contains(traninfo->desc())==0 ) {
            pLst->append( traninfo->desc() );
            pLst->sort();
            _pCfg->setDirty(true);
        }
    }

    delete currtran;
}

// --- slotMenuTran -----------------------------------------------------------
void Checkbook::slotMenuTran(QListViewItem *item, const QPoint &pnt)
{
    // active item?
    if( !item )
        return;

    // Display menu
    QPopupMenu m;
    m.insertItem( QWidget::tr( "Edit" ), 1 );
    m.insertItem( QWidget::tr( "New" ), 2 );
    m.insertItem( QWidget::tr( "Delete" ), 3 );
    int r = m.exec( pnt );
    switch(r) {
    case 1:
        slotEditTran();
        break;
    case 2:
        slotNewTran();
        break;
    case 3:
        slotDeleteTran();
        break;
    }
}


// --- slotDeleteTran ---------------------------------------------------------
void Checkbook::slotDeleteTran()
{
    QListViewItem *curritem = tranTable->currentItem();
    if ( !curritem )
        return;

    TranInfo *traninfo = info->findTransaction( curritem->text(COL_ID) );

    if ( QPEMessageBox::confirmDelete ( this, tr( "Delete transaction" ), traninfo->desc() ) )
    {
        info->removeTransaction( traninfo );
        delete curritem;
        adjustBalance();
    }
}

void Checkbook::slotDrawGraph()
{
    if ( graphInfo )
    {
        delete graphInfo;
    }

    switch ( graphList->currentItem() )
    {
        case 0 : drawBalanceChart();
            break;
        case 1 : drawCategoryChart( TRUE );
            break;
        case 2 : drawCategoryChart( FALSE );
            break;
    };

    graphWidget->setGraphInfo( graphInfo );
    graphWidget->drawGraph( TRUE );
}

void Checkbook::drawBalanceChart()
{
    DataPointList *list = new DataPointList();

    float balance = info->startingBalance();
    float amount;
    QString label;
    int i = 0;
    int count = tranList->count();

    for ( TranInfo *tran = tranList->first(); tran; tran = tranList->next() )
    {
        i++;
        balance -= tran->fee();
        amount = tran->amount();
        if ( tran->withdrawal() )
        {
            amount *= -1;
        }
        balance += amount;
        if ( i == 1 || i == count / 2 || i == count )
        {
            label = tran->datestr(true);
        }
        else
        {
            label = "";
        }
        list->append( new DataPointInfo( label, balance ) );
    }

    graphInfo = new GraphInfo( GraphInfo::BarChart, list );
}

void Checkbook::drawCategoryChart( bool withdrawals )
{
    DataPointList *list = new DataPointList();

    TranInfo *tran = tranList->first();
    if ( tran && tran->withdrawal() == withdrawals )
    {
        list->append( new DataPointInfo( tran->category(), tran->amount() ) );
    }
    tran = tranList->next();

    DataPointInfo *cat;
    for ( ; tran; tran = tranList->next() )
    {
        if ( tran->withdrawal() == withdrawals )
        {
            // Find category in list
            for ( cat = list->first(); cat; cat = list->next() )
            {
                if ( cat->label() == tran->category() )
                {
                    break;
                }
            }
            if ( cat && cat->label() == tran->category() )
            { // Found category, add to transaction to category total
                cat->addToValue( tran->amount() );
            }
            else
            { // Didn't find category, add category to list
                list->append( new DataPointInfo( tran->category(), tran->amount() ) );
            }
        }
    }

    graphInfo = new GraphInfo( GraphInfo::PieChart, list );
}

CBListItem::CBListItem( TranInfo *pTran, QListView *parent, QString label1, QString label2,
                     QString label3, QString label4, QString label5, QString label6, QString label7,
                     QString label8 )
    : QListViewItem( parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
    _pTran=pTran;
    m_known = FALSE;
    owner = parent;
}

void CBListItem::paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align )
{
    QColorGroup _cg = cg;
    const QPixmap *pm = listView()->viewport()->backgroundPixmap();
    if ( pm && !pm->isNull() )
    {
        _cg.setBrush( QColorGroup::Base, QBrush( cg.base(), *pm ) );
        p->setBrushOrigin( -listView()->contentsX(), -listView()->contentsY() );
    }
    else if ( isAltBackground() )
        _cg.setColor(QColorGroup::Base, cg.background() );

    QListViewItem::paintCell(p, _cg, column, width, align);
}

// --- CBListItem::isAltBackground --------------------------------------------
bool CBListItem::isAltBackground()
{
    QListView *lv = static_cast<QListView *>( listView() );
    if ( lv )
    {
        CBListItem *above = 0;
        above = (CBListItem *)( itemAbove() );
        m_known = above ? above->m_known : true;
        if ( m_known )
        {
            m_odd = above ? !above->m_odd : false;
        }
        else
        {
            CBListItem *item;
            bool previous = true;
            if ( parent() )
            {
                item = (CBListItem *)( parent() );
                if ( item )
                    previous = item->m_odd;
                item = (CBListItem *)( parent()->firstChild() );
            }
            else
            {
                item = (CBListItem *)( lv->firstChild() );
            }

            while(item)
            {
                item->m_odd = previous = !previous;
                item->m_known = true;
                item = (CBListItem *)( item->nextSibling() );
            }
        }
        return m_odd;
    }
    return false;
}


// --- slotTab ----------------------------------------------------------------
void Checkbook::slotTab(QWidget *tab)
{
    if( !tab || !info ) return;
    info->setLastTab( tab->name() );
}


// --- slotSortChanged ---------------------------------------------------------
void Checkbook::slotSortChanged( const QString &selc )
{
    if( selc==tr("Entry Order") ) {
        _sortCol=COL_ID;
    } else if( selc==tr("Number") ) {
        _sortCol=COL_NUM;
    } else if( selc==tr("Date") ) {
        _sortCol=COL_SORTDATE;
    }
    info->setSortOrder( selc );
    resort();
}

