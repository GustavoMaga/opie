#ifndef DUNDIALOG_H
#define DUNDIALOG_H


#include <qdialog.h>
#include <opie2/oprocess.h>

class QVBoxLayout;
class QPushButton;
class QMultiLineEdit;
class QLineEdit;
class QCheckBox;

namespace OpieTooth {
    class DunDialog : public QDialog {

        Q_OBJECT

    public:
        DunDialog(const QString& device = 0, int port = 0, QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0);
        ~DunDialog();

    private slots:
        void connectToDevice();
        void fillOutPut( Opie::Core::OProcess* pppDial, char* cha, int len );
        void fillErr(Opie::Core::OProcess*, char*, int);
        void slotProcessExited(Opie::Core::OProcess* proc);
        void closeEvent(QCloseEvent* e);
    protected:
        QVBoxLayout* layout;
        QLineEdit* cmdLine;
        QPushButton* connectButton;
        QMultiLineEdit* outPut;
        QCheckBox* doEncryption;

    private:
        QString m_device; //device BT address
        int m_port; //device process
        Opie::Core::OProcess* m_dunConnect; //DUN process
    };
}
#endif
