/**
* @filename:RankSystem.h
* @date: 2010/3/22
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 排行榜客户端系统
*/

#pragma  once
#include "../../../../Public/Common/RankDef.h"

/////////////////////////////////////////////////
// zhaohang  2010/3/23 
// 消息处理
/////////////////////////////////////////////////
/// @brief
/// 排行榜的查询，消息的接收
class RankMsgProc
{
public:
	///构造函数
	RankMsgProc(){ m_eRankQuerySate = RT_OWNER;}
	///接收消息
	void ReceiveMsg(BYTE* pByte, long& lPos) { m_RankInfo.DecordFromByteArray(pByte,lPos); };
	///设置排行榜状态
	void SetRankState(eRankType state){ m_eRankQuerySate = state ; }

	/// @brief
	/// 获取当前服务器返回的消息查询状态
	/// @return eRankQueryState
	/// 返回状态枚举值
	eRankType GetRankState() { return m_eRankQuerySate; }

	/// 获取当前排行榜的页码
	long GetCurPage()const { return m_RankInfo.lCurPage ; }

	/// 获取当前排行榜的排行总页码
	long GetCurTotalPage() {return m_RankInfo.lTotalPage ;}

	/// 获取排行榜信息
	tagRankInfo& GetRankInfo()  { return m_RankInfo;}

	/// 获取排行榜编号
	uint GetRankID(){return (uint)m_RankInfo.lRankID ; }

	/// @brief
	/// 向服务器发送排行榜请求
	/// @param type
	/// 排行榜查询类型
	/// @param RankID
	/// 查询ID
	/// @return void
	/// 
	void Query(eRankType type,long RankID,const char* QueryPlayerName,const char* QueryContent,long PageOrRank = 0);
protected:
private:
	///排行榜信息结构体
	tagRankInfo m_RankInfo;
	/// 当前排行榜查询类型
	eRankType m_eRankQuerySate; 
};

/////////////////////////////////////////////////
// zhaohang  2010/3/25 
// 查询项配置读取
/// @brief
/// 排行榜菜单对应的查询内容读取管理类(从文件"setup/RankSet.ini"中读取
class RankQueryConfig
{
public:
	///构造函数
	RankQueryConfig(){ vecMQC.clear(); }
	///析构函数
	~RankQueryConfig(){ vecMQC.clear(); }
	/// @brief
	/// 读取查询配置文件
	bool ReadConfig();
	/// 获取主菜单项大小
	size_t GetMainMenuSize() const{ return vecMQC.size() ; }
	///  根据索引获取菜单的名称
	const char* GetMainMenuName(uint mainMenuIdx)const { return vecMQC[mainMenuIdx].strMainMenu.c_str(); }
	/// 根据主菜单索引获取对应的子菜单大小
	size_t GetSubMenuSize(size_t index) const { return vecMQC[index].vecSubItem.size(); }

	/// 指定主菜单，子菜单索引，获取菜单名称
	const std::string& GetSubMenuName(size_t mainMenuIdx,size_t subMenuIdx) const { return vecMQC[mainMenuIdx].vecSubItem[subMenuIdx].strItem ; }

	/// 指定主菜单、子菜单索引，获取子菜单ID
	uint GetSubMenuID(size_t mainMenuIdx,size_t subMenuIdx) const { return vecMQC[mainMenuIdx].vecSubItem[subMenuIdx].itemID ;}

	/// 指定主菜单、子菜单索引，获取菜单查询内容
	const std::string& GetSubMenuQueryContent(size_t mainMenuIdx,size_t subMenuIdx) const { return vecMQC[mainMenuIdx].vecSubItem[subMenuIdx].strQueryContent ;}

	/// 指定主菜单、子菜单索引，获取子菜单行大小
	size_t GetSubMenuRowSize(size_t mainMenuIdx,size_t subMenuIdx) { return vecMQC[mainMenuIdx].vecSubItem[subMenuIdx].rowInfo.size(); }
	/// 指定主菜单、子菜单索引、子菜单行索引，获取子菜单行的Header
	const std::string& GetSubMenuRowHeader(size_t mainMenuIdx,size_t subMenuIdx,size_t rowHeaderIdx) const
	{
		return vecMQC[mainMenuIdx].vecSubItem[subMenuIdx].rowInfo[rowHeaderIdx].strRowHeader;
	}
	/// 
	const std::string& GetSubMenuRowDataHeader(size_t mainMenuIdx,size_t subMenuIdx,size_t rowHeaderIdx) const
	{
		return vecMQC[mainMenuIdx].vecSubItem[subMenuIdx].rowInfo[rowHeaderIdx].strRowDataHeader;
	}
protected:
private:
	
	
	///客户端一个子菜单显示的列信息配置
	struct tagRowInfo{
		std::string strRowHeader;///列标题
		std::string strRowDataHeader;///在数据库中对应的字符索引
	};
	typedef std::vector<tagRowInfo> VECRI;
	///一个子菜单查询需要存储的结构
	struct tagSubItem{
		uint itemID;///子菜单ID
		std::string strItem;///子菜单lable
		std::string strQueryContent;///查询内容
		VECRI rowInfo;///列信息配置
	};
	typedef std::vector<tagSubItem> VECSI;
	/// 一个主菜单查询需要存储的结构
	struct tagMenuQueryConfig
	{
		uint mainMenuID;///主菜单ID
		std::string strMainMenu;///主菜单lable
		VECSI vecSubItem;///子菜单结构
	};
	///整个查询需要存储的结构
	std::vector<tagMenuQueryConfig>vecMQC;
};
/////////////////////////////////////////////////