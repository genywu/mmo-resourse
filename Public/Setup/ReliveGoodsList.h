//======================================================================================
/**
*	@file		ReliveGoodsList.h
*	@data		2009-08-27
*	@author		陈先进
*/
//======================================================================================
#pragma  once


/**
*	@class ReLiveGoodsList
*
*	@brief 复活物品列表类
*/
class ReLiveGoodsList
{
public:	
	/// 复活物品结构
	struct tagReLiveGoods
	{
		string strOrignalName;	//原始名
		ulong  dwIndex ;		//索引
		ulong  dwNum;			//数量
		ulong  dwRecoverRatio;	//恢复比例
	};
	
	/// 构造
	ReLiveGoodsList();

	/// 析构
	~ReLiveGoodsList();

	/// 从文件加载复活物品列表
	static bool Init(const char* filename);

	/// 清除
	static void Release();

	/// 编码
	static bool AddToByteArray(vector<uchar>& pByteArray);

	/// 解码	
	static bool DecordFromByteArray(uchar* pByte, long& pos);

	/// 获取复活物品列表
	static list<tagReLiveGoods*>& GetReliveGoodsList() {return m_lReLiveGoods;}
	
private:
	/// 所有的复活物品列表
	static list<tagReLiveGoods*> m_lReLiveGoods;
};