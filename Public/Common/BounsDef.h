#pragma once

// 奖励描述
struct tagBounsDes
{
	ulong	dwBounsID;		// 奖励资源ID
	string	strTitle;		// 标题
	string	strDes;			// 描述
	uchar	bType;			// 接口类型(1-道具、2-数值、3-脚本)
	string	strName;		//	bType 1-道具名称、2-属性名、3-空
	ulong	dwValue;		//	bType 1-道具数量、2-属性值、3-脚本ID

	tagBounsDes()
	{
		dwBounsID	= 0;
		strTitle	= "";
		strDes		= "";
		bType		= 0;
		strName		= "";
		dwValue		= 0;
	}

	void EncodeToDataBlock(DBWriteSet& setWriteDB);
	void DecodeFromDataBlock(DBReadSet& setReadDB);
	
};

// 奖励信息
struct tagBounsInfo
{
	CGUID	guid;				// 奖励策略编号
	string	strAccount;			// 领奖玩家账号
	uchar	bStatus;			// 领取状态(1-未领取、2-冻结、3-已领取、4-过期) 
	ulong	dwBounsID;			// 奖励资源ID
	time_t	tValidTime;			// 有效期

	tagBounsInfo()
	{
		strAccount	= "";
		bStatus		= 0;
		dwBounsID	= 0;
	}

	void EncodeToDataBlock(DBWriteSet& setWriteDB);
	void DecodeFromDataBlock(DBReadSet& setReadDB);
};

