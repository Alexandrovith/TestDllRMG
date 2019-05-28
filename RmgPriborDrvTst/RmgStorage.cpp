#include "stdafx.h"
#include <time.h>
#include "RmgPriborDrv.h"
#include "RmgStorage.h"
#include "Common.h"


RmgStorage::RmgStorage(void)
{
	setConnectionStatus(false);
	initLock();
}


RmgStorage::~RmgStorage(void)
{
	delLock();
}

/// \brief This function writes parameter to Storage by parameter name
/// \param name parameter name
/// \param param pointer to structure with parameter description
/// \return true on successful parameter write
bool RmgStorage::set(string name,  void* param)
{
	LOCK;
	tRmgParamsIt it = _rmgRawData.find(name);
	if (it == _rmgRawData.end()){//create new entrance
		auto res = _rmgRawData.emplace(name, *(RmgRawParam*) param);
		if(res.second == false){
			UNLOCK;
			return false;
		}
		UNLOCK;
		return true;
	}
	_rmgRawData[name] = * (RmgRawParam*) param;
	UNLOCK;
	return true;
}

/// \brief This function writes archive data to Storage by parameter name
/// \param name parameter name
/// \param param pointer to structure with parameter description
/// \return true on successful parameter write
bool RmgStorage::setArchiveData(string name,  vector<tRmgParams> paramsVector)
{
	LOCK;
	tStorageDataIt it = _rmgArchiveData.find(name);
	if(it == _rmgArchiveData.end()){ 
		auto res = _rmgArchiveData.emplace(name, paramsVector);
		UNLOCK;
		if(res.second == FALSE){
			return false;
		} 
		return true;
	}
//	(*it).second.clear(); // remove old records
	_rmgArchiveData[name].insert(_rmgArchiveData[name].end(), paramsVector.begin(),paramsVector.end());
	UNLOCK;
	return true;
}

/// \brief This function is used  to get parameter from Storage
/// \param name parameter name
/// \return pointer to structure with parameter description
void* RmgStorage::get(string name)
{
	if (!getConnectionStatus())
		return 0;
	LOCK;
	tRmgParamsIt it = _rmgRawData.find(name);
	if (it == _rmgRawData.end()){
		UNLOCK;
		return 0;
	}
	UNLOCK;
	return (void*)&_rmgRawData[name];
}

/// \brief This function is used  to get parameter from Storage 
///		and return it as char array according to type
/// \param param RMG parameter descriptor
/// \param buf pointer to char array to return parameter
/// \return number of bytes in array
int RmgStorage::getParamValue(RmgRawParam* param, UCHAR* buf)
{
	if (!getConnectionStatus())
		return 0;
	if (param->getType() == PARAM_TYPE_HEX) {
		UCHAR hbuf[8];
		int cnt = param->getBinaryData(hbuf);
		for (int i = 0; i < cnt; i++){
			sprintf((char*) buf + i*2, "%02X", hbuf[i]);
		}
		//int cnt = param->getBinaryData((unsigned char *)buf);
		return cnt*2;
	} else if (param->getType() == PARAM_TYPE_INT) {
		int valInt;
		if (!param->getInt(&valInt)){
			return 0;
		}
		*(int*)buf = valInt;
		return sizeof(int);
	} else if (param->getType() == PARAM_TYPE_DOUBLE) {
		double valDouble;
		if (!param->getDouble(&valDouble)) {
			return 0;
		}
		*(double*)buf = valDouble;
		return sizeof(valDouble);
	} else if (param->getType() == PARAM_TYPE_DATETIME) {
		if(!param->getDateTime((unsigned char *)buf)) {
			return 0;
		}
		return 6;		// size of datetime
	} else {
		string rawval = param->getRawValue();
		memcpy(buf, rawval.c_str(), rawval.size());
		return rawval.size();
	}
	return 0;
}

/// \brief This function is used  to get parameter from Storage 
///		and return it as char array according to type
/// \param name parameter name
/// \param buf pointer to char array to return parameter
/// \return number of bytes in array
int RmgStorage::getValue(string name, UCHAR* buf) {
	double v1, v2;
	int ret;

	if (name.compare("Qsum_yest") == 0) {
		RmgRawParam* param1 = (RmgRawParam*) get("Qn_yest");
		RmgRawParam* param2 = (RmgRawParam*) get("Qnest_yest");
		if (param2==0 || param1==0 || !param1->getDouble(&v1) || !param2->getDouble(&v2)) {
			UNLOCK;
			return 0;
		}
		if(v1+v2 <= 0)
			LOG("Qsum: %f + %f: %f", v1, v2, v1+v2);
		*(double*) buf = v1 + v2;
		ret = sizeof(double);
	} else if (name.compare("Vsum") == 0) {
		RmgRawParam* param1 = (RmgRawParam*) get("V");
		RmgRawParam* param2 = (RmgRawParam*) get("Vest");
		if (param2==0 || param1==0 || !param1->getDouble(&v1) || !param2->getDouble(&v2)) {
			return 0;
		}
		if(v1+v2 <= 0)
			LOG("Vsum: %f + %f: %f", v1, v2, v1+v2);
		*(double*) buf = v1 + v2;
		ret = sizeof(double);
	} else if (name.compare("Vnsum") == 0) {
		RmgRawParam* param1 = (RmgRawParam*) get("Vn");
		RmgRawParam* param2 = (RmgRawParam*) get("Vnest");
		if (param2==0 || param1==0 || !param1->getDouble(&v1) || !param2->getDouble(&v2)) {
			return 0;
		}
		if(v1+v2 <= 0)
			LOG("Vnsum: %f + %f: %f", v1, v2, v1+v2);
		*(double*) buf = v1 + v2;
		ret = sizeof(double);
	} else if (name.compare("Vsum_raising") == 0) {
		RmgRawParam* param1 = (RmgRawParam*) get("V_raising");
		RmgRawParam* param2 = (RmgRawParam*) get("Vest_raising");
		if (param2==0 || param1==0 || !param1->getDouble(&v1) || !param2->getDouble(&v2)) {
			return 0;
		}
		if(v1+v2 <= 0)
			LOG("Vsum_raising: %f + %f: %f", v1, v2, v1+v2);
		*(double*) buf = v1 + v2;
		ret = sizeof(double);
	} else if (name.compare("Vnsum_raising") == 0) {
		RmgRawParam* param1 = (RmgRawParam*) get("Vn_raising");
		RmgRawParam* param2 = (RmgRawParam*) get("Vnest_raising");
		if (param2==0 || param1==0 || !param1->getDouble(&v1) || !param2->getDouble(&v2)) {
			return 0;
		}
		if(v1+v2 <= 0)
			LOG("Vnsum_raising: %f + %f: %f", v1, v2, v1+v2);
		*(double*) buf = v1 + v2;
		ret = sizeof(double);
	} else {
		RmgRawParam* param = (RmgRawParam*) get(name);
		if(!param){ 
			return 0;
		}
		ret = getParamValue(param, buf);
	}
	return ret;
	
}

string RmgStorage::getStringByType(PARAM_TYPE type){
	switch (type)
	{
	case PARAM_TYPE_BYTE:
		return "byte";
	case PARAM_TYPE_DATETIME:
		return "datetime";
	case PARAM_TYPE_DOUBLE:
	case PARAM_TYPE_DOUBLE_FIXED:
	case PARAM_TYPE_DOUBLE_KP:
		return "double";
	case PARAM_TYPE_HEX:
		return "hex";
	case PARAM_TYPE_INT:
	case PARAM_TYPE_BCD:
		return "integer";
	}
	return "string";
}


/// \brief This function converts storage param data to archive param
Json::Value RmgStorage::convertStorageParamToArchiveParam(tRmgParamsIt & recIt)
{
	//int valsize = recIt->second.getSize();
	PARAM_TYPE tp = recIt->second.getType();
	int valsize;
	UCHAR tmpData[64];
	char tmpDataHex[128];
	valsize = getParamValue(&((*recIt).second), tmpData);
	Json::Value param;

	// save name
	param["Name"] = recIt->first.c_str();//string(recIt->first.c_str()+3);		
	param["Type"] = getStringByType(tp);
	// save generated value
	if(tp == PARAM_TYPE_STRING){
		tmpData[valsize] = 0;
		param["Value"] = string((char*)tmpData);
	} else {
		for(int i=0; i<valsize; i++){
			sprintf_s(&tmpDataHex[i*2], sizeof(tmpData)-1-i*2, "%02X", tmpData[i]);
		}
		param["Value"] = string(tmpDataHex);
	}

	return param;
}

//#define DEBUG_TABLE

/// \brief This function is used  to get Archive records from Storage 
/// \param name archive name
/// \return string data of converted archive values
string RmgStorage::getNewArchiveData(Json::Value & jsonAddress)
{
	string outstring;
	Json::Value root;
	Json::Value records(Json::arrayValue);  
	Json::Value record;
	Json::Value params(Json::arrayValue);
	Json::Value param;
	vector<tRmgParams> vparams;

	string archName = jsonAddress["ArchiveName"].asString();
	//if (archName.compare("as") == 0){
		LOCK;
#ifdef DEBUG_TABLE
		string header = "tagid, valueKey";
		string datatable;
		string typestable;
		int row = 0;
		FILE * f;
		fopen_s(&f, (archName + ".txt").c_str(), "wb");
		fprintf(f, "CREATE TABLE %s (tagid int, valueKey varchar(40));\n", ("rmg_" +archName).c_str());
#endif	
		
		archName = "rmg_" + archName;
		vparams = _rmgArchiveData[archName];
		for (UINT recordIndx=0; recordIndx<vparams.size(); recordIndx++){
			//tRmgParams paramsraw = vparams[recordIndx];
			for (tRmgParamsIt recParIt = vparams[recordIndx].begin(); recParIt != vparams[recordIndx].end(); recParIt++) {
				//(*recParIt).first;// name
				//(*recParIt).second;
				param = convertStorageParamToArchiveParam(recParIt);
#ifdef DEBUG_TABLE
					//if(row == 0) {
						header += ", " + recParIt->first + " ";
						//PARAM_TYPE t = recParIt->second.getType();
						//typestable += RmgStorage::getStringByType(t) + "; ";
					//}
					double value;
					int lvalue;
					string svalue;
					char vbuf[20];
					switch(recParIt->second.getType()){
					case PARAM_TYPE_DOUBLE:
						fprintf(f, "ALTER TABLE %s ADD %s float;\n", archName.c_str(), recParIt->first.c_str());
						//header += "float";
						recParIt->second.getDouble(&value);
						sprintf(vbuf, ", %f", value);
						datatable += vbuf;
						break;
					case PARAM_TYPE_INT:
						fprintf(f, "ALTER TABLE %s ADD %s int;\n", archName.c_str(), recParIt->first.c_str());
						//header += "int";
						recParIt->second.getInt(&lvalue);
						sprintf(vbuf, ", %li", lvalue);
						datatable += vbuf;
						break;
					case PARAM_TYPE_STRING:
						fprintf(f, "ALTER TABLE %s ADD %s varchar(40);\n", archName.c_str(), recParIt->first.c_str());
						//header += "varchar";
						svalue = ", '"+recParIt->second.getRawValue() + "'";
						//sprintf(vbuf, "%l; ", value);
						datatable += svalue;
						break;
					default:
						fprintf(f, "ALTER TABLE %s ADD %s varchar(40);\n", archName.c_str(), recParIt->first.c_str());
						//header += "varchar";
						svalue = ", '"+recParIt->second.getRawValue() + "'";
						datatable += svalue;

					}
#endif
				params.append(param);
			}
#ifdef DEBUG_TABLE
			fprintf(f, "INSERT INTO %s(%s) VALUES (%d, %d %s);\n", archName.c_str(), header.c_str(), row, row*10, datatable.c_str());
			row ++;
			datatable.clear();
			header = "tagid, valueKey";
#endif

			record["Params"] = params;
			params.clear();
			records.append(record);
		}
#ifdef DEBUG_TABLE
		fclose(f);
#endif
		if(vparams.size()) {
			char timebuf[20];
			_strtime_s(timebuf);
			LOG("[%s] Archive[%s]: %d records", timebuf, archName.c_str(), vparams.size());
		}
		root["Records"] = records;
		Json::FastWriter writer;
		outstring = writer.write(root);
		_rmgArchiveData[archName].clear();
	//}
	
	UNLOCK;
	return outstring;
}

