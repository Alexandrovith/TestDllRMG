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
#include "TCP.h"
#include "Messages.h"

#pragma comment(lib, "Ws2_32.lib")


CTCP::CTCP (/*void* Owner, */std::string IP, USHORT Port/*, CMessages *Message*/) : 
	IConnect(/*Owner, dynamic_cast<CMessages*> (Message)*/)
	,asIP(IP)
	,usPort(Port)
{
	Opening ();
}
///____________________________________________________________________________

bool CTCP::Opening ()
{
	if (WSAStartup (MAKEWORD (2, 2), &wsaData) != NO_ERROR)	// Initialize Winsock
		throw CErrSock ("Сбой применения Winsock DLL");
	Socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);		// Connect to server
	if (Socket == INVALID_SOCKET)
		throw CErrSock (CErrSock::WinSockErrCodeAsStr ());
}
///____________________________________________________________________________

CTCP::~CTCP ()
{
	ClosePort ();
}
///____________________________________________________________________________

bool CTCP::OpenPort ()
{
	sockaddr_in ServerAddr;
	//inet_pton (AF_INET, asIP.c_str (), htons (usPort));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr (asIP.c_str());
	ServerAddr.sin_port = htons (usPort);

	int iRet = connect (Socket, (sockaddr *)&ServerAddr, sizeof (ServerAddr));

	if (iRet == SOCKET_ERROR)
	{
		CloseErr ();
		return false;
	}
	return true;
}
///____________________________________________________________________________

bool CTCP::ClosePort ()
{
	return closesocket (Socket) == WSANOTINITIALISED;
}
///____________________________________________________________________________

void CTCP::OutMessErr (const std::string asPrefix)
{
	OutMess ((asPrefix + " " + CErrSock::WinSockErrCodeAsStr ()).c_str());
}
///____________________________________________________________________________

void CTCP::OutMess (const char* asMess)
{
	Mess->Save (asMess);
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
	asRet = "(" + asIP + ")" + "(" + (std::string)caPort + ")";
	return asRet;
}
///____________________________________________________________________________

bool CTCP::Send (char *cpBufTX, int iSize)
{
	int iRet = send (Socket, cpBufTX, iSize, 0);
	if (iRet)
		return true;
	OutMessErr (IP_Port ());
	return false;
}
///____________________________________________________________________________

int CTCP::Recieve (char* cpBufRX, int iMaxSizeRX, int& iCountBites, std::string asEnd)
{
	int iRet = recv (Socket, cpBufRX, LEN_MAX, 0);
	if (iRet > 0)
	{
		iCountBites = iRet;
		cpBufRX[iRet] = 0;
	}
	else if (iRet == 0)
	{
		iCountBites = iRet;
		OutMess ("Ответа нету");			// printf ("Connection closing...\n");
	}
	else
	{
		CloseErr ();						 // printf ("recv failed: %d\n", WSAGetLastError ());
		return 1;
	}
	return 0;
}
