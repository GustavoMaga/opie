/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#include "oxygen.h"
#include "datawidgetui.h"

#include <qpe/config.h>
#include <qstring.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtable.h>
#include <qhbox.h>
#include <qlabel.h>

dataWidgetUI::dataWidgetUI() : QDialog()
{
    this->setCaption("Chemical Data");
    
    QGridLayout *qgrid = new QGridLayout( this, 2,1 );
    
    dataCombo = new QComboBox( this );
    OxydataWidget *oxyDW = new OxydataWidget(this);
    oxyDW->setElement( 1 );
    qgrid->addWidget(  dataCombo, 0,0);
    qgrid->addWidget(  oxyDW , 1,0 );
    
    connect ( dataCombo, SIGNAL( activated(int) ), oxyDW, SLOT( setElement(int) ) );
    Config test( "/home/opie/Settings/oxygendata", Config::File );
    
    //read in all names of the 118 elements
    for ( int i = 1 ; i < 119 ; i++ )
    {
        test.setGroup( QString::number( i ) );
        QString foo = test.readEntry( "Name" );
        dataCombo->insertItem( foo );
    }
}



void dataWidgetUI::slotShowData(int number){
    oxyDW->setElement( 6 );
}
