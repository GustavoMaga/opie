#ifndef _CONVERTER_H_
#define _CONVERTER_H_


#include "converter_base.h"


class Converter: public converter_base {
public:
        Converter(QWidget *parent, const char* name, WFlags fl);
	static QString appName() { return QString::fromLatin1("opimconverter");}

	// Slots defined in the ui-description file
	void start_conversion();
	void start_upgrade();
	void selectedDatabase( int num );
	void selectedDestFormat( int num );
	void selectedSourceFormat( int num );

	void closeEvent( QCloseEvent *e );
	
private:
	// Caution:
	// The order and value of the following enums must be regarding
	// the predefinition in the UI-File !!
	// If you don't understand what I am talking about: Keep your fingers away!!
	enum DataBases{
		ADDRESSBOOK = 0,
		TODOLIST    = 1,
		DATEBOOK    = 2,
	};

	enum DbFormats{
		XML = 0,
		SQL = 1,
	};

	int m_selectedDatabase;
	int m_selectedSourceFormat;
	int m_selectedDestFormat;
	bool m_criticalState;

};


#endif
