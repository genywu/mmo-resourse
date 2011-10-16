



#include "stdafx.h"
#include "Task.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTask::CTask()
{

}

CTask::~CTask()
{
	Release();
}

void CTask::Release()
{
	list<tagRoleRecord*>::iterator it = m_listRoleRecord.begin();
	for( ;it != m_listRoleRecord.end();)
	{
		tagRoleRecord* p = (*it++);
		SAFE_DELETE(p);
	}
	m_listRoleRecord.clear();
}

bool CTask::DecodeFromDataBlock(DBReadSet& dbr)
{
	m_lChapterID = dbr.GetLongFromByteArray();
	long lRoleNum = dbr.GetLongFromByteArray();
	for(int i=0;i<lRoleNum;i++)
	{
		tagRoleRecord* record = new tagRoleRecord;
		record->m_lRoleID = dbr.GetLongFromByteArray();
		record->m_cRoleStep = dbr.GetByteFromByteArray();
		record->m_cAimNum = dbr.GetByteFromByteArray();
		dbr.GetBufferFromByteArray(record->m_Param,(sizeof(tagRoleParam))*(record->m_cAimNum));

		if(!AddOneRoleRecord(record))
 			SAFE_DELETE(record);
	}

	long LexRecirdNum = dbr.GetLongFromByteArray();
	for(int a =0;a<LexRecirdNum;a++)
	{
		tagExcessRecord record;
		WORD wIndex = dbr.GetShortFromByteArray();
		dbr.GetBufferFromByteArray(&record,sizeof(tagExcessRecord));
		m_mapExcessRecord[wIndex] = record;
	}
	//
	LONG lRoleRecordNum = dbr.GetLongFromByteArray();
	for( int a=0; a< lRoleRecordNum; a++)
	{
		LONG _long = dbr.GetLongFromByteArray();
		m_setRoleRecord.insert(_long);
	}
	return true;
}

BOOL CTask::IsRoleBeingByID(long id)
{
	if(m_listRoleRecord.empty())
		return FALSE;

	list<tagRoleRecord*>::iterator it = m_listRoleRecord.begin();
	for(;it!=m_listRoleRecord.end();it++)
	{
		if( (*it)->m_lRoleID == id )
			return TRUE;
	}
	return FALSE;
}
// 
// BOOL CTask::AddOneRoleRecord(LONG roleid,LONG step)
// {
// 	tagRoleRecord* record = new tagRoleRecord;
// 	record->m_lRoleID = roleid;
// 	record->m_cRoleStep = step;
// 	record->m_cAimNum = 0;
// 	memset(record->m_Param,0,sizeof(record->m_Param));
// 	//
// 	if( !AddOneRoleRecord(record))
// 		delete record;
// 	return TRUE;
// }


BOOL CTask::AddOneRoleRecord(tagRoleRecord*& pRole)
{
	if ( m_listRoleRecord.size() >= ROLE_NUM_MAX)
		return FALSE;
	//
	list<tagRoleRecord*>::iterator it = m_listRoleRecord.begin();
	for( ;it != m_listRoleRecord.end();++it)
	{
		if((*it)->m_lRoleID == pRole->m_lRoleID)
			return FALSE;
	}
	m_listRoleRecord.push_back(pRole);
	return TRUE;
}