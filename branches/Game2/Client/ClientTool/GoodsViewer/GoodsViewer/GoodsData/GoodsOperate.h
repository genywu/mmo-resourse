/**
* @filename:GoodsOperate
* @date: 2010/2/3
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 

* @purpose: 封装Goods的数据操作
*/

#pragma  once
#include "GoodsAttrTypeDef.h"
#include <CEGUI.h>
namespace UIData{
	const int gGoodsTypeNum = 26 ;
	using namespace CEGUI;
	class GoodsOperate
	{
	public:
		/// @brief
		/// 构造函数,无操作
		/// @param
		/// 
		/// @return
		/// 
		GoodsOperate();

		/// @brief
		/// 析构函数，无操作
		/// @param
		/// 
		/// @return
		/// 
		~GoodsOperate();

		/// @brief
		/// 以二进制从goodslist.dat中读取Goods信息
		/// @param const char* FileName
		/// 文件名称（包含路径),默认为“goodslist.dat”
		/// @return
		/// 读取成功返回true,失败返回false
		bool LoadGoodsListData(const char* FileName = "goodslist.dat");

		/// @brief
		/// 读取物品附加属性项名称
		/// @param const char* FileName
		/// 文件名称（包含路径),默认为“itemtype.dat”
		/// @return
		/// 成功返回true，失败返回false
		bool LoadGoodsItemTypeData(const char* FileName = "itemtype.dat");

		
		/// @brief
		/// 保存物品信息指定路径的文件中
		/// @param SavePath
		/// 保存的文件路径
		/// @return
		/// 成功保存返回true，失败返回false
		bool SaveGoodsListData(const char* SavePath = "");

		bool SaveGoodsListDataForClient();

		bool CountConvetEGoodsProperty(tagGoodAttr* pGoods );

		/// @brief
		/// 根据物品索引号获取物品数据结构
		/// @param Index
		/// 物品的索引号
		/// @return tagGoodAtttr*
		/// 返回指定索引号的物品数据结构体的指针，没有就返回NULL;
		const tagGoodAttr* GetGoodAttrByIndex(size_t Index) const ;

		/// @brief
		/// 根据物品原始名称获取物品数据结构体
		/// @param &OgriName
		/// 
		/// @return tagGoodAttr*
		/// 返回指定原始名称的物品数据结构体指针
		const tagGoodAttr* GetGoodAttrByOgriName(const String &OgriName) const;

		/// @brief
		/// 返回物品总数
		/// @param
		/// 
		/// @return size_t
		/// 返回物品总数
		size_t GetGoodsTotalNum() const { return m_vecGoodAttr.size();}

		/// @brief
		/// 获取指定索引的物品类型描述字符串
		/// @param Index
		/// 
		/// @return &string
		/// 
		static const std::string GetGoodsType(size_t Index) ;

		/// @brief
		/// 返回物品附加属性条目数
		/// @param void
		/// 
		/// @return size_t
		/// 
		size_t GetGoodItemTypeSize() const { return m_vecGoodItemType.size() ;}

		//根据索引返回物品附加属性字符串描述
		std::string& GetGoodItemType(const size_t Index) {return m_vecGoodItemType[Index];}
	protected:
	private:
		std::vector<tagGoodAttr> m_vecGoodAttr;
		std::vector<std::string> m_vecGoodItemType;
		static const std::string s_vecGoodsType[26];
		};
}