
//定义和Player相关的结构、枚举
#pragma once

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif

class CGoods;
/*
* 功能: 邮件系统 's Def
* 摘要: 玩家可以通过发（收）邮件，获得彼此之间联系，或互相交流物品等道具。
*		该信息将存储于服务器内一定时间，直到玩家阅读后主动删除信息，或信件保存时间到达。
* 作者: 刘科
* 创建日期: 08.9.17
* 修改日志: 2008.12.1 tanhaowen 添加到test下
*/
// 会产生提示的邮件操作类型

enum eMailTipType
{
	DELMAIL_TIP = 0,			// 删除邮件的提示类型
	SENDMAIL_TIP,				// 发送邮件的提示类型
	UNKNOWN_TIP				// 不知类型
};

enum eMailGoodsType
{
	GOODS_NORMAL_TYPE = 0,			// 普通物品
	GOODS_TSHOP_ASKFOR_TYPE,		// 增值商店索要物品
	GOODS_TSHOP_LARG_TYPE,			// 增值商店赠送物品
	GOODS_UNKNOW_TYPE				// 不明物品
};

struct stGoodsInfo 
{
	CGoods *pGoods;					//  物品
	DWORD	dwPos;					//	在背包栏中的坐标
	DWORD	dwType;					//	物品所在的容器类型
	DWORD	dwYuanBao;				//	元宝价格
	stGoodsInfo() : pGoods(NULL), dwPos(0), dwType(0), dwYuanBao(0)
	{}
};

// 客户端定义的邮件信息结构
// struct	stMailData
// {
// 	CGUID			gMailID; 			// 邮件的ID
// 	DWORD			dwTimeStart;		// 接收邮件的开始时间(秒为单位)
// 	DWORD			dwTime;				// 邮件的有效时间(秒为单位)
// 	DWORD			dwGoldNum;			// 操作的金钱数
// 	bool			bIsReaded;			// 是否阅读过
// 	bool			bIsInDelList;		// 是否已被添加到了被删除列表中去
// 	bool			bIsCanReBack;		// 是否可以退信
// 	string			strMailName;		// 邮件名称
// 	//char			strMailName[128];
// 	string			strMailDesc;		// 邮件内容
// 	//char			strMailDesc[2048];
// 	string			strMailDescEx;		// 邮件附加内容
// 	//char			strMailDescEx[2048];
// 	string			strSenderName;		// 邮件发送者名称
// 	//char			strSenderName[128];
// 	long			colorTitle;			// 信件标题字体颜色
// 	long			colorSender;		// 信件发送者字体颜色
// 	long			colorGoodsEx;		// 附件物品遮罩的颜色
// 	list<CGoods*>	GoodsList;			// 附件物品列表
// 	eMAILTYPE		eMailType;			// 邮件的类型（5种：普通，普通物品，付款收信，索要，赠送）
// 	stMailData()
// 	{
// 		gMailID = CGUID::GUID_INVALID;
// 		dwTimeStart   = 0;
// 		dwTime		  = 0;
// 		dwGoldNum	  = 0;
// 		bIsReaded	  = false;
// 		bIsInDelList  = false;
// 		bIsCanReBack  = true;
// 		ZeroMemory(strMailName,strlen(strMailName));
// 		ZeroMemory(strMailDesc,strlen(strMailDesc));
// 		ZeroMemory(strMailDescEx,strlen(strMailDescEx));
// 		ZeroMemory(strSenderName,strlen(strSenderName));
// 		colorTitle	  = 0xffffffff;
// 		colorSender	  = 0xffffffff;
// 		colorGoodsEx  = 0xffffffff;
// 		eMailType	  = COMMON_MAIL;
// 	}
// };
struct	stMailData
{
	CGUID			gMailID; 			// 邮件的ID
	DWORD			dwTimeStart;		// 接收邮件的开始时间(秒为单位)
	DWORD			dwTime;				// 邮件的有效时间(秒为单位)
	DWORD			dwGoldNum;			// 操作的金钱数
	bool			bIsReaded;			// 是否阅读过
	bool			bIsInDelList;		// 是否已被添加到了被删除列表中去
	bool			bIsCanReBack;		// 是否可以退信
	string			strMailName;		// 邮件名称
	string			strMailDesc;		// 邮件内容
	string			strMailDescEx;		// 邮件附加内容
	string			strSenderName;		// 邮件发送者名称
	long			colorTitle;			// 信件标题字体颜色
	long			colorSender;		// 信件发送者字体颜色
	long			colorGoodsEx;		// 附件物品遮罩的颜色
	list<CGoods*>	GoodsList;			// 附件物品列表
	eMAILTYPE		eMailType;			// 邮件的类型（3种：普通，普通物品，付款收信）
	stMailData()
	{
		gMailID = CGUID::GUID_INVALID;
		dwTimeStart = 0;
		dwTime	= 0;
		dwGoldNum = 0;
		bIsReaded = false;
		bIsInDelList = false;
		bIsCanReBack = true;
		strMailName = "";
		strMailDesc = "";
		strMailDescEx = "";
		strSenderName = "";
		colorTitle = 0xffffffff;
		colorSender = 0xffffffff;
		colorGoodsEx = 0xffffffff;
		eMailType = COMMON_MAIL;
	}
}; 
#define MAIL_PREPAGE 8			// 邮件列表一页的个数
// 状态列表
