////////////////////////////////////////////////////////////////////////////
//CityGate.h//////////////////////////////////////////////////////////////
//城门类
//Author:Joe
//Create Time:2004.721
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "build.h"

class CCityGate :
	public CBuild
{
public:
	CCityGate(void);
	~CCityGate(void);
public:
	virtual	bool Display(void);
	virtual void AI(void);
	long AI_Open();							// 打开的AI

	virtual void SetAction(WORD l);

	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);

};
