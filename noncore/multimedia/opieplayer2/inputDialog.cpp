#include "inputDialog.h"

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <opie/ofiledialog.h>

#include <qfileinfo.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>

InputDialog::InputDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ) {
    if ( !name ) {
        setName( "InputDialog" );
    }
    resize( 234, 115);
    setMaximumSize( QSize( 240, 40));
    setCaption( tr( name ) );

    QPushButton *browserButton;
    browserButton = new QPushButton( Resource::loadIconSet("fileopen"),"",this,"BrowseButton");
    browserButton->setGeometry( QRect( 205, 10, 22, 22));
    connect( browserButton, SIGNAL(released()),this,SLOT(browse()));
    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setGeometry( QRect( 4, 10, 190, 22 ) );
    LineEdit1->setFocus();
}
/*
 * return the current text(input)
 */
QString InputDialog::text() const {
   return LineEdit1->text(); 
}
/*
 *  Destroys the object and frees any allocated resources
 */
InputDialog::~InputDialog() {
}

void InputDialog::browse() {
  Config cfg( "OpiePlayer" );
  cfg.setGroup("Dialog");
      MimeTypes types;
    QStringList audio, video, all;
    audio << "audio/*";
    audio << "playlist/plain";
    audio << "audio/x-mpegurl";
    audio << "audio/x-ogg";
    
    video << "video/*";
    video << "playlist/plain";

    all += audio;
    all += video;
    types.insert("All Media Files", all );
    types.insert("Audio",  audio );
    types.insert("Video", video );

    QString str = OFileDialog::getOpenFileName( 1,
                  cfg.readEntry("LastDirectory",QPEApplication::documentDir()),"",
                  types, 0 );
    if(str.left(2) == "//") str=str.right(str.length()-1);
    LineEdit1->setText(str);
    cfg.writeEntry("LastDirectory" ,QFileInfo(str).dirPath());
}

