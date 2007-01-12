#include <qpe/qpeapplication.h>
#include <qdialog.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qpushbutton.h>

#ifndef CONFIGDLG_H
#define CONFIGDLG_H

class ConfigDlg : public QDialog
{
    Q_OBJECT

public:
    ConfigDlg ();
    static QStringList loadSw();

signals:
    void pickboardToggled(bool on_off);
    void repeatToggled(bool on_off);
    void setMapToDefault();
    void setMapToFile(QString map);
    void reloadKeyboard();
    void configDlgClosed();
    void reloadSw();

protected:
    virtual void accept();

protected slots:
    void moveSelectedUp();
    void moveSelectedDown();

private slots:
    void setMap(int index);
    void addMap();
    void removeMap();
    virtual void closeEvent ( QCloseEvent * );

    // all those required slots for the color push buttons
    void keyColorClicked();
    void keyColorPressedClicked();
    void keyColorLinesClicked();
    void textColorClicked();

private:
    QCheckBox *pick_button;
    QCheckBox *repeat_button;
    QListBox *keymaps;
    QPushButton *add_button;
    QPushButton *remove_button;

    QStringList default_maps; // the maps in your share/multikey/ dir
    QStringList custom_maps; // maps you added with the 'add' button
    QStringList sw_maps; // maps used in keyboard switch rotation ring

    /* color buttons */
    QPushButton *keycolor_button;
    QPushButton *keycolor_pressed_button;
    QPushButton *keycolor_lines_button;
    QPushButton *textcolor_button;

};

#endif
