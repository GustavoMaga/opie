#include "irctab.h"
#include "mainwindow.h"

#include <opie2/okeyconfigmanager.h>
#include <qpe/resource.h>


#include <qpushbutton.h>
#include <qwhatsthis.h>


QString IRCTab::m_errorColor;
QString IRCTab::m_serverColor;
QString IRCTab::m_textColor;
QString IRCTab::m_backgroundColor;
QString IRCTab::m_selfColor;
QString IRCTab::m_otherColor;
QString IRCTab::m_notificationColor;
int IRCTab::m_maxLines;


static bool g_useTime =  false;

void IRCTab::setUseTimeStamps( bool b ) {
    g_useTime = b;
}

// ## FIXME use TimeString later for AM/PM Setting
QString IRCTab::appendTimestamp( const QString& text ) {
    return g_useTime ?
        "[" +QTime::currentTime().toString()+"]" + text + "\n" :
        text + "\n";

}


IRCTab::IRCTab(QWidget *parent, const char *name, WFlags f) : QWidget(parent, name, f) {
    m_layout = new QVBoxLayout(this);
    QHBoxLayout *descLayout = new QHBoxLayout(m_layout);
    descLayout->setMargin(5);
    m_description = new QLabel(tr("Missing description"), this);
    QWhatsThis::add(m_description, tr("Description of the tab's content"));
    descLayout->addWidget(m_description);
    descLayout->setStretchFactor(m_description, 5);
    QPushButton *close = new QPushButton(this);
    QWhatsThis::add(close, tr("Close this tab"));
    close->setPixmap(Resource::loadPixmap("close"));
    connect(close, SIGNAL(clicked()), this, SLOT(remove()));
    descLayout->addWidget(close);
    descLayout->setStretchFactor(m_description, 1);
}


void IRCTab::setID(int id) {
    m_id = id;
}

int IRCTab::id() {
    return m_id;
}

void IRCTab::showEvent( QShowEvent *ev ) {
    topLevelWidget()->setCaption( MainWindow::appCaption() + " " + title() );
    QWidget::showEvent( ev );
    emit editFocus();
}
