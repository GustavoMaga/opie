#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qstringlist.h>
#include <qcombobox.h>


#include <opie/otabwidget.h>

#include "profileeditorplugins.h"
#include "metafactory.h"
#include "profileeditordialog.h"

namespace {
    void setCurrent( const QString& str, QComboBox* bo ) {
        for (uint i = 0; i < bo->count(); i++ ) {
            if ( bo->text(i) == str ) {
                bo->setCurrentItem( i );
            }
        }
    }


}

ProfileEditorDialog::ProfileEditorDialog( MetaFactory* fact,
                                          const Profile& prof )
    : QDialog(0, 0, TRUE), m_fact( fact ), m_prof( prof )
{
    initUI();

	// Apply current profile
	// plugin_plugin->load(profile);
	// ... (reset profile name line edit etc.)
}

ProfileEditorDialog::ProfileEditorDialog( MetaFactory* fact )
    : QDialog(0, 0, TRUE), m_fact( fact )
{
	// Default profile
	m_prof = Profile("New Profile", "serial", "default", Profile::Black, Profile::White, Profile::VT102);

	initUI();

	// Apply current profile
	// plugin_plugin->load(profile);
}

Profile ProfileEditorDialog::profile() const
{
	return m_prof;
}

void ProfileEditorDialog::initUI()
{
    m_con = m_term = 0l;

    QVBoxLayout *mainLayout = new QVBoxLayout( this );
    OTabWidget *tabWidget = new OTabWidget( this );
    tabWidget->setTabStyle(OTabWidget::TextTab);
    mainLayout->add(tabWidget);

    QWidget *tabprof;

    /* base tabs */
    tabprof   = new QWidget(this);
    m_tabTerm = new QWidget(this);
    m_tabCon  = new QWidget(this);

    /* base layout for tabs */
    m_layCon  = new QHBoxLayout( m_tabCon , 2 );
    m_layTerm = new QHBoxLayout( m_tabTerm, 2 );

    // profile tab

    QLabel *name = new QLabel(QObject::tr("Profile name"), tabprof);
    m_name = new QLineEdit(tabprof);
    QLabel *con = new QLabel(tr("Connection"), tabprof );
    QLabel *term = new QLabel(tr("Terminal"), tabprof );
    m_conCmb = new QComboBox( tabprof );
    m_termCmb = new QComboBox( tabprof );

    // layouting
    QVBoxLayout *vbox3 = new QVBoxLayout(tabprof, 2);
    vbox3->add(name);
    vbox3->add(m_name);
    vbox3->add(con );
    vbox3->add(m_conCmb );
    vbox3->add(term );
    vbox3->add(m_termCmb );
    vbox3->addStretch(1);

    tabWidget->addTab(tabprof, "", QObject::tr("Profile"));
    tabWidget->addTab(m_tabCon, "", QObject::tr("Connection"));
    tabWidget->addTab(m_tabTerm, "", QObject::tr("Terminal"));
    tabWidget->setCurrentTab( tabprof );


    // fill the comboboxes
    QStringList list = m_fact->connectionWidgets();
    QStringList::Iterator it;
    for (it =list.begin(); it != list.end(); ++it ) {
        m_conCmb->insertItem( (*it) );
    }
    list = m_fact->terminalWidgets();
    for (it =list.begin(); it != list.end(); ++it ) {
        m_termCmb->insertItem( (*it) );
    }

    // load profile values
    m_name->setText(m_prof.name());
    slotConActivated(  m_fact->external(m_prof.ioLayerName()  ) );
    slotTermActivated( m_fact->external(m_prof.terminalName() ) );
    setCurrent( m_fact->external(m_prof.ioLayerName() ), m_conCmb );
    setCurrent( m_fact->external(m_prof.terminalName() ), m_termCmb );

    qWarning("Layer: %s %s", m_prof.ioLayerName().data(),
             m_fact->external(m_prof.ioLayerName() ).latin1() );
    qWarning("Term: %s %s", m_prof.terminalName().data(),
             m_fact->external(m_prof.terminalName() ).latin1() );

    // signal and slots
    connect(m_conCmb, SIGNAL(activated(const QString& ) ),
            this, SLOT(slotConActivated(const QString&) ) );
    connect(m_termCmb, SIGNAL(activated(const QString& ) ),
            this, SLOT(slotTermActivated(const QString& ) ) );

}

ProfileEditorDialog::~ProfileEditorDialog() {

}
void ProfileEditorDialog::accept()
{
	if(profName().isEmpty())
	{
		QMessageBox::information(this,
			QObject::tr("Invalid profile"),
			QObject::tr("Please enter a profile name."));
		return;
	}
	// Save profile and plugin profile
	//if(plugin_plugin) plugin_plugin->save();

	// Save general values
	m_prof.setName(profName());
	m_prof.setIOLayer(      m_fact->internal(m_conCmb ->currentText()  ) );
        m_prof.setTerminalName( m_fact->internal(m_termCmb->currentText()  ) );
        qWarning("Term %s %s", m_fact->internal(m_termCmb->currentText() ).data(),
                 m_termCmb->currentText().latin1() );

        if (m_con )
            m_con->save( m_prof );
        if (m_term )
            m_term->save( m_prof );

	QDialog::accept();
}


QString ProfileEditorDialog::profName()const
{
	return m_name->text();
}

QCString ProfileEditorDialog::profType()const
{
    /*QStringList w = m_fact->configWidgets();
	for(QStringList::Iterator it = w.begin(); it != w.end(); it++)
		if(device_box->currentText() == m_fact->name((*it))) return (*it);
    */
    return QCString();
}
/*
 * we need to switch the widget
 */
void ProfileEditorDialog::slotConActivated( const QString& str ) {
    delete m_con;
    m_con = m_fact->newConnectionPlugin( str, m_tabCon );

    if (m_con ) {
        m_con->load( m_prof );
        m_layCon->addWidget( m_con );
    }
}
/*
 * we need to switch the widget
 */
void ProfileEditorDialog::slotTermActivated( const QString& str ) {
    delete m_term;
    m_term = m_fact->newTerminalPlugin( str, m_tabTerm );
    qWarning("past");

    if (m_term) {
        m_term->load(m_prof );
        m_layTerm->addWidget( m_term );
    }
}

