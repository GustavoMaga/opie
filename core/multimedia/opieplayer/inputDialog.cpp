#include "inputDialog.h"

#include <qpe/resource.h>

#include <opie/ofiledialog.h>

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

    MimeTypes types;
    QStringList audio, video, all;
    audio << "audio/*";
    audio << "playlist/plain";
    audio << "audio/x-mpegurl";

    video << "video/*";
    video << "playlist/plain";

    all += audio;
    all += video;
    types.insert("All Media Files", all );
    types.insert("Audio",  audio );
    types.insert("Video", video );

    QString str = OFileDialog::getOpenFileName( 1,"/","", types, 0 );
    LineEdit1->setText(str);
}

