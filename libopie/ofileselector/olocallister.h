#ifndef OPIE_LOCAL_LISTER
#define OPIE_LOCAL_LISTER

#include "olister.h"

class OLocalLister : public OLister {
public:
    OLocalLister( OFileSelector* );
    ~OLocalLister();
    void reparse( const QString& path );
    QMap<QString, QStringList> mimeTypes(const QString& dir );
};

#endif
