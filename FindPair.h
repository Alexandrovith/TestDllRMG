//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//~~~~~~~~~	Проект:			Базовая библиотека опроса приборов (driver)      
//~~~~~~~~~	Прибор:			Все                                     
//~~~~~~~~~	Модуль:			Вычитывание настроек для параметров из строки тегов	(as json)
//~~~~~~~~~	Разработка:	Демешкевич С.А.                                    		  
//~~~~~~~~~	Дата:				27.03.2019                                         		  

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#pragma once
//#ifdef GAZPRIBORDRV_EXPORTS
//#define MYDLL_API __declspec(dllexport)
//#else
//#define MYDLL_API __declspec(dllimport)
//#endif


#include <Windows.h>
#include <string>

using namespace std;

//=============================================================================

class CFindPair
{
public:
	CFindPair (string Tag);
	CFindPair () {}
	~CFindPair ();

	string operator	[](string asKey)
	{
		return Value (asKey);
	}
	int CurrPos () { return iCurrPos; }
	string Tag () { return asTag; }
	void InitTag (string Tag)
	{
		asTag = Tag;
		iCurrPos = 0;
	}

	string Value (string sKey);

private:
	string asTag;
	int iCurrPos = 0;

	int Find (string sKey, int iPosNameParam);
	int FindWord (string sKey, int iPosNameParam);
	int FindWord (string sKey);
	string ValueNext (string sKey);
	virtual string Value (string sStr, int iStartFind);

	bool IsLetterOrDigit (char cSym)
	{
		return (isascii (cSym) == 0)/* || (isalnum (cSym) == 0)*/;
	}


	//_________________________________________________________________________

};

