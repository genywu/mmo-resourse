#pragma once
#include "Monster.h"
#include "MasterInfo.h"

class  CSummonedCreature : public CMonster
{
protected:
	DWORD									m_dwSummonedTimeStamp;
	DWORD									m_dwLifeTime;

public:
	/**## 召唤兽的召唤程序处理过程：先创建一个这个类的实例，然后 */
	/**## 调用SetOriginalName函数设置要召唤那种怪物，然后设置好  */
	/**## 生存时间，以及行为模式，对于召唤兽的ai有几点需要注意， */
	/**## 一是不能攻击主人，二是要能理解主人的命令，三是要能根据 */
	/**## 主人的意图进行攻击 */
	CSummonedCreature						();
	virtual ~CSummonedCreature				();

	//##继承于CMonster的函数
	//virtual bool	AddToByteArray			( vector<BYTE>* pByteArray, bool bExData = true );		//## 添加到CByteArray
	//virtual bool	DecordFromByteArray		( BYTE* pByte, long& pos, bool bExData = true );		//## 解码
	virtual void	AI						();														//## 主要是生存时间的判断
	//virtual void	OnDied					();														//## 死亡处理，不处理掉宝事件

	//##Set函数
	VOID			SetLifeTime				( DWORD dwLifeTime );									//## 生存时间

	//##Get函数
	DWORD			GetRemainedTime			();
};
