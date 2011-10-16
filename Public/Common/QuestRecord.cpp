#include "StdAfx.h"
#include "QuestRecord.h"
#include "Public.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CQuestRecord::CQuestRecord()
{
	m_lQuestID=0;
	m_cQuestStep=0;
	m_cAimNum=0;
	memset(m_Param,0,sizeof(m_Param));
}
CQuestRecord::CQuestRecord(long QuestID,long StepID)
{
	m_lQuestID=QuestID;
	m_cQuestStep=(uchar)StepID;
	m_cAimNum=0;
	memset(m_Param,0,sizeof(m_Param));
}
CQuestRecord::~CQuestRecord()
{
}

// 压数据
bool CQuestRecord::AddDatatoByteArray(vector<uchar>* pByteArray)
{
	_AddToByteArray(pByteArray,m_lQuestID);
	_AddToByteArray(pByteArray,m_cQuestStep);
	_AddToByteArray(pByteArray,m_cAimNum);
	_AddToByteArray(pByteArray,m_Param,sizeof(sQuestParam)*m_cAimNum);
	//_AddToByteArray(pByteArray,(long)m_listQuestRecord.size());
	//for (list<CQuestRecord*>::iterator it=m_listQuestRecord.begin(); it!= m_listQuestRecord.end(); it++)
	//{
	//	_AddToByteArray(pByteArray,*it,sizeof(CQuestRecord));
	//}

	return true;
}
//解来数据
bool CQuestRecord::DecordDataFromByteArray(uchar* pByte, long& pos)
{
	m_lQuestID=_GetLongFromByteArray(pByte,pos);
	m_cQuestStep=_GetByteFromByteArray(pByte,pos);
	m_cAimNum= _GetByteFromByteArray(pByte,pos);
	_GetBufferFromByteArray(pByte,pos,m_Param,sizeof(sQuestParam)*m_cAimNum);
	return true;
}

bool CQuestRecord::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray(m_lQuestID);
	setWriteDB.AddToByteArray(m_cQuestStep);
	setWriteDB.AddToByteArray(m_cAimNum);
	setWriteDB.AddToByteArray(m_Param,sizeof(sQuestParam)*m_cAimNum);
	return true;
}
bool CQuestRecord::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	m_lQuestID=setReadDB.GetLongFromByteArray();
	m_cQuestStep=setReadDB.GetByteFromByteArray();
	m_cAimNum= setReadDB.GetByteFromByteArray();
	setReadDB.GetBufferFromByteArray(m_Param,sizeof(sQuestParam)*m_cAimNum);
	return true;
}

long CQuestRecord::GetParam(long Index)
{
	if (Index>=0 && Index<ROLE_PRAME_NUM )
	{
		return m_Param[Index].m_wParam;
	}
	return ROLE_ERRO;
}

bool CQuestRecord::SetParam(long Index, long val)
{
	if (Index>=0 && Index<ROLE_PRAME_NUM )
	{
		if(Index>= m_cAimNum)
		{
			m_cAimNum=(uchar)Index+1;
		}
		m_Param[Index].m_wParam= val;
		return true;
	}
	return false;
}

bool CQuestRecord::AddParam(long Index, long addval)
{
	if (Index>=0 && Index<ROLE_PRAME_NUM )
	{
		if(Index>= m_cAimNum)
		{
			m_cAimNum=(uchar)Index+1;
		}
		m_Param[Index].m_wParam+=addval;
		return true;
	}
	return false;
}

long CQuestRecord::GetAccParam(long Index)
{
	if (Index>=0 && Index<ROLE_PRAME_NUM )
	{
		return m_Param[Index].m_lAccParam;
	}
	return ROLE_ERRO;
}

bool CQuestRecord::SetAccParam(long Index, long val)
{
	if (Index>=0 && Index<ROLE_PRAME_NUM )
	{
		if(Index>= m_cAimNum)
		{
			m_cAimNum=(uchar)Index+1;
		}
		m_Param[Index].m_lAccParam=val;
		return true;
	}
	return false;
}

bool CQuestRecord::AddAccParam(long Index, long addval)
{
	if (Index>=0 && Index<ROLE_PRAME_NUM )
	{
		if(Index>= m_cAimNum)
		{
			m_cAimNum=(uchar)Index+1;
		}
		m_Param[Index].m_lAccParam+=addval;
		return true;
	}
	return false;
}

//获得额外记录索引
ushort CQuestRecord::GetExcessRecordIndex(long Index)
{
	if (Index>=0 && Index<ROLE_PRAME_NUM )
	{
		return (ushort)m_Param[Index].m_wExcessRecordIndex;
	}
	return 0;
}

//设置额外记录索引
bool CQuestRecord::SetExcessRecordIndex(long Index,ushort addval)
{
	if (Index>=0 && Index<ROLE_PRAME_NUM )
	{
		if(Index>= m_cAimNum)
		{
			m_cAimNum=(uchar)Index+1;
		}
		m_Param[Index].m_wExcessRecordIndex=addval;
		return true;
	}
	return false;
}


	//场景id
	//取得变量值
//long CQuestRecord::GetRegionID(long Index)
//{
//	if (Index>=0 && Index<ROLE_PRAME_NUM )
//	{
//		return 0;//m_Param[Index].m_lRegionID;
//	}
//	return ROLE_ERRO;
//}
	
	//设置变量值
//bool CQuestRecord::SetRegionID(long Index, long val)
//{
//	if (Index>=0 && Index<ROLE_PRAME_NUM )
//	{
//		m_Param[Index].m_lRegionID=val;
//		return true;
//	}
//	return false;
//}
	

	
	//场景x
	//取得变量值
//long CQuestRecord::GetRegionX(long Index)
//{
//	if (Index>=0 && Index<ROLE_PRAME_NUM )
//	{
//		return 0;// m_Param[Index].m_lRegionX;
//	}
//	return ROLE_ERRO;
//}
	
	//设置变量值
//bool CQuestRecord::SetRegionX(long Index, long val)
//{
//	if (Index>=0 && Index<ROLE_PRAME_NUM )
//	{
//		//m_Param[Index].m_lRegionX=val;
//		return true;
//	}
//	return false;
//}

	
	//场景y
	//取得变量值
//long CQuestRecord::GetRegionY(long Index)
//{
//	if (Index>=0 && Index<ROLE_PRAME_NUM )
//	{
//		return m_Param[Index].m_lRegionY;
//	}
//	return ROLE_ERRO;
//}
	
	//设置变量值
//bool CQuestRecord::SetRegionY(long Index, long val)
//{
//	if (Index>=0 && Index<ROLE_PRAME_NUM )
//	{
//		m_Param[Index].m_lRegionY=val;
//		return true;
//	}
//	return false;
//
//}
	
//------------------CQuestQueue
CQuestQueue::CQuestQueue()
{
	m_pCurrentPtr=NULL;
	m_lChapter=0;
}
CQuestQueue::~CQuestQueue()
{
	list<CQuestRecord*>::iterator it=m_listQuestRecord.begin();
	for (; it!= m_listQuestRecord.end(); it++)
	{
		delete *it;
	}
}

CQuestRecord *CQuestQueue::GetRecordByID(long ID)
{
	list<CQuestRecord*>::iterator it=m_listQuestRecord.begin();
	for (;it!=m_listQuestRecord.end();it++)
	{
		if ( ((*it)->GetQuestID()==ID))
		{
			return (*it);
		}
	}
	return NULL;
}

long CQuestQueue::GetQuestStepNum(long ID)
{
	CQuestRecord* record=GetRecordByID(ID);
	if (record==NULL)
	{
		return 0;
	}
	return record->GetQuestStep();
}

bool CQuestQueue::FindQuestStep(long ID , long Step)
{
	list<CQuestRecord*>::iterator it=m_listQuestRecord.begin();
	for (;it!=m_listQuestRecord.end();it++)
	{
		if ( ((*it)->GetQuestID()==ID) && ((*it)->GetQuestStep()==Step) )
		{
			m_pCurrentPtr=(*it);
			return true;
		}
	}
	m_pCurrentPtr = NULL;
	return false;
}

//long CQuestQueue::GetParam(long Index)
//{
//	if (m_pCurrentPtr==0)
//	{
//		return ROLE_ERRO;
//	}
//	if (Index < ROLE_PRAME_NUM)
//	{
//		return m_pCurrentPtr->GetParam(Index);
//	}
//	return ROLE_ERRO;
//}

long CQuestQueue::GetParam(long QuestID, long Step, long Index)
{
	if ( FindQuestStep(QuestID,Step))
	{
		return m_pCurrentPtr->GetParam(Index);
	}
	return ROLE_ERRO;
}

//bool CQuestQueue::SetParam(long Index, long val)
//{
//	if (m_pCurrentPtr==NULL)
//	{
//		return ROLE_ERRO;
//	}
//	return m_pCurrentPtr->SetParam(Index,val);
//}

bool CQuestQueue::SetParam(long QuestID, long Step, long Index, long val)
{
	if (FindQuestStep(QuestID,Step))
	{
		return m_pCurrentPtr->SetParam(Index,val);
	}
	return false;
}

//bool CQuestQueue::AddParam(long Index, long val)
//{
//	if (m_pCurrentPtr==NULL)
//	{
//		return false;
//	}
//	return m_pCurrentPtr->AddParam(Index,val);
//}

bool CQuestQueue::AddParam(long QuestID, long Setp, long Index, long val)
{
	if (FindQuestStep(QuestID,Setp))
	{
		return m_pCurrentPtr->AddParam(Index,val);
	}
	return false;
}
//........................................
//long CQuestQueue::GetAccParam(long Index)
//{
//	if (m_pCurrentPtr==0)
//	{
//		return ROLE_ERRO;
//	}
//	if (Index < ROLE_PRAME_NUM)
//	{
//		return m_pCurrentPtr->GetAccParam(Index);
//	}
//	return ROLE_ERRO;
//}

long CQuestQueue::GetAccParam(long QuestID, long Step, long Index)
{
	if ( FindQuestStep(QuestID,Step))
	{
		return m_pCurrentPtr->GetAccParam(Index);
	}
	return ROLE_ERRO;
}

//bool CQuestQueue::SetAccParam(long Index, long val)
//{
//	if (m_pCurrentPtr==NULL)
//	{
//		return ROLE_ERRO;
//	}
//	return m_pCurrentPtr->SetAccParam(Index,val);
//}

bool CQuestQueue::SetAccParam(long QuestID, long Step, long Index, long val)
{
	if (FindQuestStep(QuestID,Step))
	{
		return m_pCurrentPtr->SetAccParam(Index,val);
	}
	return false;
}

//bool CQuestQueue::AddAccParam(long Index, long val)
//{
//	if (m_pCurrentPtr==NULL)
//	{
//		return false;
//	}
//	return m_pCurrentPtr->AddAccParam(Index,val);
//}

bool CQuestQueue::AddAccParam(long QuestID, long Setp, long Index, long val)
{
	if (FindQuestStep(QuestID,Setp))
	{
		return m_pCurrentPtr->AddAccParam(Index,val);
	}
	return false;
}

//取得 某一任务某一步骤的Index对应的场景id
long CQuestQueue::GetRegionID(long QuestID, long Step ,long Index)
{
	if ( !FindQuestStep(QuestID,Step))
	{
		return ROLE_ERRO;
	}
	ushort wExcRecordIndex=m_pCurrentPtr->GetExcessRecordIndex(Index);
	if (wExcRecordIndex==0)
	{
		return ROLE_ERRO;		
	}
	map<ushort,sExcessRecord>::iterator it= m_mapExcessRecord.find(wExcRecordIndex);
	if ( it == m_mapExcessRecord.end())
	{
		return ROLE_ERRO;
	}
	return (*it).second.m_lRegionID;
	
}


//设置 某一任务某一步骤的Index对应的场景id
bool CQuestQueue::SetRegionID(long QuestID,  long Step,long Index, long val)
{	
	if ( !FindQuestStep(QuestID,Step))
	{
		return false;
	}
	ushort wExcRecordIndex=m_pCurrentPtr->GetExcessRecordIndex(Index);
	if (wExcRecordIndex==0)
	{
		wExcRecordIndex=CreatExcessRecord(val,0,0);
		m_pCurrentPtr->SetExcessRecordIndex(Index,wExcRecordIndex);
		return true;		
	}
	map<ushort,sExcessRecord>::iterator it= m_mapExcessRecord.find(wExcRecordIndex);
	if ( it == m_mapExcessRecord.end())
	{
		return false;
	}
	(*it).second.m_lRegionID= val;
	return true;
}

//取得 某一任务某一步骤的Index对应的场景x坐标
long CQuestQueue::GetRegionX(long QuestID, long Step ,long Index)
{	
	if ( !FindQuestStep(QuestID,Step))
	{
		return ROLE_ERRO;
	}
	ushort wExcRecordIndex=m_pCurrentPtr->GetExcessRecordIndex(Index);
	if (wExcRecordIndex==0)

	{
		return ROLE_ERRO;		
	}
	map<ushort,sExcessRecord>::iterator it= m_mapExcessRecord.find(wExcRecordIndex);

	if ( it == m_mapExcessRecord.end())
	{
		return ROLE_ERRO;
	}
	return (*it).second.m_lRegionX;
}

//设置 某一任务某一步骤的Index对应的场景x坐标
bool CQuestQueue::SetRegionX(long QuestID,  long Step,long Index, long val)
{	
	if ( !FindQuestStep(QuestID,Step))
	{
		return false;
	}
	ushort wExcRecordIndex=m_pCurrentPtr->GetExcessRecordIndex(Index);
	if (wExcRecordIndex==0)
	{
		wExcRecordIndex=CreatExcessRecord(0,val,0);
		m_pCurrentPtr->SetExcessRecordIndex(Index,wExcRecordIndex);
		return true;		
	}
	map<ushort,sExcessRecord>::iterator it= m_mapExcessRecord.find(wExcRecordIndex);
	if ( it == m_mapExcessRecord.end())
	{
		return false;
	}
	(*it).second.m_lRegionX= val;
	return true;
}

//取得 某一任务某一步骤的Index对应的场景y坐标
long CQuestQueue::GetRegionY(long QuestID, long Step ,long Index)
{	
	if ( !FindQuestStep(QuestID,Step))
	{
		return ROLE_ERRO;
	}
	ushort wExcRecordIndex=m_pCurrentPtr->GetExcessRecordIndex(Index);
	if (wExcRecordIndex==0)
	{
		return ROLE_ERRO;		
	}
	map<ushort,sExcessRecord>::iterator it= m_mapExcessRecord.find(wExcRecordIndex);
	if ( it == m_mapExcessRecord.end())
	{
		return ROLE_ERRO;
	}
	return (*it).second.m_lRegionY;
}

//设置 某一任务某一步骤的Index对应的场景y坐标
bool CQuestQueue::SetRegionY(long QuestID,  long Step,long Index, long val)
{	
	if ( !FindQuestStep(QuestID,Step))
	{
		return false;
	}
	ushort wExcRecordIndex=m_pCurrentPtr->GetExcessRecordIndex(Index);
	if (wExcRecordIndex==0)
	{
		wExcRecordIndex=CreatExcessRecord(0,0,val);
		m_pCurrentPtr->SetExcessRecordIndex(Index,wExcRecordIndex);
		return true;		
	}
	map<ushort,sExcessRecord>::iterator it= m_mapExcessRecord.find(wExcRecordIndex);
	if ( it == m_mapExcessRecord.end())
	{
		return false;
	}
	(*it).second.m_lRegionY= val;
	return true;
}


//设置 m_pCurrentPtr 指向的任务的Index的场景id
//bool CQuestQueue::SetRegionID(long Index, long val)
//{
//	if (m_pCurrentPtr==NULL)
//	{
//		return false;
//	}
//	return m_pCurrentPtr->SetRegionID(Index,val);
//}

//设置 m_pCurrentPtr 指向的任务的Index的场景id
//long CQuestQueue::GetRegionID(long Index)
//{
//	if (m_pCurrentPtr==NULL)
//	{
//		return false;
//	}
//	return m_pCurrentPtr->GetRegionID(Index);
//}
//取得  m_pCurrentPtr 指向的任务的Index的场景x
//long CQuestQueue::GetRegionX(long Index)
//{
//	if (m_pCurrentPtr==NULL)
//	{
//		return false;
//	}
//	return m_pCurrentPtr->GetRegionX(Index);
//}

//设置 m_pCurrentPtr 指向的任务的Index的场景x
//bool CQuestQueue::SetRegionX(long Index, long val)
//{
//	if (m_pCurrentPtr==NULL)
//	{
//		return false;
//	}
//	return m_pCurrentPtr->SetRegionX(Index,val);
//}

//取得  m_pCurrentPtr 指向的任务的Index的场景y
//long CQuestQueue::GetRegionY(long Index)
//{
//	if (m_pCurrentPtr==NULL)
//	{
//		return false;
//	}
//	return m_pCurrentPtr->GetRegionY(Index);
//}

//设置 m_pCurrentPtr 指向的任务的Index的场景y
//bool CQuestQueue::SetRegionY(long Index, long val)
//{
//	if (m_pCurrentPtr==NULL)
//	{
//		return false;
//	}
//	return m_pCurrentPtr->SetRegionY(Index,val);
//}




void CQuestQueue::Clean()
{
	list<CQuestRecord*>::iterator it= m_listQuestRecord.begin();
	for (; it!= m_listQuestRecord.end(); it++)
	{
		delete *it;
	}
	m_listQuestRecord.clear();
	m_setQuestRecord.clear();
	m_mapExcessRecord.clear();
}

//bool CQuestQueue::AddParam(long QuestID, long Setp, long Index, long val)
//{
//	if (FindQuestStep(QuestID,Setp))
//	{
//		return m_pCurrentPtr->AddParam(Index,val);
//	}
//	return false;
//}


bool CQuestQueue::AddOneRecord(CQuestRecord*& ptr)
{
	if (m_listQuestRecord.size() >= ROLE_NUM_MAX)
	{
		return false;	
	}

	if (GetRecordByID(ptr->GetQuestID())!=NULL)
	{
		return false;
	}

	m_listQuestRecord.push_back(ptr);
	ptr=NULL;
	return true;
}

bool CQuestQueue::AddOneRecord(long QuestID,long step)
{
	CQuestRecord* record= new CQuestRecord(QuestID,step);
	if (!AddOneRecord(record))
	{
		delete record;
		return false;
	}
	return true;
}

bool CQuestQueue::DelOneRecord(long ID)
{
	list<CQuestRecord*>::iterator it=m_listQuestRecord.begin();
	while (it!=m_listQuestRecord.end())
	{
		if ((*it)->GetQuestID()==ID)
		{
			CQuestRecord* pRole=(*it);
			ClearOneRecordEx(pRole);
			delete *it;
			m_listQuestRecord.erase(it);
			return true;
		}
		it++;
	}
	return false;
}

//清理一个任务的附加记录
bool CQuestQueue::ClearOneRecordEx(CQuestRecord* pQuestRecord)
{
	if (pQuestRecord==NULL) return false;
	//ROLE_PRAME_NUM
	for(int a=0; a<ROLE_PRAME_NUM; a++)
	{
		ushort ExIndex=pQuestRecord->GetExcessRecordIndex(a);
		if (ExIndex != 0)
		{
			map<ushort , sExcessRecord>::iterator it= m_mapExcessRecord.find(ExIndex);
			if (it!= m_mapExcessRecord.end())	
				m_mapExcessRecord.erase(it);
		}
	}
	return true;
}

bool CQuestQueue::DeleteExcessRecord()
{
	map<ushort , sExcessRecord>::iterator it=m_mapExcessRecord.begin();
	while(it!=m_mapExcessRecord.end())
	{
		ushort wIndex=it->first;
		list<CQuestRecord*>::iterator itlist=m_listQuestRecord.begin();
		bool b=false;
		//是否有任务的额外记录是wIndex
		for (;itlist!=m_listQuestRecord.end();itlist++)
		{
			//找到一个后表示额外记录是有效数据。wIndex唯一，跳出循环
			if((*itlist)->IsExist(wIndex))
			{
				b=true;
				break;
			}
		}
		//无效的额外记录
		if (!b)
		{
			m_mapExcessRecord.erase(it++);
		}
		else
			++it;
	}
	return true;
}

bool CQuestRecord::IsExist(ushort wIndex)
{
	for (int i=0;i<ROLE_PRAME_NUM;i++)
	{
		if (m_Param[i].m_wExcessRecordIndex==wIndex)
		{
			return true;
		}
	}
	return false;
}

//将任务调至第n步
void CQuestQueue::JumpStepTo(long lQuestID,uchar stepnum)
{
	CQuestRecord *p=GetRecordByID(lQuestID);
	if (p!=NULL)
	{
		ClearOneRecordEx(p);
		p->JumpStepTo(stepnum);

	}
}


//压码
bool CQuestQueue::AddQuestDataByteArray(vector<uchar>* pByteArray)
{

	_AddToByteArray(pByteArray,m_lChapter);
	//_AddToByteArray(pByteArray,(long)m_mapQuestRecord.size());

	_AddToByteArray(pByteArray,(long)m_listQuestRecord.size());
	for (list<CQuestRecord*>::iterator it=m_listQuestRecord.begin(); it!= m_listQuestRecord.end(); it++)
	{
		//_AddToByteArray(pByteArray,*it,sizeof(CQuestRecord));
		(*it)->AddDatatoByteArray(pByteArray);
	}

	_AddToByteArray(pByteArray,(long)m_mapExcessRecord.size());
	for (map<ushort,sExcessRecord>::iterator it= m_mapExcessRecord.begin(); it!= m_mapExcessRecord.end() ; it++)
	{
		_AddToByteArray(pByteArray,(ushort)(*it).first);
		_AddToByteArray(pByteArray,&(*it).second,sizeof(sExcessRecord));
		//(*it).second.AddDatatoByteArray(pByteArray);
	}

	_AddToByteArray(pByteArray,(long)m_setQuestRecord.size());
	for (set<long>::iterator ite=m_setQuestRecord.begin(); ite!= m_setQuestRecord.end(); ite++)
	{
		_AddToByteArray(pByteArray,(long)*ite);
	}


	//...
	//map<long , bool>		m_mapQuestRecord;					//任务记录
	//list<CQuestRecord*>		m_listQuestRecord;					//任务列表
	//CQuestRecord*			m_pCurrentPtr;						//操作指针，优化速度。

	//long					m_lChapter;							//所在主线任务章节
	//long					m_lMostlyQuestNum;					//主线任务当前id


	return true;
}


//解码
bool CQuestQueue::DecordQuestDataFromByteArray(uchar* pByte, long& pos)
{
	Clean();
	m_lChapter=_GetLongFromByteArray(pByte,pos);
	long rolenum= _GetLongFromByteArray(pByte,pos);

	for (int a=0 ; a<rolenum; a++)
	{
		CQuestRecord* record=new CQuestRecord;
		//_GetBufferFromByteArray(pByte,pos,record,sizeof(CQuestRecord));
		record->DecordDataFromByteArray(pByte,pos);
		//m_listQuestRecord.push_back(record);
		if (!AddOneRecord(record))
		{
			delete record;
		}
	}

	long lExcRecordNun= _GetLongFromByteArray(pByte,pos);
	for (int a=0; a< lExcRecordNun; a++)
	{
		sExcessRecord record;
		ushort index= _GetShortFromByteArray(pByte,pos);
		_GetBufferFromByteArray(pByte,pos,&record,sizeof(sExcessRecord));
		m_mapExcessRecord[index]= record;
	}

	long rolerecordnum=_GetLongFromByteArray(pByte,pos);

	for (int a=0; a<rolerecordnum; a++)
	{
		long _long=_GetLongFromByteArray(pByte,pos);//_GetCharFromByteArray(pByte,pos);
		//m_setQuestRecord[_long]=true;
		m_setQuestRecord.insert(_long);
	}
	DeleteExcessRecord();
	return false;
}

bool CQuestQueue::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray(m_lChapter);
	
	setWriteDB.AddToByteArray((long)m_listQuestRecord.size());
	for (list<CQuestRecord*>::iterator it=m_listQuestRecord.begin(); it!= m_listQuestRecord.end(); it++)
	{
		(*it)->CodeToDataBlock(setWriteDB);
	}

	setWriteDB.AddToByteArray((long)m_mapExcessRecord.size());
	for (map<ushort,sExcessRecord>::iterator it= m_mapExcessRecord.begin(); it!= m_mapExcessRecord.end() ; it++)
	{
		setWriteDB.AddToByteArray((ushort)(*it).first);
		setWriteDB.AddToByteArray(&(*it).second,sizeof(sExcessRecord));
	}

	setWriteDB.AddToByteArray((long)m_setQuestRecord.size());
	for (set<long>::iterator ite=m_setQuestRecord.begin(); ite!= m_setQuestRecord.end(); ite++)
	{
		setWriteDB.AddToByteArray((long)*ite);
	}

	return true;
}
bool CQuestQueue::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	Clean();
	m_lChapter=setReadDB.GetLongFromByteArray();
	long rolenum= setReadDB.GetLongFromByteArray();

	for (int a=0 ; a<rolenum; a++)
	{
		CQuestRecord* record=new CQuestRecord;
		record->DecodeFromDataBlock(setReadDB);
		if (!AddOneRecord(record))
		{
			delete record;
		}
	}

	long lExcRecordNun= setReadDB.GetLongFromByteArray();
	for (int a=0; a< lExcRecordNun; a++)
	{
		sExcessRecord record;
		ushort index= setReadDB.GetShortFromByteArray();
		setReadDB.GetBufferFromByteArray(&record,sizeof(sExcessRecord));
		m_mapExcessRecord[index]= record;
	}

	long rolerecordnum=setReadDB.GetLongFromByteArray();

	for (int a=0; a<rolerecordnum; a++)
	{
		long _long=setReadDB.GetLongFromByteArray();
		m_setQuestRecord.insert(_long);
	}
	DeleteExcessRecord();
	return true;
}

bool CQuestQueue::AddFinishedQuestRecord(long index)
{
	set<long>::iterator it=m_setQuestRecord.find(index);
	if(it== m_setQuestRecord.end())
	{
		m_setQuestRecord.insert(index);
		return true;
	}
	return false;
}
//删除任务记录
bool CQuestQueue::DelFinishedQuestRecord(long index)
{
	set<long>::iterator it=m_setQuestRecord.find(index);
	if(it!= m_setQuestRecord.end())
	{
		m_setQuestRecord.erase(index);
		return true;
	}
	return false;
}

bool CQuestQueue::GetQuestFinishedRecord(long index)
{
	set<long>::iterator it=m_setQuestRecord.find(index);
	if (it!=m_setQuestRecord.end())
	{
		return true;
	}
	return false;
}

sExcessRecord* CQuestQueue::FindsExcessRecord(ushort index)
{
	map<ushort,sExcessRecord>::iterator it= m_mapExcessRecord.find(index);
	if (it== m_mapExcessRecord.end())
	{
		return NULL;
	}
	return &((*it).second);
}

ushort	CQuestQueue::CreatExcessRecord(long regionID,long regionX,long regionY)
{
	ushort index;
	map<ushort,sExcessRecord>::iterator it;
	do 
	{
		index=rand()%(0xffff);
	} while(index==0 || m_mapExcessRecord.find(index)!= m_mapExcessRecord.end());
	sExcessRecord& ExcRecord= m_mapExcessRecord[index];
	//(*it).second.m_lRegionID= regionID;
	//(*it).second.m_lRegionX = regionX;
	//(*it).second.m_lRegionY = regionY;
	ExcRecord.m_lRegionID = regionID;
	ExcRecord.m_lRegionX = regionX;
	ExcRecord.m_lRegionY = regionY;
	return index;
}

bool	CQuestQueue::DelExcessRecord(ushort index)
{
	m_mapExcessRecord.erase(index);
	return true;
}


//初始化任务记录列表
//void CQuestQueue::InitQuestRecord()
//{
//
//}

//俑兵任务系统类
//定义

CMercenaryQuestSys::CMercenaryQuestSys()
:m_lMasterQuestID(0)
{
	//所有俑兵任务列表
	m_MerQuests.clear();
	//当前接受的俑兵任务
	m_lCurQuestID = 0;
	//上次刷新俑兵任务的时间
	//m_dwLastUpdateTime=0;
	m_lLastQuestID = 0;
	DisbandQuests.clear();
}

CMercenaryQuestSys::~CMercenaryQuestSys()
{
}


//压数据
bool CMercenaryQuestSys::AddDataByteArray(vector<uchar>* pByteArray)
{
	_AddToByteArray(pByteArray,m_lCurQuestID);
	_AddToByteArray(pByteArray,m_lLastQuestID);
	_AddToByteArray(pByteArray,(ushort)m_MerQuests.size());
	itMerQ it = m_MerQuests.begin();
	for(;it != m_MerQuests.end();it++)
	{
		_AddToByteArray(pByteArray,(*it).first);
		tagMerQuest &MercQuests = (*it).second;
		_AddToByteArray(pByteArray,MercQuests.dwLastUpdateTime);
		_AddToByteArray(pByteArray,(ushort)MercQuests.Quests.size());
		vector<long>::iterator itQ = MercQuests.Quests.begin();
		for(;itQ != MercQuests.Quests.end();itQ++)
		{
			_AddToByteArray(pByteArray,(*itQ));
		}
	}
	set<long>::iterator itDisband=DisbandQuests.begin();
	_AddToByteArray( pByteArray,(ushort)DisbandQuests.size() );
	for (;itDisband != DisbandQuests.end();itDisband++)
	{
		_AddToByteArray( pByteArray,(*itDisband) );
	}
	return true;
}
//解数据
bool CMercenaryQuestSys::DecordDataFromByteArray(uchar* pByte, long& pos)
{
	m_MerQuests.clear();
	DisbandQuests.clear();
	m_lCurQuestID = _GetLongFromByteArray(pByte,pos);
	m_lMasterQuestID = _GetLongFromByteArray(pByte,pos);
	ushort wSize = _GetWordFromByteArray(pByte,pos);
	tagMerQuest MerQuest;
	for(ushort i=0;i<wSize;i++)
	{
		long lID = _GetLongFromByteArray(pByte,pos);
		long time= _GetDwordFromByteArray(pByte,pos);
		long lSize =  _GetWordFromByteArray(pByte,pos);
		MerQuest.Quests.clear();
		MerQuest.dwLastUpdateTime=time;
		for(ushort j=0;j<lSize;j++)
		{
			long lID = _GetLongFromByteArray(pByte,pos);
			MerQuest.Quests.push_back(lID);
		}
		m_MerQuests[lID] = MerQuest;

	}
	wSize=_GetWordFromByteArray(pByte,pos);
	for ( ushort i=0;i<wSize;i++ )
	{
		long lId = _GetLongFromByteArray(pByte,pos);
		DisbandQuests.insert(lId);
	}
	return true;
}


bool CMercenaryQuestSys::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray(m_lCurQuestID);
	setWriteDB.AddToByteArray((ushort)m_MerQuests.size());
	itMerQ it = m_MerQuests.begin();
	for(;it != m_MerQuests.end();it++)
	{
		setWriteDB.AddToByteArray((*it).first);
		tagMerQuest &MercQuests = (*it).second;
		setWriteDB.AddToByteArray(MercQuests.dwLastUpdateTime);
		setWriteDB.AddToByteArray((ushort)MercQuests.Quests.size());
		vector<long>::iterator itQ = MercQuests.Quests.begin();
		for(;itQ != MercQuests.Quests.end();itQ++)
		{
			setWriteDB.AddToByteArray((*itQ));
		}
	}
	set<long>::iterator itDisband=DisbandQuests.begin();
	setWriteDB.AddToByteArray( (ushort)DisbandQuests.size() );
	for (;itDisband != DisbandQuests.end();itDisband++)
	{
		setWriteDB.AddToByteArray( (*itDisband) );
	}
	return true;
}
bool CMercenaryQuestSys::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	m_MerQuests.clear();
	DisbandQuests.clear();
	m_lCurQuestID = setReadDB.GetLongFromByteArray();

	ushort wSize = setReadDB.GetWordFromByteArray();
	tagMerQuest MerQuest;
	for(ushort i=0;i<wSize;i++)
	{
		long lID = setReadDB.GetLongFromByteArray();
		long time= setReadDB.GetDwordFromByteArray();
		long lSize =  setReadDB.GetWordFromByteArray();
		MerQuest.Quests.clear();
		MerQuest.dwLastUpdateTime=time;
		for(ushort j=0;j<lSize;j++)
		{
			long lID = setReadDB.GetLongFromByteArray();
			MerQuest.Quests.push_back(lID);
		}
		m_MerQuests[lID] = MerQuest;

	}
	wSize=setReadDB.GetWordFromByteArray();
	for ( ushort i=0;i<wSize;i++ )
	{
		long lId = setReadDB.GetLongFromByteArray();
		DisbandQuests.insert(lId);
	}
	return true;
}

//判断在列表中是否存在改任务
long CMercenaryQuestSys::IsExistQuest(long lQuestID)
{
	itMerQ it = m_MerQuests.begin();
	for(;it != m_MerQuests.end();it++)
	{
		tagMerQuest &MercQuests = (*it).second;
		vector<long>::iterator itQ = find(MercQuests.Quests.begin(),MercQuests.Quests.end(),lQuestID);
		if(itQ != MercQuests.Quests.end())
			return it->first;
	}
	return 0;
}

//判断在列表中是否存在该任务列表的数据
bool CMercenaryQuestSys::IsExistQuestList(long lQuestListID)
{
	itMerQ it = m_MerQuests.find(lQuestListID);
	if (it==m_MerQuests.end())
	{
		return false;
	}
	return true;
}
//得到该列表ID的俑兵任务
bool CMercenaryQuestSys::GetMercQuests(long lListID,CMercenaryQuestSys::tagMerQuest& MercQuests)
{
	itMerQ it = m_MerQuests.find(lListID);
	if(it != m_MerQuests.end())
	{
		MercQuests = (*it).second;
		return true;
	}
	return false;
}

void CMercenaryQuestSys::GetMercListQuests(set<long> &quests)
{
	quests.clear();
	if(0 != m_lCurQuestID)
		quests.insert(m_lCurQuestID);
	//itMerQ it = m_MerQuests.begin();
	//for(;it != m_MerQuests.end();it++)
	//{
	//	tagMerQuest &MercQuests = (*it).second;
	//	vector<long>::iterator itQ = MercQuests.Quests.begin();
	//	for(;itQ != MercQuests.Quests.end();itQ++)
	//	{
	//		quests.insert((*itQ));
	//	}
	//}
}

//清除所有俑兵任务数据
void CMercenaryQuestSys::Clear()
{
	m_MerQuests.clear();
}

//得到参数值
long CMercenaryQuestSys::GetData(long lQuestListID,long lPos)
{
	size_t lRet = lPos;
	itMerQ it = m_MerQuests.find(lQuestListID);
	if(it != m_MerQuests.end())
	{
		tagMerQuest& MercQuests = (*it).second;
		if(lRet <= MercQuests.Quests.size())
		{
			return MercQuests.Quests[lPos-1];
		}
	}
	return 0;
}
//设置参数值 
void CMercenaryQuestSys::SetData(long lQuestListID,long lPos,long lValue)
{
    size_t lRet = lPos;
	itMerQ it = m_MerQuests.find(lQuestListID);
	if(it != m_MerQuests.end())
	{
		tagMerQuest& MercQuests = (*it).second;
		if(lRet <= MercQuests.Quests.size())
		{
			MercQuests.Quests[lPos-1] = lValue;
		}
	}
}

//当添加一个任务的时候
long CMercenaryQuestSys::OnAddQuest(long lQuestID)
{
	itMerQ it = m_MerQuests.begin();
	for(;it != m_MerQuests.end();it++)
	{
		tagMerQuest &MercQuests = (*it).second;
		vector<long>::iterator itQ = find(MercQuests.Quests.begin(),MercQuests.Quests.end(),lQuestID);
		if(itQ != MercQuests.Quests.end())
		{
			//MercQuests.Quests.erase(itQ);
			m_lCurQuestID = lQuestID;
			return (*it).first;
		}
	}
	return 0;
}
//当删除一个任务的时候
bool CMercenaryQuestSys::OnDelQuest(long lQuestID)
{
	if(lQuestID == m_lCurQuestID)
	{
		m_lCurQuestID = 0;
		return true;
	}
	return false;
}

bool CMercenaryQuestSys::ClearTime(long MerListID)
{
	itMerQ it=m_MerQuests.find(MerListID);
	if (it!=m_MerQuests.end())
	{
		(*it).second.dwLastUpdateTime=0;
		return true;
	}
	return false;
}

void CMercenaryQuestSys::DisbandQuestRecord(long lQuestID,bool bFlag)
{
	if (!bFlag)
	{
		DisbandQuests.insert(lQuestID);
	}
	else
		DisbandQuests.erase(lQuestID);
}

long CMercenaryQuestSys::FindDisbandQuest(long lQuestID)
{
	set<long>::iterator it=DisbandQuests.find(lQuestID);
	if (it!=DisbandQuests.end())
	{
		return *it;
	}
	return 0;
}

void CMercenaryQuestSys::GetDisbandQuest(set<long> &quests)
{
	set<long>::iterator itQ = DisbandQuests.begin();
	for (;itQ!=DisbandQuests.end();itQ++)
	{
		quests.insert( (*itQ) );
	}
}

void CMercenaryQuestSys::SetMasterQuestID(long lID)
{
	/*if (lID == m_lMasterQuestID)
	{
		m_lMasterQuestID = 0;
	}
	else*/
		m_lMasterQuestID = lID;
}