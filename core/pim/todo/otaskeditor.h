#ifndef OPIE_TASK_EDITOR_H
#define OPIE_TASK_EDITOR_H

#include <qdialog.h>

#include <opie2/opimtodo.h>
#include <opie2/otabwidget.h>
#include <opie2/opimrecurrencewidget.h>
#include <opie2/opimrecurrencewidget.h>

class TaskEditorOverView;
class TaskEditorStatus;
class TaskEditorAlarms;
class QMultiLineEdit;

class OTaskEditor : public QDialog {
    Q_OBJECT
public:
    OTaskEditor(int cur);
    OTaskEditor( const Opie::OPimTodo& todo );
    ~OTaskEditor();

    /*
     * same as the c'tor but this gives us the
     * power to 'preload' the dialog
     */
    void init( int cur );
    void init( const Opie::OPimTodo& todo );

    Opie::OPimTodo todo()const;
private:
    void load( const Opie::OPimTodo& );
    void init();

    Opie::Ui::OTabWidget         *m_tab;
    TaskEditorOverView *m_overView;
    TaskEditorStatus   *m_stat;
    TaskEditorAlarms   *m_alarm;
    TaskEditorAlarms   *m_remind;
    OPimRecurrenceWidget  *m_rec;
    Opie::OPimTodo		m_todo;
};

#endif
