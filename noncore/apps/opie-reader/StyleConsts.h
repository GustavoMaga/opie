#ifndef __STYLECONSTS_H
#define __STYLECONSTS_H

typedef unsigned short StyleType;

#ifdef _WINDOWS
#include <string.h>
#endif
#include <stdlib.h>
#include <qglobal.h>
class QImage;

struct GraphicLink
{
    QImage* graphic;
    bool isLink;
    unsigned long link;
    GraphicLink(QImage* p, bool isLnk, unsigned long tgt) :
	graphic(p), isLink(isLnk), link(tgt) {}
    ~GraphicLink();
};

struct pmstore
{
    unsigned int count;
    bool m_isScaleable;
    GraphicLink*     graphic;
    pmstore(bool _canScale, QImage* p, bool isLnk, unsigned long tgt) : count(1), m_isScaleable(_canScale)
	{
	    graphic = new GraphicLink(p, isLnk, tgt);
	}
    ~pmstore();
};

enum EalignmentType
{
    m_AlignLeft,
    m_AlignRight,
    m_AlignCentre,
    m_AlignJustify
};

class CBasicStyle
{
    friend class CStyle;
    bool m_bold,
	m_italic;
    int m_fontsize;
    EalignmentType m_align;
    unsigned char red, green, blue;
    unsigned long data;
    bool isLink;
    bool m_underline;
    bool m_strikethru;
    bool m_monospaced;
    unsigned char m_leftmargin, m_rightmargin;
    signed char m_extraspace;
    signed char m_voffset;
    CBasicStyle()
	{
	    unset();
	}
    bool operator!=(const CBasicStyle& rhs)
	{
	    return (memcmp(this, &rhs, sizeof(CBasicStyle)) != 0);
	}
    void unset()
	{
	    m_bold = false;
	    m_italic = false;
	    m_fontsize = 0;
	    m_align = m_AlignLeft;
	    red = green = blue = 0;
	    data = 0;
	    isLink = false;
	    m_underline = false;
	    m_strikethru = false;
	    m_leftmargin = 0;
	    m_rightmargin = 0;
	    m_monospaced = false;
	    m_extraspace = 0;
	    m_voffset = 0;
	}
};

class CStyle
{
    CBasicStyle sty;
    pmstore* graphic;
 public:
    signed char getVOffset() { return sty.m_voffset; }
    void setVOffset(signed char sp) { sty.m_voffset = sp; }
    signed char getExtraSpace() { return sty.m_extraspace; }
    void setExtraSpace(signed char sp) { sty.m_extraspace = sp; }
    bool getPictureLink()
	{
	    return (graphic != NULL && graphic->graphic->isLink);
	}
    unsigned long getPictureLinkData()
	{
	    return graphic->graphic->link;
	}
    void setLeftMargin(unsigned char m) { sty.m_leftmargin = m; }
    unsigned char getLeftMargin() { return sty.m_leftmargin; }
    void setRightMargin(unsigned char m) { sty.m_rightmargin = m; }
    unsigned char getRightMargin() { return sty.m_rightmargin; }
    unsigned char Red() { return sty.red; }
    unsigned char Green() { return sty.green; }
    unsigned char Blue() { return sty.blue; }
    void setColour(unsigned char r, unsigned char g, unsigned char b)
	{
	    sty.red = r;
	    sty.green = g;
	    sty.blue = b;
	}
    CStyle() : graphic(NULL) {}
    ~CStyle();
//    CStyle(CStyle&);
    CStyle(const CStyle&);
    CStyle& operator=(const CStyle&);
    void unset();
    bool isPicture() { return (graphic != NULL); }
    bool canScale() { return graphic->m_isScaleable; }
    void clearPicture();
    void setPicture(bool canScale, QImage* _g, bool il=false, unsigned long tgt=0);
    QImage* getPicture()
	{ 
	    QImage* pm = ((graphic != NULL) ? graphic->graphic->graphic : NULL);
	    return pm;
	}
    void setUnderline() { sty.m_underline = true; }
    void unsetUnderline() { sty.m_underline = false; }
    bool isUnderline() { return sty.m_underline; }
    void setStrikethru() { sty.m_strikethru = true; }
    void unsetStrikethru() { sty.m_strikethru = false; }
    bool isStrikethru() { return sty.m_strikethru; }
    void setBold() { sty.m_bold = true; }
    void unsetBold() { sty.m_bold = false; }
    bool isBold() { return sty.m_bold; }
    void setItalic() { sty.m_italic = true; }
    void unsetItalic() { sty.m_italic = false; }
    bool isItalic() { return sty.m_italic; }
    void setMono() { sty.m_monospaced = true; }
    void unsetMono() { sty.m_monospaced = false; }
    bool isMono() { return sty.m_monospaced; }

    void setLeftJustify()
	{
	    sty.m_align = m_AlignLeft;
	}
    void setRightJustify()
	{
	    sty.m_align = m_AlignRight;
	}
    void setCentreJustify()
	{
	    sty.m_align = m_AlignCentre;
	}
    void setFullJustify()
	{
	    sty.m_align = m_AlignJustify;
	}
    StyleType getJustify()
	{
	    return sty.m_align;
	}

    void setFontSize(int _fs)
	{
	    sty.m_fontsize = _fs;
	}
    int getFontSize() const
	{
	    return sty.m_fontsize;
	}
    bool operator!=(const CStyle& rhs)
	{
	    return
		(
		    (sty != rhs.sty) ||
		    (graphic != rhs.graphic)
		    );
	}
    void setLink(bool _l) { sty.isLink = _l; }
    bool getLink() { return sty.isLink; }
    void setData(unsigned long _d) { sty.data = _d; }
    unsigned long getData() { return sty.data; }
};

#endif
