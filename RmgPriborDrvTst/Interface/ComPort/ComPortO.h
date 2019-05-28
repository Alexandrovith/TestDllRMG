/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

~~~~~~~~~	������:			������� RMG																							 _
~~~~~~~~~	������:			EC-605         											 	 	 								 _
~~~~~~~~~	������:			��������� ���-�����             			 		 	 				 	 	 _
~~~~~~~~~	����������:	���������� �.�.                                    		 	 _
~~~~~~~~~	����:				26.06.2018                                         		 	 _

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

#pragma once

/*�������������������������		� � � � � � � � �		 ���������������������������*/

const int BAUDCOUNT	= 20;		// ������������ ���������� �������� ��������� ������
const int DATABITCOUNT = 4;	// ������������ ���������� �������� ���������� ��� ������
const int PARITYCOUNT	= 5;	// ������������ ���������� �������� ��������
const int STOPBITCOUNT = 2;	// ������������ ���������� �������� ����-�����

const int MAXCOM = 256;			// ������������ ���������� ����������� ���-������

/*��������������������������� 		� � � � � � �     ��������������������������*/

//VOID CALLBACK CntIrq (int port);
void ShowSysErr (string title);		// ��������� � ��������� ������

/*����������������������    � � � � � � � � � � � �    ����������������������*/

//#define SINGLETON(Type) 

/*��������������������������				� � � �   		�����������������������������*/

class CComPort
{
private:
	int iComPort;								// ������� ���-����
	int iBaudRate;								// �������� ������ �� ���-����� - �������
	int iParity;									// �������� �������
	int iByteSize;								// ����� ����� ������ �� ���-�����
	int iStopBits;								// ������� ����-����� � ������� ������ �� ���-�����

	bool bPortIsOpen;						// ��������� ���-�����

	bool Opening();								// ����������� / ��������� ���-�����
	void* Own;

public:

	CComPort(void* Owner);
	CComPort(void* Owner, int Port, int Baud, int iParity, int ByteSize, int StopBits);
	~CComPort(void);		
	//static TComPort& This();			// Singleton ������
	bool Init(int Port, int Baud, int Parity, int ByteSize, int StopBits);							// ��������� �������� �������� ������

	int GetComPort() { return iComPort; }			// ������ �������� ���-�����
	int GetBaudRate() { return iBaudRate; }		// ����� ������� �������� ������
	int GetParity() { return iParity; }				// ������ ������� ��������
	int GetByteSize() { return iByteSize; }		// ������ ������� ����� �����
	int GetStopBits() { return iStopBits; }		// ������ �������� ������� ����-�����

	int BaudToCode (int Baud);
	bool OpenPort();						// �������� ���-�����
	bool ClosePort();						// �������� ���-�����
	bool Send (unsigned __int8 *i8Buf, int iSize);		// �������� ������ ����� ���-����
	int Recieve (unsigned __int8* i8BufRX, int iMaxSizeRX, int& iCountBites, string asEnd = "da");
	const char* MessErr (int err, string title);			// ����� ��������� �� ������ ��� �������� ����� ���-����
	bool IsOpen() { return bPortIsOpen; }							// �������� ������� ��������� ��������� ���-�����

	bool ChangePort(int iPort);							// ��������� ������ ���-����� �������������
	bool ChangeParity(int iParity);					// ��������� ����� �������� �������������
	bool ChangeDataBit(int iDataBit);				// ��������� ����� ����� ����� ������ �������������
	bool ChangeStopBits(int iStopBit);			// ��������� ������ �������� ������� ����-��� �������������
	bool ChangeBaud(int iBaud);							// ��������� ����� �������� ������ �������������
};

