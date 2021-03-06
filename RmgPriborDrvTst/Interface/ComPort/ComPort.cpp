/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

~~~~~~~~~	������:			������� RMG																							 _
~~~~~~~~~	������:			EC-605         											 	 	 								 _
~~~~~~~~~	������:			��������� ���-�����             			 		 	 				 	 	 _
~~~~~~~~~	����������:	���������� �.�.                                    		 	 _
~~~~~~~~~	����:				26.06.2018                                         		 	 _

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

#include "stdafx.h"
#include "Common.h"
#include "RmgPriborDrv.h"
#include "SerialDev.h"

#include "Messages.h"
#include "ComPort.h"
#include "pcomm.h"

#pragma comment(lib, "pcomm.lib")

/*�������������������������		� � � � � � � � �		 ���������������������������*/

//const int iaBits[DATABITCOUNT] = { 0,0,0,0, BIT_5, BIT_6, BIT_7, BIT_8 };
const int iaBits[] = { 0,0,0,0,0, BIT_5, BIT_6, BIT_7, BIT_8 };
//const string csBits[DATABITCOUNT] = { "5", "6", "7", "8" };
const int iaParity[PARITYCOUNT] = { P_NONE, P_ODD, P_EVEN, P_MRK, P_SPC };
//const string csParity[PARITYCOUNT] = { "None", "Even", "Odd", "Mark", "Space" };
const int iaStopBits[] = { 0, STOP_1, STOP_2 };//STOPBITCOUNT
//const string csStopBits[STOPBITCOUNT] = { "1", "2" };
//const string csBauds[BAUDCOUNT] = {
//	"50","75","110","134","150","300","600","1200","1800","2400","4800","7200",
//	"9600","19200","38400","57600","115200","230400","460800","921600"
//};
//const int iaBauds[BAUDCOUNT] = {
//	50,75,110,134,150,300,600,1200,1800,2400,
//	4800,7200,9600,19200,38400,57600,115200,230400,460800,921600
//};
/*�������������������������			� � �   � � � � � �     ����������������������*/

volatile int iCurrPort = -1;

//\brif ���� ������� �� ���-�����
//\parameter port - ����� ���-�����
VOID CALLBACK CntIrq(int port)
{
	iCurrPort = port;
	//PostMessage(hDll, WM_RECIEVE_COMMAND, port, 0); // TODO ����  ������� �� ���-�����
	//TAnswer::MThis().Send (port);
}

//_________________    ����������� ������ TComPort   ___________________________

//\brif ������������� ���������� ������ � �������� �������� ������ �������
CComPort::CComPort(/*void* Owner,CMess* Message */) :
	IConnect(/*Owner,dynamic_cast<CMessages*> (Message) */)
	, iComPort(1)
	, iBaudRate(B9600)
	, iParity(P_NONE)
	, iByteSize(BIT_8)
	, iStopBits(STOP_1)
{	//Hw = FALSE;	Sw = FALSE;	Dtr = TRUE;	Rts = TRUE;
}
//_____________________________________________________________________________

//\brif ������������� ���������� ������ � �������� �������� ������ �������
CComPort::CComPort(/*void* Owner, */int Port, int Baud, int Parity, int ByteSize, int StopBits/*, CMess* Message*/) :
	IConnect(/*Owner,dynamic_cast<CMessages*> (Message) */)
	, iComPort(Port)
	, iParity(Parity)
	, iByteSize(ByteSize)
	, iStopBits(StopBits)
{	//Hw = FALSE;	Sw = FALSE;	Dtr = TRUE;	Rts = TRUE;
	iBaudRate = BaudToCode(Baud);
	Connect();
}
//____________________   ���������� ������ TComPort   _________________________

CComPort::~CComPort(void)
{
	Disconnect();	//ivFreePorts.clear();
	LocalFree (lpMsgBuf);												// Free the buffer.
}
//____________________________________________________________________________
//��������� �������� �������� ������
bool CComPort::Init(int Port, int Baud, int Parity, int ByteSize, int StopBits)
{
	iComPort = Port;
	iBaudRate = BaudToCode(Baud);
	iParity = Parity;
	iByteSize = ByteSize;
	iStopBits = StopBits;
	return Connect();
}
//___________________________   Singleton ������    ___________________________
//TComPort& TComPort::This()
//{
//	static TComPort MInstance;
//	return MInstance;
//}
//_________________   ��������� �������� �������� ������   ____________________

int CComPort::BaudToCode(int Baud)
{
	switch (Baud)
	{
	case 50: return 0x00;
	case 75: return 0x01;
	case 110: return 0x02;
	case 134: return 0x03;
	case 150: return 0x04;
	case 300: return 0x05;
	case 600: return 0x06;
	case 1200: return 0x07;
	case 1800: return 0x08;
	case 2400: return 0x09;
	case 4800: return 0x0A;
	case 7200: return 0x0B;
		//case 9600: return 0x0C;
	case 19200: return 0x0D;
	case 38400: return 0x0E;
	case 57600: return 0x0F;
	case 115200: return 0x10;
	case 230400: return 0x11;
	case 460800: return 0x12;
	case 921600: return 0x13;
	}
	return B9600;
}
//___________________   ��������� ������������ ���-�����   _____________________

bool CComPort::Connecting()
{
	int iPort = iComPort;
	int iRet = sio_open(iPort);
	if (iRet != 0)
	{
		Mess->Out (MessErr(iRet)); return false;//LOGF(((SerialDev*)Own)->getName(), MessErr(iRet))
	}

	iRet = sio_ioctl(iPort, iBaudRate, iaParity[iParity] | iaBits[iByteSize] | iaStopBits[iStopBits]);
	if (iRet != SIO_OK)
	{
		Mess->Out (MessErr (iRet)); return false;/*LOGF(((SerialDev*)Own)->getName(), MessErr(iRet));*/
	}
	//	iRet = sio_SetWriteTimeouts(iPort, MAXDWORD); // Return immediately without waiting for the data to be sending out.
	//	if(iRet != SIO_OK) 	return MMessErr (iRet, _T("");
	iRet = sio_SetReadTimeouts(iPort, 5000, 500);	//iRet = sio_SetReadTimeouts(iPort, 5000, 80);
	//sio_SetWriteTimeouts (iPort, 8000);
	if (iRet != SIO_OK)
	{
		Mess->Out (MessErr (iRet));  return false;/*LOGF(((SerialDev*)Own)->getName(), MessErr(iRet));*/
	}
	//iRet = sio_cnt_irq (iPort, CntIrq, 1);//TODO restore?
	//if (iRet != SIO_OK)
	//{
	//	LOG (MessErr (iRet, "sio_cnt_irq")); return false;	//	iRet = sio_Tx_empty_irq (iPort, TxEmptyIrq); 
	//}
	//	if (iRet != SIO_OK) 	return MMessErr (iRet, _T("sio_Tx_empty_irq"));
	Mess->Out ("���� COM%d ������", iPort);		// LOGF (((SerialDev*)Own)->getName (), "���� COM%d ������", iPort);
	return true;
}
//_________________________   �������� ���-�����   ___________________________

bool CComPort::Connect()
{
	if (bConnected)								// ���� ���� ��� ������ -
		Disconnect();								//  ��������� ����
	bConnected = Connecting();		// ��������� ���� 
	return bConnected;
}
//___________________________   �������� ���-�����   ___________________________

bool CComPort::Disconnect()
{
	if (bConnected)
	{
		bConnected = false;
		try
		{
			//sio_cnt_irq (iComPort, NULL, 0);        //			sio_Tx_empty_irq (iPort, NULL);
			sio_close(iComPort);
			Mess->Out ("���� COM%d ������", iComPort);		// LOGF (((SerialDev*)Own)->getName (), "���� COM%d ������", iComPort);
		}
		catch (...) {}
	}
	return bConnected;
}
///____________________   �������� ������ �� ������   ___________________________
//\brif �������� ������ �� ������
//\parameter i8Buf - ��������� �� ����� � ������� ��� �������� ����� ���-����
//\parameter iSize - ����� �������
bool CComPort::Send(char *cpBufTX, int iSize)
{
	if (bConnected)
	{
		sio_flush(iComPort, 2);
		int iRet = sio_write(iComPort, (char*)cpBufTX, iSize);
		if (iRet < 0)
		{
			Mess->Out ("COM%d: %s", iComPort, MessErr(iRet));		// LOGF(((SerialDev*)Own)->getName(), "COM%d: %s", iComPort, MessErr(iRet));
			return false;
		}          					//		sio_Tx_empty_irq (iMComPort, TxEmptyIrq);
	}
	return true;
}
///____________________________________________________________________________
//\brif ��������� � ��������� ������
//\return ���������� ���������� ����
int CComPort::Recieve(char* cpBufRX, int iMaxSizeRX, int& iCountBites, std::string asEnd)
{
	iCountBites = 0;
	int iCountCurr = 0;
	//do
	//{
	iCountCurr = sio_read(iComPort, cpBufRX, iMaxSizeRX);
	cpBufRX += iCountCurr;
	iCountBites += iCountCurr;
	//} while (iCountCurr != 0 && (!((i8BufRX[-1] & 0x7f) == asEnd[1] && (i8BufRX[-2] & 0x7f) == asEnd[0])));
	if (iCountBites < 0)
	{
		Mess->Out ("COM%d: %s", iComPort, MessErr(iCountBites));// LOGF(((SerialDev*)Own)->getName(), 
		return 1;
	}
	return iCountBites == 0;
}
///____________________________________________________________________________

const char* CComPort::CoeErrToStr(int iCode)
{
	switch (iCode)
	{
	case SIO_BADPORT: return cpSIO_BADPORT;
	case SIO_BADPARM: return cpSIO_BADPARM;
	case SIO_WIN32FAIL: return cpSIO_WIN32FAIL;

	default: return "ERR_CONNECT";
	}
}
///____________________________________________________________________________
//\brif ��������� � ��������� ������
//\parameter title - ��������� ���������
void CComPort::ShowSysErr()
{
	DWORD	syserr;

	syserr = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		syserr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	// Display the string.
	//MessageBox (NULL,(LPCWSTR)lpMsgBuf, title.c_str(), MB_OK|MB_ICONEXCLAMATION);	
	//LocalFree(lpMsgBuf);												// Free the buffer.
}
///__________   ��������� � ��������� ����������� �������� ���-�����   ________
//\brif ��������� � ��������� ����������� �������� ���-�����
//\parameter err - ��� ������
//\parameter title - ������������� ���������� �� ������
const char* CComPort::MessErr(int err)
{
	switch (err)
	{
	case SIO_BADPORT: 			return "���� �� ������ ������� ��� �������� ����� �����.";
	case SIO_BADPARM: 			return "��������� ��������.";
	case SIO_ABORTWRITE: 		return "��������� ����������� ������ �������������.";
	case SIO_WRITETIMEOUT: 	return "����� �������� ������ �����������.";
	case SIO_OPENFAIL: 			return "����� ������ ��� ��� ���� ����� ������ ����������.\n";//*� +  IntToStr((int)iMComPort) + */			

	case SIO_WIN32FAIL: 		ShowSysErr(); return (const char*)lpMsgBuf;   //return "������ ������� Win32 �������� �������.";
	case SIO_OUTCONTROL:		return "Moxa �� ������������ ��� �������";
	case SIO_NODATA:				return "��� ������ ��� ������";
	case SIO_RTS_BY_HW:			return "RTS �� ��� ���������� ������ ��� RTS/CTS Flowctrl";
	case SIO_BOARDNOTSUPPORT:return "����� �� ������������ ��� �������";
	}
	return "���� ����������.";
}
///_______________   ��������� ������ ���-����� �������������   ________________

bool CComPort::ChangePort(int iPort)
{
	//TSetup::MThis().MParamChanged();
	//iMComPortNew = ivMFreePorts[FDlg->CBPorts.GetCurSel()];		//((int)SendDlgItemMessage (FDlg->H_WND, IDC_PORT, CB_GETCURSEL, 0, 0L))];
	iComPort = iPort;
	return Connect();
	//iComPortNew = iPort;
}
//__________________   ��������� ����� �������� �������������   _______________

bool CComPort::ChangeParity(int iNumParity)
{
	return sio_ioctl(iComPort, iBaudRate, iaParity[iNumParity] | 8 - iByteSize | iaStopBits[iStopBits]) == SIO_OK;
	//iParityNew = iaParity[iNumParity];		//iParityBytStop = iMParity | iMByteSize | iMStopBits;TODO
}
//_____________   ��������� ����� ����� ����� ������ �������������   __________

bool CComPort::ChangeDataBit(int iNumDataBit)
{
	return sio_ioctl(iComPort, iBaudRate, iaParity[iParity] | iaBits[iNumDataBit] | iaStopBits[iStopBits]) == SIO_OK;
	//iByteSizeNew = iaBits[iNumDataBit];	//TODO
}
//________   ��������� ������ �������� ������� ����-��� �������������   _______

bool CComPort::ChangeStopBits(int iNumStopBit)
{
	return sio_ioctl(iComPort, iBaudRate, iaParity[iParity] | iaBits[iByteSize] | iaStopBits[iNumStopBit]) == SIO_OK;
	//iStopBitsNew = iaStopBits[iNumStopBit];		//TODO
}
//_____________   ��������� ����� �������� ������ �������������   _____________

bool CComPort::ChangeBaud(int iBaud)
{
	iBaudRate = BaudToCode(iBaud);
	return sio_baud(iComPort, iBaudRate) == SIO_OK;//	iBaudRateNew = iBaud;	//TODO
}
//_____________________________________________________________________________
//������ ������������ ������� ��������� � �������
bool MessIsReady()
{
	return false;
}

//void CComPort::Init (void)
//{
//	//ivFreePorts.reserve (10);
//
//	//FindCOMports();
//	//MFillComboBox();			
//	//TAnswer::MThis().Init();		// Answer = new TAnswer();
//}
//______________________  ����� ��������� COM �������  ________________________
//����� ��������� COM �������
//bool CComPort::FindCOMports ()
//{
//	bool bRet = false;
//	Disconnect ();
//	//ivFreePorts.clear();
//	string csCOM;
//	for (int iCom = 1; iCom < MAXCOM; iCom++)
//	{
//		if (sio_open (iCom) == SIO_OK)   					// Is the port opened
//		{
//			//csCOM.Format (_T("COM%d"), iCom);
//			//FDlg->CBPorts.AddString (csCOM);
//			sio_close (iCom);
//			iComPort = iCom;
//			//ivFreePorts.push_back(iCom);
//			bRet = true;
//		}
//	}						//SendMessage (GetDlgItem (FDlg->H_WND, IDC_PORT), CB_SETCURSEL, iLastPort, 0L);
//	return bRet;
//}
