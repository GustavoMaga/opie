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

#ifndef OPIMNOTIFY_H
#define OPIMNOTIFY_H

/* QT */
#include <qdatetime.h>
#include <qvaluelist.h>

namespace Opie
{
/**
 * This is the base class of Notifiers. Possible
 * notifiers would be Alarms, Reminders
 * What they share is that they have
 * A DateTime, Type, Duration
 * This is what this base class takes care of
 * on top of that it's shared
 */
/*
 * TALK to eilers: have a class OPimDuration which sets the Duration
 *                 given on the Due/Start Date? -zecke
 * discuss: do we need a uid for the notify? -zecke
 */
class OPimNotify
{

  public:
    typedef QValueList<OPimNotify> ValueList;
    OPimNotify( const QDateTime& start = QDateTime(), int duration = 0, int parent = 0 );
    OPimNotify( const OPimNotify& );
    virtual ~OPimNotify();

    OPimNotify &operator=( const OPimNotify& );
    bool operator==( const OPimNotify& );

    virtual QString type() const = 0;

    /** start date */
    QDateTime dateTime() const;
    QString service() const;

    /**
     * RETURN the parent uid
     */
    int parent() const;

    /**
     * in Seconds
     */
    int duration() const;

    /**
     * Start Time + Duration
     */
    QDateTime endTime() const;

    void setDateTime( const QDateTime& );
    void setDuration( int dur );
    void setParent( int uid );
    void setService( const QString& );


  private:
    inline void copyIntern();
    void deref();
    struct Data;
    Data* data;

    /* d-pointer */
    class NotifyPrivate;
    NotifyPrivate* d;

};
/**
 * An alarm is a sound/mail/buzzer played/send
 * at a given time to inform about
 * an Event
 */
class OPimAlarm : public OPimNotify
{
  public:
    enum Sound{Loud = 1, Silent = 0, Custom = 2 };
    OPimAlarm( int sound = Silent, const QDateTime& start = QDateTime(), int duration = 0, int parent = 0 );
    OPimAlarm( const OPimAlarm& );
    ~OPimAlarm();

    OPimAlarm &operator=( const OPimAlarm& );
    bool operator==( const OPimAlarm& );
    QString type() const;

    int sound() const;
    QString file() const;

    void setSound( int );
    /* only when sound is custom... */
    void setFile( const QString& sound );

  private:
    void deref();
    void copyIntern();
    struct Data;
    Data * data;

    class Private;
    Private* d;

};

/**
 * A Reminder will be put into the
 * datebook
 * Note that the returned dateTime() may be not valid.
 * In these cases one must resolve the uid and get the OEvent
 */
class OPimReminder : public OPimNotify
{
  public:
    /**
     * c'tor of a reminder
     * @param uid The uid of the Record inside the Datebook
     * @param start the StartDate invalid for all day...
     * @param duration The duration of the event ( -1 for all day )
     * @param parent The 'parent' record of this reminder
     */
    OPimReminder( int uid = 0, const QDateTime& start = QDateTime(),
                  int duration = 0, int parent = 0 );
    OPimReminder( const OPimReminder& );
    OPimReminder &operator=( const OPimReminder& );

    QString type() const;

    bool operator==( const OPimReminder& );

    /**
     * the uid of the alarm
     * inside the 'datebook' application
     */
    int recordUid() const;
    void setRecordUid( int uid );

  private:
    void deref();
    void copyIntern();

    struct Data;
    Data* data;
    class Private;
    Private *d;
};

}

#endif
