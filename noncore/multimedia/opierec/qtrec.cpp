/****************************************************************************
	 // qtrec.cpp
 Created: Thu Jan 17 11:19:58 2002
 copyright 2002 by L.J. Potter <ljp@llornkcor.com>
****************************************************************************/
//#define DEV_VERSION

#include "pixmaps.h"
#include "qtrec.h"
#include "waveform.h"
extern "C" {
#include "adpcm.h"
}

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/storage.h>
using namespace Opie::Core;

/* QT */
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qtabwidget.h>
#include <qtimer.h>

/* STD */
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <mntent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <pthread.h>

#ifdef PDAUDIO //ALSA
#include <alsa/asoundlib.h>
static int deviceSampleRates[8] = { 32000, 44100, 48000, 88200, 96000, 176400, 192000, -1 };
static int deviceBitRates[] = { 8, 16, 24, 32, -1 };
#else //OSS
static int deviceSampleRates[6] = { 11025, 16000, 22050, 32000, 44100, -1 };
static int deviceBitRates[] = { 8, 16, -1 };
#endif


//#define ZAURUS 0
struct adpcm_state encoder_state;
struct adpcm_state decoder_state;

typedef struct {
		int sampleRate;
			/*      int fragSize; */
			/*      int blockSize; */
		int resolution; //bitrate
		int channels; //number of channels
		int fd; //file descriptor
		int sd; //sound device descriptor
		int numberSamples; //total number of samples
		int SecondsToRecord; // number of seconds that should be recorded
		float numberOfRecordedSeconds; //total number of samples recorded
		int samplesToRecord; //number of samples to be recorded
		int inVol; //input volume
		int outVol; //output volume
		int format; //wavfile format PCM.. ADPCM
		const  char *fileName; //name of fiel to be played/recorded
} fileParameters;

fileParameters filePara;

bool monitoring, recording, playing;
bool stopped;
QLabel *timeLabel;
QSlider *timeSlider;
int sd;

Waveform* waveform;
Device *soundDevice;


#ifdef THREADED
void quickRec()
#else
		void QtRec::quickRec()
#endif
{

		odebug << ( filePara.numberSamples/filePara.sampleRate * filePara.channels ) << oendl;
		odebug << "samples " << filePara.numberSamples << ", rate " << filePara.sampleRate
					 << ", channels " << filePara.channels << oendl;

		int total = 0;  // Total number of bytes read in so far.
		int bytesWritten, number;

		bytesWritten = 0;
		number = 0;
		QString num;
		int level = 0;
		int threshold = 0;
		int bits = filePara.resolution;
		odebug << "bits " << bits << "" << oendl;

		if( filePara.resolution == 16 ) { //AFMT_S16_LE)
				odebug << "AFMT_S16_LE size " << filePara.SecondsToRecord << "" << oendl;
				odebug << "samples to record " << filePara.samplesToRecord << "" << oendl;
				odebug << "" << filePara.sd << "" << oendl;
				level = 7;
				threshold = 0;

				if( filePara.format == WAVE_FORMAT_DVI_ADPCM) {
						odebug << "start recording WAVE_FORMAT_DVI_ADPCM" << oendl;
// <<<<<<<<<<<<<<<<<<<<<<<<<<<  WAVE_FORMAT_DVI_ADPCM  >>>>>>>>>>>>>>>>>>>>>>
						char abuf[ BUFSIZE/2 ];
						short sbuf[ BUFSIZE ];
						short sbuf2[ BUFSIZE ];
						memset( abuf, 0, BUFSIZE/2);
						memset( sbuf, 0, BUFSIZE);
						memset( sbuf2, 0, BUFSIZE);

						for(;;) {
								if ( stopped) {
										odebug << "quickRec:: stopped" << oendl;
										break;
								}

//             number=::read( filePara.sd, sbuf, BUFSIZE);
								number =  soundDevice->devRead( filePara.sd, sbuf, BUFSIZE);

								if(number <= 0) {
										perror("recording error ");
										odebug << "" << filePara.fileName << " " << number << "" << oendl;
										stopped = true;
										return;
								}
									//if(stereo == 2) {
//        adpcm_coder( sbuf2, abuf, number/2, &encoder_state);
								adpcm_coder( sbuf, abuf, number/2, &encoder_state);

								bytesWritten = ::write( filePara.fd , abuf, number/4);

								waveform->newSamples( sbuf, number );

								total += bytesWritten;
								filePara.numberSamples = total;
								timeSlider->setValue( total);

								printf("%d, bytes %d,total %d\r", number, bytesWritten, total);
								fflush(stdout);

								filePara.numberOfRecordedSeconds = (float)total / (float)filePara.sampleRate * (float)2;//	/ filePara.channels;

								qApp->processEvents();
								if( total >= filePara.samplesToRecord) {
										stopped = true;
										break;
								}
						}
				} else {
							// <<<<<<<<<<<<<<<<<<<<<<<<<<<  WAVE_FORMAT_PCM   >>>>>>>>>>>>>>>>>>>>>>
						odebug << "start recording WAVE_FORMAT_PCM" << oendl;
						short inbuffer[ BUFSIZE ], outbuffer[ BUFSIZE ];
						memset( inbuffer, 0, BUFSIZE);
						memset( outbuffer, 0, BUFSIZE);

						for(;;) {
								if ( stopped) {
										odebug << "quickRec:: stopped" << oendl;
										stopped = true;
										break;  // stop if playing was set to false
										return;
								}

								number = soundDevice->devRead( soundDevice->sd /*filePara.sd*/, (short *)inbuffer, BUFSIZE);

								if( number <= 0) {
										perror( "recording error ");
										odebug << filePara.fileName << oendl;
										stopped = true;
										return;
								}

								bytesWritten = ::write( filePara.fd , inbuffer, number);
								waveform->newSamples( inbuffer, number );

								if( bytesWritten < 0) {
										perror("File writing error ");
										stopped = true;
										return;
								}

								total += bytesWritten;

								filePara.numberSamples = total;

								if( filePara.SecondsToRecord != 0)
										timeSlider->setValue( total);
								printf("%d, bytes %d,total %d\r",number,  bytesWritten , total);
								fflush(stdout);

								filePara.numberOfRecordedSeconds = (float)total / (float)filePara.sampleRate
										/ (float)2/filePara.channels;
								qApp->processEvents();

								if( total >= filePara.samplesToRecord) {
										stopped = true;
										break;
								}
						}
				} //end main loop

		} else {
// <<<<<<<<<<<<<<<<<<<<<<<   format = AFMT_U8;
				unsigned char unsigned_inbuffer[ BUFSIZE ], unsigned_outbuffer[ BUFSIZE ];
				memset( unsigned_inbuffer, 0, BUFSIZE);
				memset( unsigned_outbuffer, 0, BUFSIZE);

				for(;;) {
						if ( stopped) {
								odebug << "quickRec:: stopped" << oendl;
								break;  // stop if playing was set to false
						}

						number = ::read( soundDevice->sd /*filePara.sd*/ , unsigned_inbuffer, BUFSIZE);
						bytesWritten = ::write( filePara.fd , unsigned_inbuffer, number);
            waveform->newSamples( (const short *) unsigned_inbuffer, number );

						if(bytesWritten < 0) {
								stopped = true;
								QMessageBox::message("Note","<p>There was a problem writing to the file</p>");
								perror("File writing error ");
								return;
						}

						total += bytesWritten;
						filePara.numberSamples = total;
							//             printf("%d, bytes %d,total %d \r",number,  bytesWritten , total);
							//             fflush(stdout);
						if( filePara.SecondsToRecord !=0)
								timeSlider->setValue( total);

						filePara.numberOfRecordedSeconds = (float)total / (float)filePara.sampleRate;

						qApp->processEvents();
						if( total >= filePara.samplesToRecord) {
								stopped = true;
								break;
						}
				} //end main loop
		}
		printf("\n");
} /// END quickRec()


#ifdef THREADED
void playIt()
#else
		void QtRec::playIt()
#endif
{
		int bytesWritten = 0;
		int number = 0;
		int total = 0;  // Total number of bytes read in so far.
		if( filePara.resolution == 16 ) { //AFMT_S16_LE) {
				if( filePara.format == WAVE_FORMAT_DVI_ADPCM) {
						char abuf[ BUFSIZE / 2 ];
						short sbuf[ BUFSIZE ];
						short sbuf2[ BUFSIZE * 2 ];
						memset( abuf, 0, BUFSIZE / 2);
						memset( sbuf, 0, BUFSIZE);
						memset( sbuf2, 0, BUFSIZE * 2);
// <<<<<<<<<<<<<<<<<<<<<<<<<<<  WAVE_FORMAT_DVI_ADPCM  >>>>>>>>>>>>>>>>>>>>>>
						for(;;) {  // play loop
								if ( stopped) {
										break;
										return;
								}// stop if playing was set to false

								number = ::read( filePara.fd, abuf, BUFSIZE / 2);
								adpcm_decoder( abuf, sbuf, number * 2, &decoder_state);

//                 for (int i=0;i< number * 2; 2 * i++) { //2*i is left channel
//                     sbuf2[i+1]=sbuf2[i]=sbuf[i];
//                 }
								bytesWritten = write ( soundDevice->sd , sbuf, number * 4);

								waveform->newSamples( sbuf, number );

									//          if(filePara.channels==1)
									//              total += bytesWritten/2; //mono
									//          else
								total += bytesWritten;
								filePara.numberSamples = total/4;
								filePara.numberOfRecordedSeconds = (float)total / (float)filePara.sampleRate / 2;

								timeSlider->setValue( total/4);
//            timeString.sprintf("%.2f", filePara.numberOfRecordedSeconds);
//         if(filePara.numberOfRecordedSeconds>1)
//            timeLabel->setText( timeString+ tr(" seconds"));
//                              printf("playing number %d, bytes %d, total %d\n",number,  bytesWritten, total/4);
//                              fflush(stdout);

								qApp->processEvents();

								if( /*total >= filePara.numberSamples || */ bytesWritten == 0) {
//            if( total >= filePara.numberSamples ){//||  secCount > filePara.numberOfRecordedSeconds ) {
										stopped = true;
										break;
								}
						}
				} else {
							// <<<<<<<<<<<<<<<<<<<<<<<<<<<  WAVE_FORMAT_PCM   >>>>>>>>>>>>>>>>>>>>>>
						short inbuffer[ BUFSIZE ], outbuffer[ BUFSIZE ];
						memset( inbuffer, 0, BUFSIZE);
						memset( outbuffer, 0, BUFSIZE);

						for(;;) {  // play loop
								if ( stopped) {
										break;
										return;
								}
// stop if playing was set to false
								number = ::read( filePara.fd, inbuffer, BUFSIZE);
//                 for (int i=0;i< number * 2; 2 * i++) { //2*i is left channel
//                       //        for (int i=0;i< number ; i++) { //2*i is left channel
//                     outbuffer[i+1]= outbuffer[i]=inbuffer[i];
//                 }
								bytesWritten = ::write( soundDevice->sd, inbuffer, number);
								waveform->newSamples( inbuffer, number);
									//-------------->>>> out to device
									//        total+=bytesWritten;
									//          if(filePara.channels==1)
									//              total += bytesWritten/2; //mono
									//          else
								total += bytesWritten;
								timeSlider->setValue( total);

								filePara.numberSamples = total;
								filePara.numberOfRecordedSeconds = (float)total / (float)filePara.sampleRate / (float)2;

//             timeString.sprintf("%.2f",filePara.numberOfRecordedSeconds);
//             timeLabel->setText( timeString + tr(" seconds"));

								qApp->processEvents();

								if( /*total >= filePara.numberSamples || */ bytesWritten == 0) {
										owarn << "Jane! Stop this crazy thing!" << oendl;
										stopped = true;
//                playing = false;
										break;
								}
						}
//             printf("\nplaying number %d, bytes %d, total %d\r",number,  bytesWritten, total);
//             fflush(stdout);
				} //end loop
		} else {
/////////////////////////////// format = AFMT_U8;
				unsigned char unsigned_inbuffer[ BUFSIZE ]; //, unsigned_outbuffer[BUFSIZE];
				memset( unsigned_inbuffer, 0, BUFSIZE);
				for(;;) {
// main loop
						if (stopped) {
								break;  // stop if playing was set to false
								return;
						}
						number = ::read( filePara.fd, unsigned_inbuffer, BUFSIZE);
//data = (val >> 8) ^ 0x80;
							//          unsigned_outbuffer = (unsigned_inbuffer >> 8) ^ 0x80;
						bytesWritten = write ( filePara.sd, unsigned_inbuffer, number);
						waveform->newSamples(  (const short *)unsigned_inbuffer, bytesWritten );
						total += bytesWritten;

            timeSlider->setValue( total);
						filePara.numberSamples = total;

						filePara.numberOfRecordedSeconds = (float)total / (float)filePara.sampleRate;
//          timeString.sprintf("%.2f",filePara.numberOfRecordedSeconds);
//          timeLabel->setText( timeString + tr(" seconds"));
						qApp->processEvents();

						if( /*total >= filePara.numberSamples || */ bytesWritten == 0) {
//         if( total >= filePara.numberSamples ) {
								stopped = true;
								break;
						}
//                      printf("Writing number %d, bytes %d, total %d, numberSamples %d\r",number,  bytesWritten , total, filePara.numberSamples);
//                      fflush(stdout);
				}
		}
}


QtRec::QtRec( QWidget* parent,  const char* name, WFlags fl )
				: QWidget( parent, name, fl )
{
		if ( !name )
				setName( "OpieRec" );
		init();
		initConfig();
		initConnections();
		renameBox = 0;

// open sound device to get volumes
		Config hwcfg("OpieRec");
		hwcfg.setGroup("Hardware");


		soundDevice = new Device( this, false); //open play

		getInVol();
		getOutVol();

		soundDevice->closeDevice( true);
		soundDevice->sd = -1;
		soundDevice = 0;
		wavFile = 0;
//      if( soundDevice) delete soundDevice;
		QTimer::singleShot(100,this, SLOT(initIconView()));

		if( autoMute)
				doMute( true);
//		ListView1->setFocus();
		playing = false;
}

QtRec::~QtRec() {
//   if( soundDevice) delete soundDevice;

}

void QtRec::cleanUp() {

		if( !stopped) {
				stopped = true;
				endRecording();
		}

		ListView1->clear();

		if( autoMute)
				doMute(false);

//      if( wavFile) delete wavFile;
// if(soundDevice) delete soundDevice;
}

void QtRec::init() {

		needsStereoOut = false;
		QPixmap image3( ( const char** ) image3_data );
		QPixmap image4( ( const char** ) image4_data );
		QPixmap image6( ( const char** ) image6_data );

		stopped = true;
		setCaption( tr( "OpieRecord " ));
		QGridLayout *layout = new QGridLayout( this );
		layout->setSpacing( 2);
		layout->setMargin( 2);

		TabWidget = new QTabWidget( this, "TabWidget" );
		layout->addMultiCellWidget(TabWidget, 0, 7, 0, 8);
//    TabWidget->setTabShape(QTabWidget::Triangular);

			///**********<<<<<<<<<<<<>>>>>>>>>>>>***************
		tab = new QWidget( TabWidget, "tab" );

		QGridLayout *layout1 = new QGridLayout( tab);
		layout1->setSpacing( 2);
		layout1->setMargin( 2);

		timeSlider = new QSlider( 0,100,10,0, QSlider::Horizontal, tab, (const char *) "timeSlider" );
		layout1->addMultiCellWidget( timeSlider, 1, 1, 0, 3);

// 		timeLabel = new QLabel( tab, "TimeLabel" );
// 		layout1->addMultiCellWidget( timeLabel, 0, 0, 0, 3);

// 		playLabel2 = new QLabel(tab, "PlayLabel2" );
// 		playLabel2->setText(tr("Play") );
// 		playLabel2->setFixedHeight( 18);
// 		layout1->addMultiCellWidget( playLabel2, 0, 0, 4, 4);

		Stop_PushButton = new QPushButton(  tab, "Stop_PushButton" );
		layout1->addMultiCellWidget( Stop_PushButton, 1, 1, 4, 4);
		Stop_PushButton->setFixedSize( 22, 22);
		Stop_PushButton->setPixmap( image4 );

		toBeginningButton = new QPushButton(  tab, "Beginning_PushButton" );
		layout1->addMultiCellWidget(toBeginningButton, 1, 1, 5, 5);
		toBeginningButton->setFixedSize( 22, 22);
		toBeginningButton->setPixmap( Resource::loadPixmap("fastback") );

		toEndButton = new QPushButton(  tab, "End_PushButton" );
		layout1->addMultiCellWidget( toEndButton, 1, 1, 6, 6);
		toEndButton->setFixedSize( 22, 22);
		toEndButton->setPixmap(  Resource::loadPixmap( "fastforward" )  );

// 		QLabel *recLabel2;
// 		recLabel2 = new QLabel( tab, "recLabel2" );
// 		recLabel2->setText(tr("Rec"));
// 		recLabel2->setFixedHeight( 18);
// 		layout1->addMultiCellWidget( recLabel2, 0, 0, 7, 7);

		Rec_PushButton = new QPushButton( tab, "Rec_PushButton" );
		layout1->addMultiCellWidget(  Rec_PushButton, 1, 1, 7, 7);
		Rec_PushButton->setFixedSize( 22, 22);
		Rec_PushButton->setPixmap( image6 );

		t = new QTimer( this );
		connect( t, SIGNAL( timeout() ), SLOT( timerBreak() ) );

		rewindTimer = new QTimer( this );
		connect( rewindTimer, SIGNAL( timeout() ),
						 this, SLOT( rewindTimerTimeout() ) );

		forwardTimer = new QTimer( this );
		connect( forwardTimer, SIGNAL( timeout() ),
						 this, SLOT( forwardTimerTimeout() ) );

		deleteSoundButton  = new QPushButton( tab, "deleteSoundButton" );
		layout1->addMultiCellWidget( deleteSoundButton, 1, 1, 8, 8);
		deleteSoundButton->setText( tr( "Del" ) );

		ListView1 = new QListView( tab, "IconView1" );
		layout1->addMultiCellWidget( ListView1, 2, 2, 0, 8);

		ListView1->addColumn( tr( "Name" ) );
		ListView1->setSorting( 1, false);
		ListView1->addColumn( tr( "Time" ) ); //in seconds
		ListView1->setColumnWidthMode(0, QListView::Maximum);
 		ListView1->setColumnAlignment( 1, QListView::AlignCenter);
		ListView1->setAllColumnsShowFocus( true );
		QPEApplication::setStylusOperation( ListView1->viewport(), QPEApplication::RightOnHold);

		TabWidget->insertTab( tab, tr( "Files" ) );

			///**********<<<<<<<<<<<<>>>>>>>>>>>>***************
		tab_3 = new QWidget( TabWidget, "tab_3" );
		QGridLayout *glayout3 = new QGridLayout( tab_3 );
		glayout3->setSpacing( 2);
		glayout3->setMargin( 2);
			////////////////////////////////////
		sampleGroup = new QGroupBox( tab_3, "samplegroup" );
		sampleGroup->setTitle( tr( "Sample Rate" ) );
		sampleGroup->setFixedSize( 95,50);

		sampleRateComboBox = new QComboBox( false, sampleGroup, "SampleRateComboBox" );
		sampleRateComboBox->setGeometry( QRect( 10, 20, 80, 25 ) );
		QString s;
		int z = 0;
		while( deviceSampleRates[z] != -1) {
				sampleRateComboBox->insertItem( s.setNum( deviceSampleRates[z], 10));
				z++;
		}


		glayout3->addMultiCellWidget(  sampleGroup, 0, 0, 0, 0);

		sizeGroup= new QGroupBox( tab_3, "sizeGroup" );
		sizeGroup->setTitle( tr( "Limit Size" ) );
		sizeGroup->setFixedSize( 80, 50);

		sizeLimitCombo = new QComboBox( false, sizeGroup, "sizeLimitCombo" );
		sizeLimitCombo ->setGeometry( QRect( 5, 20, 70, 25 ) );
		sizeLimitCombo->insertItem(tr("Unlimited"));

		for(int i=1;i<13; i++) {
				sizeLimitCombo->insertItem( QString::number( i * 5));
		}


		glayout3->addMultiCellWidget(  sizeGroup, 0, 0, 1, 1);
		dirGroup = new QGroupBox( tab_3, "dirGroup" );
		dirGroup->setTitle( tr( "File Directory" ) );
		dirGroup->setFixedSize( 130, 50);

		directoryComboBox = new QComboBox( false, dirGroup, "dirGroup" );
		directoryComboBox->setGeometry( QRect( 10, 15, 115, 25 ) );

		glayout3->addMultiCellWidget(  dirGroup, 1, 1, 0, 0);

		bitGroup = new QGroupBox( tab_3, "bitGroup" );
		bitGroup->setTitle( tr( "Bit Depth" ) );
		bitGroup->setFixedSize( 65, 50);

		bitRateComboBox = new QComboBox( false, bitGroup, "BitRateComboBox" );

		z = 0;
		while( deviceBitRates[z] != -1) {
				bitRateComboBox->insertItem( s.setNum( deviceBitRates[z], 10) );
				z++;
		}

		bitRateComboBox->setGeometry( QRect( 5, 20, 50, 25 ) );

		glayout3->addMultiCellWidget(  bitGroup, 1, 1, 1, 1);

		compressionCheckBox = new QCheckBox ( tr("Wave Compression (smaller files)"), tab_3 );

		autoMuteCheckBox = new QCheckBox ( tr("Auto Mute"), tab_3 );
		stereoCheckBox = new QCheckBox ( tr("Stereo"), tab_3 );

		glayout3->addMultiCellWidget( compressionCheckBox, 2, 2, 0, 3);
		glayout3->addMultiCellWidget( autoMuteCheckBox, 3, 3, 0, 0);
		glayout3->addMultiCellWidget( stereoCheckBox, 3, 3, 1, 1);

		tab_5 = new QWidget( TabWidget, "tab_5" );

		QHBoxLayout *Layout19a;
		Layout19a = new QHBoxLayout( tab_5);
		Layout19a->setSpacing( 2 );
		Layout19a->setMargin( 0 );

		Layout15 = new QVBoxLayout( this);
		Layout15->setSpacing( 2 );
		Layout15->setMargin( 0 );

		Layout15b = new QVBoxLayout( this);
		Layout15b->setSpacing( 2 );
		Layout15b->setMargin( 0 );

		TextLabel2 = new QLabel( tab_5, "InputLabel" );
		TextLabel2->setText( tr( "In"));
		TextLabel2->setFixedWidth( 35);
		Layout15->addWidget( TextLabel2 );

		TextLabel3 = new QLabel( tab_5, "OutputLabel" );
		TextLabel3->setText( tr( "Out" ) );
		Layout15b->addWidget( TextLabel3 );

		InputSlider = new QSlider( -100, 0, 10, 0, QSlider::Vertical, tab_5, (const char *) "InputSlider" );
		InputSlider->setTickmarks( QSlider::Both);
		Layout15->addWidget( InputSlider);

		OutputSlider = new QSlider( -100,0,10,0, QSlider::Vertical,tab_5,(const char *) "OutputSlider" );
		OutputSlider->setTickmarks( QSlider::Both);

		Layout15b->addWidget( OutputSlider );

		outMuteCheckBox = new QCheckBox ( tr("mute"), tab_5 );
		Layout15->addWidget( outMuteCheckBox );

		inMuteCheckBox = new QCheckBox ( tr("mute"), tab_5 );
		inMuteCheckBox-> setFocusPolicy ( QWidget::NoFocus );
		Layout15b->addWidget( inMuteCheckBox );


		Layout19a->addLayout( Layout15 );
		Layout19a->addLayout( Layout15b );

		fillDirectoryCombo();

		TabWidget->insertTab( tab_3, tr( "Options" ) );

		TabWidget->insertTab( tab_5, tr( "Volume" ) );

		waveform = new Waveform( this, "waveform" );
		waveform->setMinimumSize( QSize( 0, 50 ) );

		layout->addMultiCellWidget( waveform, 8, 8, 0, 8);
		waveform->setBackgroundColor ( black );
}

void QtRec::initIconView() {
		ListView1->clear();
		Config cfg("OpieRec");
		cfg.setGroup("Sounds");
		QString temp;
		QPixmap image0( ( const char** ) image0_data );

		int nFiles = cfg.readNumEntry("NumberofFiles",0);
//    owarn << "init number of files " << nFiles << "" << oendl;

		for(int i = 1; i <= nFiles; i++) {

				QListViewItem * item;
				QString fileS, mediaLocation, fileDate,  filePath;

				temp.sprintf( "%d",i);
				temp = cfg.readEntry( temp,""); //reads currentFile
				filePath = cfg.readEntry( temp,""); //currentFileName

				QFileInfo info(filePath);
				fileDate = info.lastModified().toString();

				fileS = cfg.readEntry( filePath, "0" );// file length in seconds
				mediaLocation = getStorage( filePath);
				if( info.exists()) {
//						owarn << "new item " << temp << oendl;
						item = new QListViewItem( ListView1, temp,  fileS /*,  mediaLocation, fileDate*/);
						item->setPixmap( 0, image0);
						if( currentFileName == filePath)
								ListView1->setSelected( item, true);
				}
		}
}

void QtRec::initConnections() {
		connect(qApp,SIGNAL(aboutToQuit()),SLOT(cleanUp()));

		connect(toBeginningButton,SIGNAL(pressed()),this,SLOT(rewindPressed()));
		connect(toBeginningButton,SIGNAL(released()),this,SLOT(rewindReleased()));
		connect(toEndButton,SIGNAL(pressed()),this,SLOT(FastforwardPressed()));
		connect(toEndButton,SIGNAL(released()),this,SLOT(FastforwardReleased()));
		connect(deleteSoundButton,SIGNAL(released()),this,SLOT(deleteSound()));
		connect(Stop_PushButton,SIGNAL(released()),this,SLOT(doPlayBtn()));
		connect(Rec_PushButton,SIGNAL(released()),this,SLOT(newSound()));

		connect(TabWidget,SIGNAL(currentChanged(QWidget*)),this,SLOT(thisTab(QWidget*)));

		connect(OutputSlider,SIGNAL(sliderReleased()),this,SLOT(changedOutVolume()));
		connect(InputSlider,SIGNAL(sliderReleased()),this,SLOT(changedInVolume()));

		connect(sampleRateComboBox,SIGNAL(activated(int)),this,SLOT(changesamplerateCombo(int)));
		connect(bitRateComboBox,SIGNAL(activated(int)),this,SLOT(changebitrateCombo(int)));
		connect(directoryComboBox,SIGNAL(activated(int)),this,SLOT(changeDirCombo(int)));
		connect(sizeLimitCombo,SIGNAL(activated(int)),this,SLOT(changeSizeLimitCombo(int)));

		connect(stereoCheckBox,SIGNAL(toggled(bool)),this,SLOT(changeStereoCheck(bool)));
		connect(outMuteCheckBox,SIGNAL(toggled(bool)),this,SLOT(doVolMuting(bool)));
		connect(inMuteCheckBox,SIGNAL(toggled(bool)),this,SLOT(doMicMuting(bool)));

		connect(ListView1,SIGNAL(doubleClicked(QListViewItem*)),this,SLOT(itClick(QListViewItem*)));
		connect(ListView1,SIGNAL(mouseButtonPressed(int,QListViewItem*,const QPoint&,int)),this,SLOT(listPressed(int,QListViewItem*,const QPoint&,int)));

		connect(timeSlider,SIGNAL(sliderMoved(int)),this,SLOT(changeTimeSlider(int)));
		connect(timeSlider,SIGNAL(sliderPressed()),this,SLOT(timeSliderPressed()));
		connect(timeSlider,SIGNAL(sliderReleased()),this,SLOT(timeSliderReleased()));

		connect(compressionCheckBox,SIGNAL(toggled(bool)),this,SLOT(compressionSelected(bool)));
		connect(autoMuteCheckBox,SIGNAL(toggled(bool)),this,SLOT(slotAutoMute(bool)));
}

void QtRec::initConfig() {
		int index, fred, i;
		Config cfg("OpieRec");
		cfg.setGroup("Settings");

		index = cfg.readNumEntry("samplerate",22050);
		bool ok;

		for(int ws=0;ws<sampleRateComboBox->count();ws++) {
				fred = sampleRateComboBox->text(ws).toInt(&ok, 10);
				if( index == fred) {
						filePara.sampleRate  = fred;
						sampleRateComboBox->setCurrentItem(ws);
				}
		}

		i = cfg.readNumEntry("bitrate",16);
		if(i == 16)
				bitRateComboBox->setCurrentItem( 1);
		else    if(i == 24)
				bitRateComboBox->setCurrentItem( 2);
		else    if(i == 32)
				bitRateComboBox->setCurrentItem( 3);
		else
				bitRateComboBox->setCurrentItem( 0);

		filePara.resolution = i;

		i = cfg.readNumEntry("sizeLimit", 5 );
		QString temp;
		sizeLimitCombo->setCurrentItem((i/5));

		stereoCheckBox->setChecked( cfg.readBoolEntry("stereo", 1));
		if( stereoCheckBox->isChecked()) {
				filePara.channels = 2;
		} else {
				filePara.channels = 1;
		}

		compressionCheckBox->setChecked( cfg.readBoolEntry("wavCompression",1));
		if( compressionCheckBox->isChecked()) {
				bitRateComboBox->setCurrentItem(1);
				bitRateComboBox->setEnabled(false);
				filePara.resolution=16;
		}

		autoMuteCheckBox->setChecked( cfg.readBoolEntry("useAutoMute",0));
		if( autoMuteCheckBox->isChecked())
				slotAutoMute(true);
		else
				slotAutoMute(false);

		Config cofg( "qpe");
		cofg.setGroup( "Volume");
		outMuteCheckBox->setChecked( cofg.readBoolEntry( "Mute",0));
		inMuteCheckBox->setChecked( cofg.readBoolEntry( "MicMute",0));
}

void QtRec::stop() {
//		owarn << "STOP" << oendl;
		setRecordButton(false);

		if( !recording)
				endPlaying();
		else
				endRecording();
		timeSlider->setValue(0);
}

void QtRec::doPlayBtn() {

		if(!stopped) {
//				playLabel2->setText(tr("Play"));
				stop();
		} else {
				if(ListView1->currentItem() == 0) return;
//				playLabel2->setText(tr("Stop"));
				currentFile = ListView1->currentItem()->text(0);
				start();
		}
}

void QtRec::start() { //play
		if( stopped) {
				QPixmap image3( ( const char** ) image3_data );
				Stop_PushButton->setPixmap( image3 );
				Stop_PushButton->setDown( true);
				stopped = false;
				paused = false;
				secCount = 1;

				if( openPlayFile())
						if( setupAudio( false))  //recording is false
								doPlay();
		}
}

bool QtRec::rec() { //record
		QString timeString;
		timeString.sprintf("%.0f",  0.0);
//		timeLabel->setText( timeString+ " seconds");
		if(!stopped) {
				monitoring = true;
				return false;
		} else {
				secCount = 1;
//				playLabel2->setText(tr("Stop"));
				monitoring = false;
				setRecordButton( true);

				if( setupAudio( true))
						if(setUpFile())  {
								int fileSize = 0;
								Config cfg("OpieRec");
								cfg.setGroup("Settings");
//             odebug << "<<<<<<<Device bits " << soundDevice->getDeviceBits()
//                    << ", device rate " << soundDevice->getDeviceRate()
//                    << ", device channels " << soundDevice->getDeviceChannels() << oendl;

									//filePara.sampleRate = cfg.readNumEntry("samplerate", 22050);
//             odebug << "sample rate is " << filePara.sampleRate << "" << oendl;
								filePara.SecondsToRecord = getCurrentSizeLimit();

//             odebug << "size limit " << filePara.SecondsToRecord << " sec" << oendl;
								int diskSize = checkDiskSpace( (const QString &) wavFile->trackName());

								if( filePara.SecondsToRecord == 0) {
										fileSize = diskSize;
								} else if( filePara.format == WAVE_FORMAT_PCM) {
//                odebug << "WAVE_FORMAT_PCM" << oendl;
										fileSize = (filePara.SecondsToRecord ) * filePara.channels
												* filePara.sampleRate * ( filePara.resolution / 8) + 1000;
								} else {
//                odebug << "WAVE_FORMAT_DVI_ADPCM" << oendl;
										fileSize = ((filePara.SecondsToRecord) * filePara.channels
																* filePara.sampleRate * ( filePara.resolution / 8) ) / 4 + 250;
								}

								filePara.samplesToRecord = fileSize;
								odebug << "filesize should be " << filePara.samplesToRecord
											 << ", bits " << filePara.resolution << ", rate " << filePara.sampleRate;
								if( paused) {
										paused = false;
								}
									//                  else {
								odebug << "Setting timeslider " << filePara.samplesToRecord << "" << oendl;
									//           if(fileSize != 0)
								timeSlider->setRange(0, filePara.samplesToRecord);
									//                  }

								if( diskSize < fileSize/1024) {
										QMessageBox::warning(this,
																				 tr("Low Disk Space"),
																				 tr("You are running low of\nrecording space\n"
																						"or a card isn't being recognized"));
										stopped = true; //we need to be stopped
										stop();
								} else {
										QString msg;
										msg.sprintf("%d, %d, %d", filePara.sampleRate, filePara.channels, filePara.resolution);
#ifdef DEV_VERSION
										setCaption( msg);
#endif
										filePara.fileName=currentFile.latin1();
										odebug << "Start recording" << oendl;
										stopped = false;

#ifdef THREADED
										odebug << "Start recording thread" << oendl;
										pthread_t thread1;
										pthread_create( &thread1, NULL, (void * (*)(void *))quickRec, NULL/* &*/);
#endif
										toBeginningButton->setEnabled( false);
										toEndButton->setEnabled( false);

										startTimer(1000);
#ifndef THREADED
										quickRec();
#endif
								}
						} //end setUpFile
		} //end setupAudio
		return true;
}
/*
  This happens when a tab is selected*/
void QtRec::thisTab(QWidget* widg) {
		if(widg != NULL) {
				int index = TabWidget->currentPageIndex();

				if(index == 0) { //file page
				}

				if(index == 1) { //control page
						fillDirectoryCombo();
//       soundDevice->getOutVol();
//       soundDevice->getInVol();
				}

				if( index==2) { //help page
				}

				qApp->processEvents();
				update();
		}
}

void QtRec::getOutVol( ) {
		filePara.outVol = soundDevice->getOutVolume();
//   odebug << "out vol " << filePara.outVol << "" << oendl;
		OutputSlider->setValue( -filePara.outVol);
}

void QtRec::getInVol() {
		filePara.inVol = soundDevice->getInVolume();
//    odebug << "in vol " << filePara.inVol << "" << oendl;
		InputSlider->setValue( -filePara.inVol);
}

void QtRec::changedOutVolume() {
		soundDevice->changedOutVolume( -OutputSlider->value());
}

void QtRec::changedInVolume( ) {
		soundDevice->changedInVolume( -InputSlider->value());
}


bool QtRec::setupAudio( bool b) {
		bool ok;
		int  sampleformat, stereo, flags;
		QString dspString, mixerString;

		filePara.resolution = bitRateComboBox->currentText().toInt( &ok,10);  //16

		if( !b) {
// we want to play
#ifdef PDAUDIO //ALSA
				if( filePara.resolution == 16 || compressionCheckBox->isChecked() ) {
						sampleformat = SND_PCM_FORMAT_S16;
						filePara.resolution = 16;
				} else if( filePara.resolution == 24 || compressionCheckBox->isChecked() ) {
						sampleformat = SND_PCM_FORMAT_S24;
						filePara.resolution = 24;
				} else if( filePara.resolution == 32 || compressionCheckBox->isChecked() ) {
						sampleformat = SND_PCM_FORMAT_S32;
						filePara.resolution = 32;
				} else {
						sampleformat = SND_PCM_FORMAT_U8;
						filePara.resolution = 8;
				}
#else
// we want to play
				if( filePara.resolution == 16 || compressionCheckBox->isChecked() ) {
						sampleformat = AFMT_S16_LE;
						filePara.resolution = 16;
				} else {
						sampleformat = AFMT_U8;
						filePara.resolution = 8;
				}
#endif

				stereo = filePara.channels;
				flags = O_WRONLY;
				recording = false;
		} else { // we want to record

#ifdef PDAUDIO //ALSA
				if( !bitRateComboBox->isEnabled() || bitRateComboBox->currentText() == "16")
						sampleformat = SND_PCM_FORMAT_S16;
				else if( !bitRateComboBox->isEnabled() || bitRateComboBox->currentText() == "24")
						sampleformat = SND_PCM_FORMAT_S24;
				else if( !bitRateComboBox->isEnabled() || bitRateComboBox->currentText() == "32")
						sampleformat = SND_PCM_FORMAT_S32;
				else
						sampleformat = SND_PCM_FORMAT_U8;

#else
				if( !bitRateComboBox->isEnabled() || bitRateComboBox->currentText() == "16")
						sampleformat = AFMT_S16_LE;
				else
						sampleformat = AFMT_U8;

				if( !compressionCheckBox->isChecked()) {
						filePara.format = WAVE_FORMAT_PCM;
//          odebug << "WAVE_FORMAT_PCM" << oendl;
				}  else {
						filePara.format = WAVE_FORMAT_DVI_ADPCM;
						sampleformat = AFMT_S16_LE;
//          odebug << "WAVE_FORMAT_DVI_ADPCM" << oendl;
				}
#endif

				stereo = filePara.channels;
//        filePara.sampleRate = sampleRateComboBox->currentText().toInt( &ok,10);//44100;
				flags= O_RDWR;
//        flags= O_RDONLY;
				recording = true;
		}

			// if(soundDevice) delete soundDevice;
		owarn << "<<<<<<<<<<<<<<<<<<<open dsp " << filePara.sampleRate << " " << filePara.channels << " " << sampleformat << "" << oendl;
//		owarn << "change waveform settings" << oendl;
		waveform->changeSettings( filePara.sampleRate, filePara.channels );

		soundDevice = new Device( this, b); //open rec
//    soundDevice->openDsp();
		soundDevice->reset();

		odebug << "device has been made " << soundDevice->sd << "" << oendl;

			//////////////////         <<<<<<<<<<<<>>>>>>>>>>>>
		soundDevice->setDeviceFormat( sampleformat);
		soundDevice->setDeviceChannels( filePara.channels);
		soundDevice->setDeviceRate( filePara.sampleRate);
		soundDevice->getDeviceFragSize();
#ifdef QT_QWS_EBX
		int frag = FRAGSIZE;
		soundDevice->setFragSize( frag);
		soundDevice->getDeviceFragSize();
#endif
			/////////////////
		filePara.sd = soundDevice->sd;

		if ( filePara.sd == -1) {

				monitoring = false;
				stopped = true;
				update();
				setCaption( tr( "OpieRecord " )+ QString::number(VERSION) );
				return false;
		}
		if(autoMute)
				doMute(false);

		return true;
}


bool QtRec::setUpFile() { //setup file for recording
//    odebug << "Setting up wavfile" << oendl;
//  if(wavFile) delete wavFile;
		wavFile = new WavFile( this, (const QString &)"",
													 true,
													 filePara.sampleRate,
													 filePara.channels,
													 filePara.resolution,
													 filePara.format);

		filePara.fd = wavFile->wavHandle();
		if(filePara.fd == -1) {
				return false;
		} else {
//      filePara.channels=1;
		}
		return true;
}

/// <<<<<<<<<<<<<<<<  PLAY >>>>>>>>>>>>>>>>>>>
bool QtRec::doPlay() {

		if( !paused) {
				total = 0;
				filePara.numberOfRecordedSeconds = 0;
		} else {
				paused = false;
				secCount = (int)filePara.numberOfRecordedSeconds;
		}
		playing = true;
		stopped = false;
		recording = false;

		QString num;
		odebug << "Play number of samples " << filePara.numberSamples << "" << oendl;

//   timeSlider->setRange( 0, filePara.numberSamples);

		timeString.sprintf("%f",  filePara.numberOfRecordedSeconds);
//		timeLabel->setText( timeString+ tr(" seconds"));

		QString msg;
		msg.sprintf("%d, %d, %d",
								filePara.sampleRate,
								filePara.channels,
								filePara.resolution);
#ifdef DEV_VERSION
		setCaption( msg);
#endif

		startTimer( 1000);
#ifdef THREADED
		pthread_t thread2;
		pthread_create( &thread2, NULL, (void * (*)(void *))playIt, NULL/* &*/);
#endif

		toBeginningButton->setEnabled( false);
		toEndButton->setEnabled( false);
#ifndef  THREADED
		playIt();
#endif
		return true;
}


void QtRec::changebitrateCombo(int i) {
		Config cfg("OpieRec");
		cfg.setGroup("Settings");
		int bits = 0;
		if( i == 1) { bits = 16; }
		else if( i == 2) { bits = 24; }
		else if( i == 3) { bits = 32; }
		else { bits=8; }
		cfg.writeEntry("bitrate", bits);
		filePara.resolution = bits;
		cfg.write();
}

void QtRec::changesamplerateCombo(int i) {
		Config cfg("OpieRec");
		cfg.setGroup("Settings");
		int rate=0;
		bool ok;
		rate = sampleRateComboBox->text(i).toInt(&ok, 10);
		cfg.writeEntry( "samplerate",rate);
		filePara.sampleRate=rate;
		odebug << "Change sample rate " << rate << "" << oendl;
		cfg.write();
}


void QtRec::changeDirCombo(int index) {
		Config cfg("OpieRec");
		cfg.setGroup("Settings");
		QString sName = directoryComboBox->text(index);

		StorageInfo storageInfo;
		const QList<FileSystem> &fs = storageInfo.fileSystems();
		QListIterator<FileSystem> it ( fs );
		QString storage;
		for( ; it.current(); ++it ){
				if( sName == (*it)->name()+" "+  (*it)->path() ||
						(*it)->name() == sName ) {
						const QString path = (*it)->path();
						recDir = path;
						cfg.writeEntry("directory", recDir);
						odebug << "new rec dir "+recDir << oendl;
				}
		}
		cfg.write();
}


void QtRec::changeSizeLimitCombo(int) {
		Config cfg("OpieRec");
		cfg.setGroup("Settings");
		cfg.writeEntry("sizeLimit", getCurrentSizeLimit() );
		cfg.write();
}

void QtRec::newSound() {
		if( !rec()) {
				endRecording();
				deleteSound();
		}
}

void QtRec::itClick(QListViewItem *item) {
		currentFile = item->text(0);
		setCaption("OpieRecord  "+currentFile);
}

void QtRec::deleteSound() {
		Config cfg("OpieRec");
		cfg.setGroup("Sounds");
		if( ListView1->currentItem() == NULL)
				return;
// #ifndef DEV_VERSION
// 		switch ( QMessageBox::warning(this,tr("Delete"),
// 																	tr("Do you really want to <font size=+2><B>DELETE</B></font>\nthe selected file?"),
// 																	tr("Yes"),tr("No"),0,1,1) ) {
// 			case 0:
// #endif
			//		{
		QString file = ListView1->currentItem()->text(0);
		QString fileName;
		fileName = cfg.readEntry( file, "");
		QFile f( fileName);
		if( f.exists())
				if( !f.remove())
						QMessageBox::message( tr("Error"), tr("Could not remove file."));

		int nFiles = cfg.readNumEntry( "NumberofFiles",0);
		bool found = false;
		for(int i=0;i<nFiles+1;i++) {

				if( cfg.readEntry( QString::number(i),"").find( file,0,true) != -1) {
						found = true;
						cfg.writeEntry( QString::number(i), cfg.readEntry( QString::number(i+1),""));
				}
				if(found)
						cfg.writeEntry( QString::number(i), cfg.readEntry( QString::number(i+1),""));
		}

		cfg.removeEntry( cfg.readEntry( file));
		cfg.removeEntry( file);
		cfg.writeEntry( "NumberofFiles", nFiles-1);
		cfg.write();

		ListView1->takeItem( ListView1->currentItem() );
		delete ListView1->currentItem();

		ListView1->clear();
		ListView1->setSelected( ListView1->firstChild(), true);
		initIconView();
		update();
		setCaption( tr( "OpieRecord " ));
}

void QtRec::keyPressEvent( QKeyEvent *e) {

		switch ( e->key() ) {
					//  case Key_F1:
					//     if(stopped && !recording)
					//       newSound();
					//     else
					//       stop();
					//    break;
					//   case Key_F2: {
					//     if( !e->isAutoRepeat())
					//     rewindPressed();
					//   }
					//     break;
					//   case Key_F3: {
					//     if( !e->isAutoRepeat())
					//     FastforwardPressed();
					//   }
					//     break;

					////////////////////////////// Zaurus keys
			case Key_F9: //activity
					break;
			case Key_F10: //contacts
					break;
			case Key_F11: //menu
					break;
			case Key_F12: //home
					break;
			case Key_F13: //mail
					break;
			case Key_Space:
					break;
			case Key_Delete:
					break;
			case Key_Up:
						//    stop();
					break;
			case Key_Down:
						//    newSound();
					break;
			case Key_Left: {
					odebug << "rewinding" << oendl;
					if( !e->isAutoRepeat())
							rewindPressed();
			}
					break;
			case Key_Right: {
					if( !e->isAutoRepeat())
							FastforwardPressed();
			}
					break;
		}
}

void  QtRec::keyReleaseEvent( QKeyEvent *e) {
		switch ( e->key() ) {
					//   case Key_F1:
					//      if(stopped && !recording)
					//        newSound();
					//      else
					//        stop();
					//     break;
					//   case Key_F2:
					//     rewindReleased();
					//     break;
					//   case Key_F3:
					//     FastforwardReleased();
					//     break;

					////////////////////////////// Zaurus keys
			case Key_F9: //activity
					break;
			case Key_F10: //contacts
					break;
			case Key_F11: //menu
					break;
			case Key_F12: //home
					if(stopped)
							doPlayBtn();
					else
							stop();
					break;
			case Key_F13: //mail
					break;
			case Key_Space:
					if(stopped && !recording)
							newSound();
					else
							stop();
					break;
			case Key_Delete:
					deleteSound();
					break;
			case Key_Up:
						//      stop();
					odebug << "Up" << oendl;
					break;
			case Key_Down:
						//      start();
						//      odebug << "Down" << oendl;
						//    newSound();
					break;
			case Key_Left:
					odebug << "Left" << oendl;
					rewindReleased();
					break;
			case Key_Right:
					odebug << "Right" << oendl;
					FastforwardReleased();
					break;
		}
}

void QtRec::endRecording() {
		
		monitoring = false;
		recording = false;
		stopped = true;
		waveform->reset();
		setRecordButton( false);

		toBeginningButton->setEnabled( true);
		toEndButton->setEnabled( true);

		killTimers();

		if(autoMute)
				doMute( true);

		soundDevice->closeDevice( true);

		if( wavFile->track.isOpen()) {
				wavFile->adjustHeaders( filePara.fd, filePara.numberSamples);
					//    soundDevice->sd=-1;
				filePara.numberSamples = 0;
					//      filePara.sd=-1;
				wavFile->closeFile();
				filePara.fd=0;

				if( wavFile->isTempFile()) {
// move tmp file to regular file
						QString cmd;
						cmd.sprintf("mv "+ wavFile->trackName() + " " + wavFile->currentFileName);
						odebug << "moving tmp file to "+currentFileName << oendl;
						system( cmd.latin1());
				}

				odebug << "Just moved " + wavFile->currentFileName << oendl;
				Config cfg("OpieRec");
				cfg.setGroup("Sounds");

				int nFiles = cfg.readNumEntry( "NumberofFiles",0);

				currentFile = QFileInfo( wavFile->currentFileName).fileName();
				currentFile = currentFile.left( currentFile.length() - 4);

				cfg.writeEntry( "NumberofFiles", nFiles + 1);
				cfg.writeEntry( QString::number( nFiles + 1), currentFile);
				cfg.writeEntry( currentFile, wavFile->currentFileName);

				QString time;
				time.sprintf("%.2f", filePara.numberOfRecordedSeconds);
				cfg.writeEntry( wavFile->currentFileName, time );
				odebug << "writing config numberOfRecordedSeconds "+time << oendl;

				cfg.write();
				odebug << "finished recording" << oendl;
//				timeLabel->setText("");
		}

//      if(soundDevice) delete soundDevice;

		timeSlider->setValue(0);
		initIconView();
		selectItemByName( currentFile);
		setCaption( tr( "OpieRecord " ));
		
}

void QtRec::endPlaying() {
		monitoring = false;
		recording = false;
		playing = false;
		stopped = true;
		waveform->reset();
//   errorStop();
    odebug << "end playing" << oendl;
		setRecordButton( false);

		toBeginningButton->setEnabled( true);
		toEndButton->setEnabled( true);

		if(autoMute)
				doMute( true);

		soundDevice->closeDevice( false);
		soundDevice->sd = -1;
			//  if(soundDevice) delete soundDevice;
    odebug << "file and sound device closed" << oendl;
//		timeLabel->setText("");
		total = 0;
		filePara.numberSamples = 0;
		filePara.sd = -1;
//   wavFile->closeFile();
		filePara.fd = 0;
//  if(wavFile) delete wavFile; //this crashes

    odebug << "track closed" << oendl;
		killTimers();
//		owarn << "reset slider" << oendl;
		timeSlider->setValue(0);

//      if(soundDevice) delete soundDevice;

}

bool QtRec::openPlayFile() {
		qWarning("opening file");
		qApp->processEvents();
		if( currentFile.isEmpty()) {
				QMessageBox::message(tr("Opierec"),tr("Please select file to play"));
				endPlaying();
				return false;
		}
		QString currentFileName;
		Config cfg("OpieRec");
		cfg.setGroup("Sounds");
		int nFiles = cfg.readNumEntry( "NumberofFiles", 0);
		for(int i=0;i<nFiles+1;i++) { //look for file
				if( cfg.readEntry( QString::number(i),"").find( currentFile,0,true) != -1) {
						currentFileName = cfg.readEntry( currentFile, "" );
						odebug << "opening for play: " + currentFileName << oendl;
				}
		}
		wavFile = new WavFile(this,
													currentFileName,
													false);
		filePara.fd = wavFile->wavHandle();
		if(filePara.fd == -1) {
					//  if(!track.open(IO_ReadOnly)) {
				QString errorMsg = (QString)strerror(errno);
				monitoring = false;
				setCaption( tr( "OpieRecord " ));
				QMessageBox::message(tr("Note"), tr("Could not open audio file.\n")
														 + errorMsg + "\n" + currentFile);
				return false;
		} else {

				filePara.numberSamples = wavFile->getNumberSamples();
				filePara.format = wavFile->getFormat();
				filePara.sampleRate = wavFile->getSampleRate();
				filePara.resolution = wavFile->getResolution();
				filePara.channels = wavFile->getChannels();
				timeSlider->setPageStep(1);
				monitoring = true;

				odebug << "file " << filePara.fd << ", samples " << filePara.numberSamples << " " << filePara.sampleRate << "" << oendl;
				int sec = (int) (( filePara.numberSamples / filePara.sampleRate) / filePara.channels) / ( filePara.channels*( filePara.resolution/8));

//				owarn << "seconds " << sec << "" << oendl;

				timeSlider->setRange(0, filePara.numberSamples );
		}

		return true;
}

void QtRec::listPressed( int mouse, QListViewItem *item, const QPoint &, int ) {
		if(item == NULL )
				return;
		switch (mouse) {
			case 1: {
					if( renameBox != 0 ) //tricky
							cancelRename();

					currentFile = item->text(0);
//					setCaption( "OpieRecord  " + currentFile);
			}
					break;
			case 2:
					showListMenu(item);
					ListView1->clearSelection();
					break;
		};
}

void QtRec::showListMenu(QListViewItem * item) {
		if(item == NULL)
				return;
		QPopupMenu *m = new QPopupMenu(this);
		m->insertItem( tr("Play"), this, SLOT( doMenuPlay() ));
		if(Ir::supported())  m->insertItem( tr( "Send with Ir" ), this, SLOT( doBeam() ));
		m->insertItem( tr( "Rename" ), this, SLOT( doRename() ));
			// #if defined (QTOPIA_INTERNAL_FSLP)
			//     m->insertItem( tr( "Properties" ), this, SLOT( doProperties() ));
			// #endif
		m->insertSeparator();
		m->insertItem( tr("Delete"), this, SLOT( deleteSound() ) );
		m->exec( QCursor::pos() );
		qApp->processEvents();
}

void QtRec::fileBeamFinished( Ir *ir) {
		if(ir)
				QMessageBox::message( tr("Ir Beam out"), tr("Ir sent.") ,tr("Ok") );

}

void QtRec::doBeam() {
		qApp->processEvents();
		if(  ListView1->currentItem() == NULL)
				return;
		Ir ir;
		if( ir.supported()) {
				QString file = ListView1->currentItem()->text(0);
				Config cfg("OpieRec");
				cfg.setGroup("Sounds");

				int nFiles = cfg.readNumEntry("NumberofFiles",0);

				for(int i=0;i<nFiles+1;i++) {
						if( cfg.readEntry( QString::number( i),"").find( file, 0, true) != -1) {
								QString filePath = cfg.readEntry(file,"");
								Ir *file = new Ir(this, "IR");
								connect( file, SIGNAL( done(Ir*)),
												 this, SLOT( fileBeamFinished(Ir*)));
								file->send( filePath, "OPieRec audio file\n" + filePath );
						}
				}
		}
}

void QtRec::doMenuPlay() {
		qApp->processEvents();
		currentFile = ListView1->currentItem()->text(0);
}

void QtRec::doRename() {
		QRect r = ListView1->itemRect( ListView1->currentItem( ));
		r = QRect( ListView1->viewportToContents( r.topLeft() ), r.size() );
		r.setX( ListView1->contentsX() );
		if ( r.width() > ListView1->visibleWidth() )
				r.setWidth( ListView1->visibleWidth() );

		renameBox = new QLineEdit( ListView1->viewport(), "qt_renamebox" );
		renameBox->setFrame(true);

		renameBox->setText(  ListView1->currentItem()->text(0) );

		renameBox->selectAll();
		renameBox->installEventFilter( this );
		ListView1->addChild( renameBox, r.x(), r.y() );
		renameBox->resize( r.size() );
		ListView1->viewport()->setFocusProxy( renameBox );
		renameBox->setFocus();
		renameBox->show();

}

void QtRec::okRename() {
		odebug << renameBox->text() << oendl;
		QString  filename = renameBox->text();
		cancelRename();

		if(  ListView1->currentItem() == NULL)
				return;

		Config cfg("OpieRec");
		cfg.setGroup("Sounds");

		QString file = ListView1->currentItem()->text(0);

		odebug << "filename is  " + filename << oendl;

		int nFiles = cfg.readNumEntry("NumberofFiles",0);

		for(int i=0;i<nFiles+1;i++) { //look for file
				if( cfg.readEntry( QString::number(i),"").find(file,0,true) != -1) {

						QString filePath = cfg.readEntry(file,"");

						cfg.writeEntry( QString::number(i), filename );
						cfg.writeEntry( filename, filePath );
						cfg.removeEntry( file);
						cfg.write();
				}
		}

		ListView1->takeItem( ListView1->currentItem() );
		delete ListView1->currentItem();
		ListView1->clear();
		initIconView();
		update();
}

void QtRec::cancelRename() {
		bool resetFocus = ListView1->viewport()->focusProxy() == renameBox;
		delete renameBox;
		renameBox = 0;
		if ( resetFocus ) {
				ListView1->viewport()->setFocusProxy( ListView1 );
				ListView1->setFocus();
		}
}

bool QtRec::eventFilter( QObject * o, QEvent * e ) {
		if ( o->inherits( "QLineEdit" ) ) {
				if ( e->type() == QEvent::KeyPress ) {
						QKeyEvent *ke = (QKeyEvent*)e;
						if ( ke->key() == Key_Return ||
								 ke->key() == Key_Enter ) {
								okRename();
								return true;
						} else if ( ke->key() == Key_Escape ) {
								cancelRename();
								return true;
						}
				} else if ( e->type() == QEvent::FocusOut ) {
						cancelRename();
						return true;
				}
		}
		return QWidget::eventFilter( o, e );
}


int QtRec::getCurrentSizeLimit() {
		return  sizeLimitCombo->currentItem() * 5;
}

void QtRec::timerBreak() {
		endPlaying();
}

void QtRec::doVolMuting(bool b) {
		Config cfg( "qpe" );
		cfg. setGroup( "Volume" );
		cfg.writeEntry( "Mute",b);
		cfg.write();
		QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << b;
}

void QtRec::doMicMuting(bool b) {
			//  odebug << "mic mute" << oendl;
		Config cfg( "qpe" );
		cfg. setGroup( "Volume" );
		cfg.writeEntry( "MicMute",b);
		cfg.write();
		QCopEnvelope( "QPE/System", "micChange(bool)" ) << b;
}

void QtRec::compressionSelected(bool b) {
		Config cfg("OpieRec");
		cfg.setGroup("Settings");
		cfg.writeEntry("wavCompression", b);
		cfg.writeEntry("bitrate", 16);
		filePara.resolution = 16;
		cfg.write();

		if(b) {
				qWarning("set adpcm");
				bitRateComboBox->setCurrentItem( 1);
				bitRateComboBox->setEnabled( false);
				filePara.resolution = 16;
		} else{
				bitRateComboBox->setEnabled( true);
		}
}

long QtRec::checkDiskSpace(const QString &path) {

		struct statfs fs;

		if ( !statfs( path.latin1(), &fs ) ) {

				int blkSize = fs.f_bsize;
				int availBlks = fs.f_bavail;

				long mult = blkSize / 1024;
				long div = 1024 / blkSize;

				if ( !mult ) mult = 1;
				if ( !div ) div = 1;

				return  availBlks * mult / div;
		}
		return -1;
}

//   short  f_fstyp;    /* File system type */
//   long  f_bsize;    /* Block size */
//   long  f_frsize;   /* Fragment size */
//   long  f_blocks;   /* Total number of blocks*/
//   long  f_bfree;    /* Count of free blocks */
//   long  f_files;    /* Total number of file nodes */
//   long  f_ffree;    /* Count of free file nodes */
//   char  f_fname[6]; /* Volumename */
//   char  f_fpack[6]; /* Pack name */

void QtRec::receive( const QCString &msg, const QByteArray & ) {
		odebug << "Voicerecord received message "+msg << oendl;

}


///////////////////////////// timerEvent
void QtRec::timerEvent( QTimerEvent * ) {

//    if(!recording)
//       timeSlider->setValue( secCount);
//     else
//        timeSlider->setValue( filePara.numberOfRecordedSeconds);

		if( stopped && playing) {
				stop();
		}

		if( stopped && recording ){
				stop();
		}

		if( recording && filePara.SecondsToRecord < secCount && filePara.SecondsToRecord != 0) {
				stop();
		}

		odebug << "" << secCount << "" << oendl;
		QString  timeString;

		timeString.sprintf("%d", secCount);
//		timeLabel->setText( timeString + " seconds");

		secCount++;
}

void QtRec::changeTimeSlider(int index) {
		if( ListView1->currentItem() == 0 || !wavFile->track.isOpen()) return;
		odebug << "Slider moved to " << index << "" << oendl;
		paused = true;
		stopped = true;

		sliderPos=index;

		QString timeString;
		filePara.numberOfRecordedSeconds = (float)sliderPos / (float)filePara.sampleRate * (float)2;
		timeString.sprintf( "%.2f", filePara.numberOfRecordedSeconds);
		secCount = (int)filePara.numberOfRecordedSeconds;
//		timeLabel->setText( timeString + tr(" seconds"));
}

void QtRec::timeSliderPressed() {
		if( ListView1->currentItem() == 0) return;
		odebug << "slider pressed" << oendl;
		paused = true;
		stopped = true;
}

void QtRec::timeSliderReleased() {
		if( ListView1->currentItem() == 0) return;
		sliderPos = timeSlider->value();

		odebug << "slider released " << sliderPos << "" << oendl;
		stopped = false;
		int newPos =  lseek( filePara.fd, sliderPos, SEEK_SET);
		total =  newPos*4;
		filePara.numberOfRecordedSeconds = (float)sliderPos / (float)filePara.sampleRate * (float)2;

		doPlay();
}

void QtRec::rewindPressed() {
		if( ListView1->currentItem() == 0) return;
		if( !wavFile->track.isOpen()) {
				if( !openPlayFile() )
						return;
				else
						if( !setupAudio( false))
								return;
		} else {
				killTimers();
				paused = true;
				stopped = true;
				rewindTimer->start( 50, false);
		}
}

void QtRec::rewindTimerTimeout() {
		int  sliderValue = timeSlider->value();
		sliderValue = sliderValue - ( filePara.numberSamples / 100);
			//  if(toBeginningButton->isDown())
		timeSlider->setValue( sliderValue ) ;
		odebug << "" << sliderValue << "" << oendl;
		QString timeString;
		filePara.numberOfRecordedSeconds = (float)sliderValue / (float)filePara.sampleRate * (float)2;
		timeString.sprintf( "%.2f", filePara.numberOfRecordedSeconds);
//		timeLabel->setText( timeString+ tr(" seconds"));
}

void QtRec::rewindReleased() {
		rewindTimer->stop();
		if( wavFile->track.isOpen()) {
				sliderPos=timeSlider->value();
				stopped = false;
				int newPos =  lseek( filePara.fd, sliderPos, SEEK_SET);
				total =  newPos * 4;
				odebug << "rewind released " << total << "" << oendl;
				startTimer( 1000);
				doPlay();
		}
}

void QtRec::FastforwardPressed() {
		if( ListView1->currentItem() == 0) return;
		if( !wavFile->track.isOpen())
				if( !openPlayFile() )
						return;
				else
						if( !setupAudio( false))
								return;
		killTimers();

		paused = true;
		stopped = true;
		forwardTimer->start(50, false);
}


void QtRec::forwardTimerTimeout() {
		int sliderValue = timeSlider->value();
		sliderValue = sliderValue + ( filePara.numberSamples / 100);

			//  if(toEndButton->isDown())
		timeSlider->setValue( sliderValue);

		QString timeString;
		filePara.numberOfRecordedSeconds = (float)sliderValue / (float)filePara.sampleRate * (float)2;
		timeString.sprintf( "%.2f", filePara.numberOfRecordedSeconds);
//		timeLabel->setText( timeString+ tr(" seconds"));
}

void QtRec::FastforwardReleased() {
		forwardTimer->stop();
		if( wavFile->track.isOpen()) {
				sliderPos=timeSlider->value();
				stopped = false;
				int newPos =  lseek( filePara.fd, sliderPos, SEEK_SET);
				total =  newPos * 4;
				filePara.numberOfRecordedSeconds = (float)sliderPos / (float)filePara.sampleRate * (float)2;
				startTimer( 1000);
				doPlay();
		}
}


QString QtRec::getStorage(const QString &fileName) {

		StorageInfo storageInfo;
		const QList<FileSystem> &fs = storageInfo.fileSystems();
		QListIterator<FileSystem> it ( fs );
		QString storage;
		for( ; it.current(); ++it ){
				const QString name = ( *it)->name();
				const QString path = ( *it)->path();
				const QString disk = ( *it)->disk();
				if( fileName.find( path,0,true) != -1)
						storage = name;
					//        const QString options = (*it)->options();
					//        if( name.find( tr("Internal"),0,true) == -1) {
					//            storageComboBox->insertItem( name +" -> "+disk);
					//    odebug << name << oendl;
		}
		return storage;
			//      struct mntent *me;
			//  //     if(fileName == "/etc/mtab") {
			//      FILE *mntfp = setmntent( fileName.latin1(), "r" );
			//      if ( mntfp ) {
			//          while ( (me = getmntent( mntfp )) != 0 ) {
			//              QString filesystemType = me->mnt_type;

			//          }
			//      }
			//      endmntent( mntfp );
}

void QtRec::setRecordButton(bool b) {

		if(b) { //about to record or play

				Rec_PushButton->setDown( true);
				QPixmap image3( ( const char** ) image3_data );
				Stop_PushButton->setPixmap( image3 );
				if(Stop_PushButton->isDown())
						Stop_PushButton->setDown( true);
//				playLabel2->setText( tr("Stop") );

		} else { //about to stop

				QPixmap image4( ( const char** ) image4_data );
				Stop_PushButton->setPixmap( image4);
				if(Stop_PushButton->isDown())
						Stop_PushButton->setDown( false);
//				playLabel2->setText( tr("Play") );
				if(Rec_PushButton->isDown())
						Rec_PushButton->setDown( false);
		}
}

void QtRec::fillDirectoryCombo() {
		if( directoryComboBox->count() > 0)
				directoryComboBox->clear();
		int index = 0;
		Config cfg("OpieRec");
		cfg.setGroup("Settings");
		QString dir = cfg.readEntry("directory", "/");
		StorageInfo storageInfo;
		const QList<FileSystem> &fs = storageInfo.fileSystems();
		QListIterator<FileSystem> it ( fs );
		QString storage;
		for( ; it.current(); ++it ){
				const QString name = ( *it)->name();
				const QString path = ( *it)->path();
					//    directoryComboBox->insertItem(name+" "+path);
				directoryComboBox->insertItem(name);
				if( path == dir)
						directoryComboBox->setCurrentItem( index);
				index++;
		}
}

void QtRec::errorStop() {
		stopped = true;
		wavFile->closeFile();
		killTimers();
}

void QtRec::doMute(bool b) {
		doVolMuting( b);
		doMicMuting( b);
}

void QtRec::slotAutoMute(bool b) {
		autoMute = b;
		Config cfg("OpieRec");
		cfg.setGroup("Settings");
		cfg.writeEntry("useAutoMute",b);
		doMute( b);
		outMuteCheckBox->setChecked( b);
		inMuteCheckBox->setChecked( b);
}

void QtRec::selectItemByName(const QString & name) {
		QListViewItemIterator it( ListView1 );
		for ( ; it.current(); ++it )
				if( name == it.current()->text(0))
						ListView1->setCurrentItem(it.current());
}


// long findPeak(long input ) {

// // halfLife = time in seconds for output to decay to half value after an impulse
//    static float output = 0.0;
//    float halfLife = .0025;
//    float vsf = .0025;
//    float scalar = pow( 0.5, 1.0/(halfLife * filePara.sampleRate ));
//    if( input < 0.0 )
//       input = -input;  // Absolute value.
//    if ( input >= output ) {
//       // When we hit a peak, ride the peak to the top.
//       output = input;
//    } else {
// // Exponential decay of output when signal is low.
//       output = output * scalar;
// //
// //   When current gets close to 0.0, set current to 0.0 to prevent FP underflow
// //   which can cause a severe performance degradation due to a flood
// //   of interrupts.
// //
//       if( output < vsf ) output = 0.0;
//    }

//    return (long) output;
// }

void QtRec::changeStereoCheck(bool b) {
		Config cfg("OpieRec");
		cfg.setGroup("Settings");
		int ch = 0;
		if ( !b) { ch = 1;}
		else { ch = 2;}
		cfg.writeEntry("stereo", b);
		filePara.channels = ch;

		cfg.write();
}


