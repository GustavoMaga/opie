#ifndef OPIE_DATEBOOK_SHOW_H
#define OPIE_DATEBOOK_SHOW_H

#include <qstring.h>
#include <qtextview.h>

#include <opie2/opimevent.h>

namespace Opie {
namespace Datebook {
    class MainWindow;

    /**
     * here is the show
     * The show interface will
     * show the richtext information
     * of the OPimEvent
     */
    class Show {
    public:
        /**
         * mainwindow as parent
         */
        Show(MainWindow* );
        virtual ~Show();

        /**
         * show the OPimEvent
         */
        virtual void show(const OPimEvent& str) = 0;

        /**
         * the Widget
         */
        virtual QWidget* widget() = 0;
    protected:
        /**
         * the show is over
         * ask the mainwindow to hide
         */
        void  hideMe();

    private:
        MainWindow* m_win;
    };
    class TextShow : public QTextView {
        Q_OBJECT
    public:
        TextShow( QWidget* parent, MainWindow* win );
        ~TextShow();

        QWidget* widget();
        void show(const OPimEvent&);

    };
}
}

#endif
