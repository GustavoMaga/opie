#ifndef __CDRAWBUFFER_H
#define __CDRAWBUFFER_H

#include "StyleConsts.h"
#include "CBuffer.h"
#include "my_list.h"
#include "linktype.h"

class QPainter;

struct textsegment
{
    int start;
    CStyle style;
    textsegment(int _start, const CStyle& _style)
	:
	start(_start), style(_style)
	{}
};

class FontControl;

class CDrawBuffer : public CBuffer
{
    CList<textsegment> segs;
    int len;
    FontControl* fc;
    int m_maxstyle, m_ascent, m_descent, m_lineSpacing, m_lineExtraSpacing;
    bool m_bEof;
    bool m_bSop, m_bEop;
    CDrawBuffer(const CDrawBuffer&);
    CDrawBuffer& operator=(const tchar*sztmp);
 public:
    void setstartpara() { m_bSop = true; }
    void setendpara() { m_bEop = true; }
    int leftMargin();
    int rightMargin();
    void setEof() { m_bEof = true; }
    bool eof() { return m_bEof; }
    CDrawBuffer& operator=(CDrawBuffer&);
    CDrawBuffer(FontControl* _fs = NULL)
	:
	fc(_fs)
	{
	    empty();
	}
    ~CDrawBuffer();
/*
    CDrawBuffer()
	:
	size(0)
	{
	    empty();
	}
*/
    int width(int numchars = -1, bool onscreen = false, int scwidth = 0, unsigned char _border = 0);
    int offset(int, unsigned char);
    void render(QPainter* _p, int _y, bool _bMono, int _charWidth, int scw, unsigned char);
    void empty();
    void addch(tchar ch, CStyle _style);
    void truncate(int);
    void setright(CDrawBuffer&, int);
    CStyle laststyle();
    int ascent() { return m_ascent; }
    int descent() { return m_descent; }
    int lineSpacing() { return m_lineSpacing; }
    int lineExtraSpacing() { return m_lineExtraSpacing; }

//    void frig();
    linkType getLinkType(int numchars, size_t& tgt);
    void resize();
};
#endif
