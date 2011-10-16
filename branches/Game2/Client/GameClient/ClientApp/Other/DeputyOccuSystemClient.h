//		文件名:		CDeputyOccuSystemClient.h
//		作者:		肖杨
//		时间:		2008/2/3
//		简介:		副职业系统客户端数据
#pragma once

class CCollectionLayout;
class CFacture;
class CFactureStudy;
#include "../../../Public/Common/DeputyOccu.h"
class CDeputyOccuSystemClient : public CDeputyOccuSystem
{
public:
		//初始化
		static		 bool				InitOccuSystemClient(const char* filename);
		static		 BOOL				InitCollectionListClient();
		static		 BOOL				InitFactureStudyListClient();

		//采集接口
		//根据传入的名字获取某个采集物配置信息
		static const CCollectionLayout* GetCollectionLayout(const char* Name);
		
		//根据传入的采集物ID获取某个采集物配置信息
		static const CCollectionLayout* GetCollectionLayout(DWORD dwCollectionGoodsID);
		
		//根据传入的人物与采集物的采集等级差获得对应的采集经验
		static		 LONG				GetCollectionExp(LONG level);
		
		//根据传入的人物与采集物的采集等级差获得显示采集物名字的颜色值
		static		 DWORD				GetCollectionColor(DWORD level);

		//根据传入的人物的采集等级获取当前等级升级的经验值
		static		 DWORD				GetCollectionNeededExp(DWORD level);
		
		//制作接口
		//根据传入的ID获取制作条目的配置信息
		static const CFacture*			GetFacture(DWORD ID);

		//根据传入的人物的制作等级与制作条目要求的等级差获取制作此物品能获取的经验值
		static		 LONG				GetFactureExp(LONG level);

		//根据传入的人物的制作等级与制作条目要求的等级差获取制作此物品能获取的声望值
		static		 LONG				GetFactureCredit(LONG level);

		//根据传入的人物的制作等级与制作条目要求的等级差获取显示此制作条目的颜色值
		static		 DWORD				GetFactureColor(DWORD level);

		//根据传入的人物的制作等级获取当前升级需要多少经验值（玩家每次升级后经验值都要归零）
		static		 DWORD				GetFactureNeededExp(DWORD level);

		//根据传入的NPC名字获取此NPC处可以学到的制作条目列表
		static const CFactureStudy*		GetFactureStudy(const char* NpcName);

		//根据传入的物品类型ID获取类型的名字
		static const string*			GetFactureTypeName(DWORD FactureType);

		//采集物在场景中的特效显示刷新
		static		 void              UpdateCollectionEffectByQuest(DWORD dwQuestID,bool bAddOrDel); 
	
		//采集物的条件满足与否影响特效的显示
		//static		 void				UpdateCollectionEffectByCondition();
private:
		//采集列表(客户端使用)
		static map<DWORD,CCollectionLayout*> m_mapCollectionListClient;
};
