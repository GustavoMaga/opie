#include "newmaildir.h"
#include <qdialog.h>
#include <qvariant.h>
#include <qlineedit.h>
#include <qcheckbox.h>

Newmdirdlg::Newmdirdlg( QWidget* parent, const char* name)
    : Newmdirdlgui(parent,name,true),ndir(""),possible_subs(false)
{
}

Newmdirdlg::~Newmdirdlg()
{
}

void Newmdirdlg::accept()
{
    ndir = dirnameEdit->text();
    possible_subs = subdirsPossibleBox->isChecked();
    if (ndir.isEmpty()) {
        return;
    }
    Newmdirdlgui::accept();
}

const QString&Newmdirdlg::Newdir()const
{
    return ndir;
}

const bool Newmdirdlg::subpossible()const
{
    return possible_subs;
}
