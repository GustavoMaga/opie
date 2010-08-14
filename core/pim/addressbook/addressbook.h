/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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
#ifndef Addressbook_H
#define Addressbook_H

// Remove this for OPIE releae 1.0 !
// #define __DEBUG_RELEASE

#include <opie2/opimmainwindow.h>

#include <qvaluelist.h>
#include <qstringlist.h>
#include <qlineedit.h>
#include "ofloatbar.h"
#include "abview.h"
#include "abconfig.h"

class ContactEditor;
class AbLabel;
class AbTable;
class QToolBar;
class QPopupMenu;
class QToolButton;
class QDialog;
class Ir;
class QAction;
class LetterPicker;
namespace Opie {
    class OPimRecord;
}

class AddressbookWindow: public Opie::OPimMainWindow
{
    Q_OBJECT
public:
    enum EntryMode { NewEntry=0, EditEntry };

    static QString appName() { return QString::fromLatin1("addressbook"); }
    AddressbookWindow( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~AddressbookWindow();

protected:
    void resizeEvent( QResizeEvent * e );

    void editPersonal();
    void editEntry( EntryMode );
    void closeEvent( QCloseEvent *e );
    bool save();

public slots:
    void flush();
    void reload();
    void appMessage(const QCString &, const QByteArray &);
    void setDocument( const QString & );  // Called by obex receiver and internally!
#ifdef __DEBUG_RELEASE
    // void slotSave();
#endif

protected slots:
    int  create();
    bool remove( int uid );
    void beam( int uid );
    void show( int uid );
    void edit( int uid );
    void add( const Opie::OPimRecord& );

private slots:
    void slotItemNew();
    void slotItemEdit();
    void slotItemDuplicate();
    void slotItemDelete();
    void slotItemBeam();
    void slotItemFind();
    void slotConfigure();

    void importvCard();
    void exportvCard();
    void slotViewBack();
    void slotPersonalView();
    void writeMail();
    void beamDone( Ir * );
    void slotSetCategory( const QString &category );
    void slotSetLetter( char );
    void slotSetFont(int);

    void slotShowFind( bool show );
    void slotFind();
    void slotNotFound();
    void slotWrapAround();

    void slotViewSwitched( int );
    void slotListView();
    void slotCardView();

private:
    QToolBar *listTools;
    enum Panes { paneList=0, paneView, paneEdit };
    ContactEditor *abEditor;
    LetterPicker *pLabel;
    AbView* m_abView;
    QWidget *m_listContainer;

    // Searching stuff
    OFloatBar* m_searchBar;
    QLineEdit* m_searchEdit;

    QAction *m_actionPersonal, *m_actionMail;

    int viewMargin;

    bool syncing;
    QFont *defaultFont;
    int m_curFontSize;

    bool isLoading;

    AbConfig m_config;

    int active_view;
    bool m_forceClose;
};

#endif
