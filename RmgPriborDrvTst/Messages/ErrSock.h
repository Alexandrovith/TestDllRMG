#pragma once

#include <exception>

class CErrSock : public std::exception
{
public:
	CErrSock (const char* cpMess);
	~CErrSock ();
	static const char* WinSockErrCodeAsStr ();
};

class CErrMess : public std::exception
{
public:
	CErrMess () {}
	CErrMess (const char* cpMess);
	~CErrMess ();
};

