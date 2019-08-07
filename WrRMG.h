#pragma once

#include <memory>
#include <string>
#include "LoadDLL.h"

class WrRMG
{
	CLoadDLL::DWriteValue WriteValue;
	CLoadDLL::DGetValue GetValue;

	WrRMG (CLoadDLL::DWriteValue WriteValue, CLoadDLL::DGetValue GetValue);
public:
	int Write (const char* cpAddr, const char* cpVal, int iSize);
};

