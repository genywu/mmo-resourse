#include "StdAfx.h"
#include "GateList.h"
#include "../Common/Public.h"

/// 静态成员初始化
map<string, GateList::tagGate> GateList::m_mapGateList;

/// 构造
GateList::GateList()
{
}

/// 析构
GateList::~GateList()
{
	m_mapGateList.clear();
}

/// 从文件加载Gate列表
bool GateList::LoadGateList(const char* filename)
{
	m_mapGateList.clear();

	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		return false;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);
	while(ReadTo(stream,"*"))
	{
		tagGate gate;
		string strOrigName;
		string strName;
		string strHit;
		string strDestory;
		string strOpen;
		string strClose;
		stream >> strOrigName
			>> strName
			>> gate.wLenth
			>> gate.wWidth
			>> gate.wLevel
			>> gate.dwModelId
			>> gate.wMaxHp
			>> gate.bIsunbeatable
			>> *(int*)(&gate.byBlockType)
			>> gate.wDef
			>> gate.wMagicDef
			>> gate.wNameColor
			>> strHit
			>> strDestory
			>> strOpen
			>> strClose;
		size_t nLen = strOrigName.size();  //原始名字
		if(nLen<50)
		{
			memcpy(gate.cOrigName, strOrigName.c_str(), nLen);
			gate.cOrigName[nLen] = '\0';
		}
		else
		{
			strncpy(gate.cOrigName, strOrigName.c_str(), 50-1);
			gate.cOrigName[50-1] = '\0';
		}
		nLen = strName.size ();  //名字
		if(nLen<50)
		{
			if ( 0==strcmp("0",strName.c_str()) )
			{
				memcpy(gate.cName, "", nLen);
			}
			else
				memcpy(gate.cName, strName.c_str(), nLen);
			gate.cName[nLen] = '\0';
		}
		else
		{
			strncpy(gate.cName, strName.c_str(), 50-1);
			gate.cName[50-1] = '\0';
		}
		nLen = strHit.size ();  //击中音效
		if(nLen<50)
		{
			memcpy(gate.cHitSound, strHit.c_str(), nLen);
			gate.cHitSound[nLen] = '\0';
		}
		else
		{
			strncpy(gate.cHitSound, strHit.c_str(), 50-1);
			gate.cHitSound[50-1] = '\0';
		}
		nLen = strDestory.size();  //损毁音效
		if(nLen<50)
		{
			memcpy(gate.cDestorySound, strDestory.c_str(), nLen);
			gate.cDestorySound[nLen] = '\0';
		}
		else
		{
			strncpy(gate.cDestorySound, strDestory.c_str(), 50-1);
			gate.cDestorySound[50-1] = '\0';
		}
		nLen = strOpen.size ();  //开门音效
		if(nLen<50)
		{
			memcpy(gate.cOpenSound, strOpen.c_str(), nLen);
			gate.cOpenSound[nLen] = '\0';
		}
		else
		{
			strncpy(gate.cOpenSound, strOpen.c_str(), 50-1);
			gate.cOpenSound[50-1] = '\0';
		}
		nLen = strClose.size();  //关门音效
		if(nLen<50)
		{
			memcpy(gate.cCloseSound, strClose.c_str(), nLen);
			gate.cCloseSound[nLen] = '\0';
		}
		else
		{
			strncpy(gate.cCloseSound, strClose.c_str(), 50-1);
			gate.cCloseSound[50-1] = '\0';
		}

		if (gate.byBlockType > 4 || gate.byBlockType < 0)
		{
			Log4c::Trace(ROOT_MODULE,FormatText("WS_GATELIST_10", strOrigName));
			continue;
		}

		m_mapGateList[gate.cOrigName] = gate;
	}
	return true;
}

/// 编码
bool GateList::AddToByteArray(vector<uchar>* pByteArray)
{
	_AddToByteArray(pByteArray, (long)m_mapGateList.size());
	map<string, tagGate>::iterator it = m_mapGateList.begin();
	for(; it!=m_mapGateList.end(); ++it)
	{
		tagGate *pGate = &(*it).second;
		if(pGate)
		{
			_AddToByteArray(pByteArray, (uchar*)pGate, (long)sizeof(tagGate));
		}
	}
	return true;
}

/// 解码
bool GateList::DecordFromByteArray(uchar* pByte, long& pos)
{
	m_mapGateList.clear();
	long lGateNum = _GetLongFromByteArray(pByte,pos);
	for(int i=0; i<lGateNum; i++)
	{
		tagGate gate;
		_GetBufferFromByteArray(pByte, pos, &gate, sizeof(tagGate));
		m_mapGateList[string(gate.cOrigName)] = gate;
	}
	return true;
}

/// 根据原始名取得Gate属性
GateList::tagGate* GateList::GetPropertyByOrigName(const char *strName)
{
	return (m_mapGateList.find(strName)!=m_mapGateList.end()) ? (&m_mapGateList[strName]) : NULL;
}