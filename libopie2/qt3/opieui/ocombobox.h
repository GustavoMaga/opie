/*
  This file                  Copyright (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
    is part of the           Copyright (C) 2000 Carsten Pfeiffer <pfeiffer@kde.org>
       Opie Project          Copyright (C) 2000 Dawit Alemayehu <adawit@kde.org>

              =.             Originally part of the KDE projects
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

#ifndef OCOMBOBOX_H
#define OCOMBOBOX_H

/* QT */

#include <qcombobox.h>

/* OPIE */

#include <opie2/olineedit.h>
#include <opie2/ocompletion.h>
#include <opie2/ocompletionbase.h>

/* FORWARDS */

class QListBoxItem;
class QPopupMenu;
class OCompletionBox;
typedef QString OURL;

/**
 * A combined button, line-edit and a popup list widget.
 *
 * @par Detail
 *
 * This widget inherits from @ref QComboBox and implements
 * the following additional functionalities:  a completion
 * object that provides both automatic and manual text
 * completion as well as text rotation features, configurable
 * key-bindings to activate these features, and a popup-menu
 * item that can be used to allow the user to set text completion
 * modes on the fly based on their preference.
 *
 * To support these new features OComboBox also emits a few
 * more additional signals as well.  The main ones are the
 * @ref completion( const QString& ) and @ref textRotation( KeyBindingType )
 * signals.  The completion signal is intended to be connected to a slot
 * that will assist the user in filling out the remaining text while
 * the rotation signals is intended to be used to traverse through all
 * possible matches whenever text completion results in multiple matches.
 * The @ref returnPressed() and @ref returnPressed( const QString& )
 * signal is emitted when the user presses the Enter/Return key.
 *
 * This widget by default creates a completion object when you invoke
 * the @ref completionObject( bool ) member function for the first time
 * or use @ref setCompletionObject( OCompletion*, bool ) to assign your
 * own completion object.  Additionally, to make this widget more functional,
 * OComboBox will by default handle the text rotation and completion
 * events internally whenever a completion object is created through either
 * one of the methods mentioned above.  If you do not need this functionality,
 * simply use @ref OCompletionBase::setHandleSignals( bool ) or alternatively
 * set the boolean parameter in the above methods to FALSE.
 *
 * The default key-bindings for completion and rotation is determined
 * from the global settings in @ref OStdAccel.  These values, however,
 * can be overriden locally by invoking @ref OCompletionBase::setKeyBinding().
 * The values can easily be reverted back to the default setting, by simply
 * calling @ref useGlobalSettings(). An alternate method would be to default
 * individual key-bindings by usning @ref setKeyBinding() with the default
 * second argument.
 *
 * Note that if this widget is not editable ( i.e. select-only ), then only
 * one completion mode, @p CompletionAuto, will work.  All the other modes are
 * simply ignored.  The @p CompletionAuto mode in this case allows you to
 * automatically select an item from the list by trying to match the pressed
 * keycode with the first letter of the enteries in the combo box.
 *
 * @par Usage
 *
 * To enable the basic completion feature:
 *
 * <pre>
 * OComboBox *combo = new OComboBox( true, this, "mywidget" );
 * OCompletion *comp = combo->completionObject();
 * // Connect to the return pressed signal - optional
 * connect(combo,SIGNAL(returnPressed(const QString&)),comp,SLOT(addItem(const QString&));
 * </pre>
 *
 * To use your own completion object:
 *
 * <pre>
 * OComboBox *combo = new OComboBox( this,"mywidget" );
 * OURLCompletion *comp = new OURLCompletion();
 * combo->setCompletionObject( comp );
 * // Connect to the return pressed signal - optional
 * connect(combo,SIGNAL(returnPressed(const QString&)),comp,SLOT(addItem(const QString&));
 * </pre>
 *
 * Note that you have to either delete the allocated completion object
 * when you don't need it anymore, or call
 * setAutoDeleteCompletionObject( true );
 *
 * Miscellaneous function calls:
 *
 * <pre>
 * // Tell the widget not to handle completion and rotation
 * combo->setHandleSignals( false );
 * // Set your own completion key for manual completions.
 * combo->setKeyBinding( OCompletionBase::TextCompletion, Qt::End );
 * // Hide the context (popup) menu
 * combo->setContextMenuEnabled( false );
 * // Temporarly disable signal emition
 * combo->disableSignals();
 * // Default the all key-bindings to their system-wide settings.
 * combo->useGlobalKeyBindings();
 * </pre>
 *
 * @short An enhanced combo box.
 * @author Dawit Alemayehu <adawit@kde.org>
 */
class OComboBox : public QComboBox, public OCompletionBase
{
  Q_OBJECT

  //Q_PROPERTY( bool autoCompletion READ autoCompletion WRITE setAutoCompletion )
  //Q_PROPERTY( bool contextMenuEnabled READ isContextMenuEnabled WRITE setContextMenuEnabled )
  //Q_PROPERTY( bool urlDropsEnabled READ isURLDropsEnabled WRITE setURLDropsEnabled )

public:

    /**
    * Constructs a read-only or rather select-only combo box with a
    * parent object and a name.
    *
    * @param parent The parent object of this widget
    * @param name The name of this widget
    */
    OComboBox( QWidget *parent=0, const char *name=0 );

    /**
    * Constructs a "read-write" or "read-only" combo box depending on
    * the value of the first argument( @p rw ) with a parent, a
    * name.
    *
    * @param rw When @p true, widget will be editable.
    * @param parent The parent object of this widget.
    * @param name The name of this widget.
    */
    OComboBox( bool rw, QWidget *parent=0, const char *name=0 );

    /**
    * Destructor.
    */
    virtual ~OComboBox();

    /**
     * Sets @p url into the edit field of the combobox. It uses
     * @ref OURL::prettyURL() so that the url is properly decoded for
     * displaying.
     */
    //void setEditURL( const OURL& url );

    /**
     * Inserts @p url at position @p index into the combobox. The item will
     * be appended if @p index is negative. @ref OURL::prettyURL() is used
     * so that the url is properly decoded for displaying.
     */
    //void insertURL( const OURL& url, int index = -1 );

    /**
     * Inserts @p url with the pixmap &p pixmap at position @p index into
     * the combobox. The item will be appended if @p index is negative.
     * @ref OURL::prettyURL() is used so that the url is properly decoded
     * for displaying.
     */
    //void insertURL( const QPixmap& pixmap, const OURL& url, int index = -1 );

    /**
     * Replaces the item at position @p index with @p url.
     * @ref OURL::prettyURL() is used so that the url is properly decoded
     * for displaying.
     */
    //void changeURL( const OURL& url, int index );

    /**
     * Replaces the item at position @p index with @p url and pixmap @p pixmap.
     * @ref OURL::prettyURL() is used so that the url is properly decoded
     * for displaying.
     */
    //void changeURL( const QPixmap& pixmap, const OURL& url, int index );

    /**
    * Returns the current cursor position.
    *
    * This method always returns a -1 if the combo-box is @em not
    * editable (read-write).
    *
    * @return Current cursor position.
    */
    int cursorPosition() const { return ( lineEdit() ) ? lineEdit()->cursorPosition() : -1; }

    /**
    * Re-implemented from @ref QComboBox.
    *
    * If @p true, the completion mode will be set to automatic.
    * Otherwise, it is defaulted to the global setting.  This
    * method has been replaced by the more comprehensive
    * @ref setCompletionMode().
    *
    * @param autocomplete Flag to enable/disable automatic completion mode.
    */
    virtual void setAutoCompletion( bool autocomplete );

    /**
    * Re-implemented from QComboBox.
    *
    * Returns @p true if the current completion mode is set
    * to automatic.  See its more comprehensive replacement
    * @ref completionMode().
    *
    * @return @p true when completion mode is automatic.
    */
    bool autoCompletion() const {
        return completionMode() == OGlobalSettings::CompletionAuto;
    }

    /**
    * Enables or disable the popup (context) menu.
    *
    * This method only works if this widget is editable, i.e.
    * read-write and allows you to enable/disable the context
    * menu.  It does nothing if invoked for a none-editable
    * combo-box.  Note that by default the mode changer item
    * is made visiable whenever the context menu is enabled.
    * Use @ref hideModechanger() if you want to hide this
    * item.    Also by default, the context menu is created if
    * this widget is editable. Call this function with the
    * argument set to false to disable the popup menu.
    *
    * @param showMenu If @p true, show the context menu.
    * @param showMode If @p true, show the mode changer.
    */
    virtual void setContextMenuEnabled( bool showMenu );

    /**
    * Returns @p true when the context menu is enabled.
    */
    bool isContextMenuEnabled() const { return m_bEnableMenu; }

    /**
     * Enables/Disables handling of URL drops. If enabled and the user
     * drops an URL, the decoded URL will be inserted. Otherwise the default
     * behaviour of QComboBox is used, which inserts the encoded URL.
     *
     * @param enable If @p true, insert decoded URLs
     */
    //void setURLDropsEnabled( bool enable );

    /**
     * Returns @p true when decoded URL drops are enabled
     */
    //bool isURLDropsEnabled() const;

    /**
     * Convenience method which iterates over all items and checks if
     * any of them is equal to @p text.
     *
     * If @p text is an empty string, @p false
     * is returned.
     *
     * @return @p true if an item with the string @p text is in the combobox.
     */
    bool contains( const QString& text ) const;

    /**
     * By default, OComboBox recognizes Key_Return and Key_Enter
     * and emits
     * the @ref returnPressed() signals, but it also lets the event pass,
     * for example causing a dialog's default-button to be called.
     *
     * Call this method with @p trap equal to true to make OComboBox
     * stop these
     * events. The signals will still be emitted of course.
     *
     * Only affects read-writable comboboxes.
     *
     * @see setTrapReturnKey()
     */
    void setTrapReturnKey( bool trap );

    /**
     * @return @p true if keyevents of Key_Return or Key_Enter will
     * be stopped or if they will be propagated.
     *
     * @see setTrapReturnKey ()
     */
    bool trapReturnKey() const;

    /**
    * Re-implemented for internal reasons.  API not affected.
    *
    * @reimplemented
    */
    virtual bool eventFilter( QObject *, QEvent * );

    /**
     * @returns the completion-box, that is used in completion mode
     * @ref OGlobalSettings::CompletionPopup and @ref OGlobalSettings::CompletionPopupAuto.
     * This method will create a completion-box by calling
     * @ref OLineEdit::completionBox, if none is there, yet.
     *
     * @param create Set this to false if you don't want the box to be created
     *               i.e. to test if it is available.
     */
    OCompletionBox * completionBox( bool create = true );

    virtual void setLineEdit( OLineEdit * );

signals:
    /**
    * Emitted when the user presses the Enter key.
    *
    * Note that this signal is only
    * emitted if this widget is editable.
    */
    void returnPressed();

    /**
    * Emitted when the user presses
    * the Enter key.
    *
    * The argument is the current
    * text being edited.  This signal is just like
    * @ref returnPressed() except it contains the
    * current text as its argument.
    *
    * Note that this signal is only emitted if this
    * widget is editable.
    */
    void returnPressed( const QString& );

    /**
    * This signal is emitted when the completion key
    * is pressed.
    *
    * The argument is the current text
    * being edited.
    *
    * Note that this signal is @em not available if this
    * widget is non-editable or the completion mode is
    * set to @p OGlobalSettings::CompletionNone.
    */
    void completion( const QString& );

    /**
     * Emitted when the shortcut for substring completion is pressed.
     */
    void substringCompletion( const QString& );

    /**
    * Emitted when the text rotation key-bindings are pressed.
    *
    * The argument indicates which key-binding was pressed.
    * In this case this can be either one of four values:
    * @p PrevCompletionMatch, @p NextCompletionMatch, @p RotateUp or
    * @p RotateDown. See @ref OCompletionBase::setKeyBinding() for
    * details.
    *
    * Note that this signal is @em NOT emitted if the completion
    * mode is set to CompletionNone.
    */
    void textRotation( OCompletionBase::KeyBindingType );

    /**
     * Emitted when the user changed the completion mode by using the
     * popupmenu.
     */
    void completionModeChanged( OGlobalSettings::Completion );

    /**
     * Emitted before the context menu is displayed.
     *
     * The signal allows you to add your own entries into the
     * the context menu that is created on demand.
     *
     * NOTE: Do not store the pointer to the QPopupMenu
     * provided through since it is created and deleted
     * on demand.
     *
     * @param the context menu about to be displayed
     */
    void aboutToShowContextMenu( QPopupMenu * );

public slots:

    /**
    * Iterates through all possible matches of the completed text
    * or the history list.
    *
    * Depending on the value of the argument, this function either
    * iterates through the history list of this widget or the all
    * possible matches in whenever multiple matches result from a
    * text completion request.  Note that the all-possible-match
    * iteration will not work if there are no previous matches, i.e.
    * no text has been completed and the *nix shell history list
    * rotation is only available if the insertion policy for this
    * widget is set either @p QComobBox::AtTop or @p QComboBox::AtBottom.
    * For other insertion modes whatever has been typed by the user
    * when the rotation event was initiated will be lost.
    *
    * @param type The key-binding invoked.
    */
    void rotateText( OCompletionBase::KeyBindingType /* type */ );

    /**
     * Sets the completed text in the line-edit appropriately.
     *
     * This function is an implementation for
     * @ref OCompletionBase::setCompletedText.
     */
    virtual void setCompletedText( const QString& );

    /**
     * Sets @p items into the completion-box if @ref completionMode() is
     * CompletionPopup. The popup will be shown immediately.
     */
    void setCompletedItems( const QStringList& items );

  public:
    /**
     * Selects the first item that matches @p item. If there is no such item,
     * it is inserted at position @p index if @p insert is true. Otherwise,
     * no item is selected.
     */
    void setCurrentItem( const QString& item, bool insert = false, int index = -1 );
    void setCurrentItem(int index);

protected slots:

    /**
    * @deprecated.
    */
    virtual void itemSelected( QListBoxItem* ) {};

    /**
    * Completes text according to the completion mode.
    *
    * Note: this method is @p not invoked if the completion mode is
    * set to CompletionNone.  Also if the mode is set to @p CompletionShell
    * and multiple matches are found, this method will complete the
    * text to the first match with a beep to inidicate that there are
    * more matches.  Then any successive completion key event iterates
    * through the remaining matches.  This way the rotation functionality
    * is left to iterate through the list as usual.
    */
    virtual void makeCompletion( const QString& );

protected:
    /*
    * This function simply sets the lineedit text and
    * highlights the text appropriately if the boolean
    * value is set to true.
    *
    * @param
    * @param
    */
    virtual void setCompletedText( const QString& /* */, bool /*marked*/ );

    /**
     * Reimplemented for internal reasons, the API is not affected.
     */
    virtual void create( WId = 0, bool initializeWindow = true,
                         bool destroyOldWindow = true );

private:
    // Constants that represent the ID's of the popup menu.
    // TODO: See if we can replace this mess with OActionMenu
    // in the future though this is working lovely.
    enum MenuID {
        Default=0,
        Cut,
        Copy,
        Paste,
        Clear,
        Unselect,
        SelectAll,
        NoCompletion,
        AutoCompletion,
        ShellCompletion,
        PopupCompletion,
        SemiAutoCompletion
    };

    /**
     * Initializes the variables upon construction.
     */
    void init();
    /**
     * Temporary functions to delete words back and foward until
     * alternatives are available in QT3 (Seth Chaiklin, 21 may 2001)
     */
    void deleteWordBack();
    void deleteWordForward();

    bool m_bEnableMenu;

    // indicating if we should stop return-key events from propagating
    bool m_trapReturnKey;

//protected:
//    virtual void virtual_hook( int id, void* data );
private:
    class OComboBoxPrivate;
    OComboBoxPrivate *d;
};


class OPixmapProvider;

/**
 * A combobox which implements a history like a unix shell. You can navigate
 * through all the items by using the Up or Down arrows (configurable of
 * course). Additionally, weighted completion is available. So you should
 * load and save the completion list to preserve the weighting between
 * sessions.
 *
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 * @short A combobox for offering a history and completion
 */
class OHistoryCombo : public OComboBox
{
    Q_OBJECT
    Q_PROPERTY( QStringList historyItems READ historyItems WRITE setHistoryItems )

public:
    /**
     * Constructs a "read-write" combobox. A read-only history combobox
     * doesn't make much sense, so it is only available as read-write.
     * Completion will be used automatically for the items in the combo.
     *
     * The insertion-policy is set to NoInsertion, you have to add the items
     * yourself via the slot @ref addToHistory. If you want every item added,
     * use
     *
     * <pre>
     * connect( combo, SIGNAL( activated( const QString& )),
     *          combo, SLOT( addToHistory( const QString& )));
     * </pre>
     *
     * Use @ref QComboBox::setMaxCount() to limit the history.
     *
     * @p parent the parent object of this widget.
     * @p name the name of this widget.
     */
    OHistoryCombo( QWidget *parent = 0L, const char *name = 0L );

    // ### merge these two constructors
    /**
     * Same as the previous constructor, but additionally has the option
     * to specify whether you want to let OHistoryCombo handle completion
     * or not. If set to @p true, OHistoryCombo will sync the completion to the
     * contents of the combobox.
     */
    OHistoryCombo( bool useCompletion,
		   QWidget *parent = 0L, const char *name = 0L );

    /**
     * Destructs the combo, the completion-object and the pixmap-provider
     */
    ~OHistoryCombo();

    /**
     * Inserts @p items into the combobox. @p items might get
     * truncated if it is longer than @ref maxCount()
     *
     * @see #historyItems
     */
    inline void setHistoryItems( QStringList items ) {
        setHistoryItems(items, false);
    }

    /**
     * Inserts @p items into the combobox. @p items might get
     * truncated if it is longer than @ref maxCount()
     *
     * Set @p setCompletionList to true, if you don't have a list of
     * completions. This tells OHistoryCombo to use all the items for the
     * completion object as well.
     * You won't have the benefit of weighted completion though, so normally
     * you should do something like
     * <pre>
     * OConfig *config = kapp->config();
     * QStringList list;
     *
     * // load the history and completion list after creating the history combo
     * list = config->readListEntry( "Completion list" );
     * combo->completionObject()->setItems( list );
     * list = config->readListEntry( "History list" );
     * combo->setHistoryItems( list );
     *
     * [...]
     *
     * // save the history and completion list when the history combo is
     * // destroyed
     * list = combo->completionObject()->items()
     * config->writeEntry( "Completion list", list );
     * list = combo->historyItems();
     * config->writeEntry( "History list", list );
     * </pre>
     *
     * Be sure to use different names for saving with OConfig if you have more
     * than one OHistoryCombo.
     *
     * Note: When @p setCompletionList is true, the items are inserted into the
     * OCompletion object with mode OCompletion::Insertion and the mode is set
     * to OCompletion::Weighted afterwards.
     *
     * @see #historyItems
     * @see OComboBox::completionObject
     * @see OCompletion::setItems
     * @see OCompletion::items
     */
    void setHistoryItems( QStringList items, bool setCompletionList );

    /**
     * Returns the list of history items. Empty, when this is not a read-write
     * combobox.
     *
     * @see #setHistoryItems
     */
    QStringList historyItems() const;

    /**
     * Removes all items named @p item.
     *
     * @return @p true if at least one item was removed.
     *
     * @see #addToHistory
     */
    bool removeFromHistory( const QString& item );

    /**
     * Sets a pixmap provider, so that items in the combobox can have a pixmap.
     * @ref OPixmapProvider is just an abstract class with the one pure virtual
     * method @ref OPixmapProvider::pixmapFor(). This method is called whenever
     * an item is added to the OHistoryComboBox. Implement it to return your
     * own custom pixmaps, or use the @ref OURLPixmapProvider from libkio,
     * which uses @ref OMimeType::pixmapForURL to resolve icons.
     *
     * Set @p prov to 0L if you want to disable pixmaps. Default no pixmaps.
     *
     * @see #pixmapProvider
     */
    void setPixmapProvider( OPixmapProvider *prov );

    /**
     * @returns the current pixmap provider.
     * @see #setPixmapProvider
     * @see OPixmapProvider
     */
    OPixmapProvider * pixmapProvider() const { return myPixProvider; }

    /**
     * Resets the current position of the up/down history. Call this
     * when you manually call @ref setCurrentItem() or @ref clearEdit().
     */
    void reset() { slotReset(); }

public slots:
    /**
     * Adds an item to the end of the history list and to the completion list.
     * If @ref maxCount() is reached, the first item of the list will be
     * removed.
     *
     * If the last inserted item is the same as @p item, it will not be
     * inserted again.
     *
     * If @ref duplicatesEnabled() is false, any equal existing item will be
     * removed before @p item is added.
     *
     * Note: By using this method and not the Q and OComboBox insertItem()
     * methods, you make sure that the combobox stays in sync with the
     * completion. It would be annoying if completion would give an item
     * not in the combobox, and vice versa.
     *
     * @see #removeFromHistory
     * @see QComboBox::setDuplicatesEnabled
     */
    void addToHistory( const QString& item );

    /**
     * Clears the history and the completion list.
     */
    void clearHistory();

signals:
    /**
     * Emitted when the history was cleared by the entry in the popup menu.
     */
    void cleared();

protected:
    /**
     * Handling key-events, the shortcuts to rotate the items.
     */
    virtual void keyPressEvent( QKeyEvent * );


    /**
     * Inserts @p items into the combo, honouring @ref pixmapProvider()
     * Does not update the completionObject.
     *
     * Note: @ref duplicatesEnabled() is not honored here.
     *
     * Called from @ref setHistoryItems() and @ref setPixmapProvider()
     */
    void insertItems( const QStringList& items );

    /**
     * @returns if we can modify the completion object or not.
     */
    bool useCompletion() const { return compObj() != 0L; }

private slots:
    /**
     * Resets the iterate index to -1
     */
    void slotReset();

    /**
     * Called from the popupmenu,
     * calls clearHistory() and emits cleared()
     */
    void slotClear();

    /**
     * Appends our own context menu entry.
     */
    void addContextMenuItems( QPopupMenu* );

private:
    void init( bool useCompletion );

    /**
     * The current position (index) in the combobox, used for Up and Down
     */
    int myIterateIndex;

    /**
     * The text typed before Up or Down was pressed.
     */
    QString myText;

    /**
     * Indicates that the user at least once rotated Up through the entire list
     * Needed to allow going back after rotation.
     */
    bool myRotated;
    OPixmapProvider *myPixProvider;

private:
    class OHistoryComboPrivate;
    OHistoryComboPrivate *d;
};


#endif

