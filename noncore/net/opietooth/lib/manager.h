
#ifndef OpieToothManager_H
#define OpieToothManager_H

#include <qobject.h>
#include <qstring.h>
#include <qmap.h>
#include <qvaluelist.h>

#include "connection.h"
#include "remotedevice.h"
#include "services.h"

namespace Opie {namespace Core {class OProcess;}}
namespace OpieTooth {
    class Device;
    /** Manager manages a blueZ device (hci0 for example)
     *  without Manager you can control the things you
     *  could do from command line in a OO and asynchronus
     *  way.
     */
    class Manager : public QObject {
        Q_OBJECT
    public:
        /** c'tor whichs create a new Manager
         *  @param device is the device to use. Either a mac or blueZ device name
         *
         */
        Manager( const QString &device );

        /** c'tor
         * @param dev The Device to be managed
         * We don't care of Device so you need to delete it
         */
        Manager( Device* dev );

        /**
         * c'tor
         */
        Manager();

        ~Manager();

        /** Set the manager to control a new device
         *  @param device the new device to control (hci0 )
         */
        void setDevice( const QString& device );

        /**
         * Convience functions for setting a new device
         */
        void setDevice( Device *dev );

        /**
         * Wether or not a device is connected. The function
         * is asynchron
         * If device is empty it will take the currently managed
         * device and see if it's up
         * for Remote devices it will ping and see.
         * @param either mac or hciX
         */
        void isAvailable(const QString& device= QString::null );

        /**
         * same as above
         */
        void isAvailable(Device *dev );

        /** this searchs for devices reachable from the
         *  currently managed device
         *  or from device if @param device is not empty
         */
        void searchDevices(const QString& device= QString::null );
        /** same as above
         *
         */
        void searchDevices(Device *d );

        /**
         * This will add the service @param name
         * to the sdpd daemon
         * It will start the daemon if necessary
         */
        void addService(const QString &name );

        /**
         * This will add the services @param names
         * to the sdpd daemon
         * It will start the daemon if necessary
         */
        void addServices( const QStringList& names );

        /**
         * This removes a service from the sdps
         */
        void removeService(const QString &name );

        /**
         * Removes a list from the sdpd
         */
        void removeServices(const QStringList& );

        /**
         * search for services on a remote device
         *
         */
        void searchServices( const QString& remDevice, bool userecords=false );

        /**
         * search for services on a remote device
         */
        void searchServices( const RemoteDevice& dev, bool userecords=false );

        /**
         * Starts to connect to the device
         * in @param
         */
        void connectTo(const QString& );

        /**
         * Searches for active connections
         * the result is emitted with the
         * connections signal
         */
        void searchConnections();

        void signalStrength( const QString &mac );
//// not implemented yet
        /*static*/ QString toDevice( const QString& mac );
        /*static*/ QString toMac( const QString &device );
//// not implemented yet over

    signals:
        // device either mac or dev name
        // the first device is the device which you access
        void available( const QString& device, bool connected );
        void addedService(  const QString& service, bool added );
        void removedService( const QString& service, bool removed );
        void foundServices( const QString& device, Services::ValueList );
        void foundDevices( const QString& device, RemoteDevice::ValueList );
        void connections( ConnectionState::ValueList );
        void signalStrength( const QString& mac, const QString& strengh );

    private slots:
        void slotProcessExited(Opie::Core::OProcess* );

        void slotSDPExited(Opie::Core::OProcess*);
        void slotSDPOut(Opie::Core::OProcess*, char*, int);

        void slotHCIExited(Opie::Core::OProcess* );
        void slotHCIOut(Opie::Core::OProcess*, char*, int );

        void slotConnectionExited(Opie::Core::OProcess* );
        void slotConnectionOutput(Opie::Core::OProcess*, char*, int );

        void slotSignalStrengthOutput( Opie::Core::OProcess*, char*, int );
        void slotSignalStrengthExited( Opie::Core::OProcess* );
    private:
        Services::ValueList parseSDPOutput( const QString& );
        RemoteDevice::ValueList parseHCIOutput( const QString& );
        ConnectionState::ValueList parseConnections( const QString& );
        Opie::Core::OProcess *m_hcitool;
        Opie::Core::OProcess *m_sdp; // not only one
        QString m_device;
        QMap<QString, int> m_signalStrength;
        QMap<QString, QString> m_out;
        QMap<QString, QString> m_devices;
        QMap<Opie::Core::OProcess*, QString> m_signalMac;
        QString m_hcitoolCon;
    };
};

#endif
