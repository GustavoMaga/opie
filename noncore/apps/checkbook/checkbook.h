/*
� � � � � � � �              This file is part of the OPIE Project
               =.
� � � � � � �.=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This file is free software; you can
.> <`_, � > �. � <=          redistribute it and/or modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
� � .%`+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This file is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful, but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|`    MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.= � � � = � � � ;      Public License for more details.
++= � -. � � .` � � .:
�: � � = �...= . :.=-        You should have received a copy of the GNU
�-. � .:....=;==+<;          General Public License along with this file;
� -_. . . � )=. �=           see the file COPYING. If not, write to the
� � -- � � � �:-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef CHECKBOOK_H
#define CHECKBOOK_H

#include "traninfo.h"

#include <qdialog.h>
#include <qlistview.h>

class OTabWidget;

class Graph;
class GraphInfo;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListView;
class QMultiLineEdit;
class QString;

class Checkbook : public QDialog
{
	Q_OBJECT

	public:
		Checkbook( QWidget * = 0x0, const QString & = 0x0, const QString & = 0x0,
					const QString & = "$" );
		~Checkbook();

		const QString &getName();

	private:
		TranInfoList transactions;
		QString      name;
		QString      filename;
		QString      filedir;
		QString      currencySymbol;
		QString      password;
		int          highTranNum;

		OTabWidget *mainWidget;
		void        loadCheckbook();
		void        adjustBalance( float );
		TranInfo   *findTran( const QString &, const QString &, const QString & );

		// Info tab
		QWidget        *initInfo();
		QCheckBox      *passwordCB;
		QLineEdit      *nameEdit;
		QComboBox      *typeList;
		QLineEdit      *bankEdit;
		QLineEdit      *acctNumEdit;
		QLineEdit      *pinNumEdit;
		QLineEdit      *balanceEdit;
		QMultiLineEdit *notesEdit;
		float           startBalance;

		// Transactions tab
		QWidget   *initTransactions();
		QListView *tranTable;
		QLabel    *balanceLabel;
		float      currBalance;

		// Charts tab
		QWidget   *initCharts();
		GraphInfo *graphInfo;
		QComboBox *graphList;
		Graph     *graphWidget;

		void drawBalanceChart();
		void drawCategoryChart( bool = TRUE );

	protected slots:
		void accept();

	private slots:
		void slotPasswordClicked();
		void slotNameChanged( const QString & );
		void slotStartingBalanceChanged( const QString & );
		void slotNewTran();
		void slotEditTran();
		void slotDeleteTran();
		void slotDrawGraph();
};

class CBListItem :  public QListViewItem
{
	//Q_OBJECT

	public:
		CBListItem( QListView *, QString = QString::null, QString = QString::null,
					 QString = QString::null, QString = QString::null, QString = QString::null,
					 QString = QString::null, QString = QString::null, QString = QString::null );

		void paintCell( QPainter *, const QColorGroup &, int, int, int );

	private:
		QListView *owner;
		bool m_known;
		bool m_odd;

		bool isAltBackground();
};

#endif
