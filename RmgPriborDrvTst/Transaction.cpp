#include "stdafx.h"
#include "Transaction.h"
#include <iostream>
#include <limits>

#include "Common.h"
#include "RmgPriborDrv.h"

using namespace std;

Transaction::Transaction (void* dev, string requestName) :
	cpBufTX (0),
	cpBufRX (0),
	iLenBufRX (0),
	iLenBufTX (0),
	_oneShot (false),
	_alignMinute (-1)
{
	_device = dev;
	_transactionName = requestName;
	time_t curTime;
	time (&curTime);
	setStartTime (curTime);
	setFireTime (curTime);
}
///____________________________________________________________________________

Transaction::~Transaction (void)
{
	if (cpBufRX != 0)
	{
		delete []cpBufRX;
		cpBufRX = 0;
	}
	if (cpBufTX != 0)
	{
		delete []cpBufTX;
		cpBufTX = 0;
	}
}
///____________________________________________________________________________

time_t Transaction::setFireTime (time_t tRefTime)
{
	_fireTime = tRefTime;
	return _fireTime;
}
///____________________________________________________________________________

time_t Transaction::setStartTime (time_t tRefTime)
{
	_startTime = tRefTime;
	return _startTime;
}
///____________________________________________________________________________

time_t Transaction::resetFireTime (time_t tRefTime)
{
	if (_lReqPeriod == 0)
		return (_fireTime += 24 * 60 * 60);

	if (_alignMinute >= 0)
	{
		struct tm tmtime;
		localtime_s (&tmtime, &tRefTime);
		tmtime.tm_sec = 0;
		tmtime.tm_min = _alignMinute;
		tRefTime = mktime (&tmtime);
	}
	time_t newFire = _fireTime + _lReqPeriod;
	if (newFire < tRefTime)
	{
		long nperiod = (long)(tRefTime - _startTime) / _lReqPeriod;
		newFire = _startTime + (nperiod + 1) * _lReqPeriod;
	}
	_fireTime = newFire;
	return _fireTime;
}
///____________________________________________________________________________

bool Transaction::isFire (time_t tRefTime)
{
	return (tRefTime >= _fireTime);
}
///____________________________________________________________________________

void Transaction::setAddr (UCHAR* addr)
{
	_address[0] = addr[0];
	_address[1] = 0;
}
