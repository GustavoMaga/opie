/*
                             This file is part of the Opie Project

                             Copyright (c)  2002 Trolltech AS <info@trolltech.com>
                             Copyright (c)  2002 Dan Williams <drw@handhelds.org>
                             Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
:     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "appearance.h"
#include "editScheme.h"
#include "stylelistitem.h"
#include "decolistitem.h"
#include "colorlistitem.h"
#include "exceptlistitem.h"
#include "sample.h"

/* OPIE */
#include <opie2/odevice.h>
#include <opie2/ofiledialog.h>
#include <opie2/otabwidget.h>
#include <opie2/odebug.h>

#include <qpe/config.h>
#include <qpe/global.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpemessagebox.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpestyle.h>
#include <qpe/lightstyle.h>
#include <qpe/styleinterface.h>

/* QT */
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qdir.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#if QT_VERSION >= 0x030000
#include <qstylefactory.h>
#endif
#include <qtoolbutton.h>
#include <qwindowsstyle.h>
#include <qlistview.h>
#include <qheader.h>
#include <qvbox.h>
#include <qwhatsthis.h>

using namespace Opie;
using namespace Opie::Ui;
using namespace Opie::Core;

class DefaultWindowDecoration : public WindowDecorationInterface
{
public:
    DefaultWindowDecoration() : ref(0) {}
    QString name() const
    {
        return "Default";
    }
    QPixmap icon() const
    {
        return QPixmap();
    }
    QRESULT queryInterface( const QUuid &uuid, QUnknownInterface **iface )
    {
        *iface = 0;
        if ( uuid == IID_QUnknown )
            *iface = this;
        else if ( uuid == IID_WindowDecoration )
            *iface = this;

        if ( *iface )
            (*iface)->addRef();
        return QS_OK;
    }
    Q_REFCOUNT

private:
    ulong ref;
};







QWidget *Appearance::createStyleTab ( QWidget *parent, Config &cfg )
{
    QWidget* tab = new QWidget( parent, "StyleTab" );
    QVBoxLayout* vertLayout = new QVBoxLayout( tab, 3, 3 );

    m_style_list = new QListBox( tab, "m_style_list" );
    vertLayout->addWidget( m_style_list );
    QWhatsThis::add( m_style_list, tr( "Styles control the way items such as buttons and scroll bars appear in all applications.\n\nClick here to select an available style." ) );

    m_style_settings = new QPushButton ( tr( "Settings..." ), tab );
    connect ( m_style_settings, SIGNAL( clicked()), this, SLOT( styleSettingsClicked()));
    vertLayout-> addWidget ( m_style_settings );
    QWhatsThis::add( m_style_settings, tr( "Click here to configure the currently selected style.\n\nNote:  This option is not available for all styles." ) );

    QString s = cfg. readEntry ( "Style", "Light" );


#if QT_VERSION >= 0x030000
    m_style_list->insertStringList(QStyleFactory::styles());
#else
    m_style_list-> insertItem ( new StyleListItem ( "Windows", new QWindowsStyle ( )));
    m_style_list-> insertItem ( new StyleListItem ( "Light", new LightStyle ( )));
    m_style_list-> insertItem ( new StyleListItem ( "QPE", new QPEStyle ( )));
#endif

    {
        QString path = QPEApplication::qpeDir ( );
        path.append( "/plugins/styles/" );
        QStringList sl = QDir ( path, "lib*.so" ). entryList ( );

        for ( QStringList::Iterator it = sl. begin ( ); it != sl. end ( ); ++it )
        {
            QString libstr = path;
            libstr.append( "/" );
            libstr.append( *it );
            QLibrary *lib = new QLibrary ( libstr );
            StyleInterface *iface;

            if (( lib-> queryInterface ( IID_Style, (QUnknownInterface **) &iface ) == QS_OK ) && iface )
            {
                StyleListItem *slit = new StyleListItem ( lib, iface );
                m_style_list-> insertItem ( slit );

                if ( slit-> key ( ) == s )
                    m_style_list-> setCurrentItem ( slit );
            }
            else
                delete lib;
        }
    }

    m_original_style = m_style_list-> currentItem ( );
    styleClicked ( m_original_style );

    connect( m_style_list, SIGNAL( highlighted(int) ), this, SLOT( styleClicked(int) ) );

    return tab;
}

QWidget *Appearance::createDecoTab ( QWidget *parent, Config &cfg )
{
    QWidget* tab = new QWidget( parent, "DecoTab" );
    QVBoxLayout* vertLayout = new QVBoxLayout( tab, 3, 3 );

    m_deco_list = new QListBox( tab, "m_deco_list" );
    vertLayout->addWidget( m_deco_list );
    QWhatsThis::add( m_deco_list, tr( "Window decorations control the way the application title bar and its buttons appear.\n\nClick here to select an available decoration." ) );

    QString s = cfg. readEntry ( "Decoration", "libflat.so" );

    m_deco_list-> insertItem ( new DecoListItem ( "QPE" ));

    {
        QString path = QPEApplication::qpeDir();
        path.append( "/plugins/decorations/" );
        QStringList sl = QDir ( path, "lib*.so" ). entryList ( );

        for ( QStringList::Iterator it = sl. begin ( ); it != sl. end ( ); ++it )
        {
            QString libstr = path;
            libstr.append( "/" );
            libstr.append( *it );
            QLibrary *lib = new QLibrary ( libstr );
            WindowDecorationInterface *iface;

            if ( lib-> queryInterface ( IID_WindowDecoration, (QUnknownInterface **) &iface ) == QS_OK )
            {
                DecoListItem *dlit = new DecoListItem ( lib, iface );
                m_deco_list-> insertItem ( dlit );

                if ( dlit-> key ( ) == s )
                    m_deco_list-> setCurrentItem ( dlit );
            }
            else
                delete lib;
        }
    }

    m_original_deco = m_deco_list-> currentItem ( );
    if ( m_deco_list-> currentItem  ( ) < 0 )
        m_deco_list-> setCurrentItem ( 0 );
    decoClicked ( m_original_deco );

    connect( m_deco_list, SIGNAL( highlighted(int) ), this, SLOT( decoClicked(int) ) );

    return tab;
}

QWidget *Appearance::createFontTab ( QWidget *parent, Config &cfg )
{
    QString familyStr = cfg. readEntry ( "FontFamily", "Helvetica" );
    QString styleStr = cfg. readEntry ( "FontStyle", "Regular" );
    int size = cfg. readNumEntry ( "FontSize", 10 );

    m_fontselect = new OFontSelector ( false, parent, "FontTab" );
    m_fontselect-> setSelectedFont ( familyStr, styleStr, size );
    QWhatsThis::add( m_fontselect, tr( "Select the desired name, style and size of the default font applications will use." ) );

    connect( m_fontselect, SIGNAL( fontSelected(const QFont&)),
             this, SLOT( fontClicked(const QFont&)));

    return m_fontselect;
}

QWidget *Appearance::createColorTab ( QWidget *parent, Config &cfg )
{
    QWidget *tab = new QWidget( parent, "ColorTab" );
    QGridLayout *gridLayout = new QGridLayout( tab, 0, 0, 3, 3 );
    gridLayout->setRowStretch ( 3, 10 );

    m_color_list = new QListBox ( tab );
    gridLayout->addMultiCellWidget ( m_color_list, 0, 3, 0, 0 );
    connect( m_color_list, SIGNAL( highlighted(int) ), this, SLOT( colorClicked(int) ) );
    QWhatsThis::add( m_color_list, tr( "Color schemes are a collection of colors which are used for various parts of the display.\n\nClick here to select an available scheme." ) );

    m_color_list-> insertItem ( new ColorListItem ( tr( "Current scheme" ), cfg ));

    QString path = QPEApplication::qpeDir ( );
    path.append( "/etc/colors/" );
    QStringList sl = QDir ( path ). entryList ( "*.scheme" );

    for ( QStringList::Iterator it = sl. begin ( ); it != sl. end ( ); ++it )
    {
        QString name = (*it). left ((*it). find ( ".scheme" ));
        QString pathstr = path;
        pathstr.append( *it );
        Config config ( pathstr, Config::File );
        config. setGroup ( "Colors" );

        m_color_list-> insertItem ( new ColorListItem ( name, config ));
    }

    m_color_list-> setCurrentItem ( 0 );

    QPushButton* tempButton = new QPushButton( tab, "editSchemeButton" );
    tempButton->setText( tr( "Edit..." ) );
    connect( tempButton, SIGNAL( clicked() ), this, SLOT( editSchemeClicked() ) );
    gridLayout->addWidget( tempButton, 0, 1 );
    QWhatsThis::add( tempButton, tr( "Click here to change the colors in the current color scheme." ) );

    tempButton = new QPushButton( tab, "deleteSchemeButton" );
    tempButton->setText( tr( "Delete" ) );
    connect( tempButton, SIGNAL( clicked() ), this, SLOT( deleteSchemeClicked() ) );
    gridLayout->addWidget( tempButton, 1, 1 );
    QWhatsThis::add( tempButton, tr( "Click here to delete the color scheme selected in the list to the left." ) );

    tempButton = new QPushButton( tab, "saveSchemeButton" );
    tempButton->setText( tr( "Save" ) );
    connect( tempButton, SIGNAL( clicked() ), this, SLOT( saveSchemeClicked() ) );
    gridLayout->addWidget( tempButton, 2, 1 );
    QWhatsThis::add( tempButton, tr( "Click here to name and save the current color scheme." ) );

    return tab;
}

QWidget *Appearance::createAdvancedTab ( QWidget *parent, Config &cfg )
{
    QWidget *tab = new QWidget ( parent );
    QVBoxLayout *vertLayout = new QVBoxLayout( tab, 3, 3 );

    /*
     * show scrollbars on the left?
     */
    m_leftHand = new QCheckBox( tr("Show Scrollbars on the left"), tab );
    m_leftHand->setChecked( cfg.readBoolEntry( "LeftHand", false ) );
    QWhatsThis::add( m_leftHand, tr( "Click here to display scrollbars on the left side instead of the right." ) );
    vertLayout->addWidget( m_leftHand );

    QFrame *f = new QFrame ( tab );
    f-> setFrameStyle ( QFrame::HLine | QFrame::Sunken );
    vertLayout-> addWidget ( f );
    vertLayout-> addSpacing ( 3 );


    QGridLayout* gridLayout = new QGridLayout ( vertLayout, 0, 0, 3, 0 );

    int style = cfg. readNumEntry ( "TabStyle", 2 ) - 1;
    bool tabtop = ( cfg. readEntry ( "TabPosition", "Top" ) == "Top" );

    QLabel* label = new QLabel( tr( "Tab style:" ), tab );
    gridLayout-> addWidget ( label, 0, 0 );
    QWhatsThis::add( label, tr( "Click here to select a desired style for tabbed dialogs (such as this application). The styles available are:\n\n1. Tabs - normal tabs with text labels only\n2. Tabs w/icons - tabs with icons for each tab, text label only appears on current tab\n3. Drop down list - a vertical listing of tabs\n4. Drop down list w/icons - a vertical listing of tabs with icons" ) );

    QButtonGroup* btngrp = new QButtonGroup( tab, "buttongroup" );
    btngrp-> hide ( );
    btngrp-> setExclusive ( true );

    m_tabstyle_list = new QComboBox ( false, tab, "tabstyle" );
    m_tabstyle_list-> insertItem ( tr( "Tabs" ));
    m_tabstyle_list-> insertItem ( tr( "Tabs w/icons" ));
    m_tabstyle_list-> insertItem ( tr( "Drop down list" ));
    m_tabstyle_list-> insertItem ( tr( "Drop down list w/icons" ));
    m_tabstyle_list-> setCurrentItem ( style );
    gridLayout-> addMultiCellWidget ( m_tabstyle_list, 0, 0, 1, 2 );
    QWhatsThis::add( m_tabstyle_list, tr( "Click here to select a desired style for tabbed dialogs (such as this application). The styles available are:\n\n1. Tabs - normal tabs with text labels only\n2. Tabs w/icons - tabs with icons for each tab, text label only appears on current tab\n3. Drop down list - a vertical listing of tabs\n4. Drop down list w/icons - a vertical listing of tabs with icons" ) );

    m_tabstyle_top = new QRadioButton( tr( "Top" ), tab, "tabpostop" );
    btngrp-> insert ( m_tabstyle_top );
    gridLayout-> addWidget( m_tabstyle_top, 1, 1 );
    QWhatsThis::add( m_tabstyle_top, tr( "Click here so that tabs appear at the top of the window." ) );

    m_tabstyle_bottom = new QRadioButton( tr( "Bottom" ), tab, "tabposbottom" );
    btngrp-> insert ( m_tabstyle_bottom );
    gridLayout-> addWidget( m_tabstyle_bottom, 1, 2 );
    QWhatsThis::add( m_tabstyle_bottom, tr( "Click here so that tabs appear at the bottom of the window." ) );

    m_tabstyle_top-> setChecked ( tabtop );
    m_tabstyle_bottom-> setChecked ( !tabtop );

    m_original_tabstyle = style;
    m_original_tabpos = tabtop;

    vertLayout-> addSpacing ( 3 );
    QHBoxLayout *rotLay = new QHBoxLayout ( vertLayout, 3 );

    QLabel* rotlabel = new QLabel( tr( "Rotation direction:" ), tab );
    m_rotdir_cw = new QRadioButton( tab, "rotdir_cw" );
    m_rotdir_ccw = new QRadioButton( tab, "rotdir_ccw" );
    m_rotdir_flip = new QRadioButton( tab, "rotdir_flip" );
    QButtonGroup* rotbtngrp = new QButtonGroup( tab, "rotbuttongroup" );

    rotbtngrp-> hide ( );
    rotbtngrp-> setExclusive ( true );
    rotbtngrp-> insert ( m_rotdir_cw );
    rotbtngrp-> insert ( m_rotdir_ccw );
    rotbtngrp-> insert ( m_rotdir_flip );

    QImage ccwImage = Opie::Core::OResource::loadImage( "redo", Opie::Core::OResource::SmallIcon );
    QPixmap cw, ccw, flip;
    cw.convertFromImage( ccwImage );
    ccw.convertFromImage( ccwImage.mirror( 1, 0 ) );
    flip.convertFromImage( Opie::Core::OResource::loadImage( "pass", Opie::Core::OResource::SmallIcon ) );

    m_rotdir_cw-> setPixmap( cw );
    m_rotdir_ccw-> setPixmap( ccw );
    m_rotdir_flip-> setPixmap( flip );

    rotLay-> addWidget ( rotlabel, 0 );
    rotLay-> addWidget ( m_rotdir_cw, 0 );
    rotLay-> addWidget ( m_rotdir_ccw, 0 );
    rotLay-> addWidget ( m_rotdir_flip, 0 );

    int rotDirection = cfg.readNumEntry( "rotatedir" );
    ODirection rot = CW;

    if (rotDirection == -1)
    {
        rot = ODevice::inst ( )-> direction ( );
    }
    else
    {
        rot = (ODirection)rotDirection;
    }

    m_rotdir_cw-> setChecked ( rot == CW );
    m_rotdir_ccw-> setChecked ( rot == CCW );
    m_rotdir_flip-> setChecked ( rot == Flip );

    QFrame *f2 = new QFrame ( tab );
    f2-> setFrameStyle ( QFrame::HLine | QFrame::Sunken );
    vertLayout-> addWidget ( f2 );
    vertLayout-> addSpacing ( 3 );

    QHBoxLayout *bigIconlay = new QHBoxLayout ( vertLayout, 3 );

    QLabel* label2 = new QLabel( tr( "&Big Icon size:" ), tab );
    bigIconlay-> addWidget ( label2, 0, 0 );

    m_bigIconSize = new QSpinBox(0, 128, 1, tab);
    m_bigIconSize->setValue(cfg.readNumEntry( "BigIconSize" ));
    bigIconlay->addWidget( m_bigIconSize );
    label2->setBuddy( m_bigIconSize );
    QWhatsThis::add( label2, tr( "Big Icon Size determines the size of the application icons in Launcher" ) );
    QWhatsThis::add( m_bigIconSize, tr( "Big Icon Size determines the size of the application icons in Launcher" ) );

    QHBoxLayout *smallIconlay = new QHBoxLayout ( vertLayout, 3 );

    QLabel* label3 = new QLabel( tr( "&Small Icon size:" ), tab );
    smallIconlay-> addWidget ( label3, 0, 0 );

    m_smallIconSize = new QSpinBox(0, 128, 1, tab);
    m_smallIconSize->setValue(cfg.readNumEntry( "SmallIconSize" ));
    smallIconlay->addWidget( m_smallIconSize );
    label3->setBuddy( m_smallIconSize );
    QWhatsThis::add( label3, tr( "Small Icon Size determines the size of many of the icons seen in applications (in menus, tab bars, tool bars, etc.), as well as the size of taskbar." ) );
    QWhatsThis::add( m_smallIconSize, tr( "Small Icon Size determines the size of many of the icons seen in applications (in menus, tab bars, tool bars, etc.), as well as the size of taskbar." ) );

    m_useBigPixmaps = new QCheckBox( tr("use Big &Pixmaps"), tab);
    m_useBigPixmaps->setChecked(cfg.readBoolEntry( "useBigPixmaps" ));
    vertLayout->addWidget( m_useBigPixmaps );
    QWhatsThis::add( m_useBigPixmaps, tr( "Enlarge toolbar pixmaps" ) );

    /*
     * add a spacing
     */
    vertLayout->addItem( new QSpacerItem( 1, 1, QSizePolicy::MinimumExpanding,  QSizePolicy::MinimumExpanding ) );
    return tab;
}


Appearance::Appearance( QWidget* parent,  const char* name, WFlags )
        : QDialog ( parent, name, true, WStyle_ContextHelp )
{
    setCaption( tr( "Appearance Settings" ) );

    Config config( "qpe" );
    config.setGroup( "Appearance" );

    QVBoxLayout *top = new QVBoxLayout ( this, 3, 3 );

    m_sample = new SampleWindow ( this );

    m_sample-> setDecoration ( new DefaultWindowDecoration ( ) );
    QWhatsThis::add( m_sample, tr( "This is a preview window.  Look here to see your new appearance as options are changed." ) );

    OTabWidget* tw = new OTabWidget ( this, "tabwidget", OTabWidget::Global, OTabWidget::Bottom );
    QWidget *styletab;

    m_color_list = 0;

    tw-> addTab ( styletab = createStyleTab ( tw, config ), "appearance", tr( "Style" ));
    tw-> addTab ( createFontTab ( tw, config ), "font", tr( "Font" ));
    tw-> addTab ( createColorTab ( tw, config ), "appearance/color", tr( "Colors" ) );
    tw-> addTab ( createDecoTab ( tw, config ), "appearance/deco", tr( "Windows" ) );
    tw-> addTab ( m_advtab = createAdvancedTab ( tw, config ), "SettingsIcon", tr( "Advanced" ) );

    top-> addWidget ( tw, 10 );
    top-> addWidget ( m_sample, 1 );

    tw-> setCurrentTab ( styletab );
    connect ( tw, SIGNAL( currentChanged(QWidget*)), this, SLOT( tabChanged(QWidget*)));

    m_style_changed = m_font_changed = m_color_changed = m_deco_changed = false;
}

Appearance::~Appearance()
{}

void Appearance::tabChanged ( QWidget *w )
{
    if ( w == m_advtab )
    {
        m_sample-> hide ( );
        updateGeometry ( ); // shouldn't be necessary ...
    }
    else
        m_sample-> show ( );
}

void Appearance::accept ( )
{
    bool newtabpos = m_tabstyle_top-> isChecked ( );
    int newtabstyle = m_tabstyle_list-> currentItem ( );

    Config config ( "qpe" );
    config. setGroup ( "Appearance" );

    if ( m_style_changed )
    {
        StyleListItem *item = (StyleListItem *) m_style_list-> item ( m_style_list-> currentItem ( ));
        if ( item )
            config.writeEntry( "Style", item-> key ( ));
    }

    if ( m_deco_changed )
    {
        DecoListItem *item = (DecoListItem *) m_deco_list-> item ( m_deco_list-> currentItem ( ));
        if ( item )
            config.writeEntry( "Decoration", item-> key ( ));
    }

    if (( newtabstyle != m_original_tabstyle ) || ( newtabpos != m_original_tabpos ))
    {
        config. writeEntry ( "TabStyle", newtabstyle + 1 );
        config. writeEntry ( "TabPosition", newtabpos ? "Top" : "Bottom" );
    }

    if ( m_font_changed )
    {
        config. writeEntry ( "FontFamily", m_fontselect-> fontFamily ( ));
        config. writeEntry ( "FontStyle", m_fontselect-> fontStyle ( ));
        config. writeEntry ( "FontSize", m_fontselect-> fontSize ( ));
    }


    if ( m_color_changed )
    {
        ColorListItem *item = (ColorListItem *) m_color_list-> item ( m_color_list-> currentItem ( ));

        if ( item )
            item-> save ( config );
    }

    ODirection rot;
    if (m_rotdir_ccw-> isChecked ( ))
    {
        rot = CCW;
    }
    else if (m_rotdir_cw-> isChecked ( ))
    {
        rot = CW;
    }
    else
    {
        rot = Flip;
    }
    config. writeEntry ( "rotatedir", (int)rot );

    config. writeEntry( "LeftHand", m_leftHand->isChecked() );

    config. writeEntry( "useBigPixmaps", m_useBigPixmaps->isChecked() );
    config. writeEntry( "BigIconSize", m_bigIconSize->value() );
    config. writeEntry( "SmallIconSize", m_smallIconSize->value() );

    config. write ( ); // need to flush the config info first
    Global::applyStyle ( );

    QDialog::accept ( );
}

void Appearance::done ( int r )
{
    QDialog::done ( r );
    close ( );
}


void Appearance::styleClicked ( int index )
{
    StyleListItem *sli = (StyleListItem *) m_style_list-> item ( index );
    m_style_settings-> setEnabled ( sli ? sli-> hasSettings ( ) : false );

    if ( m_sample && sli && sli-> style ( ))
    {
        int ci = m_color_list ? m_color_list-> currentItem ( ) : -1;

        m_sample-> setStyle2 ( sli-> style ( ), ci < 0 ? palette ( ) : ((ColorListItem *) m_color_list-> item ( ci ))-> palette ( ));
    }

    m_style_changed |= ( index != m_original_style );
}

void Appearance::styleSettingsClicked ( )
{
    StyleListItem *item = (StyleListItem *) m_style_list-> item ( m_style_list-> currentItem ( ));

    if ( item && item-> hasSettings ( ))
    {
        QDialog *d = new QDialog ( this, "SETTINGS-DLG", true );
        QVBoxLayout *vbox = new QVBoxLayout ( d, 3, 0 );

        QWidget *w = item-> settings ( d );

        if ( w )
        {
            vbox-> addWidget ( w );

            d-> setCaption ( w-> caption ( ));

            bool accepted = ( QPEApplication::execDialog ( d ) == QDialog::Accepted );

            if ( item-> setSettings ( accepted ))
                m_style_changed = true;
        }
        delete d;
    }
}

void Appearance::decoClicked ( int index )
{
    DecoListItem *dli = (DecoListItem *) m_deco_list-> item ( index );

    if ( m_sample )
    {
        if ( dli && dli-> interface ( ))
            m_sample-> setDecoration ( dli-> interface ( ));
        else
            m_sample-> setDecoration ( new DefaultWindowDecoration ( ));
        m_sample-> repaint ( );
    }
    m_deco_changed |= ( index != m_original_deco );
}

void Appearance::fontClicked ( const QFont &f )
{
    m_font_changed |= ( f != m_sample-> font ( ));
    m_sample-> setFont ( f );
}

void Appearance::colorClicked ( int index )
{
    ColorListItem *item = (ColorListItem *) m_color_list-> item ( index );

    if ( item )
        m_sample-> setPalette ( item-> palette ( ));

    m_color_changed |= ( item-> palette ( ) != qApp-> palette ( ));
}


void Appearance::editSchemeClicked ( )
{
    ColorListItem *item = (ColorListItem *) m_color_list-> item ( m_color_list-> currentItem ( ));

    int cnt = 0;
    QString labels [QColorGroup::NColorRoles];
    QColor colors [QColorGroup::NColorRoles];

    for ( int role = 0; role < (int) QColorGroup::NColorRoles; ++role )
    {
        QColor col = item->color( static_cast<QColorGroup::ColorRole>( role ) );

        if ( col. isValid ( ))
        {
            labels[cnt] = item->label( static_cast<QColorGroup::ColorRole>( role ) );
            colors[cnt] = col;

            cnt++;
        }
    }

    EditScheme* editdlg = new EditScheme( cnt, labels, colors, this, "editScheme", true );
    if ( QPEApplication::execDialog( editdlg ) == QDialog::Accepted )
    {
        ColorListItem *citem = (ColorListItem *) m_color_list-> item ( 0 );
        cnt = 0;

        for ( int role = 0; role < (int) QColorGroup::NColorRoles; ++role )
        {
            if ( item->color( static_cast<QColorGroup::ColorRole>( role ) ).isValid() )
            {
                citem->setColor( static_cast<QColorGroup::ColorRole>( role ), colors[cnt] );
                cnt++;
            }
        }

        m_color_list-> setCurrentItem ( 0 );
        colorClicked ( 0 );

        m_color_changed = true;
    }
    delete editdlg;
}


void Appearance::saveSchemeClicked()
{
    ColorListItem *item = (ColorListItem *) m_color_list-> item ( m_color_list-> currentItem ( ));

    if ( !item )
        return;

    QDialog *d = new QDialog ( this, 0, true );
    d-> setCaption ( tr( "Save Scheme" ));
    QLineEdit *ed = new QLineEdit ( d );
    ( new QVBoxLayout ( d, 3, 3 ))-> addWidget ( ed );
    ed-> setFocus ( );

    if ( d-> exec ( ) == QDialog::Accepted )
    {
        QString schemename = ed-> text ( );
        QString filestr = QPEApplication::qpeDir();
        filestr.append( "/etc/colors/" );
        filestr.append( schemename );
        filestr.append( ".scheme" );
        QFile file ( filestr );
        if ( !file. exists ( ))
        {
            QPalette p = item-> palette ( );

            Config config ( file.name(), Config::File );
            config. setGroup( "Colors" );

            item-> save ( config );

            config. write ( ); // need to flush the config info first

            m_color_list-> insertItem ( new ColorListItem ( schemename, config ));
        }
        else
        {
            QMessageBox::information ( this, tr( "Save scheme" ), tr( "Scheme does already exist." ));
        }
    }
    delete d;
}

void Appearance::deleteSchemeClicked()
{
    ColorListItem *item = (ColorListItem *) m_color_list-> item ( m_color_list-> currentItem ( ));

    if ( !item )
        return;

    if ( m_color_list-> currentItem ( ) > 0 )
    {
        if ( QPEMessageBox::confirmDelete ( this, tr( "Delete scheme" ), item-> text ( ) ) )
        {
            QString filestr = QPEApplication::qpeDir ( );
            filestr.append( "/etc/colors/" );
            filestr.append( item-> text ( ) );
            filestr.append( ".scheme" );
            QFile::remove ( filestr );
            delete item;
        }
    }
    else
    {
        QMessageBox::information( this, tr( "Delete scheme" ), tr( "Unable to delete current scheme." ));
    }
}
