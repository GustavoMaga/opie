
#include <qpe/qpeapplication.h>

#include "otaskeditor.h"
#include "todoeditor.h"

using namespace Opie;
using namespace Todo;

Editor::Editor() {
    m_accepted = false;
    m_self = 0l;
}
Editor::~Editor() {
    delete m_self;
    m_self = 0;
}
OPimTodo Editor::newTodo( int cur,
                           QWidget*) {

    OTaskEditor *e = self();
    e->setCaption( QObject::tr("Enter Task") );
    e->init( cur );

    int ret = QPEApplication::execDialog( e );

    if ( QDialog::Accepted == ret ) {
        m_accepted = true;
    }else
        m_accepted = false;

    OPimTodo ev = e->todo();
    qWarning("Todo uid");
    qWarning("Todo %s %d %d", ev.summary().latin1(), ev.progress(), ev.isCompleted() );
    ev.setUid(1);

    return ev;
}
OPimTodo Editor::edit( QWidget *,
                        const OPimTodo& todo ) {
    OTaskEditor *e = self();
    e->init( todo );
    e->setCaption( QObject::tr( "Edit Task" ) );

    int ret = QPEApplication::execDialog( e );

    OPimTodo ev = e->todo();
    if ( ret == QDialog::Accepted )
        m_accepted = true;
    else
        m_accepted = false;

    return ev;
}
bool Editor::accepted()const {
    return m_accepted;
}
OTaskEditor* Editor::self() {
    if (!m_self )
        m_self = new OTaskEditor(0);

    return m_self;
}
