
#ifndef MEDIA_PLAYER_STATE_H
#define MEDIA_PLAYER_STATE_H


#include <qobject.h>


class MediaPlayerDecoder;
class Config;


class MediaPlayerState : public QObject {
Q_OBJECT
public:
    MediaPlayerState( QObject *parent, const char *name );
    ~MediaPlayerState();

    bool isStreaming;
    bool fullscreen()    { return isFullscreen; }
    bool scaled()    { return isScaled; }
    bool looping()     { return isLooping; }
    bool shuffled()    { return isShuffled; }
    bool playlist()    { return usePlaylist; }
    bool paused()    { return isPaused; }
    bool playing()     { return isPlaying; }
    long position()    { return curPosition; }
    long length()    { return curLength; }
    char view()      { return curView; }

    MediaPlayerDecoder *newDecoder( const QString& file );
    MediaPlayerDecoder *curDecoder();

public slots:
    void setFullscreen( bool b ) { if ( isFullscreen == b ) return; isFullscreen = b; emit fullscreenToggled(b); }
    void setScaled( bool b )     { if ( isScaled     == b ) return; isScaled = b;     emit scaledToggled(b); }
    void setLooping( bool b )    { if ( isLooping    == b ) return; isLooping = b;    emit loopingToggled(b); }
    void setShuffled( bool b )   { if ( isShuffled   == b ) return; isShuffled = b;   emit shuffledToggled(b); }
    void setPlaylist( bool b )   { if ( usePlaylist  == b ) return; usePlaylist = b;  emit playlistToggled(b); }
    void setPaused( bool b )   { if ( isPaused     == b ) return; isPaused = b;     emit pausedToggled(b); }
    void setPlaying( bool b )  { if ( isPlaying    == b ) return; isPlaying = b;    emit playingToggled(b); }
    void setPosition( long p )   { if ( curPosition  == p ) return; curPosition = p;  emit positionChanged(p); }
    void updatePosition( long p ){ if ( curPosition  == p ) return; curPosition = p;  emit positionUpdated(p); }
    void setLength( long l )   { if ( curLength    == l ) return; curLength = l;    emit lengthChanged(l); }
    void setView( char v )   { if ( curView      == v ) return; curView = v;      emit viewChanged(v); }

    void setPrev()     { emit prev(); }
    void setNext()     { emit next(); }
    void setList()     { setPlaying( FALSE ); setView('l'); }
    void setVideo()    { setView('v'); }
    void setAudio()    { setView('a'); }

    void toggleFullscreen()  { setFullscreen( !isFullscreen ); }
    void toggleScaled()    { setScaled( !isScaled); }
    void toggleLooping()   { setLooping( !isLooping); }
    void toggleShuffled()  { setShuffled( !isShuffled); }
    void togglePlaylist()  { setPlaylist( !usePlaylist); }
    void togglePaused()    { setPaused( !isPaused); }
    void togglePlaying()   { setPlaying( !isPlaying); }

signals:
    void fullscreenToggled( bool );
    void scaledToggled( bool );
    void loopingToggled( bool );
    void shuffledToggled( bool );
    void playlistToggled( bool );
    void pausedToggled( bool );
    void playingToggled( bool );
    void positionChanged( long ); // When the slider is moved
    void positionUpdated( long ); // When the media file progresses
    void lengthChanged( long );
    void viewChanged( char );

    void prev();
    void next();

private:
    bool isFullscreen;
    bool isScaled;
    bool isLooping;
    bool isShuffled;
    bool usePlaylist;
    bool isPaused;
    bool isPlaying;
    long curPosition;
    long curLength;
    char curView;

    MediaPlayerDecoder *decoder;

    void readConfig( Config& cfg );
    void writeConfig( Config& cfg ) const;
};


#endif // MEDIA_PLAYER_STATE_H

