/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
// largly modified by Maximilian Reiss <max.reiss@gmx.de>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <locale.h>
#include <math.h>
#include <assert.h>

#include <qapplication.h>
#include <qmessagebox.h>
#include <qregexp.h>

#include <qpe/config.h>

// for network handling
#include <netinet/in.h>
#include <netdb.h>
#include <linux/limits.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


//#define HAVE_MMAP

#if defined(HAVE_MMAP)
#   include <sys/mman.h>
#endif
#include "libmadplugin.h"


extern "C" {
#include "mad.h"
}


#define MPEG_BUFFER_SIZE    65536
//#define MPEG_BUFFER_SIZE    32768 //16384 // 8192
//#define debugMsg(a)     qDebug(a)
#define debugMsg(a)


class Input {
public:
    char const *path;
    int fd;
#if defined(HAVE_MMAP)
    void *fdm;
#endif
    unsigned long fileLength;
   unsigned char *data;
    unsigned long length;
    int eof;
};


class Output {
public:
    mad_fixed_t attenuate;
    struct filter *filters;
    unsigned int channels_in;
    unsigned int channels_out;
    unsigned int speed_in;
    unsigned int speed_out;
    const char *path;
};


# if defined(HAVE_MMAP)
static void *map_file(int fd, unsigned long *length)
{
  void *fdm;

  *length += MAD_BUFFER_GUARD;

  fdm = mmap(0, *length, PROT_READ, MAP_SHARED, fd, 0);
  if (fdm == MAP_FAILED)
    return 0;

# if defined(HAVE_MADVISE)
  madvise(fdm, *length, MADV_SEQUENTIAL);
# endif

  return fdm;
}


static int unmap_file(void *fdm, unsigned long length)
{
  if (munmap(fdm, length) == -1)
    return -1;

  return 0;
}
# endif


static inline QString tr( const char *str ) {
    // Apparently this is okay from a plugin as it runs in the process space of the owner of the plugin
    return qApp->translate( "OpiePlayer", str, "libmad strings for mp3 file info" );
}


class LibMadPluginData {
public:
    Input input;
    Output output;
    int bad_last_frame;
    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;
    bool flush;
};


LibMadPlugin::LibMadPlugin() {
    d = new LibMadPluginData;
    d->input.fd = 0;
#if defined(HAVE_MMAP)
    d->input.fdm = 0;
#endif
    d->input.data = 0;
    d->flush = TRUE;
    info = tr( "No Song Open" );
}


LibMadPlugin::~LibMadPlugin() {
    close();
    delete d;
}


bool LibMadPlugin::isFileSupported( const QString& path ) {
    debugMsg( "LibMadPlugin::isFileSupported" );

    // Mpeg file extensions
    //  "mp2","mp3","m1v","m2v","m2s","mpg","vob","mpeg","ac3"
    // Other media extensions
    //  "wav","mid","mod","s3m","ogg","avi","mov","sid"

    char *ext = strrchr( path.latin1(), '.' );

    // Test file extension
    if ( ext ) {
        if ( strncasecmp(ext, ".mp2", 4) == 0 )
            return TRUE;
        if ( strncasecmp(ext, ".mp3", 4) == 0 )
            return TRUE;
    }

    // UGLY - just for fast testing
    if ( path.left(4) == "http") {
        return TRUE;
    }

   return FALSE;
}



int LibMadPlugin::is_address_multicast(unsigned long address) {
    if ((address & 255) >= 224 && (address & 255) <= 239)
        return (1);
    return (0);
}


int LibMadPlugin::udp_open(char *address, int port) {

    int enable = 1L;
    struct sockaddr_in stAddr;
    struct sockaddr_in stLclAddr;
    struct ip_mreq stMreq;
    struct hostent *host;
    int sock;

    stAddr.sin_family = AF_INET;
    stAddr.sin_port = htons(port);

    if ((host = gethostbyname(address)) == NULL) {
        return (0);
    }

    stAddr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

    /* Create a UDP socket */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return (0);
    }

    /* Allow multiple instance of the client to share the same address and port */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&enable, sizeof(unsigned long int)) < 0) {
        return (0);
    }

    /* If the address is multicast, register to the multicast group */
    if (is_address_multicast(stAddr.sin_addr.s_addr)) {
        /* Bind the socket to port */
        stLclAddr.sin_family = AF_INET;
        stLclAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        stLclAddr.sin_port = stAddr.sin_port;
        if (bind(sock, (struct sockaddr *)&stLclAddr, sizeof(stLclAddr)) < 0) {
            return (0);
        }

        /* Register to a multicast address */
        stMreq.imr_multiaddr.s_addr = stAddr.sin_addr.s_addr;
        stMreq.imr_interface.s_addr = INADDR_ANY;
        if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&stMreq, sizeof(stMreq)) < 0) {
            return (0);
        }
    } else {
        /* Bind the socket to port */
        stLclAddr.sin_family = AF_INET;
        stLclAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        stLclAddr.sin_port = htons(0);
        if (bind(sock, (struct sockaddr *)&stLclAddr, sizeof(stLclAddr)) < 0) {
            return (0);
        }
    }
    return (sock);
}

int LibMadPlugin::tcp_open(char *address, int port) {
    struct sockaddr_in stAddr;
    struct hostent *host;
    int sock;
    struct linger l;

    memset(&stAddr, 0, sizeof(stAddr));
    stAddr.sin_family = AF_INET;
    stAddr.sin_port = htons(port);

    if ((host = gethostbyname(address)) == NULL) {
        return (0);
    }

    stAddr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        return (0);
    }

    l.l_onoff = 1;
    l.l_linger = 5;
    if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *)&l, sizeof(l)) < 0) {
        return (0);
    }

    if (connect(sock, (struct sockaddr *)&stAddr, sizeof(stAddr)) < 0) {
        return (0);
    }

    return (sock);
}


/**
 * Read a http line header.
 * This function read character by character.
 * @param tcp_sock the socket use to read the stream
 * @param buf a buffer to receive the data
 * @param size size of the buffer
 * @return the size of the stream read or -1 if an error occured
 */
int LibMadPlugin::http_read_line(int tcp_sock, char *buf, int size) {
    int offset = 0;

    do {
        if (::read(tcp_sock, buf + offset, 1) < 0)
            return -1;
        if (buf[offset] != '\r')    /* Strip \r from answer */
            offset++;
    } while (offset < size - 1 && buf[offset - 1] != '\n');

    buf[offset] = 0;
    return offset;
}

int LibMadPlugin::http_open(const QString& path ) {
    char *host;
    int port;
    char *request;
    int tcp_sock;
    char http_request[PATH_MAX];
    char filename[PATH_MAX];
    //char c;
    char *arg =strdup(path.latin1());
    if (!arg)
	return (0);

    /* Check for URL syntax */
    if (strncmp(arg, "http://", strlen("http://"))) {
	free(arg);
        return (0);
    }

    /* Parse URL */
    port = 80;
    host = arg + strlen("http://");
    if ((request = strchr(host, '/')) == NULL) {
	free(arg);
        return (0);
    }

    *request++ = 0;

    if (strchr(host, ':') != NULL) { /* port is specified */
        port = atoi(strchr(host, ':') + 1);
        *strchr(host, ':') = 0;
    }

    /* Open a TCP socket */
    if (!(tcp_sock = tcp_open(host, port))) {
        perror("http_open");
        return (0);
    }

    snprintf(filename, sizeof(filename) - strlen(host) - 75, "%s", request);

    /* Send HTTP GET request */
    /* Please don't use a Agent known by shoutcast. Lynx and Mozilla seem to be
     * reconized and print an html page and not the stream
     */
    snprintf(http_request, sizeof(http_request), "GET /%s HTTP/1.0\r\n"
    /*  "User-Agent: Mozilla/2.0 (Win95; I)\r\n" */
        "Pragma: no-cache\r\n" "Host: %s\r\n" "Accept: */*\r\n" "\r\n", filename, host);

    send(tcp_sock, http_request, strlen(http_request), 0);

    QString name;
    QString genre;
    QString bitrate;
    QString url;
    QString message = tr("Info: ");
    do {

        int len;

        len = http_read_line(tcp_sock, http_request, sizeof(http_request));

        if (len == -1) {
           //            qDebug( "http_open: "+ QString(strerror(errno)) +"\n");
            return 0;
        }

        if (QString(http_request).left(9) == "Location:") {
            /* redirect */
            ::close(tcp_sock);
            http_request[strlen(http_request) - 1] = '\0';
            return http_open(&http_request[10]);
        }

        if (QString(http_request).left(4) == "ICY ") {
            /* This is shoutcast/icecast streaming */
            if (strncmp(http_request + 4, "200 ", 4)) {
               //                qDebug("http_open: " + QString(http_request) + "\n");
                return 0;
            }
        } else if (QString(http_request).left(4) == "icy-") {
            /* we can have: icy-noticeX, icy-name, icy-genre, icy-url, icy-pub, icy-metaint, icy-br */
            if ( QString( http_request ).left( 8 ) == "icy-name" ) {
                name = tr("Name: ") + QString(http_request).mid(9, (QString(http_request).length())- 9 );
            } else if ( QString( http_request ).left( 9 ) == "icy-genre" ) {
                genre = tr("Genre: ") + QString(http_request).mid(10, (QString(http_request).length())-10 );
            } else if ( QString( http_request ).left( 6 ) == "icy-br" ) {
                bitrate = tr("Bitrate: ") + QString(http_request).mid(7, (QString(http_request).length())- 7 );
            } else if ( QString( http_request ).left( 7 ) == "icy-url" ) {
                url = tr("URL: ") + QString(http_request).mid(8, (QString(http_request).length())- 8 );
            }  else if ( QString( http_request ).left( 10 ) == "icy-notice" ) {
                message += QString(http_request).mid(11, QString(http_request).length()-11 ) ;
            }
        }
    } while (strcmp(http_request, "\n") != 0);

    info = QString(name + genre + url + bitrate + message).replace( QRegExp("\n"), " : " );

    //    qDebug("Stream info: " + info);

    return (tcp_sock);
}



bool LibMadPlugin::open( const QString& path ) {
    debugMsg( "LibMadPlugin::open" );
    Config cfg("OpiePlayer");
    cfg.setGroup("Options");
    bufferSize = cfg.readNumEntry("MPeg_BufferSize",MPEG_BUFFER_SIZE);
    //    qDebug("buffer size is %d", bufferSize);
    d->bad_last_frame = 0;
    d->flush = TRUE;
    info = QString( "" );

    //qDebug( "Opening %s", path.latin1() );

    if (path.left( 4 ) == "http" ) {
        // in case of any error we get 0 here
        if ( !(http_open(path) == 0) ) {
            d->input.fd = http_open(path);
        } else {
            return FALSE;
        }
    } else {
        d->input.path = path.latin1();
        d->input.fd = ::open( d->input.path, O_RDONLY );
        // thats a better place, since it should only seek for ID3 tags on mp3 files, not streams
        printID3Tags();
    }
    if (d->input.fd == -1) {
       //        qDebug("error opening %s", d->input.path );
        return FALSE;
    }

    struct stat stat;
    if (fstat(d->input.fd, &stat) == -1) {
       //    qDebug("error calling fstat"); return FALSE;
    }
    if (S_ISREG(stat.st_mode) && stat.st_size > 0)
  d->input.fileLength = stat.st_size;
    else
  d->input.fileLength = 0;

#if defined(HAVE_MMAP)
    if (S_ISREG(stat.st_mode) && stat.st_size > 0) {
        d->input.length = stat.st_size;
        d->input.fdm = map_file(d->input.fd, &d->input.length);
        if (d->input.fdm == 0) {
           //            qDebug("error mmapping file"); return FALSE;
        }
        d->input.data = (unsigned char *)d->input.fdm;
    }
#endif

    if (d->input.data == 0) {
        d->input.data = (unsigned char *)malloc( bufferSize /*MPEG_BUFFER_SIZE*/);
        if (d->input.data == 0) {
           //            qDebug("error allocating input buffer");
            return FALSE;
        }
        d->input.length = 0;
    }

    d->input.eof = 0;

    mad_stream_init(&d->stream);
    mad_frame_init(&d->frame);
    mad_synth_init(&d->synth);

    return TRUE;
}


bool LibMadPlugin::close() {
    debugMsg( "LibMadPlugin::close" );

    int result = TRUE;

    mad_synth_finish(&d->synth);
    mad_frame_finish(&d->frame);
    mad_stream_finish(&d->stream);

#if defined(HAVE_MMAP)
    if (d->input.fdm) {
        if (unmap_file(d->input.fdm, d->input.length) == -1) {
           //            qDebug("error munmapping file");
            result = FALSE;
        }
        d->input.fdm  = 0;
        d->input.data = 0;
    }
#endif

    if (d->input.data) {
        free(d->input.data);
        d->input.data = 0;
    }

    if (::close(d->input.fd) == -1) {
       //        qDebug("error closing file %s", d->input.path);
        result = FALSE;
    }

    d->input.fd = 0;

    return result;
}


bool LibMadPlugin::isOpen() {
    debugMsg( "LibMadPlugin::isOpen" );
    return ( d->input.fd != 0 );
}


int LibMadPlugin::audioStreams() {
    debugMsg( "LibMadPlugin::audioStreams" );
    return 1;
}


int LibMadPlugin::audioChannels( int ) {
    debugMsg( "LibMadPlugin::audioChannels" );
/*
  long t; short t1[5]; audioReadSamples( t1, 2, 1, t, 0 );
  qDebug( "LibMadPlugin::audioChannels: %i", d->frame.header.mode > 0 ? 2 : 1 );
  return d->frame.header.mode > 0 ? 2 : 1;
*/
    return 2;
}


int LibMadPlugin::audioFrequency( int ) {
    debugMsg( "LibMadPlugin::audioFrequency" );
    long t; short t1[5]; audioReadSamples( t1, 2, 1, t, 0 );
    //    qDebug( "LibMadPlugin::audioFrequency: %i", d->frame.header.samplerate );
    return d->frame.header.samplerate;
}


int LibMadPlugin::audioSamples( int ) {
   debugMsg( "LibMadPlugin::audioSamples" );

   long t; short t1[5]; audioReadSamples( t1, 2, 1, t, 0 );
   mad_header_decode( (struct mad_header *)&d->frame.header, &d->stream );
/*
  qDebug( "LibMadPlugin::audioSamples: %i*%i", d->frame.header.duration.seconds,
  d->frame.header.samplerate );
  return d->frame.header.duration.seconds * d->frame.header.samplerate;
*/
   if ( d->frame.header.bitrate == 0 )
      return 0;
   int samples = (d->input.fileLength / (d->frame.header.bitrate/8)) * d->frame.header.samplerate;

   //   qDebug( "LibMadPlugin::audioSamples: %i * %i * 8 / %i", (int)d->input.fileLength,
   //           (int)d->frame.header.samplerate, (int)d->frame.header.bitrate );
   //   qDebug( "LibMadPlugin::audioSamples: %i", samples );

   return samples;

//    return 10000000;
}


bool LibMadPlugin::audioSetSample( long, int ) {
    debugMsg( "LibMadPlugin::audioSetSample" );

//     long totalSamples = audioSamples(0);
//     if ( totalSamples <= 1 )
//   return FALSE;

//     // Seek to requested position
//     qDebug( "seek pos: %i", (int)((double)pos * d->input.fileLength / totalSamples) );
//     ::lseek( d->input.fd, (long)((double)pos * d->input.fileLength / totalSamples), SEEK_SET );
//     mad_stream_sync(&d->stream);

//     mad_stream_init(&d->stream);
//     mad_frame_init(&d->frame);
//     mad_synth_init(&d->synth);

//     return TRUE;
    debugMsg( "LibMadPlugin::audioSetSample" );
    return FALSE;
}


long LibMadPlugin::audioGetSample( int ) {
    debugMsg( "LibMadPlugin::audioGetSample" );
    return 0;
}

/*
bool LibMadPlugin::audioReadSamples( short *, int, long, int ) {
debugMsg( "LibMadPlugin::audioReadSamples" );
return FALSE;
}


bool LibMadPlugin::audioReReadSamples( short *, int, long, int ) {
debugMsg( "LibMadPlugin::audioReReadSamples" );
    return FALSE;
    }
*/

bool LibMadPlugin::read() {
    debugMsg( "LibMadPlugin::read" );
    int len;

      if (d->input.eof)
  return FALSE;

#if defined(HAVE_MMAP)
      if (d->input.fdm) {
          unsigned long skip = 0;

          if (d->stream.next_frame) {
              struct stat stat;

              if (fstat(d->input.fd, &stat) == -1)
                  return FALSE;

              if (stat.st_size + MAD_BUFFER_GUARD <= (signed)d->input.length)
                  return FALSE;

              // file size changed; update memory map
              skip = d->stream.next_frame - d->input.data;

              if (unmap_file(d->input.fdm, d->input.length) == -1) {
                  d->input.fdm  = 0;
                  d->input.data = 0;
                  return FALSE;
              }

              d->input.length = stat.st_size;

              d->input.fdm = map_file(d->input.fd, &d->input.length);
              if (d->input.fdm == 0) {
                  d->input.data = 0;
                  return FALSE;
              }

              d->input.data = (unsigned char *)d->input.fdm;
          }

          mad_stream_buffer(&d->stream, d->input.data + skip, d->input.length - skip);

      } else
#endif
      {
          if (d->stream.next_frame) {
              memmove(d->input.data, d->stream.next_frame,
                      d->input.length = &d->input.data[d->input.length] - d->stream.next_frame);
          }

          do {
              len = ::read(d->input.fd, d->input.data + d->input.length, bufferSize /* MPEG_BUFFER_SIZE*/ - d->input.length);
          }
          while (len == -1 && errno == EINTR);

          if (len == -1) {
             //              qDebug("error reading audio");
              return FALSE;
          }
          else if (len == 0) {
              d->input.eof = 1;

              assert(bufferSize /*MPEG_BUFFER_SIZE*/ - d->input.length >= MAD_BUFFER_GUARD);

              while (len < MAD_BUFFER_GUARD)
                  d->input.data[d->input.length + len++] = 0;
          }

          mad_stream_buffer(&d->stream, d->input.data, d->input.length += len);
      }

      return TRUE;
}


static mad_fixed_t left_err, right_err;
static const int bits = 16;
static const int shift = MAD_F_FRACBITS + 1 - bits;


inline long audio_linear_dither( mad_fixed_t sample, mad_fixed_t& error ) {
    sample += error;
    mad_fixed_t quantized = (sample >= MAD_F_ONE) ? MAD_F_ONE - 1 : ( (sample < -MAD_F_ONE) ? -MAD_F_ONE : sample );
    quantized &= ~((1L << shift) - 1);
    error = sample - quantized;
    return quantized >> shift;
}


inline void audio_pcm( short *data, unsigned int nsamples, mad_fixed_t *left, mad_fixed_t *right ) {
    if ( right ) {
        while (nsamples--) {
            data[0] = audio_linear_dither( *left++,  left_err );
            data[1] = audio_linear_dither( *right++, right_err );
            data += 2;
        }
    } else {
        while (nsamples--) {
            data[0] = data[1] = audio_linear_dither( *left++,  left_err );
            data += 2;
        }
    }
}


bool LibMadPlugin::decode( short *output, long samples, long& samplesMade ) {
    debugMsg( "LibMadPlugin::decode" );

    static int buffered = 0;
    static mad_fixed_t buffer[2][65536 * 2];
    int offset = buffered;
    samplesMade = 0;

    static int maxBuffered = 8000; // 65536;

    if ( samples > maxBuffered ) {
        samples = maxBuffered;
    }

    if ( d->flush ) {
        buffered = 0;
        offset = 0;
        d->flush = FALSE;
    }

    while ( buffered < maxBuffered ) {

        while (mad_frame_decode(&d->frame, &d->stream) == -1) {
            if (!MAD_RECOVERABLE(d->stream.error)) {
                debugMsg( "feed me" );
                return FALSE; // Feed me
            }
            if ( d->stream.error == MAD_ERROR_BADCRC ) {
                mad_frame_mute(&d->frame);
                //                qDebug( "error decoding, bad crc" );
            }
        }

        mad_synth_frame(&d->synth, &d->frame);
        int decodedSamples = d->synth.pcm.length;
        memcpy( &(buffer[0][offset]), d->synth.pcm.samples[0], decodedSamples * sizeof(mad_fixed_t) );
        if ( d->synth.pcm.channels == 2 )
            memcpy( &(buffer[1][offset]), d->synth.pcm.samples[1], decodedSamples * sizeof(mad_fixed_t) );
        offset += decodedSamples;
        buffered += decodedSamples;
    }

//qApp->processEvents();
    audio_pcm( output, samples, buffer[0], (d->synth.pcm.channels == 2) ? buffer[1] : 0 );
//    audio_pcm( output, samples, buffer[1], buffer[0] );
//    audio_pcm( output, samples, buffer[0], buffer[1] );
    samplesMade = samples;
    memmove( buffer[0], &(buffer[0][samples]), (buffered - samples) * sizeof(mad_fixed_t) );
    if ( d->synth.pcm.channels == 2 ) {
        memmove( buffer[1], &(buffer[1][samples]), (buffered - samples) * sizeof(mad_fixed_t) );
    }
    buffered -= samples;

    return TRUE;
}

/*bool LibMadPlugin::audioReadStereoSamples( short *output, long samples, long& samplesMade, int ) {
*/
bool LibMadPlugin::audioReadSamples( short *output, int /*channels*/, long samples, long& samplesMade, int ) {
    debugMsg( "LibMadPlugin::audioReadStereoSamples" );

    static bool needInput = TRUE;

    if ( samples == 0 )
        return FALSE;

    do {
        if ( needInput )
            if ( !read() ) {
                return FALSE;
            }

        needInput = FALSE;

        if ( decode( output, samples, samplesMade ) )
            return TRUE;
        else
            needInput = TRUE;
    }
    while ( ( samplesMade < samples ) && ( !d->input.eof ) );

    return FALSE;
}


double LibMadPlugin::getTime() {
    debugMsg( "LibMadPlugin::getTime" );
    return 0.0;
}


void LibMadPlugin::printID3Tags() {
   //    qDebug( "LibMadPlugin::printID3Tags" );

    char id3v1[128 + 1];

    if ( ::lseek( d->input.fd, -128, SEEK_END ) == -1 ) {
       //        qDebug( "error seeking to id3 tags" );
        return;
    }

    if ( ::read( d->input.fd, id3v1, 128 ) != 128 ) {
       //        qDebug( "error reading in id3 tags" );
        return;
    }

    if ( ::strncmp( (const char *)id3v1, "TAG", 3 ) != 0 ) {
        debugMsg( "sorry, no id3 tags" );
    } else {
        int len[5] = { 30, 30, 30, 4, 30 };
        QString label[5] = { tr( "Title" ), tr( "Artist" ), tr( "Album" ), tr( "Year" ), tr( "Comment" ) };
        char *ptr = id3v1 + 3, *ptr2 = ptr + len[0];
        //        qDebug( "ID3 tags in file:" );
        info = "";
        for ( int i = 0; i < 5; ptr += len[i], i++, ptr2 += len[i] ) {
            char push = *ptr2;
            *ptr2 = '\0';
            char *ptr3 = ptr2;
            while ( ptr3-1 >= ptr && isspace(ptr3[-1]) ) ptr3--;
            char push2 = *ptr3; *ptr3 = '\0';
            if ( strcmp( ptr, "" ) ) {
                if( ((QString)ptr).find("  ") == -1) // don't add anything that has blanks
                info += ( i != 0 ? ", " : "" ) + label[i] + ": " + ptr;
            }
//             qDebug( info.latin1() );
            *ptr3 = push2;
            *ptr2 = push;
        }
        if (id3v1[126] == 0 && id3v1[127] != 0)
            info += tr( ", Track: " ) + id3v1[127];
    }

    if ( ::lseek(d->input.fd, 0, SEEK_SET) == -1 ) {
       //        qDebug( "error seeking back to beginning" );
        return;
    }
}

