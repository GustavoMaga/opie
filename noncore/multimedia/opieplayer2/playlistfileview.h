#ifndef PLAYLISTFILEVIEW_H
#define PLAYLISTFILEVIEW_H

#include "playlistview.h"

#include <qpe/applnk.h>

class PlayListFileView : public PlayListView
{
    Q_OBJECT
public:
    PlayListFileView( const QString &mimeTypePattern, const QString &itemPixmapName, QWidget *parent, const char *name = 0 );
    virtual ~PlayListFileView();

    bool hasSelection() const;

public slots:
    void scanFiles();
    void populateView();

signals:
    void itemsSelected( bool selected );

private slots:
    void checkSelection();

private:
    void fileSize(unsigned long size, QString &str);
    QString m_mimeTypePattern;
    QString m_itemPixmapName;
    DocLnkSet m_files;
    bool m_scannedFiles;
    bool m_viewPopulated;
};

#endif // PLAYLISTFILEVIEW_H
/* vim: et sw=4 ts=4
 */
