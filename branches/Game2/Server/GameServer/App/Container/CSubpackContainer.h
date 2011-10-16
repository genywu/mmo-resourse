#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif


#include "../Listener/CContainerListener.h"
#include "CVolumeLimitGoodsContainer.h"
#include "CGoodsContainer.h"

class CPlayer;
class CGoods;
class CBaseObject;

class CContainer;

//子背包容器类
class CSubpackContainer 
: public CGoodsContainer
, public CContainerListener
{
public:
	
	enum SUBPACK_COLUMN
	{
		EC_PACK1=10001,
		EC_PACK2=10002,
		EC_PACK3=10003,
		EC_PACK4=10004,
		EC_PACK5=10005
	};

	//子背包结构
	struct tagSubpack
	{
		CGoods* pGoods;//背包物品
		CVolumeLimitGoodsContainer* pSubpackContainer;//背包容器
	};

	static const int SUBPACK_NUM=5;

private:
	tagSubpack m_Subpack[SUBPACK_NUM];
	

public:	
	
	CSubpackContainer();		

	~CSubpackContainer();
	
	virtual bool Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41B3D0A1025D
	virtual CBaseObject* Find(const CGUID& guid);

	//##ModelId=41B3D0B80395
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

	//##ModelId=41C0E6DB0119
	virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid = NULL);

	//##ModelId=41B3D0C20191
	virtual void Clear(LPVOID pVoid = NULL);

	//##ModelId=41B3D0C9024D
	virtual void Release();

	//! 清除容器中的所有物品，并释放物品内存
	void ReleaseAllGoods();

	//##ModelId=41B418F902D3
	virtual void TraversingContainer(CContainerListener* pListener);

	//##ModelId=41B5456901AB
	//virtual DWORD GetContentsWeight();

	//##ModelId=41B6917E00AB
	virtual bool IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex);

	//##ModelId=41BFB91C01D4
	virtual bool OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41BFB92601A5
	virtual bool OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41BF9DAB0109
	virtual bool Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41BFA0F30177
	virtual void AI();

	//##ModelId=41BFA1030271
	virtual bool IsFull();

	//##ModelId=41BFA16E03B9
	virtual bool QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition);

	//##ModelId=41BFA17A00BB
	virtual bool QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition);

	//##ModelId=41BFA197036B
	virtual CGoods* GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex);

	//##ModelId=41BFA1AC0242
	virtual CGoods* GetGoods(DWORD dwPosition);

	//##ModelId=41BFA1B8007D
	virtual void GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut);

	//##ModelId=41BFEAD70138
	virtual DWORD GetGoodsAmount();
	virtual DWORD GetEquipAmount();

	virtual DWORD GetGoodsAmountByName(const char* strName);

	virtual DWORD GetGoodsAmountByType(int nType);


	//在指定位置替换一个物品,如果成功,返回被替换出的物品,否则
	//返回NULL。
	//##ModelId=41B570CD0068
	bool Swap(SUBPACK_COLUMN ecColumn, CGoods* pIn, CGoods** ppOut, LPVOID pVoid = NULL);
	CSubpackContainer::tagSubpack* GetSubpack(int i);
	CSubpackContainer::tagSubpack* GetBusinessPackTag(DWORD& dwPos);

	virtual bool CanPutIn(CGoods* pGoods,CPlayer* pPlayer);

	bool CanSwap(SUBPACK_COLUMN ecColumn,CGoods* pIn);

	virtual CBaseObject*	Remove					( CBaseObject* pObject, LPVOID pVoid = NULL );

	virtual CBaseObject*	Remove					( LONG oType, const CGUID& guid, LPVOID pVoid = NULL );

	virtual CBaseObject*	Find					( CBaseObject* pObj );

	virtual CBaseObject*	Find					( LONG oType, const CGUID& guid );

	virtual bool			Serialize				( DBWriteSet& setWriteDB, bool b = true );
	virtual bool			Unserialize				( DBReadSet& setReadDB, bool b = true );
};