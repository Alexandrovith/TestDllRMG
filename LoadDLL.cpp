///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	Проект:			Тест драйверов на С++
///~~~~~~~~~	Прибор:			Все приборы
///~~~~~~~~~	Модуль:			Подключение dll
///~~~~~~~~~	Разработка:	Демешкевич С.А.
///~~~~~~~~~	Дата:				xx.xx.2019

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "LoadDLL.h"



CLoadDLL::CLoadDLL (LPCSTR lpLibFileName)
{
	hDLL = LoadLibrary (lpLibFileName);
}
//_____________________________________________________________________________

CLoadDLL::~CLoadDLL ()
{
	Close ();
}
//_____________________________________________________________________________

void* CLoadDLL::Load (LPCSTR FuncName)
{              // Handle to DLL  
	void* lpfnDllFunc = nullptr;    // Function pointer  
	if (hDLL != nullptr)
	{
		lpfnDllFunc = GetProcAddress (hDLL, FuncName);
		if (!lpfnDllFunc)
		{
			Close ();
			return nullptr;
		}
	}
	return lpfnDllFunc;
}
//_____________________________________________________________________________

void CLoadDLL::Close ()
{
	if (hDLL != nullptr)
	{
		FreeLibrary (hDLL);
		hDLL = nullptr;
	}
}
