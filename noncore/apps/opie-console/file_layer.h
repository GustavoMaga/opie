#ifndef OPIE_FILE_LAYER_H
#define OPIE_FILE_LAYER_H

#include <qmap.h>

#include "io_layer.h"

class QFile;
/**
 * this is the layer for sending files
 */
class FileTransferLayer : public QObject {
    Q_OBJECT

public:
    enum Errors{
        NotSupported,
        StartError,
        NoError,
        Unknown,
        Undefined,
        Incomplete
    };
    enum Features {
        Append = 0,
        Twostop = 1,
        Escape = 2,
        Rename = 4,
        FullPath = 8,
        SendIfNewer = 16,
        SendIfLonger = 32,
        Resume = 64

    };
    enum Mode {
        Ascii = 0,
        Binary
    };
    enum BlockSize {
        Block_1k,
        Block_4k,
        Block_8k
    };
    /**
     *the io layer to be used
     */
    FileTransferLayer( IOLayer* );
    virtual ~FileTransferLayer();

public slots:

    virtual void sendFile( const QString& file,
                           Mode mode, BlockSize blk,
                           Features feat ) {};

    /**
     * send a file over the layer
     */
    virtual void sendFile( const QString& file ) = 0;
    virtual void sendFile( const QFile& ) = 0;
    virtual void cancel() = 0;

signals:
    /**
     * sent the file
     */
    void sent();

    /**
     * an error occured
     */

    void error( int, const QString& );

    /*
     * @param file The file to send
     * @param progress the progress made from 0-100
     * @param speed Speed in bps
     * @param hours The hours it take to finish
     * @param minutes The minutes it takes to finish
     * @param send The seconds...
     *
     */
    void progress( const QString& file, int progress, int speed, int hours, int minutes, int seconds );

protected:
    IOLayer* layer();

private:
    IOLayer* m_layer;

};

#endif
