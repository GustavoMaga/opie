#include <qlineedit.h>

#include "logindialog.h"

LoginDialog::LoginDialog(const QString&user,const QString&pass, QWidget *parent, const char *name, bool modal, WFlags flags )
    : LoginDialogUI( parent, name, modal, flags )
{
    userLine->setText( (user.isEmpty()?"":user) );
    passLine->setText( (pass.isEmpty()?"":pass) );
    _user = user;
    _pass = pass;

    if ( user.isEmpty() ) {
        userLine->setFocus();
    } else {
        passLine->setFocus();
    }
}

void LoginDialog::accept()
{
    //_user.replace( 0, _user.length(), userLine->text() );
    //_pass.replace( 0, _pass.length(), passLine->text() );
    _user = userLine->text();
    _pass = passLine->text();

    qDebug("User im accept: |%s|",_user.latin1());
    QDialog::accept();
}
