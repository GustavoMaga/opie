/*
 *
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: general.h,v 1.6 2004-10-14 01:44:27 zecke Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _GENERAL_H_
#define _GENERAL_H_

#include <qwidget.h>

class QSlider;
class QSpinBox;
class QCombobox;
class QLabel;
class QCheckBox;
class QComboBox;
class PPPData;
class InterfacePPP;
class QLineEdit;

class ModemWidget : public QWidget {
  Q_OBJECT
public:
  ModemWidget(PPPData*, QWidget *parent=0, const char *name=0 );
  ~ModemWidget();

  bool save();

private slots:
    /*
     * temporarily commit configuration so queryModem
     * will use 'current' settings
     */
    void slotBeforeModemQuery();
    void slotAfterModemQuery();

/* private slots: */
/*   void 	setmodemdc(int); */
/*   void  setmodemdc(const QString &); */
/*   void 	setflowcontrol(int); */
/*   void 	modemtimeoutchanged(int); */
/*   void 	modemlockfilechanged(bool); */
/*   void 	setenter(int); */
/*   void  speed_selection(int); */

private:
  QComboBox 	*enter;
/*   QLabel 	*label1; */
/*   QLabel 	*label2; */
/*   QLabel 	*labeltmp; */
/*   QLabel 	*labelenter; */
  QLineEdit     *modemname;
  QComboBox 	*modemdevice;
  QComboBox 	*flowcontrol;

  QComboBox *baud_c;
  QLabel *baud_label;

  QSpinBox 	*modemtimeout;
  QCheckBox     *modemlockfile;
  PPPData *_pppdata;

private:
    QString m_oldModemDev, m_oldSpeed;
    bool m_oldModemLock;
    int m_oldModemTimeout, m_oldFlowControl, m_oldLineEnd;
};


class ModemWidget2 : public QWidget {
  Q_OBJECT
public:
  ModemWidget2( PPPData*, InterfacePPP*, QWidget *parent=0, const char *name=0 );
  bool save();

signals:
    void sig_beforeQueryModem();
    void sig_afterQueryModem();

private slots:
/*   void  waitfordtchanged(bool); */
/*   void 	busywaitchanged(int); */
//  void 	use_cdline_toggled(bool);
  void 	modemcmdsbutton();
  //  void 	terminal();
  void 	query_modem();
  //  void  volumeChanged(int);

private:
  QLabel 	*labeltmp;
  QPushButton 	*modemcmds;
  QPushButton 	*modeminfo_button;
  //  QPushButton 	*terminal_button;
  //  QFrame 	*fline;
  QCheckBox     *waitfordt;
  QSpinBox 	*busywait;
  QCheckBox 	*chkbox1;
  QSlider       *volume;
  PPPData *_pppdata;
  InterfacePPP *_ifaceppp;
};

#endif


