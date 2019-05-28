#pragma once

#include <windows.h>
#include <string> 
#include <time.h> 
#include "json.h" 

enum class RESP_RETVAL
{
	SUCCESS,
	FAIL,
	NOTREADY,
	REWRITE,
	REPEAT
}; //PROC_RESP_RETVAL;

//Команда для записи или чтения
enum class TRANSACT { READ, WRITE };

using namespace std;

class Transaction
{
public:

	Transaction (void* dev, string requestName);
	~Transaction (void);
	void setPeriod (long period) { _lReqPeriod = period; }
	time_t setFireTime (time_t);
	time_t resetFireTime (time_t);
	time_t setStartTime (time_t);
	time_t getStartTime (void) { return _startTime; }
	bool isFire (time_t);
	unsigned __int8* getOutBuffer () { return cpBufTX; }
	unsigned __int8* getInBuffer () { return cpBufRX; }
	bool isOneShot () { return _oneShot; }
	void setOneShot (bool isOneShot) { _oneShot = isOneShot; }
	// \brief Set align time for fire time event (for archive request to get new daily data as fast as possible)
	void setAlignMinute (int alignMinute) { _alignMinute = alignMinute; }

	DWORD getInSize () { return iLenBufRX; }			///< allocated size for Input
	DWORD getOutSize () { return iLenBufTX; }		///< allocated size for Output
	const string getName () { return _transactionName; }

	char* getAddr () { return _address; }
	virtual void setAddr (UCHAR* addr);

	virtual void prepareToSend () = 0;
	virtual void sendDone () = 0;
	virtual void prepareToRead () = 0;
	virtual void readDone () = 0;
	virtual bool createRequest () = 0;
	//	virtual BOOL AddParameter(TransactionParam::SET_PARAMETER output, const char* key, UCHAR* value)=0;
	virtual DWORD GetRecieveLen () = 0;
	virtual DWORD getOutLength (unsigned __int8 ucR = '\r') = 0;
	virtual USHORT getCommand () = 0;
	virtual void setCommand (void* cmd) = 0;
	virtual void setNeedContinuation (bool) = 0;
	virtual bool isNeedContinuation () = 0;
	virtual RESP_RETVAL processResponse (DWORD size) = 0;
	//virtual void setBadResponse () = 0;
	//virtual bool setWriteParameter (const Json::Value* jsonAddress, char* value, int size) = 0;
	virtual bool setWriteParameter (string asParamName, char* value, int size) = 0;
	// \brief This function is used to check additional parameters to compare transactions
	//		for example for SuperFlo device we need to check RunNumber to be the same for transaction
	// \return true as default behavior
	virtual bool checkAdditionalParams () { return true; }
	virtual void addMoreParameters (Json::Value& deviceConfig) {}
	string GetParamName () { return asParamName; }
	virtual void InitParam (char *cpVal, int iSize) = 0;
	void ComIsWrite ()
	{
		TheComm = TRANSACT::WRITE;
	}

protected:
	//string _deviceName;			///< Name of Device current transaction belongs to
	unsigned __int8* cpBufRX;		///< Raw Buffer for response
	unsigned __int8* cpBufTX;
	int iLenBufRX;		///< Buffer Size for response
	int iLenBufTX;	///< Buffer Size for request
	bool _oneShot;				///< flag for Transaction that should fire only ones by request
	USHORT _command;
	__int8 _address[8];
	long  _lReqPeriod;
	//string getDeviceName() {return _deviceName;};
	void* _device;			///< serial device transaction belongs to
	string _transactionName;	///< Transaction Name 
	string asParamName;
	TRANSACT TheComm = TRANSACT::READ;

private:
	time_t _startTime;
	time_t _fireTime;
	int	   _alignMinute;
};

