////////////////////////////////////////////////////////////////////////////
//build.h///////////////////////////////////////////////////////////////////
//建筑类，此物体有生命和防御，无AI
//Author:Joe
//Create Time:2004.721
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "MoveShape.h"
//#include "ServerRegion.h"

class CBuild :
	public CMoveShape
{
public:
	CBuild(void);
	~CBuild(void);

protected:
	enum eBuildState
	{
		STATE_CLOSE,
		STATE_OPEN,
		STATE_DESTORY,
		STATE_REFRESH,
	};
	long			m_lIndex;				//编号

	long			m_lMaxHP;				//  最大生命值 
	long			m_lHP;					//	当前生命值
	long			m_lDef;					//  防御力
	long			m_lMagicDef;			//  魔法防御力

	long			m_lNameColor;			//名字颜色
	long			m_lLenth;				//长度
	long			m_lWidth;				//宽度
	long			m_lLevel;				//等级
	string			m_strAttack;			//受攻击脚本
	string			m_strDestory;			//损毁脚本
	string			m_strRefresh;			//重刷脚本	
	string			m_strHitSound;			//击中音效
	string			m_strDestorySound;		//损毁音效
	BYTE			m_byBlockType;			//阻挡类型
public:
	virtual void SetAction(WORD l);


	virtual void SetIndex(long x)			{m_lIndex=x;}
	void SetBlock(int x, int y, unsigned short block);
	virtual void SetMaxHP(long lmaxhp)		{m_lMaxHP=lmaxhp;}
	virtual void SetHP(long lhp);
	virtual void SetDef(long ldef)			{m_lDef=ldef;}
	virtual void SetMagicDef(long lmagicdef){m_lMagicDef=lmagicdef;}
	virtual void SetNameColor(long lcolor)	{m_lNameColor=lcolor;}
	virtual void SetLenth(long llenth)		{m_lLenth=llenth;}
	virtual void SetWidth(long lwidth)		{m_lWidth=lwidth;}
	virtual void SetLevel(long level)		{m_lLevel=level;}	
	virtual void SetAttackScp(string str)	{m_strAttack=str;}
	virtual void SetDestoryScp(string str)	{m_strDestory=str;}
	virtual void SetRefreshScp(string str)	{m_strRefresh=str;}
	virtual void SetHitSound(string str)	{m_strHitSound=str;}
	virtual void SetDestorySound(string str){m_strDestorySound=str;}
	virtual long GetIndex()					{return m_lIndex;}
	virtual DWORD GetHP()					{return m_lHP;}
	virtual string &GetDestory()			{return m_strDestory;}
	virtual string &GetRefresh()			{return m_strRefresh;}
	void SetBlockType(BYTE byBlock)			{m_byBlockType = byBlock;}
	BYTE GetBlockType()						{return m_byBlockType;}
	//virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	//virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
	virtual bool CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData = true);		// 添加到CByteArray
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);	// 解码

	virtual void OnBeenAttacked			( tagAttackInformation& ai );
	virtual void ApplyFinalDamage		( tagAttackInformation& ai, vector<tagDamage*>& vRet );
	virtual void OnDied();															// 被击毁时处理
	virtual void OnBeenMurdered( LONG lMurdererType, const CGUID& MurdererGuid );
	virtual void OnBeenHurted( LONG lAttackerType, const CGUID& AttackerGuid ); 
	

	virtual DWORD	GetMaxHP()						{return m_lMaxHP;}
	virtual long	GetLenth()						{return m_lLenth;}
	virtual long	GetWidth()						{return m_lWidth;}
	WORD	GetDef()								{return static_cast<WORD>(m_lDef);}
	virtual long OnRefresh();
	virtual bool IsAttackAble(CMoveShape* pAttackShape);								// 可否攻击
	virtual WORD GetFigure(eDIR dir=DIR_UP);										// 根据方向得到体形
	virtual long GetAttackerDir(long lAttackerX,long lAttackerY,long ldx,long ldy);
	virtual void GetBeAttackedPoint(long lAttackerX,long lAttackerY,long& lTileX,long& lTileY);

	void MoveAroundPlayer();
	map<string, tagPty>		    m_mapNameValue;		// 变量名字对应的变量地址
	typedef map<string, tagPty>::iterator mapNameValueItr; 
	//脏数据
	set<string> m_mapDirtyAttrData;
	void SetBuild(WORD l);
	void AddAttributeToDirty(string attrName);
	void InitNameValueMap();
	bool SetAttribute(std::string& attrName, long value);
	bool SetAttribute(char* attrName, long value);
	long GetAttribute( std::string attrName );
	virtual long GetCurrentValue(const tagPty& tagValue, long evalue/*改变值*/, long bvalue/*基本值*/);
	virtual bool  ChangeAttribute(std::string& attrName, long value);
	virtual long  UpdateAttributeToClient();
	long  UpdateAttribute(BOOL bImmediately=0);
	long  OnUpdateAttributeTimeOut();

	virtual long GetValue(const char* strName);
	virtual long SetValue(const char* strName, long dwValue);
	long GetCurrentValue();
	//void AddChangeAttrToMsgArray(std::vector<BYTE>* bArray);
	void CodeChangeAttrToDataBlock(DBWriteSet& setWriteDB);
public:

//------------------------------------------------------------------------
//	继承于CMoveShape需要实例化的虚函数 -- Added By Jar
//------------------------------------------------------------------------
 
	virtual void			UpdateCurrentState()	{}
private:
	void SetPosXY(float fX,float fY);
};
