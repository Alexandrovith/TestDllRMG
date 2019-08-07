#include "WrConstParam.h"
#include <iostream>
#include <memory>
#include <timeapi.h>
#pragma comment( lib, "winmm.lib")

//static char* capTagWr[] = {
//	//"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"W1\",\"ParamName\":\"kCO2\"}"
//	//,"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"W1\",\"ParamName\":\"kN2\"}"
//	//,"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"W1\",\"ParamName\":\"kd\"}"
//	//,"{\"DeviceName\":\"ЕС605_Ясень\",\"RequestName\":\"W1\",\"ParamName\":\"Dat\"}"
//"{\"DeviceName\":\"SF ГРС Добруш\",\"RequestName\":\"R47\",\"ParamName\":\"Differential_Pressure_value\",\"ParamsOutList\":[{\"Name\":\"SuperFlo_Write_Password\",\"Value\":\"%SuperFlo_Write_Password%\"},{\"Name\":\"RunNumber\",\"Value\":\"1\"}]}",
//"{\"DeviceName\":\"SF ГРС Добруш\",\"RequestName\":\"R47\",\"ParamName\":\"Static_Pressure_Value\",\"ParamsOutList\":[{\"Name\":\"SuperFlo_Write_Password\",\"Value\":\"%SuperFlo_Write_Password%\"},{\"Name\":\"RunNumber\",\"Value\":\"1\"}]}",
//"{\"DeviceName\":\"SF ГРС Добруш\",\"RequestName\":\"R47\",\"ParamName\":\"Temperature_Value\",\"ParamsOutList\":[{\"Name\":\"SuperFlo_Write_Password\",\"Value\":\"%SuperFlo_Write_Password%\"},{\"Name\":\"RunNumber\",\"Value\":\"1\"}]}",
//"{\"DeviceName\":\"SF ГРС Добруш\",\"RequestName\":\"R46\",\"ParamName\":\"Differential_Pressure_Flag\",\"ParamsOutList\":[{\"Name\":\"RunNumber\",\"Value\":\"1\"}]}",
//"{\"DeviceName\":\"SF ГРС Добруш\",\"RequestName\":\"R46\",\"ParamName\":\"Static_Pressure_Flag\",\"ParamsOutList\":[{\"Name\":\"RunNumber\",\"Value\":\"1\"}]}",
//"{\"DeviceName\":\"SF ГРС Добруш\",\"RequestName\":\"R46\",\"ParamName\":\"Temperature_Flag\",\"ParamsOutList\":[{\"Name\":\"RunNumber\",\"Value\":\"1\"}]}"
//};

CWrConstParam::CWrConstParam (CLoadDLL::DWriteValue WriteValue, CLoadDLL::DGetValue GetValue, char** caTagWr) :
	capTagWr (caTagWr)
	, WriteValue (WriteValue)
	, GetValue (GetValue)
{
	for (auto Tag : iaTagsRdOfConstPar)
		Tag = 0;
	for (auto Tag : iaTagsWrOfConstPar)
		Tag = 0;
}
//_____________________________________________________________________________

CWrConstParam::~CWrConstParam ()
{
}
//_____________________________________________________________________________

void CWrConstParam::GetIdOfConstParam (char* cpTag, int NumValByWr)
{
	FindPair->InitTag (cpTag);
	string RequestName = FindPair->operator[]("RequestName");
	string ParamName = FindPair->operator[]("ParamName");
	if (RequestName == "R46")
	{
		if (ParamName == "Differential_Pressure_Value")
			iaTagsRdOfConstPar[(int)EParam::Differential_Pressure_Value] = NumValByWr;
		else if (ParamName == "Static_Pressure_Value")
			iaTagsRdOfConstPar[(int)EParam::Static_Pressure_Value] = NumValByWr;
		else if (ParamName == "Temperature_Value")
			iaTagsRdOfConstPar[(int)EParam::Temperature_Value] = NumValByWr;
		else if (ParamName == "Differential_Pressure_Flag")
			iaTagsRdOfConstPar[(int)EParam::Differential_Pressure_Flag] = NumValByWr;
		else if (ParamName == "Static_Pressure_Flag")
			iaTagsRdOfConstPar[(int)EParam::Static_Pressure_Flag] = NumValByWr;
		else if (ParamName == "Temperature_Flag")
			iaTagsRdOfConstPar[(int)EParam::Temperature_Flag] = NumValByWr;
	}
	else if (RequestName == "R47")
	{
		if (ParamName == "Differential_Pressure_Value")
			iaTagsWrOfConstPar[(int)EParam::Differential_Pressure_Value] = NumValByWr;
		else if (ParamName == "Static_Pressure_Value")
			iaTagsWrOfConstPar[(int)EParam::Static_Pressure_Value] = NumValByWr;
		else if (ParamName == "Temperature_Value")
			iaTagsWrOfConstPar[(int)EParam::Temperature_Value] = NumValByWr;
		else if (ParamName == "Differential_Pressure_Flag")
			iaTagsWrOfConstPar[(int)EParam::Differential_Pressure_Flag] = NumValByWr;
		else if (ParamName == "Static_Pressure_Flag")
			iaTagsWrOfConstPar[(int)EParam::Static_Pressure_Flag] = NumValByWr;
		else if (ParamName == "Temperature_Flag")
			iaTagsWrOfConstPar[(int)EParam::Temperature_Flag] = NumValByWr;
	}
	else if (RequestName == "R43")
	{
		if (ParamName == "Tap_Location")
			iaTagsWrOfConstPar[(int)EParam::Tap_Location] = NumValByWr;
	}
	else if (RequestName == "R42")
	{
		if (ParamName == "Tap_Location")
			iaTagsRdOfConstPar[(int)EParam::Tap_Location] = NumValByWr;
	}

}
//_____________________________________________________________________________

float fValues[] = {  4235.5, 27.0, 1.214, 0.048 };
char cpFlag[] = { 1,1,1 };

void CWrConstParam::WrValue (EParam ParName, int iSizeVal)
{
	char caBuf[64];
	std::cout << "Запись " << ParName.ToString ().c_str () << "[";
	if (iSizeVal == 4) //if (ParName.ToInt() < EParam::Static_Pressure_Flag)
	{
		char caVal[4];
		*((float*)caVal) = fValues[ParName.ToInt ()];
		WriteValue (capTagWr[iaTagsWrOfConstPar[ParName.ToInt ()]], caVal, sizeof (caVal));

		OutBuf (caVal, 4);
	}
	else
	{
		int iParName = ParName.ToInt ();
		int iParNum = (int)(EParam::Differential_Pressure_Flag - iParName);
		char caVal[1];
		caVal[0] = (char)cpFlag[iParNum];
		WriteValue (capTagWr[iaTagsWrOfConstPar[iParName]], caVal, sizeof (caVal));
		OutBuf (caVal, 1);
	}
}
//_____________________________________________________________________________	

void CWrConstParam::OutBuf (char* caVal, int iSize)
{
	char caBuf[64];
	for (int i = 0; i < iSize; i++)
	{
		_itoa_s ((int)caVal[i], caBuf, sizeof (caBuf), 16);
		std::cout << " " << caBuf;
	}
	std::cout << "]" << std::endl;
}
//_____________________________________________________________________________

const int TIME_WAIT_REQUEST = 10 * 1000;

int CWrConstParam::Process ()
{
	std::cout << "e - выход, r - повторить чтение, w - начать запись. Ожидание..." << std::endl;
	for (int  i = 0; i < 3; i++)
	{
		std::cout << ".";
		Sleep (1000);
	}
	std::cout << "Ожидание завершено. Чтение" << std::endl;
	bool retflag;
	int retval = ReadParams (retflag);
	if (retflag) return retval;
		std::cout << "Запись" << std::endl;
	// Выводим значения и заданного списка параметров 
	//for (EParam::Epar Param = EParam::Differential_Pressure_Value; Param < EParam::Epar::SIZE; ++Param)
	//{
	//	unsigned char* ucpBuf, ucA1[1] = { 0 }, ucA4[4] = { 0 };
	//	int iSize;
	//	if (Param < EParam::Differential_Pressure_Flag)
	//	{
	//		ucpBuf = ucA4;
	//		iSize = 4;
	//	}
	//	else
	//	{
	//		ucpBuf = ucA1;
	//		iSize = 1;
	//	}
	//	GetValue (iaTagsRdOfConstPar[(int)Param], ucpBuf);
	//	OutRecieveVal (EParam::ToString(Param), iSize, ucpBuf);
	//}
	// Цикл записи-чтения параметров 
	for (;;)
	{
		int iSizeVal = sizeof (float); EParam ParName;
		char Key;
		std::cin >> Key;

		switch (Key)
		{
		case 0x20: return 1;		// space  - новый цикл 			//case 10: return 0;			// Enter - конец работы 
		case 'e': case 'E': case 227: case 147: return 0;			// e - конец работы 
		case (char)EParam::Static_Pressure_Value + 0x30: ParName = EParam::Static_Pressure_Value;  break;
		case (char)EParam::Temperature_Value + 0x30: ParName = EParam::Temperature_Value; break;
		case (char)EParam::Differential_Pressure_Value + 0x30: ParName = EParam::Differential_Pressure_Value; break;
		case (char)EParam::Tap_Location + 0x30: ParName = EParam::Tap_Location; iSizeVal = 1; break;
		case (char)EParam::Static_Pressure_Flag + 0x30: ParName = EParam::Static_Pressure_Flag; iSizeVal = 1; break;
		case (char)EParam::Temperature_Flag + 0x30: ParName = EParam::Temperature_Flag; iSizeVal = 1; break;
		case (char)EParam::Differential_Pressure_Flag + 0x30: ParName = EParam::Differential_Pressure_Flag; iSizeVal = 1; break;
		default: std::cout << "неправильный это ввод." << std::endl; continue;
		}
		WrValue (ParName, iSizeVal);
		DWORD iTime = 0;
		DWORD ulStartTime = timeGetTime ();
		std::unique_ptr<unsigned char[]> cpVal (new unsigned char[iSizeVal]);

	//	//for (int i = 0; i < iSizeVal; i++)
	//	//	cpVal[i] = i + 0x30;
	//	while (iTime < TIME_WAIT_REQUEST)
	//	{
	//		Sleep (1000);
	//		iTime = timeGetTime () - ulStartTime;
	//		if (GetValue (iaTagsWrOfConstPar[ParName.ToInt ()], cpVal.get ()))
	//		{
	//			OutRecieveVal (ParName.ToString(), iSizeVal, cpVal.get());
	//			break;
	//		}
	//		std::cout << (char)5;
	//	}
	//	std::cout << "Данных нет" << std::endl;
		//OutRecieveVal (ParName.ToString (), iSizeVal, cpVal.get ());
		std::cout << "Чтение" << std::endl;
		int retval = ReadParams (retflag);
		if (retflag) return retval;
	}
	return 0;
}
//_____________________________________________________________________________

int CWrConstParam::ReadParams (bool& retflag)
{
	retflag = true;
	for (;;)
	{
		char cIn;
		std::cin >> cIn;
		if (cIn == 'e')
			return 0;
		if (cIn == 'w')
			break;
		int iQuantData = (int)CWrConstParam::EParam::SIZE;
		for (int i = 0; i < iQuantData; i++)
		{
			char ucpBuf[4], caBuf[32];
			GetValue (i, (unsigned char*)ucpBuf);//(iaTagsRdOfConstPar[i]
																					 //_itoa (i, caBuf, 16);
			int iSizePar = i < (int)CWrConstParam::EParam::Static_Pressure_Flag ? 4 : 1;
			OutRecieveVal ((std::string)CWrConstParam::EParam::ToString ((CWrConstParam::EParam::Epar)i) + "= ", iSizePar, (unsigned char*)ucpBuf);
		}
	}
	retflag = false;
	return {};
}
//_____________________________________________________________________________

void CWrConstParam::OutRecieveVal (std::string ParName, int iSizeVal, unsigned char* cpVal)
{
	//if (*cpVal == 0)
	//{
	//	std::cout << ParName.c_str () << ":\tнет данных" << endl;
	//	return;
	//}
	unsigned char caBuf[64];
	std::cout << "Получено: " << ParName.c_str () << "[";
	for (int i = 0; i < iSizeVal; i++)
	{
		_itoa_s (cpVal[i], (char*)caBuf, sizeof (caBuf), 16);
		std::cout << " " << caBuf;
	}
	std::cout << "]" << std::endl;
}
