#ifndef OPIE_IO_BT
#define OPIE_IO_BT

#include <opie2/oprocess.h>
#include "io_serial.h"

/* Default values to be used if the profile information is incomplete */
#define BT_DEFAULT_DEVICE       "/dev/ttyU0"
#define BT_DEFAULT_BAUD         9600
#define BT_DEFAULT_PARITY       0
#define BT_DEFAULT_DBITS        8
#define BT_DEFAULT_SBITS        1
#define BT_DEFAULT_FLOW         0
#define BT_DEFAULT_MAC          0


/* IOSerial implements a RS232 IO Layer */

class IOBt : public IOSerial {

    Q_OBJECT

public:

    IOBt(const Profile &);
    ~IOBt();

    virtual QString identifier() const;
    virtual QString name() const;
    virtual QBitArray supports() const;
    virtual bool isConnected();

signals:
    void received(const QByteArray &);
    void error(int, const QString &);

public slots:
    virtual void send( const QByteArray& );
    virtual bool open();
    virtual void close();
    virtual void reload(const Profile &);

private:
    Opie::Core::OProcess *m_attach;
    QString m_mac;
private slots:
    void slotExited(Opie::Core::OProcess* proc);

};

#endif /* OPIE_IO_IRDA */
