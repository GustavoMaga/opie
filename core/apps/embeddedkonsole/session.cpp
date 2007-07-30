/* -------------------------------------------------------------------------- */
/*									      */
/* Ported Konsole to Qt/Embedded                                              */
/*									      */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*									      */
/* -------------------------------------------------------------------------- */
#include "session.h"
// #include <kdebug.h>

#include <stdlib.h>

#define HERE fprintf(stderr,"%s(%d): here\n",__FILE__,__LINE__)

/*! \class TESession

    Sessions are combinations of TEPTy and Emulations.

    The stuff in here does not belong to the terminal emulation framework,
    but to main.cpp. It serves its duty by providing a single reference
    to TEPTy/Emulation pairs. In fact, it is only here to demonstrate one
    of the abilities of the framework - multiple sessions.
*/

TESession::TESession(QMainWindow* main, TEWidget* _te, const char* _pgm, QStrList & _args, const char *_term) : schema_no(0), font_no(3), pgm(_pgm), args(_args)
{
  te = _te;
  term = _term;

  // sh = new TEPty();
  sh = new MyPty();
  em = new TEmuVt102(te);

  if (te->Lines() >= 0 && te->Lines() < 65535 &&
      te->Columns() >= 0 &&te->Columns() < 65535)
    sh->setSize(static_cast<unsigned short>(te->Lines()),
		static_cast<unsigned short>(te->Columns()));
    ///< @note It is not absolutely necessary to make this call. But by doing it
    ///<  we can notify clients of the session about a change in the size of the
    ///<  window.
  QObject::connect( sh,SIGNAL(block_in(const char*,int)),
                    em,SLOT(onRcvBlock(const char*,int)) );
  QObject::connect( em,SIGNAL(ImageSizeChanged(int,int)),
                    sh,SLOT(setSize(int,int)));

  /*!
   * @note 'main' should connect the ImageSizeChanged event itself.
   * These aren't KTMW's slots, but konsole's. We therefore need to
   * comment out the connect call for this event. Be careful if you
   * feel you need to provide this connection here.
   */

//  QObject::connect( em,SIGNAL(ImageSizeChanged(int,int)),
//                    main,SLOT(notifySize(int,int)));
  QObject::connect( em,SIGNAL(sndBlock(const char*,int)),
                    sh,SLOT(send_bytes(const char*,int)) );
  QObject::connect( em,SIGNAL(changeColumns(int)),
                    main,SLOT(changeColumns(int)) );

  

  QObject::connect( em,SIGNAL(changeTitle(int,const QString&)),
                    this,SLOT(changeTitle(int,const QString&)) );

  QObject::connect( sh,SIGNAL(done(int)), this,SLOT(done(int)) );
}



void TESession::run()
{
  //kdDebug() << "Running the session!" << pgm << "\n";
  sh->run(pgm,args,term.data(),FALSE);
}

void TESession::kill(int ) // signal)
{
//  sh->kill(signal);
}

TESession::~TESession()
{
 QObject::disconnect( sh, SIGNAL( done(int) ),
		      this, SLOT( done(int) ) );
  delete em;
  delete sh;
}

void TESession::setConnect(bool c)
{
  em->setConnect(c);
}

void TESession::done(int status)
{
  emit done(te,status);
}

void TESession::terminate()
{
  delete this;
}

TEmulation* TESession::getEmulation()
{
  return em;
}

// following interfaces might be misplaced ///

int TESession::schemaNo()
{
  return schema_no;
}

int TESession::keymap()
{
  return keymap_no;
}

int TESession::fontNo()
{
  return font_no;
}

const char* TESession::emuName()
{
  return term.data();
}

void TESession::setSchemaNo(int sn)
{
  schema_no = sn;
}

void TESession::setKeymapNo(int kn)
{
  keymap_no = kn;
  em->setKeytrans(kn);
}

void TESession::setFontNo(int fn)
{
  font_no = fn;
}

void TESession::changeTitle(int, const QString& title)
{
  this->title = title;
  emit changeTitle(te, title);
}

const QString& TESession::Title()
{
  return title;
}

void TESession::setHistory(bool on)
{
  em->setHistory( on );
}

bool TESession::history()
{
  return em->history();
}

// #include "session.moc"
