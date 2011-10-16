#pragma once
#include "Shape.h"
#include "..\..\..\Nets\MySocket.h"
#include "MasterInfo.h"
#include "..\..\..\Public\SkillRelated.h"

//##召唤的没有生命的召唤物,比如毒雾,魔法阵
class CSummonShape : public CShape
{
protected:
	//##主人的有关信息
	tagMasterInfo									m_tMasterInfo;
	//##是否可以移动
	BOOL											m_bMoveable;
	//##生命长度，0表示不消失
	DWORD											m_dwLifeTime;
	//##诞生时间
	DWORD											m_dwTimeStamp;

	tagSkillID										m_tSkillID;

	//static LONG										g_lID;

	DWORD				GetRemainedTime				();

public:
	//##构造/析构函数
    CSummonShape									( tagMasterInfo& tMasterInfo, DWORD dwLifeTime );
	~CSummonShape									();

	//##设置当前动作
	virtual VOID		SetAction					( WORD act );
	//##接收到消息后的处理
	virtual VOID		OnMessage					( CMessage* pMsg ) {}
	//##AI相关
	virtual VOID		AI							() = 0;
    //##移动
	virtual VOID		MoveStep					( LONG lDir );
	//##强制移动
	virtual VOID		ForceMove					( LONG lDestX, LONG lDestY, DWORD dwTime );

	//##返回召唤技能的类型
	virtual tagSkillID	GetSkillID					()
	{
		return m_tSkillID;
	}
	//##相同技能的作用区域替换
	virtual VOID		ReplaceAffectRegion			( LONG lLevel, LONG lXStart, LONG lYStart ) = 0;

	virtual	VOID		End							();

	virtual bool		AddToByteArray				( vector<BYTE>* pByteArray, bool bExData = true ) = 0;
	
	virtual bool		DecordFromByteArray			( BYTE* pByte, LONG& lPos, bool bExData ) { return true; }
};

struct tagRetangle
{
	DWORD											dwX;
	DWORD											dwY;
	DWORD											dwLength;
	DWORD											dwHeight;
};

//##召唤物的作用域，有些不规则的形状
class CScope
{
	BYTE*											m_pScope;
	DWORD											m_dwLength;
	DWORD											m_dwHeight;

public:
	CScope											( DWORD dwLength, DWORD dwHeight );
	~CScope											();

	//##获取此格是否为作用区域
	BOOL				Get							( DWORD dwX, DWORD dwY );
	//##设置好作用域
	VOID				Set							( BYTE* pBfr );
	VOID				Set							( BYTE b, DWORD dwX, DWORD dwY );

	//##获取宽
	__inline DWORD		GetLength					() 
	{
		return m_dwLength;
	}
	//##获取高
	__inline DWORD		GetHeight					()
	{
		return m_dwHeight;
	}

	CScope&				operator =					( CScope& cIn );
	BOOL				operator ==					( CScope& cIn );

	//##判断2个区域是否重叠,如果重叠,则返回重叠区域的大小
	static BOOL			GetOverlappingField			( tagRetangle& rSrc1, tagRetangle rSrc2, tagRetangle& rOut );
};