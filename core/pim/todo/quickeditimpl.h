#ifndef OPIE_QUICK_EDIT_IMPL_H
#define OPIE_QUICK_EDIT_IMPL_H

#include <qpixmap.h>

#include <qtoolbar.h>

#include "quickedit.h"

class QLineEdit;
class QLabel;

class QuickEditImpl : public QToolBar, public Todo::QuickEdit {
    Q_OBJECT
public:
    QuickEditImpl( QWidget* parent,  bool visible);
    ~QuickEditImpl();
    OPimTodo todo()const;
    QWidget* widget();
private slots:
    void slotEnter();
    void slotPrio();
    void slotMore();
    void slotCancel();
private:
    void reinit();
    int m_state;
    QLabel* m_lbl;
    QLineEdit* m_edit;
    QLabel* m_enter;
    QLabel* m_more;
    QPopupMenu* m_menu;
    OPimTodo m_todo;
    bool m_visible;

    QPixmap priority1;
    QPixmap priority3;
    QPixmap priority5;
};

#endif
