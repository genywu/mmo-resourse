#pragma once
#include "..\..\PUBLIC\BaseDef.h"
#include "..\..\PUBLIC\statedef.h"
class CGateList
{
public:
	CGateList();
	~CGateList();
public:
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
		BYTE		byBlockType;				//阻挡类型
		long		lDef;						//物理防御
		long		lMagicDef;					//魔法防御
		WORD		wNameColor;					//名字颜色
		char		cHitSound[MAX_PATH];		//击中音效
		char		cDestorySound[MAX_PATH];	//损毁音效
		char		cOpenSound[MAX_PATH];		//开门音效
		char		cCloseSound[MAX_PATH];		//关门音效
	};
private:
	static map<string, tagGate>m_mapGateList;
public:
	static BOOL LoadGateList(const char* filename);
	static BOOL AddToByteArray(vector<BYTE>* pByteArray);
	static BOOL DecordFromByteArray(BYTE* pByte, long& pos);
	static tagGate*	GetPropertyByOrigName(const char* strName);
};