/*
 * addresspluginconfig.cpp
 *
 * copyright   : (c) 2003 by Stefan Eilers
 * email       : eilers.stefan@epost.de
 *
 * This implementation was derived from the todolist plugin implementation
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "addresspluginconfig.h"

#include <qpe/config.h>

#include <qlayout.h>
#include <qhbox.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qwhatsthis.h>



AddressBookPluginConfig::AddressBookPluginConfig( QWidget *parent,  const char* name)
    : TodayConfigWidget(parent,  name ) {

    QVBoxLayout * layout = new QVBoxLayout( this );
    layout->setMargin( 20 );

    QHBox *box1 = new QHBox( this );

    QLabel* TextLabel6 = new QLabel( box1, "TextLabel6" );
    TextLabel6->setText( tr( "Max Lines " ) );

    SpinBox2 = new QSpinBox( box1, "SpinBox2" );
    SpinBox2->setMaxValue( 40 );
    QWhatsThis::add( SpinBox2 , tr( "Set the maximum number of lines that should be shown for each" ) );

    QHBox *box2 = new QHBox( this );

    QLabel* clipLabel = new QLabel( box2, "" );
    clipLabel->setText( tr( "Clip line after X chars" ) );

    SpinBoxClip = new QSpinBox( box2, "SpinClip" );
    SpinBoxClip->setMaxValue( 200 );
    QWhatsThis::add( SpinBoxClip , tr( "After how many chars should be the info about the task be cut off" ) );

    QHBox *box3 = new QHBox( this );

    QLabel* daysLabel = new QLabel( box3, "" );
    daysLabel->setText( tr( "Days look ahead" ) );

    SpinDaysClip = new QSpinBox( box3, "SpinDays" );
    SpinDaysClip->setMaxValue( 200 );
    QWhatsThis::add( SpinDaysClip , tr( "How many days we should search forward" ) );

    layout->addWidget( box1 );
    layout->addWidget( box2 );
    layout->addWidget( box3 );

    readConfig();
}

void AddressBookPluginConfig::readConfig() {
    Config cfg( "todayaddressplugin" );
    cfg.setGroup( "config" );
    m_max_lines_task = cfg.readNumEntry( "maxlinestask", 5 );
    SpinBox2->setValue( m_max_lines_task );
    m_maxCharClip =  cfg.readNumEntry( "maxcharclip", 38 );
    SpinBoxClip->setValue( m_maxCharClip );
    m_daysLookAhead = cfg.readNumEntry( "dayslookahead", 14 );
    SpinDaysClip->setValue( m_daysLookAhead );
}


void AddressBookPluginConfig::writeConfig() {
    Config cfg( "todayaddressplugin" );
    cfg.setGroup( "config" );
    m_max_lines_task = SpinBox2->value();
    cfg.writeEntry( "maxlinestask", m_max_lines_task );
    m_maxCharClip = SpinBoxClip->value();
    cfg.writeEntry( "maxcharclip",  m_maxCharClip );
    m_daysLookAhead = SpinDaysClip->value();
    cfg.writeEntry( "dayslookahead",  m_daysLookAhead );
    cfg.write();
}


AddressBookPluginConfig::~AddressBookPluginConfig() {
}
