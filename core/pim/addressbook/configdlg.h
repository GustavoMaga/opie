#ifndef _CONFIGDLG_H_
#define _CONFIGDLG_H_

#include "configdlg_base.h"

class ConfigDlg: public ConfigDlg_Base
{
    Q_OBJECT
public:
    ConfigDlg( QWidget *parent = 0, const char *name = 0 );
    
    // Search Settings
    bool useRegExp() const;
    bool useWildCards() const;
    bool beCaseSensitive() const;
    bool signalWrapAround() const;
    
    void setUseRegExp( bool v );
    void setUseWildCards( bool v );
    void setBeCaseSensitive( bool v ); 
    void setSignalWrapAround( bool v );
};


#endif
