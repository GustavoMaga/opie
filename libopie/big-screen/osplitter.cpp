/*
� � � � � � � �=.            This file is part of the OPIE Project
� � � � � � �.=l.            Copyright (c)  2003 hOlgAr <zecke@handhelds.org>
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This library is free software; you can
.> <`_, � > �. � <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU Library General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
� � .%`+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This library is distributed in the hope that
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

#include <qvaluelist.h>
#include <qvbox.h>

#include <opie/otabwidget.h>

#include "osplitter.h"


/**
 *
 * This is the constructor of OSplitter
 * You might want to call setSizeChange to tell
 * OSplitter to change its layout when a specefic
 * mark was crossed. OSplitter sets a default value.
 *
 * You cann add widget with addWidget to the OSplitter.
 *
 * @param orient The orientation wether to layout horizontal or vertical
 * @param parent The parent of this widget
 * @param name   The name passed on to QObject
 * @param fl     Additional widgets flags passed to QWidget
 *
 * @short single c'tor of the OSplitter
 */
OSplitter::OSplitter( Orientation orient, QWidget* parent, const char* name, WFlags fl )
    : QFrame( parent, name, fl )
{
    m_orient = orient;
    m_hbox = 0;
    m_size_policy = 330;
    setFontPropagation( AllChildren );
    setPalettePropagation( AllChildren );

    /* start by default with the tab widget */
    m_tabWidget = 0;
    changeTab();

}


/**
 * Destructor destructs this object and cleans up. All child
 * widgets will be deleted
 * @see addWidget
 */
OSplitter::~OSplitter() {
    delete m_hbox;
    delete m_tabWidget;
}


/**
 * This function sets the size change policy of the splitter.
 * If this size marked is crossed the splitter will relayout.
 * Note: that depending on the set Orientation it'll either look
 * at the width or height.
 * Note: If you want to from side to side view to tabbed view you need
 * to make sure that the size you supply is not smaller than the minimum
 * size of your added widgets. Note that if you use widgets like QComboBoxes
 * you need to teach them to accept smaller sizes as well @see QWidget::setSizePolicy
 *
 * @param width_height The mark that will be watched. Interpreted depending on the Orientation of the Splitter.
 * @return void
 */
void OSplitter::setSizeChange( int width_height ) {
    m_size_policy = width_height;
    QSize sz(width(), height() );
    QResizeEvent ev(sz, sz );
    resizeEvent(&ev);
}

/**
 * Adds a widget to the Splitter. The widgets gets inserted
 * at the end of either the Box or TabWidget.
 * Ownership gets transfered and the widgets gets reparented.
 * Note: icon and label is only available on small screensizes
 *  if size is smaller than the mark
 * Warning: No null checking of the widget is done. Only on debug
 * a message will be outputtet
 *
 * @param wid The widget which will be added
 * @param icon The icon of the possible Tab
 * @param label The label of the possible Tab
 */
void OSplitter::addWidget( QWidget* wid, const QString& icon, const QString& label ) {
#ifdef DEBUG
    if (!wid ) {
        qWarning("Widget is not valid!");
        return;
    }
#endif
    Opie::OSplitterContainer cont;
    cont.widget = wid;
    cont.icon =icon;
    cont.name = label;

    m_container.append( cont );

    if (m_hbox )
        addToBox( cont );
    else
        addToTab( cont );
}


/**
 * Removes the widget from the tab widgets if necessary.
 * OSplitter drops ownership of this widget and the widget
 * will be reparented i tto 0.
 * The widget will not be deleted.
 *
 * @param w The widget to be removed
 */
void OSplitter::removeWidget( QWidget* w) {
    ContainerList::Iterator it;
    for ( it = m_container.begin(); it != m_container.end(); ++it )
        if ( (*it).widget == w )
            break;

    if (it == m_container.end() )
        return;


    /* only tab needs to be removed.. box recognizes it */
    if ( !m_hbox )
        removeFromTab( w );


    /* Find reparent it and remove it from our list */

    w->reparent( 0, 0, QPoint(0, 0));
    it = m_container.remove( it );

}


/**
 * This method will give focus to the widget. If in a tabwidget
 * the tabbar will be changed
 *
 * @param w The widget which will be set the current one
 */
void OSplitter::setCurrentWidget( QWidget*  w) {
    if (m_tabWidget )
        m_tabWidget->setCurrentTab( w );
//    else
    //      m_hbox->setFocus( w );

}

/**
 * This is an overloaded member function and only differs in the
 * argument it takes.
 * Searches list of widgets for label. It'll pick the first label it finds
 *
 * @param label Label to look for. First match will be taken
 */
void OSplitter::setCurrentWidget( const QString& label ) {
    ContainerList::Iterator it;
    for (it = m_container.begin(); it != m_container.end(); ++it ) {
        if ( (*it).name == label ) {
            setCurrentWidget( (*it).widget );
            break;
        }
    }
}

/**
 * This will only work when the TabWidget is active
 * If everything is visible this signal is kindly ignored
 * @see OTabWidget::setCurrentTab(int)
 *
 * @param tab The tab to make current
 */
void OSplitter::setCurrentWidget( int tab ) {
    if (m_tabWidget )
        m_tabWidget->setCurrentTab( tab );
}

/**
 * return the currently activated widget if in tab widget mode
 * or null because all widgets are visible
 */
QWidget* OSplitter::currentWidget() {
    if ( m_hbox )
        return 0l;
    else
        return m_tabWidget->currentWidget();
}

#if 0
/**
 * @reimplented for internal reasons
 * returns the sizeHint of one of its sub widgets
 */
QSize OSplitter::sizeHint()const {
    return QSize(10, 10);

    if (m_hbox )
        return m_hbox->sizeHint();
    else
        return m_tabWidget->sizeHint();
}

QSize OSplitter::minimumSizeHint()const {
    return QSize(10, 10 );
}
#endif

/**
 * @reimplemented for internal reasons
 */
void OSplitter::resizeEvent( QResizeEvent* res ) {
    QFrame::resizeEvent( res );
    /*
     *
     */
//    qWarning("Old size was width = %d height = %d", res->oldSize().width(), res->oldSize().height() );
    qWarning("New size is  width = %d height = %d", res->size().width(), res->size().height() );
    if ( res->size().width() > m_size_policy &&
         m_orient == Horizontal ) {
        changeHBox();
    }else if ( (res->size().width() <= m_size_policy &&
               m_orient == Horizontal ) ||
               (res->size().height() <= m_size_policy &&
                m_orient == Vertical ) ) {
        changeTab();
    }else if ( res->size().height() > m_size_policy &&
               m_size_policy == Vertical ) {
        changeVBox();
    }
}


void OSplitter::addToTab( const Opie::OSplitterContainer& con ) {
    QWidget *wid = con.widget;
// not needed widgetstack will reparent as well    wid.reparent(m_tabWidget, wid->getWFlags(), QPoint(0, 0) );
    m_tabWidget->addTab( wid, con.icon, con.name );
}

void OSplitter::addToBox( const Opie::OSplitterContainer& con ) {
    QWidget* wid = con.widget;
    wid->reparent(m_hbox, 0,  QPoint(0, 0) );
}

void OSplitter::removeFromTab( QWidget* wid ) {
    m_tabWidget->removePage( wid );
}

void OSplitter::changeTab() {
    if (m_tabWidget ) {
        m_tabWidget->setGeometry( frameRect() );
        return;
    }

    qWarning(" New Tab Widget ");
    /*
     * and add all widgets this will reparent them
     * delete m_hbox set it to 0
     *
     */
    m_tabWidget = new OTabWidget( this );
    connect(m_tabWidget, SIGNAL(currentChanged(QWidget*) ),
            this, SIGNAL(currentChanged(QWidget*) ) );

    for ( ContainerList::Iterator it = m_container.begin(); it != m_container.end(); ++it ) {
        qWarning("Widget is %s", (*it).name.latin1() );
        addToTab( (*it) );
    }

    delete m_hbox;
    m_hbox = 0;
    m_tabWidget->setGeometry( frameRect() );
    m_tabWidget->show();

}

void OSplitter::changeHBox() {
    if (m_hbox ) {
        m_hbox->setGeometry( frameRect() );
        return;
    }

    qWarning("new HBox");
    m_hbox = new QHBox( this );
    commonChangeBox();
    delete m_tabWidget;
    m_tabWidget = 0;
    m_hbox->setGeometry( frameRect() );
    m_hbox->show();
}

void OSplitter::changeVBox() {
    if (m_hbox ) {
        m_hbox->setGeometry( frameRect() );
        return;
    }

    qWarning("New VBOX");
    m_hbox = new QVBox( this );

    commonChangeBox();
    delete m_tabWidget;
    m_tabWidget = 0;
    m_hbox->setGeometry( frameRect() );
    m_hbox->show();
}


void OSplitter::commonChangeBox() {
    for (ContainerList::Iterator it = m_container.begin(); it != m_container.end(); ++it ) {
        qWarning("Adding to box %s", (*it).name.latin1() );
        addToBox( (*it) );
    }
    delete m_tabWidget;
    m_tabWidget = 0;
}
