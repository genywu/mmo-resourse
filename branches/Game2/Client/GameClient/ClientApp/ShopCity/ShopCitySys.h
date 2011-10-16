/**
* @filename:ShopCitySys.h
* @date: 2010/7/19
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 商城的逻辑层模块
*/

class CMessage;
class ShopCityMsgMgr;

//#define SAFEDELETE(op)         if (op) { delete op;op=NULL;}

///商城里的道具数据逻辑层管理类
class SCGData
{
public:
	///商城里的商店类型
	enum eSCType{
		ST_XINGZUAN = 0,	///星钻
		ST_DIANQUAN,		///点券
		ST_WEIMIAN,			///位面
		ST_NUM				///类型数目
	};
	/// 物品属于的商店类型
	enum eSType
	{
		/// 热销商品
		TABTYPE_HOT	 = 0,
		/// 药品商店
		TABTYPE_YAOPIN,
		/// 日常杂货
		TABTYPE_ASSISTANT,
		/// 装备材料
		TABTYPE_EQUIP,
		/// 时装饰品
		TABTYPE_FASHION,
		/// 种类数量
		TABTYPE_NUM,
	};
	///物品的信息的部分结构
	struct tagGDPTA{
		uint type;
		uint index;
	};
	///完整物品信息结构
	struct tagGD 
	{
		uint type;///类型
		uint index;///编号
		uint price;///价格
		uint num;///数量
		uint discount;///打折
		string timelimit;///限时
		long numlimit;///限量
		long buylimit;///限购
	};
	///购买记录
	struct tagBRec{
		std::string buytime;///购买时间
		uint pricenum;///货币数量
		uint priceType;///货币类型
		uint goodsIdx;///商品ID
		uint buynum;///购买数量
		tagBRec(){
			buytime = "";
		};
		~tagBRec(){
			buytime = "";
		};
	};

	typedef std::vector<tagGD>VecGD;
	//typedef std::vector<tagGD*>VecGDPtr;
	typedef std::map<eSCType,VecGD> MapSCGD;///所有商品
	//typedef std::map<eSCType,VecGDPtr>MapSCGDPtr;

	typedef std::vector<tagBRec> VecSCGBRec;///个人所有购买记录

	typedef std::vector<tagGDPTA>VecGDPTA;
	typedef std::map<eSCType,VecGDPTA> MapTop10A;///热销商品

	typedef MapTop10A MapNewestA;///新品推荐

	typedef MapTop10A MapSBT10;///个人最近购买

	typedef std::map<std::string,VecGDPTA> MapStrGGDTPA;
	
	typedef std::map<eSCType,MapStrGGDTPA> MapGuideDataA; ///导购

	typedef std::map<std::string,VecGDPTA> MapStrFilDTPA;
	typedef std::map<eSType,MapStrFilDTPA> MapUFLDTPA;
	typedef std::map<eSCType,MapUFLDTPA> MapFLDTA;///筛选

	SCGData();
	~SCGData();

	///获取新品推荐列表
	MapNewestA& GetNewestVec() { return m_newestMapA;}

	///获取所有物品数据
	MapSCGD& GetAllGoodsList() {return m_goodsDTMap;}

	///获取个人够买详细记录
	VecSCGBRec& GetBuyLogList() { return m_buyRecordVec;}

	///获取热销商品列表
	MapTop10A& GetTop10List() {return m_Top10MapA;}
	///获取导购列表
	MapGuideDataA& GetGuideList() { return m_GuideMapA;}
	///获取筛选列表
	MapFLDTA& GetFilterList() { return m_FilterMapA;}
	///获取个人最近购买简单记录
	//VecSelfBuyTop10A& GetSelfBuyTop10() { return m_SelfLatestBuyTop10VecA;}
	MapSBT10& GetSelfBuyTop10() { return m_SelfLatestBuyTop10MapA; }

	/// @brief
	/// 根据商城类型，商店类型返回对应的物品数据Vec
	/// @param
	/// 
	/// @return
	/// 
	void GetVecGoodsData(eSCType scType,eSType sType,VecGD& vecGD);

	/// @brief
	/// 根据给定的物品部分结构，返回物品详细数据VEC
	/// @param
	/// 
	/// @return
	/// 
	void GetTop10InGoodsData(eSCType compType,VecGD& vecGD);

	/// @brief
	/// 根据给定的物品商城，商店，和筛选文本，返回物品详细数据
	/// @param
	/// 
	/// @return
	/// 
	void GetFilterInGoodsData(eSCType sctype,eSType stype,string filterstr,VecGD& vecGD);
	/// @brief
	/// 根据给定的商城，商店和导购文本，返回物品详细数据
	/// @param
	/// 
	/// @return
	/// 
	void GetGuideInGoodsData(eSCType sctype,string guidstr,VecGD& vecGD);
	/// @brief
	/// 根据物品索引和商城类型获取具体的物品售卖数据
	/// @param
	/// 
	/// @return
	/// 
	tagGD* GetGoodsData(uint index,eSCType shopcitytype);

protected:
private:
	friend class ShopCityMsgMgr;
	///解码商城售卖数据
	void DecodeFromDataBlock(CMessage* pMsg);
	/// 解码购买记录数据
	void DecodeBuyLogDataBlock(CMessage* pMsg);
	/// 更新数据
	void Update(CMessage* pMsg);
	/// @brief
	/// 根据物品商店类型，ID， 商城类型获取一个物品数据结构
	/// @param
	/// 
	/// @return
	/// 
	tagGD* GetGoodsData(eSType goodsType,uint index,eSCType shoptype);
	/// @brief
	/// 清空所有列表数据
	/// @param
	/// 
	/// @return
	/// 
	void Clear();
	//清空所有商店物品数据
	void ClearAllGoodsList();
	//清空个人购买详细记录
	void ClearBuyLogList();
	//清空热销商品列表
	void ClearTop10List();
	//清空新品推荐列表
	void ClearNewestList();
	//清空导购列表
	void ClearGuideList();
	//清空筛选列表
	void ClearSelectList();
	//清空个人最近购买简单记录
	void ClearSelfLatestBuyList();

	MapSCGD m_goodsDTMap;///根据商店类型定义的所有商品信息列表
	//MapSCGDPtr m_goodsDTMapPtr;
	VecSCGBRec m_buyRecordVec;		///个人购买商品记录，包含详细的购买时间等

	MapTop10A			m_Top10MapA;		///热销商品

	MapNewestA			m_newestMapA;		///新品推荐

	MapGuideDataA		m_GuideMapA;		///导购

	MapFLDTA      		m_FilterMapA;				///筛选

	MapSBT10			m_SelfLatestBuyTop10MapA;	///个人最近购买记录，只包含了物品Id和type
};
///商城的消息管理类
class ShopCityMsgMgr
{
public:
	/// @brief
	/// 构造函数,对商城数据进行了new操作
	/// @param
	/// 
	/// @return
	/// 
	ShopCityMsgMgr();
	/// @brief
	/// 析构函数,delete 商城数据
	/// @param
	/// 
	/// @return
	/// 
	~ShopCityMsgMgr();
	/// @brief
	/// 切换页面开或关
	/// @param
	/// 
	/// @return
	/// 
	void SwitchPage();
	/// @brief
	/// 接收服务器返回商城打开界面反馈的消息
	/// @param 
	/// 
	/// @return
	/// 
	void Receive_PageOpen(CMessage* pMsg);
	/// @brief
	/// 接收服务器返回商城关闭消息
	/// @param
	/// 
	/// @return
	/// 
	void Receive_PageClose(CMessage* pMsg);

	/// @brief
	/// 接收购买反馈
	/// @param
	/// 
	/// @return
	/// 
	void Receive_BuyResult(CMessage* pMsg);

	/// @brief
	/// 接收更新消息
	/// @param
	/// 
	/// @return
	/// 
	void Receive_Updata(CMessage* pMsg);

	/// @brief
	/// 接收购买日志
	/// @param
	/// 
	/// @return
	/// 
	void Receive_LogResponse(CMessage* pMsg);
	/// @brief
	/// 接收商城通告
	/// @param
	/// 
	/// @return
	/// 
	void Receive_AfficheResponse(CMessage* pMsg);

	/// @brief
	/// 
	/// @param
	/// 
	/// @return
	/// 
	const char* GetAffiche();

	/// @brief
	/// 返回商城数据
	/// @param
	/// 
	/// @return
	/// 
	SCGData* GetShopCityGoodsData(){ return dt;}

	/// @brief
	/// 
	/// @param
	/// 
	/// @return
	/// 
	uint GetPlayerXinZuan();
	/// @brief
	/// 
	/// @param
	/// 
	/// @return
	/// 
	uint GetPlayerDianQuan();
	/// @brief
	/// 
	/// @param
	/// 
	/// @return
	/// 
	uint GetPlayerWeiMian();
	/// @brief
	/// 
	/// @param
	/// 
	/// @return
	/// 
	void SetPageState(bool IsOpen);
	/// @brief
	/// 
	/// @param
	/// 
	/// @return
	/// 
	static int GetPageInfo(int ShopCityIdx,int ShopIndex);

	/// @brief
	/// 
	/// @param
	/// 
	/// @return
	/// 
	static void SetPageInfo(int ShopCityIdx,int ShopIndex,int InPage);
	/// @brief
	/// 获取UI更新源类型,0.商城商店，1.筛选 2.导购
	/// @param
	/// 
	/// @return
	/// 
	static uint GetStateUpdateUIByType(){return stateUpdateUIbyType;}

	/// @brief
	/// 设置UI更新源类型,0.商城商店，1.筛选 2.导购
	/// @param
	/// 
	/// @return
	/// 
	static void SetStateUpdateUIByType(uint i){ stateUpdateUIbyType = i ;}
	/// @brief
	/// 发送购买消息
	/// @param goodsID
	/// 物品ID
	/// @param buyNum
	/// 购买数量
	/// @param buyMode
	/// 购买时用哪种付费，与商店类型一致
	/// @return
	/// 
	void Send_BuyQuery(uint goodsID = 0,uint buyNum = 0,uint buyMode = 0);
	/// @brief
	/// 发送获取购买日志
	/// @param
	/// 
	/// @return
	/// 
	void Send_GetBuyLog();

	/// @brief
	/// 发送获取商城公告
	/// @param
	/// 
	/// @return
	/// 
	void Send_GetAffiche();

	/// @brief
	/// 发送关闭商城消息
	/// @param
	/// 
	/// @return
	/// 
	void Send_CloseShop(void);
private:
	/// @brief
	/// 向服务器发送请求打开商城界面
	/// @param
	/// 
	/// @return
	/// 
	void Send_PageOpen();

	/// @brief
	/// 发送关系商城
	/// @param
	/// 
	/// @return
	/// 
	void Send_PageClose();

	/// @brief
	/// 判断玩家是否已经死亡,是返回true并弹出用CEGUI封装好的messagebox，否返回false
	/// @param
	/// 
	/// @return
	/// 
	bool IsPlayerDied();
	/// @brief
	/// 调用接口
	/// @param
	/// 
	/// @return
	/// 
	void DoMsgBoxWhenDied();
	

	bool IsPageOpen;		///标识界面是否打开(纯逻辑层次上，与具体界面无关)

	SCGData* dt;	///商城物品数据类
	std::string Affiche;	///商城公告信息字符串
	
	static int arrPGInfo[3][4];///保存对应的浏览页信息[商城类型数目][商店类型数目]

	static uint stateUpdateUIbyType;///标识UI更新源类型,以方便在翻页时进行区分:0.商城商店，1.筛选 2.导购
};