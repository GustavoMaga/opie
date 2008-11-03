#ifndef OPIE_DATEBOOK_EDITOR_H
#define OPIE_DATEBOOK_EDITOR_H

#include <qdialog.h>

#include <opie2/opimevent.h>

#include "descriptionmanager.h"
#include "locationmanager.h"

namespace Opie {
namespace Datebook {
    class MainWindow;
    /**
     * This is the editor interface
     */
    class Editor {
    public:
        Editor( MainWindow*, QWidget* parent );
        virtual ~Editor();

        virtual bool newEvent( const QDate& ) = 0;
        virtual bool newEvent( const QDateTime& start, const QDateTime& end ) = 0;
        virtual bool edit( const OPimEvent&, bool showRec = TRUE ) = 0;

        virtual OPimEvent event() const = 0;

    protected:
        DescriptionManager descriptions()const;
        LocationManager locations()const;
        void setDescriptions( const DescriptionManager& );
        void setLocations( const LocationManager& );

        /**
         * Is the clock AM/PM or 24h?
         */
        bool isAP()const;

        /**
         * if the week starts on monday?
         */
        bool weekStartOnMonday()const;

        QString defaultLocation()const;
        QArray<int> defaultCategories()const;
        bool alarmPreset()const;
        int alarmPresetTime()const;

    private:
        MainWindow* m_win;

    };
}
}

#endif
