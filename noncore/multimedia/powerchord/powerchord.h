#ifndef POWERCHORD_H
#define POWERCHORD_H
#include "powerchordbase.h"

class Powerchord : public PowerchordBase
{
    Q_OBJECT

public:
    Powerchord( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Powerchord();

    static QString appName() { return QString::fromLatin1("powerchord"); }

private slots:
    void goodBye();
};

#endif // POWERCHORD_H
