/**************************************************************************************94x78**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*********************************************************************************************/
#ifndef KEYBOARDIMPL_H
#define KEYBOARDIMPL_H

#include <qpe/inputmethodinterface.h>

namespace JumpX
{
    class Keyboard;
};
class QPixmap;

namespace
{

class KeyboardImpl : public InputMethodInterface
{
public:
    KeyboardImpl();
    virtual ~KeyboardImpl();

#ifndef QT_NO_COMPONENT
    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT
#endif

    virtual QWidget *inputMethod( QWidget *parent, Qt::WFlags f );
    virtual void resetState();
    virtual QPixmap *icon();
    virtual QString name();
    virtual void onKeyPress( QObject *receiver, const char *slot );

private:
    JumpX::Keyboard *input;
    QPixmap *icn;
};

} // anonymous namespace

#endif
