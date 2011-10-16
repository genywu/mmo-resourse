////////////////////////////////////////////////////////////////////////////
//CityGate.h//////////////////////////////////////////////////////////////
//城门类
//Author:Joe
//Create Time:2004.721
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "build.h"
#include "ServerRegion.h"
#include "..\setup\GateList.h"


//门类
class CGate:
	public CBuild
{
public:
	CGate(void);
	~CGate(void);
	virtual BOOL InitProperty(CGateList::tagGate* p);
	virtual BOOL InitSetProperty(CServerRegion::tagGateProperty* p);
public:
	virtual void SetGate(WORD l);
	virtual void SetOpen(string str)	{m_strOpen=str;}
	virtual void SetClose(string str)	{m_strClose=str;}
	virtual void SetOpenSound(string str)	{m_strOpenSound=str;}
	virtual void SetCloseSound(string str)	{m_strCloseSound=str;}
	virtual string &GetOpen()				{return m_strOpen;}
	virtual string &GetClose()				{return m_strClose;}
	virtual string &GetOpenSound()			{return m_strOpenSound;}
	virtual string &GetCloseSound()			{return m_strCloseSound;}
	
	virtual bool CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData = true);		// 添加到CByteArray
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);	// 解码
private:


	string	m_strOpen;		//打开脚本
	string	m_strClose;		//关闭脚本
	string	m_strOpenSound;	//开门音效
	string  m_strCloseSound;//关门音效

public:
	virtual void OnDied();															// 被击毁时处理
	virtual bool IsAttackAble(CMoveShape* pAttackShape);								// 可否攻击		
};