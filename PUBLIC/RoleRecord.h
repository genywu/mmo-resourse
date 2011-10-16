#pragma once

const DWORD ROLE_PRAME_NUM=10;
const DWORD ROLE_NUM_MAX=65535;
const DWORD ROLE_ERRO=0xffffffff;
const DWORD ROLE_NULL=0xffffffff - 1;

#include <list>
#include <set>
using namespace std;

enum eQuestState
{
	eQS_No=0,//没有该任务
	eQS_Complete,//存在且完成该任务
	eQS_NotComplete,//存在但未完成
};

struct sRoleParam
{
	sRoleParam()
	{
		m_wParam=0;
		m_wExcessRecordIndex=0;
		m_lAccParam=0;
	}
	WORD m_wParam;
	WORD m_wExcessRecordIndex;
	LONG m_lAccParam;
};

struct sExcessRecord
{
	sExcessRecord()
	{
		m_lRegionID=0;
		m_lRegionX=0;
		m_lRegionY=0;
	}
	LONG m_lRegionID;
	LONG m_lRegionX;
	LONG m_lRegionY;
};

//一条任务记录
class CRoleRecord
{
public:
	CRoleRecord();
	CRoleRecord(LONG RoleID,LONG StepID);
	~CRoleRecord();
	// 压数据
	BOOL AddDatatoByteArray(vector<BYTE>* pByteArray);
	//解来数据
	BOOL DecordDataFromByteArray(BYTE* pByte, long& pos);

	BOOL CodeToDataBlock(DBWriteSet& setWriteDB);
	BOOL DecodeFromDataBlock(DBReadSet& setReadDB);

	//取得任务id
	LONG GetRoleID(){return m_lRoleID;}

	//取得当前的步骤id
	WORD GetRoleStep(){return m_cRoleStep;}

	//将任务步骤跳至。。
	VOID JumpStepTo(BYTE stepnum){m_cRoleStep=stepnum;	memset(m_Param,0,sizeof(m_Param));m_cAimNum=0;}

	//变量值
	//取得变量值
	LONG GetParam(LONG Index);						
	
	//设置变量值
	BOOL SetParam(LONG Index, LONG val);			
	
	//增加变量的值
	BOOL AddParam(LONG Index, LONG addval);

	//取得附加变量
	LONG GetAccParam(LONG Index);						

	//设置附加变量值
	BOOL SetAccParam(LONG Index, LONG val);			

	//增加附加变量的值
	BOOL AddAccParam(LONG Index, LONG addval);

	//取得额外记录索引
	WORD GetExcessRecordIndex(LONG aimIndex);

	//设置额外记录索引
	BOOL SetExcessRecordIndex(LONG aimIndex,WORD IndexVal);
	//检查任务变量中的m_wExcessRecordIndex
	BOOL IsExist(WORD wIndex);
private:
	LONG m_lRoleID;									//任务id
	BYTE m_cRoleStep;								//任务进行的步骤
	BYTE m_cAimNum;
	sRoleParam m_Param[ROLE_PRAME_NUM];					//任务变量
};

//玩家任务列表
class CRoleQueue
{
public:
	CRoleQueue();
	~CRoleQueue();
	//根据一个id  获取一个任务指针
	CRoleRecord *GetRecordByID(LONG ID);
	//获取某一任务当前的步骤数
	LONG GetRoleStepNum(LONG ID);
	//寻找某个任务的某一步骤是否存在
	BOOL FindRoleStep(LONG ID , LONG Step);
	
	//取得 某一任务某一步骤的Index对应的变量值
	LONG GetParam(LONG RoleID, LONG Step ,LONG Index);

	//设置 某一任务某一步骤的Index对应的变量值
	BOOL SetParam(LONG RoleID,  LONG Step,LONG Index, LONG val);

	//增加 某一任务某一步骤的Index对应的值
	BOOL AddParam(LONG RoleID, LONG Step ,LONG Index, LONG val);
	//*********************************************************
	//取得 某一任务某一步骤的Index对应的变量值
	LONG GetAccParam(LONG RoleID, LONG Step ,LONG Index);

	//设置 某一任务某一步骤的Index对应的变量值
	BOOL SetAccParam(LONG RoleID,  LONG Step,LONG Index, LONG val);

	//增加 某一任务某一步骤的Index对应的值
	BOOL AddAccParam(LONG RoleID, LONG Step ,LONG Index, LONG val);

	//*********************************************************
	//取得 某一任务某一步骤的Index对应的场景id
	LONG GetRegionID(LONG RoleID, LONG Step ,LONG Index);

	//设置 某一任务某一步骤的Index对应的场景id
	BOOL SetRegionID(LONG RoleID,  LONG Step,LONG Index, LONG val);

	//取得 某一任务某一步骤的Index对应的场景x坐标
	LONG GetRegionX(LONG RoleID, LONG Step ,LONG Index);

	//设置 某一任务某一步骤的Index对应的场景x坐标
	BOOL SetRegionX(LONG RoleID,  LONG Step,LONG Index, LONG val);

	//取得 某一任务某一步骤的Index对应的场景y坐标
	LONG GetRegionY(LONG RoleID, LONG Step ,LONG Index);

	//设置 某一任务某一步骤的Index对应的场景y坐标
	BOOL SetRegionY(LONG RoleID,  LONG Step,LONG Index, LONG val);
	//**********************************************************


	//取得任务列表的任务数量
	LONG GetRoleSum(){return (LONG)m_listRoleRecord.size();}

	//增加一个任务
	BOOL AddOneRecord(CRoleRecord*& ptr);
	//
	BOOL AddOneRecord(LONG roleid,LONG step);
	//删除一个任务
	BOOL DelOneRecord(LONG ID);

	//清理一个任务的附加记录
	BOOL ClearOneRecordEx(CRoleRecord*);
	//将任务调至第n步
	VOID JumpStepTo(LONG lRoleID,BYTE stepnum);

	//取得所在主线任务章节index
	LONG GetChapter(){return m_lChapter;}

	//设置主线任务章节数
	VOID SetChapter(LONG num){m_lChapter=num;}

	//移动主线任务章节数到下一章节
	BOOL JumpChapterNext(){m_lChapter++;return TRUE;}

	//向服务器（world server  压数据）
	BOOL AddQuestDataByteArray(vector<BYTE>* pByteArray);
	//向客户端 压数据
//	bool AddQuestDataByteArray_ForClient(vector<BYTE>* pByteArray);
	//解来自服务器的数据
	BOOL DecordQuestDataFromByteArray(BYTE* pByte, long& pos);
	//客户端解来自服务器的数据
	BOOL CodeToDataBlock(DBWriteSet& setWriteDB);
	BOOL DecodeFromDataBlock(DBReadSet& setReadDB);

	//添加任务记录
	BOOL AddFinishedRoleRecord(LONG index);
	//删除任务记录
	BOOL DelFinishedRoleRecord(LONG index);

	//取得一个就索引的任务的状态（是否完成）
	BOOL GetRoleFinishedRecord(LONG index);
	// 获得任务列表
	list<CRoleRecord*> * GetRoleRecordList()	{return &m_listRoleRecord;}
	//删除任务额外记录多余的数据
	BOOL DeleteExcessRecord();
private:
	sExcessRecord* FindsExcessRecord(WORD index);
	WORD	CreatExcessRecord(LONG regionID,LONG regionX,LONG regionY);
	BOOL	DelExcessRecord(WORD index);

	VOID Clean();
private:
	list<CRoleRecord*>		m_listRoleRecord;					//任务列表
	CRoleRecord*			m_pCurrentPtr;						//操作指针，优化速度。
	LONG					m_lChapter;							//所在主线任务章节
	set<LONG>				m_setRoleRecord;					//任务记录
	map<WORD , sExcessRecord> m_mapExcessRecord;				//额外记录
};

//俑兵任务系统类
class CMercenaryQuestSys
{
public:
	CMercenaryQuestSys();
	~CMercenaryQuestSys();

	//定义一个俑兵任务列表结构体,包含4个可接受的任务
	struct tagMerQuest
	{
		tagMerQuest():dwLastUpdateTime(0){}
		DWORD dwLastUpdateTime;
		vector<long>	Quests;
	};
private:	
	set<long>	DisbandQuests;
	typedef map<long,tagMerQuest>	mapMerQuests;
	typedef map<long,tagMerQuest>::iterator itMerQ;
	//所有俑兵任务列表
	mapMerQuests	m_MerQuests;
	//当前接受的俑兵任务
	long	m_lCurQuestID;
	//最后一次完成的赏金任务
	long	m_lLastQuestID;
	//队长共享的任务
	long	m_lMasterQuestID;
public:
	//压数据
	bool AddDataByteArray(vector<BYTE>* pByteArray);
	//解数据
	bool DecordDataFromByteArray(BYTE* pByte, long& pos);

	bool CodeToDataBlock(DBWriteSet& setWriteDB);
	bool DecodeFromDataBlock(DBReadSet& setReadDB);

	//得到当前的接收的俑兵任务
	long GetCurQuestID()	{ return m_lCurQuestID; }
	void SetCurQuestID(long lID) { m_lCurQuestID=lID; }
	//设置队长共享的任务
	void SetMasterQuestID(long lID);
	long GetMasterQuestID()				{return m_lMasterQuestID;}		
	//得到最后次完成的赏金任务
	long GetLastQuestID()	{return m_lLastQuestID;}
	void SetLastQuestID(long lID)	{ m_lLastQuestID = lID; }
	//判断在列表中是否存在该任务
	long IsExistQuest(long lQuestID);
	//判断在列表中是否存在该任务列表的数据
	bool IsExistQuestList(long lQuestListID);
	//得到该列表ID的俑兵任务
	bool GetMercQuests(long lListID,tagMerQuest& MercQuests);
	//得到当前所有的俑兵任务
	void GetMercListQuests(set<long> &quests);
	//清除所有俑兵任务数据
	void Clear();
	//设置某个列表的俑兵任务
	void SetMercQuests(long lListID,const tagMerQuest& quests){m_MerQuests[lListID]=quests;}
	//得到参数值
	long GetData(long lQuestListID,long lPos);
	//设置参数值 
	void SetData(long lQuestListID,long lPos,long lValue);
	//当添加一个任务的时候
	long OnAddQuest(long lQuestID);
	//当删除一个任务的时候
	bool OnDelQuest(long lQuestID);
	//清空刷新时间
	bool ClearTime(long MerListID);
	//记录在赏金任务列表中,放弃的任务.增加lListID参数判断是否是当前列表里面的任务
	void DisbandQuestRecord(long lQuestID,bool bFlag = false);
	//查找放弃任务列表
	long FindDisbandQuest(long lQuestID);
	//得到放弃任务列表
	void GetDisbandQuest(set<long> &quests);
};
