/*
  Derived from makedoc9 by Pat Beirne
*/

#ifndef __Aportis_h
#define __Aportis_h
#include "CExpander.h"
#include "pdb.h"

#define COUNT_BITS 3

typedef UInt32 DWORD;
typedef UInt16 WORD;

//#define DISP_BITS 11
struct tDocRecord0 {
	WORD wVersion;	// 1=plain text, 2=compressed
	WORD wSpare;
	DWORD dwStoryLen;   // in chars, when decompressed
	WORD wNumRecs; 			// text records only; equals tDocHeader.wNumRecs-1
	WORD wRecSize;			// usually 0x1000
	DWORD dwSpare2;
};

struct PeanutHeader
{
    UInt16 Version;
    UInt8 Junk1[6];
    UInt16 Records;
    UInt8 Junk2[106];
};

////////////// utilities //////////////////////////////////////

inline WORD SwapWord(WORD r)
{
	return (r>>8) + (r<<8);
}

inline DWORD SwapLong(DWORD r)
{
	return  ((r>>24) & 0xFF) + (r<<24) + ((r>>8) & 0xFF00) + ((r<<8) & 0xFF0000);
}

class Aportis : public Cpdb {
  void dePeanut(int&);

  bool peanutfile, html;
  DWORD dwLen;
  WORD nRecs2;
  DWORD dwTLen;
  WORD nRecs;
  WORD BlockSize;
  DWORD dwRecLen;
  WORD mobiimagerec;
  int currentrec, currentpos;
  unsigned int cbptr;
  unsigned int outptr;
  unsigned char circbuf[2048];
  char bCompressed;
public:
  void sizes(unsigned long& _file, unsigned long& _text)
  {
    qDebug("Calling aportis sizes:(%u,%u,%u)", dwTLen, nRecs, BlockSize);
    _file = dwLen;
    _text = dwTLen;
  }
  bool hasrandomaccess() { return true; }
  virtual ~Aportis() {}
  Aportis();
  int OpenFile(const char *src);
  int getch();
  unsigned int locate();
  void locate(unsigned int n);
  CList<Bkmk>* getbkmklist();
  MarkupType PreferredMarkup()
      {
	  return (peanutfile) ? cPML : ((html) ? cHTML : cTEXT);
      }
  QImage* getPicture(unsigned long);
  QString about() { return QString("AportisDoc codec (c) Tim Wentford"); }
private:
  bool refreshbuffer();
  unsigned int GetBS(unsigned int bn);
};
#endif
