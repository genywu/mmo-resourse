#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

class CGoods;

//装备镶嵌孔
class CEnchaseHole 
{
public:
	CEnchaseHole();
	CEnchaseHole(CGoods* pParent);
	~CEnchaseHole();	
private:
	CGoods* m_pParent;
	//孔槽是否被激活
	//王晓轩：已更新为所有孔槽都默认为开启状态
	bool m_bIsActived; 
	//所镶嵌卡片
	CGoods* m_pCard;
public:
	void SetParent(CGoods* pGoods){m_pParent=pGoods;}	

	CGoods* GetParent(){return m_pParent;}	

	CGoods* GetCard(){return m_pCard;}

	void SetCard(CGoods* pCard){m_pCard=pCard;}

	void SetState(bool bActived){m_bIsActived=bActived;}

	bool GetState(){return m_bIsActived;}

	bool CanEnchase(CGoods* pGoods);

	bool EnchaseCard(CPlayer* pPlayer,CGoods* pCard);

	bool RemoveCard(CPlayer* pPlayer,bool bReserve);
};