#pragma once

const ulong ROLE_PRAME_NUM=10;
const ulong ROLE_NUM_MAX=65535;
const ulong ROLE_ERRO=0xffffffff;
const ulong ROLE_NULL=0xffffffff - 1;

enum eQuestState
{
	eQS_No=0,       //没有该任务
	eQS_Complete,   //存在且完成该任务
	eQS_NotComplete,//存在但未完成
};

enum eShareResult
{
    eQsExist,         //存在玩家任务列表
    eQsFinish,        //已完成此任务列表
    eQsNo,            //不存在
};

enum  eQuestTextId
{
    eNoQuestId,
    eNoQuest =1,
    eComQuest,
    eNoComQuest_No,
    eNoComQuest_Fin,
    eComQuestText,    //附加任务完成后结束显示文本
    /// 客户端
    eQuestDes,        //任务描述
    eQuestAward,
};

struct sQuestParam
{
	sQuestParam()
	{
		m_wParam=0;
		m_wExcessRecordIndex=0;
		m_lAccParam=0;
	}
	long m_wParam;
	long m_wExcessRecordIndex;
	long m_lAccParam;
};

struct sExcessRecord
{
	sExcessRecord()
	{
		m_lRegionID=0;
		m_lRegionX=0;
		m_lRegionY=0;
	}
	long m_lRegionID;
	long m_lRegionX;
	long m_lRegionY;
};

//一条任务记录
class CQuestRecord
{
public:
	CQuestRecord();
	CQuestRecord(long QuestID,long StepID);
	~CQuestRecord();

	// 压数据
	bool AddDatatoByteArray(vector<uchar>* pByteArray);
	//解来数据
	bool DecordDataFromByteArray(uchar* pByte, long& pos);

    bool CodeToDataBlock(DBWriteSet& setWriteDB);
	bool DecodeFromDataBlock(DBReadSet& setReadDB);

	//取得任务id
	long   GetQuestID(){return m_lQuestID;}

	//取得当前的步骤id
	ushort GetQuestStep(){return m_cQuestStep;}

	//将任务步骤跳至。。
	void   JumpStepTo(uchar stepnum){m_cQuestStep=stepnum;	memset(m_Param,0,sizeof(m_Param));m_cAimNum=0;}

	//变量值
	//取得变量值
	long GetParam(long Index);						
	
	//设置变量值
	bool SetParam(long Index, long val);			
	
	//增加变量的值
	bool AddParam(long Index, long addval);

	//取得附加变量
	long GetAccParam(long Index);						

	//设置附加变量值
	bool SetAccParam(long Index, long val);			

	//增加附加变量的值
	bool AddAccParam(long Index, long addval);

	//取得额外记录索引
	ushort GetExcessRecordIndex(long aimIndex);

	//设置额外记录索引
	bool SetExcessRecordIndex(long aimIndex,ushort IndexVal);

	//检查任务变量中的m_wExcessRecordIndex
	bool IsExist(ushort wIndex);

private:

	long    m_lQuestID;									//任务id
	uchar   m_cQuestStep;								//任务进行的步骤
	uchar   m_cAimNum;
	sQuestParam m_Param[ROLE_PRAME_NUM];				    //任务变量
};

//玩家任务列表
class CQuestQueue
{

public:
	CQuestQueue();
	~CQuestQueue();

	//根据一个id  获取一个任务指针
	CQuestRecord *GetRecordByID(long ID);
	//获取某一任务当前的步骤数
	long         GetQuestStepNum(long ID);
	//寻找某个任务的某一步骤是否存在
	bool         FindQuestStep(long ID , long Step);
	
	//取得 某一任务某一步骤的Index对应的变量值
	long GetParam(long QuestID, long Step ,long Index);

	//设置 某一任务某一步骤的Index对应的变量值
	bool SetParam(long Quest,  long Step, long Index, long val);

	//增加 某一任务某一步骤的Index对应的值
	bool AddParam(long Quest, long Step ,long Index, long val);
	//*********************************************************
	//取得 某一任务某一步骤的Index对应的变量值
	long GetAccParam(long Quest, long Step ,long Index);

	//设置 某一任务某一步骤的Index对应的变量值
	bool SetAccParam(long Quest,  long Step,long Index, long val);

	//增加 某一任务某一步骤的Index对应的值
	bool AddAccParam(long Quest, long Step ,long Index, long val);

	//*********************************************************
	//取得 某一任务某一步骤的Index对应的场景id
	long GetRegionID(long Quest, long Step ,long Index);

	//设置 某一任务某一步骤的Index对应的场景id
	bool SetRegionID(long Quest,  long Step,long Index, long val);

	//取得 某一任务某一步骤的Index对应的场景x坐标
	long GetRegionX(long Quest, long Step ,long Index);

	//设置 某一任务某一步骤的Index对应的场景x坐标
	bool SetRegionX(long Quest,  long Step,long Index, long val);

	//取得 某一任务某一步骤的Index对应的场景y坐标
	long GetRegionY(long Quest, long Step ,long Index);

	//设置 某一任务某一步骤的Index对应的场景y坐标
	bool SetRegionY(long Quest,  long Step,long Index, long val);
	//**********************************************************


	//取得任务列表的任务数量
	long GetQuestSum(){return (long)m_listQuestRecord.size();}
	//增加一个任务
	bool AddOneRecord(CQuestRecord*& ptr);
	//
	bool AddOneRecord(long Quest,long step);
	//删除一个任务
	bool DelOneRecord(long ID);

	//清理一个任务的附加记录
	bool ClearOneRecordEx(CQuestRecord*);
	//将任务调至第n步
	void JumpStepTo(long lQuest,uchar stepnum);

	//取得所在主线任务章节index
	long GetChapter(){return m_lChapter;}

	//设置主线任务章节数
	void SetChapter(long num){m_lChapter=num;}

	//移动主线任务章节数到下一章节
	bool JumpChapterNext(){m_lChapter++;return true;}

	//向服务器（world server  压数据）
	bool AddQuestDataByteArray(vector<uchar>* pByteArray);
	//向客户端 压数据
    //bool AddQuestDataByteArray_ForClient(vector<uchar>* pByteArray);
	//解来自服务器的数据
	bool DecordQuestDataFromByteArray(uchar* pByte, long& pos);
	//客户端解来自服务器的数据
	bool CodeToDataBlock(DBWriteSet& setWriteDB);
	bool DecodeFromDataBlock(DBReadSet& setReadDB);

	//添加任务记录
	bool AddFinishedQuestRecord(long index);
	//删除任务记录
	bool DelFinishedQuestRecord(long index);

	//取得一个就索引的任务的状态（是否完成）
	bool GetQuestFinishedRecord(long index);
	// 获得任务列表
	list<CQuestRecord*> * GetQuestRecordList()	{return &m_listQuestRecord;}
	//删除任务额外记录多余的数据
	bool DeleteExcessRecord();

private:
	sExcessRecord* FindsExcessRecord(ushort index);

	ushort	CreatExcessRecord(long regionID,long regionX,long regionY);
	bool	DelExcessRecord(ushort index);

	void Clean();

private:

	list<CQuestRecord*>		m_listQuestRecord;					//任务列表
	CQuestRecord*			m_pCurrentPtr;						//操作指针，优化速度。
	long					m_lChapter;							//所在主线任务章节
	set<long>				m_setQuestRecord;					//任务记录
	map<ushort , sExcessRecord> m_mapExcessRecord;				//额外记录
};

//俑兵任务系统类
class CMercenaryQuestSys
{ 
public:
    //定义一个俑兵任务列表结构体,包含4个可接受的任务
    struct tagMerQuest
    {
        tagMerQuest():dwLastUpdateTime(0){}
        ulong dwLastUpdateTime;
        vector<long>	Quests;
    };

    typedef map<long,tagMerQuest>	mapMerQuests;
    typedef map<long,tagMerQuest>::iterator itMerQ;

public:

	CMercenaryQuestSys();
	~CMercenaryQuestSys();

private:

	set<long>	    DisbandQuests;
	//所有俑兵任务列表
	mapMerQuests	m_MerQuests;
	//当前接受的俑兵任务
	long	        m_lCurQuestID;
	//最后一次完成的赏金任务
	long	        m_lLastQuestID;
	//队长共享的任务
	long	        m_lMasterQuestID;

public:
	//压数据
	bool AddDataByteArray(vector<uchar>* pByteArray);
	//解数据
	bool DecordDataFromByteArray(uchar* pByte, long& pos);
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
