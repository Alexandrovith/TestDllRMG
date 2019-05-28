#pragma once

// Обрезание длины выводимой команды 
class CLenOut
{
public:
#ifdef _DEBUG
	static const int SIZE_BUF = 1024;
#else
	static const int SIZE_BUF = 72;
#endif // _DEBUG
	
	CLenOut (char *cpBufOut, int &iSize, int Radix);
	~CLenOut ();
	void Init (char *cpBufOut, int &iSize, int Radix);

private:
	char* cpBufOut;
	bool bSizeIsTrunk = false;
};

