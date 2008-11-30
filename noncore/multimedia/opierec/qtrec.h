/****************************************************************************
 ** Created: Thu Jan 17 11:19:45 2002
copyright 2002 by L.J. Potter ljp@llornkcor.com
****************************************************************************/
#ifndef QTREC_H
#define QTREC_H
#define VERSION 20040628

#include <qpe/ir.h>

#include <qfile.h>
#include <qimage.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qvariant.h>
#include <qwidget.h>
#include <stdio.h>
#include <stdlib.h>

#include "device.h"
#include "wavFile.h"

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QGridLayout; 
class QGroupBox;
class QHBoxLayout; 
class QIconView;
class QIconViewItem;
class QLabel;
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;
class QSlider;
class QTabWidget;
class QTimer;
class QVBoxLayout; 
class QLineEdit;

#define MAX_TRACKS 2
//#define BUFSIZE 4096
// #define BUFSIZE 8182 //Z default buffer size
#define BUFSIZE 1024
//#define BUFSIZE 2048
#define FRAGSIZE 0x7fff000A;

#define WAVE_FORMAT_DVI_ADPCM (0x0011)
#define WAVE_FORMAT_PCM (0x0001)
//AFMT_IMA_ADPCM

enum playerMode { MODE_IDLE, MODE_PLAYING, MODE_PAUSED, MODE_STOPPING, MODE_RECORDING };

class QtRec : public QWidget
{ 
   Q_OBJECT

public:
    static QString appName() { return QString::fromLatin1("opierec"); }
   QtRec( QWidget* parent=0, const char* name=0, WFlags fl=0 );
   ~QtRec();
   QSlider *OutputSlider,*InputSlider;

signals:
    void stopRecording();
    void startRecording();
    void stopPlaying();
    void startPlaying();

public slots:

private:
//    int fragment;
    int fd1;
    int secCount;
    QString timeString;
    QString m_dirPath;

    QLineEdit *renameBox;
    QGroupBox* GroupBox1;
    QString date, tmpFileName;
    QTimer *t_timer;
    bool needsStereoOut;
    bool useTmpFile, autoMute;

    bool eventFilter( QObject * , QEvent * );
    void okRename();
    void cancelRename();
    QString getStorage(const QString &);
    bool rec();
    int getCurrentSizeLimit();
    long checkDiskSpace(const QString &);
    void doMute(bool);
    void errorStop();
    void fillDirectoryCombo();
    void getInVol();
    void getOutVol();
    void init();
    void initConfig();
    void initConnections();
    void selectItemByName(const QString &);
    void start();
    void stop();
    void timerEvent( QTimerEvent *e );
    void setButtons();
    void fileSize(unsigned long size, QString &str);
    QString getSelectedFile();
    
private slots:
    void endPlaying();
    void endRecording();

    void FastforwardPressed();
    void FastforwardReleased();

    void changeDirCombo(int);
    void changeSizeLimitCombo(int);
    void changeTimeSlider(int);
    void changebitrateCombo(int);
    void changeStereoCheck( bool);

    void changedInVolume();
    void changedOutVolume();
    void changesamplerateCombo(int);

    void cleanUp();
    void compressionSelected(bool);
    void deleteSound();
    void doBeam();
    void doMicMuting(bool);
    void doPlayBtn();
    void doRename();
    void doVolMuting(bool); 
    void forwardTimerTimeout();
    void listPressed(int, QListViewItem *, const QPoint&, int);
    void newSound();
    void rewindPressed();
    void rewindReleased();
    void rewindTimerTimeout();
    void slotAutoMute(bool);
    void thisTab(QWidget*);
    void timeSliderPressed();
    void timeSliderReleased();
    void timerBreak();
    void initIconView();
/*     void changedOutVolume(int); */
/*    void changedInVolume(int); */

protected:

    WavFile *wavFile;
    QButtonGroup *ButtonGroup1;
    QCheckBox *outMuteCheckBox, *inMuteCheckBox, *compressionCheckBox, *autoMuteCheckBox, *stereoCheckBox;
    QComboBox* sampleRateComboBox, * bitRateComboBox, *directoryComboBox, *sizeLimitCombo;
    QHBoxLayout* Layout12;
    QHBoxLayout* Layout13;
    QHBoxLayout* Layout14;
    QHBoxLayout* Layout16;
    QHBoxLayout* Layout17;
    QHBoxLayout* Layout19;
    QIconView *IconView1;
    QLabel *NewSoundLabel,*playLabel2;
    QLabel *TextLabel3, *TextLabel1, *TextLabel2;
    QListView *ListView1;
    QPushButton *Stop_PushButton, *Play_PushButton, *Rec_PushButton,  *NewSoundButton, *deleteSoundButton, *toBeginningButton, *toEndButton;
    QString recDir;
    QTabWidget *TabWidget;
    QTimer *t, *rewindTimer, *forwardTimer;
    QVBoxLayout* Layout15;
    QVBoxLayout* Layout15b;
    QVBoxLayout* Layout18;
    QWidget *tab, *tab_2, *tab_3, *tab_4, *tab_5;
    int sliderPos, total;
//    short inbuffer[BUFSIZE], outbuffer[BUFSIZE];
//    unsigned short unsigned_inbuffer[BUFSIZE], unsigned_outbuffer[BUFSIZE];
    QGroupBox *sampleGroup, *bitGroup, *dirGroup, *sizeGroup;
/*     short inbuffer[65536], outbuffer[65536]; */
/*     unsigned short unsigned_inbuffer[65536], unsigned_outbuffer[65536]; */


    bool doPlay();
    bool openPlayFile();
    bool setUpFile();
    bool setupAudio( bool b);
    void fileBeamFinished( Ir *ir);
    void keyPressEvent( QKeyEvent *e);
    void keyReleaseEvent( QKeyEvent *e);
    void hideEvent( QHideEvent * );
    void receive( const QCString &, const QByteArray & );
    void showListMenu(QListViewItem * );
    void quickRec();
    void playIt();
 
};

#endif // QTREC_H
