#ifndef OPIE_EXAMPLE_NETWORK_MODULE_H
#define OPIE_EXAMPLE_NETWORK_MODULE_H

#include <interfaces/module.h>

class VirtualModule : Module {

signals:
    void updateInterface(Interface* i );

public:
    VirtualModule();
    ~VirtualModule();

    const QString type() {return QString::fromLatin1("vpn" );}
    void setProfile( const QString& ) {}
    bool isOwner( Interface* );
    QWidget *configure( Interface* );
    QWidget *information(  Interface* );
    QList<Interface> getInterfaces();
    void possibleNewInterfaces( QMap<QString, QString>& );
    Interface *addNewInterface( const QString& );
    bool remove( Interface* iface );
    QString getPixmapName( Interface*  ) {return QString::fromLatin1("Tux"); }
    void receive( const QCString& msg, const QByteArray& ar ) { Q_CONST_UNUSED(msg) Q_CONST_UNUSED(ar) } // don't listen
private:
    QList<Interface> m_interfaces;
};

extern "C" {
    void* create_plugin() {
        return new VirtualModule();
    }
};

#endif
