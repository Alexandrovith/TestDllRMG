/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

~~~~~~~~~	������:			������� RMG																							 _
~~~~~~~~~	������:			EC-605         											 	 	 								 _
~~~~~~~~~	������:			��������� ���-�����             			 		 	 				 	 	 _
~~~~~~~~~	����������:	���������� �.�.                                    		 	 _
~~~~~~~~~	����:				26.06.2018                                         		 	 _

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

#pragma once
//#include <vector>
#include "Connect.h"

using namespace std;

/*�������������������������		� � � � � � � � �		 ���������������������������*/

const int BAUDCOUNT	= 20;		// ������������ ���������� �������� ��������� ������
const int DATABITCOUNT = 4;	// ������������ ���������� �������� ���������� ��� ������
const int PARITYCOUNT	= 5;	// ������������ ���������� �������� ��������
const int STOPBITCOUNT = 2;	// ������������ ���������� �������� ����-�����

static const char* cpSIO_BADPORT = "SIO_BADPORT";
static const char* cpSIO_BADPARM = "SIO_BADPARM";
static const char* cpSIO_WIN32FAIL = "SIO_WIN32FAIL";

const int MAXCOM = 256;			// ������������ ���������� ����������� ���-������

/*��������������������������� 		� � � � � � �     ��������������������������*/

VOID CALLBACK CntIrq (int port);
void ShowSysErr ();		// ��������� � ��������� ������

/*����������������������    � � � � � � � � � � � �    ����������������������*/

//#define SINGLETON(Type) 

/*��������������������������				� � � �   		�����������������������������*/

class CComPort : public IConnect
{
private:
	int iComPort;								// ������� ���-����
	int iBaudRate;								// �������� ������ �� ���-����� - �������
	int iParity;									// �������� �������
	int iByteSize;								// ����� ����� ������ �� ���-�����
	int iStopBits;								// ������� ����-����� � ������� ������ �� ���-�����
	LPSTR	lpMsgBuf;

	virtual bool Opening();				// ����������� / ��������� ���-�����

public:

	CComPort(/*void* Owner, CMess* Message*/);
	CComPort(/*void* Owner, */int Port, int Baud, int iParity, int ByteSize, int StopBits/*, CMess* Message*/);
	~CComPort(void);		
	//static TComPort& This();			// Singleton ������
	bool Init(int Port, int Baud, int Parity, int ByteSize, int StopBits);	// ��������� �������� �������� ������

	int GetComPort() { return iComPort; }			// ������ �������� ���-�����
	int GetBaudRate() { return iBaudRate; }		// ����� ������� �������� ������
	int GetParity() { return iParity; }				// ������ ������� ��������
	int GetByteSize() { return iByteSize; }		// ������ ������� ����� �����
	int GetStopBits() { return iStopBits; }		// ������ �������� ������� ����-�����

	int BaudToCode (int Baud);
	virtual bool OpenPort();						// �������� ���-�����
	virtual bool ClosePort();						// �������� ���-�����
	virtual bool Send (char *cpBufTX, int iSize);		// �������� ������ ����� ���-����
	virtual int Recieve (char* cpBufRX, int iMaxSizeRX, int& iCountBites, std::string asEnd = "da");

	bool ChangePort(int iPort);							// ��������� ������ ���-����� �������������
	bool ChangeParity(int iParity);					// ��������� ����� �������� �������������
	bool ChangeDataBit(int iDataBit);				// ��������� ����� ����� ����� ������ �������������
	bool ChangeStopBits(int iStopBit);			// ��������� ������ �������� ������� ����-��� �������������
	bool ChangeBaud(int iBaud);							// ��������� ����� �������� ������ �������������
	static const char* CoeErrToStr(int iCode);
	void ShowSysErr ();
	const char* MessErr (int err);			// ���� ��������� �� ������ ��� �������� ����� ���-����
};

