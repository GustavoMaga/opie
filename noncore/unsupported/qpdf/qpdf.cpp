zsh: no such file or directory: /tr
		setFullscreen ( true );
}

void QPdfDlg::focusInEvent ( QFocusEvent * )
{
	if ( m_fullscreen )
		setFullscreen ( true );
}

void QPdfDlg::toggleFullscreen ( )
{
	if ( m_to_full-> isOn ( ) == m_fullscreen )
		m_to_full-> setOn ( !m_fullscreen );

	m_fullscreen = !m_fullscreen;
	setFullscreen ( m_fullscreen );
}

void QPdfDlg::setFullscreen ( bool b )
{
	static QSize normalsize;

	if ( b ) {
		if ( !normalsize. isValid ( )) 
			normalsize = size ( );
		
		setFixedSize ( qApp-> desktop ( )-> size ( ));
		showNormal ( );
		reparent ( 0, WStyle_Customize | WStyle_NoBorder, QPoint ( 0, 0 ));
		showFullScreen ( );
	}
	else {
		showNormal ( );
		reparent ( 0, 0, QPoint ( 0, 0 ));
		resize ( normalsize );
		showMaximized ( );
		normalsize = QSize ( );
	}	
}

// ^^ Fullscreen handling (for broken QT-lib)

void QPdfDlg::setBusy ( bool b )
{
	if ( b != m_busy ) {
		m_busy = b;
		
		m_outdev-> setBusy ( m_busy );
		setEnabled ( !m_busy );
	}
}

bool QPdfDlg::busy ( ) const
{
	return m_busy;
}


void QPdfDlg::updateCaption ( )
{
	QString cap = "";
	
	if ( !m_currentdoc. isEmpty ( )) 
		cap = QString ( "%1 - " ). arg ( m_currentdoc );
	cap += "QPdf";
	
	setCaption ( cap );
}


void QPdfDlg::setZoom ( int id )
{
	int dpi = 0;

	switch ( id ) {
	case 1:
		if ( m_doc && m_doc-> isOk ( )) 
			dpi = m_outdev-> visibleWidth ( ) * 72 / m_doc-> getPageWidth ( m_currentpage );
		break;
	
	case 2:
		if ( m_doc && m_doc-> isOk ( )) 
			dpi = QMIN( m_outdev-> visibleWidth ( ) * 72 / m_doc-> getPageWidth ( m_currentpage ), \
			            m_outdev-> visibleHeight ( ) * 72 / m_doc-> getPageHeight ( m_currentpage ));
		break;
		
	default:
		dpi = id * 72 / 100;
		break;
	}
	
	if ( dpi < 18 )
		dpi = 18;
	if ( dpi > 216 )
		dpi = 216;

	for ( uint i = 0; i < m_pm_zoom-> count ( ); i++ ) {
		int xid = m_pm_zoom-> idAt ( i );
		m_pm_zoom-> setItemChecked ( xid, xid == id );
	}

	if ( dpi != m_zoom ) { 
		m_zoom = dpi;

		renderPage ( );
	}
}


void QPdfDlg::gotoPageDialog ( )
{
	QDialog *d = new QDialog ( this, "gotodlg", true );
	d-> setCaption ( tr( "Goto page" ));
	
	QBoxLayout *lay = new QVBoxLayout ( d, 4, 4  );	
	
	QLabel *l = new QLabel ( tr( "Select from 1 .. %1:" ). arg ( m_pages ), d );
	lay-> addWidget ( l );
	
	QSpinBox *spin = new QSpinBox ( 1, m_pages, 1, d );
	spin-> setValue ( m_currentpage );
	spin-> setWrapping ( true );
	spin-> setButtonSymbols ( QSpinBox::PlusMinus );
	lay-> addWidget ( spin );
	
	if ( d-> exec ( ) == QDialog::Accepted ) {
		gotoPage ( spin-> value ( ));
	}
	
	delete d;
}

void QPdfDlg::toggleFindBar ( )
{
	if ( m_to_find-> isOn ( ) == m_tb_find-> isVisible ( ))
		m_to_find-> setOn ( !m_tb_find-> isVisible ( ));

	if ( m_tb_find-> isVisible ( )) {
		m_tb_find-> hide ( );
		m_outdev-> setFocus ( );
	}
	else {
		m_tb_find-> show ( );
		m_findedit-> setFocus ( );
	}
}

void QPdfDlg::findText ( const QString &str )
{
	if ( !m_doc || !m_doc-> isOk ( ) || str. isEmpty ( )) 
		return;

	TextOutputDev *textOut = 0;
	int pg = 0;

	setBusy ( true );
		
	int len = str. length ( );
	Unicode *u = new Unicode [len];
	for ( int i = 0; i < len; i++ )
		u [i] = str [i]. unicode ( );
		
	int xMin = 0, yMin = 0, xMax = 0, yMax = 0;
	QRect selr = m_outdev-> selection ( );
	bool fromtop = true; 
	
	if ( selr. isValid ( )) {
		xMin = selr. right ( );
		yMin = selr. top ( ) + selr. height ( ) / 2;
		fromtop = false;
	}

	if ( m_outdev-> findText ( u, len, fromtop, true, &xMin, &yMin, &xMax, &yMax )) 
		goto found;

	qApp-> processEvents ( );
		
	// search following pages
	textOut = new TextOutputDev ( 0, gFalse, gFalse );
	if ( !textOut-> isOk ( )) 
		goto done;
	
	qApp-> processEvents ( );
		
	for ( pg = ( m_currentpage % m_pages ) + 1; pg != m_currentpage; pg = ( pg % m_pages ) + 1 ) {
		m_doc-> displayPage ( textOut, pg, 72, 0, gFalse );
		
		fp_t xMin1, yMin1, xMax1, yMax1;
		
		qApp-> processEvents ( );
		
		if ( textOut-> findText ( u, len, gTrue, gTrue, &xMin1, &yMin1, &xMax1, &yMax1 ))
			goto foundPage;
			
		qApp-> processEvents ( );	
	}

	// search current page ending at current selection
	if ( selr. isValid ( )) {
		xMax = selr. left ( );
		yMax = selr. top ( ) + selr. height ( ) / 2;

		if ( m_outdev-> findText ( u, len, gTrue, gFalse, &xMin, &yMin, &xMax, &yMax )) 
			goto found;
	}
	
	// not found
	QMessageBox::information ( this, tr( "Find..." ), tr( "'%1' could not be found." ). arg ( str ));
	goto done;

foundPage:
	qApp-> processEvents ( );

	gotoPage ( pg );
	
	if ( !m_outdev-> findText ( u, len, gTrue, gTrue, &xMin, &yMin, &xMax, &yMax )) {
		// this can happen if coalescing is bad
		goto done;
	}

found:
	selr. setCoords ( xMin, yMin, xMax, yMax );
	m_outdev-> setSelection ( selr, true ); // this will emit QPEOutputDev::selectionChanged ( ) -> copyToClipboard ( )

done:			

	delete [] u; 
	delete textOut;

	setBusy ( false );
}
		

void QPdfDlg::findText ( )
{
	findText ( m_findedit-> text ( ));
}

void QPdfDlg::copyToClipboard ( const QRect &r )
{
	if ( m_doc && m_doc-> isOk ( ) && m_doc-> okToCopy ( ))
		qApp-> clipboard ( )-> setText ( m_outdev-> getText ( r ));
}

void QPdfDlg::firstPage ( )
{
	gotoPage ( 1 );
}

void QPdfDlg::prevPage ( )
{
	gotoPage ( m_currentpage - 1 );
}

void QPdfDlg::nextPage ( )
{
	gotoPage ( m_currentpage + 1 );
}

void QPdfDlg::lastPage ( )
{
	gotoPage ( m_pages );
}

void QPdfDlg::gotoPage ( int n )
{
	if ( n < 1 )
		n = 1;
	if ( n > m_pages )
		n = m_pages;
		
	if ( n != m_currentpage ) {
		m_currentpage = n;	
		
		renderPage ( );
	}
}

void QPdfDlg::renderPage ( )
{
	if ( m_renderok && m_doc && m_doc-> isOk ( )) {
		m_renderok = false;
	
		setBusy ( true );
		m_doc-> displayPage ( m_outdev, m_currentpage, m_zoom, 0, true ); 
		setBusy ( false );
		
		m_outdev-> setPageCount ( m_currentpage, m_pages );
		
		m_renderok = true;
	}
}

void QPdfDlg::closeFileSelector ( )
{
	m_tb_menu-> show ( );
	m_tb_tool-> show ( );
	m_stack-> raiseWidget ( m_outdev );
}

void QPdfDlg::openFile ( )
{
	m_tb_menu-> hide ( );
	m_tb_tool-> hide ( );
	m_tb_find-> hide ( );
	m_stack-> raiseWidget ( m_filesel );
}

void QPdfDlg::openFile ( const QString &f )
{
	DocLnk nf;
	nf. setType ( "application/pdf" );
	nf. setFile ( f );
	QFileInfo fi ( f );
	nf. setName ( fi. baseName ( ));
	openFile ( nf );
}

void QPdfDlg::openFile ( const DocLnk &f )
{
	QString fn = f. file ( );
	QFileInfo fi ( fn );
	
	if ( fi. exists ( )) {
		delete m_doc;

		m_doc = new PDFDoc ( new GString ( fn. local8Bit ( )), 0, 0 );

		if ( m_doc-> isOk ( )) {
			m_currentdoc = f. name ( );
			int sep = m_currentdoc. findRev ( '/' );
			if ( sep > 0 )
				m_currentdoc = m_currentdoc. mid ( sep + 1 );

			m_pages = m_doc-> getNumPages ( );
			m_currentpage = 0;
		
			QTimer::singleShot ( 0, this, SLOT( delayedInit ( )));
		}
		else {
			delete m_doc;
			m_doc = 0;
			
			m_currentdoc = QString::null;
		}
	
		updateCaption ( );
	}
	else
		QMessageBox::warning ( this, tr( "Error" ), tr( "File does not exist!" ));
}

void QPdfDlg::setDocument ( const QString &f )
{
	if ( f. find ( ".desktop", 0, true ) == -1 ) 
		openFile ( f );
	else 
		openFile ( DocLnk ( f ));
		
	closeFileSelector ( );
}

void QPdfDlg::delayedInit ( )
{	
	closeFileSelector ( );
	
	m_currentpage = 0;
	m_zoom = 0;
	m_renderok = false;
	
	setZoom ( 100 );
	gotoPage ( 1 );
	
	m_renderok = true;
	
	renderPage ( );
	
	m_outdev-> setFocus ( );
}

