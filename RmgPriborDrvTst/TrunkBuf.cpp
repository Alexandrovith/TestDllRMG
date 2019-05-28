#include "stdafx.h"
#include "TrunkBuf.h"


CLenOut::CLenOut (char *BufOut, int &iSize, int Radix)
{
	Init (BufOut, iSize, Radix);
}

void CLenOut::Init (char *BufOut, int &iSize, int Radix)
{
	cpBufOut = BufOut;
	memset (cpBufOut, 0, SIZE_BUF);
	int iMaxQuantSym = Radix == 10 ? 4 : 3;
	if (iSize >= SIZE_BUF / iMaxQuantSym)
	{
		iSize = SIZE_BUF / iMaxQuantSym - 4;
		bSizeIsTrunk = true;
	}
}

CLenOut::~CLenOut ()
{
	if (bSizeIsTrunk)
	{
		strcat_s (cpBufOut, SIZE_BUF, "...");
	}
}
