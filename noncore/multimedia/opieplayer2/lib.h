

#ifndef ZECKEXINELIB_H
#define ZECKEXINELIB_H

#include <qcstring.h>
#include <qstring.h>
#include <qobject.h>

#include <xine.h>

namespace XINE {

    /**
     * Lib wrapps the simple interface
     * of libxine for easy every day use
     * This will become a full C++ Wrapper
     * It supports playing, pausing, info,
     * stooping, seeking.
     */
    class Frame;
    class Lib : public QObject {
        Q_OBJECT
    public:
        Lib();
        ~Lib();
        QCString version();
        int majorVersion()/*const*/;
        int minorVersion()/*const*/;
        int subVersion()/*const*/;


        int play( const QString& fileName,
                  int startPos = 0,
                  int start_time = 0 );
        void stop() /*const*/;
        void pause()/*const*/;

        int speed() /*const*/;
        void setSpeed( int speed = SPEED_PAUSE );

        int status() /*const*/;

        int currentPosition()/*const*/;
        //in seconds
        int currentTime()/*const*/;
        int length() /*const*/;

        bool isSeekable()/*const*/;

        /**
         * Whether or not to show video output
         */
        void setShowVideo(bool video);

        /**
         * is we show video
         */
        bool isShowingVideo() /*const*/;

        /**
         *
         */
        void showVideoFullScreen( bool fullScreen );

        /**
         *
         */
        bool isVideoFullScreen()/*const*/ ;

        /**
         *
         */
        bool isScaling();

        /**
         *
         */
        void setScaling( bool );
        /**
         * test
         */
        Frame currentFrame()/*const*/;

        /**
         * Returns the error code
         */
        int error() /*const*/;

    signals:
        void stopped();
    private:
        xine_t *m_xine;
        config_values_t *m_config;
        vo_driver_t *m_videoOutput;
        ao_driver_t* m_audioOutput;

        void handleXineEvent( xine_event_t* t );
        // C -> C++ bridge for the event system
        static void xine_event_handler( void* user_data, xine_event_t* t);
    };
};


#endif
