/****************************************************************************
** $Id: colordialog.h,v 1.5 2003-04-16 10:59:24 zecke Exp $
**
** Definition of OColorDialog class
**
** Created : 990222
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the dialogs module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef OColorDialog_H
#define OColorDialog_H

#ifndef QT_H
#include <qdialog.h>
#endif // QT_H

class OColorDialogPrivate;

/*
 * @class OColorDialog
 * @brief The OColorDialog class is a copy of QColorDialog for use in Opie.
 *
 * OColorDialog is a copy of TrollTech's QColorDialog for use in Opie.  The default
 * build of QT/Embedded used by Opie does not include QColorDialog, so it is provided
 * here.  It is renamed to prevent conflicts in the event the QColorDialog is included
 * at a later date in QP/E.
 *
 * See http://doc.trolltech.com/2.3/qcolordialog.html for complete documentation of
 * QColorDialog.
 */
class Q_EXPORT OColorDialog : public QDialog
{
    Q_OBJECT

public:
    static QColor getColor( const QColor&, QWidget *parent=0, const char* name=0 );
    static QRgb getRgba( const QRgb&, bool* ok = 0,
			 QWidget *parent=0, const char* name=0 );

private:
    ~OColorDialog();

    OColorDialog( QWidget* parent=0, const char* name=0, bool modal=FALSE );
    void setColor( const QColor& );
    QColor color() const;

private:
    void setSelectedAlpha( int );
    int selectedAlpha() const;
private:
    OColorDialogPrivate *d;
    friend class OColorDialogPrivate;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    OColorDialog( const OColorDialog & );
    OColorDialog& operator=( const OColorDialog & );
#endif
};

#endif
