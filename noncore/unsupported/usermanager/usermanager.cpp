/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "usermanager.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oresource.h>
using namespace Opie::Core;
#include <qpe/qpeapplication.h>
/* QT */
#include <qlayout.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qregexp.h>

/**
 * The mainwindow constructor.
 *
 * @param QWidget *parent
 * @param const char *name
 * @ param WFlags fl
 *
 */
UserConfig::UserConfig(QWidget* parent, const char* name, WFlags fl) : QMainWindow(parent, name, fl) {
	setCaption(tr("Opie User Manager"));
	
	// Create an instance of the global object 'accounts'. This holds all user/group info, and functions to modify them.
	accounts=new Passwd();
	accounts->open();	// This actually loads the files /etc/passwd & /etc/group into memory.

	// Create the toolbar.
    setToolBarsMovable( false );
    bool useBigIcon = qApp->desktop()->size().width() > 330;
	QToolBar *toolbar = new QToolBar(this,"Toolbar");
    toolbar->setHorizontalStretchable( true );
	adduserToolButton = new QToolButton(Opie::Core::OResource::loadPixmap("usermanager/adduser", Opie::Core::OResource::SmallIcon),
                                        "Add User",0,this,SLOT(addUser()),toolbar,"Add User");
    adduserToolButton->setUsesBigPixmap( useBigIcon );
    edituserToolButton = new QToolButton(Opie::Core::OResource::loadPixmap("usermanager/edituser", Opie::Core::OResource::SmallIcon),
                                         "Edit User",0,this,SLOT(editUser()),toolbar,"Edit User");
    edituserToolButton->setUsesBigPixmap( useBigIcon );
	deleteuserToolButton = new QToolButton(Opie::Core::OResource::loadPixmap("usermanager/deleteuser", Opie::Core::OResource::SmallIcon),
                                           "Delete User",0,this,SLOT(delUser()),toolbar,"Delete User");
    deleteuserToolButton->setUsesBigPixmap( useBigIcon );
	//QToolButton *userstext = new QToolButton(0,"User",0,0,0,toolbar,"User");
	//userstext->setUsesTextLabel(true);
	toolbar->addSeparator();
	addgroupToolButton = new QToolButton(Opie::Core::OResource::loadPixmap("usermanager/addgroup", Opie::Core::OResource::SmallIcon),
                                         "Add Group",0,this,SLOT(addGroup()),toolbar,"Add Group");
	addgroupToolButton->setUsesBigPixmap( useBigIcon );
    editgroupToolButton = new QToolButton(Opie::Core::OResource::loadPixmap("usermanager/editgroup", Opie::Core::OResource::SmallIcon),
                                          "Edit Group",0,this,SLOT(editGroup()),toolbar,"Edit Group");
    editgroupToolButton->setUsesBigPixmap( useBigIcon );
	deletegroupToolButton = new QToolButton(Opie::Core::OResource::loadPixmap("usermanager/deletegroup", Opie::Core::OResource::SmallIcon),
                                            "Delete Group",0,this,SLOT(delGroup()),toolbar,"Delete Group");
    deletegroupToolButton->setUsesBigPixmap( useBigIcon );
	//QToolButton *groupstext = new QToolButton(0,"Group",0,0,0,toolbar,"Group");
	//groupstext->setUsesTextLabel(true);
	addToolBar(toolbar,"myToolBar");

	// Add a tabwidget and all the tabs.
	myTabWidget = new QTabWidget(this,"My Tab Widget");
	setupTabAccounts();
	setupTabAllUsers();
	setupTabAllGroups();
	userPopupMenu.insertItem("Copy",0);
	
	getUsers(); // Fill out the iconview & listview with all users.
	getGroups(); // Fill out the group listview with all groups.
	
	setCentralWidget(myTabWidget);
}

UserConfig::~UserConfig() {
	accounts->close();
	delete accounts;
}

void UserConfig::setupTabAccounts() {
	QWidget *tabpage = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(tabpage);
	layout->setMargin(5);
	
	usersIconView=new QListView(tabpage,"users");
	usersIconView->addColumn("Icon");
	usersIconView->addColumn("Username");
	usersIconView->setAllColumnsShowFocus(true);
	layout->addWidget(usersIconView);
	
	connect(usersIconView,SIGNAL(returnPressed(QListViewItem*)),this,SLOT(showUserMenu(QListViewItem*)));
	
	myTabWidget->addTab(tabpage,"Users");
}

void UserConfig::setupTabAllUsers() {
	QWidget *tabpage = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(tabpage);
	layout->setMargin(5);
	
	usersListView=new QListView(tabpage,"allusers");
	usersListView->addColumn("UID");
	usersListView->addColumn("Login");
	usersListView->addColumn("Username");
	layout->addWidget(usersListView);
	usersListView->setSorting(1,1);
	usersListView->setAllColumnsShowFocus(true);

	myTabWidget->addTab(tabpage,"All Users");
}

void UserConfig::setupTabAllGroups() {
	QWidget *tabpage = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(tabpage);
	layout->setMargin(5);

	groupsListView=new QListView(tabpage,"groups");
	groupsListView->addColumn("GID");
	groupsListView->addColumn("Groupname");
	layout->addWidget(groupsListView);
	groupsListView->setSorting(1,1);
	groupsListView->setAllColumnsShowFocus(true);
	
	myTabWidget->addTab(tabpage,"All Groups");
}
void UserConfig::getUsers() {
	QString mytext;
	QPixmap mypixmap;
	QListViewItem *listviewitem;
			
	// Empty the iconview & the listview.
	usersIconView->clear();
	usersListView->clear();
	
	// availableUID is used as a deposite for the next available UID on the system, this should start at an ID over 500.
	availableUID=500;
	for(QStringList::Iterator it=accounts->passwdStringList.begin(); it!=accounts->passwdStringList.end(); ++it) {
		accounts->splitPasswdEntry(*it); // Split the string into it's components and store in variables in the accounts object. ("pr_name" and so on.)
		if(accounts->pw_name.find(QRegExp("^#"),0)) {	// Skip commented lines.
			new QListViewItem(usersListView,QString::number(accounts->pw_uid),accounts->pw_name,accounts->pw_gecos);
			if((accounts->pw_uid>=500) && (accounts->pw_uid<65000)) {	// Is this user a "normal" user ?
				mytext=QString(accounts->pw_name)+" - ("+QString(accounts->pw_gecos)+")"; // The string displayed next to the icon.
				if(!(mypixmap.load("/opt/QtPalmtop/pics/users/"+accounts->pw_name+".png"))) { //  Is there an icon for this user? Opie::Core::OResource::loadPixmap is caching, doesn't work.
					mypixmap=Opie::Core::OResource::loadPixmap("usermanager/usericon", Opie::Core::OResource::SmallIcon);	// If this user has no icon, load the default icon.
				}
				listviewitem=new QListViewItem(usersIconView,"",mytext);	// Add the icon+text to the qiconview.
				listviewitem->setPixmap(0,mypixmap);
			}
			if((accounts->pw_uid>=availableUID) && (accounts->pw_uid<65000)) availableUID=accounts->pw_uid+1; // Increase 1 to the latest know UID to get a free uid.
		}
	}
	usersIconView->sort();
}

void UserConfig::addUser() {
	if(UserDialog::addUser(availableUID,availableGID)) {	// Add the user to the system, also send next available UID and GID.
		getUsers(); // Update users views.
		getGroups(); // Update groups view.
	}
}

void UserConfig::editUser() {
	QString username;
	if(myTabWidget->currentPageIndex()==0) {	// Users
		if(usersIconView->currentItem()) {	// Any icon selected?
			username=usersIconView->currentItem()->text(1);	// Get the text associated with the icon.
			username=username.left(username.find(" - (",0,true));	// Strip out the username.
			if(UserDialog::editUser(username)) {	// Bring up the userinfo dialog.
				// If there were any changed also update the views.
				getUsers();
				getGroups();
			}
		} else {
			QMessageBox::information(this,"No selection.","No user has been selected.");
		}
	}
	if(myTabWidget->currentPageIndex()==1) {	// All users
		if(usersListView->currentItem()) {	// Anything changed!?
			username=usersListView->currentItem()->text(1);	// Get the username.
			if(UserDialog::editUser(username)) {	// Bring up the userinfo dialog.
				// And again update the views if there were any changes.
				getUsers();
				getGroups();
			}
		} else  {
			QMessageBox::information(this,"No selection.","No user has been selected.");
		}
	}
}

void UserConfig::delUser() {
	QString username;
	
	if(myTabWidget->currentPageIndex()==0) {	// Users, Iconview.
		if(usersIconView->currentItem()) {	// Anything selected?
			username=usersIconView->currentItem()->text(1);	// Get string associated with icon.
			username=username.left(username.find(" - (",0,true));	// Strip out the username.
			if(QMessageBox::warning(this,"Delete user","Are you sure you want to\ndelete this user? \""+QString(username)+"\" ?","&No","&Yes",0,0,1)) {
				if(UserDialog::delUser(username)) {	// Delete the user if possible.
					// Update views.
					getUsers();
					getGroups();
				}
			}
		} else  {
			QMessageBox::information(this,"No selection","No user has been selected.");
		}
	}
	if(myTabWidget->currentPageIndex()==1) {	// All users
		if(usersListView->currentItem()) {	// Anything changed!?
			username=usersListView->currentItem()->text(1);	// Get the username.
			if(QMessageBox::warning(this,"Delete user","Are you sure you want to\ndelete this user? \""+QString(username)+"\" ?","&No","&Yes",0,0,1)) {
				if(UserDialog::delUser(username)) {	// Try to delete the user.
					// Update views.
					getUsers();
					getGroups();
				}
			}
		} else  {
			QMessageBox::information(this,"No selection","No user has been selected.");
		}
	}
	
}

void UserConfig::getGroups() {
	groupsListView->clear();	// Empty the listview.
	availableGID=500;	// We need to find the next free GID, and are only interested in values between 500 & 65000.
	for(QStringList::Iterator it=accounts->groupStringList.begin(); it!=accounts->groupStringList.end(); ++it) {	// Split the list into lines.
		accounts->splitGroupEntry(*it);	// Split the line into its components and fill the variables of 'accounts'. (gr_name, gr_uid & gr_mem).
		if(accounts->gr_name.find(QRegExp("^#"),0)) {	// Skip commented lines.
			new QListViewItem(groupsListView,QString::number(accounts->gr_gid),accounts->gr_name);
			if((accounts->gr_gid>=availableGID) && (accounts->gr_gid<65000)) availableGID=accounts->gr_gid+1;	// Maybe a new free GID.
		}
	}
}

void UserConfig::addGroup() {
	if(GroupDialog::addGroup(availableGID)) getGroups();	// Bring up the add group dialog.
}

void UserConfig::editGroup() {
	int gid;
	if(groupsListView->currentItem()) {	// Any group selected?
		gid=groupsListView->currentItem()->text(0).toInt();	// Get the GID from the listview.
		if(GroupDialog::editGroup(gid)) getGroups();	// Bring up the edit group dialog.
	} else  {
		QMessageBox::information(this,"No selection","No group has been selected.");
	}
}

void UserConfig::delGroup() {
	const char *groupname;
	if(groupsListView->currentItem()) {	// Any group selected?
		groupname=groupsListView->currentItem()->text(1);	// Get the groupname from the listview.
		if(QMessageBox::warning(this,"Delete group","Are you sure you want to\ndelete the group \""+QString(groupname)+"\" ?","&No","&Yes",0,0,1)) {
			// If confirmed, try to delete the group.
			if(GroupDialog::delGroup(groupname)) getGroups(); // And also update the view afterwards if the user was deleted.
		}
	} else  {
		QMessageBox::information(this,"No selection","No group has been selected.");
	}
}

void UserConfig::showUserMenu(QListViewItem *item) {
//	userPopupMenu.exec(item->mapToGlobal(QPoint(0,0)));
	owarn << "Pressed!" << oendl; 
}
