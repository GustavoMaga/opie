/* main.cpp
 *
 * copyright   : (c) 2002 by Maximilian Rei�
 * email       : max.reiss@gmx.de
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef SCANDIALOG_H
#define SCANDIALOG_H

#include <qvariant.h>
#include <qdialog.h>


#include <remotedevice.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QFrame;
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;

class Manager;
class Device;


namespace OpieTooth {



    class ScanDialog : public QDialog {
        Q_OBJECT

    public:
        ScanDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
        ~ScanDialog();

        QFrame* Frame7;
        QLabel* TextLabel10;
        QPushButton* StartButton;
        QPushButton* PushButton6;
        QListView* ListView1;


    protected:
        QGridLayout* Layout11;

    private slots:
        void startSearch();
        void fillList(const QString& device, RemoteDevices::ValueList list);

    private:

        Manager *localDevice;
    };

}

#endif // SCANDIALOG_H
