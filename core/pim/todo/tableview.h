/*
� � � � � � � �=.            This file is part of the OPIE Project
� � � � � � �.=l.            Copyright (c)  2002 <>
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This program is free software; you can
.> <`_, � > �. � <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU General Public
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

#ifndef OPIE_TABLE_VIEW_H
#define OPIE_TABLE_VIEW_H

#include <qtable.h>
#include <qmap.h>
#include <qpixmap.h>

#include "todoview.h"

class QTimer;

namespace Todo {
    class CheckItem;
    class DueTextItem;
    class TableView : public QTable, public TodoView {
        Q_OBJECT
    public:
        TableView( MainWindow*, QWidget* parent );
        ~TableView();


        QString type()const;
        int current();
        QString currentRepresentation();
        int next();
        int prev();

        void clear();
        void showOverDue( bool );
        void updateView();
        void setTodo( int uid, const OPimTodo& );
        void addEvent( const OPimTodo& event );
        void replaceEvent( const OPimTodo& );
        void removeEvent( int uid );
        void setShowCompleted( bool );
        void setShowDeadline( bool );

        void setShowCategory(const QString& =QString::null );
        void newDay();
        QWidget* widget();
        void sortColumn(int, bool, bool );

        /*
         * we do our drawing ourselves
         * because we don't want to have
         * 40.000 QTableItems for 10.000
         * OPimTodos where we only show 10 at a time!
         */
        void paintCell(QPainter* p, int row, int col,  const QRect&, bool );
    private:
        /* reimplented for internal reasons */
        void viewportPaintEvent( QPaintEvent* );
        QTimer *m_menuTimer;
        bool m_enablePaint:1;
        QString m_oleCat;
        bool m_first : 1;

        QPixmap m_pic_completed;
        QPixmap m_pic_priority[ 5 ];

    protected:
        void keyPressEvent( QKeyEvent* );
        void contentsMouseReleaseEvent( QMouseEvent* );
        void contentsMouseMoveEvent( QMouseEvent* );
        void timerEvent( QTimerEvent* e );
        QWidget* createEditor(int row, int col, bool initFromCell )const;
        void setCellContentFromEditor( int row, int col );
        /**
         * for inline editing on single click and to work around some
         * bug in qt2.3.5
         */
        struct EditorWidget {
            EditorWidget();
            void setCellWidget(QWidget*, int row, int col );
            void releaseCellWidget();
            QWidget* cellWidget()const;
            int cellRow()const;
            int cellCol()const;
        private:
            QWidget* m_wid;
            int m_row, m_col;
        };
        EditorWidget m_editorWidget;

private slots:
        void slotShowMenu();
        void slotClicked(int, int, int,
                         const QPoint& );
        void slotPressed(int, int, int,
                         const QPoint& );
        void slotValueChanged(int, int);
        void slotCurrentChanged(int, int );
        void slotPriority();
    private:
        void initConfig();
        int m_completeStrokeWidth;
        bool m_row : 1;
        QPoint m_prevP;
    };
};

#endif
