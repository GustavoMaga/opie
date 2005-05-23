/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */
#include <stdlib.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>

#include <opie2/oresource.h>

#include <qpe/storage.h>


#include "filesystem.h"

PFileSystem::PFileSystem(  QToolBar* bar)
    : QToolButton( bar )
{
    setIconSet( Opie::Core::OResource::loadPixmap( "cardmon/pcmcia", Opie::Core::OResource::SmallIcon ) );

    m_pop = new QPopupMenu( this );
    connect( m_pop, SIGNAL( activated( int ) ),
             this, SLOT(slotSelectDir( int ) ) );

    m_storage = new StorageInfo();
    connect(m_storage, SIGNAL(disksChanged() ),
            this, SLOT( changed() ) );
    changed();

    connect(this,SIGNAL(pressed()),SLOT(slotPopUp()));
}

PFileSystem::~PFileSystem() {
    delete m_storage;
}


void PFileSystem::changed() {
    m_pop->clear();
    m_dev.clear();

    /* home dir, too */
    QString f = getenv( "HOME" );
    if (!f.isEmpty()) {
        m_dev.insert("Home directory",f);
        m_pop->insertItem("Home directory");
    }

    const QList<FileSystem> &fs = m_storage->fileSystems();
    QListIterator<FileSystem> it(fs );
    for ( ; it.current(); ++it ) {
        const QString disk = (*it)->name();
        const QString path = (*it)->path();
        m_dev.insert( disk, path );
        m_pop->insertItem( disk );
    }
}

void PFileSystem::slotPopUp() {
    m_pop->exec(QCursor::pos());
    setDown(false);
}

void PFileSystem::slotSelectDir( int id ) {
    emit changeDir( m_dev[m_pop->text(id )] );
}
