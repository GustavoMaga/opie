#ifndef OPIE_FUNCTION_KEYBOARD_H
#define OPIE_FUNCTION_KEYBOARD_H

#include <opie2/oresource.h>

#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <qframe.h>
#include <qpainter.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include <qmap.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include "profiledialogwidget.h"


struct FKey {

    FKey(): label(), pix(0), pixFile(), qcode(0), unicode(0) {}
    FKey(const QString &l, const QString &f, ushort q, ushort u)
        : label(l)
        , pix(0)
        , pixFile(f)
        , qcode(q)
        , unicode(u)
    {
        if (!f.isEmpty())
            pix = new QPixmap ( Opie::Core::OResource::loadPixmap("console/keys/" + f ) );
    }
//    Let Qt clean up the QPixmap. If you enable this dtor, it will cause crashing
//    ~FKey() { if (pix) delete pix; }

    QString label;
    QPixmap *pix;
    QString pixFile;
    ushort qcode;
    ushort unicode;
};

class FunctionKeyboard : public QFrame {
    Q_OBJECT

public:
    FunctionKeyboard(QWidget *parent = 0);
    ~FunctionKeyboard();

    friend class FunctionKeyboardConfig;

    void changeRows(int);
    void changeCols(int);

    void load(const Profile &);
    void loadDefaults();

    void paintEvent(QPaintEvent *);
    void paintKey(uint, uint);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void resizeEvent(QResizeEvent*);
    QSize sizeHint() const;

signals:

    void keyPressed(FKey, ushort, ushort, bool);


private:

    // thie key for the map is the row/col
    QMap<QString, FKey> keys;

    uint numRows;
    int test;
    uint numCols;
    uint keyHeight;
    double keyWidth; // decimal point matters!

    int pressedRow, pressedCol;

    QObject *parent;

};

class FunctionKeyboardConfig : public ProfileDialogKeyWidget {

    Q_OBJECT

public:
    FunctionKeyboardConfig(const QString& name, QWidget *wid, const char* na = 0l );
    ~FunctionKeyboardConfig();

    void load(const Profile&);
    void save(Profile&);

private slots:

    void slotKeyPressed(FKey, ushort, ushort, bool);
    void slotChangeRows(int);
    void slotChangeCols(int);
    void slotChangeIcon(int);
    void slotChangeLabelText(const QString &);
    void slotChangeQCode(const QString &);
    void slotChangeUnicode(const QString &);

private:

    QString selectedHandle;
    ushort selectedRow, selectedCol;

    FunctionKeyboard *kb;
    QSpinBox *m_rowBox, *m_colBox;
    QComboBox *m_labels, *m_qvalues, *m_uniValues;

};

#endif
