#ifndef OPIE_TODO_EDITOR_H
#define OPIE_TODO_EDITOR_H

#include <opie/otodo.h>


namespace Todo {
    class Editor {
    public:
        Editor();
        ~Editor();

        OTodo newTodo( int currentCatId,
                           QWidget* par );
        OTodo edit( QWidget* par,
                        const OTodo& ev = OTodo() );


        bool accepted()const;
    private:
        bool m_accepted: 1;
    };
};

#endif
