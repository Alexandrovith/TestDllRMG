#include "WrRMG.h"

WrRMG::WrRMG (CLoadDLL::DWriteValue WriteValue, CLoadDLL::DGetValue GetValue)
{
	this->GetValue = GetValue;
	this->WriteValue = WriteValue;
}

int WrRMG::Write (const char* cpAddr, const char* cpVal, int iSize)
{
	WriteValue ((char*)cpAddr, (char*)cpVal, iSize);
	return 0;
}
