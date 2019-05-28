#pragma once


using namespace std;


	//char* GetTimeNow ();

class CLog
{
	std::string asCurrDir;

public:
	CLog (HMODULE hModule);
	~CLog ();
	std::string CharArrToStrAsInt (void *caBuf, int iSize, int Radix = 16);
	void* CharArrToCharAsInt (void *vBuf, int iSize, int Radix = 16);
	void* CharArrToCharAsInt (std::string asBuf, int iSize, int Radix);
	void LogWrLine (string asMess);
};

CLog *Log;

# define LogWriteLine(fname, fmt, ...) Log->LogWrLine(Format(fmt, __VA_ARGS__))