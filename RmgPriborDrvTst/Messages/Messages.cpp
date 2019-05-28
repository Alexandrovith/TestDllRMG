///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	Проект:			Driver EC605
///~~~~~~~~~	Прибор:			Все приборы
///~~~~~~~~~	Модуль:			Вывод диагностических сообщений (в лог или др. устройство)
///~~~~~~~~~	Разработка:	Демешкевич С.А.
///~~~~~~~~~	Дата:				26.07.2018

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "stdafx.h"
#include <Windows.h>
#include <string>
#include <fstream>

#include "Messages.h"
#include "ErrSock.h"

std::shared_ptr<CMess> Mess;

static const std::string asDirFileLog = "logs\\";

CMessages::CMessages (const std::string asPach_FileLog)
{
	asFileLog = asPach_FileLog;

	if (asPach_FileLog.length() < 1)
		throw CErrMess ((const char*)"Ошибка имени log-файла");
}
//_____________________________________________________________________________

CMessages::~CMessages ()
{
}
//_____________________________________________________________________________

void CMessages::SetFileNameLog (std::string FileNameLog)
{
	asFileLog = asFileLog.substr (0, asFileLog.find_last_of ("\\", 512, 2) + 1) + asDirFileLog + FileNameLog + ".log";
}
//_____________________________________________________________________________

char* CMessages::Format (const char* fmt, ...)
{
	static const int SIZE = 6000;
	static char caRet[SIZE];

	va_list ap;
	va_start (ap, fmt);
	int n = vsnprintf_s ((char*)caRet, SIZE, SIZE, fmt, ap);
	va_end (ap);
	if (n > -1 && n < SIZE)
	{
		return (char*)caRet;
	}
	return (char*)"NONE";
}
//_____________________________________________________________________________

std::string CMessages::CharArrToStrAsInt (char *caBuf, int iSize, int Radix)
{
	asRet = "";
	for (size_t i = 0; i < iSize; i++)
	{
		char Buf[32];
		asRet += _itoa ((unsigned char)(*caBuf), Buf, Radix);
		asRet += " ";
		caBuf++;
	}
	return asRet;
}
//_____________________________________________________________________________
// Если iSymbolsQuant == 0, то вывод до числа 0
UCHAR* CMessages::CharArrToCharAsInt (void *vBuf, int iSymbolsQuant, int Radix)
{	
	if (iSymbolsQuant <= 0)
	{
		cpBufOut[0] = 0;
		return (UCHAR*)cpBufOut;
	}
	std::unique_ptr<CTrunkBuf> BufOut (new CTrunkBuf (cpBufOut, iSymbolsQuant, Radix));

	char* caBuf = (char*)vBuf;
	if (iSymbolsQuant)
	{
		for (size_t i = 0; i < iSymbolsQuant; i++)
		{
			char Buf[16];
			//strcat_s (cpBufOut, CTrunkBuf::SIZE_BUF, _itoa ((unsigned char)(*caBuf), Buf, Radix));
			//strcat_s (cpBufOut, CTrunkBuf::SIZE_BUF, " ");
			strncat_s (cpBufOut, CTrunkBuf::SIZE_BUF, _itoa ((unsigned char)(*caBuf), Buf, Radix), 2);
			strncat_s (cpBufOut, CTrunkBuf::SIZE_BUF, " ", 1);
			caBuf++;
		}
		strncat_s (cpBufOut, CTrunkBuf::SIZE_BUF, "\0", 1);
	}
	else
	{
		int iDiap = 0;
		while (*caBuf)
		{
			char Buf[16];
			strncat_s (cpBufOut, CTrunkBuf::SIZE_BUF, _itoa ((unsigned char)(*caBuf), Buf, Radix), 2);
			strncat_s (cpBufOut, CTrunkBuf::SIZE_BUF, " ", 1);
			caBuf++;
			if (++iDiap == CTrunkBuf::SIZE_BUF)
				break;
		}
	}
	return (UCHAR*)cpBufOut;
}
//_____________________________________________________________________________

char* CMessages::CharArrToCharAsInt (std::string asBuf, int iSize, int Radix)
{
	cpBufOut[0] = 0;
	for (size_t i = 0; i < iSize; i++)
	{
		char Buf[16];
		strcat_s (cpBufOut, CTrunkBuf::SIZE_BUF, _itoa ((unsigned char)(asBuf[i]), Buf, Radix));
		strcat_s (cpBufOut, CTrunkBuf::SIZE_BUF, " ");
	}
	return cpBufOut;
}
//_____________________________________________________________________________

const char* GetTimeNow ()
{
	SYSTEMTIME st;
	GetLocalTime (&st);
	static char caBuf[48];
	sprintf_s (caBuf, "%02d%02d%02d %02d:%02d:%02d.%03d\t", st.wDay, st.wMonth,
						 st.wYear - 2000, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return caBuf;
}


//=============================================================================

CMess::CMess (const std::string cpPach_FileLog) :
	CMessages (cpPach_FileLog + "RMGstart.log")
{}
//_____________________________________________________________________________

CMess::~CMess ()
{}
//_____________________________________________________________________________

void CMess::Out (const char* cpMess, ...)
{
	static const int SIZE = 6000;
	char caRet[SIZE];

	va_list ap;
	va_start (ap, cpMess);
	int n = vsnprintf_s ((char*)caRet, SIZE, SIZE, cpMess, ap);
	va_end (ap);
	if (n > -1 && n < SIZE)
	{
		std::ofstream F (asFileLog, std::ios::app);
		F << GetTimeNow () << caRet << std::endl;
		F.close ();
	}
}
//_____________________________________________________________________________

void CMessages::SetNameDev (std::string asDevName)
{
	if (asNameDev.length () < 2)
	{
		asNameDev = asDevName;
		SetFileNameLog (asDevName);
	}
}
