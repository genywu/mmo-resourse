#include "StdAfx.h"
#include "incrementlog.h"

CIncrementLog *CIncrementLog::_instance = NULL;

CIncrementLog *CIncrementLog:: GetInstance()
{
	if( !_instance )
	{
		_instance = new CIncrementLog;
	}
	return _instance;
}

CIncrementLog::CIncrementLog(void)
{
}

CIncrementLog::~CIncrementLog(void)
{
}

bool CIncrementLog::Load()
{
	// [未完成]
//	return _DbIncrementLog.LoadAll();
	return false;
}

void CIncrementLog::Add(const CGUID& PlayerGuid, LPSYSTEMTIME pTime, BYTE byType, int nMoeny, const char *szDesc)
{
	if( NULL_GUID == PlayerGuid ) return;

	map<CGUID, vector<tagIncrementLog*>*>::iterator it = m_IncrementLogs.find(PlayerGuid);

	// addnew
	if( it == m_IncrementLogs.end() )
	{
		m_IncrementLogs[PlayerGuid] = new vector<tagIncrementLog*>;
	}

	// append
	vector<tagIncrementLog*> *pLogs = m_IncrementLogs[PlayerGuid];
	if(pLogs)
	{
		tagIncrementLog *stLogs = new tagIncrementLog;
		if( stLogs )
		{
			memcpy( &stLogs->m_tTime, pTime, sizeof(SYSTEMTIME) );

			stLogs->m_byType = byType;
			stLogs->m_nMoney = nMoeny;
			stLogs->m_sDesc = szDesc;
			pLogs->push_back(stLogs);
		}
	}
}

void CIncrementLog::UnInitialize()
{
	for(map<CGUID, vector<tagIncrementLog*>*>::iterator it = m_IncrementLogs.begin();
		it != m_IncrementLogs.end(); ++it)
	{
		it->second->clear();
		SAFE_DELETE(it->second);
	}
	m_IncrementLogs.clear();
}

vector<CIncrementLog::tagIncrementLog*>* CIncrementLog::GetIncrementLogsByPlayerID(const CGUID& PlayerGuid)
{
	vector<tagIncrementLog*>* vLogs = NULL;
	if( NULL_GUID != PlayerGuid )
	{
		map<CGUID, vector<tagIncrementLog*>*>::iterator it = m_IncrementLogs.find(PlayerGuid);
		if( it != m_IncrementLogs.end() )
		{
			vLogs = m_IncrementLogs[PlayerGuid];
		}
	}
	return vLogs;
}

int CIncrementLog::GetIncrementLogsSizeByPlayerID(const CGUID& PlayerGuid)
{
	int size = 0;
	if( NULL_GUID != PlayerGuid )
	{
		map<CGUID, vector<tagIncrementLog*>*>::iterator it = m_IncrementLogs.find(PlayerGuid);
		if( it != m_IncrementLogs.end() )
		{
			vector<tagIncrementLog*>* vLogs = m_IncrementLogs[PlayerGuid];
			if( vLogs )
			{
				size = vLogs->size();
			}
		}
	}
	return size;
}
// 删除指定玩家的所有日志
void CIncrementLog::DelAllPlayerIncLog(const CGUID& playerGUID)
{
	if( NULL_GUID != playerGUID )
	{
		map<CGUID, vector<tagIncrementLog*>*>::iterator it = m_IncrementLogs.find(playerGUID);
		if( it != m_IncrementLogs.end() )
		{
			for(int i=0; i<it->second->size(); i++)
			{
				SAFE_DELETE((*it->second)[i]);
			}
			SAFE_DELETE(it->second);

			it = m_IncrementLogs.erase(it);
		}
	}
}
//添加帮会列表ByteArray
bool CIncrementLog::AddIncrementLogsToByteArray(vector<BYTE>& ByteArray, const CGUID& PlayerGuid)
{
	vector<tagIncrementLog*>* vLogs = GetIncrementLogsByPlayerID(PlayerGuid);
	if( vLogs )
	{
		long lSize = vLogs->size();
		_AddToByteArray(&ByteArray, lSize);

		for( vector<tagIncrementLog*>::iterator it=vLogs->begin(); it!=vLogs->end(); ++it )
		{
			_AddToByteArray( &ByteArray, &(*it)->m_tTime, sizeof(SYSTEMTIME) );
			_AddToByteArray( &ByteArray, (*it)->m_byType );
			_AddToByteArray( &ByteArray, (long)(*it)->m_nMoney );
			_AddToByteArray( &ByteArray, (char*)(*it)->m_sDesc.data() );
		}

		return true;
	}

	return false;
}
