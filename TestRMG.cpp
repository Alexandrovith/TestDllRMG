#include <Windows.h>
#include <iostream>
#include <locale>
#include <string>
#include <functional>
#include <msclr\auto_gcroot.h>

#include "LoadDLL.h"

#include "Init_Superflo.cpp"
#include "Subscribe_Superflo.cpp"
#include "ZipByCppLog.h"

using namespace std;
//#using "ZipByCppLog.dll"; 

#define RMG
//#define LOAD_ZIP
#define LOAD_DLL

LPCSTR cpDll =
#ifdef RMG
"GeneralR.dll";
#ifndef LOAD_DLL
#include "RmgPriborDrv.h"

#endif // !LOAD_DLL
#elif LOAD_SUPER
//#include "UsefullF.h"
"UsefullF.dll";
#else
"ZipByCppLog.dll";
//using namespace System::Runtime::InteropServices; // Marshal
#endif

#define COMM(sym1,sym2) ((sym1) + ((sym2) << 8))
string CRCtoASCII (USHORT crc);

//static char* Format (const char* fmt, ...)
//{
//	static const int SIZE = 6000;
//	static char caRet[SIZE + 1];
//
//	va_list ap;
//	va_start (ap, fmt);
//	int n = vsnprintf_s ((char*)caRet, SIZE, _TRUNCATE, fmt, ap);
//	va_end (ap);
//	if (n > -1/* && n < SIZE*/)
//	{
//		return (char*)caRet;
//	}
//	return (char*)"";
//}

void TestTCP ();
unsigned short CRC (const unsigned char* data, UINT length, USHORT poly, USHORT init = 0, bool isbinary = false);

union UCommand
{
	char caCommand[3];
	USHORT usComm;
	UCommand ()
	{
		caCommand[2] = 0;
	}
} Command;

int main ()
{
	setlocale (LC_ALL, "ru-ru"); 

#ifdef LOAD_DLL
	std::unique_ptr<CLoadDLL> LoadDLL (new CLoadDLL (cpDll));

#ifdef LOAD_ZIP
	volatile CLoadDLL::DZipByCppLog ZipByCppLog = (CLoadDLL::DZipByCppLog)LoadDLL->Load ("AXONIM.ZipByCppLog.Pack");
	if (ZipByCppLog == nullptr)
		return 1;
	ZipByCppLog ();
	
#else
	volatile CLoadDLL::DInitConfig InitConfig = (CLoadDLL::DInitConfig)LoadDLL->Load ("InitConfig");
	if (InitConfig == nullptr)
		return 1;
	volatile CLoadDLL::DSubscribe Subscribe = (CLoadDLL::DSubscribe)LoadDLL->Load ("Subscribe");
	if (Subscribe == nullptr)
		return 1;
	volatile CLoadDLL::DWriteValue WriteValue = (CLoadDLL::DWriteValue)LoadDLL->Load ("WriteValue");
	if (WriteValue == nullptr)
		return 1;
	volatile CLoadDLL::DClose Close = (CLoadDLL::DClose)LoadDLL->Load ("Close");
	if (Close == nullptr)
		return 1;
	volatile CLoadDLL::DStopStartDev StopStartDev = (CLoadDLL::DStopStartDev)LoadDLL->Load ("StopStartDev");
	if (StopStartDev == nullptr)
		return 1;
#endif
#endif

	//USHORT usComm = COMM ('W', '1');
	//Command.usComm = usComm;
	//int ival = 99;
	//UINT plength = 5;
	//std::string outvalue = Format ("%.2s(%.2s\t%.2s%0*d)", "TR_name", "Userid", "1234", plength, ival);

	USHORT us = 0x6ad3; //0x5c12;
	std::string as = CRCtoASCII (us);
	unsigned char F365[] = { 0xf3, 0x65 };
	char c1[] = { '/', '?', 'W', 'K', 'E', 'R', '!', 'm','w', '(', ')', 'x', 'p', 'a' }; // pa = 0x7061
	char ms[] = { 0x6D, 0x73, 0x28, 0x30, 0x42, 0x59, 0x35, 0x44, 0x30, 0x36, 0x36, 0x39, 0x31, 0x32, 0x35, 0x30, 0x09, 0x30, 0x30, 0x30, 0x29, 0x3F, 0x3B, 0x3D, 0x32, 0x21, 0x0D, 0x0A };
	volatile char se[] = { 0x73,0x65,0x14,0x39,0x18,0x28,0x2b,0x13,0x14,0x32,0x32,0x4b,0x45,0x32,0x35,0x18 };
	const char mw_x302_[] = { 0x6d, 0x77, 0x28, 0x78, 0x33, 0x30, 0x32, 0x29, 0x38, 0x36, 0x35, 0x34, 0x21, 0xd, 0xa };
	const char mw_k301_[] = { 0x6d, 0x77, 0x28, 0x6b, 0x33, 0x30, 0x31, 0x29, 0x30, 0x3b, 0x35, 0x3f, 0x21 };
	volatile char mwW1[] = { 0x6d, 0x77, 0x57, 0x31, 0x21 };
	char cW[] = { 0x2F,0x3F,0x30,0x30,0x30,0x52,0x32,0x32,0x32,0x32,0x32,0x32,0x21,0x0D,0x0A };

	//USHORT cmd = COMM ('W', '1');// ('R', '6');	// 'R' + ('6' << 8); copy /y $(TargetPath) $(SolutionDir)FlexVisServer\bin\Debug
	//const char cv[] = "mv(k200)195?!";
	//char c4b = 0x4b;
	//char cB2 = 0xB2;

	std::cout << "InitConfig..." << std::endl;//ЕС605_Ясень

#ifdef RMG
										//"{\"Devices\":[{\"Name\":\"EC605_test\",\"Type\":\"\",\"Password\":\"22222\",\"WritePassword\":\"33333\",\"UserId\":\"1\",\"Address\":1,\"BaudRate\":9600,\"Port\":\"520\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"FlowCtrl\":0,\"SetRTS\":0,\"UrlSingleParam\":\"192.168.1.23\",\"TD\":\"20022019\",\"TH\":\"20022019\",\"TI\":\"\",\"TA\":\"\",\"Transport\":\"Ethernet\"}]}"
	char* cpInitDrv = "{\"Devices\":[{\"Name\":\"EC605_test\",\"Type\":\"\",\"Password\":\"22222\",\"WritePassword\":\"33333\",\"UserId\":\"01\",\"UrlSingleParam\":\"192.168.1.23\",\"Address\":1,\"BaudRate\":9600,\"Port\":\"520\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"FlowCtrl\":0,\"SetRTS\":0,\"TD\":\"23112017\",\"TH\":\"23112017\",\"TI\":\"01112017\",\"TA\":\"14042017\",\"Transport\":\"Ethernet\"}]}";
	
	//192.168.127.254  192.168.1.123          FlexVisServer\bin
	InitConfig (cpInitDrv);	//"{\"Devices\":[{\"Name\":\"ЕС605_Ясень\",\"Type\":\"\",\"Password\":\"22222\",\"WritePassword\":\"33333\",\"UserId\":\"00\",\"Address\":0,\"BaudRate\":9600,\"Port\":\"COM10\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"FlowCtrl\":0,\"SetRTS\":0,\"TD\":\"10052018\",\"TH\":\"10052018\",\"TI\":\"02042018\",\"TA\":\"11042018\",\"Transport\":\"\"}]}"

	for (;;)
	{
		std::cout << "Subscribe..." << std::endl;

		char* caTags[] = {
			"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"R2\",\"ParamName\":\"Tpr\"}",
			"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"R6\",\"ArchiveName\":\"as\"}",
			"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"H2\",\"ArchiveName\":\"H2\"}",
			"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"R2\",\"ParamName\":\"verze\"}",
			"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"R2\",\"ParamName\":\"CZ\"}",
			"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"R2\",\"ParamName\":\"Pvc\"}",
			"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"R2\",\"ParamName\":\"Dsm\"}",
			"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"R2\",\"ParamName\":\"Adr\"}" };
#elif LOAD_SUPER
	InitConfig (caInit ());
	//InitConfig ("{\"Devices\":[{\"Name\":\"SF_Жорновка\",\"Type\":\"SF21RU5D\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM14\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"29102018\",\"TH\":\"26102018\",\"TI\":\"23102018\",\"TA\":\"06102018\",\"FlowCtrl\":0},{\"Name\":\"SF_Осиповичи\",\"Type\":\"SF21RU7C\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM11\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"01012000\",\"TH\":\"26102018\",\"TI\":\"09122016\",\"TA\":\"13052018\",\"FlowCtrl\":0},{\"Name\":\"SF_Елизово\",\"Type\":\"SF21RU7C\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM24\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"01012000\",\"TH\":\"26102018\",\"TI\":\"12122016\",\"TA\":\"01012000\",\"FlowCtrl\":0},{\"Name\":\"SF_Светлогорск\",\"Type\":\"SF21RU7C\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM13\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"29102018\",\"TH\":\"26102018\",\"TI\":\"25102018\",\"TA\":\"24102018\",\"FlowCtrl\":0},{\"Name\":\"SF_МГорка\",\"Type\":\"SF21RU7C\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM19\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"01012000\",\"TH\":\"26102018\",\"TI\":\"19042018\",\"TA\":\"15072018\",\"FlowCtrl\":0},{\"Name\":\"SF_Червень\",\"Type\":\"SF21RU7C\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM21\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"23012001\",\"TH\":\"25102018\",\"TI\":\"01012000\",\"TA\":\"12102018\",\"FlowCtrl\":0},{\"Name\":\"SF_СтДороги\",\"Type\":\"SF21RU6D\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM16\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"29102018\",\"TH\":\"26102018\",\"TI\":\"24102018\",\"TA\":\"16102018\",\"FlowCtrl\":0},{\"Name\":\"SF_Глуск\",\"Type\":\"SF21RU6D\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM17\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"29102018\",\"TH\":\"26102018\",\"TI\":\"24102018\",\"TA\":\"16102018\",\"FlowCtrl\":0},{\"Name\":\"SF_НОстров\",\"Type\":\"SF21RU7C\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM25\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"01012000\",\"TH\":\"26102018\",\"TI\":\"12062017\",\"TA\":\"14072018\",\"FlowCtrl\":0},{\"Name\":\"SF_Рассвет\",\"Type\":\"SF21RU7C\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM18\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"24012001\",\"TH\":\"26102018\",\"TI\":\"01012000\",\"TA\":\"01012000\",\"FlowCtrl\":0},{\"Name\":\"SF_Октябрьский\",\"Type\":\"SF21RU6D\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM15\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"29102018\",\"TH\":\"26102018\",\"TI\":\"25102018\",\"TA\":\"17102018\",\"FlowCtrl\":0},{\"Name\":\"SF1_Бобруйск\",\"Type\":\"SF21RU7C\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM30\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"15042004\",\"TH\":\"26102018\",\"TI\":\"20092017\",\"TA\":\"10102017\",\"FlowCtrl\":0},{\"Name\":\"SF2_Бобруйск\",\"Type\":\"SF21RU7C\",\"Password\":\"123\",\"Address\":2,\"BaudRate\":1200,\"Port\":\"COM31\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"01012000\",\"TH\":\"26102018\",\"TI\":\"12122016\",\"TA\":\"10102017\",\"FlowCtrl\":0},{\"Name\":\"SF_Кличев\",\"Type\":\"SF21RU7C\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM34\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"01012000\",\"TH\":\"26102018\",\"TI\":\"07072018\",\"TA\":\"01012000\",\"FlowCtrl\":0},{\"Name\":\"SF_Рогачев\",\"Type\":\"SF21RU7C\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM26\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"01012000\",\"TH\":\"26102018\",\"TI\":\"13022018\",\"TA\":\"17072018\",\"FlowCtrl\":0},{\"Name\":\"SF_Жлобин\",\"Type\":\"SF21RU7C\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":1200,\"Port\":\"COM28\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"01012000\",\"TH\":\"26102018\",\"TI\":\"12122016\",\"TA\":\"19092018\",\"FlowCtrl\":0},{\"Name\":\"SF_ПХГ\",\"Type\":\"SF21RU6D\",\"Password\":\"123\",\"Address\":1,\"BaudRate\":9600,\"Port\":\"COM36\",\"DataBits\":8,\"Parity\":0,\"StopBit\":1,\"TD\":\"24102018\",\"TH\":\"26102018\",\"TI\":\"29102018\",\"TA\":\"29102018\",\"FlowCtrl\":0}]}");
	//$(SolutionDir)DevProcessorTest\Debug\DevProcessorTest.exe
	for (;;)
	{
		std::cout << "Subscribe..." << std::endl;
		//char* caTags[] = GetSubscribe ();
		//char* caTags[]
		//{
				//"{\"DeviceName\":\"SF_Жорновка\",\"RequestName\":\"R4\",\"ParamName\":\"YestFlowTotal\",\"ParamsOutList\":[{\"Name\":\"RunNumber\",\"Value\":\"1\"}]}"
		//};
#endif

#ifndef LOAD_ZIP
		int i = 0;
		for (auto T : caTags)
		{
			Subscribe (++i, T);			//Sleep (50);
		}
		char *addr[] = {
			"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"W1\",\"ParamName\":\"kCO2\"}"
			,"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"W1\",\"ParamName\" : \"kN2\"}"
			,"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"W1\",\"ParamName\":\"kd\"}"
			,"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"W1\",\"ParamName\":\"Dat\"}"
		};
		//char value[] = { 0x3D,0x0A,0x57,0x3E };		*((float*)value) = 0.21;
		//char caInitDrv[64] = "SF_Жлобин";//EC605_test ,192.168.1.23
		//StopStartDev (caInitDrv, true);
		//Sleep (40000);
		//StopStartDev (caInitDrv, false);
		//StopStartDev (caInitDrv, true);
		
		//float fValues[] = { 0.214, 0.585, 0.5621 };
		//std::cout << "Write..." << std::endl;
		//char caValues[3][4];
		//char caVal[4];
		//i = 0;
		//for (auto a : addr)			//for (; i < 0; i++)
		//{
		//	*((float*)caVal) = fValues[i];
		//	WriteValue (a, caVal, sizeof (caVal));
		//	i++;
		//	//*((float*)caVal) = fValues[i];
		//	//WriteValue (addr[i], caVal, sizeof (caVal));
		//}
		std::cout << "Процесс ... (Enter - выход, пробел - новый цикл)";
		int Key = getchar ();
		while (Key != 0x20)
		{
			if (Key == 10)		// Enter
				goto lbExit;
			std::cout << "Enter - выход, Space - новый цикл";
			Key = getchar ();
			Sleep (0);
		}
		Close ();
	}
lbExit:
	Close ();
#endif
	return 0;
}
//_____________________________________________________________________________

//void* LoadDll (LPCSTR lpLibFileName, LPCSTR FuncName)
//{              // Handle to DLL  
//	/*DInitConfig*/void* lpfnDllFunc = nullptr;    // Function pointer  
//	if (hDLL == nullptr)
//		hDLL = LoadLibrary (lpLibFileName);
//	if (hDLL != nullptr)
//	{
//		lpfnDllFunc = /*(DInitConfig)*/GetProcAddress (hDLL, FuncName);
//		if (!lpfnDllFunc)
//		{
//			FreeLibrary (hDLL);
//			hDLL = nullptr;
//			return nullptr;
//		}
//	}
//	return lpfnDllFunc;
//}
//_____________________________________________________________________________

string CRCtoASCII (USHORT crc)
{
	string res;
	for (int i = 12; i >= 0; i -= 4)
	{
		res += (char)(0x30 + ((crc >> i) & 0xf));
	}
	return res;
}
//_____________________________________________________________________________

void TestTCP ()
{
	//CTCP *TCP;
	//CMess *Mess = new CMess ("LogTestRMG.log");
	//char caBufTX[CTCP::LEN_MAX] = { 1, 3, 0, 1, 0, 3, 0 };
	//char caBufRX[CTCP::LEN_MAX];
	//int iCountBites = 0;

	//int iLenData = 6;
	//unsigned short usCRC = CRC ((unsigned char *)caBufTX, iLenData, 0x8005, 0xFFFF);
	//caBufTX[iLenData] = usCRC >> 8;
	//caBufTX[iLenData + 1] = usCRC & 0xFF;
	//caBufTX[iLenData + 2] = 0;
	//try
	//{
	//	TCP = new CTCP ("127.0.0.1", 503/*, dynamic_cast<CMessages*> (Mess)*/);
	//	if (TCP->Opening())
	//	{
	//		if (TCP->Send (caBufTX, iLenData + 2))
	//		{
	//			Mess->Save (("ТХ " + Mess->CharArrToStrAsInt (caBufTX, iLenData + 2, 16)).c_str());
	//			TCP->Recieve (caBufRX, CTCP::LEN_MAX, iCountBites);
	//			Mess->Save (("RХ " + Mess->CharArrToStrAsInt (caBufRX, iCountBites, 16)).c_str());
	//		}
	//	}
	//}
	//catch (const CErrSock& exc)
	//{
	//	Mess->Save (exc.what ());
	//}
	//delete Mess;
	//if (TCP)
	//	delete TCP;
}
///____________________________________________________________________________

 // \brief Calculating CRC with polynom and init value as parameter
 //		Note: special algorithm used with left shift
 // \param data pointer to input array
 // \param length number of input bytes to be used for CRC calculations
 // \param poly polynom to use in CRC calculation
 // \param init initial value
 // \return CRC value
unsigned short CRC (const unsigned char* data, UINT length, USHORT poly, USHORT init, bool isbinary)
{
	USHORT crc = init;

	for (UINT i = 0; i < length; i++)
	{
		if (!isbinary && (data[i] == '\r' || data[i] == '\n'))
			crc ^= 0x30 << 8;
		else
			crc ^= data[i] << 8;
		for (int k = 0; k < 8; k++)
		{
			if (crc & 0x8000)
			{
				crc = crc << 1;
				crc ^= poly;
			}
			else
			{
				crc = crc << 1;
			}
		}
	}
	return crc;
}