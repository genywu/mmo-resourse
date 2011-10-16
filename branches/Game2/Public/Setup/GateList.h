//======================================================================================
/**
*	@file		GateList.h
*	@data		
*	@author		
*/
//======================================================================================

#pragma once

/**
*	@class GateList
*
*	@brief Gate配置类
*/

class GateList
{
public:
	/// 构造
	GateList();

	/// 析构
	~GateList();

public:
	/// Gate结构
	struct tagGate
	{
		char		cOrigName[50];				//原始名
		char		cName[50];					//名字
		WORD		wLenth;				    	//长度
		WORD		wWidth;						//宽度
		WORD		wLevel;						//等级
		DWORD		dwModelId;					//模型ID
		WORD		wMaxHp;						//最大HP
		bool		bIsunbeatable;				//是否是无敌状态
		uchar		byBlockType;				//阻挡类型
		WORD		wDef;						//物理防御
		WORD		wMagicDef;					//魔法防御
		WORD		wNameColor;					//名字颜色
		char		cHitSound[MAX_PATH];		//击中音效
		char		cDestorySound[MAX_PATH];	//损毁音效
		char		cOpenSound[MAX_PATH];		//开门音效
		char		cCloseSound[MAX_PATH];		//关门音效
	};

private:
	/// Gate列表
	static map<string, tagGate>m_mapGateList;

public:
	/// 从文件加载Gate列表
	static bool LoadGateList(const char* filename);

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);

	/// 解码
	static bool DecordFromByteArray(uchar* pByte, long& pos);

	/// 根据原始名取得Gate属性
	static tagGate*	GetPropertyByOrigName(const char* strName);
};