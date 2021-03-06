#include "metafactory.h"

MetaFactory::MetaFactory() {
}
MetaFactory::~MetaFactory() {

}
void MetaFactory::addConnectionWidgetFactory( const QCString& name,
                                              const QString& str,
                                              configWidget wid) {
    m_strings.insert( str, name );
    m_conFact.insert( str, wid );
}
void MetaFactory::addTerminalWidgetFactory( const QCString& name,
                                            const QString& str,
                                            configWidget wid ) {
    m_strings.insert( str, name );
    m_termFact.insert( str, wid );
}
void MetaFactory::addKeyboardWidgetFactory( const QCString& name,
                                            const QString & str,
                                            configWidget wid) {
    m_strings.insert( str, name );
    m_keyFact.insert( str, wid );

}
void MetaFactory::addIOLayerFactory( const QCString& name,
                                     const QString& str,
                                     iolayer lay) {
    m_strings.insert( str, name );
    m_layerFact.insert( str, lay );
}
void MetaFactory::addFileTransferLayer( const QCString& name,
                                        const QString& str,
                                        filelayer lay) {
    m_strings.insert(str, name );
    m_fileFact.insert( str, lay );
}
void MetaFactory::addReceiveLayer( const QCString& name,
                                   const QString& str,
                                   receivelayer lay) {
    m_strings.insert(str, name );
    m_receiveFact.insert( str, lay );
}
QStringList MetaFactory::ioLayers()const {
    QStringList list;
    QMap<QString, iolayer>::ConstIterator it;
    for (it = m_layerFact.begin(); it != m_layerFact.end(); ++it ) {
        list << it.key();
    }
    return list;
}
QStringList MetaFactory::connectionWidgets()const {
    QStringList list;
    QMap<QString,  configWidget>::ConstIterator it;
    for ( it = m_conFact.begin(); it != m_conFact.end(); ++it ) {
        list << it.key();
    }
    return list;
}
QStringList MetaFactory::terminalWidgets()const {
    QStringList list;
    QMap<QString,  configWidget>::ConstIterator it;
    for ( it = m_termFact.begin(); it != m_termFact.end(); ++it ) {
        list << it.key();
    }
    return list;
}
QStringList MetaFactory::fileTransferLayers()const {
    QStringList list;
    QMap<QString, filelayer>::ConstIterator it;
    for ( it = m_fileFact.begin(); it != m_fileFact.end(); ++it ) {
        list << it.key();
    }
    return list;
}
QStringList MetaFactory::receiveLayers()const {
    QStringList list;
    QMap<QString, receivelayer>::ConstIterator it;
    for ( it = m_receiveFact.begin(); it != m_receiveFact.end(); ++it ) {
        list << it.key();
    }
    return list;
}

IOLayer* MetaFactory::newIOLayer( const QString& str,const Profile& prof ) {
    IOLayer* lay = 0l;

    QMap<QString, iolayer>::Iterator it;
    it = m_layerFact.find( str );
    if ( it != m_layerFact.end() ) {
        lay = (*(it.data()))(prof);
        /*
        iolayer laye = it.data();
        lay = (*laye )(conf);*/
    }

    return lay;
}

ProfileDialogWidget *MetaFactory::newConnectionPlugin ( const QString& str, QWidget *parent) {
    ProfileDialogWidget* wid = 0l;

    QMap<QString, configWidget>::Iterator it;
    it = m_conFact.find( str );
    if ( it != m_conFact.end() ) {
        wid = (*(it.data() ) )(str,parent);
    }
    return wid;
}
ProfileDialogWidget *MetaFactory::newTerminalPlugin( const QString& str, QWidget *parent) {
    if (str.isEmpty() )
        return 0l;
    ProfileDialogWidget* wid = 0l;

    QMap<QString, configWidget>::Iterator it;
    it = m_termFact.find( str );
    if ( it != m_termFact.end() ) {
        wid = (*(it.data() ) )(str,parent);
    }
    return wid;
}
ProfileDialogWidget *MetaFactory::newKeyboardPlugin( const QString& str, QWidget *parent) {

    if (str.isEmpty() )
        return 0l;
    ProfileDialogWidget* wid = 0l;

    QMap<QString, configWidget>::Iterator it;
    it = m_keyFact.find( str );
    if ( it != m_keyFact.end() ) {
        wid = (*(it.data() ) )(str,parent);
    }
    return wid;
}
FileTransferLayer* MetaFactory::newFileTransfer(const QString& str, IOLayer* lay ) {
    FileTransferLayer* file = 0l;
    QMap<QString, filelayer>::Iterator it;
    it = m_fileFact.find( str );
    if ( it != m_fileFact.end() ) {
        file = (*(it.data() ) )(lay);
    }
    return file;
}
ReceiveLayer* MetaFactory::newReceive(const QString& str, IOLayer* lay ) {
    ReceiveLayer* file = 0l;
    QMap<QString, receivelayer>::Iterator it;
    it = m_receiveFact.find( str );
    if ( it != m_receiveFact.end() ) {
        file = (*(it.data() ) )(lay);
    }
    return file;
}
QCString MetaFactory::internal( const QString& str )const {
    return m_strings[str];
}
QString MetaFactory::external( const QCString& str )const {
    QMap<QString, QCString>::ConstIterator it;
    for ( it = m_strings.begin(); it != m_strings.end(); ++it ) {
        if ( it.data() == str )
            return it.key();
    }
    return QString::null;
}
