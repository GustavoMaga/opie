#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

extern int debugLevel;

#define HACK


#define pvDebug(I, S) \
if ( debugLevel < 3 ) \
{ \
	if ( I <= debugLevel ) qDebug(S);\
}else{\
	if ( I <= debugLevel ) \
		printf("# %s \t\t(Level: %i)\n",QString(S).latin1(),I);\
}

#endif