#pragma once
#include "Transaction.h"
#include <map>
#include "json.h"
#include "SerialDev.h"
#include "RmgStorage.h"

const int RMG_FF_BYTES = 23;

#define COMM(sym1,sym2) ((sym1) + ((sym2) << 8))

const USHORT RMG_W0_COMMAND = 0x3057;
const USHORT RMG_W1_COMMAND = 0x3157;
const USHORT RMG_W2_COMMAND = 0x3257;
const USHORT RMG_H1_COMMAND = 0x3148;
const USHORT RMG_H2_COMMAND = 0x3248;
const USHORT RMG_D1_COMMAND = 0x3144;
const USHORT RMG_D2_COMMAND = 0x3244;
const USHORT RMG_R1_COMMAND = 0x3152;

#define IS_RMG_W_COMMAND(x) ((x & 0xff) == (RMG_W0_COMMAND & 0xff))
#define IS_RMG_DH_COMMAND(x) ((x & 0xff) <= (RMG_H1_COMMAND & 0xff))
#define IS_RMG_D_COMMAND(x) ((x & 0xff) == (RMG_D1_COMMAND & 0xff))
#define IS_RMG_H_COMMAND(x) ((x & 0xff) == (RMG_H1_COMMAND & 0xff))

enum class StateWR { NoSendYet, SendR1ok, SendHeadOK, SendDataWrOK };
//����. ������ ������� ��� ��������� ������ ���
const int SIZE_VAL_MAX = 16;

class RmgTransaction :
	public Transaction
{
public:
	RmgTransaction(SerialDev* device, string requestName, string ParamName);
	~RmgTransaction (void) {}
	static Transaction* createTransaction(SerialDev* device, string requestName, string asParamName);//const Json::Value &jsonAddress
	void processAmRecordAndStore(Json::Value& params, vector<string>& paramValuesStrPack);
	void processAzRecordAndStore(Json::Value& params, vector<string>& paramValuesStrPack);
	void processAsRecordAndStore(Json::Value& params, vector<string>& paramValuesStrPack);
	void processNmRecordAndStore(Json::Value& params, vector<string>& paramValuesStrPack);
	void processParametersAndStore(Json::Value& params, vector<string>& paramValuesStr);
	RmgStorage::tRmgParams processCommandChangeValue(Json::Value& commands, string rawCommandChangeValue);
	Json::Value getCommandsDescription();
	Json::Value getStatusDescription();

	virtual void setAddr(char* addr);
	virtual USHORT getCommand() { return _command & 0xffff; }
	virtual void setCommand(void* cmd);
	virtual void prepareToSend();
	virtual void prepareToRead () {}
	virtual void sendDone () {}
	virtual void readDone () {}
	virtual bool createRequest();
	virtual DWORD GetRecieveLen();
	virtual DWORD getOutLength(unsigned __int8 ucR = '\r');
	virtual void setNeedContinuation(bool);
	virtual bool isNeedContinuation();
	virtual RESP_RETVAL processResponse(DWORD size);
	void Conv7to8byt (const DWORD size);
	virtual void addMoreParameters(Json::Value* deviceConfig);
	virtual bool setWriteParameter(string asParamName, char* value, int size);//const Json::Value* jsonAddress
	virtual void InitParam (char *cpVal, int iSize);
	
private:
	Json::Value _jsonRequest;
	char caValue[SIZE_VAL_MAX];
	bool _needContinuation;
	bool _8bit_transaction;
	string _userId;
	string _password;
	string _writePassword;
	string _chainCommand;
	string _writeParam;
	//��������� ��� ������ ���
	StateWR StateWr = StateWR::NoSendYet;
	//bool bWasRecieveHeaderByWr = false;

	RESP_RETVAL processBinaryResponse(DWORD size); //< process buffer for H1,H2,D1,D2 commands
	Json::Value findRequest(string& reqName);
	bool addChainedRequest(const Json::Value* jsonAddress);	/// < adding command to be send after successful response
	void setStartDate();
	inline SerialDev* getSerialDev();
	bool checkResponseCRC(string str);
	USHORT CRC(const unsigned char* data, UINT length, USHORT poly, USHORT init, bool isbinary=false);
	string CRCtoASCII(USHORT crc);
	vector<string> getSeparatedStrings(string str, char* separator);
	void remove0D0A(string str);
	PARAM_TYPE getTypeByString(string stype);
	void saveTimeFromStorage(string reqPrefix, int add_minute);
	time_t getTimeFromStorage(string reqPrefix, string reqPostfix);
	RmgRawParam processParamValue(Json::Value& paramDesc, string rawValue, string am_prefix);
	RmgRawParam processParamValueBinary(Json::Value& paramDesc, unsigned char* rawValue, string am_prefix);
	double convertWithKp(string paramName, double initValue, string kpname);
	inline RmgStorage * getStorage() { return (RmgStorage *)(getSerialDev ()->getStorage ()); }
	void LogRX (const DWORD &size, SerialDev* Dev);

};

