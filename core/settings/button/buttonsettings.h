/*
� � � � � � � �=.            This file is part of the OPIE Project
� � � � � � �.=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This file is free software; you can
.> <`_, � > �. � <=          redistribute it and/or modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
� � .%`+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This file is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful, but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|`    MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.= � � � = � � � ;      Public License for more details.
++= � -. � � .` � � .:
�: � � = �...= . :.=-        You should have received a copy of the GNU
�-. � .:....=;==+<;          General Public License along with this file;
� -_. . . � )=. �=           see the file COPYING. If not, write to the
� � -- � � � �:-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef __BUTTON_SETTINGS_H__
#define __BUTTON_SETTINGS_H__

#include <qdialog.h>
#include <qlist.h>
#include <opie2/odevice.h>

class QTimer;


class buttoninfo;

class ButtonSettings : public QDialog {
    Q_OBJECT

public:
    ButtonSettings ( QWidget *parent = 0, const char *name = 0,  bool modal = FALSE,WFlags f = 0 );
    ~ButtonSettings();

    static QString appName() { return QString::fromLatin1("buttonsettings"); }
    virtual void accept();
    virtual void done ( int r );

private slots:
    void keyTimeout();
    void updateLabels();

protected:
    virtual void keyPressEvent ( QKeyEvent *e );
    virtual void keyReleaseEvent ( QKeyEvent *e );

private:
    buttoninfo *buttonInfoForKeycode ( ushort key );

    void edit ( buttoninfo *bi, bool hold );
    QString qcopToString ( const Opie::Core::OQCopMessage &c );

private:
    QTimer *m_timer;
    buttoninfo *m_last_button;

    QList <buttoninfo> m_infos;
    bool m_lock;
};

#endif


