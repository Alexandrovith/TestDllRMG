///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	Проект:			Driver EC605
///~~~~~~~~~	Прибор:			EC605
///~~~~~~~~~	Модуль:			Абстрактный класс коммуникационного обмена с прибором
///~~~~~~~~~	Разработка:	Демешкевич С.А.
///~~~~~~~~~	Дата:				27.07.2018

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#pragma once

#include "stdafx.h"

class IConnect
{
	std::string asRet;

protected:
	//CMessages * Mess;
	//void* Own;
	bool bConnected;														// Состояние порта

public:
	IConnect (/*void* Owner, CMessages* Message*/);
	~IConnect ();
	virtual bool Connecting () = 0;							// Подключение / настройка соединения	
	virtual bool Connect () = 0;								// Открытие соединения
	virtual bool Disconnect () = 0;							// Закрытие соединения
	virtual bool Send (char *cpBufTX, int iSize) = 0;		// Отправка буфера через порт
	virtual int Recieve (char* cpBufRX, int iMaxSizeRX, int& iCountBites, std::string asEnd = "da") = 0;
	bool IsConnected () { return bConnected; }					// Проверка наличия открытого состояния порта
};

