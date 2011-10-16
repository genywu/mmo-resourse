
/*
*	file:		AutoSupplyData.h
*	Brief:		自动补给数据对象
*	detail:		提供对象的编解码接口
*	Authtor:	张科智
*	Datae:		2008-05-21
*/

#include "SpriteDef.h"

#pragma once

class AutoSupplyData
{
public:
	AutoSupplyData(void);
	~AutoSupplyData(void);


public:
	//!								编码
	void							AddToByteArray		(vector<BYTE> &ByteArray);
	//!								解码
	void							GetFormByteArray	(BYTE *pByteArray, long &lPos);
	
	//!								编码
	void							CodeToDataBlock		(DBWriteSet& setWriteDB);
	//!								解码
	void							DecodeFromDataBlock	(DBReadSet& setReadDB);


	//!								重置对象
	void							Reset				(void);


	//!								获得一个设置（uType）
	const	tagAotoSupplyNode*		GetOneSetup			(LONG eType);
	//!								修改一个设置
	LONG							SetOneSetup			(LONG eType, const tagAotoSupplyNode &AotoSupplyNode);



private:
	//!								自动喝药的设置数据
	tagAotoSupplyNode				m_AotoSupplySetup[AotoSupplyTypeNum];

};