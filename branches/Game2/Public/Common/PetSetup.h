/**
* @file    PetCtrl.h
* @author  wing 
* @date    2010-6-24
* @brief   客户端、服务器通用宠物配置.
*/

#pragma once 

#include "BasePetDef.h"
#include "../DataStream/DataBlockSetWriteRead.h"

class PetSetup
{
public:
	PetSetup(void);
	~PetSetup(void);

	/// 加载宠物强化材料配置
	bool LoadPetMaterialConfig(void);

	/// 加载宠物推荐加点配置
	bool LoadPetNominateModeConfig(void);

	/// 添加到CByteArray
	bool CodeToDataBlock(DBWriteSet& setWriteDB);		

	/// 解码
	bool DecodeFromDataBlock(DBReadSet& setReadDB);

	/// 取得强化配置列表
	MATERIAL_LIST* GetMaterialList(void) { return &m_materialList; }

	/// 根据强化类型取得强化说明文字
	string GetExplainTextByID(long type);

private:
	MATERIAL_LIST	m_materialList;							///< 侍从强化配置
};