#pragma once

#include "guid.h"
#include "DataBlockSetWriteRead.h"

// 奖励描述
struct tagBounsDes
{
	DWORD	dwBounsID;		// 奖励资源ID
	string	strTitle;		// 标题
	string	strDes;			// 描述
	//BYTE	bType;			// 接口类型(1-道具、2-数值、3-脚本)
	//string	strName;		//	bType 1-道具名称、2-属性名、3-空
	//DWORD	dwValue;		//	bType 1-道具数量、2-属性值、3-脚本ID

	tagBounsDes()
	{
		dwBounsID	= 0;
		strTitle	= "";
		strDes		= "";
	}

	void EncodeToDataBlock(DBWriteSet& setWriteDB);
	void DecodeFromDataBlock(DBReadSet& setReadDB);
	
};

// 奖励信息
struct tagBounsInfo
{
	CGUID	guid;				// 奖励策略编号
	string	strAccount;			// 领奖玩家账号
	BYTE	bStatus;			// 领取状态(1-未领取、2-冻结、3-已领取、4-过期) 
	DWORD	dwBounsID;			// 奖励资源ID
    BYTE	bType;			    // 接口类型(1-道具、2-数值、3-脚本)
    string	strName;		    //	bType 1-GoodsIndex、2-属性名、3-空
    DWORD	dwValue;		    //	bType 1-道具数量、2-属性值、3-脚本ID
	time_t	tValidTime;			// 有效期

	tagBounsInfo()
	{
		strAccount	= "";
		bStatus		= 0;
		dwBounsID	= 0;
        bType		= 0;
        strName		= "";
        dwValue		= 0;
	}

	void EncodeToDataBlock(DBWriteSet& setWriteDB);
	void DecodeFromDataBlock(DBReadSet& setReadDB);
};

// 上传物品信息
struct tagUploadGoods
{
    CGUID   playerGUID;                 // 角色GUID
    string  strAccount;                 // 角色Account(盛大数字ID)
    string  strGoodsName;               // 物品名称
    DWORD   dwGoodsIndex;               // 物品Index
    DWORD   dwGoodsCounts;              // 物品数量
    DWORD   dwWSNumber;                 // WorldServer编号
    DWORD   dwGSNumber;                 // GameServer编号    

    tagUploadGoods()
    {
        memset(this, 0, sizeof(tagUploadGoods));
    }
    void EncodeToDataBlock(DBWriteSet& setWriteDB)
    {
        setWriteDB.AddToByteArray(playerGUID);
        setWriteDB.AddToByteArray((char*)strAccount.c_str());
        setWriteDB.AddToByteArray((char*)strGoodsName.c_str());
        setWriteDB.AddToByteArray(dwGoodsIndex);
        setWriteDB.AddToByteArray(dwGoodsCounts);
        setWriteDB.AddToByteArray(dwWSNumber);
        setWriteDB.AddToByteArray(dwGSNumber);
    }
    void DecodeFromDataBlock(DBReadSet& setReadDB)
    {
        char strTmp[256];
        setReadDB.GetBufferFromByteArray(playerGUID);
        strAccount = setReadDB.GetStringFromByteArray(strTmp, 256);
        strGoodsName = setReadDB.GetStringFromByteArray(strTmp, 256);
        dwGoodsIndex = setReadDB.GetDwordFromByteArray();
        dwGoodsCounts = setReadDB.GetDwordFromByteArray();
        dwWSNumber = setReadDB.GetDwordFromByteArray();
        dwGSNumber = setReadDB.GetDwordFromByteArray();
    }
};

// 公告信息
struct tagNotice
{
    DWORD   dwID;
    time_t  tBegin;             // 公告开始时间
    time_t  tEnd;               // 公告结束时间
    string  strText;            // 内容
    WORD    wType;              // 播放类型：0系统，1玩家
    WORD    wPlay;              // 播放方式：0播放一次，1循环播放
 //   WORD    wGameID;            // 0全区全服，101星尘传说，102奇侠
    WORD    wFlag;              // 播放状态：0未播放，1正在播放，2已播放

    tagNotice()
    {
        memset(this, 0, sizeof(tagNotice));
    }
};

typedef map<DWORD, tagNotice> MapNotice;
