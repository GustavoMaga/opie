/*
� � � � � � � �              This file is part of the OPIE Project
               =.            Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
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

#ifndef APPEARANCESETTINGS_H
#define APPEARANCESETTINGS_H

#include <opie2/ofontselector.h>

#include <qpe/fontdatabase.h>

#include <qdialog.h>

using Opie::Ui::OFontSelector;

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListBox;
class QMultiLineEdit;
class QPushButton;
class QRadioButton;
class QToolButton;
class SampleWindow;
namespace Opie {namespace Ui {class OFontSelector;}}
class QListView;
class QListViewItem;
class Config;

class Appearance : public QDialog
{
    Q_OBJECT

public:
    Appearance( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Appearance();
    static QString appName() { return QString::fromLatin1("appearance"); }

protected:
    virtual void accept ( );
    virtual void done ( int r );

protected slots:
    void styleClicked ( int );
    void styleSettingsClicked ( );
    void decoClicked ( int );
    void fontClicked ( const QFont & );
    void colorClicked ( int );

    void editSchemeClicked();
    void saveSchemeClicked();
    void deleteSchemeClicked();

    void tabChanged ( QWidget * );

private:
    void changeText();

    QWidget *createStyleTab ( QWidget *parent, Config &cfg );
    QWidget *createDecoTab ( QWidget *parent, Config &cfg );
    QWidget *createFontTab ( QWidget *parent, Config &cfg );
    QWidget *createColorTab ( QWidget *parent, Config &cfg );
    QWidget *createAdvancedTab ( QWidget *parent, Config &cfg );

private:
    bool m_style_changed;
    bool m_font_changed;
    bool m_scheme_changed;
    bool m_deco_changed;
    bool m_color_changed;

    int  m_original_style;
    int  m_original_deco;
    int  m_original_tabstyle;
    bool m_original_tabpos;

    QListBox *    m_style_list;
    QPushButton * m_style_settings;

    QListBox *    m_deco_list;

    QListBox *    m_color_list;

    Opie::Ui::OFontSelector *m_fontselect;

    SampleWindow *m_sample;

    QComboBox *   m_tabstyle_list;
    QRadioButton *m_tabstyle_top;
    QRadioButton *m_tabstyle_bottom;

    QRadioButton *m_rotdir_cw;
    QRadioButton *m_rotdir_ccw;
    QRadioButton *m_rotdir_flip;

    QWidget *     m_advtab;
    QCheckBox    *m_leftHand;
};

#endif
