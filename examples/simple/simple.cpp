#include <qlabel.h> // a label
#include <qpushbutton.h> // the header file for the QPushButton
#include <qlayout.h>

#include <qpe/qpeapplication.h> // the QPEApplication

#include <opie/oapplicationfactory.h> // a template + macro to save the main method and allow quick launching

#include "simple.h"

/*
 * implementation of simple
 */

/*
 * The factory is used for quicklaunching
 * It needs a constructor ( c'tor ) with at least QWidget, const char* and WFlags as parameter and a static QString appName() matching the TARGET of the .pro
 *
 * Depending on the global quick launch setting this will create
 * either a main method or one for our component plugin system
 */

OPIE_EXPORT_APP( OApplicationFactory<Simple> )

Simple::Simple( QWidget* parent, const char* name,  WFlags fl )
    : QWidget( parent, name, fl ) {

    /*
     * sets the caption of this toplevel widget
     * put all translatable string into tr()
     */
    setCaption(tr("My Simple Application") );

    /*
     * A simple vertical layout
     * either call layout->setAutoAdd( true )
     * or use layout->addWidget( wid ) to add widgets
     */
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setSpacing( 8 );
    layout->setMargin( 11 );

    /*
     * creates a label
     * The first parameter is this widget so the Label is a child
     * of us and will be deleted when we're deleted.
     */
    QLabel *lbl = new QLabel( this, "a name for the label" );
    lbl->setText( tr("Click on the button or follow the white rabbit") );
    layout->addWidget( lbl );


    /*  creates a button as child of this widget */
    m_button = new QPushButton(this);
    /*
     * another way to call tr. The first parameter is the string
     * to translate and the second a hint to the translator
     */
    m_button->setText( tr("Quit", "translatable quit string" ) );
    layout->addWidget( m_button );

    /*
     * Now we bring the action into it. The power of qt is the dynamic
     * signal and slots model
     * Usage is simple connect m_buttons clicked signal to  our
     * slotQuit slot.
     * We could also have connected a SIGNAL to a SIGNAL or the clicked
     * signal directly to qApp and SLOT(quit() )
     */
    connect( m_button, SIGNAL(clicked() ),
             this, SLOT( slotQuit() ) );
}

/*
 * Our destructor is empty because all child
 * widgets and layouts will be deleted by Qt.
 * Same applies to QObjects
 */
Simple::~Simple() {

}

void Simple::slotQuit() {
    /*
     * we will close this window and Qt will recognize that
     * the last window was closed and initiate a shutdown
     */
    close();
}
