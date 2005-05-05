#ifndef __PLUCKER_BASE_H
#define __PLUCKER_BASE_H

#include "CExpander.h"
#include <zlib.h>
#include "ztxt.h"
#include "pdb.h"
#include "CBuffer.h"
#include "my_list.h"
#include "Navigation.h"
#include "hrule.h"

struct CPlucker_record0
{
    UInt16 uid;
    UInt16 version;
    UInt16 nRecords;
};

struct CPluckerbkmk
{
    UInt32 offset;
    tchar title[MAX_BMRK_LENGTH];
};

#ifdef LOCALPICTURES
class QScrollView;
class QWidget;
#endif
class QString;

class CPlucker_base : public Cpdb
{
protected:
  static const UInt8 continuation_bit;
    virtual void setbuffersize() = 0;
    virtual void GetHeader(UInt16&, UInt16&, UInt32&, UInt8&, UInt8&) = 0;
    virtual int HeaderSize() = 0;
    unsigned short finduid(unsigned short);
    char* geturl(UInt16);
    void Expand(UInt32, UInt8, UInt8*, UInt32);
    CList<unsigned long> visited;
    bool m_lastIsBreak;
#ifdef LOCALPICTURES
    QScrollView* m_viewer;
    QWidget* m_picture;
#endif
    size_t textlength, m_lastBreak, m_offset;
    UInt16 uid;
    int m_nextPara, m_nextParaIndex;
    CBufferFace<UInt16> m_ParaOffsets;
    CBufferFace<UInt16> m_ParaAttrs;
    UInt16 m_nParas;
    CStyle mystyle;
//    bool bInit;
    UInt32 buffersize;
    UInt32 compressedbuffersize;
    UInt32 buffercontent;
    UInt8* expandedtextbuffer;
    UInt8* compressedtextbuffer;
//    char* urls;
//    size_t urlsize;
    size_t bufferpos;
    int bufferrec;
    CPlucker_record0 hdr0;
    bool m_bufferisreserved;
    size_t currentpos;
    bool expand(int);
    //static void UnZip(UInt8*, size_t, UInt8*, size_t);
    static size_t UnDoc(UInt8*, size_t, UInt8*, size_t);

    size_t (*m_decompress)(UInt8*, size_t, UInt8*, size_t);

#ifdef LOCALPICTURES
    void showimg(UInt16 tgt);
#endif
    QImage* getimg(UInt16 tgt);
    QImage* expandimg(UInt16 tgt, bool border=false);
    void home();
    virtual int bgetch() = 0;
    CNavigation m_nav;
 public:
    QString about();
    QImage* getPicture(unsigned long tgt);
    void sizes(unsigned long& _file, unsigned long& _text);
    bool hasrandomaccess() { return true; }
    virtual ~CPlucker_base();
    CPlucker_base();
    int OpenFile(const char *src);
    virtual tchar getch(bool) = 0;
    tchar getch_base(bool);
    int getch();
    void getch(tchar&, CStyle&, unsigned long& pos);
    unsigned int locate();
    void locate(unsigned int n);
    CList<Bkmk>* getbkmklist();
    linkType hyperlink(unsigned int, unsigned int, QString&, QString&);
    MarkupType PreferredMarkup()
	{
	    return cNONE;
	}
    void saveposn(size_t posn) { m_nav.saveposn(posn); }
    void writeposn(size_t posn) { m_nav.writeposn(posn); }
    linkType forward(size_t& loc) { return (m_nav.forward(loc)) ? eLink : eNone; }
    linkType back(size_t& loc) { return (m_nav.back(loc)) ? eLink : eNone; }
    bool hasnavigation() { return true; }
    void setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen);
    void putSaveData(unsigned char*& src, unsigned short& srclen);

    virtual bool CorrectDecoder() = 0;
//    virtual void setlink(QString&, const QString&) = 0;
    virtual QImage* imagefromdata(UInt8*, UInt32) = 0;
    QString getTableAsHtml(unsigned long loc);
};

#endif
