/*
  This file                  Copyright (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
    is part of the           Copyright (C) 2000 Carsten Pfeiffer <pfeiffer@kde.org>
       Opie Project          Copyright (C) 2000 Stefan Schimanski <1Stein@gmx.de>

              =.             Originally part of the KDE Project
            .=l.
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This program is free software; you can
.> <`_, � > �. � <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU Library General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
� � .%`+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This program is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful,  but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|`    MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.= � � � = � � � ;      Library General Public License for more
++= � -. � � .` � � .:       details.
�: � � = �...= . :.=-
�-. � .:....=;==+<;          You should have received a copy of the GNU
� -_. . . � )=. �=           Library General Public License along with
� � -- � � � �:-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef OCOMPLETIONBOX_H
#define OCOMPLETIONBOX_H

class QEvent;
#include <qstringlist.h>
#include <qlistbox.h>

// ML: Until we don't have an own OListBox, we use the QListBox
#define OListBox QListBox

/**
 * A little utility class for "completion-widgets", like @ref OLineEdit or
 * @ref OComboBox. OCompletionBox is a listbox, displayed as a rectangle without
 * any window decoration, usually directly under the lineedit or combobox.
 * It is filled with all possible matches for a completion, so the user
 * can select the one he wants.
 *
 * It is used when OGlobalSettings::Completion == CompletionPopup or CompletionPopupAuto.
 *
 * @short A helper widget for "completion-widgets" (OLineEdit, OComboBox))
 * @short Adapted for the Opie project by Michael Lauer <mickey@tm.informatik.uni-frankfurt.de>
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 *
 */
class OCompletionBox : public OListBox
{
    Q_OBJECT
    Q_PROPERTY( bool isTabHandling READ isTabHandling WRITE setTabHandling )
    Q_PROPERTY(QString cancelledText READ cancelledText WRITE setCancelledText)

public:
    /**
     * Constructs a OCompletionBox.
     *
     * Notice: the parent needs to be always 0L,
     * so you can't specify it in the constructor. Because of that, Qt's
     * auto-deletion does not take place, so you have to explicitly delete
     * this widget when you don't need it anymore.
     *
     * The parent widget is used to give the focus back when pressing the
     * up-button on the very first item.
     */
    OCompletionBox( QWidget *parent, const char *name = 0 );

    /**
     * Destroys the box
     */
    ~OCompletionBox();

    virtual QSize sizeHint() const;

public slots:
    /**
     * Returns a list of all items currently in the box.
     */
    QStringList items() const;

    /**
     * Inserts @p items into the box. Does not clear the items before.
     * @p index determines at which position @p items will be inserted.
     * (defaults to appending them at the end)
     */
    void insertItems( const QStringList& items, int index = -1 );

    /**
     * Clears the box and inserts @p items.
     */
    void setItems( const QStringList& items );

    /**
     * Adjusts the size of the box to fit the width of the parent given in the
     * constructor and pops it up at the most appropriate place, relative to
     * the parent.
     *
     * Depending on the screensize and the position of the parent, this may
     * be a different place, however the default is to pop it up and the
     * lower left corner of the parent.
     *
     * Make sure to hide() the box when appropriate.
     */
    virtual void popup();

    /**
     * Makes this widget (when visible) capture Tab-key events to traverse the
     * items in the dropdown list.
     *
     * Default off, as it conflicts with the usual behavior of Tab to traverse
     * widgets. It is useful for cases like Konqueror's Location Bar, though.
     *
     * @see #isTabHandling
     */
    void setTabHandling( bool enable );

    /**
     * @returns true if this widget is handling Tab-key events to traverse the
     * items in the dropdown list, otherwise false.
     *
     * Default is false.
     *
     * @see #setTabHandling
     */
    bool isTabHandling() const;

    /**
     * Sets the text to be emitted if the user chooses not to
     * pick from the available matches.
     *
     * If the cancelled text is not set through this function, the
     * @ref userCancelled signal will not be emitted.
     *
     * @see userCancelled( const QString& )
     * @param txt  the text to be emitted if the user cancels this box
     */
    void setCancelledText( const QString& );

    /**
     * @returns the text set via @ref setCancelledText() or QString::null.
     */
    QString cancelledText() const;

    /**
     * Moves the selection one line down or select the first item if nothing is selected yet.
     */
    void down();

    /**
     * Moves the selection one line up or select the first item if nothing is selected yet.
     */
    void up();

    /**
     * Moves the selection one page down.
     */
    void pageDown();

    /**
     * Moves the selection one page up.
     */
    void pageUp();

    /**
     * Moves the selection up to the first item.
     */
    void home();

    /**
     * Moves the selection down to the last item.
     */
    void end();

    /**
     * Re-implemented for internal reasons.  API is unaffected.
     */
    virtual void show();

    /**
     * Re-implemented for internal reasons.  API is unaffected.
     */
    virtual void hide();

signals:
    /**
     * Emitted when an item was selected, contains the text of
     * the selected item.
     */
    void activated( const QString& );

    /**
     * Emitted whenever the user chooses to ignore the available
     * selections and close the this box.
     */
    void userCancelled( const QString& );

protected:
    /**
     * Reimplemented from OListBox to get events from the viewport (to hide
     * this widget on mouse-click, Escape-presses, etc.
     */
    virtual bool eventFilter( QObject *, QEvent * );

protected slots:
    /**
     * Called when an item was activated. Emits
     * @ref activated() with the item.
     */
    virtual void slotActivated( QListBoxItem * );

private slots:
    void slotSetCurrentItem( QListBoxItem *i ) { setCurrentItem( i ); } // grrr
    void slotCurrentChanged();
    void cancelled();
    void slotItemClicked( QListBoxItem * );

private:
    class OCompletionBoxPrivate;
    OCompletionBoxPrivate* d;
};


#endif // OCOMPLETIONBOX_H
