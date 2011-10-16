//		文件名:	Collection.h
//		作者:	heliangchen
//		时间:	2008/1/26
//		简介:	采集物类的实现.在场景中添加采集物,供副职业 和 任务采集用

#include "MoveShape.h"

#include "../../../Public/Common/DeputyOccu.h"

class CCollection:public CMoveShape
{
public:
	CCollection();
	~CCollection();
public:

	//对应刷新id
	DWORD				GetRefreshIndex	()						{return m_dwRefreshIndex;}
	void				SetRefreshIndex	(DWORD l)				{m_dwRefreshIndex=l;}

	//hp
	virtual DWORD		GetHP()									{return 1;}
	virtual void		SetHP(DWORD l)							{}
	BYTE				GetBlockType()							{return m_byBlockType;}
	void				SetBlockType(BYTE l)					{m_byBlockType= l;}

public:
	BOOL Init(const CCollectionLayout* p);
	//virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 压码
	//virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

	virtual bool CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData = true);		// 添加到CByteArray
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);	// 解码


	virtual void				UpdateCurrentState	(){}
	virtual bool IsAttackAble(CMoveShape* pAttacker/*攻击者*/){return false;};	// 可否攻击
	//添加删除事件
	//BOOL AddDeleteEvent(DWORD dwTime);
	//添加重生事件
	//BOOL AddReliveEvent(DWORD dwTime);
	//定时器操作
	//virtual long OnTimeOut(DWORD timerid,DWORD curtime,const void* var);
	//被完全采集
	virtual BOOL BeenCollectedAll();
//
	void OnLoseBeneficiary();			//丢失受益者事件
	void StartRecordBeneficiary(CGUID guidFirst,OBJECT_TYPE typeFirst,DWORD dwBeneficiaryTime);				//开始记录受益者
	//
	const string& GetStartCollectScript(){return m_strStartCollectScript;}
	void	SetStartCollectScript(const string & strScriptName){m_strStartCollectScript= strScriptName;}

	const string& GetEndCollectScript(){return m_strEndCollectScript;}
	void	SetEndCollectScript(const string & strScriptName){m_strEndCollectScript= strScriptName;}

	void SetPosXY(float fX,float fY);
	void SetBlock(int x, int y, unsigned short block);
protected:
	long m_lDiedEvent;
	void AddDiedEvent();
	void OnDiedEvent();
	bool LogoutDiedEvent();
	void DelDiedEvent();
protected:
	////事件操作
	//BOOL AddEvent(eChildObjType dwType,DWORD dwTime);
	//BOOL LogoutEvent(LONG lTimerID);
	//BOOL DeleteEvent(LONG lTimerID);
	//事件响应
	//void OnDelEvent();
	BOOL OnRelive();
protected:

	//刷新索引
	DWORD	m_dwRefreshIndex;

	//记录timerid
	LONG	m_lTimerID;
	BYTE	m_byBlockType;

	//开始采集脚本
	string m_strStartCollectScript;
	//采集结束脚本脚本
	string m_strEndCollectScript;


};