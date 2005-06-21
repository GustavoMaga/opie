/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "menubutton.h"
#include <QMenu>

/*!
  \class MenuButton menubutton.h
  \brief The MenuButton class is a pushbutton with a menu.

  When the user presses the menubutton's pushbutton, the menu pops up.
  A menu is composed of menu items each of which has a string label,
  and optionally an icon.

  The index of the item that the user's input device (e.g. stylus) is
  pointing at is the currentItem(), whose text is available using
  currentText().

  Menu items are inserted with the \link MenuButton::MenuButton()
  constructor\endlink, insertItem() or insertItems(). Separators are
  inserted with insertSeparator(). All the items in the menu can be
  removed by calling clear().

  Items can be selected programmatically using select(). When a menu
  item is selected (programmatically or by the user), the selected()
  signal is emitted.

  \ingroup qtopiaemb
*/

/*!
  \overload void MenuButton::selected(int index)

  This signal is emitted when the item at position \a index is selected.
*/

/*!
  \fn void MenuButton::selected(const QString& text)

  This signal is emitted when the item with the label \a text is selected.
*/


/*!
  Constructs a MenuButton. A menu item is created (see insertItem()
  and insertItems()) for each string in the \a items string list. The
  standard \a parent an \a name arguments are passed to the base
  class.
*/
MenuButton::MenuButton( const QStringList& items, QWidget* parent ) :
    QPushButton(parent)
{
    useLabel = true;
    init();
    insertItems(items);
}

/*!
  Constructs an empty MenuButton.
  The standard \a parent an \a name arguments are passed to the base class.

  \sa insertItem() insertItems()
*/
MenuButton::MenuButton( QWidget* parent ) :
    QPushButton(parent)
{
    init();
}

void MenuButton::init()
{
    setAutoDefault(FALSE);
    pop = new QMenu(this);
    connect(pop, SIGNAL(activated(int)), this, SLOT(select(int)));
    setMenu(pop);
}

/*!
  Removes all the menu items from the button and menu.
*/
void MenuButton::clear()
{
   delete pop;
   init();
}

/*!
  A menu item is created (see insertItem()) for each string in the \a
  items string list. If any string is "--" a separator (see
  insertSeparator()) is inserted in its place.
*/
void MenuButton::insertItems( const QStringList& items )
{
    QStringList::ConstIterator it=items.begin();
    for (; it!=items.end(); ++it) {
  if ( (*it) == "--" )
      insertSeparator();
  else
      insertItem(*it);
    }
}

/*!
  Inserts a menu item with the icon \a icon and label \a text into
  the menu.

  \sa insertItems()
*/
void MenuButton::insertItem( const QIcon& icon, const QString& text )
{
	pop->addAction(icon, text);
}

/*!
  \overload
  Inserts a menu item with the label \a text into the menu.

  \sa insertItems()
*/
void MenuButton::insertItem( const QString& text )
{
    pop->addAction(text);
}

/*!
  Inserts a separator into the menu.

  \sa insertItems()
*/
void MenuButton::insertSeparator()
{
    pop->addSeparator();
}

/*!
  Selects the items with label text \a s.
*/
void MenuButton::select(const QString& s)
{
    for (int i=0; i<pop->actions().count(); i++) {
        if ( pop->actions()[i]->text() == s ) {
            select(i);
            break;
        }
    }
}

/*!
  \overload
  Selects the item at index position \a s.
*/
void MenuButton::select(int s)
{
    cur = s;
    updateLabel();
    setIcon(pop->actions()[cur]->icon());
    emit selected(cur);
    emit selected(currentText());
}

/*!
  Returns the index position of the current item.
*/
int MenuButton::currentItem() const
{
    return cur;
}

/*!
  Returns the label text of the current item.
*/
QString MenuButton::currentText() const
{
    return pop->activeAction()->text();
}

/*!
  Sets the menubutton's label. If \a label is empty, the
  current item text is displayed, otherwise \a label should contain
  "%1", which will be replaced by the current item text.
*/
void MenuButton::setLabel(const QString& label)
{
    lab = label;
    updateLabel();
}

void MenuButton::updateLabel()
{
    if(useLabel)
      {
          QString t = pop->actions()[cur]->text();
          if ( !lab.isEmpty() )
              t = lab.arg(t);
          setText(t);
      }
}


/*!
  remove item at id
 */
void MenuButton::remove(int id)
{
     pop->removeAction(pop->actions()[id]);
}

/*!
  return count of items in menu
 */
int MenuButton::count()
{
     return pop->actions().count();
}

/*!
  returns text of item id
 */
QString MenuButton::text(int id)
{
    return pop->actions()[id]->text();
}

/*!
  sets true or false the use of label
 */
void MenuButton::setUseLabel(bool b)
{
    useLabel = b;
}
