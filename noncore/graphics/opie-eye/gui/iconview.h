/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#ifndef PHUNK_ICON_VIEW_H
#define PHUNK_ICON_VIEW_H

#include <qvbox.h>


class QIconView;
class QIconViewItem;
class QComboBox;
class PIconViewItem;
class PDirLister;
class Ir;

namespace Opie {
namespace Core{
    class OConfig;
    class OKeyConfigManager;
}
}

class PIconView : public QVBox {
    Q_OBJECT
    friend class PIconViewItem;
    enum ActionIds {
        BeamItem, DeleteItem, ViewItem, InfoItem,SlideItem
    };
public:
    PIconView( QWidget* wid, Opie::Core::OConfig *cfg );
    ~PIconView();
    void resetView();
    Opie::Core::OKeyConfigManager* manager();

    void setDoccalled(bool);

    static const int sMAX_ICON_SIZE;
    static const int sMIN_ICON_SIZE;
    static const int sDEF_ICON_SIZE;

signals:
    void sig_showInfo( const QString& );
    void sig_display(const QString&);
    void sig_startslide(int timeout);

public slots:
    virtual void slotShowNext();
    virtual void slotShowPrev();
    virtual void slotShowLast();
    virtual bool slotShowFirst();
    virtual void polish();

protected:
    void resizeEvent( QResizeEvent* );

private:
    void initKeys();
    QString currentFileName(bool &isDir)const;
    QString nextFileName(bool &isDir)const;
    QString prevFileName(bool &isDir)const;
    void loadViews();
    void calculateGrid(QResizeEvent*e = 0);
    bool m_setDocCalled:1;

private slots:
    void slotDirUp();
    void slotChangeDir(const QString&);
    void slotTrash();
    void slotViewChanged( int );
    void slotReloadDir();
    void slotRename();
    void slotBeam();
    void slotBeamDone( Ir* );

    void slotShowImage();
    void slotShowImage( const QString& );
    void slotImageInfo();
    void slotImageInfo( const QString& );
    void slotStartSlide();

    void slotStart();
    void slotEnd();

/* for performance reasons make it inline in the future */
    void addFolders(  const QStringList& );
    void addFiles( const QStringList& );
    void slotClicked(QIconViewItem* );
    void slotRetrun(QIconViewItem* );

/**/
    void slotThumbInfo(const QString&, const QString&);
    void slotThumbNail(const QString&, const QPixmap&);

    void  slotChangeMode( int );
private:
    Opie::Core::OKeyConfigManager *m_viewManager;
    Opie::Core::OConfig *m_cfg;
    QComboBox* m_views;
    QIconView* m_view;
    QString m_path;
    bool m_updatet : 1;
    int m_mode;
    bool m_internalReset:1;
    int m_iconsize;
    QHBox*m_hbox;
    QWidget*m_customWidget;
};

#endif
