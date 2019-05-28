/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

~~~~~~~~~	Проект:			Драйвер RMG																							 _
~~~~~~~~~	Прибор:			EC-605         											 	 	 								 _
~~~~~~~~~	Модуль:			Настройка СОМ-порта             			 		 	 				 	 	 _
~~~~~~~~~	Разработка:	Демешкевич С.А.                                    		 	 _
~~~~~~~~~	Дата:				26.06.2018                                         		 	 _

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

#pragma once
//#include <vector>
#include "Connect.h"

using namespace std;

/*ююююююююююююююююююююююююю		К О Н С Т А Н Т Ы		 ююююююююююююююююююююююююююю*/

const int BAUDCOUNT	= 20;		// Максимальное количество значений скоростей обмена
const int DATABITCOUNT = 4;	// Максимальное количество значений количества бит данных
const int PARITYCOUNT	= 5;	// Максимальное количество значений чётности
const int STOPBITCOUNT = 2;	// Максимальное количество значений стоп-битов

static const char* cpSIO_BADPORT = "SIO_BADPORT";
static const char* cpSIO_BADPARM = "SIO_BADPARM";
static const char* cpSIO_WIN32FAIL = "SIO_WIN32FAIL";

const int MAXCOM = 256;			// Максимальное количество сканируемых СОМ-портов

/*ююююююююююююююююююююююююююю 		Ф У Н К Ц И И     юююююююююююююююююююююююююю*/

VOID CALLBACK CntIrq (int port);
void ShowSysErr ();		// Сообщение о системной ошибке

/*юююююююююююююююююююююю    М А К Р О К О М А Н Д Ы    юююююююююююююююююююююю*/

//#define SINGLETON(Type) 

/*юююююююююююююююююююююююююю				Т И П Ы   		ююююююююююююююююююююююююююююю*/

class CComPort : public IConnect
{
private:
	int iComPort;								// Текущий СОМ-порт
	int iBaudRate;								// Скорость обмена по СОМ-порту - текущая
	int iParity;									// Четность текущая
	int iByteSize;								// Длина байта обмена по СОМ-порту
	int iStopBits;								// Наличие стоп-битов в команде обмена по СОМ-порту
	LPSTR	lpMsgBuf;

	virtual bool Opening();				// Подключение / настройка СОМ-порта

public:

	CComPort(/*void* Owner, CMess* Message*/);
	CComPort(/*void* Owner, */int Port, int Baud, int iParity, int ByteSize, int StopBits/*, CMess* Message*/);
	~CComPort(void);		
	//static TComPort& This();			// Singleton класса
	bool Init(int Port, int Baud, int Parity, int ByteSize, int StopBits);	// Установка исходных настроек модуля

	int GetComPort() { return iComPort; }			// Запрос текущего СОМ-порта
	int GetBaudRate() { return iBaudRate; }		// Зпрос текущей скорости обмена
	int GetParity() { return iParity; }				// Запрос текущей чётности
	int GetByteSize() { return iByteSize; }		// Запрос текущей длины байта
	int GetStopBits() { return iStopBits; }		// Запрос текущего наличия стоп-битов

	int BaudToCode (int Baud);
	virtual bool OpenPort();						// Открытие СОМ-порта
	virtual bool ClosePort();						// Закрытие СОМ-порта
	virtual bool Send (char *cpBufTX, int iSize);		// Отправка буфера через СОМ-порт
	virtual int Recieve (char* cpBufRX, int iMaxSizeRX, int& iCountBites, std::string asEnd = "da");

	bool ChangePort(int iPort);							// Установка нового СОМ-порта пользователем
	bool ChangeParity(int iParity);					// Установка новой чётности пользователем
	bool ChangeDataBit(int iDataBit);				// Установка новой длины байта данных пользователем
	bool ChangeStopBits(int iStopBit);			// Установка нового значения наличия стоп-бит пользователем
	bool ChangeBaud(int iBaud);							// Установка новой скорости обмена пользователем
	static const char* CoeErrToStr(int iCode);
	void ShowSysErr ();
	const char* MessErr (int err);			// Выод сообщения об ошибке при передаче через СОМ-порт
};

