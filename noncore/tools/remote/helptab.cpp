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

#include "helptab.h"

HelpTab::HelpTab(QWidget *parent=0, const char *name=0):QWidget(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	QString *string = new QString("<qt><h1>Opie-Remote Usage Instructions</h1><p>First, some definitions.  A Remote is a remote entry in an lircd.conf file, it represents one remote you want to emulate.  A Remote Layout is one entry in your ~/Settings/Remote.conf file.  It represents the buttons that you see on your screen.  Each button on a Remote Layout can be mapped to any button in a Remote.  This way you can have, for example, a vcr remote layout, in which all the play/pause/etc buttons are mapped to the buttons on your vcr's remote.  However, most VCR's don't have volume controls, so the volume buttons can be mapped to the volume buttons on your TV.</p><p>The first things you need the lirc and lirc-modules ipkgs.  If you installed this from an ipkg, they should already be there, thanks to the wonderful world of dependencies.  If not, get them.  The next thing you need is an lircd.conf file.  you can get these at <a href=http://www.lirc.org/>http://www.lirc.org/</a>.  Read the documentation there to figure out how to make your own, if one for your remote doesn't exist, or how to have multiple remotes in one lircd.conf file.  Once you have a good lircd.conf file, put it in /etc, kill the lircd daemon (if its running) and do a modprobe lirc_sir.  Then, run lircd again.</p><p>The next thing you want to do is to create a remote layout.  Go to the config tab, and enter a name for your remote layout in the pulldown menu. Dont use the name Remotes, as that could confuse the app.  Hopefully, that will be fixed soon.  after entering the name you want to use, press New, and then select the name again from the pulldown menu (another oddity that i hope to fix).  Then, press each button that you want to map, and a dialog should appear.  Select the remote and button that you want to use, and click OK.  Once you are done, go to the Remote tab, and select the new remote from the dropdown menu.  It should works fine.  If at any time you want to change a remote layout, go to the Config tab, select the layout from the dropdown menu, and change the buttons you want to change.</p><p>This is program is written and maintaned by Thomas (spiralman) Stephens. <a href=mailto:spiralman@softhome.net>spiralman@softhome.net</a>.  Or, look for me on #opie or #handhelds.org on irc.openprojects.net.</p></qt>");
	QTextView *view = new QTextView((const QString &)*string, 0,  this, "view");
	layout->insertSpacing( -1, 5);
	layout->insertWidget(-1, view);
	layout->insertSpacing(-1, 5);
}