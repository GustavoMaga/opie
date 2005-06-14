#include "imageinfoui.h"

#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtextview.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qfileinfo.h>

#include "lib/slavemaster.h"
#include "lib/imagecache.h"

#include <opie2/oconfig.h>
#include <opie2/okeyconfigwidget.h>
#include <opie2/odebug.h>
#include <opie2/oresource.h>

#include <qpe/qcopenvelope_qws.h>

static const int THUMBSIZE = 128;

using namespace Opie::Core;

imageinfo::imageinfo(Opie::Core::OConfig *cfg,QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    m_viewManager = 0;
    m_cfg = cfg;
    init(name);
    initKeys();
}

imageinfo::imageinfo(const QString&_path, QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl ),currentFile(_path)
{
    m_viewManager = 0;
    m_cfg = 0;
    init(name);
    initKeys();
    slotChangeName(_path);
}

Opie::Core::OKeyConfigManager* imageinfo::manager()
{
    if (!m_viewManager) {
        initKeys();
    }
    return m_viewManager;
}

void imageinfo::initKeys()
{
#if 0
    if (!m_cfg) {
        m_cfg = new Opie::Core::OConfig("phunkview");
        m_cfg->setGroup("imageinfo_keys" );
    }
#endif
    Opie::Core::OKeyPair::List lst;
    m_viewManager = new Opie::Core::OKeyConfigManager(m_cfg, "imageinfo_keys",
                                                    lst, false,this, "keyconfig name" );
    m_viewManager->addKeyConfig( Opie::Core::OKeyConfigItem(tr("View Full Image"), "infoview",
                                                            Opie::Core::OResource::loadPixmap("1to1", Opie::Core::OResource::SmallIcon),
                                                            ViewItem, Opie::Core::OKeyPair(Qt::Key_V, Qt::ShiftButton),
                                                            this, SLOT(slotShowImage())));
    m_viewManager->load();
    m_viewManager->handleWidget( this );
    m_viewManager->handleWidget( TextView1 );
}

void imageinfo::slotShowImage()
{
    emit dispImage(currentFile);
}

void imageinfo::init(const char* name) {
    {
        QCopEnvelope( "QPE/Application/opie-eye_slave", "refUp()" );
    }
    if ( !name )
    setName( "imageinfo" );
    resize( 289, 335 );
    setCaption( tr( "Image info" ) );
    imageinfoLayout = new QVBoxLayout( this );
    imageinfoLayout->setSpacing(2);
    imageinfoLayout->setMargin(4);

    PixmapLabel1 = new QLabel( this, "PixmapLabel1" );
    PixmapLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, PixmapLabel1->sizePolicy().hasHeightForWidth() ) );
    QWhatsThis::add(  PixmapLabel1, tr("Displays an thumbnail of the image") );

    imageinfoLayout->addWidget( PixmapLabel1 );

    Line1 = new QFrame( this, "Line1" );
    Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    imageinfoLayout->addWidget( Line1 );

    fnameLabel = new QLabel( this, "FnameLabel" );
    imageinfoLayout->addWidget( fnameLabel);

    TextView1 = new QTextView( this, "TextView1" );
    TextView1->setFrameShadow( QTextView::Sunken );
    TextView1->setResizePolicy( QTextView::AutoOneFit );
    TextView1->setBackgroundOrigin( QTextView::ParentOrigin );
    TextView1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, TextView1->sizePolicy().hasHeightForWidth() ) );
//    TextView1->setVScrollBarMode(QScrollView::AlwaysOn);
    QWhatsThis::add(  TextView1, tr("Displays info of selected image") );
    imageinfoLayout->addWidget( TextView1 );

    SlaveMaster* master = SlaveMaster::self();
    connect( master, SIGNAL(sig_fullInfo(const QString&, const QString&)),
             this, SLOT(slot_fullInfo(const QString&, const QString&)) );
    connect(master, SIGNAL( sig_thumbNail(const QString&, const QPixmap&)),
            this, SLOT(slotThumbNail(const QString&, const QPixmap&)));
}

void imageinfo::slotChangeName(const QString&_path)
{
    currentFile=_path;
    QFileInfo fi(_path);
    fnameLabel->setText("<qt><center><b>"+fi.fileName()+"</b></center></qt>");
    SlaveMaster::self()->imageInfo( currentFile );

    QPixmap*m_pix = PPixmapCache::self()->cachedImage( _path, THUMBSIZE,THUMBSIZE );
    if (!m_pix) {
        PixmapLabel1->setPixmap(QPixmap( Opie::Core::OResource::loadPixmap( "UnknownDocument", Opie::Core::OResource::SmallIcon )));
        SlaveMaster::self()->thumbNail(currentFile,THUMBSIZE,THUMBSIZE);
    } else {
        PixmapLabel1->setPixmap(*m_pix);
    }
}

imageinfo::~imageinfo()
{
    {
        QCopEnvelope( "QPE/Application/opie-eye_slave", "refDown()" );
    }
    if (m_viewManager) {
        delete m_viewManager;
    }
}

void imageinfo::slot_fullInfo(const QString&_path, const QString&_t)
{
    if (_path == currentFile) {
        odebug << _t << oendl;
        QString t = _t;
        t.replace(QRegExp("\n"),"<br>");
        TextView1->setText(t);
    }
}

void imageinfo::slotThumbNail(const QString&_path, const QPixmap&_pix)
{
    if (_path == currentFile) {
        if (_pix.width()>0) {
            PPixmapCache::self()->insertImage( _path, _pix, THUMBSIZE, THUMBSIZE );
            PixmapLabel1->setPixmap( _pix );
            PixmapLabel1->resize(QSize(_pix.width(),_pix.height()));
        }
    }
}

void imageinfo::setPath( const QString& str ) {
    slotChangeName( str );
}

void imageinfo::setDestructiveClose() {
    WFlags fl = getWFlags();
    /* clear it just in case */
    fl &= ~WDestructiveClose;
    fl |= WDestructiveClose;
    setWFlags( fl );
}


/* for testing */
infoDlg::infoDlg(const QString&fname,QWidget * parent, const char * name)
    :QDialog(parent,name,true,WStyle_ContextHelp)
{
    QVBoxLayout*dlglayout = new QVBoxLayout(this);
    dlglayout->setSpacing(2);
    dlglayout->setMargin(1);
    imageinfo*inf = new imageinfo(fname,this);
    dlglayout->addWidget(inf);
}

infoDlg::~infoDlg()
{
}


