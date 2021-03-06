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
#ifndef __QTREADERAPP_H
#define __QTREADERAPP_H

//#define _SCROLLPIPE
//#define __ISEARCH

//#define MAX_ENCODING 6
#define MAX_ACTIONS 5

#include <sys/timeb.h>
#include <qmainwindow.h>
#include "CExpander.h"
#include "CEncoding.h"
#include <qlist.h>
//#include <qpe/filemanager.h>
#include <qmap.h>
#include <qlineedit.h>
#include <qstack.h>
#include <qlistbox.h>
#ifdef USEQPE
#include <qpe/qpeapplication.h>
#endif
#include "orkey.h"
//#include "Queue.h"
#include "BGType.h"

class QBoxLayout;
class QWidgetStack;
class QToolButton;
class QPopupMenu;
class QToolBar;
#ifdef USEQPE
class QPEToolBar;
class QPEMenuBar;
#endif
class QScrollBar;
class CBkmkSelector;
class QProgressBar;
class QAction;
class CAnnoEdit;
class QFloatBar;
class CDrawBuffer;
class QTReader;
class QImage;
class Config;
class CButtonPrefs;

#ifdef USEQPE
enum ToolbarPolicy
{
    cesSingle = 0,
    cesMenuTool,
    cesMultiple
};
#else
enum ToolbarPolicy
{
    cesMenuTool = 0,
    cesMultiple
};
#endif

enum regedit_type
{
    cAutoGen,
    cAddBkmk,
    cJump,
    cMonoSpace,
    cSetTarget,
#ifdef _SCROLLPIPE
    cSetPipeTarget,
#endif
    cSetConfigName,
    cMargin,
    cExtraSpace,
    cExtraLead,
    cRepara
};

enum bkmk_action
{
    cOpenFile,
    cGotoBkmk,
    cDelBkmk,
    cRmBkmkFile,
    cLdConfig,
    cRmConfig,
    cExportLinks,
    cLdTheme
};

enum fontselector_action
{
    cChooseFont,
    cChooseEncoding
};

#ifdef __ISEARCH
struct searchrecord
{
  QString s;
  size_t pos;
  searchrecord(const QString& _s, size_t _pos) : s(_s), pos(_pos) {}
};
#endif

class infowin;
class GraphicWin;

class QTReaderApp : public QMainWindow
{
    Q_OBJECT

      QColor getcolour(int c);

      QMap<orKey, int> kmap;
    unsigned long m_savedpos;
    int m_debounce;
    bool m_kmapchanged;
    bground m_bgtype;
    timeb m_lastkeytime;
    QScrollBar* m_scrollbar;
    QScrollBar* scrollbar;
    int m_qtscroll, m_localscroll;
    bool m_hidebars, m_scrollishidden, m_statusishidden;
    QBoxLayout *m_layout;
    QLabel* m_prog;
    bool m_annoIsEditing;
    bool m_propogatefontchange, m_bFloatingDialog;
    bool m_url_clipboard, m_url_localfile, m_url_globalfile;
    CButtonPrefs* m_buttonprefs;
    fontselector_action m_fontAction;
    void doAction(QKeyEvent* e);

	public:
    QTReaderApp( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~QTReaderApp();

    void hideEvent(QHideEvent*)
	{
#ifdef USEQPE
	if (m_grabkeyboard)
	  {
	    ((QPEApplication*)qApp)->ungrabKeyboard();
	  }
#endif
	    suspend();
	}
#ifdef USEQPE
    void showEvent(QShowEvent*)
      {
	if (m_grabkeyboard)
	  {
	    ((QPEApplication*)qApp)->grabKeyboard();
	  }
      }
#endif
    void suspend();
    void openFile( const QString &, unsigned int loc = 0 );


 protected:
    void setfontHelper(const QString& lcn, int size = 0);
    QAction* m_bkmkAvail, *m_actFullscreen;
    CAnnoEdit* m_annoWin;
    Bkmk* m_anno;
    int m_scrollcolor, m_scrollbarcolor, m_background, m_foreground;
//    void resizeEvent(QResizeEvent* e);
    void closeEvent( QCloseEvent *e );
#if defined(USEQPE) && defined(USENEWFULLSCREEN)
    bool m_usenewfullscreen;
    void resizeEvent(QResizeEvent *);
    void focusInEvent(QFocusEvent*);
#endif
	void readbkmks();
	void do_mono(const QString&);
	void do_jump(const QString&);
	void do_reparastring(const QString&);
	void do_settarget(const QString&);
#ifdef _SCROLLPIPE
//	void do_setpipetarget(const QString&);
#endif
	void do_saveconfig(const QString&, bool);
	bool readconfig(const QString&, const QString&, bool);
	bool PopulateConfig(const char*, bool usedirs = false);
	ActionTypes ActNameToInt(const QString&);
#ifdef USEQPE
	bool m_grabkeyboard;
#endif
    bool m_doAnnotation;
    bool m_doDictionary;
    bool m_doOutput;
    bool m_doClipboard;
    bool m_fullscreen;
    bool m_loadedconfig;
 public:
	void saveprefs();
public slots:
      void setBackgroundBitmap();
  void UpdateStatus();
    void setScrollState(bool _b);
    void handlekey(QKeyEvent* e);
      void forceopen(const QString& filename);
    void setDocument(const QString&);
private slots:
#ifdef _SCRIPT
//    void RunScript();
#endif
  void actionscroll(int v);
    void SaveConfig();
 void LoadTheme();
 void LoadConfig();
 void TidyConfig();
 void ExportLinks();
    void zoomin();
    void zoomout();
    void chooseencoding();
    void setfullscreen(bool sfs);
    void setrotated(bool sfs);
    void setinverted(bool sfs);
    void setgrab(bool sfs);
//    void setcontinuous(bool sfs);
    void setTwoTouch(bool _b);
 void restoreFocus();
 void OnAnnotation(bool _b)
	{
	    m_doAnnotation = _b;
	}
    void OnDictionary(bool _b)
	{
	    m_doDictionary = _b;
	}
    void OnClipboard(bool _b)
	{
	    m_doClipboard = _b;
	}
    void OnWordSelected(const QString&, size_t, size_t, const QString&);
    void OnURLSelected(const QString& href, const size_t tgt);
    void showgraphic(QImage&);
    void addAnno(const QString&, const QString&, size_t, size_t);
    void addAnno(const QString&, const QString&);
    void addanno();
    void showAnnotation();
    void do_setencoding(int i);
	void do_setfont(const QString&);
	//	void buttonActionSelected(QAction*);
	void msgHandler(const QCString&, const QByteArray&);
	void monospace(bool);
	void jump();
	void reparastring();
	void settarget();
#ifdef _SCROLLPIPE
//	void setpipetarget();
//	void setpause(bool);
#endif
//	void setspacing();
	void setfont();
	void clearBkmkList();
	void listBkmkFiles();
	void editMark();
	void autoScroll(bool);
	void addbkmk();
	void savebkmks();
//	void importFiles();
	void showprefs();
	void showtoolbarprefs();
	void showbuttonprefs();
	void infoClose();
	//    void oldFile();
	void showinfo();

//    void indentplus();
//    void indentminus();

    void fileOpen();
    void fileClose();

    void editCopy();
    void editFind();

    void gotoStart();
    void gotoEnd();

    void pageup();
    void pagedn();

    void findNext();
    void findClose();

    void regClose();

#ifdef __ISEARCH
//  void search( const QString& );
#else
    void search();
#endif

    void showEditTools();

//    void stripcr(bool);
//    void setfulljust(bool);
//    void onespace(bool);
//    void repalm(bool);
//    void peanut(bool _b);
//    void remap(bool);
//    void embolden(bool);
//    void autofmt(bool);
//    void textfmt(bool);
//    void striphtml(bool);
//    void dehyphen(bool);
//    void depluck(bool);
//    void dejpluck(bool);
//    void unindent(bool);
//    void repara(bool);
//    void dblspce(bool);
    void pagemode(bool);
    //  void gotobkmk(const QString& bm);
    void gotobkmk(int);
    void cancelbkmk();
    void do_gotomark();
    void do_delmark();
    void do_autogen();
    void do_regaction();
    void OnRedraw();

 private:
    void setscrollcolour();
    void setscrollbarcolour();
    void writeUrl(const QString& file, const QString& href);
    QAction *m_preferences_action, *m_open_action, *m_close_action;
    QAction *m_info_action, *m_touch_action, *m_find_action, *m_start_action;
    QAction *m_end_action, *m_jump_action, *m_pageline_action;
    QAction *m_pageup_action, *m_pagedn_action, *m_back_action;
    QAction *m_home_action, *m_forward_action, *m_zoomin_action;
    QAction *m_zoomout_action, *m_setfont_action, *m_mark_action;
    QAction *m_annotate_action, *m_goto_action, *m_delete_action;
    QAction *m_autogen_action, *m_clear_action, *m_save_action;
    QAction *m_tidy_action, *m_startBlock_action, *m_endBlock_action;
    QAction *m_setenc_action, *m_setmono_action, *m_saveconfig_action;
    QAction *m_loadconfig_action, *m_loadtheme_action, *m_toolbarprefs_action, *m_tidyconfig_action;
    QAction *m_exportlinks_action, *m_rotate_action, *m_buttonprefs_action, *m_inverse_action;
    QAction *m_repara_action;
#ifdef USEQPE
    QAction *m_grab_action;
#endif
    void addtoolbars(Config* config);
    ToolbarPolicy m_tbpol, m_tbpolsave;
    ToolBarDock m_tbposition;
    bool m_tbmove, m_tbmovesave;
    QToolBar* filebar();
    QToolBar* viewbar();
    QToolBar* navbar();
    QToolBar* markbar();
    void hidetoolbars();
    void addfilebar(Config* _config, const QString& key, QAction* a);
    void addviewbar(Config* _config, const QString& key, QAction* a);
    void addnavbar(Config* _config, const QString& key, QAction* a);
    void addmarkbar(Config* _config, const QString& key, QAction* a);
    bool checkbar(Config* _config, const QString& key);
#ifdef _SCRIPT
    void SaveScript(const char* sname);
#endif
/*
    void setstate(unsigned char* _sd, unsigned short _sdlen);
    void getstate(unsigned char*& data, unsigned short& len);
*/
    void fileOpen2();
    void readfilelist();
    void savefilelist();
    void updatefileinfo();
    bool openfrombkmk(Bkmk*);
  QString m_targetapp, m_targetmsg, m_statusstring, m_themename;
    bool listbkmk(CList<Bkmk>*, const QString& _lab = QString::null, bool presel=false);
    QString usefilebrowser();
    void do_regedit();
    void colorChanged( const QColor &c );
    void clear();
    void updateCaption();
    void do_autogen(const QString&);
    void do_addbkmk(const QString&);
    bool findNextBookmark(size_t start);

 private:

    QAction* m_scrollButton;

    QAction* m_buttonAction[MAX_ACTIONS];

    CBkmkSelector* bkmkselector;

    //    ActionTypes m_spaceTarget, m_escapeTarget, m_returnTarget, m_leftTarget, m_rightTarget,
    //m_upTarget, m_downTarget;
    //bool m_leftScroll, m_rightScroll, m_upScroll, m_downScroll;
    bool m_bcloseDisabled, m_disableesckey;
    size_t searchStart;
#ifdef __ISEARCH
    QStack<searchrecord>* searchStack;
    bool dosearch(size_t start, CDrawBuffer& test, const QString& arg);
#else
    bool dosearch(size_t start, CDrawBuffer& test, const QRegExp& arg);
#endif
    QWidgetStack *editorStack;
    QTReader* reader;
    QComboBox* m_fontSelector;
//    QPEToolBar /* *menu,*/ *fileBar;
#if defined(USEQPE)
    QToolBar *menubar;
#endif
    QToolBar *fileBar, *navBar, *viewBar, *markBar;
#if defined(USEQPE)
    QPEMenuBar *mb;
#else
    QMenuBar *mb;
#endif
    QFloatBar *searchBar, *regBar/*, *m_fontBar*/;
    QToolBar /* *searchBar, *regBar,*/ *m_fontBar;
    QLineEdit *searchEdit, *regEdit;
    bool searchVisible;
    bool regVisible;
    bool m_fontVisible, m_twoTouch;
    static unsigned long m_uid;
    long unsigned get_unique_id() { return m_uid++; }
    /*
      void resizeEvent( QResizeEvent * r)
      {
//      qDebug("resize:(%u,%u)", r->oldSize().width(), r->oldSize().height());
//      qDebug("resize:(%u,%u)", r->size().width(), r->size().height());
      //    bgroup->move( width()-bgroup->width(), 0 );
      }
    */
    CList<Bkmk>* pBkmklist;
    CList<Bkmk>* pOpenlist;
    infowin*  m_infoWin;
    GraphicWin* m_graphicwin;
    QProgressBar* pbar;
    bool m_fBkmksChanged;
//    int m_nRegAction;
    regedit_type m_nRegAction;
    bkmk_action m_nBkmkAction;
    QString m_autogenstr;
    bool m_dontSave;
};

//const int cAutoGen = 0;
//const int cAddBkmk = 1;
//const int cDelBkmk = 2;
//const int cGotoBkmk = 3;
//const int cRmBkmkFile = 4;
//const int cJump = 5;
//const int cMonoSpace = 6;
//const int cOverlap = 7;
//const int cSetTarget = 8;
//const int cOpenFile = 9;
//const int cSetPipeTarget = 10;
//const int cSetConfigName = 11;
//const int cMargin = 12;
//const int cExtraSpace = 14;
//const int cExtraLead = 15;
//const int cGfxSize = 16;
//const int cChooseFont = 2;
//const int cChooseEncoding = 1;

#endif



