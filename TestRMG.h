#pragma once

#include "LoadDLL.h"
#include "WrConstParam.h"

enum class EModeWrite { PAR_CONST, };
void WrVal (EModeWrite ModeWrite);

shared_ptr <CWrConstParam> WrConstParam;

std::string CRCtoASCII (USHORT crc);
void TestTCP ();
unsigned short CRC (const unsigned char* data, UINT length, USHORT poly, USHORT init = 0, bool isbinary = false);

volatile CLoadDLL::DInitConfig InitConfig;
volatile CLoadDLL::DSubscribe Subscribe;
volatile CLoadDLL::DWriteValue WriteValue;
volatile CLoadDLL::DClose Close;
volatile CLoadDLL::DStopStartDev StopStartDev;
volatile CLoadDLL::DGetValue GetValue;
