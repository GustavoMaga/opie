#include <stdio.h>
#include <string.h>
#include "Aportis.h"
#include "my_list.h"
#include "Bkmks.h"

Aportis::Aportis() : peanutfile(false) { /*printf("constructing:%x\n",fin);*/ }

void Aportis::dePeanut(int& ch)
{
    if (peanutfile && ch != EOF)
    {
	unsigned char c = ch;
	if (peanutfile) c ^= 0xa5;
	ch = c;
    }
}

CList<Bkmk>* Aportis::getbkmklist()
{
/*
    if (peanutfile)
    {
	if (nRecs2 > nRecs)
	{
	    CList<Bkmk>* t = new CList<Bkmk>;
	    for (int i = nRecs; i < nRecs2; i++)
	    {
		char name[17];
		name[16] = '\0';
//		qDebug("Record:%d, Length:%u",i,recordlength(i));
		gotorecordnumber(i);
		fread(name,1,16,fin);
		unsigned long lcn;
		fread(&lcn,sizeof(lcn),1,fin);
		lcn ^= 0xa5a5a5a5;
		lcn = SwapLong(lcn);
//		qDebug("Bookmark:%s:%u", name,lcn);
		tchar tname[17];
		memset(tname, 0, sizeof(tname));
		for (int i = 0; name[i] != 0; i++)
		{
		    tname[i] = name[i] ^ 0xa5;
		}
		t->push_back(Bkmk(tname, NULL, lcn));
	    }
	    return t;
	}
	else
	{
	    return NULL;
	}
    }
*/
  if (bCompressed != 4) return NULL;
  CList<Bkmk>* t = new CList<Bkmk>;
  size_t cur = ftell(fin);
  for (int i = 0; i < nRecs2; i++)
    {
      DWORD dwPos;
      fseek(fin, 0x56 + 8*i, SEEK_SET);
      fread(&dwPos, 4, 1, fin);
      dwPos = SwapLong(dwPos);
      fseek(fin,dwPos,SEEK_SET);
      unsigned char ch;
      fread(&ch,1,1,fin);
      if (ch != 241)
      {
	char name[17];
	name[16] = '\0';
	fseek(fin,dwPos,SEEK_SET);
	fread(name,1,16,fin);
	unsigned long lcn;
	fread(&lcn,sizeof(lcn),1,fin);
	lcn = SwapLong(lcn);
#ifdef _UNICODE
	tchar tname[17];
	memset(tname, 0, sizeof(tname));
	for (int i = 0; name[i] != 0; i++)
	{
	    tname[i] = name[i];
	}
	t->push_back(Bkmk(tname, NULL, lcn));
#else
	t->push_back(Bkmk(name,lcn));
#endif
      }
    }
  fseek(fin, cur, SEEK_SET);
  return t;
}

int Aportis::OpenFile(const char *src)
{
  //  printf("In openfile\n");
  int ret = 0;

  if (!Cpdb::openfile(src)) return -1;

  if (head.creator != 0x64414552 //   'dAER'
      || head.type != 0x74584554) // 'tXET')
    {

	if (memcmp(&head.creator, "PPrs", 4) == 0 && memcmp(&head.type, "PNRd", 4) == 0)
	{
	    peanutfile = true;
	}
	else
	{
	    return -2;
	}
    }

  nRecs2 = nRecs = SwapWord(head.recordList.numRecords) - 1;
  fseek(fin,0,SEEK_END);
  dwLen = ftell(fin);

  if (peanutfile)
  {
      
      PeanutHeader hdr0;
      gotorecordnumber(0);
      fread(&hdr0, sizeof(hdr0), 1, fin);
//      qDebug("Version:%x", ntohs(hdr0.Version));
      if (hdr0.Version && 0x0200)
      {
	  bCompressed = 2;
      }
      else
      {
	  bCompressed = 1;
      }
      BlockSize = 4096;
      nRecs = SwapWord(hdr0.Records)-1;
      dwTLen = nRecs*BlockSize;
  }
  else
  {
      gotorecordnumber(0);
      tDocRecord0 hdr0;
      fread(&hdr0, sizeof(hdr0), 1, fin);
      bCompressed = SwapWord(hdr0.wVersion);
      if (bCompressed!=1 && bCompressed!=2 && bCompressed != 4) {
	  ret = bCompressed;
	  bCompressed = 2;
      }
      switch (bCompressed)
      {
	  case 4:
	  {
	      dwTLen = 0;
	      int i;
	      for (i = 0; i < nRecs; i++)
	      {
		  unsigned int bs = GetBS(i);
		  if (bs == 0) break;
		  else dwTLen += bs;
	      }
	      nRecs = i;
	      BlockSize = 0;
	  }
	  break;
	  case 1:
	  case 2:
	  default:
	      nRecs = SwapWord(hdr0.wNumRecs);
	      dwTLen = SwapLong(hdr0.dwStoryLen);
	      BlockSize = SwapWord(hdr0.wRecSize);
	      if (BlockSize == 0)
	      {
		  BlockSize = 4096;
		  printf("WARNING: Blocksize not set in source file\n");
	      }
      }
  }



  // this is the main record buffer
  // it knows how to stretch to accomodate the decompress
  currentrec = 0;
  cbptr = 0;
  outptr = 0;
  refreshbuffer();
//  qDebug("Number of records:[%u,%u]", nRecs, nRecs2);
  return ret;
}

int Aportis::getch()
{
  if (bCompressed == 1)
    {
      if ((dwRecLen == 0) && !refreshbuffer()) return EOF;
      else
	{
	  int c = getc(fin);
	  dePeanut(c);
	  dwRecLen--;
	  currentpos++;
	  return c;
	}
    }
  if (outptr != cbptr)
    {
      currentpos++;
      return (circbuf[outptr = (outptr + 1) % 2048]);
    }
  if ((dwRecLen == 0) && !refreshbuffer()) return EOF;
  currentpos++;
  int c;

  // take a char from the input buffer
  c = getc(fin);
  dePeanut(c);
  dwRecLen--;
  // separate the char into zones: 0, 1...8, 9...0x7F, 0x80...0xBF, 0xC0...0xFF

  // codes 1...8 mean copy that many chars; for accented chars & binary
  if (c == 0)
    {
      circbuf[outptr = cbptr = (cbptr+1)%2048] = c;
      return c;
    }
  else if (c >= 0x09 && c <= 0x7F)
    {
      circbuf[outptr = cbptr = (cbptr+1)%2048] = c;
      return c;
    }
  else if (c >= 0x01 && c <= 0x08)
    {
      dwRecLen -= c;
      while(c--)
	{
	    int c = getc(fin);
	    dePeanut(c);
	    circbuf[cbptr = (cbptr+1)%2048] = c;
	}
      return circbuf[outptr = (outptr+1)%2048];
    }
  else if (c >= 0x80 && c <= 0xBF)
    {
      int m,n;
      c <<= 8;
      int c1 = getc(fin);
      dePeanut(c1);
      c += c1;
      dwRecLen--;
      m = (c & 0x3FFF) >> COUNT_BITS;
      n = c & ((1<<COUNT_BITS) - 1);
      n += 3;
      while (n--)
	{
	  cbptr = (cbptr+1)%2048;
	  circbuf[cbptr] = circbuf[(cbptr+2048-m)%2048];
	}
      return circbuf[outptr = (outptr+1)%2048];
    }
  else if (c >= 0xC0 && c <= 0xFF)
    {
      circbuf[cbptr = (cbptr+1)%2048] = ' ';
      circbuf[cbptr = (cbptr+1)%2048] = c^0x80;
      return circbuf[outptr = (outptr+1)%2048];
    }
}

unsigned int Aportis::GetBS(unsigned int bn)
{
  DWORD dwPos;
  WORD fs;

  fseek(fin, 0x56 + 8*bn, SEEK_SET);
  fread(&dwPos, 4, 1, fin);
  dwPos = SwapLong(dwPos);
  fseek(fin,dwPos,SEEK_SET);

//  gotorecordnumber(bn+1);
  unsigned char ch;
  fread(&ch,1,1,fin);
  if (ch == 241)
    {
      fread(&fs,sizeof(fs),1,fin);
      fs = SwapWord(fs);
    }
  else
    fs = 0;
  return fs;
}

unsigned int Aportis::locate()
{
  if (bCompressed == 4)
    {
      size_t cur = ftell(fin);
      unsigned int clen = 0;
      for (unsigned int i = 0; i < currentrec-1; i++)
	{
	  unsigned int bs = GetBS(i);
	  if (bs == 0) break;
	  clen += bs;
	}
      fseek(fin,cur,SEEK_SET);
      return clen+currentpos;
    }
  else
    return (currentrec-1)*BlockSize+currentpos;
}

void Aportis::locate(unsigned int n)
{
  unsigned int offset;
  //  currentrec = (n >> OFFBITS);
  switch (bCompressed)
    {
    case 4:
      {
	DWORD clen = 0;
	offset = n;
	unsigned int i;
	for (i = 0; i < nRecs; i++)
	  {
	    unsigned int bs = GetBS(i);
	    if (bs == 0) break;
	    clen += bs;
	    if (clen > n) break;
	    offset = n - clen;
	  }
	currentrec = i;
      }
      break;
    case 1:
    case 2:
    default:
      currentrec = n / BlockSize;
      offset = n % BlockSize;
    }

  outptr = cbptr;
  refreshbuffer();
  while (currentpos < offset && getch() != EOF);
}

bool Aportis::refreshbuffer()
{
  if (currentrec < nRecs)
    {
	dwRecLen = recordlength(currentrec+1);
	gotorecordnumber(currentrec+1);
      if (bCompressed == 4)
	{
	  unsigned char t[3];
	  fread(t,1,3,fin);
	  if (t[0] != 241)
	    {
	      printf("You shouldn't be here!\n");
	      return false;
	    }
	  dwRecLen -= 3;
	}
      /*
	int n = fread(t.buf, 1, dwRecLen, fin);
	t.len = n;
	//		if(bCompressed)
	t.Decompress();

	t.buf[t.Len()] = '\0';
      */
      currentpos = 0;
      currentrec++;
      return true;
    }
  else {
    return false;
  }
}
