/*
� � � � � � � �             This file is part of the Opie Project

� � � � � � �                Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
                             Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
              =.
            .=l.
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This program is free software; you can
.> <`_, � > �. � <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
� � .%`+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This program is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful,  but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|`    MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.= � � � = � � � ;      General Public License for more
++= � -. � � .` � � .:       details.
�: � � = �...= . :.=-
�-. � .:....=;==+<;          You should have received a copy of the GNU
� -_. . . � )=. �=           General Public License along with
� � -- � � � �:-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef PLAY_LIST_WIDGET_H
#define PLAY_LIST_WIDGET_H

#include <qmainwindow.h>
#include <qpe/applnk.h>
#include <qtabwidget.h>
#include <qpe/fileselector.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>

#include "playlistwidgetgui.h"


//class PlayListWidgetPrivate;
class Config;
class QListViewItem;
class QListView;
class QPoint;
class QAction;
class QLabel;

class PlayListWidget : public PlayListWidgetGui {
    Q_OBJECT
public:
    enum TabType { CurrentPlayList, AudioFiles, VideoFiles, PlayLists };
    enum { TabTypeCount = 4 };

    PlayListWidget( QWidget* parent=0, const char* name=0, WFlags fl=0 );
    ~PlayListWidget();
    DocLnkSet files;
    DocLnkSet vFiles;
    bool fromSetDocument;
    bool insanityBool;
    QString setDocFileRef, currentPlayList;
    // retrieve the current playlist entry (media file link)
    const DocLnk *current();
    void useSelectedDocument();
    int selected;
    TabType currentTab() const;

public slots:
    bool first();
    bool last();
    bool next();
    bool prev();
    void writeDefaultPlaylist( );
    QString currentFileListPathName() const;
protected:
    void keyReleaseEvent( QKeyEvent *e);

private:
    int defaultSkinIndex;
    bool audioScan, videoScan, audioPopulated, videoPopulated;
    void readm3u(const QString &);
    void readPls(const QString &);
    void initializeStates();
    void populateAudioView();
    void populateVideoView();

    QListView *currentFileListView() const;

    bool inFileListMode() const;

private slots:
    void populateSkinsMenu();
    void skinsMenuActivated(int);
    void pmViewActivated(int);
    void writem3u();
    void writeCurrentM3u();
    void scanForAudio();
    void scanForVideo();
    void openFile();
    void setDocument( const QString& fileref );
    void addToSelection( const DocLnk& ); // Add a media file to the playlist
    void addToSelection( QListViewItem* ); // Add a media file to the playlist
    void setPlaylist( bool ); // Show/Hide the playlist
    void clearList();
    void addAllToList();
    void addAllMusicToList();
    void addAllVideoToList();
    void saveList();  // Save the playlist
    void loadList( const DocLnk &);  // Load a playlist
    void playIt( QListViewItem *);
    void btnPlay(bool);
    void deletePlaylist();
    void addSelected();
    void removeSelected();
    void tabChanged(QWidget*);
    void viewPressed( int, QListViewItem *, const QPoint&, int);
    void playlistViewPressed( int, QListViewItem *, const QPoint&, int);
    void playSelected();
};

#endif // PLAY_LIST_WIDGET_H

