///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	������:			Driver EC605
///~~~~~~~~~	������:			EC605
///~~~~~~~~~	������:			����� � �������� �� TCP/IP
///~~~~~~~~~	����������:	���������� �.�.
///~~~~~~~~~	����:				26.07.2018

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#pragma once

#include <winsock2.h>
#include "Connect.h"

class CTCP : public IConnect
{
public:
	static const int LEN_MAX = 1024;				//512 ��� ������ � ������ �������

private:
	std::string asIP;
	USHORT usPort;
	SOCKET Socket;  // ���������� ����� � ����� ��� ��������
	std::string asRet;

	//CMessages* Mess;
	//bool bConnected;

public:
	CTCP (/*void* Owner, */std::string IP, USHORT Port/*, CMessages *Message*/);
	~CTCP ();
	virtual bool Connecting ();
	virtual bool Connect ();
	virtual bool Disconnect ();
private:
	void OutMessErr (const std::string cpPrefix);
	void OutMess (const char* cpMess);
public:

	// ��������� �������� ����������
	void CloseErr ();
	std::string IP_Port ();
	virtual bool Send (char *cpBufTX, int iSize);
	virtual int Recieve (char* cpBufRX, int iMaxSizeRX, int& iCountBites, std::string asEnd = "da");
private:
};

