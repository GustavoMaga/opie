#include "configdlg.h"
#include "ocontactfields.h"

#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qiconset.h> 

#include <qpe/resource.h>

#include <opie/ocontact.h>

ConfigDlg::ConfigDlg( QWidget *parent, const char *name):
	ConfigDlg_Base(parent, name, true )
{
	contFields = OContactFields::trfields();

	// We add all Fields into the Listbox
	for (uint i=0; i < contFields.count(); i++) {
		allFieldListBox->insertItem( contFields[i] );
	}

	// Set Pics to Buttons and Tabs
	m_upButton->setIconSet( QIconSet( Resource::loadPixmap( "addressbook/up" ) ) );
	m_downButton->setIconSet( QIconSet( Resource::loadPixmap( "addressbook/down" ) ) );
	m_addButton->setIconSet( QIconSet( Resource::loadPixmap( "addressbook/add" ) ) );
	m_removeButton->setIconSet( QIconSet( Resource::loadPixmap( "addressbook/sub" ) ) );
	

	// Get the translation maps between Field ID and translated strings
	m_mapStrToID = OContactFields::trFieldsToId();
	m_mapIDToStr = OContactFields::idToTrFields();

	connect ( m_addButton, SIGNAL( clicked() ), this, SLOT( slotItemAdd() ) );
	connect ( m_removeButton, SIGNAL( clicked() ), this, SLOT( slotItemRemove() ) );
	connect ( m_upButton, SIGNAL( clicked() ), this, SLOT( slotItemUp() ) );
	connect ( m_downButton, SIGNAL( clicked() ), this, SLOT( slotItemDown() ) );
}

void ConfigDlg::slotItemUp()
{
	qWarning( "void ConfigDlg::slotItemUp()" );

	int i = fieldListBox->currentItem();
	if ( i > 0 ) {
		QString item = fieldListBox->currentText();
		fieldListBox->removeItem( i );
		fieldListBox->insertItem( item, i-1 );
		fieldListBox->setCurrentItem( i-1 );
	}

}

void ConfigDlg::slotItemDown()
{
	qWarning( "void ConfigDlg::slotItemDown()" );

	int i = fieldListBox->currentItem();
	if ( i < (int)fieldListBox->count() - 1 ) {
		QString item = fieldListBox->currentText();
		fieldListBox->removeItem( i );
		fieldListBox->insertItem( item, i+1 );
		fieldListBox->setCurrentItem( i+1 );
	}
}

void ConfigDlg::slotItemAdd()
{
	qWarning( "void ConfigDlg::slotItemAdd()" );

	int i = allFieldListBox->currentItem();
	if ( i > 0 ) {
		QString item = allFieldListBox->currentText();
		qWarning("ADding %s", item.latin1());
		fieldListBox->insertItem( item );
	}	
}

void ConfigDlg::slotItemRemove()
{
	qWarning( "void ConfigDlg::slotItemRemove()" );

	int i = fieldListBox->currentItem();
	if ( i > 0 ) {
		fieldListBox->removeItem( i );
	}	
}
    
void ConfigDlg::setConfig( const AbConfig& cnf )
{ 
	m_config = cnf; 

	m_useRegExp->setChecked( m_config.useRegExp() );
	m_useWildCard->setChecked( m_config.useWildCards() );
	m_useQtMail->setChecked( m_config.useQtMail() );
	m_useOpieMail->setChecked( m_config.useOpieMail() );
	m_useCaseSensitive->setChecked( m_config.beCaseSensitive() );

	switch( m_config.fontSize() ){
	case 0:
		m_smallFont->setChecked( true );
		m_normalFont->setChecked( false );
		m_largeFont->setChecked( false );
		break;
	case 1: 
		m_smallFont->setChecked( false );
		m_normalFont->setChecked( true );
		m_largeFont->setChecked( false );
		break;
	case 2: 
		m_smallFont->setChecked( false );
		m_normalFont->setChecked( false );
		m_largeFont->setChecked( true );
		break;
	}

	for( uint i = 0; i < m_config.orderList().count(); i++ ) {
	        fieldListBox -> insertItem ( m_mapIDToStr[ m_config.orderList()[i] ] );
	}

	
}
    
AbConfig ConfigDlg::getConfig()
{ 
	m_config.setUseRegExp( m_useRegExp->isOn() );
	m_config.setUseWildCards( m_useWildCard->isOn() );
	m_config.setUseQtMail( m_useQtMail->isOn() );
	m_config.setUseOpieMail( m_useOpieMail->isOn() );
	m_config.setBeCaseSensitive( m_useCaseSensitive->isChecked() );

	if ( m_smallFont->isChecked() )
		m_config.setFontSize( 0 );
	if ( m_normalFont->isChecked() )
		m_config.setFontSize( 1 );
	if ( m_largeFont->isChecked() )
		m_config.setFontSize( 2 );

	QValueList<int> orderlist;
	for( int i = 0; i < (int)fieldListBox->count(); i++ ) {
	        orderlist.append( m_mapStrToID[ fieldListBox->text(i) ] );
	}
	m_config.setOrderList( orderlist );

	return m_config;
}

