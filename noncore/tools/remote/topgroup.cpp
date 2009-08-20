/*
Opie-Remote.  emulates remote controlls on an iPaq (and maybe a Zaurus) in Opie.
Copyright (C) 2002 Thomas Stephens

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "topgroup.h"

TopGroup::TopGroup(QWidget *parent, const char *name):QWidget(parent,name)
{
	QHBoxLayout *layout = new QHBoxLayout(this, 0, -1, 0);

	QPushButton *power = new QPushButton("Power",this,"power");
	layout->addWidget(power);
	connect(power, SIGNAL(pressed()), this->parentWidget(), SLOT(sendIR()) );
//	power->setGeometry(5, 5,40, 20);

	layout->addSpacing(5);

	QPushButton *source = new QPushButton("Source",this,"source");
	layout->addWidget(source);
	connect(source, SIGNAL(pressed()), this->parentWidget(), SLOT(sendIR()) );
	//	source->setGeometry(50,5,40,20);

	remotes = new QComboBox(false, this, "remotes");
	connect(remotes, SIGNAL(activated(const QString&)), this->parentWidget(), SLOT(remoteSelected(const QString&)) );
	remotes->insertItem("Select Remote");
//	remotes->setGeometry(135,5,95,20);

	QLabel *remoteLabel = new QLabel(remotes, "Remote: ",this,"remoteLabel");
//	remoteLabel->setGeometry(90,5,40,20);
	remoteLabel->setAlignment(AlignRight | AlignVCenter);

	layout->addWidget(remoteLabel);
	layout->addWidget(remotes);
}

void TopGroup::updateRemotes(Config *cfg)
{
	// Save currently selected item if any
	QString curr_remote = remotes->currentText();

	remotes->clear();
	cfg->setGroup("Remotes");
	remotes->insertStringList(cfg->readListEntry("remoteList", ',') );

	// Select previously selected item
	if(curr_remote != "") {
		for(int i=0;i<remotes->count();i++) {
			if(remotes->text(i) == curr_remote) {
				remotes->setCurrentItem(i);
				break;
			}
		}
	}
}

QString TopGroup::getRemotesText()
{
	return remotes->currentText();
}
