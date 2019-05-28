#pragma once

//#include "stdafx.h"
//#include <windows.h>
#include <iostream>
#include <fstream>

using namespace std;
#ifdef CHECKTIMES
#define CHECK_TIME { DWORD st1, st2; st1=::GetTickCount();
#define FINISHCHECK_TIME(name_c) st2 = ::GetTickCount(); if ((st2-st1)>1) {LOG("Checktime %s = %d\n", name_c, st2-st1);} }
#else
#define CHECK_TIME
#define FINISHCHECK_TIME(name_c) 
#endif
#define LOG_TO_FILE


#ifdef LOG_TO_FILE
//# define LOGF(fname, fmt, ...) FilesLogMessage(fname, Format(fmt, __VA_ARGS__))
# define LOG(fmt, ...) FileLogMessage(Format(fmt, __VA_ARGS__))

/// \brief This function is used to log all messages to log file
/// \param message to log
/*static void FileLogMessage(string message)
{
	string logsdir = binDir + "\\logs";
	_mkdir(logsdir.c_str());
	ofstream flog("DriverOutRMG.log", ios::app);
	flog << message.c_str() <<endl;
	flog.close();
# ifdef CONSOLE_OUT
	wcout << message.c_str() << endl;
# endif
}
*/
#else
# define LOGF(fname, fmt, ...) LogMessage(Format(fmt, __VA_ARGS__))
# define LOG(fmt, ...) LogMessage(Format(fmt, __VA_ARGS__))
#endif


#ifdef DEBUG_LOG
#define LOGD LOG
#else
#define LOGD
#endif

/// \brief This function is used to log all messages from 
/// \param message to log
static void LogMessage (string message)
{
	wcout << message.c_str () << endl;
}

static char* Format (const char* fmt, ...)
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

///// \brief This function creates formated message like fprintf 
///// \param fmt format string 
//static std::string string_format (const std::string fmt, ...)
//{
//	int size = 8000;// ((int)fmt.size ()) * 2 + 1000;   // use a rubric appropriate for your code
//	std::string str;
//	va_list ap;
//	while (1)
//	{     // maximum 2 passes on a POSIX system...
//		str.resize (size);
//		va_start (ap, fmt);
//		int n = vsnprintf_s ((char *)str.data (), size, size, fmt.c_str (), ap);
//		va_end (ap);
//		if (n > -1 && n < size)
//		{  // everything worked
//			str.resize (n);
//			return str;
//		}
//		if (n > -1)  // needed size returned
//			size = n + 1;   // for null char
//		else
//			size *= 2;      // guess at a larger size (o/s specific)
//	}
//
//	return str;
//}
//static std::string string_format (const std::string fmt, ...)
//{
//	const int SIZE = 6000;// ((int)fmt.size ()) * 2 + 1000;   // use a rubric appropriate for your code
//	static char caRet[SIZE];
//	va_list ap;
//	va_start (ap, fmt);
//	int n = vsnprintf_s (caRet, SIZE, SIZE, fmt.c_str (), ap);
//	va_end (ap);
//	if (n > -1 && n < SIZE)
//	{
//		return std::string (caRet);
//	}
//	return std::string ("test");
//}
//static std::string string_format (const std::string fmt, ...)
//{
//	const int SIZE = 6000;// ((int)fmt.size ()) * 2 + 1000;   // use a rubric appropriate for your code
//	char caRet[SIZE];
//	static string asRet;
//	asRet.reserve (SIZE);
//	va_list ap;
//	va_start (ap, fmt); 
//	int n = vsnprintf_s (caRet, SIZE, SIZE, fmt.c_str (), ap);
//	va_end (ap);
//	if (n > -1 && n < SIZE)
//	{
//		asRet = caRet;
//		return asRet;
//	}
//	return std::string ("NONE");
//}
//static std::string string_format (const std::string fmt, ...)
//{
//	const int SIZE = 6000;// ((int)fmt.size ()) * 2 + 1000;   // use a rubric appropriate for your code
//	static string asRet;
//	asRet.reserve (SIZE);
//	static const char *caRet = asRet.c_str();
//	va_list ap;
//	va_start (ap, fmt); 
//	int n = vsnprintf_s ((char*)caRet, SIZE, SIZE, fmt.c_str (), ap);
//	va_end (ap);
//	if (n > -1 && n < SIZE)
//	{
//		return caRet;
//	}
//	return std::string ("NONE");
//}