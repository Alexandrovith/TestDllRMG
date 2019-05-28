///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	Проект:			Driver EC605
///~~~~~~~~~	Прибор:			EC605
///~~~~~~~~~	Модуль:			Обмен с прибором по TCP/IP
///~~~~~~~~~	Разработка:	Демешкевич С.А.
///~~~~~~~~~	Дата:				26.07.2018

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "ErrSock.h"
#include "Connect.h"
#include "TCP.h"
#include "Messages.h"

#pragma comment(lib, "Ws2_32.lib")

static const ADDRESS_FAMILY AddrFamily = AF_INET;


CTCP::CTCP (/*void* Owner, */std::string IP, USHORT Port/*, CMessages *Message*/) :
	IConnect (/*Owner, dynamic_cast<CMessages*> (Message)*/)
	, asIP (IP)
	, usPort (Port)
{
	Connecting ();
}
///____________________________________________________________________________

bool CTCP::Connecting ()
{
	WSADATA wsaData;
	if (WSAStartup (MAKEWORD (2, 2), &wsaData) != NO_ERROR)		// Initialize Winsock 
		throw CErrSock ("Сбой применения Winsock DLL");
	Socket = socket (AddrFamily, SOCK_STREAM, IPPROTO_TCP);		// Connecting to server
	if (Socket == INVALID_SOCKET)
		throw CErrSock (CErrSock::WinSockErrCodeAsStr ());
	return Connect ();
}
///____________________________________________________________________________

CTCP::~CTCP ()
{
	Disconnect ();
}
///____________________________________________________________________________

bool CTCP::Connect ()
{
	sockaddr_in ServerAddr;
	//inet_pton (AF_INET, asIP.c_str (), htons (usPort));
	ServerAddr.sin_family = AddrFamily;
	ServerAddr.sin_addr.s_addr = inet_addr (asIP.c_str ());
	ServerAddr.sin_port = htons (usPort);

	int iRet = connect (Socket, (sockaddr *)&ServerAddr, sizeof (ServerAddr));

	if (iRet == SOCKET_ERROR)
	{
		CloseErr ();
		bConnected = false;
		return false;
	}
	bConnected = true;
	return true;
}
///____________________________________________________________________________

bool CTCP::Disconnect ()
{
	bool bRet = closesocket (Socket) == WSANOTINITIALISED;
	bConnected = false;
	return bRet;
}
///____________________________________________________________________________

void CTCP::OutMessErr (const std::string asPrefix)
{
	OutMess ((asPrefix + " " + CErrSock::WinSockErrCodeAsStr ()).c_str ());
}
///____________________________________________________________________________

void CTCP::OutMess (const char* asMess)
{
	Mess->Out (asMess);
}
///____________________________________________________________________________
// Аварийное закрытие соединения
void CTCP::CloseErr ()
{
	// Префикс: IP + Port
	OutMessErr (IP_Port ());
	closesocket (Socket);
	WSACleanup ();
}
///____________________________________________________________________________

std::string CTCP::IP_Port ()
{
	char caPort[8];
	_itoa (usPort, caPort, 10);
	asRet = "(" + asIP + "-" + (std::string)caPort + ")";
	return asRet;
}
///____________________________________________________________________________

bool CTCP::Send (char *cpBufTX, int iSize)
{
	int iRet = send (Socket, cpBufTX, iSize, 0);
	//iResult = shutdown (Socket, SD_SEND);
	//if (iResult == SOCKET_ERROR)
	if (iRet)
		return true;
	OutMessErr (IP_Port ());
	return false;
}
///____________________________________________________________________________

int CTCP::Recieve (char* cpBufRX, int iMaxSizeRX, int& iCountBites, std::string asEnd)
{
	iCountBites = recv (Socket, cpBufRX, LEN_MAX, 0);
	if (iCountBites > 0)
	{
		cpBufRX[iCountBites] = 0;
	}
	else if (iCountBites == 0)
	{
		OutMess ("Ответа нету");			// printf ("Connection closing...\n");
	}
	else
	{
		iCountBites = 0;
		CloseErr ();						 // printf ("recv failed: %d\n", WSAGetLastError ());
		return 1;
	}
	return 0;
}
