/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Opie Sheet (formerly Sheet/Qt)
 * by Serdar Ozler <sozler@sitebest.com>
 */

#ifndef SORTDLG_H
#define SORTDLG_H

#include <qdialog.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qvbuttongroup.h>
#include "sheet.h"

class SortDialog: public QDialog
{
  Q_OBJECT

  // QT objects
  QBoxLayout *box;
  QTabWidget *tabs;
  QWidget *widgetSort, *widgetOptions;
  QVButtonGroup *groupOrderA, *groupOrderB, *groupOrderC, *groupDirection;
  QCheckBox *checkCase;
  QComboBox *comboFieldA, *comboFieldB, *comboFieldC;

  // Other objects & variables
  int row1, col1, row2, col2, direction;
  Sheet *sheet;

  // Private functions
  QVButtonGroup *createOrderButtons(int y);
  QComboBox *createFieldCombo(const QString &caption, int y);
  void fillFieldCombo(QComboBox *combo);
  int compareItems(QTableItem *item1, QTableItem *item2, int descending=0, bool caseSensitive=TRUE);

  private slots:
    void directionChanged(int id);

  public:
    SortDialog(QWidget *parent=0);
    ~SortDialog();

    int exec(Sheet *s);
};

#endif
