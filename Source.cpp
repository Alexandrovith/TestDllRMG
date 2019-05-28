#include <iostream>
#include <Windows.h>
#include "GazPriborDrv.h"
//#pragma comment (lib, "GazPriborDrv.dll")

int main ()
{
	try
	{
		HMODULE hLib;
		hLib = LoadLibrary ("GazPriborDrv.dll");
		char* (*pFunction)();
		(FARPROC &)pFunction = GetProcAddress (hLib, "GetName");
		char caBuf[128] = { '1','2','3','4','5' };
		strcpy_s (caBuf, 127, pFunction ());
		FreeLibrary (hLib);
		caBuf[127] = 0;
		std::cout << caBuf << std::endl;
		std::getchar ();
	}
	catch (const std::exception& exc)
	{
		std::cout << exc.what () << std::endl;
	}
	return 0;
}