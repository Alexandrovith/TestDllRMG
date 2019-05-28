#pragma once
#include "windows.h"
#include <string>
#include "json.h"

#define BAD_RESPONSES_CNT	6

using namespace std;

class Storage
{
public:
	Storage(void);
	~Storage(void);
	virtual bool set(string name, void* param)=0;		///< virtual method to write parameter to Storage
	virtual void* get(string name)=0;					///< virtual method to get parameter from Storage
	virtual int getValue(string name, UCHAR* buf)=0;		///< virtual method to return parameter from Storage as char array according to type
	virtual string getNewArchiveData(Json::Value& jsonAddress)=0;///< virtual method to return string of archive data
	bool getConnectionStatus();
	void setConnectionStatus(bool status);
private:
	bool _connectionStatus;
	long _connStatusCnt;
};
