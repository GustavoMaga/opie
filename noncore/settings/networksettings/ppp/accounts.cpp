/*
 *           kPPP: A pppd front end for the KDE project
 *
 * $Id: accounts.cpp,v 1.12 2004-04-04 13:55:00 mickeyl Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "accounts.h"
#include "authwidget.h"
#include "pppdata.h"
#include "edit.h"

/* OPIE */
#include <qpe/qpeapplication.h>

/* QT */
#include <qdir.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qtabdialog.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>
#include <qvgroupbox.h>

/* STD */
#include <stdlib.h>

void parseargs(char* buf, char** args);


AccountWidget::AccountWidget( PPPData *pd, QWidget *parent, const char *name, WFlags f )
        : ChooserWidget( pd, parent, name, f )
{

    QWhatsThis::add(edit_b, tr("Allows you to modify the selected account"));
    QWhatsThis::add(new_b, tr("Create a new dialup connection\n"
                                  "to the Internet"));
    QWhatsThis::add(copy_b,
                        tr("Makes a copy of the selected account. All\n"
                           "settings of the selected account are copied\n"
                           "to a new account, that you can modify to fit your\n"
                           "needs"));
    QWhatsThis::add(delete_b,
                        tr("<p>Deletes the selected account\n\n"
                           "<font color=\"red\"><b>Use with care!</b></font>"));



    copy_b->setEnabled( false ); //FIXME
    //  delete_b->setEnabled( false ); //FIXME

    listListbox->insertStringList(_pppdata->getAccountList());

    for (uint i = 0; i < listListbox->count(); i++)
    {
        if ( listListbox->text(i) == _pppdata->accname() )
            listListbox->setCurrentItem( i );
    }
}



void AccountWidget::slotListBoxSelect(int idx)
{
    bool ok = _pppdata->setAccount( listListbox->text(idx) );
    ok = (bool)(idx != -1);
    delete_b->setEnabled(ok);
    edit_b->setEnabled(ok);
    //FIXME  copy_b->setEnabled(ok);
}

void AccountWidget::edit()
{
    _pppdata->setAccount(listListbox->text(listListbox->currentItem()));

    int result = doTab();

    if(result == QDialog::Accepted)
    {
        listListbox->changeItem(_pppdata->accname(),listListbox->currentItem());
        //    emit resetaccounts();
        _pppdata->save();
    }
}


void AccountWidget::create()
{

    //     if(listListbox->count() == MAX_ACCOUNTS) {
    //         QMessageBox::information(this, "sorry",
    //                                  tr("Maximum number of accounts reached."));
    //         return;
    //     }

    int result;
    if (_pppdata->newaccount() == -1)
    {
        odebug << "_pppdata->newaccount() == -1" << oendl; 
        return;
    }
    result = doTab();

    if(result == QDialog::Accepted)
    {
        listListbox->insertItem(_pppdata->accname());
        listListbox->setSelected(listListbox->findItem(_pppdata->accname()),true);

        _pppdata->save();
    }
    else
        _pppdata->deleteAccount();
}


void AccountWidget::copy()
{
    //   if(listListbox->count() == MAX_ACCOUNTS) {
    //     QMessageBox::information(this, "sorry", tr("Maximum number of accounts reached."));
    //     return;
    //   }

    if(listListbox->currentItem()<0)
    {
        QMessageBox::information(this, "sorry", tr("No account selected."));
        return;
    }

    _pppdata->copyaccount(listListbox->currentText());

    listListbox->insertItem(_pppdata->accname());
    //  emit resetaccounts();
    _pppdata->save();
}


void AccountWidget::remove()
{

    QString s = tr("Are you sure you want to delete\nthe account \"%1\"?")
                .arg(listListbox->text(listListbox->currentItem()));

    if(QMessageBox::warning(this,tr("Confirm"),s,
                            QMessageBox::Yes,QMessageBox::No
                           ) != QMessageBox::Yes)
        return;

    if(_pppdata->deleteAccount(listListbox->text(listListbox->currentItem())))
        listListbox->removeItem(listListbox->currentItem());


    //  emit resetaccounts();
    //  _pppdata->save();


    slotListBoxSelect(listListbox->currentItem());

}


int AccountWidget::doTab()
{
    QDialog *dlg = new QDialog( 0, "newAccount", true, Qt::WStyle_ContextHelp );
    QVBoxLayout *layout = new QVBoxLayout( dlg );
    layout->setSpacing( 0 );
    layout->setMargin( 1 );

    QTabWidget *tabWindow = new QTabWidget( dlg, "tabWindow" );
    layout->addWidget( tabWindow );

    bool isnewaccount;

    if(_pppdata->accname().isEmpty())
    {
        dlg->setCaption(tr("New Account"));
        isnewaccount = true;
    }
    else
    {
        QString tit = tr("Edit Account: ");
        tit += _pppdata->accname();
        dlg->setCaption(tit);
        isnewaccount = false;
    }

    //   // DIAL WIDGET
    dial_w = new DialWidget( _pppdata, tabWindow, isnewaccount, "Dial Setup");
    tabWindow->addTab( dial_w, tr("Dial") );

    //   // AUTH WIDGET
    auth_w = new AuthWidget( _pppdata, tabWindow, isnewaccount, tr("Edit Login Script"));
    tabWindow->addTab( auth_w, tr("Authentication") );

    //   // IP WIDGET
    ip_w = new IPWidget( _pppdata, tabWindow, isnewaccount, tr("IP Setup"));
    tabWindow->addTab( ip_w, tr("IP") );

    //   // GATEWAY WIDGET
    gateway_w = new GatewayWidget( _pppdata, tabWindow, isnewaccount, tr("Gateway Setup"));
    tabWindow->addTab( gateway_w, tr("Gateway") );

    //   // DNS WIDGET
    dns_w = new DNSWidget( _pppdata, tabWindow, isnewaccount, tr("DNS Servers") );
    tabWindow->addTab( dns_w, tr("DNS") );

    //   // EXECUTE WIDGET
    ExecWidget *exec_w = new ExecWidget( _pppdata, tabWindow, isnewaccount, tr("Execute Programs"));
    tabWindow->addTab( exec_w, tr("Execute") );

    int result = 0;
    bool ok = false;

    while (!ok)
    {
        result = QPEApplication::execDialog( dlg );
        ok = true;

        if(result == QDialog::Accepted)
        {
            if (!auth_w->check())
            {
                ok = false;
            }
            else if(!dial_w->save())
            {
                QMessageBox::critical(this, "error", tr( "You must enter a unique account name"));
                ok = false;
            }
            else
            {
                ip_w->save();
                dns_w->save();
                gateway_w->save();
                auth_w->save();
                exec_w->save();
            }
        }
    }

    delete dlg;

    return result;
}


// QString AccountWidget::prettyPrintVolume(unsigned int n) {
//   int idx = 0;
//   const QString quant[] = {tr("Byte"), tr("KB"),
// 		   tr("MB"), tr("GB"), QString::null};

//   float n1 = n;
//   while(n >= 1024 && quant[idx] != QString::null) {
//     idx++;
//     n /= 1024;
//   }

//   int i = idx;
//   while(i--)
//     n1 = n1 / 1024.0;

//   QString s = QString::number( n1, 'f', idx==0 ? 0 : 1 );
//   s += " " + quant[idx];
//   return s;
// }


/////////////////////////////////////////////////////////////////////////////
//
// Queries the user what to reset: costs, volume or both
//
/////////////////////////////////////////////////////////////////////////////
// QueryReset::QueryReset(QWidget *parent) : QDialog(parent, 0, true) {
// //  KWin::setIcons(winId(), kapp->icon(), kapp->miniIcon());
//   setCaption(tr("Reset Accounting"));

//   QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);
//   QVGroupBox *f = new QVGroupBox(tr("What to Reset"), this);

//   QVBoxLayout *l1 = new QVBoxLayout(this, 10, 10);
// //   costs = new QCheckBox(tr("Reset the accumulated phone costs"), f);
// //   costs->setChecked(true);
// //   l1->addWidget(costs);
// //   QWhatsThis::add(costs, tr("Check this to set the phone costs\n"
// // 			      "to zero. Typically you'll want to\n"
// // 			      "do this once a month."));

// //   volume = new QCheckBox(tr("Reset volume accounting"), f);
// //   volume->setChecked(true);
// //   l1->addWidget(volume);
// //   QWhatsThis::add(volume, tr("Check this to set the volume accounting\n"
// // 			       "to zero. Typically you'll want to do this\n"
// // 			       "once a month."));

//   l1->activate();

//   // this activates the f-layout and sets minimumSize()
//   f->show();

//   tl->addWidget(f);

//   QButtonGroup *bbox = new QButtonGroup(this);
// //  bbox->addStretch(1);
//   QPushButton *ok = new QPushButton( bbox, tr("OK") );
//   bbox->insert(ok);
//   ok->setDefault(true);
//   QPushButton *cancel = new QPushButton( bbox, tr("Cancel") );
//   bbox->insert(cancel);

//   connect(ok, SIGNAL(clicked()),
// 	  this, SLOT(accepted()));
//   connect(cancel, SIGNAL(clicked()),
// 	  this, SLOT(reject()));

//   bbox->layout();
//   tl->addWidget(bbox);

// }


// void QueryReset::accepted() {
//   int result = costs->isChecked() ? COSTS : 0;
//   result += volume->isChecked() ? VOLUME : 0;

//   done(result);
// }


