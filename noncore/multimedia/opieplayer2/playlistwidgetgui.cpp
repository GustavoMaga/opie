/*
                    This file is part of the Opie Project

                      Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
                             Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
              =.
            .=l.
     .>+-=
_;:,   .>  :=|.         This program is free software; you can
.> <`_,  > .  <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This program is distributed in the hope that
  + . -:.    =       it will be useful,  but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=    =    ;      General Public License for more
++=  -.   .`   .:       details.
:   = ...= . :.=-
-.  .:....=;==+<;          You should have received a copy of the GNU
 -_. . .  )=. =           General Public License along with
  --    :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qtoolbar.h>
#include <qpe/qpeapplication.h>

#include <qlayout.h>
#include <qmenubar.h>

#include "playlistselection.h"
#include "playlistwidget.h"
#include "mediaplayerstate.h"
#include "inputDialog.h"
#include "playlistfileview.h"


PlayListWidgetGui::PlayListWidgetGui(QWidget* parent, const char* name )
    : QMainWindow( parent, name )  {

    d = new PlayListWidgetPrivate;
    d->setDocumentUsed = FALSE;

    setBackgroundMode( PaletteButton );
    setToolBarsMovable( FALSE );

    // Create Toolbar
    QToolBar *toolbar = new QToolBar( this );
    toolbar->setHorizontalStretchable( TRUE );

    // Create Menubar
    QMenuBar *menu = new QMenuBar( toolbar );
    menu->setMargin( 0 );

    bar = new QToolBar( this );
    bar->setLabel( tr( "Play Operations" ) );

    tbDeletePlaylist = new QPushButton( Opie::Core::OResource::loadIconSet( "trash", Opie::Core::OResource::SmallIcon ),
                                        "", bar, "close" );
    tbDeletePlaylist->setFlat( TRUE );
    tbDeletePlaylist->setFixedSize( 20, 20 );

    tbDeletePlaylist->hide();

    pmPlayList = new QPopupMenu( this );
    menu->insertItem( tr( "File" ), pmPlayList );

    pmView = new QPopupMenu( this );
    menu->insertItem( tr( "View" ), pmView );
    pmView->isCheckable();

    skinsMenu = new QPopupMenu( this );
    pmView->insertItem( tr( "Skins" ), skinsMenu );
    skinsMenu->isCheckable();

    gammaMenu = new QPopupMenu( this );
    pmView->insertItem( tr( "Gamma (Video)" ),  gammaMenu );

    gammaSlider = new QSlider( QSlider::Vertical,  gammaMenu );
    gammaSlider->setRange( -40, 40 );
    gammaSlider->setTickmarks( QSlider::Left );
    gammaSlider->setTickInterval( 20 );
    gammaSlider->setFocusPolicy( QWidget::StrongFocus );
    gammaSlider->setValue( 0 );
    gammaSlider->setMinimumHeight( 50 );

    gammaLCD = new QLCDNumber( 3,  gammaMenu );
    gammaLCD-> setFrameShape ( QFrame::NoFrame );
    gammaLCD-> setSegmentStyle ( QLCDNumber::Flat );

    gammaMenu->insertItem( gammaSlider );
    gammaMenu->insertItem( gammaLCD );

    connect( gammaSlider, SIGNAL( valueChanged(int) ), gammaLCD, SLOT( display(int) ) );

    vbox5 = new QVBox( this );
    QVBox *vbox4 = new QVBox( vbox5 );
    QHBox *hbox6 = new QHBox( vbox4 );

    tabWidget = new QTabWidget( hbox6, "tabWidget" );

    playListTab = new QWidget( tabWidget, "PlayListTab" );
    tabWidget->insertTab( playListTab, "Playlist");

    QGridLayout *Playout = new QGridLayout( playListTab );
    Playout->setSpacing( 2);
    Playout->setMargin( 2);

    // Add the playlist area
    QVBox *vbox3 = new QVBox( playListTab );
    d->playListFrame = vbox3;

    QHBox *hbox2 = new QHBox( vbox3 );
    d->selectedFiles = new PlayListSelection( hbox2 );

    vbox1 = new QVBox( hbox2 );
    QPEApplication::setStylusOperation( d->selectedFiles->viewport(), QPEApplication::RightOnHold );
    QVBox *stretch1 = new QVBox( vbox1 ); // add stretch

    Playout->addMultiCellWidget( vbox3, 0, 0, 0, 1 );

    QWidget *aTab;
    aTab = new QWidget( tabWidget, "aTab" );

    QGridLayout *Alayout = new QGridLayout( aTab );
    Alayout->setSpacing( 2 );
    Alayout->setMargin( 2 );
    // no m3u's here please
    audioView = new PlayListFileView( "audio/mpeg;audio/x-wav;application/ogg", "opieplayer2/musicfile", aTab, "Audioview" );
    Alayout->addMultiCellWidget( audioView, 0, 0, 0, 1 );
    tabWidget->insertTab( aTab, tr( "Audio" ) );

    QPEApplication::setStylusOperation( audioView->viewport(), QPEApplication::RightOnHold );

    QWidget *vTab;
    vTab = new QWidget( tabWidget, "vTab" );

    QGridLayout *Vlayout = new QGridLayout( vTab );
    Vlayout->setSpacing( 2 );
    Vlayout->setMargin( 2 );
    videoView = new PlayListFileView( "video/*", "opieplayer2/videofile", vTab, "Videoview" );
    Vlayout->addMultiCellWidget( videoView, 0, 0, 0, 1 );

    QPEApplication::setStylusOperation( videoView->viewport(), QPEApplication::RightOnHold );

    tabWidget->insertTab( vTab, tr( "Video" ) );

    //playlists list
    QWidget *LTab;
    LTab = new QWidget( tabWidget, "LTab" );
    QGridLayout *Llayout = new QGridLayout( LTab );
    Llayout->setSpacing( 2 );
    Llayout->setMargin( 2 );

    playLists = new FileSelector( "playlist/plain;audio/x-mpegurl", LTab, "fileselector" , FALSE, FALSE );
    Llayout->addMultiCellWidget( playLists, 0, 0, 0, 1 );

    tabWidget->insertTab( LTab, tr( "Lists" ) );

    setCentralWidget( vbox5 );
}



PlayListWidgetGui::~PlayListWidgetGui()  {
}

void PlayListWidgetGui::setView( char view )  {
    if ( view == 'l' )
        showMaximized();
    else
        hide();
}


void PlayListWidgetGui::setActiveWindow()  {
    // When we get raised we need to ensure that it switches views
    MediaPlayerState::DisplayType origDisplayType = mediaPlayerState->displayType();
    mediaPlayerState->setDisplayType( MediaPlayerState::MediaSelection ); // invalidate
    mediaPlayerState->setDisplayType( origDisplayType ); // now switch back
}


