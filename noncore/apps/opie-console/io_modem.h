#ifndef OPIE_IO_MODEM
#define OPIE_IO_MODEM

#include <opie/oprocess.h>
#include "io_serial.h"
#include "profile.h"

/* Default values to be used if the profile information is incomplete */
#define MODEM_DEFAULT_DEVICE       "/dev/ttyS0"
#define MODEM_DEFAULT_BAUD         9600
#define MODEM_DEFAULT_PARITY       0
#define MODEM_DEFAULT_DBITS        8
#define MODEM_DEFAULT_SBITS        1
#define MODEM_DEFAULT_FLOW         0

#define MODEM_DEFAULT_INIT_STRING  "AT"
#define MODEM_DEFAULT_RESET_STRING "ATZ~"
#define MODEM_DEFAULT_DIAL_PREFIX1 "ATDT"
#define MODEM_DEFAULT_DIAL_SUFFIX1 ""
#define MODEM_DEFAULT_DIAL_PREFIX2 ""
#define MODEM_DEFAULT_DIAL_SUFFIX2 ""
#define MODEM_DEFAULT_DIAL_PREFIX3 ""
#define MODEM_DEFAULT_DIAL_SUFFIX3 ""
#define MODEM_DEFAULT_CONNECT_STRING "CONNECT"
#define MODEM_DEFAULT_HANGUP_STRING "+++ATH"
#define MODEM_DEFAULT_CANCEL_STRING ""
#define MODEM_DEFAULT_DIAL_TIME     45
#define MODEM_DEFAULT_DELAY_REDIAL  2
#define MODEM_DEFAULT_NUMBER_TRIES  10
#define MODEM_DEFAULT_DTR_DROP_TIME 1
#define MODEM_DEFAULT_BPS_DETECT    0   // bool
#define MODEM_DEFAULT_DCD_LINES     1   //bool
#define MODEM_DEFAULT_MULTI_LINE_UNTAG 0 // bool

/* IOSerial implements a RS232 IO Layer */

class IOModem : public IOSerial {

    Q_OBJECT

public:

    IOModem(const Profile &);
    ~IOModem();

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

    QString m_initString, m_resetString, m_dialPref1, m_dialSuf1, m_dialPref2,
        m_dialSuf2, m_dialPref3, m_dialSuf3, m_connect, m_hangup, m_cancel;
    int  m_dialTime, m_delayRedial, m_numberTries, m_dtrDropTime,
        m_bpsDetect, m_dcdLines, m_multiLineUntag;
	Profile m_profile;

private slots:
    void slotExited(OProcess* proc);

};

#endif
