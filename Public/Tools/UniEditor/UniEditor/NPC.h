#pragma once
#include "info.h"

class CNPC :
	public CInfo
{
public:
	CNPC(void);
public:
	~CNPC(void);

public:
	// 读NPC文件 "*.npc"
	friend ifstream &operator>>(ifstream &rstream, CNPC &rNPC); 
	friend ofstream &operator<<(ofstream &rstream, const CNPC &rNPC); 

// 数据读写
	const inline string& GetName()const						{return m_strName;}
	const inline unsigned GetPicID()const					{return m_uQuantity;}
	const inline string& GetScript()const					{return m_strScript;}

	inline void SetName(const string &strName)				{m_strName = strName;}
	inline void SetPicID(const unsigned uPicID)				{m_uPicID = uPicID;}
	inline void SetScript(const string &strScript)			{m_strScript = strScript;}

	void NPC2String(CString &str);

// NPC信息
	string		m_strName;			// NPC名称
	unsigned	m_uPicID;			// 图片ID
	string		m_strScript;		// 动作脚本
};

ifstream &operator>>(ifstream &rstream, CNPC &rNPC); 
ofstream &operator<<(ofstream &rstream, const CNPC &rNPC); 
