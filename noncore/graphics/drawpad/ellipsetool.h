/***************************************************************************
 *                                                                         *
 *   DrawPad - a drawing program for Opie Environment                      *
 *                                                                         *
 *   (C) 2002 by S. Prud'homme <prudhomme@laposte.net>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ELLIPSETOOL_H
#define ELLIPSETOOL_H

#include "shapetool.h"

class EllipseTool : public ShapeTool
{
public:
    EllipseTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);
    ~EllipseTool();

protected:
    void drawFinalShape(QPainter& p);
    void drawTemporaryShape(QPainter& p);
};

#endif // ELLIPSETOOL_H
