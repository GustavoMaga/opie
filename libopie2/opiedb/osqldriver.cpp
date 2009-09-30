#include <qpe/qlibrary.h>

#include "osqldriver.h"

using namespace Opie::DB;

OSQLDriver::OSQLDriver( QLibrary* lib )
    : QObject()
    , m_lib(lib)
    , d(0)
{
}

OSQLDriver::~OSQLDriver()
{
    delete m_lib;
}

bool OSQLDriver::sync()
{
    return true;
}
