#include "StdAfx.h"
#include "GateList.h"
void AddLogText(char* msg, ...);		// 加入一条记录
map<string, CGateList::tagGate>			CGateList::m_mapGateList;
CGateList::CGateList()
{
}
CGateList::~CGateList()
{
}

BOOL CGateList::LoadGateList(const char* filename)
{
	m_mapGateList.clear();

	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		return FALSE;
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
		stream>>strOrigName
			>>strName
			>>gate.wLenth
			>>gate.wWidth
			>>gate.wLevel
			>>gate.dwModelId
			>>gate.wMaxHp
			>>gate.bIsunbeatable
			>>*(int*)(&gate.byBlockType)
			>>gate.lDef
			>>gate.lMagicDef
			>>gate.wNameColor
			>>strHit
			>>strDestory
			>>strOpen
			>>strClose;
		int nLen = strOrigName.size ();  //原始名字
		if( nLen<50)
		{
			memcpy(gate.cOrigName,strOrigName.c_str(),nLen);
			gate.cOrigName[nLen]='\0';
		}
		else
		{
			strncpy(gate.cOrigName,strOrigName.c_str(),50-1);
			gate.cOrigName[50-1]='\0';
		}
		nLen = strName.size ();  //名字
		if( nLen<50)
		{
			if ( 0==strcmp("0",strName.c_str()) )
			{
				memcpy(gate.cName,"",nLen);
			}
			else
				memcpy(gate.cName,strName.c_str(),nLen);
			gate.cName[nLen]='\0';
		}
		else
		{
			strncpy(gate.cName,strName.c_str(),50-1);
			gate.cName[50-1]='\0';
		}
		nLen = strHit.size ();  //击中音效
		if( nLen<50)
		{
			memcpy(gate.cHitSound,strHit.c_str(),nLen);
			gate.cHitSound[nLen]='\0';
		}
		else
		{
			strncpy(gate.cHitSound,strHit.c_str(),50-1);
			gate.cHitSound[50-1]='\0';
		}
		nLen = strDestory.size ();  //损毁音效
		if( nLen<50)
		{
			memcpy(gate.cDestorySound,strDestory.c_str(),nLen);
			gate.cDestorySound[nLen]='\0';
		}
		else
		{
			strncpy(gate.cDestorySound,strDestory.c_str(),50-1);
			gate.cDestorySound[50-1]='\0';
		}
		nLen = strOpen.size ();  //开门音效
		if( nLen<50)
		{
			memcpy(gate.cOpenSound,strOpen.c_str(),nLen);
			gate.cOpenSound[nLen]='\0';
		}
		else
		{
			strncpy(gate.cOpenSound,strOpen.c_str(),50-1);
			gate.cOpenSound[50-1]='\0';
		}
		nLen = strClose.size ();  //关门音效
		if( nLen<50)
		{
			memcpy(gate.cCloseSound,strClose.c_str(),nLen);
			gate.cCloseSound[nLen]='\0';
		}
		else
		{
			strncpy(gate.cCloseSound,strClose.c_str(),50-1);
			gate.cCloseSound[50-1]='\0';
		}
		if (gate.byBlockType > 4 || gate.byBlockType < 0)
		{
			AddLogText("编号为：%s的门阻挡类型不合法",strOrigName);
			continue;
		}

		m_mapGateList[gate.cOrigName]=gate;
	}
	return TRUE;
}

BOOL CGateList::AddToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray,(long)m_mapGateList.size());
	for(map<string, CGateList::tagGate>::iterator it=m_mapGateList.begin();it!=m_mapGateList.end();it++)
	{
		tagGate *pGate=&(*it).second;
		if(pGate)
		{
			_AddToByteArray(pByteArray,(BYTE*)pGate,(long)sizeof(tagGate));
		}
	}
	return TRUE;
}
BOOL CGateList::DecordFromByteArray(BYTE* pByte, long& pos)
{
	m_mapGateList.clear();
	long lGateNum=_GetLongFromByteArray(pByte,pos);
	for(int i=0;i<lGateNum;i++)
	{
		tagGate gate;
		_GetBufferFromByteArray(pByte, pos, &gate, sizeof(tagGate));
		m_mapGateList[string(gate.cOrigName)]=gate;
	}
	return TRUE;
}

CGateList::tagGate* CGateList::GetPropertyByOrigName(const char *strName)
{
	return (m_mapGateList.find(strName)!=m_mapGateList.end()) ? (&m_mapGateList[strName]) : NULL;
}