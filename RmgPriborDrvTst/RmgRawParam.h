#pragma once
#include <string>

using namespace std;
typedef enum PARAM_TYPE {
	PARAM_TYPE_BYTE = 0, 
	PARAM_TYPE_WORD,
	PARAM_TYPE_INT,
	PARAM_TYPE_DOUBLE,
	PARAM_TYPE_DOUBLE_FIXED,
	PARAM_TYPE_DOUBLE_KP,
	PARAM_TYPE_STRING,
	PARAM_TYPE_YEAR,
	PARAM_TYPE_DAY,
	PARAM_TYPE_MONTH,
	PARAM_TYPE_HOUR,
	PARAM_TYPE_MINUTE,
	PARAM_TYPE_SECOND,
	PARAM_TYPE_DATETIME,
	PARAM_TYPE_HEX,
	PARAM_TYPE_BCD
};

typedef union {
	int intval;
	double doubleval;
	unsigned char binary[8];	// for datetime type
} Tvariant;

class RmgRawParam
{
public:
	RmgRawParam(void);
	~RmgRawParam(void);
	RmgRawParam(string rawvalue);
	PARAM_TYPE getType();
	string getRawValue();
	bool getInt(int* retVal);
	bool getDouble(double* retVal);
	bool getDateTime(unsigned char* retVal);
	int getSize();
	int getBinaryData(unsigned char* retVal);
	void setBinaryData(unsigned char* val, int count);
	void setInt(int val);
	void setDouble(double val);
	void setType(PARAM_TYPE type);
	void setSize(int size);
	void setQuality(bool quality);
	bool getQuality();
	void setRawValue(string rawstr);
	
private:
	PARAM_TYPE	_type;
	string		_rawvalue;
	Tvariant	_parsedVal;
	int			_size;
	bool		_quality;
};

