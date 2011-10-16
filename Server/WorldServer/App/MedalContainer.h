
/*
*	file:		MedalContainer.h
*	Brief:		勋章容器类
*	detail:		
*	Authtor:	张科智
*	Datae:		2008-08-26
*/


#pragma once

class CGoods;

//! 勋章容器
////////////////////////////////////////////////////////////////////////
class MedalContainer
{
public:
	MedalContainer(void);
	~MedalContainer(void);

public:
	//!						编码
	void					AddToByteArray(vector<BYTE> &vData);
	//!						编码
	void					Decode(BYTE *pData, LONG &lPos);

	//!						编码
	void					CodeToDataBlock(DBWriteSet& setWriteDB);
	//!						编码
	void					DecodeFromDataBlock(DBReadSet& setReadDB);
public:
	//!						按索引(勋章ID)添加勋章
	BOOL					AddMedal(LONG lGoodsBaseIndex);
	//!						得到拥有的勋章
	void					GetHoldMedal(set<CGoods*> &setGoods);

private:
	//!						拥有的勋章(按索引记)
	set<LONG>				m_setHold;



};