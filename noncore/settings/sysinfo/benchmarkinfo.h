/**********************************************************************
** BenchmarkInfo
**
** A benchmark for Qt/Embedded
**
** Copyright (C) 2004 Michael Lauer <mickey@vanille.de>
** Inspired by ZBench (C) 2002 Satoshi <af230533@im07.alpha-net.ne.jp>
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

#include <qwidget.h>
#include <qdialog.h>
#include <qdict.h>
#include <qstringlist.h>

class QClipboard;
class QComboBox;
class QCheckListItem;
class QPushButton;
class QListView;

class BenchmarkInfo : public QWidget
{
    Q_OBJECT

public:
    BenchmarkInfo( QWidget *parent = 0, const char *name = 0, int wFlags = 0 );
    ~BenchmarkInfo();

    QCheckListItem* test_alu;
    QCheckListItem* test_fpu;
    QCheckListItem* test_txt;
    QCheckListItem* test_gfx;
    QCheckListItem* test_ram;
    QCheckListItem* test_sd;
    QCheckListItem* test_cf;

    bool main_rd;
    bool main_wt;
    bool sd_rd;
    bool sd_wt;
    bool cf_rd;
    bool cf_wt;

    QClipboard* clb;
    QComboBox* machineCombo;
    QListView* tests;
    QPushButton* startButton;
    QDict <QStringList> machines;

    int textRendering( int );
    int gfxRendering( int );
    void performFileTest( const QString& fname, QCheckListItem* item );

private slots:
    bool writeFile( const QString& );
    bool readFile( const QString& );
    void run();
    void machineActivated( int );
};

