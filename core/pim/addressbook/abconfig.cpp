#include "abconfig.h"
#include <qpe/config.h>

AbConfig::AbConfig( ):
	m_useQtMail( true ),
	m_useOpieMail( false ),
	m_useRegExp( false ),
	m_beCaseSensitive( false ),
	m_fontSize( 1 ),
	m_changed( false )
{
	load();
}

AbConfig::~AbConfig()
{
	save();
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
void AbConfig::load()
{
	// Read Config settings
	Config cfg("AddressBook");
	cfg.setGroup("Font");
	m_fontSize = cfg.readNumEntry( "fontSize", 1 );
	cfg.setGroup("Search");
	m_useRegExp = cfg.readBoolEntry( "useRegExp" );
	m_beCaseSensitive = cfg.readBoolEntry( "caseSensitive" );
	cfg.setGroup("Mail");
	m_useQtMail = cfg.readBoolEntry( "useQtMail", true );
	m_useOpieMail=cfg.readBoolEntry( "useOpieMail" );

	m_changed = false;
}

void AbConfig::save()
{
	if ( m_changed ){
		Config cfg("AddressBook");
		cfg.setGroup("Font");
		cfg.writeEntry("fontSize", m_fontSize);

		cfg.setGroup("Search");
		cfg.writeEntry("useRegExp", m_useRegExp);
		cfg.writeEntry("caseSensitive", m_beCaseSensitive);

		cfg.setGroup("Mail");
		cfg.writeEntry( "useQtMail", m_useQtMail );
		cfg.writeEntry( "useOpieMail", m_useOpieMail);
	}
	
}

void AbConfig::operator= ( const AbConfig& cnf )
{
	m_useQtMail = cnf.m_useQtMail;
	m_useOpieMail = cnf.m_useOpieMail;
	m_useRegExp = cnf.m_useRegExp;
	m_beCaseSensitive = cnf.m_beCaseSensitive;
	m_fontSize = cnf.m_fontSize;

}

