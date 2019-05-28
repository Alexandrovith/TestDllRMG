/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

~~~~~~~~~	Проект:			Драйвер RMG																							 _
~~~~~~~~~	Прибор:			EC-605         											 	 	 								 _
~~~~~~~~~	Модуль:			Настройка СОМ-порта             			 		 	 				 	 	 _
~~~~~~~~~	Разработка:	Демешкевич С.А.                                    		 	 _
~~~~~~~~~	Дата:				26.06.2018                                         		 	 _

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

#pragma once

/*ююююююююююююююююююююююююю		К О Н С Т А Н Т Ы		 ююююююююююююююююююююююююююю*/

const int BAUDCOUNT	= 20;		// Максимальное количество значений скоростей обмена
const int DATABITCOUNT = 4;	// Максимальное количество значений количества бит данных
const int PARITYCOUNT	= 5;	// Максимальное количество значений чётности
const int STOPBITCOUNT = 2;	// Максимальное количество значений стоп-битов

const int MAXCOM = 256;			// Максимальное количество сканируемых СОМ-портов

/*ююююююююююююююююююююююююююю 		Ф У Н К Ц И И     юююююююююююююююююююююююююю*/

//VOID CALLBACK CntIrq (int port);
void ShowSysErr (string title);		// Сообщение о системной ошибке

/*юююююююююююююююююююююю    М А К Р О К О М А Н Д Ы    юююююююююююююююююююююю*/

//#define SINGLETON(Type) 

/*юююююююююююююююююююююююююю				Т И П Ы   		ююююююююююююююююююююююююююююю*/

class CComPort
{
private:
	int iComPort;								// Текущий СОМ-порт
	int iBaudRate;								// Скорость обмена по СОМ-порту - текущая
	int iParity;									// Четность текущая
	int iByteSize;								// Длина байта обмена по СОМ-порту
	int iStopBits;								// Наличие стоп-битов в команде обмена по СОМ-порту

	bool bPortIsOpen;						// Состояние СОМ-порта

	bool Opening();								// Подключение / настройка СОМ-порта
	void* Own;

public:

	CComPort(void* Owner);
	CComPort(void* Owner, int Port, int Baud, int iParity, int ByteSize, int StopBits);
	~CComPort(void);		
	//static TComPort& This();			// Singleton класса
	bool Init(int Port, int Baud, int Parity, int ByteSize, int StopBits);							// Установка исходных настроек модуля

	int GetComPort() { return iComPort; }			// Запрос текущего СОМ-порта
	int GetBaudRate() { return iBaudRate; }		// Зпрос текущей скорости обмена
	int GetParity() { return iParity; }				// Запрос текущей чётности
	int GetByteSize() { return iByteSize; }		// Запрос текущей длины байта
	int GetStopBits() { return iStopBits; }		// Запрос текущего наличия стоп-битов

	int BaudToCode (int Baud);
	bool OpenPort();						// Открытие СОМ-порта
	bool ClosePort();						// Закрытие СОМ-порта
	bool Send (unsigned __int8 *i8Buf, int iSize);		// Отправка буфера через СОМ-порт
	int Recieve (unsigned __int8* i8BufRX, int iMaxSizeRX, int& iCountBites, string asEnd = "da");
	const char* MessErr (int err, string title);			// Вывод сообщения об ошибке при передаче через СОМ-порт
	bool IsOpen() { return bPortIsOpen; }							// Проверка наличия открытого состояния СОМ-порта

	bool ChangePort(int iPort);							// Установка нового СОМ-порта пользователем
	bool ChangeParity(int iParity);					// Установка новой чётности пользователем
	bool ChangeDataBit(int iDataBit);				// Установка новой длины байта данных пользователем
	bool ChangeStopBits(int iStopBit);			// Установка нового значения наличия стоп-бит пользователем
	bool ChangeBaud(int iBaud);							// Установка новой скорости обмена пользователем
};

