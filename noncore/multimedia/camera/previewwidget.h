/**********************************************************************
** Copyright (C) 2003 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
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

#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <qlabel.h>
#include <qimage.h>
#include <qpixmap.h>

class QTimerEvent;
class QResizeEvent;

class PreviewWidget: public QLabel
{
  Q_OBJECT

  public:
    PreviewWidget( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    virtual ~PreviewWidget();

  protected:
    virtual void timerEvent( QTimerEvent* );
    virtual void resizeEvent( QResizeEvent* );

  private:
    QPixmap p;
    QImage i;
};

#endif
