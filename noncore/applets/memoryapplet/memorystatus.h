/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
#ifndef MEMORY_STATUS_H
#define MEMORY_STATUS_H

#include <qframe.h>

#include <opie2/otabwidget.h>

class MemoryInfo;
class Swapfile;


class MemoryStatus : public QFrame
{
    Q_OBJECT
public:
    MemoryStatus(QWidget *parent = 0, WFlags f = 0);
    ~MemoryStatus();

    QSize sizeHint() const;
    MemoryInfo* mi;
    Swapfile* sf;

    int percent();

private:
    Opie::Ui::OTabWidget *tab;
};

#endif

