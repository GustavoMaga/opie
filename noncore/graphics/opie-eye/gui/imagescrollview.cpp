#include "imagescrollview.h"

#include "lib/oimagezoomer.h"

#include <opie2/odebug.h>
#include <opie2/oapplication.h>
#include <opie2/owait.h>

#include <qimage.h>
#include <qlayout.h>
#include <qpe/qcopenvelope_qws.h>

/* for usage with the bitset */
#define AUTO_SCALE 0
#define AUTO_ROTATE 1
#define SHOW_ZOOMER 2
#define FIRST_RESIZE_DONE 3
#define IMAGE_IS_JPEG 4
#define IMAGE_SCALED_LOADED 5

#define SCROLLVIEW_BITSET_SIZE 6

namespace Opie {
namespace MM {
OImageScrollView::OImageScrollView( QWidget* parent, const char* name,  WFlags f )
    :QScrollView(parent,name,f|Qt::WRepaintNoErase ),_image_data(),_original_data(),
    m_states(SCROLLVIEW_BITSET_SIZE),m_lastName("")
{
    _zoomer = 0;
    m_states[AUTO_SCALE]=true;
    m_states[AUTO_ROTATE]=true;
    m_states[FIRST_RESIZE_DONE]=false;
    m_states[IMAGE_IS_JPEG]=false;
    m_states[IMAGE_SCALED_LOADED]=false;
    m_states[SHOW_ZOOMER]=true;
    init();
}

OImageScrollView::OImageScrollView (const QImage&img, QWidget * parent, const char * name, WFlags f,bool always_scale,bool rfit)
    :QScrollView(parent,name,f|Qt::WRepaintNoErase),_image_data(),_original_data(img),
     m_states(SCROLLVIEW_BITSET_SIZE),m_lastName("")
{
    _zoomer = 0;
    m_states[AUTO_SCALE]=always_scale;
    m_states[AUTO_ROTATE]=rfit;
    m_states[FIRST_RESIZE_DONE]=false;
    m_states[IMAGE_IS_JPEG]=false;
    m_states[IMAGE_SCALED_LOADED]=false;
    m_states[SHOW_ZOOMER]=true;
    _original_data.convertDepth(QPixmap::defaultDepth());
    _original_data.setAlphaBuffer(false);
    init();
}

OImageScrollView::OImageScrollView (const QString&img, QWidget * parent, const char * name, WFlags f,bool always_scale,bool rfit)
    :QScrollView(parent,name,f|Qt::WRepaintNoErase),_image_data(),_original_data(),m_states(SCROLLVIEW_BITSET_SIZE),m_lastName("")
{
    _zoomer = 0;
    m_states.resize(SCROLLVIEW_BITSET_SIZE);
    m_states[AUTO_SCALE]=always_scale;
    m_states[AUTO_ROTATE]=rfit;
    m_states[FIRST_RESIZE_DONE]=false;
    m_states[IMAGE_IS_JPEG]=false;
    m_states[IMAGE_SCALED_LOADED]=false;
    m_states[SHOW_ZOOMER]=true;
    init();
    setImage(img);
}

void OImageScrollView::setImage(const QImage&img)
{
    _image_data = QImage();
    _original_data=img;
    _original_data.convertDepth(QPixmap::defaultDepth());
    _original_data.setAlphaBuffer(false);
    m_lastName = "";
    setImageIsJpeg(false);
    setImageScaledLoaded(false);
    if (FirstResizeDone()) {
        generateImage();
    }
}

void OImageScrollView::loadJpeg(bool interncall)
{
    if (m_lastName.isEmpty()) return;
    QImageIO iio( m_lastName, 0l );
    QString param;
    bool real_load = false;
    if (AutoScale()) {
        if (!interncall) {
            int wid, hei;
            wid = QApplication::desktop()->width();
            hei = QApplication::desktop()->height();
            if (hei>wid) {
                wid = hei;
            } else {
                hei = wid;
            }
            param = QString( "Fast Shrink( 7 ) Scale( %1, %2, ScaleFree)" ).arg( wid ).arg( hei );
            odebug << "Load jpeg scaled \"" << param << "\"" << oendl;
            iio.setParameters(param.latin1());
            setImageScaledLoaded(true);
            real_load = true;
        }
    } else {
        if (ImageScaledLoaded()||!interncall) {
            odebug << "Load jpeg unscaled" << oendl;
            real_load = true;
        }
        setImageScaledLoaded(false);
    }
    if (real_load) {
        {
            QCopEnvelope( "QPE/System", "busy()" );
        }
        _original_data = iio.read() ? iio.image() : QImage();
        {
            QCopEnvelope env( "QPE/System", "notBusy(QString)" );
            env << "Image loaded";
        }
    }
}

void OImageScrollView::setImage( const QString& path ) {
    odebug << "load new image " << oendl;
    if (m_lastName == path) return;
    m_lastName = path;
    QString itype = QImage::imageFormat(m_lastName);
    odebug << "Image type = " << itype << oendl;
    if (itype == "JPEG") {
        setImageIsJpeg(true);
        loadJpeg();
    } else {
        {
            QCopEnvelope( "QPE/System", "busy()" );
        }
        setImageIsJpeg(false);
        _original_data.load(path);
        _original_data.convertDepth(QPixmap::defaultDepth());
        _original_data.setAlphaBuffer(false);
        {
            QCopEnvelope env( "QPE/System", "notBusy(QString)" );
            env << "Image loaded";
        }
    }
    _image_data = QImage();
    if (FirstResizeDone()) {
        generateImage();
    }
}

/* should be called every time the QImage changed it content */
void OImageScrollView::init()
{
    odebug << "init " << oendl;

    /*
     * create the zoomer
     * and connect ther various signals
     */
    _zoomer = new Opie::MM::OImageZoomer( this, "The Zoomer" );
    connect(_zoomer, SIGNAL( zoomAreaRel(int,int)),
            this, SLOT(scrollBy(int,int)) );
    connect(_zoomer, SIGNAL( zoomArea(int,int)),
            this, SLOT(center(int,int)) );
    connect(this,SIGNAL(contentsMoving(int,int)),
            _zoomer, (SLOT(setVisiblePoint(int,int))) );
    connect(this,SIGNAL(imageSizeChanged(const QSize&)),
            _zoomer, SLOT(setImageSize(const QSize&)) );
    connect(this,SIGNAL(viewportSizeChanged(const QSize&)),
            _zoomer, SLOT(setViewPortSize(const QSize&)) );

    viewport()->setBackgroundColor(white);
    setFocusPolicy(QWidget::StrongFocus);
    setImageScaledLoaded(false);
    setImageIsJpeg(false);
    if (FirstResizeDone()) {
        m_last_rot = Rotate0;
        generateImage();
    } else if (_original_data.size().isValid()) {
        if (image_fit_into(_original_data.size()) || !ShowZoomer()) _zoomer->hide();
        resizeContents(_original_data.width(),_original_data.height());
    }
}

void OImageScrollView::setAutoRotate(bool how)
{
    /* to avoid double repaints */
    if (AutoRotate() != how) {
        m_states.setBit(AUTO_ROTATE,how);
        _image_data = QImage();
        generateImage();
    }
}

bool OImageScrollView::AutoRotate()const
{
    return m_states.testBit(AUTO_ROTATE);
}

void OImageScrollView::setAutoScale(bool how)
{
    m_states.setBit(AUTO_SCALE,how);
    if (!how) {
        setAutoRotate(false);
    }
    _image_data = QImage();
    if (ImageIsJpeg() && how == false && ImageScaledLoaded()==true) {
        loadJpeg(true);
    }
    generateImage();
}

bool OImageScrollView::AutoScale()const
{
    return m_states.testBit(AUTO_SCALE);
}

OImageScrollView::~OImageScrollView()
{
}

void OImageScrollView::rescaleImage(int w, int h)
{
    if (_image_data.width()==w && _image_data.height()==h) {
        return;
    }
    double hs = (double)h / (double)_image_data.height() ;
    double ws = (double)w / (double)_image_data.width() ;
    double scaleFactor = (hs > ws) ? ws : hs;
    int smoothW = (int)(scaleFactor * _image_data.width());
    int smoothH = (int)(scaleFactor * _image_data.height());
    _image_data = _image_data.smoothScale(smoothW,smoothH);
}

void OImageScrollView::rotate_into_data(Rotation r)
{
    /* realy - we must do this that way, 'cause when acting direct on _image_data the app will
       segfault :( */
    QImage dest;
    int x, y;
    if ( _original_data.depth() > 8 )
    {
        unsigned int *srcData, *destData;
        switch ( r )
        {
            case Rotate90:
                dest.create(_original_data.height(), _original_data.width(), _original_data.depth());
                for ( y=0; y < _original_data.height(); ++y )
                {
                    srcData = (unsigned int *)_original_data.scanLine(y);
                    for ( x=0; x < _original_data.width(); ++x )
                    {
                        destData = (unsigned int *)dest.scanLine(x);
                        destData[_original_data.height()-y-1] = srcData[x];
                    }
                }
                break;
            case Rotate180:
                dest.create(_original_data.width(), _original_data.height(), _original_data.depth());
                for ( y=0; y < _original_data.height(); ++y )
                {
                    srcData = (unsigned int *)_original_data.scanLine(y);
                    destData = (unsigned int *)dest.scanLine(_original_data.height()-y-1);
                    for ( x=0; x < _original_data.width(); ++x )
                        destData[_original_data.width()-x-1] = srcData[x];
                }
                break;
            case Rotate270:
                dest.create(_original_data.height(), _original_data.width(), _original_data.depth());
                for ( y=0; y < _original_data.height(); ++y )
                {
                    srcData = (unsigned int *)_original_data.scanLine(y);
                    for ( x=0; x < _original_data.width(); ++x )
                    {
                        destData = (unsigned int *)dest.scanLine(_original_data.width()-x-1);
                        destData[y] = srcData[x];
                    }
                }
                break;
            default:
                dest = _original_data;
                break;
        }
    }
    else
    {
        unsigned char *srcData, *destData;
        unsigned int *srcTable, *destTable;
        switch ( r )
        {
            case Rotate90:
                dest.create(_original_data.height(), _original_data.width(), _original_data.depth());
                dest.setNumColors(_original_data.numColors());
                srcTable = (unsigned int *)_original_data.colorTable();
                destTable = (unsigned int *)dest.colorTable();
                for ( x=0; x < _original_data.numColors(); ++x )
                    destTable[x] = srcTable[x];
                for ( y=0; y < _original_data.height(); ++y )
                {
                    srcData = (unsigned char *)_original_data.scanLine(y);
                    for ( x=0; x < _original_data.width(); ++x )
                    {
                        destData = (unsigned char *)dest.scanLine(x);
                        destData[_original_data.height()-y-1] = srcData[x];
                    }
                }
                break;
            case Rotate180:
                dest.create(_original_data.width(), _original_data.height(), _original_data.depth());
                dest.setNumColors(_original_data.numColors());
                srcTable = (unsigned int *)_original_data.colorTable();
                destTable = (unsigned int *)dest.colorTable();
                for ( x=0; x < _original_data.numColors(); ++x )
                    destTable[x] = srcTable[x];
                for ( y=0; y < _original_data.height(); ++y )
                {
                    srcData = (unsigned char *)_original_data.scanLine(y);
                    destData = (unsigned char *)dest.scanLine(_original_data.height()-y-1);
                    for ( x=0; x < _original_data.width(); ++x )
                        destData[_original_data.width()-x-1] = srcData[x];
                }
                break;
            case Rotate270:
                dest.create(_original_data.height(), _original_data.width(), _original_data.depth());
                dest.setNumColors(_original_data.numColors());
                srcTable = (unsigned int *)_original_data.colorTable();
                destTable = (unsigned int *)dest.colorTable();
                for ( x=0; x < _original_data.numColors(); ++x )
                    destTable[x] = srcTable[x];
                for ( y=0; y < _original_data.height(); ++y )
                {
                    srcData = (unsigned char *)_original_data.scanLine(y);
                    for ( x=0; x < _original_data.width(); ++x )
                    {
                        destData = (unsigned char *)dest.scanLine(_original_data.width()-x-1);
                        destData[y] = srcData[x];
                    }
                }
                break;
            default:
                dest = _original_data;
                break;
        }

    }
    _image_data = dest;
}

void OImageScrollView::generateImage()
{
    Rotation r = Rotate0;
    if (_original_data.isNull()) return;
    {
        QCopEnvelope( "QPE/System", "busy()" );
    }
    if (width()>height()&&_original_data.width()<_original_data.height() ||
        width()<height()&&_original_data.width()>_original_data.height()) {
        if (AutoRotate()) r = Rotate90;
    }

    odebug << " r = " << r << oendl;
    if (AutoScale()) {
        if (!_image_data.size().isValid()||width()>_image_data.width()||height()>_image_data.height()) {
            odebug << "Rescaling data" << oendl;
            if (r==Rotate0) {
                _image_data = _original_data;
            } else {
                rotate_into_data(r);
            }
        }
        rescaleImage(width(),height());
        resizeContents(_image_data.width(),_image_data.height());
    }  else if (!FirstResizeDone()||r!=m_last_rot||_image_data.width()==0) {
        if (r==Rotate0) {
            _image_data = _original_data;
        } else {
            rotate_into_data(r);
        }
        m_last_rot = r;
        resizeContents(_image_data.width(),_image_data.height());
    }
    _pdata.convertFromImage(_image_data);


    /*
     * update the zoomer
     */
    check_zoomer();
    emit imageSizeChanged( _image_data.size() );
    rescaleImage( 128, 128 );
    /*
     * move scrollbar
     */
     if (_zoomer) {
        _zoomer->setGeometry( viewport()->width()-_image_data.width()/2, viewport()->height()-_image_data.height()/2,
             _image_data.width()/2, _image_data.height()/2 );
        _zoomer->setImage( _image_data );
     }
    /*
     * invalidate
     */
    _image_data=QImage();
    {
        QCopEnvelope env( "QPE/System", "notBusy(QString)" );
        env << "Image generated";
    }
}

void OImageScrollView::resizeEvent(QResizeEvent * e)
{
    odebug << "OImageScrollView resizeEvent" << oendl;
    QScrollView::resizeEvent(e);
    generateImage();
    setFirstResizeDone(true);
    emit viewportSizeChanged( viewport()->size() );

}

void OImageScrollView::keyPressEvent(QKeyEvent * e)
{
    if (!e) return;
    int dx = horizontalScrollBar()->lineStep();
    int dy = verticalScrollBar()->lineStep();
    if (e->key()==Qt::Key_Right) {
        scrollBy(dx,0);
        e->accept();
    } else if (e->key()==Qt::Key_Left) {
        scrollBy(0-dx,0);
        e->accept();
    } else if (e->key()==Qt::Key_Up) {
        scrollBy(0,0-dy);
        e->accept();
    } else if (e->key()==Qt::Key_Down) {
        scrollBy(0,dy);
        e->accept();
    } else {
        e->ignore();
    }
    QScrollView::keyPressEvent(e);
}

void OImageScrollView::drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph)
{
    int w = clipw;
    int h = cliph;
    int x = clipx;
    int y = clipy;
    bool erase = false;

    if (!_pdata.size().isValid()) {
        p->fillRect(clipx,clipy,clipw,cliph,white);
        return;
    }
    if (w>_pdata.width()) {
        w=_pdata.width();
        x = 0;
        erase = true;
    } else if (x+w>_pdata.width()){
        x = _pdata.width()-w;
    }
    if (h>_pdata.height()) {
        h=_pdata.height();
        y = 0;
        erase = true;
    } else if (y+h>_pdata.height()){
        y = _pdata.height()-h;
    }
    if (erase||_original_data.hasAlphaBuffer()) {
        p->fillRect(clipx,clipy,clipw,cliph,white);
    }
    p->drawPixmap(clipx,clipy,_pdata,x,y,w,h);
}

/* using the real geometry points and not the translated points is wanted! */
void OImageScrollView::viewportMouseMoveEvent(QMouseEvent* e)
{
    odebug << "Move X and Y " << e->x() << " " << e->y() << oendl;
    int mx, my;
    mx = e->x();
    my = e->y();
    if (_mouseStartPosX!=-1 && _mouseStartPosY!=-1) {
        int diffx = _mouseStartPosX-mx;
        int diffy = _mouseStartPosY-my;
#if 0
        QScrollBar*xbar = horizontalScrollBar();
        QScrollBar*ybar = verticalScrollBar();
        if (xbar->value()+diffx>xbar->maxValue()) {
            diffx = xbar->maxValue()-xbar->value();
        } else if (xbar->value()+diffx<0) {
            diffx=0-xbar->value();
        }
        if (ybar->value()+diffy>ybar->maxValue()) {
            diffy = ybar->maxValue()-ybar->value();
        } else if (ybar->value()+diffy<0) {
            diffy=0-ybar->value();
        }
#endif
        scrollBy(diffx,diffy);
    }
    _mouseStartPosX=mx;
    _mouseStartPosY=my;
}

void OImageScrollView::contentsMousePressEvent ( QMouseEvent * e)
{
    odebug << " X and Y " << e->x() << " " << e->y() << oendl;
    /* this marks the beginning of a possible mouse move. Due internal reasons of QT
       the geometry values here may real differ from that set in MoveEvent (I don't know
       why). For getting them in real context, we use the first move-event to set the start
       position ;)
    */
    _mouseStartPosX = -1;
    _mouseStartPosY = -1;
}

void OImageScrollView::setDestructiveClose() {
    WFlags fl = getWFlags();
    /* clear it just in case */
    fl &= ~WDestructiveClose;
    fl |= WDestructiveClose;
    setWFlags( fl );
}

bool OImageScrollView::image_fit_into(const QSize&s )
{
    if (s.width()>width()||s.height()>height()) {
        return false;
    }
    return true;
}

void OImageScrollView::setShowZoomer(bool how)
{
    m_states.setBit(SHOW_ZOOMER,how);
    check_zoomer();
}

bool OImageScrollView::ShowZoomer()const
{
    return m_states.testBit(SHOW_ZOOMER);
}

void OImageScrollView::check_zoomer()
{
    if (!_zoomer) return;
    if ( (!ShowZoomer()||image_fit_into(_pdata.size()) ) && _zoomer->isVisible()) {
        _zoomer->hide();
    } else if ( ShowZoomer() && !image_fit_into(_pdata.size()) && _zoomer->isHidden()){
        _zoomer->show();
    }
}

bool OImageScrollView::FirstResizeDone()const
{
    return m_states.testBit(FIRST_RESIZE_DONE);
}

void OImageScrollView::setFirstResizeDone(bool how)
{
    m_states.setBit(FIRST_RESIZE_DONE,how);
}

bool OImageScrollView::ImageIsJpeg()const
{
    return m_states.testBit(IMAGE_IS_JPEG);
}

void OImageScrollView::setImageIsJpeg(bool how)
{
    m_states.setBit(IMAGE_IS_JPEG,how);
}

bool OImageScrollView::ImageScaledLoaded()const
{
    return m_states.testBit(IMAGE_SCALED_LOADED);
}

void OImageScrollView::setImageScaledLoaded(bool how)
{
    m_states.setBit(IMAGE_SCALED_LOADED,how);
}

} // namespace MM
} // namespace Opie
