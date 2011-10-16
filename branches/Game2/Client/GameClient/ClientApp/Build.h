////////////////////////////////////////////////////////////////////////////
//build.h///////////////////////////////////////////////////////////////////
//建筑类，此物体有生命和防御，无AI
//Author:Joe
//Create Time:2004.721
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "MoveShape.h"

class CBuild :
	public CMoveShape
{
public:
	CBuild(void);
	~CBuild(void);

	// 初始化变量名到值得影射
	void InitNameValueMap();
public:
	// 属性
	struct tagProperty
	{
		//long			lMaxHP;				//  最大生命值 
		//long			lHP;				//	当前生命值
		//long			lDef;				//  防御力
		//long			lElementResistant;	//	元素抗性
		//long			lWidthInc;			//  体型
		//long			lHeightInc;			 
		long			lIndex;				//编号

		long			lMaxHP;				//  最大生命值 
		long			lHP;				//	当前生命值
		long			lDef;				//  防御力
		long			lMagicDef;			//  魔法防御力

		long			lNameColor;			//名字颜色
		long			lWidthInc;			//长度
		long			lHeightInc;			//宽度
		WORD		    wLevel;				//等级
	//string			strHitSound;		//击中音效
	//string			strDestorySound;	//损毁音效
	};

private:
	tagProperty			m_Property;			// 属性
	long                m_Switch;           // 无敌开关

	string			strHitSound;		//击中音效
	string			strDestorySound;	//损毁音效
	string          strOpenSound;       //开门音效
	string          strCloseSound;      //关门音效

public:

	virtual void SetAction(WORD l);

	virtual	bool Display(void);
	virtual void AI(void);
	long AI_Stand();			// 站AI
	long AI_BeAttack();			// 被攻擊AI
	long AI_Died();				// 死亡AI
	long AI_Open();				// 开门AI

	virtual void OnActionChange();

	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);

	virtual DWORD	GetHp()							{return m_Property.lHP;		}
	virtual VOID	SetHp(DWORD l)					{m_Property.lHP=(long)l;	}
	virtual VOID    SetMaxHp(DWORD l)               {m_Property.lMaxHP=(long)l; }
	virtual DWORD	GetMaxHp()						{return m_Property.lMaxHP;	}
	virtual DWORD	GetLevel()						{return m_Property.wLevel;	} 
	VOID  SetSwitch(bool l)                        { m_bUnAttack=(long)l;   }
	bool GetSwitch()                               {return m_bUnAttack;}
	long  GetNameColor()                            {return m_Property.lNameColor;}
	virtual	DWORD	SetValue(const char* strName, DWORD dwValue);
	void  UpdateProperty(CMessage* pMsg);
	void  DecordChangeProperty(BYTE* pByte, long& pos);
	void  DecodeChangePropertyFromDataBlock(DBReadSet& setReadDB);

	virtual BYTE GetFigure(eDIR dir=DIR_UP);										// 根据方向得到体形
};
