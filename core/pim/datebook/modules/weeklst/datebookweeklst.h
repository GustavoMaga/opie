#ifndef DATEBOOKWEEKLST
#define DATEBOOKWEEKLST

#include <qpe/event.h>

#include <qwidget.h>
#include <qdatetime.h>

class DateBookDB;
class DateBookDBHoliday;
class DateBookWeekLstHeader;
class DateBookWeekLstEvent;
class DateBookWeekLstDblView;
class QVBoxLayout;
class QScrollView;

class DateBookWeekLst : public QWidget
{
    Q_OBJECT

public:
    DateBookWeekLst( bool ampm, bool onM, DateBookDBHoliday *newDB,
             QWidget *parent = 0,
             const char *name = 0 );
    ~DateBookWeekLst();
    void setDate( int y, int w );
    void setDate(const QDate &d );
    int week() const { return _week; };
    QDate date();
    QDate weekDate() const;

public slots:
    void redraw();
    void dateChanged(QDate &date);

protected slots:
    void keyPressEvent(QKeyEvent *);
    void slotClockChanged(bool);
    void setDbl(bool on);

signals:
    void showDate(int y, int m, int d);
    void addEvent(const QDateTime &start, const QDateTime &stop,
    const QString &str, const QString &location);
    void editEvent(const EffectiveEvent &e);
    void duplicateEvent(const Event &e);
    void removeEvent(const EffectiveEvent &e);
    void beamEvent(const Event &e);

private:
    DateBookDBHoliday *db;
    int startTime;
    bool ampm;
    bool bStartOnMonday;
    bool dbl;
    QDate bdate;
    int year, _week,dow;
    DateBookWeekLstHeader *header;
    QVBoxLayout *layout;
    QScrollView *scroll;
    DateBookWeekLstDblView*m_CurrentView;
    bool dateset:1;

    void getEvents();
};

#endif
