#pragma once
#include <string>

static class CLastErr
{
	static LPSTR messageBuffer;
	//Free the buffer.
	static void FreeBuf ();

public:
	CLastErr ();
	~CLastErr ();

	static LPSTR AsChar (DWORD ulCodeErr);
	static LPSTR AsChar (DWORD ulCodeErr, size_t &uiSize);
	static std::string GetLastErrorAsString (DWORD ulCodeErr);
	// Преобразование кода ошибки (GetLastError) в описание
	static const char* Convert (DWORD ulCode);
};

