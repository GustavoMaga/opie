#include "olister.h"
#include "ofileview.h"
#include "ofileselector.h"


OLister::OLister( OFileSelector* view)
    : m_view( view )
{
}
OLister::~OLister() {
}
bool OLister::showFiles()const {
    return m_view->showFiles();
}
bool OLister::showDirs()const {
    return m_view->showDirs();
}
void OLister::addFile( const QString& mine,
                       QFileInfo* info,
                       bool isSymlink ) {
    view()->currentView()->addFile( mine,
                                    info,
                                    isSymlink );
}
void OLister::addFile( const QString& mine,
                       const QString& path,
                       const QString& file,
                       bool isSymlink ) {
    view()->currentView()->addFile( mine,
                                    path,
                                    file,
                                    isSymlink );
}
void OLister::addDir( const QString& mine,
                      QFileInfo* info,
                      bool isSymlink  ) {

    view()->currentView()->addDir( mine,
                                   info,
                                   isSymlink );
}
void OLister::addDir( const QString& mine,
                      const QString& path,
                      const QString& dir,
                      bool isSymlink ) {
    view()->currentView()->addDir( mine,
                                   path,
                                   dir,
                                   isSymlink );
}
void OLister::addSymlink( const QString& mine,
                          QFileInfo* info,
                          bool isSymlink ) {
    view()->currentView()->addSymlink( mine,
                                       info,
                                       isSymlink );
}
void OLister::addSymlink( const QString& mine,
                          const QString& path,
                          const QString& name,
                          bool isSymlink ) {
    view()->currentView()->addSymlink( mine,
                                       path,
                                       name,
                                       isSymlink );
}
OFileSelector* OLister::view() {
    return m_view;
}
bool OLister::compliesMime( const QString& mime ) {
    return view()->compliesMime( mime );
}
