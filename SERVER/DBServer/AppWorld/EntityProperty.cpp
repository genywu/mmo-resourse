#include "stdafx.h"
#include "EntityProperty.h"
#include "..\..\..\nets\netdb\message.h"
#include "..\..\..\dbaccess\worlddb\DataEntityOperator.h"

CWrapDataObject::CWrapDataObject()
: m_ID(0),m_Name("WDO"),m_DataType(DATA_OBJECT_UNKNOWN)
{

}
CWrapDataObject::CWrapDataObject(DATA_OBJECT_TYPE type, long id, std::string& name)
: m_ID(id),m_Name(name),m_DataType(type)
{

}
CWrapDataObject::~CWrapDataObject()
{
	ValueInfoMapItr itr = m_mapDataValue.begin();
	for(; itr != m_mapDataValue.end(); itr++)
	{
		if(itr->second->buf)
		{
			delete[] itr->second->buf;
			itr->second->buf = NULL;
		}
	}
}

// 标志操作
void CWrapDataObject::PushBackOperValuePosFlag(DB_OPERATION_TYPE op, WORD pos)
{
	switch(op)
	{
	case DB_OPER_LOAD:
		m_LoadValuePosFlag.insert(m_LoadValuePosFlag.end(), pos);
		break;
	case DB_OPER_SAVE:
		m_SaveValuePosFlag.insert(m_SaveValuePosFlag.end(), pos);
		break;
	}
}
WORD CWrapDataObject::PopFrontOperValuePosFlag(DB_OPERATION_TYPE op)
{
	WORD num = 0;
	switch(op)
	{
	case DB_OPER_LOAD:
		{
			num = *m_LoadValuePosFlag.begin();
			m_LoadValuePosFlag.erase(m_LoadValuePosFlag.begin());
		}
			break;
	case DB_OPER_SAVE:
		{
			num = *m_SaveValuePosFlag.begin();
			m_SaveValuePosFlag.erase(m_SaveValuePosFlag.begin());
		}
		break;
	}
	return num;
}
// 清空队列
void CWrapDataObject::ClearOperValuePosFlag(DB_OPERATION_TYPE op)
{
	switch(op)
	{
	case DB_OPER_LOAD:
		{
			m_LoadValuePosFlag.clear();
		}
		break;
	case DB_OPER_SAVE:
		{
			m_SaveValuePosFlag.clear();
		}
		break;
	}
}
// num
long CWrapDataObject::GetOperValuePosFlagNum(DB_OPERATION_TYPE op)
{
	long num = 0;
	switch(op)
	{
	case DB_OPER_LOAD:
		num = m_LoadValuePosFlag.size();
			break;
	case DB_OPER_SAVE:
		num = m_SaveValuePosFlag.size();
		break;
	}
	return num;
}
//	ADD------------------------------------------------------------
bool CWrapDataObject::AddValueArray(long pos, DATA_OBJECT_TYPE dataType, void* buf, long size)
{
	ValueInfo* vi = NULL;
	ValueInfoMapItr itr = m_mapDataValue.find(pos);
	if(itr != m_mapDataValue.end())
	{
		vi = itr->second;
	}
	else
	{
		vi = new ValueInfo;
		m_mapDataValue[pos] = vi;
	}

	vi->dataType = dataType;
	vi->pos = pos;
	vi->size = size;
	BYTE* tBuf = new BYTE[size];
	memcpy_s(tBuf, size, vi->buf, size);

	return true;
	/*switch(dataType)
	{
	case DATA_OBJECT_LONG:
		
		break;
	case DATA_OBJECT_SHORT:
		break;
	case DATA_OBJECT_ULONG:
		break;
	case DATA_OBJECT_USHORT:
		break;
	case DATA_OBJECT_BOOL:
		break;
	case DATA_OBJECT_CHAR:
		break;
	case DATA_OBJECT_BYTE:
		break;
	case DATA_OBJECT_WORD:
		break;
	case DATA_OBJECT_DWORD:
		break;
	case DATA_OBJECT_STRING:
		break;

	case DATA_OBJECT_LONG_ARRAY:
		break;
	case DATA_OBJECT_SHORT_ARRAY:
		break;
	case DATA_OBJECT_ULONG_ARRAY:
		break;
	case DATA_OBJECT_USHORT_ARRAY:
		break;
	case DATA_OBJECT_BOOL_ARRAY:
		break;
	case DATA_OBJECT_CHAR_ARRAY:
		break;
	case DATA_OBJECT_BYTE_ARRAY:
		break;
	case DATA_OBJECT_WORD_ARRAY:
		break;
	case DATA_OBJECT_DWORD_ARRAY:
		break;

	case DATA_OBJECT_STRING_ARRAY:
		break;
	case DATA_OBJECT_BUFFER_ARRAY:
		break;
	}*/
}

//	DEL------------------------------------------------------------
bool CWrapDataObject::DelValueArray(long pos)
{
	ValueInfo* vi = NULL;
	ValueInfoMapItr itr = m_mapDataValue.find(pos);
	if(itr != m_mapDataValue.end())
	{
		if(itr->second->buf)
		{
			delete[] itr->second->buf;
			itr->second->buf = NULL;
		}

		m_mapDataValue.erase(itr);

		return true;
	}

	return false;
}
//	GET------------------------------------------------------------
CWrapDataObject::ValueInfo* CWrapDataObject::GetValueArray(long pos)
{
	ValueInfo* vi = NULL;
	ValueInfoMapItr itr = m_mapDataValue.find(pos);
	if(itr != m_mapDataValue.end())
	{
		if(itr->second)
		{
			vi = itr->second;
		}
	}

	return vi;
}

// FIND------------------------------------------------------------
long CWrapDataObject::GetValueArray(CWrapDataObject::ValueInfo& valueInfo)
{
	long ret = INVILID_ENTITY_ID;
	ValueInfoMapItr itr = m_mapDataValue.find(valueInfo.pos);
	if(itr != m_mapDataValue.end())
	{
		if(*itr->second == valueInfo)
			ret = (*itr).second->pos;
	}

	return ret;
}
//////////////////////////////////////////////////////////////////////////
CEntityProperty::CEntityProperty(long id, std::string& name)
:m_ID(id), m_Name(name),m_CurDOID(0)
{

}
CEntityProperty::~CEntityProperty()
{
	WrapDataObjectMapItr itr = m_mapDataObject.begin();
	for(; itr != m_mapDataObject.end(); itr++)
	{
		if(itr->second)
			SAFE_DELETE(itr->second);
	}

	m_mapDataObject.clear();
}

// Add DataObject
bool CEntityProperty::AddDataObject(CWrapDataObject* dataObj)
{
	if(dataObj)
	{
		WrapDataObjectMapItr itr = m_mapDataObject.find(dataObj->GetName());
		if(itr == m_mapDataObject.end()) // 未找到
		{
			m_mapDataObject[dataObj->GetName()] = dataObj;
		}
		else
		{
			delete itr->second;
			itr->second = dataObj;
		}
		return true;
	}
	return false;
}
// Add DataObject
bool CEntityProperty::AddDataObject(DATA_OBJECT_TYPE type, std::string& objName)
{
	CWrapDataObject* dataObj = NULL;
	
	WrapDataObjectMapItr itr = m_mapDataObject.find(objName);
	if(itr == m_mapDataObject.end()) // 未找到
	{
		dataObj = new CWrapDataObject(type, AllocDataObjectID(), objName);
		m_mapDataObject[objName] = dataObj;

		return true;
	}

	return false;
}

// Get DataObject
CWrapDataObject* CEntityProperty::GetDataObject(std::string& objName)
{
	CWrapDataObject* dataObj = NULL;

	WrapDataObjectMapItr itr = m_mapDataObject.find(objName);
	if(itr != m_mapDataObject.end()) // 未找到
	{
		dataObj = itr->second;
	}

	return dataObj;
}

// Del DataObject
bool CEntityProperty::DelDataObject(std::string& objName)
{
	WrapDataObjectMapItr itr = m_mapDataObject.find(objName);
	if(itr != m_mapDataObject.end()) // 未找到
	{
		delete itr->second;
		m_mapDataObject.erase(itr);
	}

	return false;
}
// 分配一个ID
long CEntityProperty::AllocDataObjectID()
{
	if(m_FreeDOID.size())
	{
		long id = *(m_FreeDOID.begin());
		m_FreeDOID.erase(m_FreeDOID.begin());
		return id;
	}
	return ++m_CurDOID;
}
// 释放一个ID
void CEntityProperty::FreeDataObjectID(long id)
{
	std::list<long>::iterator itr = std::find(m_FreeDOID.begin(), m_FreeDOID.end(), id);
	// 未找到该ID并且ID小于最大值
	if( (itr == m_FreeDOID.end()) && (id < MAX_DEFINED_ID) ) 
		m_FreeDOID.push_back(id);
}
// 标志操作
// 从队列后加入
void CEntityProperty::PushBackOperDataObjFlag(DB_OPERATION_TYPE op, string& name)
{
	switch(op)
	{
	case DB_OPER_LOAD:
		m_LoadDataObjFlag.insert(m_LoadDataObjFlag.end(), name);
		break;
	case DB_OPER_SAVE:
		m_SaveDataObjFlag.insert(m_SaveDataObjFlag.end(), name);
		break;
	}
}
// 从队列前弹出
string CEntityProperty::PopFrontOperDataObjFlag(DB_OPERATION_TYPE op)
{
	string name = "";

	switch(op)
	{
	case DB_OPER_LOAD:
		{
			name = *(m_LoadDataObjFlag.begin());
			m_LoadDataObjFlag.erase(m_LoadDataObjFlag.begin());
		}
		break;
	case DB_OPER_SAVE:
		{
			name = *(m_SaveDataObjFlag.begin());
			m_SaveDataObjFlag.erase(m_SaveDataObjFlag.begin());
		}
		break;
	}
	return name;
}
// num
long CEntityProperty::GetOperDataObjFlagNum(DB_OPERATION_TYPE op)
{
	long num = 0;
	switch(op)
	{
	case DB_OPER_LOAD:
		num = m_LoadDataObjFlag.size();
		break;
	case DB_OPER_SAVE:
		num = m_SaveDataObjFlag.size();
		break;
	}
	return num;
}
//////////////////////////////////////////////////////////////////////////
CDataEntityManager::CDataEntityManager()
: m_CurEPID(0)
{
	m_mapEntityProperty.clear();
}
CDataEntityManager::~CDataEntityManager()
{
	EntityPropertyMapItr itr = m_mapEntityProperty.begin();
	for(; itr != m_mapEntityProperty.end(); itr++)
	{
		if(itr->second)
			SAFE_DELETE(itr->second);
	}

	m_mapEntityProperty.clear();
}

// 创建数据对象
CEntityProperty* CDataEntityManager::CreateEntityProperty(std::string& name)
{
	CEntityProperty* ep = NULL;
	try
	{
		ep = new CEntityProperty(AllocEntityPropertyID(), name);
		return ep;
	}
	catch(...)
	{
		SAFE_DELETE(ep);
		AddErrorLogText("create EntityProperty Error!");
	}
}

// 删除数据对象
bool CDataEntityManager::DelEntityProperty(std::string& name)
{
	EntityPropertyMapItr itr = m_mapEntityProperty.find(name);
	if(itr != m_mapEntityProperty.end())
	{
		SAFE_DELETE(itr->second);
		m_mapEntityProperty.erase(itr);
		return true;
	}

	return false;
}

// 取得数据对象
CEntityProperty* CDataEntityManager::GetEntityProperty(std::string& name)
{
	CEntityProperty* ep = NULL;
	EntityPropertyMapItr itr = m_mapEntityProperty.find(name);
	if(itr != m_mapEntityProperty.end())
	{
		ep = itr->second;
	}

	return ep;
}

// 处理消息
void CDataEntityManager::ProcessMsg(CMessage* pMsg)
{
	// 取得Oper Entity Flag 0:1个, 1:多个
	BYTE opEntityFlag = pMsg->GetByte();

	// DB操作类型
	DB_OPERATION_TYPE op = (DB_OPERATION_TYPE)pMsg->GetByte();

	// MSGID
	long msgid = pMsg->GetLong();

	// Entity Num
	long entityNum = pMsg->GetWord();

	// 消息数据块
	BYTE msgBuf[102400];
	// pos
	long msgBufPos = 0;

	// 获取MsgBuf
	pMsg->GetEx(msgBuf, 102400);

	CMessage msg(MSG_DB2W_OPER_CARD);
	string tstr = "";

	switch(pMsg->GetType())
	{
		//////////////////////////////////////////////////////////////////////////
	case MSG_W2DB_OPER_CARD:
		{
			msg.SetType(MSG_DB2W_OPER_CARD);
			tstr = "Card";
		}
		break;
	case MSG_W2DB_OPER_CITY_WAR:
		{
			msg.SetType(MSG_W2DB_OPER_CITY_WAR);
			tstr = "CityWar";
		}
		break;
	case MSG_W2DB_OPER_COUNTRY:
		{
			msg.SetType(MSG_W2DB_OPER_COUNTRY);
			tstr = "Country";
		}
		break;

	case MSG_W2DB_OPER_ENEMY_FACTION:
		{
			msg.SetType(MSG_DB2W_OPER_ENEMY_FACTION);
			tstr = "EnemyFaction";
		}
		break;
	case MSG_W2DB_OPER_FACTION:
		{
			msg.SetType(MSG_DB2W_OPER_FACTION);
			tstr = "Faction";
		}
		break;
	case MSG_W2DB_OPER_GOOD:
		{
			msg.SetType(MSG_DB2W_OPER_GOOD);
			tstr = "Good";
		}
		break;
	case MSG_W2DB_OPER_PLAYER:
		{
			msg.SetType(MSG_DB2W_OPER_PLAYER);
			tstr = "Player";
		}
		break;
	case MSG_W2DB_OPER_REGION:
		{
			msg.SetType(MSG_DB2W_OPER_REGION);
			tstr = "Region";
		}
		break;
	case MSG_W2DB_OPER_SETUP:
		{
			msg.SetType(MSG_DB2W_OPER_SETUP);
			tstr = "Setup";
		}
		break;
	case MSG_W2DB_OPER_UNION:
		{
			msg.SetType(MSG_DB2W_OPER_UNION);
			tstr = "Union";
		}
		break;
	case MSG_W2DB_OPER_VILLAGEWAR:
		{
			msg.SetType(MSG_DB2W_OPER_VILLAGEWAR);
			tstr = "VillageWar";
		}
		break;
	}

	msg.Add((BYTE)opEntityFlag);// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
	msg.Add((BYTE)op);		  // DBOP Type:     1byte, 数据库操作类型（枚举值）
	msg.Add(msgid);		  // MSG ID:        4byte, 该消息的MSGID
	msg.Add((WORD)entityNum);	  // Entity Num:	2byte, 该实体个数(X)

	vector<BYTE> pBA;

	// 操作表
	CEntityProperty* ep = NULL;
	for(int i=0; i<entityNum; i++)
	{
		// EntityID
		long entityid = _GetLongFromByteArray(msgBuf, msgBufPos);

		_AddToByteArray(&pBA, entityid); // EntityID:		4byte, 实体ID
		EntityPropertyMapItr itr = m_mapEntityProperty.find(tstr);
		if(itr != m_mapEntityProperty.end())
		{
			ep = itr->second;
		}
		else
		{
			ep = new CEntityProperty(entityid);
			m_mapEntityProperty[tstr] = ep;	
		}

		ProcessMsg(ep, op, msgid, msgBuf, msgBufPos, pBA);
	}

	msg.AddEx(&pBA[0], pBA.size());
	msg.SendToSocket(pMsg->GetSocketID());
}
// 处理消息
void CDataEntityManager::ProcessMsg(CEntityProperty* ep, DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long msgBufPos, vector<BYTE>& pBA)
{
/*
|OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
|DBOP Type:     1byte, 数据库操作类型（枚举值）
|MSG ID:        4byte, 该消息的MSGID
|Entity Num:	2byte, 该实体个数(X)
---------------从此处开始使用 _AddToByteArray和_GetXXXXByArray 接口编解码----------------
|	|----
|   |EntityID:		4byte, 实体ID
|	|Attr Num:		2byte, 属性种类个数(N)
|	|	 |----
|	|	 |该属性个数:		2byte, 属性数据个数(M)
|X*	|N * |该属性类型:		4byte, 属性类型枚举值
|	|	 |该属性ID:			4byte, 属性ID
|	|	 |该属性名字:		nbyte, 属性名字,字符串
|	|	 |		|----
|	|	 |		|该属性POS:		2byte, 属性POS, 
|	|	 |	M *	|该属性数据块:	nbtye, 该数据块长度由属性数据长度决定 (若消息为LOAD则该数据块为1byte，值为0)
|	|	 |		|----
|	|	 |----
|	|----
*/
	WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos); //Attr Num
	_AddToByteArray(&pBA, attrNum); // Attr Num:		2byte, 属性种类个数(N)

	char tableName[128];
	sprintf(tableName, "%s_Table", ep->GetName());

	char epDataName[128]; // 该属性名字
	for(WORD j=0; j<attrNum; j++)
	{
		CWrapDataObject* obj = NULL;
		WORD epDataNum = _GetWordFromByteArray(msgBuf, msgBufPos); //该属性个数

		DATA_OBJECT_TYPE type = (DATA_OBJECT_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos); // 该属性类型
		
		long epDataID = _GetLongFromByteArray(msgBuf, msgBufPos); // 该属性ID
	
		_GetStringFromByteArray(msgBuf, msgBufPos, epDataName); // 该属性名字
		
		// 添加Load数据
		_AddToByteArray(&pBA, epDataNum);	// 该属性个数:		2byte, 属性数据个数(M)
		_AddToByteArray(&pBA, (long)type);		// 该属性类型:		4byte, 属性类型枚举值
		_AddToByteArray(&pBA, epDataID);	// 该属性ID:		4byte, 属性ID
		_AddToByteArray(&pBA, epDataName);	// 该属性名字:		nbyte, 属性名字,字符串

		string tname = epDataName;
		obj = ep->GetDataObject(tname);

		if(NULL == obj)
		{
			obj = new CWrapDataObject(type, ep->AllocDataObjectID(), tname);
			ep->AddDataObject(type, tname);
		}

		if(obj)
		{
			for(int j=0; j<epDataNum; j++)
			{
				WORD dataPos = _GetWordFromByteArray(msgBuf, msgBufPos); // 该属性POS:	2byte, 属性POS
				WORD dataSize =  _GetWordFromByteArray(msgBuf, msgBufPos); // 该属性size:	2byte, 属性SIZE, 

				if(dataPos != OPER_ALL_DATA)
					obj->PushBackOperValuePosFlag(op, dataPos);

				switch(op)
				{
				case DB_OPER_LOAD:
					{
						g_DataEntityOperator.LoadDataObject(string(tableName), ep->GetID(), obj, epDataNum, dataPos, dataSize);
					}
					break;
				case DB_OPER_SAVE:
					g_DataEntityOperator.SaveDataObject(string(tableName), ep->GetID(), obj, epDataNum, dataPos, dataSize);
					break;
				}

				// Add Msg
				AddMsg(ep, op, msgid, obj, pBA);
			}
		}
	}
}

// 添加消息
void CDataEntityManager::AddMsg(CMessage* msg)
{

}
// 添加消息
void CDataEntityManager::AddMsg(CEntityProperty* ep, DB_OPERATION_TYPE op, long msgid, CWrapDataObject* wdo, vector<BYTE>& pBA)
{
	/*
	|	|	 |		|----
	|	|	 |		|该属性POS:		2byte, 属性POS, 
	|	|	 |		|该属性size:	2byte, 属性SIZE, 
	|	|	 |	M *	|该属性数据块:	nbtye, 该数据块长度由属性数据长度决定 (若消息为LOAD则该数据块为1byte，值为0)
	|	|	 |		|----
	|	|	 |----
	|	|----
	*/
	WORD operValuePos = 0;
	switch(op)
	{
	case DB_OPER_LOAD:
		{
			if(wdo->GetOperValuePosFlagNum(DB_OPER_LOAD) != 0) // 还有已做标记的属性
			{
				operValuePos = wdo->PopFrontOperValuePosFlag(DB_OPER_LOAD);
			}
			_AddToByteArray(&pBA, operValuePos); // 属性值POS
			CWrapDataObject::ValueInfo* vi = wdo->GetValueArray(operValuePos);
			if(vi && vi->buf)
			{	
				switch(wdo->GetDataType())
				{
				case DATA_OBJECT_LONG:
					_AddToByteArray(&pBA, (WORD)sizeof(long));
					_AddToByteArray(&pBA, *(long*)(vi->buf)); // 该属性数据块:nbtye
					break;
				case DATA_OBJECT_BUFFER_ARRAY:
					_AddToByteArray(&pBA, (WORD)vi->size);
					_AddToByteArray(&pBA, vi->buf, vi->size); // 该属性数据块:nbtye
					break;
				case DATA_OBJECT_CHAR_ARRAY:
					_AddToByteArray(&pBA, (WORD)vi->size);
					_AddToByteArray(&pBA, (char*)vi->buf); // 该属性数据块:nbtye
					break;
				}
			}
		}
		break;
	case DB_OPER_SAVE:
		{
			if(wdo->GetOperValuePosFlagNum(DB_OPER_SAVE) != 0) // 还有已做标记的属性
			{
				operValuePos = wdo->PopFrontOperValuePosFlag(DB_OPER_SAVE);
			}

			_AddToByteArray(&pBA, operValuePos); // 属性值POS
		}
		break;
	}
}
// 分配一个ID
long CDataEntityManager::AllocEntityPropertyID()
{
	if(m_FreeEPID.size())
	{
		long id = *(m_FreeEPID.begin());
		m_FreeEPID.erase(m_FreeEPID.begin());
		return id;
	}
	return ++m_CurEPID;
}
// 释放一个ID
void CDataEntityManager::FreeEntityPropertyID(long id)
{
	std::list<long>::iterator itr = std::find(m_FreeEPID.begin(), m_FreeEPID.end(), id);
	// 未找到该ID并且ID小于最大值
	if( (itr == m_FreeEPID.end()) && (id < MAX_DEFINED_ID) ) 
		m_FreeEPID.push_back(id);
}