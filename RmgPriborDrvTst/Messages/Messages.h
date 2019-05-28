///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	Проект:			Driver EC605
///~~~~~~~~~	Прибор:			Все приборы
///~~~~~~~~~	Модуль:			Вывод диагностических сообщений (в лог или др. устройство)
///~~~~~~~~~	Разработка:	Демешкевич С.А.
///~~~~~~~~~	Дата:				26.07.2018

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#pragma once

#include "TrunkBuf.h"

const char* GetTimeNow ();

class CMessages
{
protected:
	std::string asFileLog;
	std::string asNameDev;

	char cpBufOut[CTrunkBuf::SIZE_BUF];
	std::string asRet;

public:
	CMessages () {}
	CMessages (const std::string asPach_FileLog);
	~CMessages ();
	//virtual void Out (const std::string asMess) = 0;
	virtual void Out (const char* asMess, ...) = 0;
	char* Format (const char* fmt, ...);
	std::string CharArrToStrAsInt (char *caBuf, int iSize, int Radix = 16);
	UCHAR* CharArrToCharAsInt (void *vBuf, int iSymbolsQuant, int Radix = 16);
	char* CharArrToCharAsInt (std::string asBuf, int iSize, int Radix = 16);
	void SetFileNameLog (std::string FileNamelog);
	void SetNameDev (std::string asDevName);
};


class CMess :
	public CMessages
{

public:
	CMess () : CMessages("")
	{
	}
	CMess (const std::string asPach_FileLog);
	~CMess ();
	//virtual void Out (const std::string asMess) {};
	virtual void Out (const char* asMess, ...);

};

extern std::shared_ptr<CMess> Mess;
