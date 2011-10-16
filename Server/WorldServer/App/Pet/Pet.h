// Fox@20081120----------------------------------------------
// File:    Pet.h
// Brief:   CPet定义了宠物的属性及操作. 现有接口暂未考虑可派生子类.
//			主要涉及宠物初始化及释放, 宠物事件管理及响应.
//
// Author:  Fox (yulefox@gmail.com)
// Date:    Nov.18, 2008
// Fox@20081118----------------------------------------------

#pragma once
#include "../MoveShape.h"
#include "PetCtrl.h"
#include "../../../../Public/Common/BasePetDef.h"

class CPet :
	public CMoveShape
{
public:
	CPet(void);
	~CPet(void);

	friend class CPetCtrl;

	virtual bool CodeToDataBlock(DBWriteSet& setWriteDB,
		bool bExData = true);							// WS==>GS编码
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,
		bool bExData = true);							// GS==>WS解码

private:

	// 采用结构方式存取的成员变量均为基本类型, 方便编解码和数据存取
	// 如果非基本类型, 在使用时应注意独立使用.
	tagPetDiff			m_DiffAttrs;					// 差异属性
	CGUID				m_HostID;						// 主人
	SKILL_LIST			m_Skills;						// 技能列表
};
