
#ifndef OpieTooth_Connection_H
#define OpieTooth_Connection_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qobject.h>

namespace OpieTooth {




    /**
     * Parent class for all kinds of starting connection
     * subclasses
     *
     */
    class StartConnection : public QObject {

    protected:

        enum ConnectionType{
            Pan = 0,
            Rfcomm,
            Obex,
            Hci
        };

        virtual ~StartConnection() {};

        virtual QString name() = 0;
        virtual void setName( QString name ) = 0;
        virtual ConnectionType type() = 0;
        virtual void setConnectionType() = 0;
        virtual void start() = 0;
        virtual void stop() = 0;

    };

    enum LinkDirection { Incoming= true, Outgoing = false };
    enum LinkMode { Master =0, Client };


    /**
     * The Connection class stores
     * the output of hcitool con
     * in a OO way
     */

    class ConnectionState {
    public:
        /**
         * typedef for a list of
         * Connections
         */
        typedef QValueList<ConnectionState> ValueList;

        /**
         * Copy c'tor.
         */
        ConnectionState( const ConnectionState& );

        /**
         * Main c'tor
         * Basicly it holds all values
         * a blueZ connections can have
         * @param in If the connection is either incoming or outgoing
         * @param conType Either ACL or SCO for connection type
         * @param mac The BD Address( mac ) of the peer
         * @param handle the blueZ handle
         * @param state the State of the connection
         * @param linkMode the linkmode of the connection MASTER or not
         *
         * < ACL 00:02:C7:09:2B:53 handle 1 state 1 lm MASTER
         *
         */
        ConnectionState( bool in,
                    const QString& conType,
                    const QString& mac,
                    int handle,
                    int state,
                    int linkMode );

        /**
         * C'tor for compability with QValueList
         * QValueList needs this c'tor.
         */
        ConnectionState();

        /**
         * Set if the connection is incoming or
         * outgoing
         * @param in Whether or not the connection is ingoing or not.
         * for param use either Incoming or Outgoing
         *
         */
        void setDirection( bool incoming = Incoming );

        /**
         * direction() will return Incoming( true )
         * if the direction is incomoning or Outgoing( false)
         * if outgoing
         */
        bool direction() const;

        /**
         * sets the ConnectionMode
         * @param comMode I know that SCO and ACL exists so far
         */
        void setConnectionMode( const QString& comMode );

        /**
         * linkMode returns the linkMode
         * MASTER for example
         */
        QString connectionMode() const;

        /**
         * The Bluetooth Address or mac
         * is set by this function
         * @param mac the BluetoothAddress( mac)
         */
        void setMac( const QString& mac);

        /**
         * returns the mac
         */
        QString mac() const;


        /**
         * Set the handle of the bt connection
         */
        void setHandle(int handle );

        /**
         * @return the handle of the connection
         */
        int handle() const;

        /**
         * sets the state
         */
        void setState( int state );

        /**
         * return the state
         */
        int state() const;

        /**
         * Sets the link mode of the Connection
         */
        void setLinkMode( int linkMode = Master );

        /**
         * returns the linkMode
         */
        int linkMode()const;

        /**
         * copy c'tor
         */
        ConnectionState &operator=( const ConnectionState& );

    private:
        class ConnectionStatePrivate;
        ConnectionStatePrivate *d;
        bool m_direction : 1;
        QString m_contype;
        QString m_mac;
        int m_handle;
        int m_state;
        int m_linkMode;

    };
};


#endif
