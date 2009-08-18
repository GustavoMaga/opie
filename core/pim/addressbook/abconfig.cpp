#include "abconfig.h"
#include "version.h"

#include <qpe/config.h>
#include <qpe/recordfields.h>

AbConfig::AbConfig( ):
    m_useQtMail( false ),
    m_useOpieMail( true ),
    m_useRegExp( false ),
    m_beCaseSensitive( false ),
    m_fontSize( 1 ),
    m_barPos( QMainWindow::Top ),
    m_fixedBars( true ),
    m_lpSearchMode( LastName ),
    m_changed( false )
{
}

AbConfig::~AbConfig()
{
}

bool AbConfig::useRegExp() const
{
    return m_useRegExp;
}
bool AbConfig::useWildCards() const
{
    return !m_useRegExp;
}
bool AbConfig::useQtMail() const
{
    return m_useQtMail;
}
bool AbConfig::useOpieMail() const
{
    return m_useOpieMail;
}
bool AbConfig::beCaseSensitive() const
{
    return m_beCaseSensitive;
}
int AbConfig::fontSize() const
{
    return m_fontSize;
}

QValueList<int> AbConfig::orderList() const
{
    return m_ordered;
}

QMainWindow::ToolBarDock AbConfig::getToolBarPos() const
{
    return (QMainWindow::ToolBarDock) m_barPos;
}

bool AbConfig::fixedBars() const
{
    return m_fixedBars;
}

AbConfig::LPSearchMode AbConfig::letterPickerSearch() const
{
    return ( AbConfig::LPSearchMode ) m_lpSearchMode;
}

const QString &AbConfig::category() const
{
    return m_category;
}

void AbConfig::setUseRegExp( bool v )
{
    m_useRegExp = v ;
    m_changed = true;
}
void AbConfig::setUseWildCards( bool v )
{
    m_useRegExp = !v;
    m_changed = true;
}
void AbConfig::setBeCaseSensitive( bool v )
{
    m_beCaseSensitive = v;
    m_changed = true;
}
void AbConfig::setUseQtMail( bool v )
{
    m_useQtMail = v;
    m_changed = true;
}
void AbConfig::setUseOpieMail( bool v )
{
    m_useOpieMail = v;
    m_changed = true;
}
void AbConfig::setFontSize( int v )
{
    m_fontSize = v;
    m_changed = true;
}

void AbConfig::setOrderList( const QValueList<int>& list )
{
    m_ordered = list;
    m_changed = true;
}

void AbConfig::setToolBarDock( const QMainWindow::ToolBarDock v )
{
    m_barPos = v;
    m_changed = true;
}

void AbConfig::setFixedBars( const bool fixed )
{
    m_fixedBars = fixed;
    m_changed = true;
}

void AbConfig::setLetterPickerSearch( const AbConfig::LPSearchMode mode )
{
    m_lpSearchMode = mode;
    m_changed = true;
}

void AbConfig::setCategory( const QString &cat )
{
    m_category = cat;
}

void AbConfig::load()
{
    // Read Config settings
    Config cfg("AddressBook");

    cfg.setGroup( "View" );
    m_category = cfg.readEntry( "Category", "All" );

    cfg.setGroup("Font");
    m_fontSize = cfg.readNumEntry( "fontSize", 1 );

    cfg.setGroup("Search");
    m_useRegExp = cfg.readBoolEntry( "useRegExp", false );
    m_beCaseSensitive = cfg.readBoolEntry( "caseSensitive", false );
    m_lpSearchMode = cfg.readNumEntry( "lpSearchMode", FileAs );

    cfg.setGroup("Mail");
    m_useQtMail = cfg.readBoolEntry( "useQtMail", false );
    m_useOpieMail = cfg.readBoolEntry( "useOpieMail", true );

    cfg.setGroup("ContactOrder");
    int ID = 0;
    int i = 0;
    ID = cfg.readNumEntry( "ContactID_"+QString::number(i++), 0 );
    while ( ID != 0 ){
        m_ordered.append( ID );
        ID = cfg.readNumEntry( "ContactID_"+QString::number(i++), 0 );
    }

    // If no contact order is defined, we set the default
    if ( m_ordered.count() == 0 ) {
        m_ordered.append( Qtopia::DefaultEmail );
        m_ordered.append( Qtopia::HomePhone);
        m_ordered.append( Qtopia::HomeMobile);
        m_ordered.append( Qtopia::BusinessPhone);
    }

    cfg.setGroup("ToolBar");
    m_barPos = cfg.readNumEntry( "Position", QMainWindow::Top );
    m_fixedBars= cfg.readBoolEntry( "fixedBars", true );

    m_changed = false;
}

void AbConfig::save()
{
    if ( m_changed ){
        Config cfg("AddressBook");
        cfg.setGroup( "View" );
        cfg.writeEntry( "Category", m_category );

        cfg.setGroup("Font");
        cfg.writeEntry("fontSize", m_fontSize);

        cfg.setGroup("Search");
        cfg.writeEntry("useRegExp", m_useRegExp);
        cfg.writeEntry("caseSensitive", m_beCaseSensitive);
        cfg.writeEntry("lpSearchMode", m_lpSearchMode );

        cfg.setGroup("Mail");
        cfg.writeEntry( "useQtMail", m_useQtMail );
        cfg.writeEntry( "useOpieMail", m_useOpieMail);

        cfg.setGroup("ContactOrder");
        cfg.clearGroup();
        for ( uint i = 0; i < m_ordered.count(); i++ ){
            cfg.writeEntry( "ContactID_"+QString::number(i), m_ordered[i] );
        }

        cfg.setGroup("ToolBar");
        cfg.writeEntry( "Position", m_barPos );
        cfg.writeEntry( "fixedBars", m_fixedBars );

        cfg.setGroup("Version");
        cfg.writeEntry( "AppName", APPNAME + QString(" V" ) + MAINVERSION + QString(".") + SUBVERSION + QString(".") + PATCHVERSION);
        cfg.writeEntry( "Mainversion", MAINVERSION );
        cfg.writeEntry( "SubVersion", SUBVERSION );
        cfg.writeEntry( "PatchVersion", PATCHVERSION );

    }

}

void AbConfig::operator= ( const AbConfig& cnf )
{
    m_useQtMail = cnf.m_useQtMail;
    m_useOpieMail = cnf.m_useOpieMail;
    m_useRegExp = cnf.m_useRegExp;
    m_beCaseSensitive = cnf.m_beCaseSensitive;
    m_fontSize = cnf.m_fontSize;
    m_ordered = cnf.m_ordered;
    m_barPos = cnf.m_barPos;
    m_fixedBars = cnf.m_fixedBars;
    m_lpSearchMode = cnf.m_lpSearchMode;
}

