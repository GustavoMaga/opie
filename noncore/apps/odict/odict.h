/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

#include <qmainwindow.h>

class QVBox;
class QPopupMenu;
class QMenuBar;
class QHBox;
class QPushButton;
class QLineEdit;
class QAction;
class QVBoxLayout;
class QActionGroup;
class DingWidget;
class QTextBrowser;
class QComboBox;
class DingWidget;

class ODict : public QMainWindow
{
	Q_OBJECT
	
    public:
		ODict();
		QVBox *vbox;
		QTextBrowser *browser_top,
					 *browser_bottom;
		DingWidget *ding;

	private:
		QPopupMenu *help, *settings, *parameter, *error_tol_menu;
		QMenuBar *menu;
		QHBox *hbox;
		QLineEdit *query_le;
		QComboBox *query_co;
		QPushButton *ok_button;

		QVBoxLayout *vbox_layout;

		QAction *setting_a, *setting_b;
		
		void setupMenus();

		int errorTol;
		bool casesens, completewords, regexp;
		
		void loadConfig();
		void saveConfig();

		QString activated_name;
	
	private slots:
		void slotDisplayAbout();
		void slotStartQuery();
		void slotSetErrorcount( int );
		void slotSettings();
		void slotSetParameter( int );
		void slotMethodChanged( int );
};
