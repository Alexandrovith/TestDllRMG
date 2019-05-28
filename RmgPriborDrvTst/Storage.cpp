#include "stdafx.h"
#include "Storage.h"


Storage::Storage(void):
	_connStatusCnt(0)
{
}


Storage::~Storage(void)
{
}

bool Storage::getConnectionStatus()
{
	return (_connStatusCnt < BAD_RESPONSES_CNT);
	//return _connectionStatus;
}

void Storage::setConnectionStatus(bool status)
{
	_connectionStatus = status;
	if (!status) 
		_connStatusCnt++; 
	else
		_connStatusCnt = 0;
}