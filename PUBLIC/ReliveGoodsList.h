/*
*	file:		ReliveGoodsList.h
*	Brief:		复活物品列表类
*	detail:		
*	Authtor:	陈先进
*	Datae:		2009-08-27
*/

#include "tinyxml.h"

// 复活物品列表类
class CReLiveGoodsList
{
public:	
	//复活物品结构
	struct tagReLiveGoods
	{
		string strOrignalName;	//原始名
		DWORD  dwIndex ;		//索引
		DWORD  dwNum;			//数量
		DWORD  dwRecoverRatio;	//恢复比例
	};
	//吸收经验物品的结构
	struct tagExpGoods
	{
		/*string strOrignalName;	//原始名*/
		DWORD   dwExpRatio;		//吸取比例
		string strScript;		//触发脚本名
		string strHint;			//提示文字
	};


	CReLiveGoodsList();
	~CReLiveGoodsList();

	
	static bool Init(const char *filename);						// 解析XML中关于复活物品的配置
	static void Release();										// 释放资源	

	static bool AddToByteArray(vector<BYTE> &pByteArray);		// 添加到CByteArray
	static bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码	
	static list<tagReLiveGoods*> &GetReliveGoodsList() {return m_lReLiveGoods;}//获取复活物品列表
	static tagExpGoods* GetExpGoods(string strName);
	
private:
	static list<tagReLiveGoods*>	 m_lReLiveGoods;				// 所有的复活物品列表
	static map<string,tagExpGoods*>  m_mpExpGoods;					// 吸收经验的物品列表
	typedef map<string,tagExpGoods*>::iterator Eit;
};