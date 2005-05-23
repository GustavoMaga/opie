/*
 * GPLv2 only
 * zecke@handhelds.org
 */

#ifndef PHUNK_VIEW_MODE_BUTTON_H
#define PHUNK_VIEW_MODE_BUTTON_H

#include <opie2/oresource.h>

#include <qtoolbutton.h>

class ViewModeButton : public QToolButton {
    Q_OBJECT
public:
    ViewModeButton( QToolBar*,int def=1 );
    ~ViewModeButton();

signals:
    void changeMode( int );
private slots:
    void slotChange( int i );
};

#endif
