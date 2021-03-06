#include "stdafx.h"
#include <stdio.h>
#include <vector>
#include "Common.h"
#include "RmgTransaction.h"
#include "RmgRawParam.h"
#include "Messages.h"

UCHAR _7e1_to_8n1[256] = {
	0x00, 0x81, 0x82, 0x03, 0x84, 0x05, 0x06, 0x87, 0x88, 0x09, 0x0A, 0x8B,
	0x0C, 0x8D, 0x8E, 0x0F, 0x90, 0x11, 0x12, 0x93, 0x14, 0x95, 0x96, 0x17,
	0x18, 0x99, 0x9A, 0x1B, 0x9C, 0x1D, 0x1E, 0x9F, 0xA0, 0x21, 0x22, 0xA3,
	0x24, 0xA5, 0xA6, 0x27, 0x28, 0xA9, 0xAA, 0x2B, 0xAC, 0x2D, 0x2E, 0xAF,
	0x30, 0xB1, 0xB2, 0x33, 0xB4, 0x35, 0x36, 0xB7, 0xB8, 0x39, 0x3A, 0xBB,
	0x3C, 0xBD, 0xBE, 0x3F, 0xC0, 0x41, 0x42, 0xC3, 0x44, 0xC5, 0xC6, 0x47,
	0x48, 0xC9, 0xCA, 0x4B, 0xCC, 0x4D, 0x4E, 0xCF, 0x50, 0xD1, 0xD2, 0x53,
	0xD4, 0x55, 0x56, 0xD7, 0xD8, 0x59, 0x5A, 0xDB, 0x5C, 0xDD, 0xDE, 0x5F,
	0x60, 0xE1, 0xE2, 0x63, 0xE4, 0x65, 0x66, 0xE7, 0xE8, 0x69, 0x6A, 0xEB,
	0x6C, 0xED, 0xEE, 0x6F, 0xF0, 0x71, 0x72, 0xF3, 0x74, 0xF5, 0xF6, 0x77,
	0x78, 0xF9, 0xFA, 0x7B, 0xFC, 0x7D, 0x7E, 0xFF, 0x00, 0x81, 0x82, 0x03,
	0x84, 0x05, 0x06, 0x87, 0x88, 0x09, 0x0A, 0x8B, 0x0C, 0x8D, 0x8E, 0x0F,
	0x90, 0x11, 0x12, 0x93, 0x14, 0x95, 0x96, 0x17, 0x18, 0x99, 0x9A, 0x1B,
	0x9C, 0x1D, 0x1E, 0x9F, 0xA0, 0x21, 0x22, 0xA3, 0x24, 0xA5, 0xA6, 0x27,
	0x28, 0xA9, 0xAA, 0x2B, 0xAC, 0x2D, 0x2E, 0xAF, 0x30, 0xB1, 0xB2, 0x33,
	0xB4, 0x35, 0x36, 0xB7, 0xB8, 0x39, 0x3A, 0xBB, 0x3C, 0xBD, 0xBE, 0x3F,
	0xC0, 0x41, 0x42, 0xC3, 0x44, 0xC5, 0xC6, 0x47, 0x48, 0xC9, 0xCA, 0x4B,
	0xCC, 0x4D, 0x4E, 0xCF, 0x50, 0xD1, 0xD2, 0x53, 0xD4, 0x55, 0x56, 0xD7,
	0xD8, 0x59, 0x5A, 0xDB, 0x5C, 0xDD, 0xDE, 0x5F, 0x60, 0xE1, 0xE2, 0x63,
	0xE4, 0x65, 0x66, 0xE7, 0xE8, 0x69, 0x6A, 0xEB, 0x6C, 0xED, 0xEE, 0x6F,
	0xF0, 0x71, 0x72, 0xF3, 0x74, 0xF5, 0xF6, 0x77, 0x78, 0xF9, 0xFA, 0x7B,
	0xFC, 0x7D, 0x7E, 0xFF };

//void convert8to7(UCHAR * buf, int size){
//	for (int i=0; i< size; i++){
//		buf[i] = _7e1_to_8n1[buf[i]];
//	}
//}

RmgTransaction::RmgTransaction (SerialDev* device, string requestName, string ParamName/*const Json::Value &jsonAddress*/) :
	Transaction (device, requestName),
	_needContinuation (FALSE),
	_8bit_transaction (FALSE)
{
	asParamName = ParamName;// jsonAddress["ParamName"].asString ();
	memset (_address, '\0', sizeof (_address));
	_userId = "00";
	setCommand ((void*)(requestName.c_str ()));
	USHORT cmd = (USHORT)getCommand ();
	addMoreParameters (device->getDeviceConfig ());
	if (cmd == RMG_W1_COMMAND || cmd == RMG_W2_COMMAND)
	{ // create commands chain w0->w1 or w0->w2
//		if(!addChainedRequest(&jsonAddress))
//			return;
		_command = RMG_W0_COMMAND;
		string w0 = "W0";
		_jsonRequest = findRequest (w0);
	}
	else if (cmd == COMM ('R', '6'))
	{
	}
	else
	{
		_jsonRequest = findRequest (_transactionName);
	}
	Mess->Out ("������ [%s][%s]", asParamName.c_str (), requestName.c_str ());//device->getName (), 
	if (_jsonRequest.empty ())
		return;

	iLenBufRX = _jsonRequest["InSize"].asUInt ();
	if (iLenBufRX > 0)
		cpBufRX = new unsigned __int8[iLenBufRX];
	memset (cpBufRX, 0, iLenBufRX);
	iLenBufTX = _jsonRequest["OutSize"].asInt ();
	_lReqPeriod = _jsonRequest["TimePeriod"].asUInt ();
	if (_lReqPeriod == 0)
		setOneShot (true);
	if (iLenBufTX > 0)
	{
		iLenBufTX += RMG_FF_BYTES;
		cpBufTX = new unsigned __int8[iLenBufTX + 1]; // +10? some more data in case of binary data enlargement (0d -> 1b 0e, 1b -> 1b 1b)
		memset (cpBufTX, '\0', iLenBufTX + 1);
	}
	else
	{
		cpBufTX = new unsigned __int8[50]; // float size
		memset (cpBufTX, '\0', 50);
	}

	if (getStorage () == 0)
		getSerialDev ()->setStorage (new RmgStorage ());

	auto Al = _jsonRequest["alignmin"];
	if (!Al.isNull ())
	{
		setAlignMinute (Al.asInt ());
	}

	auto T8 = _jsonRequest["b8transaction"];
	if (!T8.isNull ())
	{
		int bits_transaction = T8.asUInt ();
		if (bits_transaction)
			_8bit_transaction = true;
#ifdef _DEBUG
		Mess->Out ("b8transaction [%s][%s]", asParamName.c_str (), requestName.c_str ());
#endif // _DEBUG
	}
};
///____________________________________________________________________________

Transaction* RmgTransaction::createTransaction (SerialDev* device, string requestName, string asParamName)//const Json::Value &jsonAddress
{
	Transaction* transaction = (Transaction*)(new RmgTransaction (device, requestName, asParamName));
	if (transaction->getOutSize () == 0)
	{
		Mess->Out ("Can't create transaction for request %s, Device: %s)\n",
					requestName.c_str (), device->getPort ());
		delete transaction;
		return 0;
	}
	return transaction;
}
///____________________________________________________________________________

inline SerialDev* RmgTransaction::getSerialDev ()
{
	return ((SerialDev*)_device);
}
///____________________________________________________________________________

// \brief Adding request to be send after successful response to current request
// \param jsonAddress parsed address
bool RmgTransaction::addChainedRequest (const Json::Value* jsonAddress)
{

	_chainCommand = Format ("%.2s(%.2s\tD1123456789012)\r\n", _transactionName.c_str (), _userId.c_str ());
	setNeedContinuation (true);
	return true;
}
///____________________________________________________________________________

void RmgTransaction::InitParam (char *cpVal, int iSize)
{
	strncpy_s (caValue, cpVal, iSize);
}
///____________________________________________________________________________

// \brief Saving parameters value and size to be used in creating request
// \param value buffer with actual value
// \param size bytes used to contain value
// \return true on success
bool RmgTransaction::setWriteParameter (string asParamName, char* value, int size)
{
	strncpy_s (caValue, value, size);

	int ival;
	string outvalue;
	if (size == 0)
		return false;

	Json::Value nextRequest = findRequest (_transactionName);
	if (nextRequest.empty () || nextRequest["ParamsOutList"].empty ())
		return false;
	//string asParamName = (*jsonAddress)["ParamName"].asString ();
	if (asParamName.empty ())
		return false;

	UINT i;
	for (i = 0; i < nextRequest["ParamsOutList"].size (); i++)
	{
		if (nextRequest["ParamsOutList"][i]["Name"].asString ().compare (asParamName) == 0)
		{
			break;
		}
	}
	if (i == nextRequest["ParamsOutList"].size ())
		return false;
	// found request name, will process parameter and value to write
	Json::Value val1 = nextRequest["ParamsOutList"][i];
	string pcode = nextRequest["ParamsOutList"][i]["code"].asString ();
	string ptypestr = nextRequest["ParamsOutList"][i]["type"].asString ();
	UINT plength = nextRequest["ParamsOutList"][i]["length"].asUInt ();
	PARAM_TYPE ptype = getTypeByString (ptypestr);

	switch (ptype)
	{
	case PARAM_TYPE_INT:
		ival = *(int*)value;
		outvalue = Format ("%.2s(%.2s\t%.2s%0*d)", _transactionName.c_str (), _userId.c_str (), pcode.c_str (), plength, ival);
		break;
	case PARAM_TYPE_DOUBLE_FIXED:
	{
		double dbval = *(float*)value;
		int ecorr = nextRequest["ParamsOutList"][i]["ecorr"].asInt ();
		int corr = (int)(dbval / pow (10, ecorr) + 0.5);
		outvalue = Format ("%.2s(%.2s\t%.2s%0*d)", _transactionName.c_str (), _userId.c_str (), pcode.c_str (), plength, corr);
		break;
	}
	case PARAM_TYPE_HEX:
	{
		string strtmp = "";
		ival = *(int*)value;
		for (i = 0; i < plength; i++)
		{
			strtmp += (UCHAR)((ival >> ((plength - 1 - i) * 4)) & 0xf) + 0x30;
		}
		outvalue = Format ("%.2s(%.2s\t%.2s%.*s)", _transactionName.c_str (), _userId.c_str (), pcode.c_str (), plength, strtmp.c_str ());
		break;
	}
	case PARAM_TYPE_DATETIME:
	{
		time_t curTime;
		tm tmCurrent;
		time (&curTime);
		localtime_s (&tmCurrent, &curTime);

		outvalue = Format ("%.2s(%.2s\t%.2s%02d%02d%02d%02d%02d%02d)", _transactionName.c_str (),
																_userId.c_str (), pcode.c_str (),
																tmCurrent.tm_mday, tmCurrent.tm_mon + 1, tmCurrent.tm_year - 100,
																tmCurrent.tm_hour, tmCurrent.tm_min, tmCurrent.tm_sec);
		//_userId.c_str(), pcode.c_str(), value[0], value[1], value[2], value[3], value[4], value[5]);
		break;
	}
	case PARAM_TYPE_STRING:
		outvalue = Format ("%.2s(%.2s\t%.2s%.*s)", _transactionName.c_str (),
																_userId.c_str (), pcode.c_str (), plength, value);
		break;
	}
	string crcascii = CRCtoASCII (CRC ((unsigned char *)outvalue.c_str () + 2, outvalue.size () - 2, 0x8005, 0x0));
	//string crcascii = CRCtoASCII (CRC ((unsigned char *)outvalue.c_str () + 3, outvalue.size () - 4, 0x8005, 0x0));
	_writeParam = outvalue + crcascii + "!\r\n";
	return true;
}
///____________________________________________________________________________

// \brief Looking for request by Name in configuration of available requests
Json::Value RmgTransaction::findRequest (string& reqName)
{
	Json::Value* _deviceConfigType = getSerialDev ()->getDeviceConfigType ();
	if ((*_deviceConfigType)["Requests"].isNull ())
		return Json::Value ();
	for (UINT reqNum = 0; reqNum < (*_deviceConfigType)["Requests"].size (); reqNum++)
	{
		string rName = (*_deviceConfigType)["Requests"][reqNum]["Name"].asString ();
		if (rName.compare (reqName) == 0)
		{
			return (*_deviceConfigType)["Requests"][reqNum];
		}
	}
	return Json::Value ();
}
///____________________________________________________________________________

// \brief Get JSON "Commands"
Json::Value RmgTransaction::getCommandsDescription ()
{
	Json::Value* _deviceConfigType = getSerialDev ()->getDeviceConfigType ();
	if ((*_deviceConfigType)["Commands"].isNull ())
		return Json::Value ();
	return (*_deviceConfigType)["Commands"];
}
///____________________________________________________________________________

// \brief Get JSON "Status" mask description
Json::Value RmgTransaction::getStatusDescription ()
{
	Json::Value* _deviceConfigType = getSerialDev ()->getDeviceConfigType ();
	if ((*_deviceConfigType)["Status"].isNull ())
		return Json::Value ();
	return (*_deviceConfigType)["Status"];
}
///____________________________________________________________________________

void RmgTransaction::setCommand (void* cmd)
{
	if (cmd != 0)
		_command = *(USHORT*)cmd;
	else
		_command = 0;
}
///____________________________________________________________________________

void RmgTransaction::prepareToSend ()
{
	USHORT cmd = (USHORT)getCommand ();
	if (cmd == RMG_W0_COMMAND && _writeParam.size ())
	{
		string asPass = getSerialDev ()->getPassword ();
		rsize_t iSize = asPass.size ();
		int iStart = RMG_FF_BYTES + 7 + (5 - iSize);
		strncpy ((char*)cpBufTX + iStart, asPass.c_str (), iSize);		//memcpy (cpBufTX + RMG_FF_BYTES + 14 + (5 - asPass.size ()), asPass.c_str (), asPass.size ());
	}
	//else if (cmd == RMG_W1_COMMAND)
	//{
	//}
	if (!IS_RMG_W_COMMAND (cmd))
	{
		createRequest ();
	}

	if (IS_RMG_DH_COMMAND (cmd))
	{
		setStartDate ();
		UINT sum = 0;
		for (UINT i = 0; i < 17; i++)
		{
			sum += cpBufTX[RMG_FF_BYTES + i];
		}
		cpBufTX[RMG_FF_BYTES + 17] = (sum & 0x7f) | 0x40;
	}

	int iLenSend = getOutLength ();
	cpBufTX[iLenSend] = 0;		// TODO ������?!

	int iShift = cpBufTX[0] == 0xFF ? RMG_FF_BYTES : 0;
#ifdef _DEBUG
	Mess->Out ("[%s]\tWill send[%c%c] %s[%s]", asParamName.c_str (), (_command) & 0xff, _command >> 8, //TODO RESTORE
				Mess->CharArrToCharAsInt (cpBufTX + iShift, iLenSend - iShift), cpBufTX + iShift);
#else
	Mess->Out ("[%s]\tWill send[%c%c][%s]", asParamName.c_str (), (_command) & 0xff, _command >> 8, //TODO RESTORE
				cpBufTX + iShift);
#endif // _DEBUG

	if (getSerialDev ()->getDataBits () == 8)
	{ // convert to 7e1
		for (UINT i = 0; i < iLenSend; i++)
		{
			cpBufTX[i] = _7e1_to_8n1[(UCHAR)cpBufTX[i]];
		}
	}
}
///____________________________________________________________________________

void RmgTransaction::setAddr (char* addr)
{
	strncpy_s ((char*)_address, 3, addr, 3);
}
///____________________________________________________________________________

// \brief Fill buffer with prepared time values according to cmd (H1,H2, D1,D2)
void RmgTransaction::setStartDate ()
{
	struct tm tmtime;
	USHORT cmd = (USHORT)getCommand ();
	if ((RMG_H1_COMMAND == cmd) || (RMG_D1_COMMAND == cmd))
	{
		cpBufTX[RMG_FF_BYTES + 12] = 0;
		cpBufTX[RMG_FF_BYTES + 13] = 0;
		cpBufTX[RMG_FF_BYTES + 14] = 0;
		cpBufTX[RMG_FF_BYTES + 15] = 0;
		cpBufTX[RMG_FF_BYTES + 16] = 0;
		return;
	}
	time_t t;
	if (IS_RMG_H_COMMAND (cmd))
	{
		t = getSerialDev ()->getHTime ();
		localtime_s (&tmtime, &t);
	}
	else
	{
		t = getSerialDev ()->getDTime ();
		localtime_s (&tmtime, &t);
	}
	Mess->Out ("[%c%c]", *((char*)&cmd), *((char*)&cmd + 1)		//"[%c%c] start time: %02d.%02d.%02d %02d:%02d", *((char*)&cmd), *((char*)&cmd + 1),
	/*tmtime.tm_year - 100, tmtime.tm_mon + 1, tmtime.tm_mday, tmtime.tm_hour, tmtime.tm_min*/);
	int value = tmtime.tm_year - 100;
	char tx = ((value / 10) << 4) + value % 10;
	cpBufTX[RMG_FF_BYTES + 12] = tx;
	value = tmtime.tm_mon + 1;
	tx = ((value / 10) << 4) + value % 10;
	cpBufTX[RMG_FF_BYTES + 13] = tx;
	value = tmtime.tm_mday;
	tx = ((value / 10) << 4) + value % 10;
	cpBufTX[RMG_FF_BYTES + 14] = tx;
	value = tmtime.tm_hour;
	tx = ((value / 10) << 4) + value % 10;
	cpBufTX[RMG_FF_BYTES + 15] = tx;
	value = tmtime.tm_min;
	tx = ((value / 10) << 4) + value % 10;
	cpBufTX[RMG_FF_BYTES + 16] = tx;

}
///____________________________________________________________________________

// \brief Creating request for RMG transfer
bool RmgTransaction::createRequest ()
{
	int end_part = 12;
	USHORT cmd = (USHORT)getCommand ();
	//if (cmd == RMG_W0_COMMAND || cmd == RMG_W1_COMMAND || cmd == RMG_W2_COMMAND) {// create commands chain w0->w1 or w0->w2 TODO ���������������� ��� ��������
	if (IS_RMG_W_COMMAND (cmd))
	{
		// write password will be set in prepare to send
			//memcpy(cpBufTX + RMG_FF_BYTES + 7 + (5 - _writePassword.size()), _writePassword.c_str(), _writePassword.size());//TODO:COMMENT return?
	}
	else
	{
		int iSize = _password.size ();
		int iStart = RMG_FF_BYTES + 7 + (5 - iSize);
		memcpy (cpBufTX + iStart, _password.c_str (), iSize);
	}
	*(USHORT*)(cpBufTX + RMG_FF_BYTES + 5) = (USHORT)getCommand ();
	memset (cpBufTX, 0xff, RMG_FF_BYTES);
	cpBufTX[RMG_FF_BYTES + 0] = '/';
	cpBufTX[RMG_FF_BYTES + 1] = '?';
	memcpy (cpBufTX + RMG_FF_BYTES + 2, _address, 3);
	if (IS_RMG_DH_COMMAND (cmd))
	{
		end_part = 18;
	}

	cpBufTX[RMG_FF_BYTES + end_part] = '!';
	cpBufTX[RMG_FF_BYTES + end_part + 1] = '\r';
	cpBufTX[RMG_FF_BYTES + end_part + 2] = '\n';
	cpBufTX[RMG_FF_BYTES + end_part + 3] = 0;	//cpBufTX[RMG_FF_BYTES + 15] = '\0';
	return TRUE;
}
///____________________________________________________________________________

// \brief Processing additional parameters for serial device, specific for current device type RMG / EC605
// \param deviceConfig input configuration for serial device to process
void RmgTransaction::addMoreParameters (Json::Value* deviceConfig)
{
	auto U = (*deviceConfig)["UserId"];
	if (!U.isNull ())
	{
		string strid = U.asString ();
		if (strid.length () == 2)//TODO ��������� ����� (�� ����)
		{
			_userId = strid;
		}
	}

	auto P = (*deviceConfig)["Password"];
	if (!P.isNull ())
	{
		_password = P.asString ();
	}

	auto WP = (*deviceConfig)["WritePassword"];
	if (!WP.isNull ())
	{
		_writePassword = WP.asString ();
	}

	// Adding address
	auto A = (*deviceConfig)["Address"];
	if (!A.isNull ())
	{
		if (A.isNumeric ())//(*deviceConfig)["Address"]
		{
			int addr = A.asInt ();
			if (addr < 1000)
				sprintf_s (_address, "%03i", addr);
			else
				Mess->Out ("SetAddress: Address should be string of 3 bytes. Using 000 instead.");
		}
		else
		{
			string addr = A.asString ();
			if (addr.size () < 4)
			{
				memcpy (_address + (3 - addr.size ()), addr.data (), addr.size ());
			}
			else
			{
				Mess->Out ("SetAddress: Address should be string of 3 bytes. Using 000 instead.");
			}
		}
	}
}
///____________________________________________________________________________

DWORD RmgTransaction::GetRecieveLen ()
{
	if (iLenBufRX > 0)
		return strnlen_s ((char*)cpBufRX, iLenBufRX);
	return 0;
}
///____________________________________________________________________________

DWORD RmgTransaction::getOutLength (unsigned __int8 ucR)
{
	int iLen = 10;
	unsigned __int8* ucPbuf = cpBufTX + iLen;
	for (; iLen < iLenBufTX; iLen++, ucPbuf++)
	{
		unsigned char c1 = *ucPbuf;
		unsigned char c2 = *(ucPbuf + 1);
		if (c1 == ucR && c2 == '\n')
		{
			return (iLen > iLenBufTX - 2) ? iLen : iLen + 2;
		}
	}
	return iLen;
	//if (iLenBufTX > 0)
	//	return iLenBufTX;
	//else
	//	return strnlen_s ((char*)cpBufTX, iLenBufTX);
}
///____________________________________________________________________________

void RmgTransaction::setNeedContinuation (bool isNeedContinuation)
{
	_needContinuation = isNeedContinuation;
}
///____________________________________________________________________________

bool RmgTransaction::isNeedContinuation ()
{
	USHORT cmd = getCommand ();
	if (cmd == RMG_H2_COMMAND || cmd == RMG_D2_COMMAND)
	{
		// skip next archive chain to give system time to process other requests
		getSerialDev ()->nextTransaction ();
		return _needContinuation;
	}
	else
	{
		if (_writeParam.size () == 0)
		{
			_needContinuation = false;
			return false;
		}
		int iSize = _writeParam.size ();
		int iSizeMax = getOutSize ();
		strncpy ((char*)cpBufTX + RMG_FF_BYTES, _writeParam.c_str (), iSize);// TODO ��� ��������� - RMG_FF_BYTES
		_writeParam.clear ();
	}
	return true;
}
///____________________________________________________________________________

// \brief Convert CRC to ascii value (0x30 + hex sign) 
// \param crc ushort crc value to convert to ascii
// \return string with converted value
string RmgTransaction::CRCtoASCII (USHORT crc)
{
	string res;
	for (int i = 12; i >= 0; i -= 4)
	{
		res += (char)(0x30 + ((crc >> i) & 0xf));
	}
	return res;
}
///____________________________________________________________________________

// \brief Calculating CRC with polynom and init value as parameter
//		Note: special algorithm used with left shift
// \param data pointer to input array
// \param length number of input bytes to be used for CRC calculations
// \param poly polynom to use in CRC calculation
// \param init initial value
// \return CRC value
unsigned short RmgTransaction::CRC (const unsigned char* data, UINT length, USHORT poly, USHORT init, bool isbinary)
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

// \brief Remove all \r \n symbols from buffer
// \param str Buffer to process (remove \r \n)
void RmgTransaction::remove0D0A (string str)
{
	const string delim = "\r\n";
	for (string::iterator it = str.begin (); it != str.end (); ++it)
	{
		if (*it == '\r' || *it == '\n')
			str.erase (it);
	}
}

// \brief Calculating CRC and compare with value specified in stream
// \brief param str input stream
// \return true on success CRC comparison
bool RmgTransaction::checkResponseCRC (string str)
{
	if (str.size () <= 4)
		return false;
	USHORT crcval = 0;
	for (UINT i = str.size () - 4; i < str.size (); i++)
	{
		crcval <<= 4;
		UCHAR crcChar = str[i] - 0x30;
		if (crcChar > 15)
			return false;
		crcval += crcChar;

	}
	USHORT crcCalc = CRC ((unsigned char *)str.data (), str.size () - 4, 0x8005, 0x0000);
	return crcCalc == crcval;
}

// \brief Processing input buffer - sub packet xxx in packet "se(xxx)" and divide(separator \t) internal data into words
// \param str input buffer
// \param separator character to divide string into vector 
// \return vector of parsed words
vector<string> RmgTransaction::getSeparatedStrings (string str, char* separator)
{
	vector<string> result;
	int start = 0, end;
	do
	{
		end = str.find (separator, start);
		string foundWord = str.substr (start, end - start);
		start = end + strlen (separator);
		result.push_back (foundWord);
	} while (end != string::npos);
	return result;
}


PARAM_TYPE RmgTransaction::getTypeByString (string stype)
{
	if (stype.compare ("byte") == 0)
	{
		return PARAM_TYPE_BYTE;
	}
	else if (stype.compare ("word") == 0)
	{
		return PARAM_TYPE_WORD;
	}
	else if (stype.compare ("integer") == 0)
	{
		return PARAM_TYPE_INT;
	}
	else if (stype.compare ("double") == 0)
	{
		return PARAM_TYPE_DOUBLE;
	}
	else if (stype.compare ("doublefixed") == 0)
	{
		return PARAM_TYPE_DOUBLE_FIXED;
	}
	else if (stype.compare ("doublekp") == 0)
	{
		return PARAM_TYPE_DOUBLE_KP;
	}
	else if (stype.compare ("hex") == 0)
	{
		return PARAM_TYPE_HEX;
	}
	else if (stype.compare ("string") == 0)
	{
		return PARAM_TYPE_STRING;
	}
	else if (stype.compare ("year") == 0)
	{
		return PARAM_TYPE_YEAR;
	}
	else if (stype.compare ("month") == 0)
	{
		return PARAM_TYPE_MONTH;
	}
	else if (stype.compare ("day") == 0)
	{
		return PARAM_TYPE_DAY;
	}
	else if (stype.compare ("hour") == 0)
	{
		return PARAM_TYPE_HOUR;
	}
	else if (stype.compare ("minute") == 0)
	{
		return PARAM_TYPE_MINUTE;
	}
	else if (stype.compare ("second") == 0)
	{
		return PARAM_TYPE_SECOND;
	}
	else if (stype.compare ("datetime") == 0)
	{
		return PARAM_TYPE_DATETIME;
	}
	else if (stype.compare ("bcd") == 0)
	{
		return PARAM_TYPE_BCD;
	}
	return PARAM_TYPE_STRING;
}

// \brief Special processing of parameters with type doubleKp
// \param asParamName name of parameter
// \param initValue value to be changed with kp dependency
// \param kpname kp name to get value in storage
// \return converted value
double RmgTransaction::convertWithKp (string paramName, double initValue, string kpname)
{
	RmgRawParam * rrp = (RmgRawParam *)getStorage ()->get (kpname);
	if (rrp == 0)
	{
		Mess->Out ("Convert double Kp - not specified");
		return initValue;
	}

	double kp;
	rrp->getDouble (&kp);

	if (paramName.compare ("Vn") == 0 || paramName.compare ("Vnest") == 0)
	{
		initValue = (initValue * 0.0001L * kp);
	}
	else if (paramName.compare ("am_Vn") == 0 || paramName.compare ("am_Vnest") == 0 ||
					 paramName.compare ("am_Vnmden") == 0 || paramName.compare ("am_Vnmhod") == 0)
	{
		initValue = (initValue * 0.01L * kp);
	}
	else if (paramName.compare ("az_nVn") == 0 || paramName.compare ("az_Vn") == 0)
	{
		initValue = (initValue * 0.01L * kp);
	}/* else if (asParamName.compare("az_nV") == 0 || asParamName.compare("az_V") == 0) { // will be processed in the end of function
		initValue = (initValue * kp);
	} else if (asParamName.compare("am_V") == 0 || asParamName.compare("am_Vest") == 0) {
			initValue = (initValue * kp);
	}*/ else if (paramName.compare ("Q") == 0 || paramName.compare ("am_Qmmes") == 0)
	{
		initValue *= 0.001f;
		if (kp >= 10.0L)
		{
			initValue *= kp;
		}
	}
	else if (paramName.compare ("Qmax") == 0)
	{
		if (kp >= 10.0L)
		{
			initValue *= kp;
		}
	}
	else if (paramName.compare ("Qn") == 0)
	{
		if (kp == 10.0L)
		{
			initValue *= 0.01L;
		}
		else if (kp <= 1.0L)
		{
			initValue *= 0.001L;
		}
	}
	else if (paramName.compare ("H2_Qmax") == 0 || paramName.compare ("H2_Qnmax") == 0 ||
					 paramName.compare ("D2_Qmax") == 0 || paramName.compare ("D2_Qnmax") == 0)
	{
		if (kp == 10.0L)
		{
			initValue *= 0.01L;
		}
		else if (kp <= 1.0L)
		{
			initValue *= 0.001L;
		}
	}
	else
	{ //"az_nV" "az_V" "am_V" "am_Vest"
		initValue = (initValue * kp);
	}

	return initValue;
}

// \brief Process parameter from parsed word according to parameter description
// \param commands JSON array of all commands/parameter changed in az archive
// \param rawCommandChangeValue raw value to process
// \return parameter/value processed
RmgStorage::tRmgParams RmgTransaction::processCommandChangeValue (Json::Value& commands, string rawCommandChangeValue)
{
	RmgStorage::tRmgParams paramsRecord;
	int headersize = 2;
	string cmdStr = rawCommandChangeValue.substr (0, headersize);
	string cmdStrUp (cmdStr);

	cmdStrUp[0] = ::toupper (cmdStr[0]);
	string rawValue = rawCommandChangeValue.substr (headersize, rawCommandChangeValue.size () - headersize);
	for (UINT i = 0; i < commands.size (); i++)
	{
		string code = commands[i]["code"].asString ();
		if (code.compare (cmdStr) == 0 || code.compare (cmdStrUp) == 0)
		{
			string name = commands[i]["Name"].asString ();
			RmgRawParam rmgRawParam = processParamValue (commands[i], rawValue, "az_");
			if (cmdStr != cmdStrUp)
				paramsRecord[name + "_old"] = rmgRawParam;
			else
				paramsRecord[name] = rmgRawParam;
			break;
		}
	}
	return paramsRecord;
}

// \brief Process parameter from parsed word according to parameter description
// \param paramDesc description of parameter
// \param rawValues Vector of words with raw parameters value
// \param am_prefix  prefix to be added to parameter name in case of archive processing
// \return parameter/value processed
RmgRawParam RmgTransaction::processParamValue (Json::Value& paramDesc, string rawValue, string am_prefix)
{
	unsigned char dt[8];
	string name = paramDesc["Name"].asString ();
	string vtype = paramDesc["type"].asString ();
	//UINT offset = paramDesc["Offset"].asUInt();
	PARAM_TYPE pType = PARAM_TYPE_STRING;

	//string rawValue = rawValues[offset];
	RmgRawParam rmgRawParam (rawValue);

	if (vtype.compare ("string") == 0)
	{
		int length = paramDesc["length"].asUInt ();
		rmgRawParam.setSize (length);
		int cnt = rawValue.size ();
		int binoffset = 0;
		if (!paramDesc["binoffset"].isNull ())
		{
			binoffset = paramDesc["binoffset"].asUInt ();
			string s = rawValue.substr (binoffset);
			rmgRawParam.setRawValue (s);
		}

	}
	else if (vtype.compare ("integer") == 0)
	{
		int tempInt = ::atoi (rawValue.c_str ());

		pType = PARAM_TYPE_INT;

		rmgRawParam.setInt (tempInt);
		if (name.compare ("CD") == 0)
		{
			getSerialDev ()->setCD (tempInt);
		}
		if (name.compare (am_prefix + "kp") == 0)
		{						// Adding virtual parameter Kp
			double kp = 1.0L;
			RmgRawParam rmgRawParam ("");
			switch (tempInt & 0x07)
			{
			case 1: kp = 0.1L;
				break;
			case 2: kp = 0.01L;
				break;
			case 6: kp = 100.0L;
				break;
			case 7: kp = 10.0L;
				break;
			}
			rmgRawParam.setDouble (kp);
			getStorage ()->set (am_prefix + "Kp", &rmgRawParam);
			RmgRawParam rmgRunMode ("");
			rmgRunMode.setInt (tempInt >> 3);
			getStorage ()->set (am_prefix + "RunMode", &rmgRunMode);
		}
	}
	else if (vtype.compare ("doublefixed") == 0)
	{
		int tempInt = ::atoi (rawValue.c_str ());

		pType = PARAM_TYPE_DOUBLE;

		if (paramDesc["ecorr"].isInt ())
		{
			int ecorr = paramDesc["ecorr"].asInt ();
			double tempDouble = (tempInt * pow (10, ecorr));
			rmgRawParam.setDouble (tempDouble);
		}
		else
		{
			rmgRawParam.setDouble ((double)tempInt);
		}
	}
	else if (vtype.compare ("doublekp") == 0)
	{ // special processing according to kp value
		double tempDouble = ::atof (rawValue.c_str ());

		pType = PARAM_TYPE_DOUBLE;

		double val = convertWithKp (name, tempDouble, am_prefix + "Kp");

		rmgRawParam.setDouble (val);
	}
	else if (vtype.compare ("double") == 0)
	{
		double tempDouble = ::atof (rawValue.c_str ());

		pType = PARAM_TYPE_DOUBLE;

		rmgRawParam.setDouble (tempDouble);
	}
	else if (vtype.compare ("datetime") == 0)
	{

		pType = PARAM_TYPE_DATETIME;

		string tmpstr = rawValue;
		char * datestr = (char*)tmpstr.c_str ();
		int cnt = rawValue.size ();
		int tempInt = ::atoi (datestr + 10);
		dt[5] = tempInt;
		datestr[10] = 0;

		tempInt = ::atoi (datestr + 8);
		dt[4] = tempInt;
		datestr[8] = 0;

		tempInt = ::atoi (datestr + 6);
		dt[3] = tempInt;
		datestr[6] = 0;

		tempInt = ::atoi (datestr + 4);
		dt[0] = tempInt;
		datestr[4] = 0;

		tempInt = ::atoi (datestr + 2);
		dt[1] = tempInt;
		datestr[2] = 0;

		tempInt = ::atoi (datestr);
		dt[2] = tempInt;

		if (name.compare ("Dat") == 0 && am_prefix.empty ())
		{
			getSerialDev ()->setCurrentDateTime (dt);
			char strtag[30];
			sprintf_s (strtag, sizeof (strtag), "%02d/%02d/%04d",
								 dt[2], dt[1], dt[0] + 2000);
			string sval = strtag;
			RmgRawParam rmgDateStr (sval);
			rmgDateStr.setSize (sval.size ());
			rmgDateStr.setQuality (true);
			getStorage ()->set ("DevDate", &rmgDateStr);

			sprintf_s (strtag, sizeof (strtag), "%02d:%02d:%02d",
								 dt[3], dt[4], dt[5]);

			sval = strtag;
			RmgRawParam rmgTimeStr (sval);
			rmgTimeStr.setSize (sval.size ());
			rmgTimeStr.setQuality (true);
			getStorage ()->set ("DevTime", &rmgTimeStr);
		}

		/*		for (int i = cnt - 2; i>=0; i-=2) {
					int tempInt = ::atoi(datestr + i);
					dt[i/2] = tempInt;
					datestr[i] = 0;
				}
		*/
		rmgRawParam.setBinaryData (dt, cnt / 2);
		rmgRawParam.setType (PARAM_TYPE_DATETIME);
	}
	else if (vtype.compare ("hex") == 0)
	{

		pType = PARAM_TYPE_HEX;

		int index = 0;
		char * datestr = (char*)rawValue.c_str ();
		int cnt = rawValue.size ();
		int binoffset = 0;
		if (!paramDesc["binoffset"].isNull ())
		{
			binoffset = paramDesc["binoffset"].asUInt ();
		}
		for (int i = binoffset; i < cnt; i += 2)
		{
			unsigned char tempInt = ((datestr[i] - 0x30) & 0x0f) << 4;
			tempInt |= (datestr[i + 1] - 0x30) & 0x0f;
			dt[index++] = tempInt;
		}
		rmgRawParam.setBinaryData (dt, index);
	}
	return rmgRawParam;
}

// \brief Process parameter from raw binary stream according to parameter description
// \param paramDesc description of parameter
// \param rawValue pointer to char buffer to process
// \param am_prefix  prefix to be added to parameter name in case of archive processing
// \return parameter/value processed
RmgRawParam RmgTransaction::processParamValueBinary (Json::Value& paramDesc, unsigned char* rawValue, string am_prefix)
{
	string name = paramDesc["Name"].asString ();
	string vtype = paramDesc["type"].asString ();
	UINT length = paramDesc["length"].asUInt ();
	PARAM_TYPE pType = PARAM_TYPE_INT;

	RmgRawParam rmgRawParam ("");

	long long tempInt = 0;
	if (rawValue[length - 1] & 0x80)
	{
		if (vtype.compare ("doublefixed") == 0)
			tempInt = -1;
	}
	for (UINT i = 0; i < length; i++)
	{
		tempInt <<= 8;
		tempInt |= rawValue[length - i - 1];
	}
	if (vtype.compare ("integer") == 0)
	{

		pType = PARAM_TYPE_INT;

		rmgRawParam.setInt (tempInt);
		if (name.compare (am_prefix + "kp") == 0)
		{						// Adding virtual parameter Kp
			double kp = 1.0L; //0.1;//1.0L;
			if (tempInt & 0x20)
			{ //changed KP value skip this packet

			}
			RmgRawParam rmgRawParam ("");
			switch (tempInt & 0x07)
			{
			case 1: kp = 0.1L;
				break;
			case 2: kp = 0.01L;
				break;
			case 6: kp = 100.0L;
				break;
			case 7: kp = 10.0L;
				break;
			}
			rmgRawParam.setDouble (kp);
			getStorage ()->set (am_prefix + "Kp", &rmgRawParam);
			RmgRawParam rmgRunMode ("");
			rmgRunMode.setInt (tempInt >> 3);
			getStorage ()->set (am_prefix + "RunMode", &rmgRunMode);
		}
	}
	else if (vtype.compare ("doublefixed") == 0)
	{

		pType = PARAM_TYPE_DOUBLE;
		if (paramDesc["ecorr"].isInt ())
		{
			int ecorr = paramDesc["ecorr"].asInt ();
			double tempDouble = (tempInt * pow (10, ecorr));
			rmgRawParam.setDouble (tempDouble);
		}
		else
		{
			rmgRawParam.setDouble ((double)tempInt);
		}
	}
	else if (vtype.compare ("doublekp") == 0)
	{ // special processing according to kp value
		double tempDouble = (double)tempInt;

		pType = PARAM_TYPE_DOUBLE;

		double val = convertWithKp (name, tempDouble, am_prefix + "Kp");

		rmgRawParam.setDouble (val);
	}
	else if (vtype.compare ("double") == 0)
	{
		float tmpFloat;
		UCHAR* fp = (UCHAR*)&tmpFloat;
		for (UINT i = 0; i < 4; i++)
		{
			fp[i] = rawValue[3 - i];
		}
		pType = PARAM_TYPE_DOUBLE;
		rmgRawParam.setDouble (tmpFloat);
	}
	else if (vtype.compare ("bcd") == 0)
	{
		UINT bcdint = 0;
		pType = PARAM_TYPE_INT;

		for (int i = 1; i < 10000000, tempInt != 0; i *= 10)
		{
			//bcdint *= 10;
			bcdint += i * (tempInt & 0xf);
			tempInt >>= 4;
		}

		rmgRawParam.setInt (bcdint);
	}
	else if (vtype.compare ("datetime") == 0)
	{
		unsigned char databuf[8];
		pType = PARAM_TYPE_DATETIME;

		for (int i = 0; i < length; i++)
		{
			databuf[i] = (rawValue[i] & 0xf) + (rawValue[i] >> 4) * 10;
		}

		databuf[5] = 0;
		struct tm tmtime;

		tmtime.tm_year = 100 + databuf[0];
		tmtime.tm_mon = databuf[1] - 1;
		tmtime.tm_mday = databuf[2];
		tmtime.tm_hour = databuf[3];
		tmtime.tm_min = databuf[4];
		tmtime.tm_sec = 0;
		time_t calcTime = mktime (&tmtime);
		//if (calcTime <= getStartTime()) { // check
		rmgRawParam.setBinaryData (databuf, 6);
		rmgRawParam.setType (PARAM_TYPE_DATETIME);
		//}
	}

	return rmgRawParam;
}

// \brief Process "am" record to separate parameters and store into Archive Storage
// \param params json description of all AM parameters 
// \param paramValuesStrPack vector of string records each of them contains the same parameters list 
void RmgTransaction::processAmRecordAndStore (Json::Value& params, vector<string>& paramValuesStrPack)
{
	vector<RmgStorage::tRmgParams> rmgParamsVec;
	for (UINT pack = 0; pack < paramValuesStrPack.size (); pack++)
	{
		vector<string> paramValuesStr = getSeparatedStrings (paramValuesStrPack[pack], (char*)"\t");
		if (paramValuesStr[0].compare ("EE") == 0 || paramValuesStr[0].compare ("00") == 0)
		{ // Last record or blank record
			continue;
		}
		RmgStorage::tRmgParams paramsRecord;
		for (UINT i = 0; i < params.size (); i++)
		{
			if (params[i]["Offset"].isNull () || params[i]["length"].isNull ())
				continue;
			UINT offset = params[i]["Offset"].asUInt ();
			if (offset >= paramValuesStr.size ())
			{
				continue;
			}

			RmgRawParam rmgRawParam =
				processParamValue (params[i], paramValuesStr[offset], "am_"); // process parameter and store converted values
			if (rmgRawParam.getSize () == 0)
				continue;
			paramsRecord[params[i]["Name"].asString ()] = rmgRawParam;
		}
		rmgParamsVec.push_back (paramsRecord);
	}
	getStorage ()->setArchiveData ("rmg_am", rmgParamsVec); // store record with am name
}

// \brief Process "az" record to separate parameters and store into Archive Storage
// \param params json description of all AZ parameters 
// \param paramValuesStrPack vector of string records each of them contains the same parameters list 
void RmgTransaction::processAzRecordAndStore (Json::Value& params, vector<string>& paramValuesStrPack)
{
	vector<RmgStorage::tRmgParams> rmgParamsVec;
	Json::Value commands = getCommandsDescription ();
	for (UINT pack = 0; pack < paramValuesStrPack.size (); pack++)
	{
		// process not words separated by \t, but binary offset basis 
		RmgStorage::tRmgParams paramsRecord;
		for (UINT i = 0; i < params.size (); i++)
		{
			if (params[i]["boffset"].isNull () || params[i]["length"].isNull ())
				continue;
			UINT boffset = params[i]["boffset"].asUInt ();
			UINT length = params[i]["length"].asUInt ();
			string name = params[i]["Name"].asString ();
			string rawValue = paramValuesStrPack[pack].substr (boffset, length);// [offset];

			RmgRawParam rmgRawParam;
			if (name.compare ("az_param") != 0)
			{
				rmgRawParam = processParamValue (params[i], rawValue, "az_"); // process parameter and store converted values
				if (rmgRawParam.getSize () == 0)
					continue;
				paramsRecord[params[i]["Name"].asString ()] = rmgRawParam;
			}
			else
			{
				vector<string> allChangedParameters = getSeparatedStrings (rawValue, (char*)"\t");
				for (UINT chParmIndx = 0; chParmIndx < allChangedParameters.size (); chParmIndx++)
				{
					int headersize = 2;
					string cmdStr = allChangedParameters[chParmIndx].substr (0, headersize);
					//RmgRawParam rmgRawParam(allChangedParameters[chParmIndx].substr(headersize, rawCommandChangeValue.size()-headersize));
					RmgStorage::tRmgParams addedRec =	processCommandChangeValue (commands, allChangedParameters[chParmIndx]);
					if (addedRec.size () != 0)
						paramsRecord.insert (addedRec.begin (), addedRec.end ());
				}
			}
		}
		rmgParamsVec.push_back (paramsRecord);
	}
	getStorage ()->setArchiveData ("rmg_az", rmgParamsVec);
}

// \brief Process "as" record to separate parameters and store into Archive Storage
// \param params json description of all AS parameters 
// \param paramValuesStrPack vector of string records each of them contains the same parameters list 
void RmgTransaction::processAsRecordAndStore (Json::Value& params, vector<string>& paramValuesStrPack)
{
	vector<RmgStorage::tRmgParams> rmgParamsVec;
	Json::Value jsonStatus = getStatusDescription ();
	for (UINT pack = 0; pack < paramValuesStrPack.size (); pack++)
	{
		// process not words separated by \t, but binary offset basis 
		//vector<string> allChangedParameters; 
		RmgStorage::tRmgParams paramsRecord;
		for (UINT i = 0; i < params.size (); i++)
		{
			if (params[i]["boffset"].isNull () || params[i]["length"].isNull ())
				continue;
			UINT boffset = params[i]["boffset"].asUInt ();
			UINT length = params[i]["length"].asUInt ();
			string name = params[i]["Name"].asString ();
			string rawValue = paramValuesStrPack[pack].substr (boffset, length);

			RmgRawParam rmgRawParam;
			rmgRawParam = processParamValue (params[i], rawValue, "as_"); // process parameter and store converted values
			if (rmgRawParam.getSize () == 0)
				continue;
			if (name.compare ("as_status") == 0)
			{
				UINT intStatus;
				if (!rmgRawParam.getInt ((int*)&intStatus))
					continue;
				for (UINT statusIndx = 0; statusIndx < jsonStatus.size (); statusIndx++)
				{
					string bittype = jsonStatus[statusIndx]["type"].asString ();
					if (bittype.compare ("bitmask") != 0)
						continue;
					const char* bitmask = jsonStatus[statusIndx]["value"].asCString ();
					UINT mask = stoul (bitmask, 0, 16);
					string bitname = jsonStatus[statusIndx]["Name"].asString ();

					if (intStatus & mask)
					{
						RmgRawParam rmgRawParamVirtual;
						rmgRawParamVirtual.setInt (1);
						paramsRecord[bitname] = rmgRawParamVirtual;
					}
				}
			}
			else
			{
				paramsRecord[params[i]["Name"].asString ()] = rmgRawParam;
			}

		}
		rmgParamsVec.push_back (paramsRecord);
	}
	getStorage ()->setArchiveData ("rmg_as", rmgParamsVec);
}

// \brief Process "nm" record to separate parameters and store into Archive Storage
// \param params json description of all nm parameters (even alues - datetime, 
//				first 10 odd values - pressure, last 10 odd values - temperature)
// \param paramValuesStrPack vector of string records each of them contains the same parameters list 
void RmgTransaction::processNmRecordAndStore (Json::Value& params, vector<string>& paramValuesStrPack)
{
	vector<RmgStorage::tRmgParams> rmgParamsVec;
	RmgStorage::tRmgParams paramsRecord;
	for (UINT pack = 0; pack < paramValuesStrPack.size (); pack++)
	{
		RmgRawParam rmgRawParam;
		int paramIndx = 0;
		if ((pack & 0x1) != 0)
		{
			if (pack < 20)
				paramIndx = 1;
			else
				paramIndx = 2;
		}
		rmgRawParam = processParamValue (params[paramIndx], paramValuesStrPack[pack], "nm_"); // process parameter and store converted values
		paramsRecord[params[paramIndx]["Name"].asString ()] = rmgRawParam;
		if (paramsRecord.size () == 2)
		{
			rmgParamsVec.push_back (paramsRecord);
			paramsRecord.clear ();
		}
	}
	getStorage ()->setArchiveData ("rmg_nm", rmgParamsVec); // store record with am name
}

// \brief Process standard record (se, mi, pa, ...) record to separate parameters and store into Storage
// \param params json description of all parameters in record 
// \param strtoParse raw parameters list  
void RmgTransaction::processParametersAndStore (Json::Value& params, vector<string>& paramValuesStr)
{
	//vector<string> paramValuesStr = getSeparatedStrings(strToParse, "\t");
	for (UINT i = 0; i < params.size (); i++)
	{
		Json::Value Offset = params[i]["Offset"];
		//if (params[i]["Offset"].isNull () || params[i]["length"].isNull ())
		if (Offset.isNull () || params[i]["length"].isNull ())
			continue;
		UINT offset = Offset.asUInt ();//UINT offset = params[i]["Offset"].asUInt ();
		if (offset >= paramValuesStr.size ())
		{
			continue;
		}
		RmgRawParam rmgRawParam = processParamValue (params[i], paramValuesStr[offset], ""); // process parameter and store converted values
		if (rmgRawParam.getSize () == 0)
			continue;
		// Calculating raising values for current day
		string paramName = params[i]["Name"].asString ();
		if (paramName.compare ("t") == 0)
		{
			RmgRawParam rmgParamNewVirtual;
			double tempK;
			if (rmgRawParam.getDouble (&tempK))
			{
				rmgParamNewVirtual.setDouble (tempK - 273.15);
				getStorage ()->set (paramName + "_C", (void*)&rmgParamNewVirtual);
			}
		}
		if (paramName.compare ("V") == 0 || paramName.compare ("Vn") == 0 ||
				paramName.compare ("Vnest") == 0 || paramName.compare ("Vest") == 0)
		{
			string strToday = "D2_" + paramName + "_today";
			string strVirtParam = paramName + "_raising";
			RmgRawParam* rmgParamToday = (RmgRawParam*)getStorage ()->get (strToday);
			RmgRawParam* rmgParamVirtual = (RmgRawParam*)getStorage ()->get (strVirtParam);
			double actualValue, oldValue;
			if (rmgParamToday == 0 || !rmgRawParam.getDouble (&actualValue) ||
					!rmgParamToday->getDouble (&oldValue))
			{
				if (rmgParamVirtual != 0)
					rmgParamVirtual->setQuality (FALSE);
			}
			else
			{
				// Creating new virtual parameter = diff (current value - startday value)
				RmgRawParam rmgParamNewVirtual;
				rmgParamNewVirtual.setDouble (actualValue - oldValue);
				getStorage ()->set (strVirtParam, (void*)&rmgParamNewVirtual);
			}
		}
		getStorage ()->set (paramName, &rmgRawParam);
	}
}

// \brief Get date and time from storage according to specified prefix 
// \param reqPrefix prefix _"time" to get time from storage
// \param reqPostfix postfix _"time" to get time from storage
time_t RmgTransaction::getTimeFromStorage (string reqPrefix, string reqPostfix = "")
{
	struct tm tmtime;
	UCHAR value[10];
	if (getStorage ()->getValue (reqPrefix + "_date" + reqPostfix, value) == 0)
		return 0;
	tmtime.tm_year = 100 + value[0];
	tmtime.tm_mon = value[1] - 1;
	tmtime.tm_mday = value[2];
	tmtime.tm_hour = value[3];
	tmtime.tm_min = value[4];
	tmtime.tm_sec = 0;
	time_t calcTime = mktime (&tmtime);
	return calcTime;
}

// \brief Get date and time from storage according to specified prefix 
// \param reqPrefix prefix _"time" to get time from storage
// \param add_minute add minutes to saved time
void RmgTransaction::saveTimeFromStorage (string reqPrefix, int add_minute)
{
	struct tm tmtime;
	time_t calcTime;
	/*int value;
	if(getStorage()->getValue(reqPrefix + "_year", (UCHAR*)&value) == 0) // H1_start
		return;
	tmtime.tm_year = 100 + value;
	if(getStorage()->getValue(reqPrefix + "_month", (UCHAR*)&value) == 0)
		return;
	tmtime.tm_mon = value-1;
	if(getStorage()->getValue(reqPrefix + "_day", (UCHAR*)&value) == 0)
		return;
	tmtime.tm_mday = value;
	if(getStorage()->getValue(reqPrefix + "_hour", (UCHAR*)&value) == 0)
		return;
	tmtime.tm_hour = value;
	if(getStorage()->getValue(reqPrefix + "_minute", (UCHAR*)&value) == 0)
		return;
	tmtime.tm_min = value;
	tmtime.tm_sec = 0;
	calcTime = mktime(&tmtime);
	*/
	calcTime = getTimeFromStorage (reqPrefix);
	if (calcTime == 0)
		return;
	calcTime += add_minute * 60;
	if (reqPrefix[0] == 'H')
	{
		if (calcTime - getSerialDev ()->getHTime () > 0)
			getSerialDev ()->setHTime (calcTime);
	}
	else
	{
		if (calcTime - getSerialDev ()->getDTime () > 0)
			getSerialDev ()->setDTime (calcTime);
	}
}

// \brief Process input buffer got from serial line in case of H1, H2, D1, D2 commands 
// \param size bytes got from serial line
// \return error code (not ready, error, ok)
RESP_RETVAL RmgTransaction::processBinaryResponse (DWORD size)
{
	UINT newsize = 0;
	UINT normalRecordSize = 0, extendedRecordSize = 0;
	UINT numRecords = 0;
	vector<RmgStorage::tRmgParams> rmgParamsVec;
	RmgStorage::tRmgParams paramsRecord;
	// make bytes replace for special symbols 0x1b 0x0e and 0x1b 0x1b
	for (UINT i = 0; i < size; i++)
	{
		if (cpBufRX[i] == 0x1b)
		{
			i++;
			if (cpBufRX[i] == 0x0E)
				cpBufRX[newsize++] = 0x0D;
			else
				cpBufRX[newsize++] = 0x1b;
		}
		else
		{
			if (newsize != i)
				cpBufRX[newsize] = cpBufRX[i];
			newsize++;
		}
	}
	USHORT crcResp = (*(UCHAR*)(cpBufRX + (newsize - 3)) << 8) + *(UCHAR*)(cpBufRX + (newsize - 2));					// get crc from the stream
	UCHAR bufSize = *(UCHAR*)(cpBufRX + (newsize - 4));						// get actual data size
	USHORT crcCalc = CRC ((unsigned char*)cpBufRX, newsize - 3, 0x8005, 0x0000, true);		// calculated CRC16(0x8005,0) 
	UCHAR dh_num = *(UCHAR*)(cpBufRX + 5);
	if (crcCalc != crcResp)
	{
		Mess->Out ("Can't process binary response for 0x%x cmd: bad CRC.", _command);
		return RESP_RETVAL::FAIL;
	}
	Json::Value dataDescription = _jsonRequest["ParamsInList"]["data"];
	if (!_jsonRequest["recsize"].isNull ())
	{
		normalRecordSize = _jsonRequest["recsize"].asUInt ();
	}
	if (!_jsonRequest["exrecsize"].isNull ())
	{
		extendedRecordSize = _jsonRequest["exrecsize"].asUInt ();
	}

	if ((normalRecordSize != 0) && (bufSize % normalRecordSize == 0))
	{
		numRecords = bufSize / normalRecordSize;
	}
	else if ((extendedRecordSize != 0) && (bufSize % extendedRecordSize == 0))
	{
		numRecords = bufSize / extendedRecordSize;
		normalRecordSize = extendedRecordSize;
	}
	else
	{
		Mess->Out ("Wrong requests/response configuration 0x%x.", _command);
		return RESP_RETVAL::FAIL;
	}
	if (dh_num == '1')
	{ // first and last dates
// save date and times to main storage
		for (UINT i = 0; i < dataDescription.size (); i++)
		{
			UINT offset = dataDescription[i]["Offset"].asUInt ();
			if (offset > normalRecordSize + 6) // start of data block
				continue;
			RmgRawParam rmgRawParam =
				processParamValueBinary (dataDescription[i],
				(unsigned char*)cpBufRX + offset, _transactionName + "_");
			if (rmgRawParam.getSize () == 0)
				continue;
			getStorage ()->set (dataDescription[i]["Name"].asString (), &rmgRawParam);
		}

		saveTimeFromStorage (_transactionName + "_start", 0);
	}
	else
	{ // Process D2 and H2 responses
		time_t currentDevTime;
		currentDevTime = getSerialDev ()->getCurrentDateTime ();
		// save all data to archive
		if (bufSize == 0)
		{
			setNeedContinuation (false);
			return RESP_RETVAL::SUCCESS;
		}
		else
			setNeedContinuation (true);
		int bad_record_flag;
		for (UINT recIndx = 0; recIndx < numRecords; recIndx++)
		{
			bad_record_flag = 0;
			for (UINT i = 0; i < dataDescription.size (); i++)
			{
				UINT offset = dataDescription[i]["Offset"].asUInt ();
				string btype = dataDescription[i]["type"].asString ();
				if (offset > normalRecordSize + 6)
				{// start of data block
					bad_record_flag = 1;
					break;//continue;
				}
				RmgRawParam rmgRawParam =
					processParamValueBinary (dataDescription[i],
					(unsigned char*)cpBufRX + recIndx * normalRecordSize + offset, _transactionName + "_");
				if (rmgRawParam.getSize () == 0)
				{
					bad_record_flag = 1;
					break; //continue;
				}
				paramsRecord[dataDescription[i]["Name"].asString ()] = rmgRawParam;
				// check start day event
				/*if (dataDescription[i]["Name"].asString().compare("D2_hour") == 0){
				}
				*/
				/*if (getTypeByString(btype) == PARAM_TYPE_BCD) // save time values to storage
					getStorage()->set(dataDescription[i]["Name"].asString(), &rmgRawParam);
					*/
				if (getTypeByString (btype) == PARAM_TYPE_DATETIME) // save time values to storage 
					getStorage ()->set (dataDescription[i]["Name"].asString (), &rmgRawParam);
			}

			//saveTimeFromStorage(_transactionName, 0);
			if (!bad_record_flag)
			{
				if (_transactionName[0] == 'D')
				{
					//time_t archiveTime = getSerialDev()->getDTime();
					time_t archiveTime = getTimeFromStorage (_transactionName);
					if (currentDevTime - archiveTime < 24 * 60 * 60)
					{
						time_t tToday = getTimeFromStorage (_transactionName, "_today");
						time_t tYest = getTimeFromStorage (_transactionName, "_yest");
						if (tYest != 0)
							Mess->Out ("Yesterday: %s", ctime (&tYest));
						if (tToday != 0)
							Mess->Out ("Today: %s", ctime (&tToday));
						//saving values for start current day
						for (RmgStorage::tRmgParamsIt recParIt = paramsRecord.begin (); recParIt != paramsRecord.end (); recParIt++)
						{
							string nameToday = recParIt->first + "_today";
							string nameYest = recParIt->first + "_yest";
							RmgRawParam * tCurParam = (RmgRawParam *)getStorage ()->get (nameToday);
							if (tCurParam != 0 && tYest != 0 && tToday != 0 && tYest != tToday)
							{ // save _today parameters as _yest
								getStorage ()->set (nameYest, (void*)(tCurParam));
							}
							getStorage ()->set (nameToday, (void*)&(recParIt->second));
							if (nameYest.compare ("D2_V_yest") == 0)
							{
								RmgRawParam * tYestParam = (RmgRawParam *)getStorage ()->get (nameYest);
								if (tYestParam != 0)
								{
									double v1 = 0, v2 = 0;
									tYestParam->getDouble (&v1);
									recParIt->second.getDouble (&v2);
									v2 -= v1;
									RmgRawParam newpar;
									newpar.setDouble (v2);
									getStorage ()->set ("Q_yest", (void*)(&newpar));
									Mess->Out ("Q_yest: %f, %f", v2, v1);
								}
							}
							else if (nameYest.compare ("D2_Vn_yest") == 0)
							{
								RmgRawParam * tYestParam = (RmgRawParam *)getStorage ()->get (nameYest);
								if (tYestParam != 0)
								{
									double v1 = 0, v2 = 0;
									tYestParam->getDouble (&v1);
									recParIt->second.getDouble (&v2);
									v2 -= v1;
									RmgRawParam newpar;
									newpar.setDouble (v2);
									getStorage ()->set ("Qn_yest", (void*)(&newpar));
									Mess->Out ("Qn_yest: %f, %f", v2, v1);
								}
							}
							else if (nameYest.compare ("D2_Vest_yest") == 0)
							{
								RmgRawParam * tYestParam = (RmgRawParam *)getStorage ()->get (nameYest);
								if (tYestParam != 0)
								{
									double v1 = 0, v2 = 0;
									tYestParam->getDouble (&v1);
									recParIt->second.getDouble (&v2);
									v2 -= v1;
									RmgRawParam newpar;
									newpar.setDouble (v2);
									getStorage ()->set ("Qest_yest", (void*)(&newpar));
									Mess->Out ("Qest_yest: %f, %f", v2, v1);
								}
							}
							else if (nameYest.compare ("D2_Vnest_yest") == 0)
							{
								RmgRawParam * tYestParam = (RmgRawParam *)getStorage ()->get (nameYest);
								if (tYestParam != 0)
								{
									double v1 = 0, v2 = 0;
									tYestParam->getDouble (&v1);
									recParIt->second.getDouble (&v2);
									v2 -= v1;
									RmgRawParam newpar;
									newpar.setDouble (v2);
									getStorage ()->set ("Qnest_yest", (void*)(&newpar));
									Mess->Out ("Qnest_yest: %f, %f", v2, v1);
								}
							}
						}
					}
					else if (currentDevTime - archiveTime < 2 * 24 * 60 * 60)
					{
						//saving values for the previous day
						for (RmgStorage::tRmgParamsIt recParIt = paramsRecord.begin (); recParIt != paramsRecord.end (); recParIt++)
						{

							string nameYest = recParIt->first + "_yest";
							getStorage ()->set (nameYest, (void*)&(recParIt->second));
						}
					}
				}

				// bad fix rmgParamsVec.push_back(paramsRecord);
			}
			/*else{
				time_t t;
				if(IS_RMG_H_COMMAND(_command)){
					t = getSerialDev()->getHTime();
					getSerialDev()->setHTime(t - 60*60); //try to request -1 hour
					paramsRecord.clear();
					break;
				} else if(IS_RMG_D_COMMAND(_command)){
					t = getSerialDev()->getDTime();
					getSerialDev()->setDTime(t - 24*60*60); //try to request -24 hour
					paramsRecord.clear();
					break;
				}
			} // bad fix
			*/
			rmgParamsVec.push_back (paramsRecord); // bad fix
			paramsRecord.clear ();
		}
		int minutes_to_add = 5;
		if (_transactionName[0] == 'D')
		{
			minutes_to_add = 24 * 60;
		}

		saveTimeFromStorage (_transactionName, minutes_to_add);

		getStorage ()->setArchiveData ("rmg_" + _transactionName, rmgParamsVec);
	}

	return RESP_RETVAL::SUCCESS;
}

// \brief Process input buffer got from serial line
// \param size bytes got from serial line
// \return error code enum _PROC_RESP_RETVAL (not ready, error, ok)
RESP_RETVAL RmgTransaction::processResponse (DWORD size)
{
	char* pStartBlock = (char*)cpBufRX;
	char* pStartBlockName;
	char* pEndBlock;
	//int blocksProcessed = 0;

	cpBufRX[size] = 0;
	//DWORD ticks = ::GetTickCount ();
	//Sleep (100);	//TODO � ������
	auto Dev = getSerialDev ();

	if (_8bit_transaction)
	{
		LogRX (size, Dev);
		void* foundch = memchr (cpBufRX, '\r', size);
		if (foundch == 0)
			return RESP_RETVAL::NOTREADY;
		return processBinaryResponse ((UINT)foundch - (UINT)cpBufRX + 1);
	}

	Conv7to8byt (size);									// fixing 8n1 to 7e1 translation
	LogRX (size, Dev);

	if (strchr (pStartBlock, '!') == 0)	//strchr looking for end of transfer
		return RESP_RETVAL::NOTREADY;

	Json::Value::Members memb = _jsonRequest["ParamsInList"].getMemberNames ();
	Json::Value::Members::iterator membIt;

	while (1)
	{
		pStartBlock = strchr (pStartBlock, '(');
		if (pStartBlock == 0)
			break;
		pEndBlock = strchr (pStartBlock, ')');
		if (pEndBlock == 0)
			break; 			//return RESP_RETVAL::NOTREADY;// break; TODO RESTORE the next?break; //TODO delete ?

		string strToParse (pStartBlock, pEndBlock + 5); // from start to ')crcv\r'
		//remove0D0A(strToParse);
		pStartBlockName = pStartBlock - 2;
		string blockNameStr (pStartBlockName, pStartBlock);

		if (!checkResponseCRC (strToParse))							// checking CRC value
		{
			Mess->Out ("[%s]\tRX: (%s)[%c%c] bad CRC", asParamName.c_str (),
						Dev->getPort (), _command & 0xff, _command >> 8);// blockNameStr.c_str ());
			return RESP_RETVAL::FAIL;
		}
		membIt = find (memb.begin (), memb.end (), blockNameStr);
		if (membIt != memb.end ())
		{
			Json::Value params = _jsonRequest["ParamsInList"][(*membIt)];
			// calculate Password string according to data specified in last se package
			if (TheComm == TRANSACT::WRITE && _command == RMG_R1_COMMAND && blockNameStr.compare ("se") == 0)
			{
				memcpy (pEndBlock + 1, _writePassword.c_str (), 5);	// 5 - size of password
				USHORT wrpass = CRC ((unsigned char *)pStartBlock, strToParse.size () + 1, 0x8005, 0);
				string val = CRCtoASCII (wrpass);
				Dev->setPassword (val + val[1]);
				//Transaction *Tr;
				//Dev->CreateCommByWr (Tr, "W1", asParamName);
				//Tr->setWriteParameter (asParamName, caValue, strnlen_s (caValue, SIZE_VAL_MAX));
				break; //TODO delete ?
			}
			strToParse.erase (0, 1);											// delete "(" in the begin
			strToParse.erase (strToParse.size () - 5, 5);	// delete ")CRC" in the end

			vector<string> paramValuesStrPack;
			if (blockNameStr.compare ("am") == 0)
			{ // process as archive
				paramValuesStrPack = getSeparatedStrings (strToParse, (char*)"\r\n");
				processAmRecordAndStore (params, paramValuesStrPack);
			}
			else if (blockNameStr.compare ("az") == 0)
			{
				paramValuesStrPack = getSeparatedStrings (strToParse, (char*)"\r\n");
				processAzRecordAndStore (params, paramValuesStrPack);
			}
			else if (blockNameStr.compare ("as") == 0)
			{
				paramValuesStrPack = getSeparatedStrings (strToParse, (char*)"\t");
				processAsRecordAndStore (params, paramValuesStrPack);
			}
			else if (blockNameStr.compare ("nm") == 0)
			{
				paramValuesStrPack = getSeparatedStrings (strToParse, (char*)"\t");
				processNmRecordAndStore (params, paramValuesStrPack);
			}
			else if ((blockNameStr.compare ("ms") == 0 && _command == RMG_W0_COMMAND))
			{
#ifdef _DEBUG
				Mess->Out ("SendHeadOK");
#endif // DEBUG
				StateWr = StateWR::SendHeadOK;
				return RESP_RETVAL::SUCCESS;
			}
			else if ((blockNameStr.compare ("mw") == 0) && _command == RMG_W0_COMMAND)
			{
				if (StateWr == StateWR::SendHeadOK)									// ���� ��� ������� ����� �� ������ ������ ���
				{
#ifdef _DEBUG
					Mess->Out ("SendDataWrOK");
#endif // DEBUG
					//Transaction *Tr;
					//Dev->CreateCommByWr (Tr, "R4", asParamName);
					return RESP_RETVAL::SUCCESS;
				}
#ifdef _DEBUG
				Mess->Out ("[%s]\t[%c%c] REWRITE", asParamName.c_str (), _command & 0xff, _command >> 8);
#endif // DEBUG
				// ���� ������ ��������� �� ������ ������ �� �������
				if (Dev->getStorage ()->getConnectionStatus ())
				{
					StateWr = StateWR::NoSendYet;
					//������� R1 � W0 ��� ��������� ������ 
					//Transaction *Tr;
					//Dev->CreateCommByWr (Tr, "R1", asParamName);
					/*Dev->CreateCommByWr (Tr, "W0", asParamName);
					Tr->setWriteParameter (asParamName, caValue, strnlen_s (caValue, SIZE_VAL_MAX));*/
					return RESP_RETVAL::REPEAT; //REWRITE
				}
				return RESP_RETVAL::FAIL;
			}
//			else if ((blockNameStr.compare ("ms") == 0 || blockNameStr.compare ("mw") == 0) &&
//							 (_command == RMG_W0_COMMAND || _command == RMG_W1_COMMAND))
//			{
//#ifdef _DEBUG
//				LOGF (Dev->getName (), "WR");
//#endif // DEBUG
//				return RESP_RETVAL::SUCCESS;
//			}
			else
			{
				paramValuesStrPack = getSeparatedStrings (strToParse, (char*)"\t");
				processParametersAndStore (params, paramValuesStrPack);
			}
		}
		else
		{
			Mess->Out ("Can't process %s subpacket.", blockNameStr.c_str ());
			if ((blockNameStr.compare ("mw") == 0) && _command == RMG_R1_COMMAND)
			{
				if (Dev->getStorage ()->getConnectionStatus ())
					return RESP_RETVAL::REPEAT;
			}
		}
		pStartBlock = pEndBlock + 1;
		//blocksProcessed++;
		//LOG("Processing input buffer '%s' successfully done in %lu ms.\n", blockNameStr.c_str(), ::GetTickCount() - ticks);
		//ticks = ::GetTickCount();
	}
	//	LOG("Processing input buffer successfully done in %lu ms.\n", ::GetTickCount() - ticks);
	return RESP_RETVAL::SUCCESS;
}
///____________________________________________________________________________
//������������� 7-������ ������� � 8 ������
void RmgTransaction::Conv7to8byt (const DWORD size)
{
	for (int i = 0; i < size; i++)
	{
		cpBufRX[i] = cpBufRX[i] & 0x7f;
	}
}
///____________________________________________________________________________

void RmgTransaction::LogRX (const DWORD &size, SerialDev* Dev)
{
	int iLenOut;
#ifdef _DEBUG
	iLenOut = 30;
#else
	iLenOut = 3;
#endif // _DEBUG
	Mess->Out ("[%s]\tRX[%c%c](%s)(%d) %s", asParamName.c_str (), _command & 0xff, _command >> 8, Dev->getPort (), size,
				/*getSerialDev()->getAddress(), */Mess->CharArrToCharAsInt (cpBufRX, iLenOut)/*, cpBufRX*/);
}
///____________________________________________________________________________


