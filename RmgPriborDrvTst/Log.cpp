#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <fstream>

#include "Log.h"


CLog::CLog (HMODULE hModule)
{
	char moduleFileName[256];
	GetModuleFileNameA (hModule, moduleFileName, sizeof (moduleFileName));
	std::string strfile (moduleFileName);
	std::string::size_type pos = strfile.find_last_of ("\\/");
	asCurrDir = strfile.substr (0, pos) + "\\/LOG";
}


CLog::~CLog ()
{}

//_____________________________________________________________________________

std::string CLog::CharArrToStrAsInt (void *Buf, int iLength, int Radix)
{
	std::string asVal;
	char *caBuf = (char*)Buf;

	for (size_t i = 0; i < iLength; i++)
	{
		char Buf[32];
		asVal += itoa ((unsigned char)(*caBuf), Buf, Radix);
		asVal += " ";
		caBuf++;
	}
	return asVal;
}
//_____________________________________________________________________________

void* CLog::CLog::CharArrToCharAsInt (void *vBuf, int iLength, int Radix)
{
	const int SIZE_BUF = 512;
	static char cpBufOut[SIZE_BUF];
	cpBufOut[0] = 0;
	bool bSizeIsTrunk = false;
	int iMaxQuantSym = Radix == 10 ? 4 : 3;
	if (iLength >= SIZE_BUF / iMaxQuantSym)
	{
		iLength = SIZE_BUF / iMaxQuantSym - 4;
		bSizeIsTrunk = true;
	}

	char* caBuf = (char*)vBuf;
	for (size_t i = 0; i < iLength; i++)
	{
		char Buf[16];
		strcat_s (cpBufOut, SIZE_BUF, _itoa ((unsigned char)(*caBuf), Buf, Radix));
		strcat_s (cpBufOut, SIZE_BUF, " ");
		caBuf++;
	}
	if (bSizeIsTrunk)
	{
		strcat_s (cpBufOut, SIZE_BUF, "...");
	}
	return (void*)cpBufOut;
}
//_____________________________________________________________________________

void* CLog::CharArrToCharAsInt (std::string asBuf, int iSize, int Radix)
{
	const int SIZE_BUF = 512;
	static char cpBufOut[SIZE_BUF];
	cpBufOut[0] = 0;
	bool bSizeIsTrunk = false;
	int iMaxQuantSym = Radix == 10 ? 4 : 3;
	if (iSize >= SIZE_BUF / iMaxQuantSym)
	{
		iSize = SIZE_BUF / iMaxQuantSym - 4;
		bSizeIsTrunk = true;
	}

	for (size_t i = 0; i < iSize; i++)
	{
		char Buf[16];
		strcat_s (cpBufOut, SIZE_BUF, _itoa ((unsigned char)(asBuf[i]), Buf, Radix));
		strcat_s (cpBufOut, SIZE_BUF, " ");
	}
	if (bSizeIsTrunk)
	{
		strcat_s (cpBufOut, SIZE_BUF, "...");
	}
	return cpBufOut;
}
//_____________________________________________________________________________

//char* GetTimeNow ()
//{
//	SYSTEMTIME st;
//	const int SIZE_BUF = 48;
//	GetLocalTime (&st);
//	static char caBuf[SIZE_BUF];
//	sprintf_s (caBuf, "%02d%02d%02d %02d:%02d:%02d.%03d\t", st.wDay, st.wMonth,
//						 st.wYear - 2000, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
//	return caBuf;
//}

void CLog::LogWrLine (string asMess)
{
	
}
