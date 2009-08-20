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
#include <qwidget.h>
#include <qframe.h>
#include <qlist.h>
#include <qdict.h>

class QLabel;
class GraphData;
class Graph;
class GraphLegend;
class FileSystem;
class MountInfo;
class QVBoxLayout;
class QWidget;
class StorageInfo;


class FileSysInfo : public QWidget
{
    Q_OBJECT
public:
    FileSysInfo( QWidget *parent=0, const char *name=0 );

protected:
    void timerEvent(QTimerEvent*);

private:
    void updateMounts();

    QWidget     *container;
    QVBoxLayout *vb;

    StorageInfo     *storage;
    QDict<MountInfo> disks;
    QList<QFrame>    lines;

    bool rebuildDisks;

private slots:
    void disksChanged();
};

class MountInfo : public QWidget
{
    Q_OBJECT
public:
    MountInfo( FileSystem *filesys=0, QWidget *parent=0, const char *name=0 );
    ~MountInfo();

    void updateData();

    FileSystem *fs;

private:
    QString title;
    QLabel *totalSize;
    GraphData *data;
    Graph *graph;
    GraphLegend *legend;
};

