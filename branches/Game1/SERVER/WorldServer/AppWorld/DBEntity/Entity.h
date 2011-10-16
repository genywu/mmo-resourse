//Entity.h/////////////////////////////////////////////////////////////////////
//对象类:该对象只有其自己的属性数据,在结构树中成为叶子节点
//Author:安海川
//Create Time:2008.11.03
#ifndef ENTITY_H
#define ENTITY_H
#include <list>
#include <vector>
#include <string>
#include <set>
#include "../public/msgtype.h"
#include "../public/GUID.h"
#include "../public/entityproperty.h"
#include "../public/dbdefine.h"
#include "BaseEntity.h"

// 游戏物体基类，所有对象和游戏相关的类由该类派生
class CEntity : public CBaseEntity
{
public:
	CEntity(const std::string& flag, const CGUID& guid);
	virtual ~CEntity();

	// virtual interfaces
	virtual void ProcessMsg(bool isLoadOperFlag, const CGUID& SessionID, long retFlag, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	virtual void GetFromByteArray(bool isLoadOperFlag, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	void AddToByteArray(std::vector<BYTE>& pBA);

	CEntity& operator=(CEntity& right);

	// 初始化属性配置
	virtual void InitProperty();	
};

#endif//DB_ENTITY_H
