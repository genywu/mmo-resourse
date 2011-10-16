/** 
* @file   PetManager.h
* @author wing
* @brief  宠物管理类，定义宠物相关公共操作，消息处理函数.
*
* @date   2010-06-01       
*/

#pragma once

class PetManager
{
public:
	PetManager(void);
	~PetManager(void);

public:
	/// 宠物修改名称
	void OnChangePetName(const CGUID& guid, const char* name);
	/// 删除宠物
	void OnDeletePet(const CGUID& guid);
	/// 宠物升级
	void OnPetLevUpgrade(const CGUID& guid);
	/// 宠物升级道行等级
	void OnPetDLevUpgrade(const CGUID& guid);
	/// 宠物改变状态
	void OnPetChangeState(const CGUID& guid);

private:
	/// 根据宠物GUID取得宠物
	CPet* GetPet(const CGUID& guid);
};
