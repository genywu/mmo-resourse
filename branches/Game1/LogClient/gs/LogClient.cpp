

#include "../stdafx.h"
#include "LogClient.h"
#include "../../public/GUID.h"
#include "../../nets/netserver/Message.h"


LogClient::LogClient(void)
{
	hConnectThread = NULL;
	bConnectThreadExit = false;

	m_hWirteThread = NULL;
	m_lWirteThreadExit = 0;

	m_thisUsable = false;
	m_BeAffirmed = false;

	m_lClientNo = 0;
}


LogClient::~LogClient(void)
{
	if(m_thisUsable)
	{
		Release();
	}
}


bool LogClient::Init(CMessage *pMsg, LogClientInfo &logClientInfo)
{
	assert(MSG_LOG20_SERVER_TABLE == pMsg->GetType());
	m_lClientNo				= logClientInfo.lClientNo;
	m_ErrFile._file_name	= logClientInfo.ErrLogFileName;
	m_LogFile._file_name	= logClientInfo.LogFileName;
	m_TxtLogFile._file_name	= logClientInfo.TxtLogFileName;

	long bufsize = pMsg->GetLong();

	BYTE	*pData = new BYTE[bufsize + 1];
	memset(pData, 0, bufsize + 1);

	pMsg->GetEx(pData, bufsize);
	ByteArrayToTable(pData, 0);

	delete [] pData;

	InitializeCriticalSection(&(m_ErrFile._critical_section));
	InitializeCriticalSection(&(m_LogFile._critical_section));
	InitializeCriticalSection(&(m_TxtLogFile._critical_section));

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	//! 打开工作线程
	if (0 != m_hWirteThread)
		return false;

	m_lWirteThreadExit = 1;
	unsigned hThread;
	if(0 == (m_hWirteThread = (HANDLE)_beginthreadex(NULL, 0, FileWirteThread, (void*)this, 0, &hThread)))
	{	
		return false;
	}

	m_thisUsable = true;
	return true;
}

bool LogClient::UpdateCondition(CMessage *pMsg)
{
	assert(MSG_LOG20_SERVER_CONDITION == pMsg->GetType());

	//! 清楚过时状态
	for (TableMap_ite ite = m_TableInfo.begin(); ite != m_TableInfo.end(); ++ite)
	{
		ite->second.m_isLog = 1;
		for (long j = 0; j < ite->second.m_vecItem.size(); ++j)
		{
			ite->second.m_vecItem[j].m_conditionId = -1;
		}
	}

	//! 获取新的状态
	long bufsize = pMsg->GetLong();

	BYTE	*pData = new BYTE[bufsize + 1];
	memset(pData, 0, bufsize + 1);

	pMsg->GetEx(pData, bufsize);
	long pos = 0;

	long sendControl_size = _GetLongFromByteArray(pData, pos);
	for (long i = 0; i < sendControl_size; ++i)
	{
		long table_type = _GetLongFromByteArray(pData, pos);
		m_TableInfo[table_type].m_isLog = _GetLongFromByteArray(pData, pos);	
	}

	long condition_size = _GetLongFromByteArray(pData, pos);
	for (long i = 0; i < condition_size; ++i)
	{
		ItemCondition tmpItemCondition;

		tmpItemCondition.m_valueType = _GetLongFromByteArray(pData, pos);
		long table_num = _GetLongFromByteArray(pData, pos);
		for (long j = 0; j < table_num; ++j)
		{
			long table_type = _GetLongFromByteArray(pData, pos);
			long item_index = _GetLongFromByteArray(pData, pos);
			tmpItemCondition.m_vPosList.push_back(pair<long, long>(table_type, item_index));

			LogTable &logTable = m_TableInfo[table_type];
			logTable.m_vecItem[item_index].m_conditionId = (long)m_vItemCondition.size();
		}
		long ConditionTile_num = _GetLongFromByteArray(pData, pos);
		for (long j = 0; j < ConditionTile_num; ++j)
		{
			ConditionTile tmpConditionTile;
			tmpConditionTile.m_eCondition		= _GetLongFromByteArray(pData, pos);
			tmpConditionTile.m_eConditionUse	= _GetLongFromByteArray(pData, pos);
			long value_size = _GetLongFromByteArray(pData, pos);
			_AddToByteArray(&tmpConditionTile.m_vData, pData + pos, value_size);
			pos += value_size;

			tmpItemCondition.m_vConditionTile.push_back(tmpConditionTile);
		}
		m_vItemCondition.push_back(tmpItemCondition);

	}

	delete[]pData;

	return true;
}


void LogClient::PassAffirm(bool bAffirm)
{
	m_BeAffirmed = bAffirm;
}

void LogClient::SetSoket(long lLogSocket)
{
	m_LogSocket = lLogSocket;
}

long LogClient::GetSoket(void)
{
	return m_LogSocket;
}

void LogClient::Release(void)
{
	//! 结束线程
	if (0 != m_hWirteThread)
	{
		m_lWirteThreadExit = 0;
		SetEvent(m_hEvent);

		WaitForSingleObject(m_hWirteThread, INFINITE);
		m_hWirteThread = 0;

		CloseHandle(m_hEvent);
		m_hEvent = 0;
	}

	m_TableInfo.clear();

	if (m_thisUsable)
	{
		DeleteCriticalSection(&(m_ErrFile._critical_section));
		m_ErrFile._buf_is_null = true;
		m_ErrFile._str_buf.clear();
		if (m_ErrFile._ofstream.is_open())
		{
			m_ErrFile._ofstream.close();
		}

		DeleteCriticalSection(&(m_LogFile._critical_section));
		m_LogFile._buf_is_null = true;
		m_LogFile._str_buf.clear();
		if (m_LogFile._ofstream.is_open())
		{
			m_LogFile._ofstream.close();
		}

		DeleteCriticalSection(&(m_TxtLogFile._critical_section));
		m_TxtLogFile._buf_is_null = true;
		m_TxtLogFile._str_buf.clear();
		if (m_TxtLogFile._ofstream.is_open())
		{
			m_TxtLogFile._ofstream.close();
		}

		m_thisUsable = false;
	}
}

//!	发送SQL语句日志
bool LogClient::SendStrSqlLogToServer(const char *pSql)
{
	if(NULL == pSql)
		return false;

	CMessage msg(MSG_02LOG_SERVER_LOG_SQLString);
	msg.Add(m_lClientNo);
	msg.Add(pSql);
	msg.SendToSocket(m_LogSocket);

	return true;
}

//! 发送日志，按配置的类型依次输入参数
//! 参数只允许Long、float、char*、CGUID*四种类型
//! 时间和服务器编号是默认的输入的
bool LogClient::SendLogToServer(long LogType, ...)
{
	TableMap_ite ite = m_TableInfo.find(LogType);
	if (ite == m_TableInfo.end())
		return false;

	LogTable &tmpTableInfo = ite->second;

	//string strSQL;
	va_list args;
	va_start(args, LogType);
	//bool re = GetSQL(args, tmpTableInfo, strSQL);
	vector<BYTE> vByteData;
	//_AddToByteArray(&vByteData, tmpTableInfo.m_tableType);

	bool re = false;

	re = GetLogByteArray(args, tmpTableInfo, vByteData);

	va_end(args);

	if (re)
	{
		bool log_send	= false;
		bool log_here	= false;
		bool log_err	= false;

		do
		{
			switch(tmpTableInfo.m_SendControl)
			{
			case eSC_LOGSERVER:
				if (0 != m_LogSocket && m_BeAffirmed)
				{
					log_send = true;
				}
				else
				{
					log_err = true;
				}
				break;
			case eSC_HERE:
				log_here = true;
				break;
			case sSC_SERVER_AND_HERE:
				if (0 != m_LogSocket && m_BeAffirmed)
				{
					log_send = true;
				}
				else
				{
					log_err = true;
				}
				log_here = true;
				break;
			default:
				re = false;
			}
		}
		while (false);

		if (log_send)
		{
			CMessage msg(MSG_02LOG_SERVER_LOG);
			msg.Add(m_lClientNo);
			msg.Add(LogType);
			msg.Add((long)vByteData.size());
			msg.AddEx((void*)&(vByteData[0]), (long)vByteData.size());
			msg.SendToSocket(m_LogSocket);
			re = true;
		}

		/*if (log_here)
		{
			char szStr[10240] = {0};
			for (long i = 0; i < vByteData.size(); ++i)
			{
				sprintf(szStr + 2 * i, "%d,", vByteData[i]);
			}
			m_LogFile.WirteToBuf(string(szStr));
			SetEvent(m_hEvent);
			re = true;
		}

		if (log_err)
		{
			char szStr[10240] = {0};
			for (long i = 0; i < vByteData.size(); ++i)
			{
				sprintf(szStr + i, "%d,", vByteData[i]);
			}
			m_ErrFile.WirteToBuf(string(szStr));
			SetEvent(m_hEvent);
			re = true;
		}*/
	}

	return re;
}



//! 在本地记录服务器格式日志（时间自动添加）
bool LogClient::LogToHere(long LogType, ...)
{
	TableMap_ite ite = m_TableInfo.find(LogType);
	if (ite == m_TableInfo.end())
		return false;
	if (eSC_LOGSERVER == ite->second.m_SendControl)
	{
		return true;
	}

	LogTable &tmpTableInfo = ite->second;

	//string strSQL;
	va_list args;
	va_start(args, LogType);
	//bool re = GetSQL(args, tmpTableInfo, strSQL);
	vector<BYTE> vByteData;
	_AddToByteArray(&vByteData, tmpTableInfo.m_tableType);

	bool re = false;

	//如果是release版，则捕捉异常
#ifndef _DEBUG
	try
	{
#endif
		re = GetLogByteArray(args, tmpTableInfo, vByteData);
#ifndef _DEBUG
	}
	catch (...)
	{
		LogTxtToHere("在记录日志类型%d时发生了数据类型异常！！！\r\n");
		return re;
	}
#endif

	va_end(args);

	char szStr[10240] = {0};
	for (long i = 0; i < vByteData.size(); ++i)
	{
		_snprintf(szStr + 2 * i, 10240 - 2 * i, "%d,", vByteData[i]);
	}
	m_LogFile.WirteToBuf(string(szStr));
	SetEvent(m_hEvent);
	//m_LogFile.WirteToBuf(strSQL + string("/r/n"));

	return true;
}


//! 在本地记录文本格式日志（时间自动添加）
bool LogClient::LogTxtToHere(const char *pTxt)
{
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	char szTime[128] = {0};
	_snprintf(szTime, 128, "[%02d-%02d %02d:%02d:%02d] ", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	m_TxtLogFile.WirteToBuf(string(szTime) + string(pTxt));
	SetEvent(m_hEvent);
	return true;
}

//! 设置日志的保存模式
bool LogClient::SetSaveMode(long lLogType, eSendControl eSC)
{
	TableMap_ite ite = m_TableInfo.find(lLogType);
	if (ite == m_TableInfo.end())
		return false;
	if ( eSC_LOGSERVER > eSC || eSC_INVALID <= eSC)
		return false;

	ite->second.m_SendControl = eSC;
	return true;
}

//! 从数组中获得表结构
void LogClient::ByteArrayToTable(BYTE *pData, long pos)
{
	m_TableInfo.clear();

	long tableNum = _GetLongFromByteArray(pData, pos);

	for (long i = 0; i < tableNum; ++i)
	{
		LogTable tmpTableInfo;

		_GetBufferFromByteArray(pData, pos, (void*)&tmpTableInfo, LogTableBaseDataSize);
		long itemNum = _GetLongFromByteArray(pData, pos);

		for (long i = 0; i < itemNum; ++i)
		{
			LogItem tmpLogItem;
			_GetBufferFromByteArray(pData, pos, (void*)&tmpLogItem, sizeof(LogItem));
			tmpTableInfo.m_vecItem.push_back(tmpLogItem);
		}
		m_TableInfo[tmpTableInfo.m_tableType] = tmpTableInfo;
	}
}


//! 把日志参数压入BYTE数组
bool LogClient::GetLogByteArray(va_list& args, LogTable &tableInfo, vector<BYTE> &vData)
{
	if (1 == tableInfo.m_isLog)
	{
		for (long i = 0; i < tableInfo.m_vecItem.size(); ++i)
		{
			if(!ParamToByteArray(args, tableInfo.m_vecItem[i], vData))
			{
				return false;
			}
		}
	}
	else
	{
		long goon = 0;
		for (long i = 0; i < tableInfo.m_vecItem.size(); ++i)
		{
			long re = 0;
			if(-1 == (re = ParamToByteArray_AndTestCondition(args, tableInfo.m_vecItem[i], vData)))
			{
				return false;
			}
			goon += re;
		}

		if (0 == goon)
		{
			return false;
		}
	}

	return true;
}

//! 把输入的参数值按类型压入BYTE数组
//! 参数args必须是va_start之后的
//! 参数lType类型为eDataTypeCode
bool LogClient::ParamToByteArray(va_list& args, const LogItem& logItem, vector<BYTE> &vData)
{
	switch(logItem.m_eType)
	{
	case eDTC_INT32:
		{
			long var = va_arg(args, long);
			_AddToByteArray(&vData, var);
		}
		break;
	case eDTC_FLOAT:
		{
			float *var = va_arg(args, float*);
			_AddToByteArray(&vData, *var);
		}
		break;
	case eDTC_GUID:
		{
			CGUID* var = va_arg(args, CGUID*);
			char szGUID[128] = {0};
			var->tostring(szGUID);
			_AddToByteArray(&vData, (CGUID)*var);
		}
		break;
	case eDTC_DATE:
	case eDTC_STR128:
	case eDTC_STR512:
		{
			char* var = va_arg(args, char*);
#ifdef _DEBUG
			if(eDTC_STR128 == logItem.m_eType)
			{
				assert(128 > strlen(var));
			}
			else
			{
				assert(512 > strlen(var));
			}
#endif	
			_AddToByteArray(&vData, (LONG)strlen(var));
			_AddToByteArray(&vData, var, (LONG)strlen(var));
		}
		break;
	default:
		return false;
		break;
	}

	return true;
}


//! 同上，但要进行附加条件检测
//! RE: -1，执行失败； 0，无须检测或检测失败；1，检测通过
long LogClient::ParamToByteArray_AndTestCondition(va_list& args, const LogItem& logItem, vector<BYTE> &vData)
{
	long return_value = 0;

	switch(logItem.m_eType)
	{
	case eDTC_INT32:
		{
			long var = va_arg(args, long);
			if (logItem.m_conditionId != -1)
			{
				return_value = TestCondition(m_vItemCondition[logItem.m_conditionId], (void*)&var);
				if(0 > return_value) return -1;
			}
			_AddToByteArray(&vData, var);
		}
		break;
	case eDTC_FLOAT:
		{
			float *var = va_arg(args, float*);
			if (logItem.m_conditionId != -1)
			{
				return_value = TestCondition(m_vItemCondition[logItem.m_conditionId], (void*)var);
				if(0 > return_value) return -1;
			}
			_AddToByteArray(&vData, *var);
		}
		break;
	case eDTC_GUID:
		{
			CGUID* var = va_arg(args, CGUID*);
			char szGUID[128] = {0};
			var->tostring(szGUID);
			if (logItem.m_conditionId != -1)
			{
				return_value = TestCondition(m_vItemCondition[logItem.m_conditionId], (void*)var);
				if(0 > return_value) return -1;
			}
			_AddToByteArray(&vData, (CGUID)*var);
		}
		break;
	case eDTC_DATE:
	case eDTC_STR128:
	case eDTC_STR512:
		{
			char* var = va_arg(args, char*);
#ifdef _DEBUG
			if(eDTC_STR128 == logItem.m_eType)
			{
				assert(128 > strlen(var));
			}
			else
			{
				assert(512 > strlen(var));
			}
#endif	
			if (logItem.m_conditionId != -1)
			{
				return_value = TestCondition(m_vItemCondition[logItem.m_conditionId], (void*)var);
				if(0 > return_value) return -1;
			}
			_AddToByteArray(&vData, (LONG)strlen(var));
			_AddToByteArray(&vData, var, (LONG)strlen(var));
		}
		break;
	default:
		return -1;
		break;
	}

	return return_value;
}

//
////! 生成SQL语句]
////! 参数args必须是va_start之前的
//bool LogClient::GetSQL(va_list& args, LogTable &tableInfo, string &strValue)
//{
//	string strItemList;
//	string strValueList;
//
//	for (long i = 0; i < tableInfo.m_vecItem.size(); ++i)
//	{
//		if (i != tableInfo.m_vecItem.size() - 1)
//		{
//			strItemList += string(tableInfo.m_vecItem[i].m_szItemName) + ",";
//			string strItem;
//			if (eSC_LOGSERVER == tableInfo.m_SendControl)
//			{
//				if(!GetValueByType_WithDB(args, tableInfo.m_vecItem[i], strItem))
//					return false;
//			}
//			else
//			{
//				if(!GetValueByType_AndTestCondition_WithDB(args, tableInfo.m_vecItem[i], strItem))
//					return false;
//			}
//			
//			strValueList += strItem + ",";
//		}
//		else
//		{
//			strItemList += string(tableInfo.m_vecItem[i].m_szItemName);
//			string strItem;
//			if (eSC_LOGSERVER == tableInfo.m_SendControl)
//			{
//				if(!GetValueByType_WithDB(args, tableInfo.m_vecItem[i], strItem))
//					return false;
//			}
//			else
//			{
//				if(!GetValueByType_AndTestCondition_WithDB(args, tableInfo.m_vecItem[i], strItem))
//					return false;
//			}
//			strValueList += strItem;
//		}
//	}
//
//	strValue.clear();
//	strValue = "INSERT INTO " + string(tableInfo.m_szTableName) + " (" + strItemList + ") VALUES (" + strValueList + ");";
//	return true;
//}
//
////! 根据输入的类型从参数中获得一个DB格式的值的字符串（自动添加单引号）
////! 参数args必须是va_start之后的
////! 参数lType类型为eDataTypeCode
//bool	LogClient::GetValueByType_WithDB(va_list& args, const LogItem& logItem, string &strValue)
//{
//	char szTmp[513] = {0};
//	switch(logItem.m_eType)
//	{
//	case eDTC_INT32:
//	case eDTC_DATE:
//		{
//			long var = va_arg(args, long);
//			sprintf_s(szTmp, "%d", var);
//		}
//		break;
//	case eDTC_FLOAT:
//		{
//			float var = va_arg(args, float);
//			sprintf_s(szTmp, "%f", var);
//		}
//		break;
//	case eDTC_GUID:
//		{
//			CGUID* var = va_arg(args, CGUID*);
//			char szGUID[128] = {0};
//			var->tostring(szGUID);
//			sprintf_s(szTmp, "'%s'", szGUID);
//		}
//	    break;
//	case eDTC_STR128:
//	case eDTC_STR512:
//		{
//			char* var = va_arg(args, char*);
//#ifdef _DEBUG
//			if(eDTC_STR128 == logItem.m_eType)
//			{
//				assert(128 > strlen(var));
//			}
//			else
//			{
//				assert(512 > strlen(var));
//			}
//#endif	
//			sprintf_s(szTmp, "'%s'", var);
//		}
//		break;
//	default:
//		return false;
//	    break;
//	}
//	strValue.clear();
//	strValue = string(szTmp);
//	return true;
//}
//
////!							根据输入的类型从参数中获得一个DB格式的值的字符串，在执行过程中进行条件检测（自动添加单引号）
//bool	LogClient::GetValueByType_AndTestCondition_WithDB(va_list& args, const LogItem& logItem, string &strValue)
//{
//	char szTmp[513] = {0};
//	switch(logItem.m_eType)
//	{
//	case eDTC_INT32:
//	case eDTC_DATE:
//		{
//			long var = va_arg(args, long);
//			if (logItem.m_conditionId != -1)
//			{
//				if(!TestCondition(m_vItemCondition[logItem.m_conditionId], (void*)&var))
//				{
//					return false;
//				}
//			}
//			sprintf_s(szTmp, "%d", var);
//		}
//		break;
//	case eDTC_FLOAT:
//		{
//			float var = va_arg(args, float);
//			if (logItem.m_conditionId != -1)
//			{
//				if(!TestCondition(m_vItemCondition[logItem.m_conditionId], (void*)&var))
//				{
//					return false;
//				}
//			}
//			sprintf_s(szTmp, "%f", var);
//		}
//		break;
//	case eDTC_GUID:
//		{
//			CGUID* var = va_arg(args, CGUID*);
//			char szGUID[128] = {0};
//			var->tostring(szGUID);
//			if (logItem.m_conditionId != -1)
//			{
//				if(!TestCondition(m_vItemCondition[logItem.m_conditionId], (void*)var))
//				{
//					return false;
//				}
//			}
//			sprintf_s(szTmp, "'%s'", szGUID);
//		}
//		break;
//	case eDTC_STR128:
//	case eDTC_STR512:
//		{
//			char* var = va_arg(args, char*);
//#ifdef _DEBUG
//			if(eDTC_STR128 == logItem.m_eType)
//			{
//				assert(128 > strlen(var));
//			}
//			else
//			{
//				assert(512 > strlen(var));
//			}
//#endif	
//			if (logItem.m_conditionId != -1)
//			{
//				if(!TestCondition(m_vItemCondition[logItem.m_conditionId], (void*)var))
//				{
//					return false;
//				}
//			}
//			sprintf_s(szTmp, "'%s'", var);
//		}
//		break;
//	default:
//		return false;
//		break;
//	}
//	strValue.clear();
//	strValue = string(szTmp);
//	return true;
//}

long LogClient::TestCondition(const ItemCondition &condition, void *pCurrValue)
{

	long intersection_num = 0;
	long intersection_testSucceed_num = 0;

	for (long i = 0; i < condition.m_vConditionTile.size(); ++i)
	{
		void* pTestValue = (void*)(&(condition.m_vConditionTile[i].m_vData[0]));
		bool test_result = false;
		switch(condition.m_valueType)
		{
		case eDTC_INT32:
			{
				switch(condition.m_vConditionTile[i].m_eCondition)
				{
				case eC_Even:
					if (*((long*)pTestValue) == *((long*)pCurrValue))
						test_result = true;
					break;
				case eC_More_Than:
					if (*((long*)pTestValue) < *((long*)pCurrValue))
						test_result = true;
					break;
				case eC_Less_Than:
					if (*((long*)pTestValue) > *((long*)pCurrValue))
						test_result = true;
					break;
				default:
					return -1;
				}
			}
			break;
		case eDTC_FLOAT:
			{
				switch(condition.m_vConditionTile[i].m_eCondition)
				{
				case eC_Even:
					if (*((float*)pTestValue) == *((float*)pCurrValue))
						test_result = true;
					break;
				case eC_More_Than:
					if (*((float*)pTestValue) < *((float*)pCurrValue))
						test_result = true;
					break;
				case eC_Less_Than:
					if (*((float*)pTestValue) > *((float*)pCurrValue))
						test_result = true;
					break;
				default:
					return -1;
				}		
			}
			break;
		case eDTC_GUID:
			{
				switch(condition.m_vConditionTile[i].m_eCondition)
				{
				case eC_Even:
					if(0 == memcmp(pTestValue, pCurrValue, sizeof(CGUID)))
						test_result = true;
					break;
				case eC_More_Than:
				case eC_Less_Than:
				default:
					return -1;
				}
			}
			break;
		case eDTC_DATE:
		case eDTC_STR128:
		case eDTC_STR512:
			{
				switch(condition.m_vConditionTile[i].m_eCondition)
				{
				case eC_Even:
					if(0 == stricmp((char*)pTestValue, (char*)pCurrValue))
						test_result = true;
					break;
				case eC_More_Than:
					if(0 > stricmp((char*)pTestValue, (char*)pCurrValue))
						test_result = true;
					break;
				case eC_Less_Than:
					if(0 < stricmp((char*)pTestValue, (char*)pCurrValue))
						test_result = true;
					break;
				default:
					return -1;
				}
			}
			break;
		}

		if (eCU_INTERSECTION == condition.m_vConditionTile[i].m_eConditionUse)
		{
			++intersection_num;
			if (test_result)
			{
				++intersection_testSucceed_num;
			}
		}
		else if (eCU_UNION == condition.m_vConditionTile[i].m_eConditionUse)
		{
			if (test_result)
			{
				return 1;
			}
		}
		else
		{
			return -1;
		}
	}

	if (0 != intersection_num && intersection_testSucceed_num == intersection_num)
	{
		return 1;
	}
	else
	{
		return 0;
	}

	return -1;
}


//!						同步写m_TxtLogFileName文件
unsigned __stdcall	LogClient::FileWirteThread(void* pThis)
{
	LogClient *pLogClient = (LogClient*)pThis;

	while (true)
	{
		WaitForSingleObject(pLogClient->m_hEvent, INFINITE);
		pLogClient->m_ErrFile.BufToFile();
		pLogClient->m_LogFile.BufToFile();
		pLogClient->m_TxtLogFile.BufToFile();

		if (0 == pLogClient->m_lWirteThreadExit)
			break;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
void LogClient::LogFile_ForThread::WirteToBuf(string &strTxt)
{
	EnterCriticalSection(&_critical_section);
	_str_buf += strTxt + string("\r\n");
	_buf_is_null = false;
	LeaveCriticalSection(&_critical_section);
}

long LogClient::LogFile_ForThread::BufToFile(void)
{
	if (0 == _str_buf.size())
	{
		return 1;
	}
	if(!_ofstream.is_open())
	{
		if(!OpenStream())
		{
			if(1024 * 1024 <  _str_buf.size())
			{
				_str_buf.clear();
				PutLogInfo("本地日志文件不能写入，超过缓冲数量，进行清除");
			}
			return 1;
		}
	}
	string tmp;
	EnterCriticalSection(&_critical_section);
	tmp = _str_buf;
	_str_buf.clear();
	_buf_is_null = true;
	LeaveCriticalSection(&_critical_section);

	_ofstream<<tmp;
	return 0;
}

bool LogClient::LogFile_ForThread::OpenStream(void)
{
	if (_ofstream.is_open())
	{
		return true;
	}

	_ofstream.open(_file_name.c_str(), ios::app|ios::_Nocreate);

	return _ofstream.is_open();
}

