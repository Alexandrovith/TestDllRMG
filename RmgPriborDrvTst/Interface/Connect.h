///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	������:			Driver EC605
///~~~~~~~~~	������:			EC605
///~~~~~~~~~	������:			����������� ����� ����������������� ������ � ��������
///~~~~~~~~~	����������:	���������� �.�.
///~~~~~~~~~	����:				27.07.2018

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#pragma once

#include "stdafx.h"

class IConnect
{
	std::string asRet;

protected:
	//CMessages * Mess;
	//void* Own;
	bool bConnected;														// ��������� �����

public:
	IConnect (/*void* Owner, CMessages* Message*/);
	~IConnect ();
	virtual bool Connecting () = 0;							// ����������� / ��������� ����������	
	virtual bool Connect () = 0;								// �������� ����������
	virtual bool Disconnect () = 0;							// �������� ����������
	virtual bool Send (char *cpBufTX, int iSize) = 0;		// �������� ������ ����� ����
	virtual int Recieve (char* cpBufRX, int iMaxSizeRX, int& iCountBites, std::string asEnd = "da") = 0;
	bool IsConnected () { return bConnected; }					// �������� ������� ��������� ��������� �����
};

