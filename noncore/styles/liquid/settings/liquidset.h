#ifndef __OPIE_LIQUID_SET_H__
#define __OPIE_LIQUID_SET_H__

#include <qdialog.h>
#include <qcolor.h>

class QLabel;
class QToolButton;
class QSlider;

class LiquidSet : public QDialog {
	Q_OBJECT
	
public:
	LiquidSet ( QWidget *parent = 0, const char *name = 0, WFlags fl = 0 );

public slots:
	void changeType ( int t );
	void changeMenuColor ( const QColor &col );
	void changeTextColor ( const QColor &col );
	void changeShadow ( bool b );

protected:
	virtual void accept ( );
	
private:
	QColor m_menucol;
	QColor m_textcol;
	int    m_type;
	bool   m_shadow;
	
	QSlider *    m_opacsld;
	QLabel *     m_menulbl;
	QLabel *     m_textlbl;
	QLabel *     m_opaclbl;
	QToolButton *m_menubtn;
	QToolButton *m_textbtn;
};
#endif
