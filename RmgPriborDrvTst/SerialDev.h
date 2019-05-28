#pragma once
#include "RmgPriborDrv.h"
#include <vector>
#include <string>
#include "Transaction.h"
#include "json.h"
#include "Storage.h"

//#ifdef CONN_COMPORT
#include "ComPort.h"
//#else
#include "TCP.h"
//#endif

//#define DEBUG_RAW_DATA

using namespace std;

/// \brief This enum defines states of serial device
typedef enum SerialDevState
{
	SD_INIT = 0,
	SD_NEXT_REQUEST,
	SD_WAIT_REQUEST_FINISH,
};

enum class ETransport { Serial, Ethernet };
typedef vector<Transaction *> TransactionList;
const int SIZE_PORT = 8;

class SerialDev
{
public:
	SerialDev(const Json::Value& DeviceConfig, const Json::Value& DeviceType);
	~SerialDev(void);
	
	int processTransactions();
	//int WaitRequestFinish (Transaction * &activeTransaction, const time_t &tCurTime, bool &retflag);
	Transaction* getTransaction();
	const TransactionList * getTransactionsList() { return &_transactions; }
	int openPort();
	void closePort();
	int SendTX();
	int RecieveRX();
	void LogF (Transaction * tr, char* cpParamName, char *caCommand, UINT &uiCommand);
	void nextRequest(Transaction* tr, time_t tCurTime);
	int processResponse(Transaction* tr);
	void setPassword(string pass) { _password = pass; }
	string getPassword() { return _password; }
	void stop() { bStop = true; }
	void start() { bStop = false; }
	string& getName() { return _devName; }
	char* getPort() { return caPort; }
	string& getDevProtocol() { return _devProtocol; }
	string& getDeviceType() { return _deviceType; }
	int getDataBits();
	DWORD GetCommand ();
	bool subscribe(UINT tagId, Json::Value& jsonAddress, bool isOneShot);
	bool subscribeWrite(Json::Value& jsonAddress, char* value, int size);
	bool CreateCommByWr (Transaction * &transaction, std::string asReqName, string ParamName);
	Transaction* findTransactionByAddress(Json::Value& jsonAddress);
	Transaction* findTransactionByName(string& name);
	bool addRequest(Transaction * transaction);
	Json::Value findRequest(string& name);
	Json::Value* getDeviceConfigType() { return &_deviceConfigType; }
	Json::Value* getDeviceConfig() { return &_deviceConfig; }
	Storage* getStorage() { return _localStorage; }
	void setStorage(Storage* storage) { _localStorage = storage; }
	void setHTime(time_t t) { _htime = t; }
	void setDTime(time_t t) { _dtime = t; }
	time_t getHTime () { return _htime; }
	time_t getDTime() { return _dtime; }
	void nextTransaction() { _activeTransactionIdx++; }
	bool isFirstSubscribe() { return bFirstSubscribe; }
	void setCurrentDateTime(unsigned char* rawDT);
	time_t getCurrentDateTime();
	void setCD (unsigned char cd) { _startHour = cd; }
	string GetIP ()
	{
		if (Line)
			return ((CTCP*)Line)->IP_Port ();
		else
			return "";
	}

private:
	Transaction * activeTransaction;
//#ifdef CONN_COMPORT
//CComPort* Line;
//#else
	IConnect* Line;
//#endif

	time_t		_htime;					///< hourly archive start time
	time_t		_dtime;					///< daily archive start time
	struct tm _currentDateTime;		///< date and time got in last response
	UCHAR		_startHour;				///< CD parameter - contract time;
	//char		_address[4];			///< device address  
	string		_devProtocol;			///< Protocol of device
	string		_deviceType;			///< Type of device
	int _dataBits;
	ETransport Transport;		///< Transport to use (serial or Ethernet)
	TransactionList _transactions;		///< Vector of all transactions
	UINT		_activeTransactionIdx;  ///< Index of current processing transaction
	string		_devName;				///< device name
	//HANDLE		_hComm;					///< serial port handle
	char		caPort[SIZE_PORT];					///< port Name or IP address
	string		_password;				///< password 
	int		iLenRX;			///< Number of bytes received from serial port
	SerialDevState _state;				///< this defines serial device processor state
	time_t		_tRepeatTime;			///< when to start com port processing(Connect)
	bool		bStop;					///< Stop serial device flag
	Json::Value _deviceConfigType;		///< JSON configuration for requests/ responses supported by device
	Json::Value _deviceConfig;			///< Configuration of serial device in Json format (speed, bits, parity ...)
	Storage*	_localStorage;			///< Storage for all parameters for current serial device
	bool		bFirstSubscribe;		
};

typedef struct 
{
	SerialDev * serailDev;
	string ParamName;   ///< parameter name
} TagDescription;

extern map<int, TagDescription> tags;