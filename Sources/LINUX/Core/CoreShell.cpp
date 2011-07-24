/*****************************************************************************************
//	外界访问Core接口方法
//	Copyright : Kingsoft 2002
//	Author	:   
//	CreateTime:	2002-9-12
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KCore.h"
#include "GameDataDef.h"
#include "CoreShell.h"
#ifndef _SERVER
#include "CoreDrawGameObj.h"
#include "ImgRef.h"
#endif
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KItemList.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"
#ifndef _SERVER
#include "KNpcResList.h"
#include "Scene/KScenePlaceC.h"
#include "kskills.h"
#include "GameDataDef.h"
#include "MsgGenreDef.h"
#include "KOption.h"
#include "KSubWorld.h"
#endif
#include "KBuySell.h"

#define	NPC_TRADE_BOX_WIDTH		6
#define	NPC_TRADE_BOX_HEIGHT	10
#define	MAX_TRADE_ITEM_WIDTH	2
#define	MAX_TRADE_ITEM_HEIGHT	4
IClientCallback* l_pDataChangedNotifyFunc = 0;

class KCoreShell : public iCoreShell
{
public:
	int	 GetProtocolSize(BYTE byProtocol);
	int	 Debug(unsigned int uDataId, unsigned int uParam, int nParam);
	int	 OperationRequest(unsigned int uOper, unsigned int uParam, int nParam);
	void ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam);
	int	 FindSelectPlayer(int x, int y, void* pReturn);
	int FindSpecialPlayer(char* Name, void* pReturn);
	int ChatSpecialPlayer(void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength);
	int	 GetGameData(unsigned int uDataId, unsigned int uParam, int nParam);
	int  FreeGameData(unsigned int uDataId, unsigned int uParam, int nParam);
	void DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam);
	void DrawGameSpace();
	DWORD GetPing();
	void SendPing();
	int	 SetCallDataChangedNofify(IClientCallback* pNotifyFunc);
	void NetMsgCallbackFunc(void* pMsgData);
	void SetRepresentShell(struct iRepresentShell* pRepresent);
	void SetMusicInterface(void* pMusicInterface);
	void SetRepresentAreaSize(int nWidth, int nHeight);
	int  Breathe();
	void Release();	//释放接口对象
	void SetClient(LPVOID pClient);
	void SendNewDataToServer(void* pData, int nLength);
};

static KCoreShell	g_CoreShell;

CORE_API void g_InitCore();
#ifndef _STANDALONE
extern "C" __declspec(dllexport)
#endif
iCoreShell* CoreGetShell()
{
	g_InitCore();
	return &g_CoreShell;
}

//--------------------------------------------------------------------------
//	功能：发出游戏世界数据改变的通知函数
//	返回：如未被注册通知函数，则直接返回0，否则返回通知函数执行结果。
//--------------------------------------------------------------------------
void CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam)
{
	if (l_pDataChangedNotifyFunc)
		l_pDataChangedNotifyFunc->CoreDataChanged(uDataId, uParam, nParam);
}


void KCoreShell::Release()
{
	g_ReleaseCore();
}

//--------------------------------------------------------------------------
//	功能：接受与分派处理网络消息
//--------------------------------------------------------------------------
void KCoreShell::NetMsgCallbackFunc(void* pMsgData)
{
#ifndef _SERVER
	g_ProtocolProcess.ProcessNetMsg((BYTE *)pMsgData);
#endif
}
//--------------------------------------------------------------------------
//	功能：设置游戏世界数据改变的通知函数
//	参数：fnCoreDataChangedCallback pNotifyFunc --> 通知函数的指针。
//	返回：返回值为非0值表示注册成功，否则表示失败。
//--------------------------------------------------------------------------
int	KCoreShell::SetCallDataChangedNofify(IClientCallback* pNotifyFunc)
{
	l_pDataChangedNotifyFunc = pNotifyFunc;
	return true;
}

//--------------------------------------------------------------------------
//	功能：从游戏世界获取数据
//	参数：unsigned int uDataId --> 表示获取游戏数据的数据项内容索引，其值为梅举类型
//							GAMEDATA_INDEX的取值之一。
//		  unsigned int uParam  --> 依据uDataId的取值情况而定
//		  int nParam --> 依据uDataId的取值情况而定
//	返回：依据uDataId的取值情况而定。
//--------------------------------------------------------------------------
int	KCoreShell::GetGameData(unsigned int uDataId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uDataId)
	{
	case GDI_PLAYER_IS_MALE:
		nRet = (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID);
		break;
	case GDI_TRADE_ITEM_PRICE:
		if (uParam)
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			KItem*	pItem = NULL;

			switch(pObj->Obj.uGenre)
			{
			case CGOG_ITEM:
				{
					if (pObj->Obj.uId > 0)
					{
						pItem = &Item[pObj->Obj.uId];
					}
				}
				break;
			case CGOG_NPCSELLITEM:
				{
					int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
					if (nBuyIdx != -1)
					{
						int nIndex = BuySell.GetItemIndex(nBuyIdx, pObj->Obj.uId);
						if (nIndex >= 0)
							pItem = BuySell.GetItem(nIndex);
					}
				}
				break;
			default:
				break;
			}

			if (!pItem)
				break;

			KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;
		
			if (pObj->eContainer == UOC_NPC_SHOP)
				pInfo->nPrice = pItem->GetPrice();
			else
				pInfo->nPrice = pItem->GetPrice() / BUY_SELL_SCALE;
			strcpy(pInfo->szItemName, pItem->GetName());
			nRet = (pItem->GetGenre() != item_task);
		}
		else
		{
			nParam = 0;
			nRet = 0;
		}
		break;
	//游戏对象描述说明文本串
	//uParam = (KUiGameObject*) 描述游戏对象的结构数据的指针
	//nParam = (char*) 指向一个缓冲区的指针，其空间不少于256字节。
	case GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO:
#ifndef _SERVER
		if (nParam && uParam)
		{
			KUiObjAtContRegion* pObj = (KUiObjAtContRegion *)uParam;
			char* pszDescript = (char *)nParam;
			pszDescript[0] = 0;
			switch(pObj->Obj.uGenre)
			{
			case CGOG_ITEM:
				{
					if (pObj->eContainer == UOC_EQUIPTMENT)
					{
						int nActive = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetActiveAttribNum(pObj->Obj.uId);
						Item[pObj->Obj.uId].GetDesc(pszDescript, true, BUY_SELL_SCALE, nActive);
					}
					else
						Item[pObj->Obj.uId].GetDesc(pszDescript, true, BUY_SELL_SCALE);
				}
				break;			
			case CGOG_NPCSELLITEM:
				{
					int nIdx = -1;
					if (-1 == Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx)
						break;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObj->Obj.uId);

					KItem* pItem = NULL;
					if (nIdx < 0)
						break;
					pItem = BuySell.GetItem(nIdx);

					if (!pItem)
						break;
					pItem->GetDesc(pszDescript, true);
				}
				break;
			}
		}
#endif
		break;
	case GDI_GAME_OBJ_DESC:
#ifndef _SERVER
		if (nParam && uParam)
		{
			KUiObjAtContRegion* pObj = (KUiObjAtContRegion *)uParam;
			char* pszDescript = (char *)nParam;
			pszDescript[0] = 0;
			switch(pObj->Obj.uGenre)
			{
			case CGOG_ITEM:
				{
					if (pObj->eContainer == UOC_EQUIPTMENT)
					{
						int nActive = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetActiveAttribNum(pObj->Obj.uId);
						Item[pObj->Obj.uId].GetDesc(pszDescript, false, 1, nActive);
					}
					else
						Item[pObj->Obj.uId].GetDesc(pszDescript);
				}
				break;
			case CGOG_SKILL:
			case CGOG_SKILL_FIGHT:
			case CGOG_SKILL_LIVE:
			case CGOG_SKILL_SHORTCUT:
				{
					int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(pObj->Obj.uId);
					_ASSERT(nLevel >= 0);
					if (pObj->Obj.uId >0)
					{
						
						KSkill::GetDesc(
							pObj->Obj.uId,
							nLevel,
							pszDescript,
							Player[CLIENT_PLAYER_INDEX].m_nIndex,
							(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true
							);
					}
					
				}
				break;
			
			case CGOG_PLAYER_FACE:
				break;
			case CGOG_NPCSELLITEM:
				{
					int nIdx = -1;
					if (-1 == Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx)
						break;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObj->Obj.uId);

					KItem* pItem = NULL;
					if (nIdx < 0)
						break;
					pItem = BuySell.GetItem(nIdx);

					if (!pItem)
						break;
					pItem->GetDesc(pszDescript);
				}
				break;
			}
		}
		break;
#endif
	//主角的一些不易变的数据
	//uParam = (KUiPlayerBaseInfo*)pInfo	
	case GDI_PLAYER_BASE_INFO:
#ifndef _SERVER
		if (uParam)
		{
			KUiPlayerBaseInfo* pInfo = (KUiPlayerBaseInfo*)uParam;
			strcpy(pInfo->Name,Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
			//to do:no implements in this version
			pInfo->Agname[0] = 0;
			pInfo->Title[0] = 0;
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_btRankId)
			{
				char szRankId[5];
				itoa(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_btRankId, szRankId, 10);
				g_RankTabSetting.GetString(szRankId, "RANKSTR", "", pInfo->Title, 32);
			}
			pInfo->nCurFaction = Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction;
		}
#endif
		break;

	//主角的一些易变的数据
	//uParam = (KUiPlayerRuntimeInfo*)pInfo
	case GDI_PLAYER_RT_INFO:
		if (uParam)
		{
			KUiPlayerRuntimeInfo* pInfo = (KUiPlayerRuntimeInfo*)uParam;
			pInfo->nLifeFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax;			//生命满值
			pInfo->nLife = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife;					//生命
			pInfo->nManaFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax;			//内力满值
			pInfo->nMana = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana;					//内力
			pInfo->nStaminaFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStaminaMax;		//体力满值
			pInfo->nStamina = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina;				//体力

			pInfo->nAngryFull = 0;													//怒满值
			pInfo->nAngry = 0;														//怒
			
			pInfo->nExperienceFull = Player[CLIENT_PLAYER_INDEX].m_dwNextLevelExp;					//经验满值
			pInfo->nExperience = Player[CLIENT_PLAYER_INDEX].m_dwExp;									//经验
			pInfo->nCurLevelExperience = Player[CLIENT_PLAYER_INDEX].m_dwNextLevelExp - PlayerSet.m_cLevelAdd.GetLevelExp(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level - 1);
			
			pInfo->byActionDisable = 0;
			//to do	给pInfo->bActionDisable、赋予合适的值

			pInfo->byAction = PA_NONE;
#ifndef _SERVER
			if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
				pInfo->byAction |= PA_RUN;
#endif
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
				pInfo->byAction |= PA_SIT;
			pInfo->wReserved = 0;
		}
		break;

	//主角的一些易变的属性数据
	//uParam = (KUiPlayerAttribute*)pInfo
	case GDI_PLAYER_RT_ATTRIBUTE:
#ifndef _SERVER
		if (uParam)
		{
			KUiPlayerAttribute* pInfo = (KUiPlayerAttribute*)uParam;
			KNpc*	pNpc = &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex];
			pInfo->nMoney = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment);				//银两
			pInfo->nBARemainPoint = Player[CLIENT_PLAYER_INDEX].m_nAttributePoint;						//基本属性剩余点数
			pInfo->nStrength = Player[CLIENT_PLAYER_INDEX].m_nCurStrength;								//力量
			pInfo->nDexterity = Player[CLIENT_PLAYER_INDEX].m_nCurDexterity;								//敏捷
			pInfo->nVitality = Player[CLIENT_PLAYER_INDEX].m_nCurVitality;								//活力
			pInfo->nEnergy = Player[CLIENT_PLAYER_INDEX].m_nCurEngergy;									//精力

			Player[CLIENT_PLAYER_INDEX].GetEchoDamage(&pInfo->nKillMIN, &pInfo->nKillMAX, 0);				//最大最小杀伤力
			Player[CLIENT_PLAYER_INDEX].GetEchoDamage(&pInfo->nRightKillMin , &pInfo->nRightKillMax, 1);
			pInfo->nAttack = pNpc->m_CurrentAttackRating;				//攻击力
			pInfo->nDefence = pNpc->m_CurrentDefend;					//防御力
			pInfo->nMoveSpeed = pNpc->m_CurrentRunSpeed;				//移动速度
			pInfo->nAttackSpeed = pNpc->m_CurrentAttackSpeed;			//攻击速度
			//物理防御
			if (pNpc->m_CurrentPhysicsResistMax >= pNpc->m_CurrentPhysicsResist)
				pInfo->nPhyDef = pNpc->m_CurrentPhysicsResist;
			else
				pInfo->nPhyDef = pNpc->m_CurrentPhysicsResistMax;
			//冰冻防御
			if (pNpc->m_CurrentColdResistMax >= pNpc->m_CurrentColdResist)
				pInfo->nCoolDef = pNpc->m_CurrentColdResist;
			else
				pInfo->nCoolDef = pNpc->m_CurrentColdResistMax;
			//闪电防御
			if (pNpc->m_CurrentLightResistMax >= pNpc->m_CurrentLightResist)
				pInfo->nLightDef = pNpc->m_CurrentLightResist;
			else
				pInfo->nLightDef = pNpc->m_CurrentLightResistMax;
			//火焰防御
			if (pNpc->m_CurrentFireResistMax >= pNpc->m_CurrentFireResist)
				pInfo->nFireDef = pNpc->m_CurrentFireResist;
			else
				pInfo->nFireDef = pNpc->m_CurrentFireResistMax;
			//毒素防御
			if (pNpc->m_CurrentPoisonResistMax >= pNpc->m_CurrentPoisonResist)
				pInfo->nPoisonDef = pNpc->m_CurrentPoisonResist;
			else
				pInfo->nPoisonDef = pNpc->m_CurrentPoisonResistMax;
			
			pInfo->nLevel = pNpc->m_Level;

			// 根据玩家状态显示 状态文字 not end 不应该是状态，应该是五行属性 spe
			memset(pInfo->StatusDesc, 0, sizeof(pInfo->StatusDesc));
			switch(pNpc->m_Series)
			{
			case series_water:
				strcpy(pInfo->StatusDesc, "水系");
				break;
			case series_wood:
				strcpy(pInfo->StatusDesc, "木系");
				break;
			case series_metal:
				strcpy(pInfo->StatusDesc, "金系");
				break;
			case series_fire:
				strcpy(pInfo->StatusDesc, "火系");
				break;
			case series_earth:
				strcpy(pInfo->StatusDesc, "土系");
				break;
			}
		}
		break;

	//主角的立即使用物品与武功
	//uParam = (KUiPlayerImmedItemSkill*)pInfo
#endif
	case GDI_PLAYER_IMMED_ITEMSKILL:
#ifndef _SERVER
		if (uParam)
		{
			KUiPlayerImmedItemSkill* pInfo = (KUiPlayerImmedItemSkill*)uParam;
			memset(pInfo,0,sizeof(KUiPlayerImmedItemSkill));
			pInfo->IMmediaSkill[0].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[0].uId		= Player[CLIENT_PLAYER_INDEX].GetLeftSkill();
			pInfo->IMmediaSkill[1].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[1].uId		= Player[CLIENT_PLAYER_INDEX].GetRightSkill();

			for (int i = 0; i < MAX_IMMEDIACY_ITEM; i++)
			{
				pInfo->ImmediaItem[i].uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[room_immediacy].FindItem(i, 0);
				if (pInfo->ImmediaItem[i].uId > 0)
				{
					pInfo->ImmediaItem[i].uGenre = CGOG_ITEM;
				}
				else
				{
					pInfo->ImmediaItem[i].uGenre = CGOG_NOTHING;
				}
			}
		}
		break;

	//主角随身携带的钱
	//nRet = 主角随身携带的钱
#endif
	case GDI_PLAYER_HOLD_MONEY:	
		nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment);
		break;
	//主角随身携带的物品等
	//uParam = (KUiObjAtRegion*) pInfo -> KUiObjAtRegion结构数组的指针，KUiObjAtRegion
	//				结构用于存储物品或者武功的数据及其放置区域位置信息。
	//nParam = pInfo数组中包含KUiObjAtRegion结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pInfo数组中的前多少个KUiObjAtRegion
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiObjAtRegion结构的数组
	//			才够存储全部的随身携带的物品信息。

	// flying添加了一个是否可以骑马的请求，在这里实现，gg可乐一罐，mm请吃KFC哦
	case GDI_PLAYER_CAN_RIDE:
		// 实现分支的代码在这里添加
		break;

	case GDI_ITEM_TAKEN_WITH:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom)
					nCount++;
			}
			nRet = nCount;
		}
		break;

	//主角装备物品
	//uParam = (KUiObjAtRegion*)pInfo -> 包含10个元素的KUiObjAtRegion结构数组指针，
	//			KUiObjAtRegion结构用于存储装备的数据和放置位置信息。
	//			KUiObjAtRegion::Region::h 表示属于第几套装备
	//			KUiObjAtRegion::Region::v 表示属于哪个位置的装备,其值为梅举类型
	//			UI_EQUIPMENT_POSITION的取值之一。请参看UI_EQUIPMENT_POSITION的注释。
	//nParam =	要获取的是第几套装备信息
	//Return =  其值表示pInfo数组中的前多少个KUiObjAtRegion结构被填充了有效的数据。
	case GDI_EQUIPMENT:
		nRet = 0;
		if (uParam)
		{
			// TODO：暂时没有做第二套装备
			if (nParam == 1)
				break;

			int PartConvert[itempart_num] = 
			{
				UIEP_HEAD,		UIEP_BODY,
				UIEP_WAIST,		UIEP_HAND,
				UIEP_FOOT,		UIEP_FINESSE,
				UIEP_NECK,		UIEP_FINGER1,
				UIEP_FINGER2,	UIEP_WAIST_DECOR,
				UIEP_HORSE,
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < itempart_num; i++)
			{
				pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(i);
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
				
					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = 0;
					pInfo->Region.v = PartConvert[i];
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;

	//交易操作相关的数据
	//uParam = (UI_TRADE_OPER_DATA)eOper 具体含义见UI_TRADE_OPER_DATA
	//nParam 具体应用与含义由uParam的取值状况决定,见UI_TRADE_OPER_DATA的说明
	//Return 具体含义由uParam的取值状况决定,见UI_TRADE_OPER_DATA的说明
	//主角的生活技能数据
	case GDI_TRADE_OPER_DATA:
#ifndef _SERVER
		if (uParam == UTOD_IS_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock;
		else if (uParam == UTOD_IS_TRADING)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState;
		else if (uParam == UTOD_IS_OTHER_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock;
		else if (uParam == UTOD_IS_WILLING)
		{
			nRet = (Player[CLIENT_PLAYER_INDEX].m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADEOPEN);
		}
#endif
		break;

	//uParam = (KUiPlayerLiveSkillBase*) pInfo -> 主角的生活技能数据
	case GDI_LIVE_SKILL_BASE:
		if (uParam)
		{
			KUiPlayerLiveSkillBase* pInfo = (KUiPlayerLiveSkillBase*)uParam;
			//to do:no implements in this version;
			pInfo->nLiveExperience = 0 ;
			pInfo->nRemainPoint = 0 ;
			pInfo->nLiveExperienceFull = 0 ;
			memset(pInfo,0,sizeof(KUiPlayerLiveSkillBase));
		}
		break;

	//主角掌握的各项生活技能
	//uParam = (unsigned int*) pSkills -> 包含10个unsigned int的数组用于存储各项生活技能的id。
	case GDI_LIVE_SKILLS:
		if (uParam)
		{
			//to do:no implements in this version;
			KUiSkillData* pInfo = (KUiSkillData*)uParam;
			memset(pInfo,0,sizeof(KUiSkillData)*10);
		}
		break;

	//剩余战斗技能点数
	//Return = 剩余战斗技能点数
	case GDI_FIGHT_SKILL_POINT:
		//to do:no implements in this version;
		nRet = Player[CLIENT_PLAYER_INDEX].m_nSkillPoint;
		break;

	//主角掌握的各项战斗技能
	//uParam = (unsigned int*) pSkills -> 包含30个unsigned int的数组用于存储各项战斗技能的id。
	case GDI_FIGHT_SKILLS:
#ifndef _SERVER
		if (uParam)
		{
			KUiSkillData* pSkills = (KUiSkillData*)uParam;
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetSkillSortList(pSkills);
		}
#endif
		break;
	//显示左键技能列表
	//uParam = (KUiSkillData*) pSkills -> 包含65个KUiSkillData的数组用于存储各技能的数据。
	//								KUiSkillData::nLevel用来表述技能显示在第几行
	//Return = 返回有效数据的Skills的数目
	case GDI_LEFT_ENABLE_SKILLS:
#ifndef _SERVER
		{
			KUiSkillData * pSkills = (KUiSkillData*)uParam;
			int nCount = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLeftSkillSortList(pSkills);
			return nCount;

		}
#endif
		break;
	//显示右键技能列表
	//uParam = (KUiSkillData*) pSkills -> 包含65个KUiSkillData的数组用于存储各技能的数据。
	//								KUiSkillData::nLevel用来表述技能显示在第几行
	//Return = 返回有效数据的Skills的数目
	case GDI_RIGHT_ENABLE_SKILLS:
#ifndef _SERVER
		{
			KUiSkillData * pSkills = (KUiSkillData*)uParam;
			int nCount = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetRightSkillSortList(pSkills);
			return nCount;
		}
#endif
		break;
	//主角的自创武功
	//uParam = (unsigned int*) pSkills -> 包含5个unsigned int的数组用于存储各项自创武功的id。
	case GDI_CUSTOM_SKILLS:
		if (uParam)
		{
			//to do: no implements in this version;
			KUiSkillData* pSkills = (KUiSkillData*)uParam;
			memset(pSkills,0,sizeof(KUiSkillData)*5);
		}
		break;

	//主角所在的队伍信息
	//uParam = (KUiPlayerTeam*)pTeam -> 队伍信息
	//nParam = 队长id,如果此值为0表示获取的是自己所在的队伍
	//Return = pTeam结构是否被填充信息。
	case GDI_TEAM_INFO:
#ifndef _SERVER
		if (uParam)
		{
			//to do: waiting for... ;
			KUiPlayerTeam* pTeam = (KUiPlayerTeam*)uParam;
			if (nParam == 0)
				nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetInfo(pTeam);
			else
				;
		}
#endif
		break;

	//获取主角所在队伍成员列表
	//uParam = (KUiPlayerItem*)pList -> 人员信息列表
	//nParam = pList数组中包含KUiPlayerItem结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pList数组中的前多少个KUiPlayerItem
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiPlayerItem结构的数组
	//			才够存储全部的成员信息。
	case GDI_TEAM_MEMBER_LIST:
#ifndef _SERVER
		nRet = g_Team[0].GetMemberInfo((KUiPlayerItem *)uParam, nParam);
#endif
		break;

	case GDI_TEAM_REFUSE_INVITE_STATUS:
#ifndef _SERVER
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetAutoRefuseState();
#endif
		break;

	//想加入（玩家所在家）队伍的其它玩家的列表
	//uParam = (KUiPlayerItem*)pPlayerList -> KUiPlayerItem结构数组的指针，KUiPlayerItem
	//				结构用于预加入的其它玩家的信息。
	//nParam = pPlayerList数组中包含KUiPlayerItem结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pPlayerList数组中的前多少个元素被填
	//			充了有效的数据；否则表示需要传入包含多少个元素的数组才够存储全部的信息。
	case GDI_TEAM_TOJOIN_LIST:
#ifndef _SERVER
		if (uParam)
		{
			int nCount = 0;
			for(int i = 0; i < MAX_TEAM_APPLY_LIST; i++)
			{
				if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_sApplyList[i].m_dwNpcID > 0)
					nCount++;				
			}

			if (nParam >= nCount)
			{
				KUiPlayerItem* pPlayerList = (KUiPlayerItem*)uParam;
				int nPos = 0; 
				for(int i = 0;i < MAX_TEAM_APPLY_LIST;i++)
				{
					 if	(Player[CLIENT_PLAYER_INDEX].m_cTeam.m_sApplyList[i].m_dwNpcID > 0)
					 {
						 pPlayerList[nPos].uId = Player[CLIENT_PLAYER_INDEX].m_cTeam.m_sApplyList[i].m_dwNpcID;						 
						 pPlayerList[nPos].nIndex = 0;
						 strcpy(pPlayerList[nPos].Name,Player[CLIENT_PLAYER_INDEX].m_cTeam.m_sApplyList[i].m_szName);
						 nPos++;
					 }
				}
			}	
			nRet = nCount ;
		}
#endif
		break;

	//获取周围玩家的列表
	//uParam = (KUiPlayerItem*)pList -> 人员信息列表
	//			KUiPlayerItem::nData = 0
	//nParam = pList数组中包含KUiPlayerItem结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pList数组中的前多少个KUiPlayerItem
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiPlayerItem结构的数组
	//			才够存储全部人员信息。
	case GDI_NEARBY_PLAYER_LIST:
#ifndef _SERVER
		nRet = NpcSet.GetAroundNotFriendPlayer((KUiPlayerItem*)uParam, nParam);
#endif
		break;

	//获取周围孤单可受邀请的玩家的列表
	//参数含义同GDI_NEARBY_PLAYER_LIST
	case GDI_NEARBY_IDLE_PLAYER_LIST:
#ifndef _SERVER
		nRet = NpcSet.GetAroundPlayerForTeamInvite((KUiPlayerItem*)uParam, nParam);
#endif
		break;

	//主角统帅能力相关的数据
	//uParam = (KUiPlayerLeaderShip*) -> 主角统帅能力相关的数据结构指针
	case GDI_PLAYER_LEADERSHIP:
#ifndef _SERVER
		if (uParam)
		{
			KUiPlayerLeaderShip* pInfo = (KUiPlayerLeaderShip*)uParam ;
			pInfo->nLeaderShipExperience = Player[CLIENT_PLAYER_INDEX].m_dwLeadExp ;		//统帅力经验值
			//to do: waiting for...;
			pInfo->nLeaderShipExperienceFull = Player[CLIENT_PLAYER_INDEX].m_dwNextLevelExp;	//统帅力经验值
			pInfo->nLeaderShipLevel = Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel ;			//统帅力等级
		}
#endif
		break;
	case GDI_NPC_RES:
#ifndef _SERVER
		if (uParam)
		{
			RoleBaseInfo* pInfo = (RoleBaseInfo *)uParam;
			KNpcRes*	pRes = NULL;
			
			pRes = new KNpcRes;
			if (pRes)
			{
				char szName[32];
				if (pInfo->Sex)
					strcpy(szName, "女主角");
				else
					strcpy(szName, "男主角");
				pRes->Init(szName, &g_NpcResList);
				pRes->SetAction(cdo_fightstand);
//				pRes->SetArmor(pInfo->ArmorType);
//				pRes->SetHelm(pInfo->HelmType);
//				pRes->SetWeapon(pInfo->WeaponType);
				pRes->SetRideHorse(FALSE);
			}
			nRet = (int)pRes;
		}
#endif
		break;
	//获得物品在某个环境位置的属性状态
	//uParam = (KUiGameObject*)pObj（当nParam==0时）物品的信息
	//uParam = (KUiObjAtContRegion*)pObj（当nParam!=0时）物品的信息
	//nParam = (int)(bool)bJustTry  是否只是尝试放置
	//Return = (ITEM_IN_ENVIRO_PROP)eProp 物品的属性状态
	case GDI_ITEM_IN_ENVIRO_PROP:
#ifndef _SERVER
		{
			if (!nParam)
			{
				KUiGameObject *pObj = (KUiGameObject *)uParam;
				if (pObj->uGenre != CGOG_ITEM && pObj->uGenre != CGOG_NPCSELLITEM)
					break;

				KItem* pItem = NULL;

				if (pObj->uGenre == CGOG_ITEM && pObj->uId > 0 && pObj->uId < MAX_ITEM)
				{
					pItem = &Item[pObj->uId];
				}
				else if (pObj->uGenre == CGOG_NPCSELLITEM)
				{
					int nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObj->uId);
					pItem = BuySell.GetItem(nIdx);
				}

				_ASSERT(pItem);
				if (!pItem || pItem->GetGenre() != item_equip)
					break;

				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pItem))
				{
					nRet = IIEP_NORMAL;
				}
				else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
			else
			{
				KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
				if (pObj->Obj.uGenre != CGOG_ITEM || pObj->Obj.uId >= MAX_ITEM)
					break;

				int PartConvert[itempart_num] = 
				{ 
					itempart_head,		itempart_weapon,
					itempart_amulet,	itempart_cuff,
					itempart_body,		itempart_belt,
					itempart_ring1,		itempart_ring2,
					itempart_pendant,	itempart_foot,
					itempart_horse,
				};

				_ASSERT(pObj->eContainer < itempart_num);
				if (pObj->eContainer >= itempart_num || pObj->eContainer < 0)
					break;

				if (Item[pObj->Obj.uId].GetGenre() != item_equip)
					break;

				int nPlace = PartConvert[pObj->eContainer];

				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pObj->Obj.uId, nPlace))
				{
					nRet = IIEP_NORMAL;
				}
				else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
		}
#endif
		break;
	//与NPC的买卖
	//uParam = (KUiObjAtContRegion*) pInfo -> KUiObjAtRegion结构数组的指针，KUiObjAtRegion
	//				结构用于存储物品的数据及其放置区域位置信息。
	//				其中KUiObjAtContRegion::nContainer值表示第几页的物品
	//nParam = pInfo数组中包含KUiObjAtRegion结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pInfo数组中的前多少个KUiObjAtRegion
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiObjAtRegion结构的数组
	//			才够存储全部的npc列出来交易的物品信息。
	case GDI_TRADE_NPC_ITEM:
#ifndef _SERVER
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			int nPage = 0;
			int nIndex = 0;
			int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
			KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;

			if (nBuyIdx == -1)
				break;
			if (nBuyIdx >= BuySell.GetHeight())
				break;
			if (!BuySell.m_pShopRoom)
				break;
			BuySell.m_pShopRoom->Clear();
			for (int i = 0; i < BuySell.GetWidth(); i++)
			{
				nIndex = BuySell.GetItemIndex(nBuyIdx, i);
				KItem* pItem = BuySell.GetItem(nIndex);
				
				if (nIndex >= 0 && pItem)
				{
					// Set pInfo->Obj.uGenre
					pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
					// Set pInfo->Obj.uId
					pInfo->Obj.uId = i;

					POINT	Pos;
					if (BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
					{
						// nIndex + 1保证不为0
						BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
					}
					else
					{
						nPage++;
						BuySell.m_pShopRoom->Clear();
						// Clear完成后必然成功，所以没有判断
						BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos);
						BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
					}
					pInfo->Region.h = Pos.x;
					pInfo->Region.v = Pos.y;
					pInfo->Region.Width = pItem->GetWidth();
					pInfo->Region.Height = pItem->GetHeight();
					pInfo->nContainer = nPage;
					nCount++;
					pInfo++;
				}
			}			
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			int nIndex = 0;
			int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
			if (nBuyIdx == -1)
				break;
			if (nBuyIdx >= BuySell.GetHeight())
				break;
			for (int i = 0; i < BuySell.GetWidth(); i++)
			{
				nIndex = BuySell.GetItemIndex(nBuyIdx, i);
				KItem* pItem = BuySell.GetItem(nIndex);
				
				if (nIndex >= 0 && pItem)
				{
					nCount++;
				}
			}
			nRet = nCount;
		}
#endif
		break;
	case GDI_CHAT_SEND_CHANNEL_LIST:
#ifndef _SERVER
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.SendSelectChannelList((KUiChatChannel*)uParam, nParam);
#endif
		break;
	case GDI_CHAT_RECEIVE_CHANNEL_LIST:
#ifndef _SERVER
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.SendTakeChannelList((KUiChatChannel*)uParam, nParam);
#endif
		break;
	case GDI_CHAT_CURRENT_SEND_CHANNEL:
#ifndef _SERVER
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.GetCurChannel((KUiChatChannel*)uParam);
#endif
		break;
	case GDI_CHAT_GROUP_INFO:
#ifndef _SERVER
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.IGetTeamInfo(nParam, (KUiChatGroupInfo*)uParam);
#endif
		break;
	//聊天一个好友分组中好友的信息
	//uParam = (KUiPlayerItem*)pList -> 人员信息列表
	//			KUiPlayerItem::nData = (CHAT_STATUS)eFriendStatus 好友的当前状态
	//nParam = 要获取列表的好友分组的索引
	//Return = 其值表示pList数组中的前多少个KUiPlayerItem结构被填充了有效的数据.
	case GDI_CHAT_FRIENDS_IN_AGROUP:
#ifndef _SERVER
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.IGetTeamFriendInfo(nParam, (KUiPlayerItem*)uParam);
#endif
		break;
	case GDI_ITEM_IN_STORE_BOX:
#ifndef _SERVER
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_repository);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// 第一个是钱
			nCount++;
			// 后面是装备
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom)
					nCount++;
			}
			nRet = nCount;
		}
#endif
		break;
	case GDI_SCENE_TIME_INFO:
#ifndef _SERVER
		if (uParam)
		{
			KUiSceneTimeInfo* pInfo = (KUiSceneTimeInfo*)uParam;
			g_ScenePlace.GetSceneNameAndFocus(pInfo->szSceneName, pInfo->nSceneId,
				pInfo->nScenePos0, pInfo->nScenePos1);
			pInfo->nGameSpaceTime = (SubWorld[0].m_dwCurrentTime / 100) % 1440;
		}
#endif
		break;
	}
	return nRet;
}

int KCoreShell::FreeGameData(unsigned int uDataId, unsigned int uParam, int nParam)
{
	int nRet = 0;

	switch(uDataId)
	{
	case GDI_NPC_RES:
#ifndef _SERVER
		delete (KNpcRes *)uParam;
#endif
		break;
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：向游戏发送操作
//	参数：unsigned int uDataId --> Core外部客户对core的操作请求的索引定义
//							其值为梅举类型GAMEOPERATION_INDEX的取值之一。
//		  unsigned int uParam  --> 依据uOperId的取值情况而定
//		  int nParam --> 依据uOperId的取值情况而定
//	返回：如果成功发送操作请求，函数返回非0值，否则返回0值。
//--------------------------------------------------------------------------
int	KCoreShell::OperationRequest(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 1;
	switch(uOper)
	{
	//uParam = (const char*)pszFileName
	case GOI_PLAY_SOUND:
#ifndef _SERVER
		if (uParam)
		{
			static KCacheNode* pSndNode = NULL;
			KWavSound* pSound = NULL;
			pSndNode	= (KCacheNode*)g_SoundCache.GetNode((char *)uParam, (KCacheNode * )pSndNode);
			pSound		= (KWavSound*)pSndNode->m_lpData;
			if (pSound)
			{
				if (pSound->IsPlaying())
					break;
				pSound->Play(0, Option.GetSndVolume(), 0);
			}
		}
#endif
		break;
	case GOI_PLAYER_RENASCENCE:
		{
#ifndef _SERVER
			int nReviveType;
			if (nParam)	// bBackTown
			{
				nReviveType = REMOTE_REVIVE_TYPE;
			}
			else
			{
				nReviveType = LOCAL_REVIVE_TYPE;
			}
//			SendClientCmdRevive(nReviveType);
			SendClientCmdRevive();
#endif
		}
		break;
	case GOI_MONEY_INOUT_STORE_BOX:
		{
#ifndef _SERVER
			BOOL	bIn = (BOOL)uParam;
			int		nMoney = nParam;
			int		nSrcRoom, nDesRoom;


			if (bIn)
			{
				nSrcRoom = room_equipment;
				nDesRoom = room_repository;
			}
			else
			{
				nDesRoom = room_equipment;
				nSrcRoom = room_repository;
			}
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(nSrcRoom, nDesRoom, nMoney);
#endif
		}
		break;
		//离开游戏
	case GOI_EXIT_GAME:
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].SaveClientSkillData();
		g_SubWorldSet.Close();
#endif
		//		if (g_pClient)
//			g_pClient->Shutdown();
		break;
	case GOI_TRADE_NPC_BUY:
		{
#ifndef _SERVER
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			if (CGOG_NPCSELLITEM != pObject1->Obj.uGenre)
					break;

			int nIdx = 0;
			KItem* pItem = NULL;

			nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObject1->Obj.uId);
			pItem = BuySell.GetItem(nIdx);

			int nWidth, nHeight;
			ItemPos	Pos;

			nWidth = pItem->GetWidth();
			nHeight = pItem->GetHeight();
			if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
			{
				nRet = 0;
				break;
			}
			if (Pos.nPlace != pos_equiproom)
			{
				nRet = 0;

				KSystemMessage	sMsg;
				
				strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}

			if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < pItem->GetPrice())
			{
				nRet = 0;
				KSystemMessage	sMsg;
				
				strcpy(sMsg.szMessage, MSG_SHOP_NO_MONEY);
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}
			SendClientCmdBuy(pObject1->Obj.uId, pos_equiproom, Pos.nX, Pos.nY);
#endif
		}
		break;
	case GOI_TRADE_NPC_SELL:
		{
#ifndef _SERVER
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
//			KUiObjAtContRegion* pObject2 = (KUiObjAtContRegion*)nParam;

			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			//放下去的东西不为空，所以是卖东西
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_TRADE_TASK_ITEM);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				SendClientCmdSell(Item[nIdx].GetID());
				return 1;
			}
			else
			{
				return 0;
			}
#endif
		}
		break;
	case GOI_SWITCH_OBJECT:
		//uParam = (KUiObjAtContRegion*)pObject1 -> 拿起的物品操作前的信息
		//nParam = (KUiObjAtContRegion*)pObject2 -> 放下的物品操作后的信息
		{
#ifndef _SERVER
			ItemPos	P1, P2;
			int PartConvert[itempart_num] = 
			{ 
				itempart_head,		itempart_weapon,
				itempart_amulet,	itempart_cuff,
				itempart_body,		itempart_belt,
				itempart_ring1,		itempart_ring2,
				itempart_pendant,	itempart_foot,
				itempart_horse,
			};
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			KUiObjAtContRegion* pObject2 = (KUiObjAtContRegion*)nParam;
			
			if (!pObject1 && !pObject2)
				break;
			
			if (pObject1)
			{
				switch(pObject1->eContainer)
				{
				case UOC_STORE_BOX:
					P1.nPlace = pos_repositoryroom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_IMMEDIA_ITEM:
					P1.nPlace = pos_immediacy;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_ITEM_TAKE_WITH:
					P1.nPlace = pos_equiproom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_EQUIPTMENT:
					{
						// TODO:暂时没有第二装备
						if (pObject1->Region.h == 1)
							break;
						P1.nPlace = pos_equip;
						P1.nX = PartConvert[pObject1->Region.v];
					}
					break;
				case UOC_TO_BE_TRADE:
					P1.nPlace = pos_traderoom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_NPC_SHOP:
					if (CGOG_NPCSELLITEM != pObject1->Obj.uGenre)
						break;

					int nIdx = 0;
					KItem* pItem = NULL;
					
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObject1->Obj.uId);
					pItem = BuySell.GetItem(nIdx);
					
					int nWidth, nHeight;
					ItemPos	Pos;
					
					nWidth = pItem->GetWidth();
					nHeight = pItem->GetHeight();
					if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					if (Pos.nPlace != pos_equiproom)
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < pItem->GetPrice())
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_MONEY);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					// 拿起来的东西不为空，所以是买东西
					SendClientCmdBuy(pObject1->Obj.uId, pos_equiproom, Pos.nX, Pos.nY);
					break;
				}
			}
			
			if (pObject2)
			{
				switch(pObject2->eContainer)
				{
				case UOC_STORE_BOX:
					P2.nPlace = pos_repositoryroom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_IMMEDIA_ITEM:
					P2.nPlace = pos_immediacy;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_ITEM_TAKE_WITH:
					P2.nPlace = pos_equiproom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_EQUIPTMENT:
					{
						// TODO:暂时没有第二装备
						if (pObject2->Region.h == 1)
							break;
						P2.nPlace = pos_equip;
						P2.nX = PartConvert[pObject2->Region.v];
					}
					break;
				case UOC_TO_BE_TRADE:
					P2.nPlace = pos_traderoom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_NPC_SHOP:
					break;
				}
			}
			if (!pObject1)
			{
				memcpy(&P1, &P2, sizeof(P1));
			}
			if (!pObject2)
			{
				memcpy(&P2, &P1, sizeof(P1));
			}
			Player[CLIENT_PLAYER_INDEX].MoveItem(P1, P2);
#endif
		}
		break;

	//玩家点完对话框
	case GOI_INFORMATION_CONFIRM_NOTIFY:
	{
#ifndef _SERVER
		PLAYER_SELECTUI_COMMAND command;
		command.nSelectIndex = 0;
		Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_TALKDIALOG);
#endif
		break;
	}
	
	//问题选择答案
	//nParma = nAnswerIndex
	case GOI_QUESTION_CHOOSE:
#ifndef _SERVER
		if (g_bUISelLastSelCount == 0 )
			break;
		
		{
			PLAYER_SELECTUI_COMMAND command;
			command.nSelectIndex = nParam;
			Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_SELECTDIALOG);
		}
#endif
		break;

	//使用物品
	//uParam = (KUiObjAtRegion*)pInfo -> 物品的数据以及物品原来摆放的位置
	//nParam = 物品使用前放置的位置，取值为枚举类型UIOBJECT_CONTAINER。
	case GOI_USE_ITEM:
		//to do: waiting for...
#ifndef _SERVER
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			int nPlace = nParam;
			ItemPos	Pos;
			switch(nPlace)
			{
			case UOC_ITEM_TAKE_WITH:
				Pos.nPlace = pos_equiproom;
				break;
			case UOC_IMMEDIA_ITEM:
				Pos.nPlace = pos_immediacy;
				break;
			default:
				Pos.nPlace = -1;
				break;
			}
			Pos.nX = pInfo->Region.h;
			Pos.nY = pInfo->Region.v;
			if (pInfo->Obj.uGenre == CGOG_ITEM && pInfo->Obj.uId > 0 && Pos.nPlace != -1)
				Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pInfo->Obj.uId, Pos);
		}
#endif
		break;

	//穿上装备
	//uParam = (KUiObjAtRegion*)pInfo -> 装备的数据和放置位置信息
	//			KUiObjAtRegion::Region::h 表示属于第几套装备
	//			KUiObjAtRegion::Region::v 表示属于哪个位置的装备,其值为梅举类型
	//			UI_EQUIPMENT_POSITION的取值之一。请参看UI_EQUIPMENT_POSITION的注释。
	case GOI_WEAR_EQUIP:
		//to do: waiting for...
#ifndef _SERVER
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
		}
#endif
		break;

	//施展武功/技能
	//uParam = (KUiGameObject*)pInfo -> 技能数据
	case GOI_USE_SKILL:	
#ifndef _SERVER
		if (uParam)
		{
			//to do:wating for...
			KUiGameObject* pInfo = (KUiGameObject*)uParam;
		}
#endif
		break;

	//设置立即技能
	//uParam = (KUiGameObject*)pSKill, 技能信息
	//nParam = 立即位置，0表示为右键技能，1至4表示为F1至F4技能
	case GOI_SET_IMMDIA_SKILL:
#ifndef _SERVER
		if (uParam)
		{
			KUiGameObject* pSkill = (KUiGameObject*)uParam;
			if (nParam == 0)
				//to do : modify;
			{
				if ( (int)pSkill->uId > 0 )
					Player[CLIENT_PLAYER_INDEX].SetLeftSkill((int)pSkill->uId);
				else if ((int)(pSkill->uId) == -1) //设置当前物理技能为左技能
				{
					int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
					int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();
					
					//近身武器
					if (nDetailType == 0)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nMeleeWeaponSkill[nParticularType]);
					}//远程武器
					else if (nDetailType == 1)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nRangeWeaponSkill[nParticularType]);
					}//空手
					else if (nDetailType == -1)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nHandSkill);
					}
					
				}
			}
			else if (nParam == 1)
			{
				if ((int)pSkill->uId > 0)
					Player[CLIENT_PLAYER_INDEX].SetRightSkill((int)pSkill->uId);
				else if ((int)(pSkill->uId) == -1)
				{
					int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
					int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();
					
					if (nDetailType == 0)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nMeleeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == 1)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nRangeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == -1)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nHandSkill);
					}
				}
			}
		}
#endif
		break;

	//增强一种技能，，一次加一点
	//uParam = 技能类属
	//nParam = (uint)技能id
	case GOI_TONE_UP_SKILL:
		//to do : waiting for...
		{
#ifndef _SERVER
			Player[CLIENT_PLAYER_INDEX].ApplyAddSkillLevel((int)nParam, 1);
#endif
		}
		break;

	//增强一些属性的值，一次加一点
	//uParam = 表示要增强的是哪个属性，取值为UI_PLAYER_ATTRIBUTE的梅举值之一
	case GOI_TONE_UP_ATTRIBUTE:
#ifndef _SERVER
		switch (uParam)
		{
		case UIPA_STRENGTH:		//力量
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(0, 1);
			break;
		case UIPA_DEXTERITY:	//敏捷
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(1, 1);
			break;		
		case UIPA_VITALITY:		//活力
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(2, 1);
			break;
		case UIPA_ENERGY:		//精力
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(3, 1);
			break;		
		}
#endif
		break;

	//===========组队相关==================
	case GOI_TEAM_COLLECT_NEARBY_LIST:
#ifndef _SERVER
		NpcSet.GetAroundOpenCaptain(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp);
#endif
		break;

	case GOI_TEAM_APPLY:
		if (uParam)
#ifndef _SERVER
			Player[CLIENT_PLAYER_INDEX].ApplyAddTeam(((KUiTeamItem*)uParam)->Leader.nIndex);
#endif
		break;

	case GOI_TRAM_REFUSE_INVITE:
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].m_cTeam.SetAutoRefuseInvite(nParam);
#endif
		break;

	//新组队伍
	//uParam = (const char*)pTeamName -> 新组队伍的名称
	case GOI_TEAM_NEW:
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].ApplyCreateTeam();//(char*)uParam);
#endif
		break;

	//任命队长
	//uParam = uNewLeaderId -> 新队长的id
	case GOI_TEAM_APPOINT:
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].ApplyTeamChangeCaptain(((KUiPlayerItem*)uParam)->uId);
#endif
		break;

	//邀请别人加入队伍
	case GOI_TEAM_INVITE:
#ifndef _SERVER
		if (uParam)
		{
			Player[CLIENT_PLAYER_INDEX].TeamInviteAdd(((KUiPlayerItem*)uParam)->uId);

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_SEND_INVITE, ((KUiPlayerItem*)uParam)->Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
#endif
		break;

	//踢除队里的一个队员
	//uParam = uMemberId -> 要踢除的队员的id
	case GOI_TEAM_KICK:
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].TeamKickMember(((KUiPlayerItem*)uParam)->uId);
#endif
		break;

	//批准他人加入队伍
	//uParam = uPlayerId -> 欲批准的人的id
	case GOI_TEAM_APPROVE:
#ifndef _SERVER
		if (uParam)
		{
			if (nParam)
			{
				Player[CLIENT_PLAYER_INDEX].AcceptTeamMember(((KUiPlayerItem*)uParam)->uId);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cTeam.DeleteOneFromApplyList(((KUiPlayerItem*)uParam)->uId);
				Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
			}
		}
#endif
		break;

	//离开队伍
#ifndef _SERVER
	case GOI_TEAM_LEAVE:
		Player[CLIENT_PLAYER_INDEX].LeaveTeam();
		break;

	// 队伍开放、关闭
	case GOI_TEAM_CLOSE:
		if (nParam)
		{
			Player[CLIENT_PLAYER_INDEX].ApplyTeamOpenClose(TRUE);
		}
		else
			Player[CLIENT_PLAYER_INDEX].ApplyTeamOpenClose(FALSE);
		break;

	//对组队邀请的回复
	//uParam = (KUiPlayerItem*)pTeamLeader 发出组队邀请的队长
	//nParam = (int)(bool)bAccept 是否接受邀请
	case GOI_TEAM_INVITE_RESPONSE:
		if (uParam)
		{
			KUiPlayerItem	*pTeamLeader = (KUiPlayerItem*)uParam;
			if (nParam)
			{
				Player[CLIENT_PLAYER_INDEX].m_cTeam.ReplyInvite(pTeamLeader->nIndex, 1);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cTeam.ReplyInvite(pTeamLeader->nIndex, 0);
			}
		}
		break;

	//答应/拒绝交易请求
	//uParam = (KUiPlayerItem*)pRequestPlayer 发出请求的玩家
	//nParam = (int)(bool)bAccept 是否接受请求
	case GOI_TRADE_INVITE_RESPONSE:
		if (uParam)
		{
			KTrade::ReplyInvite(((KUiPlayerItem*)uParam)->nIndex, nParam);
		}
		break;

	//增减一个欲卖出的物品
	//uParam = (KUiObjAtRegion*) pObject -> 物品信息，其中坐标信息为在交易界面中的坐标
	//nParam = bAdd -> 0值表示减少，1值表示增加
	//Remark : 如果物品是金钱的话，则KUiObjAtRegion::Obj::uId表示把金钱额调整为这个值，且nParam无意义。
	case GOI_TRADE_DESIRE_ITEM:
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			if (pInfo->Obj.uGenre != CGOG_MONEY)
				break;
			Player[CLIENT_PLAYER_INDEX].TradeMoveMoney(pInfo->Obj.uId);
		}
		break;

	//有无交易意向
	//nParam = bWilling
	case GOI_TRADE_WILLING:
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMenuState() == PLAYER_MENU_STATE_TRADEOPEN)
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyClose();
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyOpen((char*)uParam, nParam);
		}
		break;

	//锁定交易
	//nParam = (int)(book)bLock 是否锁定
	case GOI_TRADE_LOCK:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock)
			Player[CLIENT_PLAYER_INDEX].TradeApplyLock(0);
		else
			Player[CLIENT_PLAYER_INDEX].TradeApplyLock(1);
		break;

	//交易
	case GOI_TRADE:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock != 1 || Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock != 1)
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState == 0)
		{
			Player[CLIENT_PLAYER_INDEX].TradeDecision(1);		// 交易确定
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].TradeDecision(2);		// 交易确定取消
		}
		break;

	//交易取消
	case GOI_TRADE_CANCEL:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		Player[CLIENT_PLAYER_INDEX].TradeDecision(0);		// 交易取消
		break;

	//查询是否可以丢某个东西到游戏窗口
	//uParam = (KUiGameObject*)pObject -> 物品信息
	//nParam = 被拖动东西的当前坐标（绝对坐标），横坐标在低16位，纵坐标在高16位。(像素点坐标)
	//Return = 是否可以放下
	case GOI_DROP_ITEM_QUERY:
		//to do : waiting for...
		break;

	case GOI_SEND_MSG:
		if (uParam && nParam)
			Player[CLIENT_PLAYER_INDEX].SendChat((KUiMsgParam*)nParam, (char*)uParam);
		break;
	case GOI_SET_SEND_CHAT_CHANNEL:	//设置当前发送消息的频道
		if (uParam)
		{
			KUiChatChannel* pChannelInfo = (KUiChatChannel*)uParam;// pChannelInfo 要设置的频道的信息
			Player[CLIENT_PLAYER_INDEX].m_cChat.SetCurChannel(pChannelInfo->nChannelNo, pChannelInfo->uChannelId, pChannelInfo->nChannelIndex);
		}
		break;
	// 设置当前发送消息的频道为与某人单聊
	//uParam = (KUiPlayerItem*) pFriend
	case GOI_SET_SEND_WHISPER_CHANNEL:
		if (uParam)
		{
			if (((KUiPlayerItem*)uParam)->nIndex)
			{
				int	nIdx = NpcSet.SearchName(((KUiPlayerItem*)uParam)->Name);
				if (nIdx > 0)
				{
					Player[CLIENT_PLAYER_INDEX].m_cChat.SetCurChannel(CHAT_CUR_CHANNEL_SCREENSINGLE, Npc[nIdx].m_dwID, 0);
				}
				else
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_NOT_NEAR, ((KUiPlayerItem*)uParam)->Name);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
			}
			else
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_NOT_ONLINE, ((KUiPlayerItem*)uParam)->Name);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
		break;

	case GOI_SET_RECEIVE_CHAT_CHANNEL://设置订阅/取消消息频道
		if (uParam)
		{
			//uParam = (KUiChatChannel*) pChannelInfo 要设置的频道的信息
			//nParam = (int)(bool) bEnable 是否订阅
			KUiChatChannel* pChannelInfo = (KUiChatChannel*)uParam;// pChannelInfo 要设置的频道的信息
			if (nParam)
			{
				Player[CLIENT_PLAYER_INDEX].m_cChat.AddChannel(pChannelInfo->nChannelNo);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cChat.SubChannel(pChannelInfo->nChannelNo);
			}
		}
		break;

	//新建聊天好友组
	//uParam = (const char*) pGroupName
	//Return = (int)(bool) bSuccessed
	case GOI_CHAT_GROUP_NEW:
		Player[CLIENT_PLAYER_INDEX].m_cChat.AddTeam(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, (char*)uParam);
		CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
		break;

	//重命名聊天好友组
	//uParam = (const char*) pGroupName
	//nParam = nIndex 组索引
	//Return = (int)(bool) bSuccessed
	case GOI_CHAT_GROUP_RENAME:
		Player[CLIENT_PLAYER_INDEX].m_cChat.RenameTeam(nParam, (char*)uParam, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
		CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
		break;

	//删除聊天好友组
	//nParam = nIndex 组索引
	//Return = (int)(bool) bSuccessed
	case GOI_CHAT_GROUP_DELETE:
		Player[CLIENT_PLAYER_INDEX].m_cChat.DeleteTeam(nParam, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
		CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
		break;

	//新添加聊天好友
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0
	case GOI_CHAT_FRIEND_ADD:
		{
			Player[CLIENT_PLAYER_INDEX].m_cChat.ApplyAddFriend(((KUiPlayerItem*)uParam)->uId);

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHAT_APPLY_ADD_FRIEND, ((KUiPlayerItem*)uParam)->Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	//删除聊天好友
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0
	//nParam = nGroupIndex 好友所在的组的索引
	//Return = (int)(bool) bSuccessed
	case GOI_CHAT_FRIEND_DELETE:
		{
			Player[CLIENT_PLAYER_INDEX].m_cChat.ApplyDeleteFriend(((KUiPlayerItem*)uParam)->uId, nParam, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHAT_DELETE_FRIEND, ((KUiPlayerItem*)uParam)->Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;

	//好友移到新组
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0
	//nParam = nGroupIndex 好友所到的新组的索引
	case GOI_CHAT_FRIEND_MOVE:
		{
			KUiPlayerItem	*pPlayer = (KUiPlayerItem*)uParam;
			int		nSrcTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.GetTeamNo(pPlayer->uId);
			if (nSrcTeamNo == -1)
			{
				// 失败，如果需要发消息，在此
				break;
			}
			if (Player[CLIENT_PLAYER_INDEX].m_cChat.OneFriendChangeTeam(pPlayer->uId, nSrcTeamNo, nParam))
			{	// 成功
				CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
			}
			else
			{
				// 失败，如果需要发消息，在此
			}
		}
		break;

	//发给好友一条讯息
	//uParam = (KUiChatMessage*)pMessage 消息数据
	//nParam = (KUiPlayerItem*)pFriend   发来讯息的好友
	case GOI_CHAT_WORDS_TO_FRIEND:
		{
			char	szSentence[MAX_SENTENCE_LENGTH];
			KUiChatMessage	*pMsg = (KUiChatMessage*)uParam;
			KUiMsgParam	sMsg;

			sMsg.eGenre = MSG_G_CHAT;
			sMsg.szName[0] = 0;
			memset(sMsg.cChatPrefix, 0, sizeof(sMsg.cChatPrefix));
			sMsg.cChatPrefixLen = sizeof(unsigned int);
			sMsg.cChatPrefix[0] = ((char*)(&pMsg->uColor))[3];
			sMsg.cChatPrefix[1] = ((char*)(&pMsg->uColor))[2];
			sMsg.cChatPrefix[2] = ((char*)(&pMsg->uColor))[1];
			sMsg.cChatPrefix[3] = ((char*)(&pMsg->uColor))[0];

			if (pMsg->nContentLen >= MAX_SENTENCE_LENGTH)
			{
				memcpy(szSentence, pMsg->szContent, MAX_SENTENCE_LENGTH - 1);
				szSentence[MAX_SENTENCE_LENGTH - 1] = 0;
				sMsg.nMsgLength = MAX_SENTENCE_LENGTH - 1;
			}
			else
			{
				memcpy(szSentence, pMsg->szContent, pMsg->nContentLen);
				szSentence[pMsg->nContentLen] = 0;
				sMsg.nMsgLength = pMsg->nContentLen;
			}

			Player[CLIENT_PLAYER_INDEX].m_cChat.QQSendSentence(
				((KUiPlayerItem*)nParam)->uId,
				((KUiPlayerItem*)nParam)->nIndex, &sMsg, szSentence);
		}
		break;

	//对别人要加自己为好友的回复
	//uParam = (KUiPlayerItem*)pRequestPlayer 发出请求的玩家
	//nParam = (int)(bool)bAccept 是否接受请求
	case GOI_CHAT_FRIEND_INVITE:
		if (uParam)
		{
			if (nParam)
			{
				Player[CLIENT_PLAYER_INDEX].ChatAddFriend(((KUiPlayerItem*)uParam)->nIndex);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].ChatRefuseFriend(((KUiPlayerItem*)uParam)->nIndex);
			}
		}
		break;

	case GOI_OPTION_SETTING:			//选项设置
		if (uParam == OPTION_DYNALIGHT)
		{
			g_ScenePlace.EnableDynamicLights(nParam != 0);
			if (g_pRepresent)
				g_pRepresent->SetOption(DYNAMICLIGHT, nParam != 0);
		}
		else if (uParam == OPTION_PERSPECTIVE)
		{
			if (g_pRepresent)
				g_pRepresent->SetOption(PERSPECTIVE, nParam != 0);
		}
		else if (uParam == OPTION_MUSIC_VALUE)
			Option.SetMusicVolume(nParam);
		else if (uParam == OPTION_SOUND_VALUE)
			Option.SetSndVolume(nParam);
		else if (uParam == OPTION_BRIGHTNESS)
			Option.SetGamma(nParam);

		break;
		
	case GOI_PLAYER_ACTION:
		{
			switch(uParam)
			{
			case PA_RUN:
				Player[CLIENT_PLAYER_INDEX].m_RunStatus = !Player[CLIENT_PLAYER_INDEX].m_RunStatus;
				break;
			case PA_SIT:
				if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
				{
					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_sit);
					else
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);
					SendClientCmdSit();
				}
				else
				{
					KSystemMessage	Msg;
					Msg.byConfirmType = SMCT_CLICK;
					Msg.eType = SMT_PLAYER;
					Msg.byPriority = 1;
					Msg.byParamSize = 0;
					strcpy(Msg.szMessage, "骑马时无法打坐！");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
				}
				break;
			}
		}
		break;
	case GOI_IS_SCENE_MAP_SHOWING:
		g_ScenePlace.SetMapParam(uParam, nParam);
		break;
	case GOI_PAINT_SCENE_MAP:
		if (uParam)
		{
			nRet = g_ScenePlace.PaintMap(((KUiRegion*)uParam)->h,
				((KUiRegion*)uParam)->v, (KUiRegion*)nParam);
		}
		break;
#endif
	default:
		nRet = 0;
		break;
	}

	return nRet;
}

//--------------------------------------------------------------------------
//	功能：发送输入设备的输入操作消息
//--------------------------------------------------------------------------
void KCoreShell::ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam)
{
#ifndef _SERVER
	Player[CLIENT_PLAYER_INDEX].ProcessInputMsg(uMsg, uParam, nParam);
#endif
}

int KCoreShell::FindSelectPlayer(int x, int y, void* pReturn)
{
#ifndef _SERVER
	Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_none | relation_ally | relation_enemy);
	int nT = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
	if (nT > 0 &&
		Npc[nT].m_Kind == kind_player)
	{
		if (pReturn)
		{
			KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
			strncpy(p->Name, Npc[nT].Name, 32);
			p->nIndex = Npc[nT].m_Index;
			p->uId = Npc[nT].m_dwID;
		}
		return true;
	}
#endif
	return false;
}

int KCoreShell::FindSpecialPlayer(char* Name, void* pReturn)
{
	if (Name == NULL || Name[0] == 0)
		return false;
	for (int nT = 0; nT < MAX_NPC; nT++)
	{
		if	(Npc[nT].m_Kind == kind_player &&
			strcmp(Npc[nT].Name, Name) == 0)
		{
			if (pReturn)
			{
				KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
				strncpy(p->Name, Npc[nT].Name, 32);
				p->nIndex = Npc[nT].m_Index;
				p->uId = Npc[nT].m_dwID;
			}
			return true;
		}
	}
	return false;
}

int KCoreShell::ChatSpecialPlayer(void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC)
		{
			int nTalker = p->nIndex;
			if (Npc[nTalker].m_Kind == kind_player &&
				Npc[nTalker].m_dwID == p->uId)
			{
				memset(Npc[nTalker].m_szChatBuffer, 0, sizeof(Npc[nTalker].m_szChatBuffer));
				memcpy(Npc[nTalker].m_szChatBuffer, pMsgBuff, nMsgLength);
				Npc[nTalker].m_nCurChatTime = NPC_SHOW_CHAT_TIME_LENGTH;
				return true;
			}
		}
	}

	return false;
}

//--------------------------------------------------------------------------
//	功能：绘制游戏对象
//--------------------------------------------------------------------------
void KCoreShell::DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam)
{
#ifndef _SERVER
	if (g_pRepresent)
		CoreDrawGameObj(uObjGenre, uId, x, y, Width, Height, nParam);
#endif
}

#ifndef _SERVER
#include "../../Represent/iRepresent/iRepresentshell.h"
#endif
//--------------------------------------------------------------------------
//	功能：绘制游戏世界
//--------------------------------------------------------------------------
void KCoreShell::DrawGameSpace()
{
#ifndef _SERVER
	if (g_pRepresent)
	{
		g_ScenePlace.Paint();
		Player[CLIENT_PLAYER_INDEX].DrawSelectInfo();
	}
#endif
}

//--------------------------------------------------------------------------
//	功能：设置绘图接口实例的指针
//--------------------------------------------------------------------------
void KCoreShell::SetRepresentShell(struct iRepresentShell* pRepresent)
{
#ifndef _SERVER
	g_pRepresent = pRepresent;
	g_ScenePlace.RepresentShellReset();
#endif
}

void KCoreShell::SetMusicInterface(void* pMusicInterface)
{
#ifndef _SERVER
	g_pMusic = (KMusic*)pMusicInterface;
#endif
}

//日常活动，core如果要寿终正寝则返回0，否则返回非0值
int KCoreShell::Breathe()
{
	g_SubWorldSet.MessageLoop();
	g_SubWorldSet.MainLoop();
#ifndef _SERVER
	g_ScenePlace.Breathe();
#endif
	return true;
}

int KCoreShell::GetProtocolSize(BYTE byProtocol)
{
	if (byProtocol <= s2c_clientbegin || byProtocol >= s2c_end)
		return -1;
	return g_nProtocolSize[byProtocol - s2c_clientbegin - 1];
}

#ifdef SWORDONLINE_SHOW_DBUG_INFO
extern int		g_bShowObstacle;
extern bool		g_bShowGameInfo;	//是否显示游戏（场景）信息
#endif
int KCoreShell::Debug(unsigned int uDataId, unsigned int uParam, int nParam)
{
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	switch(uDataId)
	{
	case DEBUG_SHOWINFO:
		Player[CLIENT_PLAYER_INDEX].m_DebugMode = !Player[CLIENT_PLAYER_INDEX].m_DebugMode;
		g_bShowGameInfo = !g_bShowGameInfo;
		break;
	case DEBUG_SHOWOBSTACLE:
		g_bShowObstacle = !g_bShowObstacle;
		break;
	}
#endif
	return 0;
}

DWORD KCoreShell::GetPing()
{
#ifndef _SERVER
	return g_SubWorldSet.GetPing();
#else
	return 0;
#endif
}

void KCoreShell::SendPing()
{
#ifndef _SERVER
	SendClientCmdPing();
#endif
}

void KCoreShell::SetRepresentAreaSize(int nWidth, int nHeight)
{
#ifndef _SERVER
	g_ScenePlace.SetRepresentAreaSize(nWidth, nHeight);
#endif
}

void KCoreShell::SetClient(LPVOID pClient)
{
#ifndef _SERVER
	g_SetClient(pClient);
#endif
}

void KCoreShell::SendNewDataToServer(void* pData, int nLength)
{
#ifndef _SERVER
	if (g_pClient)
		g_pClient->SendPackToServer(pData, nLength);
#endif
}
