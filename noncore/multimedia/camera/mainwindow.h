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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qdatetime.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qdatetime.h>

class QAction;
class QActionGroup;
class QIconSet;
class QTimerEvent;
class QToolButton;
class QLabel;
class MainWindowBase;
class QCopChannel;
class PreviewWidget;

class CameraMainWindow: public QMainWindow
{
  Q_OBJECT

  public:
    CameraMainWindow( QWidget * parent = 0, const char * name = "mainwindow", WFlags f = 0 );
    virtual ~CameraMainWindow();

  public slots:
    void changeZoom( int );
    void systemMessage( const QCString&, const QByteArray& );
    void showContextMenu();
    void resoMenuItemClicked( QAction* );
    void qualityMenuItemClicked( QAction* );
    void zoomMenuItemClicked( QAction* );
    void flipMenuItemClicked( QAction* );
    void outputMenuItemClicked( QAction* );
    void shutterClicked();

    void updateCaption();

  protected:
    void init();
    void startVideoCapture();
    void stopVideoCapture();
    void postProcessVideo();
    void performCapture( const QString& );

    virtual void timerEvent( QTimerEvent* );

  private:
    PreviewWidget* preview;
    int _rotation;
    QCopChannel* _sysChannel;

    QActionGroup* resog;
    QActionGroup* qualityg;
    QActionGroup* zoomg;
    QActionGroup* flipg;
    QActionGroup* outputg;

    QString flip;
    int quality;
    int zoom;
    int captureX;
    int captureY;
    QString captureFormat;

    bool _capturing;
    int _pics;

    QTime _time;
    int _videopics;
    int _capturefd;
    unsigned char* _capturebuf;
};

#endif
