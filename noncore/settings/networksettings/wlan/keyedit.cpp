#include "keyedit.h"
#include <qlineedit.h>

KeyEdit::KeyEdit(QWidget* parent, const char* name) :
	QLineEdit(parent, name)
{
    setEchoMode(Password);
}

KeyEdit::~KeyEdit()
{
}

void KeyEdit::focusInEvent(QFocusEvent *)
{
	setEchoMode(Normal);
}

void KeyEdit::focusOutEvent(QFocusEvent *)
{
	setEchoMode(Password);
}
