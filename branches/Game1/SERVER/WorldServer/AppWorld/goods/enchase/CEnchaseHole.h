#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

class CGoods;

//×°±¸ÏâÇ¶¿×
class CEnchaseHole:public BaseMemObj
{
public:
	CEnchaseHole();
	CEnchaseHole(CGoods* pParent);
	~CEnchaseHole();	
private:
	CGoods* m_pParent;
	//¿×²ÛÊÇ·ñ±»¼¤»î
	BOOL m_bIsActived; 
	//ËùÏâÇ¶¿¨Æ¬
	CGoods* m_pCard;
	//
	DWORD dwCardGoodsId;
public:
	VOID SetParent(CGoods* pGoods){m_pParent=pGoods;}	

	CGoods* GetParent(){return m_pParent;}	

	CGoods* GetCard(){return m_pCard;}

	VOID SetCard(CGoods* pCard){m_pCard=pCard;}

	VOID SetState(BOOL bActived){m_bIsActived=bActived;}

	VOID SetCardGoodsId(DWORD dwId){if(dwId>0) dwCardGoodsId=dwId;}

	BOOL GetState(){return m_bIsActived;}

	BOOL CanEnchase(CGoods* pGoods,int nIndex);

	BOOL EnchaseCard(CGoods* pGoods,int nIndex);

	BOOL RemoveCard(int nIndex);
};