#pragma once
#include "shape.h"

class CMoveShape :
	public CShape
{
public:
	CMoveShape(void);
	virtual ~CMoveShape(void);

//------------------------------------------------------------------------
// 异常状态
//------------------------------------------------------------------------
protected:
	vector<BYTE>						m_vExStates;

public:
	VOID			SetExStates			( BYTE* pBytes, LONG lSize );
	vector<BYTE>&	GetExStates			()	{return m_vExStates;}

//////////////////////////////////////////////////////////////////////////
//	无敌模式
//////////////////////////////////////////////////////////////////////////
public:

	void God( bool bIsGod = true )		{ m_bIsGod = bIsGod; }
	bool IsGod()						{ return m_bIsGod; }

protected:
	bool m_bIsGod;
};
