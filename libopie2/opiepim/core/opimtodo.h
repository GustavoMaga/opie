/*
                            This file is part of the Opie Project
                            Copyright (C) The Main Author <main-author@whereever.org>
             =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
           .=l.
          .>+-=
_;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
- .   .-<_>     .<>         Foundation; either version 2 of the License,
    ._= =}       :          or (at your option) any later version.
   .%`+i>       _;_.
   .i_,=:_.      -<s.       This program is distributed in the hope that
    +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
   : ..    .:,     . . .    without even the implied warranty of
   =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
:     =  ...= . :.=-
-.   .:....=;==+<;          You should have received a copy of the GNU
 -_. . .   )=.  =           Library General Public License along with
   --        :-=`           this library; see the file COPYING.LIB.
                            If not, write to the Free Software Foundation,
                            Inc., 59 Temple Place - Suite 330,
                            Boston, MA 02111-1307, USA.
*/

#ifndef OTODOEVENT_H
#define OTODOEVENT_H

/* OPIE */
#include <opie2/opimrecord.h>
#include <qpe/recordfields.h>
#include <qpe/palmtopuidgen.h>

/* QT */
#include <qarray.h>
#include <qmap.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qvaluelist.h>

namespace Opie
{

class OPimState;
class OPimRecurrence;
class OPimMaintainer;
class OPimNotifyManager;
class OPimTodo : public OPimRecord
{
  public:
    typedef QValueList<OPimTodo> ValueList;
    enum RecordFields {
        Uid = Qtopia::UID_ID,
        Category = Qtopia::CATEGORY_ID,
        HasDate,
        Completed,
        Description,
        Summary,
        Priority,
        DateDay,
        DateMonth,
        DateYear,
        Progress,
        CrossReference,
        State,
        Recurrence,
        Alarms,
        Reminders,
        Notifiers,
        Maintainer,
        StartDate,
        CompletedDate
    };
  public:
    // priorities from Very low to very high
    enum TaskPriority { VeryHigh = 1, High, Normal, Low, VeryLow };

    /* Constructs a new ToDoEvent
       @param completed Is the TodoEvent completed
       @param priority What is the priority of this ToDoEvent
       @param category Which category does it belong( uid )
       @param summary A small summary of the todo
       @param description What is this ToDoEvent about
       @param hasDate Does this Event got a deadline
       @param date what is the deadline?
       @param uid what is the UUID of this Event
    **/
    OPimTodo( bool completed = false, int priority = Normal,
           const QStringList &category = QStringList(),
           const QString &summary = QString::null ,
           const QString &description = QString::null,
           ushort progress = 0,
           bool hasDate = false, QDate date = QDate::currentDate(),
           int uid = 0 /*empty*/ );

    OPimTodo( bool completed, int priority,
           const QArray<int>& category,
           const QString& summary = QString::null,
           const QString& description = QString::null,
           ushort progress = 0,
           bool hasDate = false, QDate date = QDate::currentDate(),
           int uid = 0 /* empty */ );

    /** Copy c'tor
     *
     */
    OPimTodo( const OPimTodo & );

    /**
     *destructor
     */
    ~OPimTodo();

    /**
     * Is this event completed?
     */
    bool isCompleted() const;

    /**
     * Does this Event have a deadline
     */
    bool hasDueDate() const;
    bool hasStartDate() const;
    bool hasCompletedDate() const;

    /**
     * What is the priority?
     */
    int priority() const ;

    /**
     * progress as ushort 0, 20, 40, 60, 80 or 100%
     */
    ushort progress() const;

    /**
     * The due Date
     */
    QDate dueDate() const;

    /**
     * When did it start?
     */
    QDate startDate() const;

    /**
     * When was it completed?
     */
    QDate completedDate() const;

    /**
     * does it have a state?
     */
    bool hasState() const;

    /**
     * What is the state of this OPimTodo?
     */
    OPimState state() const;

    /**
     * has recurrence?
     */
    bool hasRecurrence() const;

    /**
     * the recurrance of this
     */
    OPimRecurrence recurrence() const;

    /**
     * does this OPimTodo have a maintainer?
     */
    bool hasMaintainer() const;

    /**
     * the Maintainer of this OPimTodo
     */
    OPimMaintainer maintainer() const;

    /**
     * The description of the todo
     */
    QString description() const;

    /**
     * A small summary of the todo
     */
    QString summary() const;

    /**
     * @reimplemented
     * Return this todoevent in a RichText formatted QString
     */
    QString toRichText() const;

    bool hasNotifiers() const;
    /*
     * FIXME check if the sharing is still fine!! -zecke
     * ### CHECK If API is fine
     */
    /**
     * return a reference to our notifiers...
     */
    OPimNotifyManager &notifiers();

    /**
     *
     */
    const OPimNotifyManager &notifiers() const;

    /**
     * reimplementations
     */
    QString type() const;
    QString toShortText() const;
    QString recordField( int id ) const;

    /**
     * toMap puts all data into the map. int relates
     * to ToDoEvent RecordFields enum
     */
    QMap<int, QString> toMap() const;

    /**
     * Set if this Todo is completed
     */
    void setCompleted( bool completed );

    /**
     * set if this todo got an end data
     */
    void setHasDueDate( bool hasDate );
    // FIXME we do not have these for start, completed
    // cause we'll use the isNull() of QDate for figuring
    // out if it's has a date...
    // decide what to do here? -zecke

    /**
     * Set the priority of the Todo
     */
    void setPriority( int priority );

    /**
     * Set the progress.
     */
    void setProgress( ushort progress );

    /**
     * set the end date
     */
    void setDueDate( const QDate& date );

    /**
     * set the start date
     */
    void setStartDate( const QDate& date );

    /**
     * set the completed date
     */
    void setCompletedDate( const QDate& date );

    void setRecurrence( const OPimRecurrence& );

    void setDescription( const QString& );
    void setSummary( const QString& );

    /**
     * set the state of a Todo
     * @param state State what the todo should take
     */
    void setState( const OPimState& state );

    /**
     * set the Maintainer Mode
     */
    void setMaintainer( const OPimMaintainer& );

    bool isOverdue();


    virtual bool match( const QRegExp &r ) const;

    bool operator<( const OPimTodo &toDoEvent ) const;
    bool operator<=( const OPimTodo &toDoEvent ) const;
    bool operator!=( const OPimTodo &toDoEvent ) const;
    bool operator>( const OPimTodo &toDoEvent ) const;
    bool operator>=( const OPimTodo &toDoEvent ) const;
    bool operator==( const OPimTodo &toDoEvent ) const;
    OPimTodo &operator=( const OPimTodo &toDoEvent );

    static int rtti();

  private:
    class OPimTodoPrivate;
    struct OPimTodoData;

    void deref();
    inline void changeOrModify();
    void copy( OPimTodoData* src, OPimTodoData* dest );
    OPimTodoPrivate *d;
    OPimTodoData *data;

};


inline bool OPimTodo::operator!=( const OPimTodo &toDoEvent ) const
{
    return !( *this == toDoEvent );
}

}

#endif
