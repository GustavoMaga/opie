/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef WELLENREITERBASE_H
#define WELLENREITERBASE_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class MScanListView;
class MScanListItem;
class QPushButton;
class MLogWindow;
class MStatWindow;
class MGraphWindow;
class PacketView;

#ifdef QWS
#include <opie2/otabwidget.h>
using namespace Opie;
#else
class QTabWidget;
#endif

class WellenreiterBase : public QWidget
{
    Q_OBJECT

public:
    WellenreiterBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~WellenreiterBase();

#ifdef QWS
    Opie::Ui::OTabWidget* TabWidget;
#else
    QTabWidget* TabWidget;
#endif
    QWidget* ap;
    MScanListView* netview;
    MLogWindow* logwindow;
    PacketView* hexwindow;
    MStatWindow* statwindow;
    MGraphWindow* graphwindow;
    QWidget* about;
    QLabel* PixmapLabel1_3_2;
    QLabel* TextLabel1_4_2;

protected:
    QVBoxLayout* WellenreiterBaseLayout;
    QVBoxLayout* apLayout;
    QGridLayout* aboutLayout;
    bool event( QEvent* );

    QPixmap* ani1;
    QPixmap* ani2;
    QPixmap* ani3;
    QPixmap* ani4;

private slots:
    virtual void slotTabChanged( QWidget* ) = 0;

};

#endif // WELLENREITERBASE_H
