#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

using namespace std;

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
	BOOL m_bIsActived; 
	//所镶嵌卡片
	CGoods* m_pCard;
public:
	VOID SetParent(CGoods* pGoods){m_pParent=pGoods;}	

	CGoods* GetParent(){return m_pParent;}	

	CGoods* GetCard(){return m_pCard;}

	VOID SetCard(CGoods* pCard){m_pCard=pCard;}

	VOID SetState(BOOL bActived){m_bIsActived=bActived;}

	BOOL GetState(){return m_bIsActived;}

	BOOL CanEnchase(CGoods* pGoods);

	BOOL EnchaseCard(CPlayer* pPlayer,CGoods* pCard);

	BOOL RemoveCard(CPlayer* pPlayer,bool bReserve);
};