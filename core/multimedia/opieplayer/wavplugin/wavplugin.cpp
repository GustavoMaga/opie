/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
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
// L.J.Potter added changes Fri 02-15-2002


#include "wavplugin.h"

/* OPIE */
#include <opie2/odebug.h>

/* QT */
#include <qfile.h>

/* STD */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

//#define debugMsg(a)     qDebug(a)
#define debugMsg(a)


struct RiffChunk {
    char id[4];
    Q_UINT32 size;
    char data[4];
};


struct ChunkData {
    Q_INT16 formatTag;
    Q_INT16 channels;
    Q_INT32 samplesPerSec;
    Q_INT32 avgBytesPerSec;
    Q_INT16 blockAlign;
    Q_INT16 wBitsPerSample;
};


const int sound_buffer_size = 512; // 4096; // you got to be kidding right?


class WavPluginData {
public:
    QFile *input;

    int wavedata_remaining;
    ChunkData chunkdata;
    RiffChunk chunk;
    uchar data[sound_buffer_size+32]; // +32 to handle badly aligned input data
    int out,max;
    int samples_due;
    int samples;

    WavPluginData() {
  max = out = sound_buffer_size;
  wavedata_remaining = 0;
  samples_due = 0;
  samples = -1;
    }

    // expands out samples to the frequency of 44kHz //not any more
    bool add( short *output, long count, long& done, bool stereo )
    {
  done = 0;

  if ( input == 0 ) {
      odebug << "no input" << oendl;
      return FALSE;
  }

        while ( count ) {
            int l,r;
            if ( getSample(l, r) == FALSE ) {
                odebug << "didn't get sample" << oendl;
                return FALSE;
            }
              samples_due += chunkdata.samplesPerSec;
              printf("samples due %d\r", samples_due);
              fflush(stdout);
             while ( count && (samples_due > chunkdata.samplesPerSec) ) {
                *output++ = l;
                if ( stereo )
                    *output++ = r;
                samples_due -= chunkdata.samplesPerSec;
                count--;
                done++;
            }
        }
        return TRUE;
    }

    bool initialise() {
  if ( input == 0 )
      return FALSE;

  wavedata_remaining = -1;

  while ( wavedata_remaining == -1 ) {
      // Keep reading chunks...
      const int n = sizeof(chunk) - sizeof(chunk.data);
      int t = input->readBlock( (char*)&chunk, n );
      if ( t != n ) {
    if ( t == -1 )
        return FALSE;
    return TRUE;
      }
      if ( qstrncmp(chunk.id,"data",4) == 0 ) {
    samples = wavedata_remaining = chunk.size;
      } else if ( qstrncmp(chunk.id,"RIFF",4) == 0 ) {
    char d[4];
    if ( input->readBlock(d,4) != 4 ) {
        return FALSE;
    }
    if ( qstrncmp(d,"WAVE",4) != 0 ) {
        // skip
        if ( chunk.size > 1000000000 || !input->at(input->at()+chunk.size-4) ) {
      return FALSE;
        }
    }
      } else if ( qstrncmp(chunk.id,"fmt ",4) == 0 ) {
    if ( input->readBlock((char*)&chunkdata,sizeof(chunkdata)) != sizeof(chunkdata) ) {
        return FALSE;
    }
#define WAVE_FORMAT_PCM 1
    if ( chunkdata.formatTag != WAVE_FORMAT_PCM ) {
        odebug << "WAV file: UNSUPPORTED FORMAT " << chunkdata.formatTag << "" << oendl;
        return FALSE;
    }
      } else {
    // ignored chunk
    if ( chunk.size > 1000000000 || !input->at(input->at()+chunk.size) ) {
        return FALSE;
    }
      }
  } // while
  odebug << "bits " << chunkdata.wBitsPerSample << "" << oendl;
  return TRUE;
    }


    // gets a sample from the file
    bool getSample(int& l, int& r)
    {
  l = r = 0;

  if ( input == 0 )
      return FALSE;

  if ( (wavedata_remaining < 0) || !max )
      return FALSE;

  if ( out >= max ) {
      max = input->readBlock( (char*)data, (uint)QMIN(sound_buffer_size,wavedata_remaining) );

      wavedata_remaining -= max;

      out = 0;
      if ( max <= 0 ) {
    max = 0;
    return TRUE;
      }
  }
  if ( chunkdata.wBitsPerSample == 8 ) {
      l = (data[out++] - 128) * 128;
  } else {
      l = ((short*)data)[out/2];
      out += 2;
  }
  if ( chunkdata.channels == 1 ) {
      r = l;
  } else {
      if ( chunkdata.wBitsPerSample == 8 ) {
    r = (data[out++] - 128) * 128;
      } else {
    r = ((short*)data)[out/2];
    out += 2;
      }
  }
  return TRUE;
    } // getSample

};


WavPlugin::WavPlugin() {
    d = new WavPluginData;
    d->input = 0;
}


WavPlugin::~WavPlugin() {
    close();
    delete d;
}


bool WavPlugin::isFileSupported( const QString& path ) {
//    odebug << "WavPlugin::isFileSupported" << oendl;

    char *ext = strrchr( path.latin1(), '.' );

    // Test file extension
    if ( ext ) {
  if ( strncasecmp(ext, ".raw", 4) == 0 )
      return TRUE;
  if ( strncasecmp(ext, ".wav", 4) == 0 )
      return TRUE;
  if ( strncasecmp(ext, ".wave", 4) == 0 )
      return TRUE;
    }

    return FALSE;
}


bool WavPlugin::open( const QString& path ) {
//    odebug << "WavPlugin::open" << oendl;

    d->max = d->out = sound_buffer_size;
    d->wavedata_remaining = 0;
    d->samples_due = 0;

    d->input = new QFile( path );
    if ( d->input->open(IO_ReadOnly) == FALSE ) {
  odebug << "couldn't open file" << oendl;
  delete d->input;
  d->input = 0;
  return FALSE;
    }

    d->initialise();
    qApp->processEvents();

    return TRUE;
}


bool WavPlugin::close() {
//    odebug << "WavPlugin::close" << oendl;

    d->input->close();
    delete d->input;
    d->input = 0;
    return TRUE;
}


bool WavPlugin::isOpen() {
//    odebug << "WavPlugin::isOpen" << oendl;
    return ( d->input != 0 );
}


int WavPlugin::audioStreams() {
//    odebug << "WavPlugin::audioStreams" << oendl;
    return 1;
}


int WavPlugin::audioChannels( int ) {
//    odebug << "WavPlugin::audioChannels" << oendl;
    return d->chunkdata.channels;// 2; // ### Always scale audio to stereo samples
}


int WavPlugin::audioFrequency( int ) {
//    odebug << "WavPlugin::audioFrequency " << d->chunkdata.samplesPerSec << "" << oendl;
    return  d->chunkdata.samplesPerSec; //44100; // ### Always scale to frequency of 44100
}


int WavPlugin::audioSamples( int ) {
//    odebug << "WavPlugin::audioSamples" << oendl;
    return d->samples / d->chunkdata.channels/2; // ### Scaled samples will be made stereo,
        // Therefore if source is mono we will double the number of samples
}


bool WavPlugin::audioSetSample( long, int ) {
//    odebug << "WavPlugin::audioSetSample" << oendl;
    return FALSE;
}


long WavPlugin::audioGetSample( int ) {
//    odebug << "WavPlugin::audioGetSample" << oendl;
    return 0;
}

/*
bool WavPlugin::audioReadSamples( short *, int, long, int ) {
    debugMsg( "WavPlugin::audioReadSamples" );
    return FALSE;
}


bool WavPlugin::audioReReadSamples( short *, int, long, int ) {
    debugMsg( "WavPlugin::audioReReadSamples" );
    return FALSE;
}


bool WavPlugin::audioReadMonoSamples( short *output, long samples, long& samplesMade, int ) {
    debugMsg( "WavPlugin::audioReadMonoSamples" );
    return !d->add( output, samples, samplesMade, FALSE );
}


bool WavPlugin::audioReadStereoSamples( short *output, long samples, long& samplesMade, int ) {
    debugMsg( "WavPlugin::audioReadStereoSamples" );
    return !d->add( output, samples, samplesMade, TRUE );
}
*/

bool WavPlugin::audioReadSamples( short *output, int channels, long samples, long& samplesMade, int ) {
//    odebug << "WavPlugin::audioReadSamples" << oendl;
    return d->add( output, samples, samplesMade, channels != 1 );
}

double WavPlugin::getTime() {
//    odebug << "WavPlugin::getTime" << oendl;  //this is a stupid hack here!!
  return d->chunkdata.wBitsPerSample; /*0.0*/;
}

// int WavPlugin::audioBitsPerSample( int ) {
// //    odebug << "WavPlugin::audioFormat " << d->chunkdata.wBitsPerSample << "" << oendl;
//     return d->chunkdata.wBitsPerSample; //
// }


