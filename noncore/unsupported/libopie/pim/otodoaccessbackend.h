#ifndef OPIE_TODO_ACCESS_BACKEND_H
#define OPIE_TODO_ACCESS_BACKEND_H

#include <qbitarray.h>

#include "otodo.h"
#include "opimaccessbackend.h"

class OTodoAccessBackend : public OPimAccessBackend<OTodo> {
public:
    OTodoAccessBackend();
    ~OTodoAccessBackend();
    virtual QArray<int> effectiveToDos( const QDate& start,
                                        const QDate& end,
                                        bool includeNoDates ) = 0;
    virtual QArray<int> overDue() = 0;
    virtual QArray<int> sorted( bool asc, int sortOrder, int sortFilter,
                                int cat ) = 0;
    virtual void        removeAllCompleted() = 0;
    virtual QBitArray supports()const = 0;
    
private:
    class Private;
    Private *d;

};

#endif
