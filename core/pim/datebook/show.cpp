#include "mainwindow.h"
#include "show.h"

using namespace Opie;
using namespace Opie::Datebook;

Show::Show( MainWindow* win )
    : m_win(win ) {
}

Show::~Show() {
}

void Show::hideMe() {
    m_win->hideShow();
}

TextShow::TextShow( QWidget* parent, MainWindow* )
    : QTextView( parent ){
}

TextShow::~TextShow() {
}

QWidget* TextShow::widget() {
    return this;
}

void TextShow::show(const OPimEvent& ev) {
    setText( ev.toRichText() );
}
