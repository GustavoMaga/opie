#ifndef OPIE_META_FACTORY_H
#define OPIE_META_FACTORY_H

/**
 * The MetaFactory is used to keep track of all IOLayers, FileTransferLayers and ConfigWidgets
 * and to instantiate these implementations on demand
 */

#include <qwidget.h>
#include <qmap.h>

#include <qpe/config.h>

#include "io_layer.h"
#include "file_layer.h"
#include "profile.h"

class MetaFactory {
public:
    typedef QWidget* (*configWidget)(QWidget* parent);
    typedef IOLayer* (*iolayer)(const Profile& );
    typedef FileTransferLayer* (*filelayer)(IOLayer*);

    MetaFactory();
    ~MetaFactory();

    void addConfigWidgetFactory( const QString&,
                                 configWidget );
    void addIOLayerFactory(const QString&,
                           iolayer );
    void addFileTransferLayer( const QString&,
                               filelayer );
    QStringList ioLayers()const;
    QStringList configWidgets()const;
    QStringList fileTransferLayers()const;
    IOLayer* newIOLayer( const QString&,const Profile& );

private:
    QMap<QString, configWidget> m_confFact;
    QMap<QString, iolayer> m_layerFact;
    QMap<QString, filelayer> m_fileFact;
};


#endif
