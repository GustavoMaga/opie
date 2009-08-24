#include "infoimp.h"
#include "wextensions.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qtimer.h>
#include <qprogressbar.h>
#include <qlabel.h>

/**
 * Constructor.  If wireless extensions are enabled on device name then
 * start a timer that every second will update the information.
 */
WlanInfoImp::WlanInfoImp( QWidget* parent, const char* name, WFlags fl): WlanInfo(parent, name, fl){
  WExtensions *wExtensions = new WExtensions(name);
  if(!wExtensions->doesHaveWirelessExtensions()){
    delete  wExtensions;
    odebug << "WlanInfoImp::No wireless extension" << oendl;
    return;
  }
  delete  wExtensions;
  timer = new QTimer( this );
  connect( timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start( 1000, false );
}

/**
 * Updates the information about the wireless device.
 */
void WlanInfoImp::update(){
  WExtensions *wExtensions = new WExtensions(this->name());
  if(!wExtensions->doesHaveWirelessExtensions()){
    odebug << "No extension" << oendl;
    delete wExtensions;
    timer->stop();
    return;
  }
  essidLabel->setText(wExtensions->essid());
  apLabel->setText(wExtensions->ap());
  stationLabel->setText(wExtensions->station());
  modeLabel->setText(wExtensions->mode());
  channelLabel->setText(QString("%1").arg(wExtensions->channel()));
  int signal = 0;
  int noise = 0;
  int quality = 0;
  wExtensions->stats(signal, noise, quality);
  if(signalProgressBar->progress() != signal)
    signalProgressBar->setProgress(signal);
  if(noiseProgressBar->progress() != noise)
    noiseProgressBar->setProgress(noise);
  if(qualityProgressBar->progress() != quality)
    qualityProgressBar->setProgress(quality);
  rateLabel->setText(QString("%1 Mb/s").arg(wExtensions->rate()));
  delete wExtensions;
}

// infoimp.cpp

