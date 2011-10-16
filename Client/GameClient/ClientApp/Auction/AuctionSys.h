/**
* @filename:AuctionHouseSys.h
* @date: 2010/5/6
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 仙贝寄售系统逻辑层
*/
#include "../../../../Public/Common/AHBaseDef.h"

class CMessage;
/// @brief
/// 数据结构，包括编解码 
class AHdata
{
public:
	///构造函数
	AHdata();
	///析构函数
	~AHdata();

	///解码，填充订单数据,optype类型属于AHBaseDef.h定义的 message operation type 枚举值里的前三个，就返回成功，否则返回失败
	bool DecodeFromDataBlock(CMessage* msg,uint optype);

	///
	typedef std::vector<AH::OrderRaw>vecOrderRaw;
	///可购买挂单列表
	vecOrderRaw m_CanBuyOrder;
	/// 可出售挂单列表
	vecOrderRaw m_CanSaleOrder;

	///以订单ID作为map的key
	typedef std::map<uint,AH::SubOrderRaw> MapSubOrderRaw;
	///玩家自己的挂单信息列表
	MapSubOrderRaw m_AgentOrder;

	///向服务器发送查询所有数据
	void Send_AH_REQUEST_ALL();

	///向服务器发送查询可购买订单列表
	void Send_AH_REQUEST_BUY();

	///向服务器发送查询可出售订单列表
	void Send_AH_REQUEST_SELL();

	///向服务器发送查询个人委托订单列表,可提取信息
	void Send_AH_REQUEST_AGENT();

	///向服务器发送添加委托订单,0 = OT_SELL,1 = OT_BUY
	void Send_AH_ADD_AGENT(AH::OrderRaw& or,int ordertype);

	///向服务器发送撤销委托订单，orderID 标识订单ID号
	void Send_AH_REMOVE_AGENT(int orderID);

	///向服务器发送购买消息,orederID指定购买的订单ID，cnt购买数量
	void Send_AH_BUY(long cnt);

	///向服务器发送出售消息，orederID指定出售的订单ID，cnt出售数量
	void Send_AH_SEL(long cnt);

	///向服务器发送提取消息,type = PT_GOODS(位面) type = PT_COIN(金币)
	void Send_AH_PICKUP(int type);

	///获得可提取金币数目
	uint GetNumGoldCanPickUp(){ return m_GoldPUp;}

	///获取可提取位面数
	uint GetNumWeimianCanPickUp(){ return m_WeiMianPUp; }

	///设置当前可出售的订单ID号
	void SetCanSaleID(uint id){ m_CanSaleID = id ; }

	/// 获取当前可出售的订单号
	uint GetCanSaleID() const { return m_CanSaleID;}

	///设置当前可购买的订单ID号
	void SetCanBuyID(uint id){ m_CanBuyID = id ;  }

	///获取当前可购买的订单号
	uint GetCanBuyID() const { return m_CanBuyID; }

	////根据可购买订单ID，获取可购买的单价
	uint GetPriceByCanBuyID(uint ID);

	////根据可购买订单ID，获取可购买的单价
	uint GetCountByCanBuyID(uint ID);

	////根据可出售订单ID，获取可出售数目
	uint GetPriceByCanSaleID(uint ID);

	////根据可出售订单ID, 获取可出售的单价
	uint GetCountByCanSaleID(uint ID);

private:
	uint m_GoldPUp;///可提取金币数量
	uint m_WeiMianPUp;///位面可提取数目

	uint m_CanSaleID;///单前要出售的订单ID号
	uint m_CanBuyID;///当前要购买的订单ID号
};