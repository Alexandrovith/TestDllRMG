// RmgPriborDrv.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <iostream>
#include <process.h> 
#include "json.h"
#include "tchar.h"
#include "RmgPriborDrv.h"
#include "SerialDev.h"
#include "Common.h"
#include <direct.h>
#include <memory>

#include "Messages.h"
#include "Log.h"
#include "TrunkBuf.h"

using namespace std;

const char* cpDeviceTypeRMGconfig = "DeviceTypeRMG.config";

volatile bool devLoopStarted = TRUE;
volatile bool bNeedStop = FALSE;
HANDLE ghDevicesMutex;							//< Mutex to lock access to devices
bool bConfigInitialized = FALSE;		//< Config were called flag to know that thread was started
Json::Value devicesConfig;					//< This contains config of the devices
//Json::Value DeviceTypesConfig;		///< This contains supported device types
std::vector<SerialDev*> Devices;		//< This vector contains devices list
char archiveTemp[16 * 1024 * 10];		//< This array is used to store temporary archive data 128
string binDir;


////_____________________________________________________________________________
//
//std::string CharArrToStrAsInt (char *caBuf, int iSize, int Radix)
//{
//	std::string asVal;
//	for (size_t i = 0; i < iSize; i++)
//	{
//		char Buf[32];
//		asVal += itoa ((unsigned char)(*caBuf), Buf, Radix);
//		asVal += " ";
//		caBuf++;
//	}
//	return asVal;
//}
////_____________________________________________________________________________
//// Если iSymbolsQuant == 0, то вывод до числа 0
//UCHAR* CharArrToCharAsInt (void *vBuf, int iSymbolsQuant, int Radix)
//{
//	static char cpBufOut[CTrunkBuf::SIZE_BUF];
//	if (iSymbolsQuant <= 0)
//	{
//		cpBufOut[0] = 0;
//		return (UCHAR*)cpBufOut;
//	}
//	std::unique_ptr<CTrunkBuf> BufOut (new CTrunkBuf (cpBufOut, iSymbolsQuant, Radix));
//
//	char* caBuf = (char*)vBuf;
//	if (iSymbolsQuant)
//	{
//		for (size_t i = 0; i < iSymbolsQuant; i++)
//		{
//			char Buf[16];
//			//strcat_s (cpBufOut, CTrunkBuf::SIZE_BUF, _itoa ((unsigned char)(*caBuf), Buf, Radix));
//			//strcat_s (cpBufOut, CTrunkBuf::SIZE_BUF, " ");
//			strncat (cpBufOut, _itoa ((unsigned char)(*caBuf), Buf, Radix), 2);
//			strncat (cpBufOut, " ", 1);
//			caBuf++;
//		}
//		strncat (cpBufOut, "\0", 1);
//	}
//	else
//	{
//		int iDiap = 0;
//		while (*caBuf)
//		{
//			char Buf[16];
//			strcat_s (cpBufOut, CTrunkBuf::SIZE_BUF, _itoa ((unsigned char)(*caBuf), Buf, Radix));
//			strcat_s (cpBufOut, CTrunkBuf::SIZE_BUF, " ");
//			caBuf++;
//			if (++iDiap == CTrunkBuf::SIZE_BUF)
//				break;
//		}
//	}
//	return (UCHAR*)cpBufOut;
//}
////_____________________________________________________________________________
//
//char* CharArrToCharAsInt (string asBuf, int iSize, int Radix)
//{
//	static char cpBufOut[CTrunkBuf::SIZE_BUF];
//	std::unique_ptr<CTrunkBuf> BufOut (new CTrunkBuf (cpBufOut, iSize, Radix));
//
//	for (size_t i = 0; i < iSize; i++)
//	{
//		char Buf[16];
//		strcat_s (cpBufOut, CTrunkBuf::SIZE_BUF, _itoa ((unsigned char)(asBuf[i]), Buf, Radix));
//		strcat_s (cpBufOut, CTrunkBuf::SIZE_BUF, " ");
//	}
//	return cpBufOut;
//}
////_____________________________________________________________________________

//char* GetTimeNow ()
//{
//	SYSTEMTIME st;
//	GetLocalTime (&st);
//	static char caBuf[48];
//	sprintf_s (caBuf, "%02d%02d%02d %02d:%02d:%02d.%03d\t", st.wDay, st.wMonth,
//						 st.wYear - 2000, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
//	return caBuf;
//}
//_____________________________________________________________________________

RMGPRIBORDRV_API void FileLogMessage (const char* message)
{
	string logsdir = binDir + "\\logs";
	_mkdir (logsdir.c_str ());
	ofstream flog (logsdir + "\\DriverOutRMG.log", ios::app | ios::binary);
	flog << GetTimeNow () << message << endl;
	flog.close ();
# ifdef CONSOLE_OUT
	wcout << message.c_str () << endl;
# endif
}
//_____________________________________________________________________________

RMGPRIBORDRV_API void FilesLogMessage (string fname, const char* message)
{
	string logsdir = binDir + "\\logs";
	_mkdir (logsdir.c_str ());
	//ofstream flog(logsdir + /*"\\EC605_" + */fname + ".log", ios::app|ios::binary);
	ofstream flog (logsdir + "\\" + fname + ".log", ios::app | ios::binary);
	flog << GetTimeNow () << message << endl;
	flog.close ();
# ifdef CONSOLE_OUT
	wcout << message.c_str () << endl;
# endif
}
//_____________________________________________________________________________

RMGPRIBORDRV_API void FileLogCharMessage (string fname, const char* message, unsigned int count)
{
	string logsdir = binDir + "\\logs";
	_mkdir (logsdir.c_str ());
	ofstream flog (logsdir + "\\EC605_" + fname + ".log", ios::app | ios::binary);
	flog.write (message, count);
	flog.close ();
}
//_____________________________________________________________________________

BOOL APIENTRY DllMain (HMODULE hModule,
											 DWORD  ul_reason_for_call,
											 LPVOID lpReserved
)
{
	//hDll = hModule;
	if (binDir.empty ())
	{
		char moduleFileName[256];
		GetModuleFileNameA ((HMODULE)hModule, moduleFileName, sizeof (moduleFileName));
		string strfile (moduleFileName);
		string::size_type pos = strfile.find_last_of ("\\/");
		binDir = strfile.substr (0, pos);

		Mess = make_shared<CMess>(binDir + "\\logs");	//new CMess (binDir + "\\logs");
	}
	//switch (ul_reason_for_call)
	//{
	//case DLL_PROCESS_ATTACH:

	//case DLL_THREAD_ATTACH:
	//case DLL_THREAD_DETACH:
	//case DLL_PROCESS_DETACH:
	//	break;
	//}
	return TRUE;
}

/// \brief Lock program in case of common resource (serial devices) access 
void lockDevices ()
{
	::WaitForSingleObject (ghDevicesMutex, INFINITE);  // no time-out interval
}

/// \brief UnLock program in case of common resource (serial devices) access
void unlockDevices ()
{
	::ReleaseMutex (ghDevicesMutex);
}

/// \brief Set indicator to stop processing
void setNeedStop ()
{
	//lockDevices ();
	bNeedStop = TRUE;	//unlockDevices ();
}

/// \brief indicator that main processing loop started
bool isLoopStarted ()
{
	return devLoopStarted;
}

/// \brief This function creates device
/// \param DeviceConfig config of the device
/// \return pointer to created device if creation was successful
SerialDev* CreateDevice (const Json::Value& DeviceConfig, const Json::Value& DeviceType)
{
	string devName = DeviceConfig["Name"].asString ();
	Mess->SetNameDev (devName);

	if (DeviceConfig["Port"].asString ().empty ())
	{
		Mess->Out ("CreateDevice: Port name is empty!");   //LOGF (devName, "CreateDevice: Port name is empty!");
		return NULL;
	}

	if (DeviceType["Protocol"].asString ().empty ())
	{
		Mess->Out ("CreateDevice: No device Type Protocol");   //LOGF (devName, "CreateDevice: No device Type Protocol");
		return NULL;
	}

	SerialDev * serialDev = new SerialDev (DeviceConfig, DeviceType);
	if (serialDev != NULL)
	{
		Mess->Out ("Подкл.прибор %s(%s)",  /*devName.c_str (),*/DeviceConfig["UrlSingleParam"].asCString (),DeviceConfig["Port"].asCString ());
				    //LOGF (devName, "Подкл.прибор (%s) %s", /*devName.c_str (),*/DeviceConfig["Port"].asCString (), DeviceType["Protocol"].asCString ());				 
		Devices.push_back (serialDev);
	}
	return serialDev;
}

/// \brief This function looks for Serial Device 
/// \param DeviceConfig device config
/// \param DeviceType type configuration 
/// \return pointer to the serial device class object
SerialDev * FindSerialDevice (const Json::Value& DeviceConfig, const Json::Value& DeviceType)
{
	// check device existence
	if (DeviceConfig["Transport"].asString ().compare ("Ethernet") == 0)
	{
		string asIP = DeviceConfig["Address"].asString ();
		if (asIP.length () < 7)
			return NULL;
		for (UINT dev = 0; dev < Devices.size (); dev++)
		{
			if (Devices[dev]->GetIP() == asIP) 
			{
				return Devices[dev];
			}
		}
	}
	else
	{
		string devicePort = DeviceConfig["Port"].asString ();
		if (devicePort.empty ())
			return NULL;
		for (UINT dev = 0; dev < Devices.size (); dev++)
		{
			//if (Devices[dev]->getPort().compare(devicePort) == 0) {
			if (strcmp (Devices[dev]->getPort (), devicePort.c_str ()) == 0)
			{
				return Devices[dev];
			}
		}
	}
	return 0;
}

/// \brief This function looks for Serial Device by name
/// \param deviceName device config
/// \return pointer to the serial device class object
inline SerialDev* FindSerialDeviceByName (string deviceName)
{
	for (UINT dev = 0; dev < Devices.size (); dev++)
	{
		if (Devices[dev]->getName ().compare (deviceName) == 0)
		{
			return Devices[dev];
		}
	}
	return 0;
}

/// \brief Thread for processing all devices in loop
/// \param passed to thread function additional param
void devProcessingLoop (void * arg)
{
	UINT devNum = 0;
	while (bNeedStop == false)
	{
		lockDevices ();
		try
		{
			for (; devNum < Devices.size () && bNeedStop == false; devNum++)
			{
				Devices[devNum]->processTransactions ();
			}
			devNum = 0;
		}
		catch (const std::exception&)
		{
			if (++devNum == Devices.size ())
			{
				devNum = 0;
			}
		}
		unlockDevices ();
		Sleep (20);
	}
	//while (devLoopStarted)
	//{
	//	lockDevices();
	//	if(bNeedStop){
	//		devLoopStarted = false;
	//		break;
	//	}
	//	for(UINT devNum = 0; devNum < Devices.size(); devNum++){
	//		Devices[devNum]->processTransactions();
	//	}			
	//	unlockDevices();
	//	Sleep(20);
	//}
	////ExitThread(0);
	devLoopStarted = false;
	_endthread ();//TODO DELETE?
}

EXTERN_C RMGPRIBORDRV_API void Init ()
{
	InitConfig (NULL);
}

extern "C" RMGPRIBORDRV_API char* GetName ()
{
	return (char*)"EC-605";
}

char strver[20];

extern "C" RMGPRIBORDRV_API char* GetVer ()
{
	int rev = atoi (VERSION_REVISION + 10);
	_snprintf (strver, sizeof (strver) - 1, "%d.%d.%04d", VERSION_MAJOR, VERSION_MINOR, rev);
	return strver;
}

/// \brief Reading config file and return it as string
/// \param configName name of file to read
string readConfigFile (const char* configName)
{
	string json;

	ifstream myfile (binDir + "\\" + configName);
	if (myfile.is_open ())
	{
		while (!myfile.eof ())
		{
			string line;
			myfile >> line;
			json += line;
		}
		myfile.close ();
		return json;
	}
	return string ();
}

/// \brief This function parses config from string
/// \param jsonConfig string with config
/// \param object for storing device configuration
/// \keyTree key to find in configuration as root element
/// \return TRUE on success
BOOL processConfig (string jsonConfig, Json::Value * jsonCfg, string keyTree)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Value deviceTypeConfiguration;
	Json::Reader reader;
	if (!reader.parse (jsonConfig, root))
	{
		LOG ("Can't parse JSON configuration");
		return FALSE;
	}

	int devicesFound = root["Devices"].size (); // "Devices"
	if (devicesFound == 0)
	{
		LOG ("Can't parse JSON configuration: Devices not set");
		return FALSE;
	}

	deviceTypeConfiguration = root["Type"];
	if (deviceTypeConfiguration.isNull ())
	{
		string strConfig = readConfigFile (cpDeviceTypeRMGconfig);
		if (strConfig.size () == 0)
		{
			LOG ("Не найден файл конфигурации %s", cpDeviceTypeRMGconfig);
			return FALSE;
		}
		if (!reader.parse (strConfig, deviceTypeConfiguration))
		{
			LOG ("Can't parse JSON Type configuration from config file");
			return FALSE;
		}
	}

	int sdCount = 0;
	for (int i = 0; i < devicesFound; i++)
	{ // loop for all devices same Type to create or change SerialDev
		SerialDev * dev = FindSerialDevice (root["Devices"][i], deviceTypeConfiguration);
		if (dev != 0)
		{
			sdCount++;
		}
		else
		{ // here we are creating new device
			dev = CreateDevice (root["Devices"][i], deviceTypeConfiguration);
		}
	}
	/*	if(sdCount == 0) {
			LOG("Configuration: Parameters for %s not found.\n", keyTree.c_str());
			return FALSE;
		}
	*/
	return TRUE;
}

/// \brief This function initializes driver and starts main device loop
EXTERN_C RMGPRIBORDRV_API void InitConfig (char* jsonConfig)
{
	try
	{
		if (!processConfig (string (jsonConfig), &devicesConfig, ""))
		{
			cout << "Can't process config from SCADA, dummy starting..." << endl;	/// < configuration will be parsed in the future function call
		}

		if (!bConfigInitialized)
		{
			ghDevicesMutex = CreateMutex (NULL, FALSE, NULL);     // default security attributes // initially not owned
			_beginthread (devProcessingLoop, 0, NULL);
			bConfigInitialized = true;
		}
	}
	catch (const std::exception& exc)
	{
		LOG ("InitConfig: %s", exc.what ());
	}
}

/// \brief This function is used to subscribe on parameter given by address string
/// \param tagId tag Id to assign for given address 
/// \param address string describing parameter
/// \return o in case of error
EXTERN_C RMGPRIBORDRV_API int Subscribe (int tagId, char * address)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

	if (!reader.parse (address, root))
	{
		Mess->Out ("Subscribe ERROR: can't parse address string"); 
		return 0;
	}

	string deviceName = root["DeviceName"].asString ();
	SerialDev *dev = FindSerialDeviceByName (deviceName);
	if (dev == 0)
	{
		const char* cpName = deviceName.c_str ();
		Mess->Out ("Subscribe ERROR: Serial device [%s](%d) not found"/* [%s]."*/, cpName, deviceName.length()/*, 
							 Mess->CharArrToCharAsInt(cpName, deviceName.length())*/);
		return 0;
	}

	//if (dev->getDebug () == 0)
	//{
	//	if (!devicesConfig["Debug"].isNull ())
	//		dev->setDebug (devicesConfig["Debug"].asUInt ());
	//}
	lockDevices ();
	if (dev->isFirstSubscribe ())
	{
		Json::Value dummy;
		dummy["RequestName"] = "R1";
		dummy["ParamName"] = "Service";//TODO restore my?
		dev->subscribe (0, dummy, true);
	}
	if (!dev->subscribe (tagId, root, false))
	{
		unlockDevices ();
		Mess->Out ("Subscribe to %s. No TagId.", address);   //LOGF (deviceName, "Subscribe to %s. No TagId.", address);
		return 0;
	}

	Mess->Out ("Подкл. [%s][%s]", root["ParamName"].asString ().c_str (), root["RequestName"].asString ().c_str ());   //LOGF (deviceName, "Подкл. [%s][%s]", root["ParamName"].asString ().c_str (), root["RequestName"].asString ().c_str ());
	unlockDevices ();
	return 1;
}

/// \brief Out some attributes of parameter given by address string
/// \param tagId tag Id to assigned for given address 
/// \param address string describing parameter
/// \param value value to write
/// \param size bytes count to write
/// \return NULL in case of error
EXTERN_C RMGPRIBORDRV_API int WriteValue (char * address, char* value, int size)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

	if (!reader.parse (address, root))
	{
		LOG ("WriteValue ERROR: can not parse address string");
		return 0;
	}

	string deviceName = root["DeviceName"].asString ();
	SerialDev *dev = FindSerialDeviceByName (deviceName);
	if (dev == 0)
	{
		Mess->Out ("[%s][%s] WriteValue ERROR: Serial device not found.", 
					root["ParamName"].asString ().c_str(), root["RequestName"].asString ().c_str());
		return 0;
	}
	//LOGF (deviceName, "WR %s %s", address, CharArrToCharAsInt (value, size));

	lockDevices ();
	if (!dev->subscribeWrite (root, value, size))
	{
		unlockDevices ();
		Mess->Out ("WriteValue: Can't subscribe to write %s, [%s].", address, deviceName.c_str ());
		return 0;
	}

	unlockDevices ();
	return 1;
}

/// \brief This function gets value for parameter assigned with tagid
/// \param tagId tag Id of parameter
/// \return integer value of parameter
EXTERN_C RMGPRIBORDRV_API int GetValue (int tagId, unsigned char* bufToStore)
{
	//return DevProc::GetValue(tagId, bufToStore);
	string name = tags[tagId].ParamName;
	if (name.empty ())
		return 0;
	SerialDev* serDev = tags[tagId].serailDev;
	if (serDev == 0)
		return 0;
	return serDev->getStorage ()->getValue (name, (UCHAR*)bufToStore);
	//int iRet = serDev->getStorage()->getValue(name, (UCHAR*) bufToStore);
	//*(double*)bufToStore = 5.0;
	//return iRet;
}

/// \brief This function returns archives data for given tag id
/// \param tagId tag Id of parameter
/// \return pointer to archive data
EXTERN_C RMGPRIBORDRV_API int * GetNewArchiveData (char * address)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

	if (!reader.parse (address, root))
	{
		LOG ("WriteValue ERROR: can not parse address string");
		return 0;
	}

	string deviceName = root["DeviceName"].asString ();
	SerialDev *serDev = FindSerialDeviceByName (deviceName);

	if (serDev == 0)
	{

		LOG ("GetArchiveData ERROR: Serial device %s not found.", deviceName.c_str ());
		return 0;
	}
	string str = serDev->getStorage ()->getNewArchiveData (root);
	int strLen = str.length ();

	if (strLen < 14)
	{
		str = "{\"Records\":[]}";
		strLen = str.length ();
	}
	memcpy (archiveTemp, str.c_str (), min (strLen + 1, sizeof (archiveTemp)));
	return (int *)archiveTemp;
}

/// \brief This function closes driver
EXTERN_C RMGPRIBORDRV_API void Close ()
{
	if (bConfigInitialized)
	{
		bConfigInitialized = false;
		setNeedStop ();
		while (isLoopStarted ())
			Sleep (0);
		for (UINT dev = 0; dev < Devices.size (); dev++)
			delete Devices[dev];
		Devices.clear ();
		CloseHandle (ghDevicesMutex);
		
	}
}


