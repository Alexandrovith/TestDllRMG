#include "stdafx.h"
#include "RmgRawParam.h"

RmgRawParam::RmgRawParam(void) :
	_size(0),
	_quality(false)
{
	_type = PARAM_TYPE_STRING;
}

RmgRawParam::~RmgRawParam(void)
{

}

RmgRawParam::RmgRawParam(string rawvalue) :
	_rawvalue(rawvalue),
	_size(0),
	_quality(false)
{
	_type = PARAM_TYPE_STRING;
}

void RmgRawParam::setRawValue(string rawvalue)
{
	_rawvalue = rawvalue;
}


PARAM_TYPE RmgRawParam::getType()
{
	return _type;
}

string RmgRawParam::getRawValue()
{
	return _rawvalue;
}

bool RmgRawParam::getInt(int* retVal){
	if(_type == PARAM_TYPE_INT){
		*retVal = _parsedVal.intval;
		return _quality;
	} else if(_type == PARAM_TYPE_HEX){
		*retVal = stoul(_rawvalue, 0, 16);
		return _quality;
	}
	return false;
}

bool RmgRawParam::getDouble(double* retVal)
{
	if(_type == PARAM_TYPE_DOUBLE){
		*retVal = _parsedVal.doubleval;
		return _quality;
	}
	return false;
}

bool RmgRawParam::getDateTime(unsigned char* retVal)
{
	if(_type == PARAM_TYPE_DATETIME){
		memcpy(retVal, _parsedVal.binary, 6);
		return _quality;
	}
	return false;
}

int RmgRawParam::getSize()
{
	return _size;
}

int RmgRawParam::getBinaryData(unsigned char* retVal)
{
	if(_type == PARAM_TYPE_HEX){
		memcpy(retVal, _parsedVal.binary, _size);
		return _size;
	}
	return 0;
}

void RmgRawParam::setBinaryData(unsigned char* val, int count)
{
	memcpy(_parsedVal.binary, val, count);
	_type = PARAM_TYPE_HEX;
	_size = count;
	_quality = true;
}

void RmgRawParam::setInt(int val)
{
	_parsedVal.intval = val;
	_type = PARAM_TYPE_INT;
	_size = sizeof(int);
	_quality = true;
}

void RmgRawParam::setDouble(double val)
{
	_parsedVal.doubleval = val;
	_type = PARAM_TYPE_DOUBLE;
	_size = sizeof(val);
	_quality = true;
}

void RmgRawParam::setType(PARAM_TYPE type)
{
	_type = type;
}

void RmgRawParam::setSize(int size)
{
	_size = size;
}

void RmgRawParam::setQuality(bool quality)
{
	_quality = quality;
}

bool RmgRawParam::getQuality()
{
	return _quality;
}