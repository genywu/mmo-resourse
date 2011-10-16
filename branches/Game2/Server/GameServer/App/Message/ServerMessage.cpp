#include "StdAfx.h"

#include "../AntiCheat/CConfiger.h"
#include "../BuffSkill/NewSkillFactory.h"
#include "../Business/CBusinessManager.h"
#include "../Country/CountryHandler.h"
#include "../Country/CountryParam.h"
#include "../FBusiness/ShopManager.h"
#include "../GoodsExManage.h"
#include "../Goods/CGoodsFactory.h"
#include "../Goods/IncrementShopManager.h"
#include "../Linkman/LinkmanSystem.h"
#include "../LuaScript/ScriptSys.h"
#include "../Monster.h"
#include "../PetCtrl.h"
#include "../Player.h"
#include "../PlayerMan/PlayerMan.h"
#include "../RgnManager.h"
#include "../ServerRegion.h"
#include "../../../../Public/ServerPublic/Server/Common/WordsFilter.h"
#include "../../../../Public/ServerPublic/Server/Common/VariableList.h"
#include "../../../../Public/Common/DupliRegionSetup.h"
#include "../../../../Public/Common/PetSetup.h"
#include "../../../../Public/DefSetup.h"

#include "../../../../Public/ServerPublic/Server/Common/AppTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern DWORD WINAPI SaveThreadFunc(LPVOID lpParameter);

// 响应其他消息
void OnServerMessage(CMessage* pMsg)
{
    switch(pMsg->GetType())
    {
        // WorldServer发送setup设置信息给GameServer
    case MSG_W2S_SERVER_SENDSETUP:
        {
            string module = "Unknown module";
            string position = "";
            uint state = 1;

            DBReadSet setReadDB;
            BYTE *ByteData = NULL;
            long lDelByteDataLen = 0;
            long lType = pMsg->GetLong();
            bool res = false;
            switch(lType)
            {
            case LOAD_AHCONF:
                {
                    module = "GAME_LOAD_AHCONF";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    pMsg->GetDBReadSet( setReadDB );
                    GetInst( AH::ConfReader ).Decode( setReadDB );
                }
                break;
            case LOAD_ANTICHEAT:               // BaiYun@2009525 反外挂
                {
                    module = "GAME_LOAD_ANTICHEAT";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    pMsg->GetDBReadSet(setReadDB);
                    GetInst( AntiCheat::CConfiger ).DecodeFromDataBlock(setReadDB);
                }
                break;
            case LOAD_BUSINESS:
                {
                    module = "GAME_LOAD_BUSINESS";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    pMsg->GetDBReadSet( setReadDB );
                    GetInst( Business::CBusinessManager ).DecodeFromDataBlock( setReadDB );
                }
                break;
            case LOAD_CONTACTS:
                {
                    module = "GAME_LOAD_CONTACTS";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    LinkmanSystem::GetInstance().InitSetup(ByteData, Pos);
                }
                break;
            case LOAD_CONTRIBUTE:		// TradeList
                {
                    module = "GAME_LOAD_CONTRIBUTE";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    ContributeSetup::DecordFromByteArray(ByteData,Pos);
                }
                break;
            case LOAD_COUNTRY:
                {
                    module = "GAME_LOAD_COUNTRY";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    GetCountryHandler()->DecordFromByteArray(ByteData,Pos);
                    M_FREE(ByteData, lDelByteDataLen);

                    lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    Pos = 0;
                    GetCountryParam()->DecordFromByteArray(ByteData,Pos);
                }
                break;
            case LOAD_DEPOT: 
                {
                    module = "GAME_LOAD_DEPOT";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(PlayerDepot::GetDepotSetup(), sizeof(tagDepotSetup));
                }
                break;
            case LOAD_EMOTE:
                {
                    module = "GAME_LOAD_EMOTE";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    EmotionSetup::Unserialize( ByteData,Pos );
                }
                break;
            case LOAD_ENCHASE:
                {
                    module = "GAME_LOAD_ENCHASE";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    KitCardRuleSetup::Unserialize( ByteData,Pos);
                }
                break;
            case LOAD_FBUSINESS:
                {
                    module = "GAME_LOAD_FBUSINESS";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    pMsg->GetDBReadSet( setReadDB );
                    GetInst( FBusiness::CShopManager ).Decode( setReadDB );
                }
                break;
            case LOAD_GATES:
                {
                    module = "GAME_LOAD_GATES";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen=pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData=(BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos=0;
                    GateList::DecordFromByteArray(ByteData,Pos);
                }
                break;
            case LOAD_GENVARS:		// GeneralVariableListFile
                {
                    module = "GAME_LOAD_GENVARS";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    if(!GetGame()->GetIsGenVarInit())
                    {
                        GetGame()->SetIsGenVarInit(true);
                        DBReadSet db;
                        pMsg->GetDBReadSet( db );
                        GetInst( ScriptSys ).UpdateGeneralVarList( db );
                    }
                }
                break;
            case LOAD_GLOBALSETUP:		// GlobalSetup
                {
                    module = "GAME_LOAD_GLOBALSETUP";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    GlobalSetup::DecordFromByteArray(ByteData,Pos);

                    AREA_WIDTH = GlobalSetup::GetSetup()->lAreaWidth;
                    AREA_HEIGHT = GlobalSetup::GetSetup()->lAreaHeight;

                    GameManager::GetInstance()->SetPolicy();
                }
                break;
            case LOAD_GMLIST:		// GMList
                {
                    module = "GAME_LOAD_GMLIST";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    res = GMList::DecordFromByteArray(ByteData,Pos);
                }
                break;
            case LOAD_GOODSEX:
                {
                    module = "GAME_LOAD_GOODSEX";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    res = GoodsExManage::GetInstance().InitSetup(ByteData, Pos);
                }
                break;
            case LOAD_GOODSLIST:		// GoodsList
                {
                    module = "GAME_LOAD_GOODSLIST";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    DBReadSet setReadDB;
                    pMsg->GetDBReadSet(setReadDB);
                    setReadDB.GetDwordFromByteArray();
                    GoodsSetup::Unserialize(setReadDB);
                    CPlayer::InitFightEnumMap();
                    MedalContainer::InitializeMedalSystem();
                }
                break;
            case LOAD_GOODSTAILOR:
                {
                    module = "GAME_LOAD_GOODSTAILOR";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    pMsg->GetDBReadSet(setReadDB);
                    GoodsTailorSetup::Decode(setReadDB);
                }
                break;
            case LOAD_HITLEVEL:
                {
                    module = "GAME_LOAD_HITLEVEL";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    HitLevelSetup::DecordFromByteArray(ByteData,Pos);
                }
                break;
            case LOAD_INCREMENTSHOP:		// TradeList
                {
                    module = "GAME_LOAD_INCREMENTSHOP";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    CIncrementShopList::DecordFromByteArray(ByteData,Pos);
                }
                break;
            case LOAD_INSTANCE:
                {
                    module = "GAME_LOAD_INSTANCE";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    GetGame()->GetDupliRegionSetup()->DecordFromByteArray(ByteData,Pos);
                }
                break;
            case LOAD_LOGSYS:
                {
                    module = "GAME_LOAD_LOGSYS";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    LogSystem::DecordFromByteArray(ByteData,Pos);
                }
                break;
            case LOAD_MONSTER:		// MonsterList
                {
                    module = "GAME_LOAD_MONSTER";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    CMonsterList::DecordFromByteArray(ByteData,Pos);

                    // 重设所有怪物的BasePropertiy
                    GetGame()->RefreashAllMonsterBaseProperty();
                }
                break;
            case LOAD_MONSTERAI:
                {
                    module = "GAME_LOAD_MONSTERAI";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    pMsg->GetDBReadSet( setReadDB );
                    GetInst( StateAI::MonsterAISetup ).Decode( setReadDB );
                }
                break;
            case LOAD_NPCSHOP:		// TradeList
                {
                    module = "GAME_LOAD_NPCSHOP";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    TradeList::DecordFromByteArray(ByteData,Pos);
                }
                break;
            case LOAD_PET:				// Fox@20081119 宠物
                {
                    module = "GAME_LOAD_PET";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    pMsg->GetDBReadSet(setReadDB);
                    GetInst(CPetCtrl).DecodeFromDataBlock(setReadDB);
                }
                break;
            case LOAD_PHASE:
                {
                    module = "GAME_LOAD_PHASE";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    pMsg->GetDBReadSet( setReadDB );
                    GetInst( PhaseSetup ).PhaseConfigDecode( setReadDB );
                }
                break;
            case LOAD_PLAYER:		// PlayerList
                {
                    module = "GAME_LOAD_PLAYER";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    PlayerList::DecordFromByteArray(ByteData,Pos);
                }
                break;
            case LOAD_PLAYERMGR:				// Fox@20081120 玩家管理
                {
                    module = "GAME_LOAD_PLAYERMGR";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    pMsg->GetDBReadSet(setReadDB);
                    GetInst(CPlayerMan).DecodeFromDataBlock(setReadDB);
                }
                break;
            case LOAD_PROFESSION:
                {
                    module = "GAME_LOAD_PROFESSION";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long pos = 0;
                    CDeputyOccuSystem::DecordFromByteArray(ByteData,pos);

                }
                break;
            case LOAD_QUEST:
                {
                    module = "GAME_LOAD_QUEST";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    CQuestSystem::DecordFromByteArray(ByteData,Pos);
                }
                break;
            case LOAD_REGIONLIST: // 场景列表
                {
                    module = "GAME_LOAD_REGIONLIST";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    eRgnType type = RGN_INVALID;
                    extern long g_lTotalMonster, g_lTotalNpc;
                    static long lM=0, lN=0;
                    long Pos = 0;
                    while ((type = (eRgnType)pMsg->GetLong()) != RGN_INVALID)
                    {
                        long rgnTemplateID = pMsg->GetLong();
                        CGUID rgnID;
                        pMsg->GetGUID(rgnID);
                        CServerRegion* pRegion = NULL;

                        if(type == RGN_NORMAL)
                            pRegion = GameManager::GetInstance()->GetRgnManager()->AddNormalRgn(rgnID, rgnTemplateID);//new CServerRegion(rgnID);

                        if (pRegion==NULL) break;

                        long lByteDataLen = pMsg->GetLong();
                        lDelByteDataLen = lByteDataLen;
                        ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);

                        pMsg->GetEx(ByteData,lByteDataLen);
                        pRegion->DecordFromByteArray(ByteData,Pos);
                        pRegion->OnChangeWeather();
                        pRegion->RegisterWeatherTimer();

                        M_FREE(ByteData, lDelByteDataLen);
                        ByteData = NULL; lDelByteDataLen = 0;

                        GetGame()->UpdateSaveRegion(pRegion);

                        char str[256];
                        _snprintf(str, 256, "%s (%d)", pRegion->GetName(), pRegion->GetID());
                        AppFrame::AddToList(AppFrame::LIST, str);

                        switch(type)
                        {
                        case RGN_NORMAL:
                           Log4c::Trace(ROOT_MODULE,FormatText("GS_RGNMGR_25", pRegion->GetID(), pRegion->GetName(), g_lTotalMonster-lM, g_lTotalNpc-lN));
                            break;
                        case RGN_PERSONAL:
                            Log4c::Trace(ROOT_MODULE,FormatText("GS_RGNMGR_26", pRegion->GetID(), pRegion->GetName(), g_lTotalMonster-lM, g_lTotalNpc-lN));
                            break;
                        case RGN_PERSONAL_HOUSE:
                            Log4c::Trace(ROOT_MODULE,FormatText("GS_RGNMGR_27", pRegion->GetID(), pRegion->GetName(), g_lTotalMonster-lM, g_lTotalNpc-lN));
                            break;
                        case RGN_TEAM:
                            Log4c::Trace(ROOT_MODULE,FormatText("GS_RGNMGR_28", pRegion->GetID(), pRegion->GetName(), g_lTotalMonster-lM, g_lTotalNpc-lN));
                            break;
                        }
                        lM = g_lTotalMonster;
                        lN = g_lTotalNpc;
                    }
                }
                break;
            case LOAD_REGIONSETUP:
                {
                    module = "GAME_LOAD_REGIONSETUP";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    CRegionSetup::DecordFromByteArray(ByteData,Pos);
                }
                break;
            case LOAD_RELIVEGOODS:
                {
                    module = "GAME_LOAD_RELIVEGOODS";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long lPos = 0;
                    ReLiveGoodsList::DecordFromByteArray(&ByteData[0], lPos);
                }
                break;
            case LOAD_REPUTATION:
                {
                    module = "GAME_LOAD_REPUTATION";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    pMsg->GetDBReadSet(setReadDB);
                    GetInst(CCreditSystem).DecodeToByteArray(setReadDB);
                }
                break;
            case LOAD_SCRIPT:		// ScriptFile
                {
                    module = "GAME_LOAD_SCRIPT";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    char strFileName[MAX_PATH];
                    pMsg->GetStr(strFileName,MAX_PATH);
                    long lFileLength = pMsg->GetLong();
                    lDelByteDataLen = lFileLength+1;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*(lFileLength+1));
                    memset(ByteData, 0, lFileLength+1);
                    pMsg->GetEx(ByteData, lFileLength);					
                    GetInst( ScriptSys ).GetHolder().Add( strFileName, (char*)ByteData, lFileLength );
                }
                break;
            case LOAD_SCRIPTS: // 所有脚本
                {
                    module = "GAME_LOAD_SCRIPTS";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    for (long l = 0; l < lByteDataLen; ++l)
                    {
                        char strFileName[MAX_PATH];
                        pMsg->GetStr(strFileName,MAX_PATH);
                        long lFileLength = pMsg->GetLong();
                        lDelByteDataLen = lFileLength+1;
                        ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*(lFileLength+1));
                        memset(ByteData, 0, lFileLength+1);
                        pMsg->GetEx(ByteData, lFileLength);					
                        GetInst( ScriptSys ).GetHolder().Add( strFileName, (char*)ByteData, lFileLength );
                        M_FREE(ByteData, lDelByteDataLen);
                        ByteData = NULL; lDelByteDataLen = 0;
                    }
                }
                break;
            case LOAD_SCRIPTAI:
                {
                    module = "GAME_LOAD_SCRIPTAI";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx( ByteData, lByteDataLen );
                    long lPos = 0;
                    CScriptAILoader::Instance().DecodeFromByteArray( ByteData, lPos );
                }
                break;
            case LOAD_SKILL:		// SkillList
                {
                    module = "GAME_LOAD_SKILL";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    CNewSkillFactory::DecodeNewSkillMessage(ByteData,Pos);
                    COccupSkillList::DecordFromByteArray(ByteData,Pos);
                    CStudySkillCondition::DecordFromByteArray(ByteData,Pos);

                    pMsg->GetDBReadSet( setReadDB );
                    GetInst( SkillAttribute ).ConfigDecode( setReadDB );
                }
                break;
            case LOAD_SPEECH:
                {
                    module = "GAME_LOAD_SPEECH";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    vector<BYTE> vData(lByteDataLen + 1);
                    pMsg->GetEx(&vData[0], lByteDataLen);
                    LONG lPos = 0;
                    CWordsFilter::GetInstance()->Decode(&vData[0], lPos);
                }
                break;
            case LOAD_SPRITE:
                {
                    module = "GAME_LOAD_SPRITE";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    SpriteSystem::SetupGetFormByteArray(ByteData, Pos);
                }
                break;
            case LOAD_SUIT:
                {
                    module = "GAME_LOAD_SUIT";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    SuitRuleSetup::Unserialize( ByteData,Pos);
                }
                break;
            case LOAD_SYSMAIL:
                {
                    module = "GAME_LOAD_SYSMAIL";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen =pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long lPos = 0;
                    MailList::DecordFromByteArray(ByteData,lPos);	
                    GameManager::GetInstance()->GetMailManager()->InitSysMail();
                }
                break;

            case LOAD_GENIUSSYS:
                {
                    module = "GAME_LOAD_GENIUSSYS";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    pMsg->GetDBReadSet( setReadDB );
                    GetInst( GeniusSetup ).GeniusConfigDecode( setReadDB );
                }
                break;
            case LOAD_TEXTRES:
                {
                    module = "GAME_LOAD_TEXTRES";
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData =(BYTE*)(BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    AppFrame::DecodeTextRes(ByteData, Pos);
                }
                break;
            case SYNC_BRANCHRGN:
                {
                    module = "GAME_SYNC_BRANCHRGN";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    long lByteDataLen = pMsg->GetLong();
                    lDelByteDataLen = lByteDataLen;
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                    pMsg->GetEx(ByteData,lByteDataLen);
                    long Pos = 0;
                    GameManager::GetInstance()->GetRgnManager()->DecodeLinedRgnInfoFromByteArray(ByteData,Pos, lByteDataLen);
                }
                break;
            case SYNC_END:	// 正式启动，初始化服务器网络
                {
                    module = "";
                    GetGame()->SetSaveDeltaTime(pMsg->GetLong());

                    //// 建立服务器端网络
                    if( !GetGame()->StartAccept() )
                    {
                        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"Can't init NetServer!");
                        Log4c::Warn(ROOT_MODULE, GetText("STR_GS_SERVERMESSAGE_INITSERVERNETFAILED"));//==========初始化服务器端网络失败!==========
                    }
                    Log4c::Trace(ROOT_MODULE,FormatText("GAME_SYS_SERVEROK", "GameServer"));
                }
                break;
            case SYNC_GSINFO:
                {
                    module = "GAME_SYNC_GSINFO";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    GetGame()->SetIDIndex(pMsg->GetByte());
                }
                break;
            case SYNC_INCLIMIT:
                {
                    module = "GAME_SYNC_INCLIMIT";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    lDelByteDataLen = pMsg->GetLong();
                    ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lDelByteDataLen);	
                    pMsg->GetEx(ByteData,lDelByteDataLen);
                    long Pos = 0;
                    CIncrementShopManager::DecordFromByteArray( ByteData, Pos );
                }
                break;
            case SYNC_INSTANCESELF:
                {
                    module = "GAME_SYNC_INSTANCESELF";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    // 
                    CGUID dupRgnID;
                    pMsg->GetGUID(dupRgnID);

                    long GSID = pMsg->GetLong();

                    char name[128];
                    pMsg->GetStr(name, 128);

                    CGUID ownerID;
                    pMsg->GetGUID(ownerID);
                    long tRgnID = pMsg->GetLong();
                    long lResourceID = pMsg->GetLong();

                    // 创建PHDupRgn
                    CRgnManager* rgnManger = GameManager::GetInstance()->GetRgnManager();
                    CServerRegion* wRgn = NULL;
                    CRgnManager::RgnByIDMapItr gsRgnItr = rgnManger->GetPersonalHouseDupRgnMap().find(dupRgnID);
                    if(gsRgnItr == rgnManger->GetPersonalHouseDupRgnMap().end())
                    {
                        wRgn = rgnManger->AddCreatedDupRgn(dupRgnID, RGN_PERSONAL_HOUSE);
                        rgnManger->GetPersonalHouseDupRgnMap()[dupRgnID] = wRgn;
                    }
                    else
                        wRgn = gsRgnItr->second;

                    if(wRgn)
                    {	
                        wRgn->SetID(tRgnID);			
                        wRgn->SetExID(dupRgnID);
                        wRgn->SetOwnerGUID(ownerID);
                        wRgn->SetRgnType((long)RGN_PERSONAL_HOUSE);
                        wRgn->SetName(name);
                        wRgn->SetResourceID(lResourceID);

                        char szRGUID[128];
                        wRgn->GetExID().tostring(szRGUID);
						//成功初始化创建个人房屋副本场景对象,场景ID［%s］,模板ID［%d］,场景名［%s］!
                        Log4c::Trace(ROOT_MODULE,FormatText("GS_SVRMSG_23",szRGUID, wRgn->GetID(), wRgn->GetName()));
                        long dataBufSize = pMsg->GetLong();
                        if(dataBufSize)
                        {
                            BYTE* rgnData = (BYTE*)M_ALLOC(sizeof(BYTE)*dataBufSize);
                            pMsg->GetEx(rgnData, dataBufSize);
                            long pos = 0;
                            wRgn->DecordFromByteArray(rgnData, pos);
                            M_FREE(rgnData, dataBufSize);
                        }
                    }

                    long goodsNum = pMsg->GetLong();
                    CRgnManager::PHGoodsMap& goodsMap = GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap();
                    CRgnManager::PHGoodsMapItr goodsItr = goodsMap.find(dupRgnID);
                    if(goodsItr == goodsMap.end())
                    {
                        vector<CRgnManager::PHGoods> goodsVec;
                        for(int t=0; t<goodsNum; t++)
                        {
                            CRgnManager::PHGoods goods;
                            pMsg->GetEx(&goods, sizeof(CRgnManager::PHGoods));
                            goodsVec.push_back(goods);
                        }
                        goodsMap[dupRgnID] = goodsVec;
                    }
                    else
                    {
                        goodsItr->second.clear();
                        for(int t=0; t<goodsNum; t++)
                        {
                            CRgnManager::PHGoods goods;
                            pMsg->GetEx(&goods, sizeof(CRgnManager::PHGoods));
                            goodsItr->second.push_back(goods);
                        }
                    }

                    if(wRgn)
                    {	
                        CPlayer* owner = GetGame()->FindPlayer(ownerID);
                        if(owner)
                            owner->SetPersonalHouseRgnGUID(wRgn->GetExID());

                        CRgnManager::PHGoodsMap& tmpGoodsMap = GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap();
                        CRgnManager::PHGoodsMapItr tmpGoodsItr = tmpGoodsMap.find(wRgn->GetExID());
                        if(tmpGoodsItr != tmpGoodsMap.end())
                        {
                            for(int t=0; t<(int)tmpGoodsItr->second.size(); t++)
                            {
                                CRgnManager::PHGoods& goods = (tmpGoodsItr->second)[t];

                                // 创建物品
                                LONG lNewX = 0; 
                                LONG lNewY = 0;
                                wRgn -> GetRandomPosInRange( lNewX, lNewY, 
                                    goods.width, goods.height, 
                                    goods.width - (int)goods.posX,
                                    goods.height -(int)goods.posY );

                                CMonster* pLeader = wRgn -> AddMonster( goods.name, lNewX, lNewY, -1 );
                                if( pLeader == NULL ) 
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
            case SYNC_INSTANCEDATA:
                {
                    module = "GAME_SYNC_INSTANCEDATA";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    GameManager::GetInstance()->GetRgnManager()->DelAllDupRgnData();
                    long lRgnNum = pMsg->GetLong();// 副本模板个数
                    for(int i=0; i<lRgnNum; i++)
                    {
                        long lRgnTemplateId = pMsg->GetLong(); // 副本模板ID
                        long lBufSize = pMsg->GetLong(); // bufsize
                        if(lBufSize > 0)
                        {
                            BYTE* pBuf = (BYTE*)M_ALLOC(sizeof(BYTE)*lBufSize);
                            pMsg->GetEx(pBuf, lBufSize);
                            // 将该内存转移到map管理
                            if(!GameManager::GetInstance()->GetRgnManager()->AddDupRgnData(lRgnTemplateId, pBuf, lBufSize))
                                M_FREE(pBuf, sizeof(BYTE)*lBufSize);
                            pBuf = NULL;
                            lBufSize = 0;
                        }
                    }
                }
                break;
            case SYNC_REGION:
                {
                    module = "GAME_SYNC_REGION";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    CGUID lRegionID;
                    pMsg->GetGUID(lRegionID);
                    CServerRegion*pServerRegion=GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(lRegionID);
                    if(pServerRegion)
                    {
                        long lByteDataLen = pMsg->GetLong();
                        lDelByteDataLen = lByteDataLen;
                        ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
                        pMsg->GetEx(ByteData,lByteDataLen);
                        long Pos = 0;
                        pServerRegion->DecordSetupFromByteArray(ByteData,Pos);
						//重装场景(%d)%s 配置...ok!
                        Log4c::Trace(ROOT_MODULE,FormatText("GS_SVRMSG_30",pServerRegion->GetID(),pServerRegion->GetName()));
                    }
                }
                break;
            case SYNC_SCRIPT:		// FunctionListFile
                {
					module = "GAME_SYNC_SCRIPT";
                    // NOTE: deprecated codes
					Log4c::Warn(ROOT_MODULE,"[SRVMSG] Script data is deprecated.");

                    //module = "GAME_SYNC_SCRIPT";
                    //LoadLog(module.c_str(), position.c_str(), 0);
                    //long lFileLength = pMsg->GetLong();
                    //lDelByteDataLen = lFileLength+1;
                    //ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*(lFileLength+1));
                    //memset(ByteData, 0, lFileLength+1);
                    //pMsg->GetEx(ByteData, lFileLength);

                    //GetGame()->SetFunctionFileData((char*)ByteData, sizeof(BYTE)*(lFileLength+1));
                }
                break;
            case SYNC_VARIABLES:		// VariableListFile
                {
                    // NOTE: deprecated codes
					Log4c::Warn(ROOT_MODULE,"[SRVMSG] Variables data is deprecated.");

                    //module = "GAME_SYNC_VARIABLES";
                    //LoadLog(module.c_str(), position.c_str(), 0);
                    //long lFileLength = pMsg->GetLong();
                    //lDelByteDataLen = lFileLength+1;
                    //ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*(lFileLength+1));
                    //memset(ByteData, 0, lFileLength+1);
                    //pMsg->GetEx(ByteData, lFileLength);

                    //GetGame()->SetVariableFileData((char*)ByteData, sizeof(BYTE)*(lFileLength+1));
                }
                break;
            case SYNC_WSINFO:
                {
                    module = "GAME_SYNC_WSINFO";
                    LoadLog(module.c_str(), position.c_str(), 0);
                    DWORD dwNumber=pMsg->GetDWord();
                    GetGame()->SetWSNumber(dwNumber);
                }
                break;
            }
            if (!module.empty())
                LoadLog(module.c_str(), position.c_str(), state);
            M_FREE(ByteData, lDelByteDataLen);
        }
        break;

        // WorldServer关闭
    case MSG_S2S_SERVER_WORLDSERVERCLOSE:
        {
            Log4c::Trace(ROOT_MODULE,GetText("GS_SVRMSG_71"));//============WorldServer closed==========!!!
            GetGame()->CreateConnectWorldThread();
        }
        break;
        // [AHC 080809 DBSMSG]
    case MSG_W2S_SERVER_QUEST_RECONNECTION:
        {
            Log4c::Trace(ROOT_MODULE,GetText("GS_SVRMSG_46"));//从WS反馈Reconnect消息OK！

        }
        break;
        // [AHC 080809 DBSMSG]
    case MSG_DB2S_SERVER_QUEST_RECONNECTION:
        {
            // 
            long msgTimerID = pMsg->GetLong();
            Log4c::Trace(ROOT_MODULE,GetText("GS_SVRMSG_47"));//从DBS反馈Reconnect消息OK！
        }
        break;
        // 返回切换服务器
    case MSG_W2S_SERVER_CHANGEREGION:
        {
            long lSignCode = pMsg->GetLong();

            char bSuccess = pMsg->GetChar();
            CGUID PlayerID;
            pMsg->GetGUID(PlayerID);

            // 成功
            if(bSuccess!=0)
            {
                // 成功
                char strIP[256];
                pMsg->GetStr(strIP,256);
                DWORD dwPort = pMsg->GetDWord();

                CMessage msg(MSG_S2C_RGN_CHANGESERVER);
                // 添加验证代码
                msg.Add(lSignCode);
                msg.Add(strIP);
                msg.Add(dwPort);
                msg.SendToPlayer(PlayerID);
            }
            else
                GetGame()->KickPlayer(PlayerID);
        }
        break;
        // 发送信息
    case MSG_W2S_SERVER_ADDINFO:
        {
            CGUID PlayerID;
            pMsg -> GetGUID(PlayerID);
            if( PlayerID != NULL_GUID )
                /**## 发送给指定的玩家 */
            {
                /*char strMsg[1024];
                char strSender[24];
                char strSend[1024];
                DWORD dwColor = 0;
                DWORD dwBkColor = 0;
                LONG lPos = 0;

                pMsg -> GetStr( strMsg,1024 );
                dwColor		= pMsg -> GetDWord();
                dwBkColor	= pMsg->GetDWord();
                lPos		= pMsg->GetLong();

                pMsg -> GetStr( strSender,24 );

                if(strlen(strSender) > 0)
                {
                    _snprintf( strSend, 1024, "%s：%s", strSender, strMsg );
                }
                else
                {
                    _snprintf( strSend, 1024, "%s",strMsg );
                }
                GetGame()->SendNotifyMessage(strSend,PlayerID,dwColor,dwBkColor, (eNOTIFY_POS)lPos);*/
				/***********************************************************************/
				/*  zhaohang 2010-11-8 fix 剔除颜色，直接将颜色加入字符串中，客户端能自动解析*/
				/***********************************************************************/
				char strMsg[1024];
				char strSender[24];
				char strSend[1024];
				
				LONG lPos = 0;

				pMsg -> GetStr( strMsg,1024 );
				/*dwColor		= pMsg -> GetDWord();
				dwBkColor	= pMsg->GetDWord();*/
				lPos		= pMsg->GetLong();

				pMsg -> GetStr( strSender,24 );

				if(strlen(strSender) > 0)
				{
					_snprintf( strSend, 1024, "%s：%s", strSender, strMsg );
				}
				else
				{
					_snprintf( strSend, 1024, "%s",strMsg );
				}
				//GetGame()->SendNotifyMessage(strSend,PlayerID,dwColor,dwBkColor, (eNOTIFY_POS)lPos);
				GetGame()->SendNotifyMessage(strSend,PlayerID,(eNOTIFY_POS)lPos);
            }
            else 
                /**## 返回给发送者无法发送的消息 */
                /**## 失败：LONG Reciver(=0), char* Reciver, LONG Sender */
            {
                char strPlayer[24];


                pMsg -> GetStr( strPlayer,24 );
                CGUID SenderID;
                pMsg -> GetGUID(SenderID);

                char strSend[256];
                _snprintf( strSend, 256, "消息无法发送给%s。", strPlayer );
				/***********************************************************************/
				/*  zhaohang 2010-11-8 fix 直接剔除颜色，颜色加入字符串中，客户端能自动解析*/
				/***********************************************************************/
                //GetGame()->SendNotifyMessage(strSend,SenderID,0xffffffff);
				GetGame()->SendNotifyMessage(strSend,SenderID);
            }
        }
        break;

        // 改变全局脚本变量的值
    case MSG_W2S_SERVER_CHANGE_SCRIPT_VARIABLE:
        {
            char strVarName[256];
            pMsg->GetStr(strVarName, 256);
            BYTE nFlag = pMsg->GetByte();
            if( eVarNumber == nFlag ) // 0 数值
            {
                long pos = pMsg->GetLong();
                LONG64 llRet = pMsg->GetLONG64();
                double nValue = *(double*)&llRet;
                //if( CScript::GetGeneralVariableList() )
                //	CScript::GetGeneralVariableList()->SetVarValue(strVarName, pos, nValue);
                GetInst( ScriptSys ).GetGeneralVarList()->SetVarValue( strVarName, nValue, pos );
            }
            else if( eVarString == nFlag ) // 字符串
            {
                char szValue[1024];
                pMsg->GetStr(szValue, 1024);
                //if( CScript::GetGeneralVariableList() )
                //	CScript::GetGeneralVariableList()->SetVarValue(strVarName, szValue);
                GetInst( ScriptSys ).GetGeneralVarList()->SetVarValue( strVarName, tString( szValue ) );
            }
            else if( eVarGuid == nFlag ) // guid
            {
                CGUID guid;
                pMsg->GetGUID(guid);
                //if( CScript::GetGeneralVariableList() )
                //	CScript::GetGeneralVariableList()->SetGuidValue(strVarName, &guid);
                GetInst( ScriptSys ).GetGeneralVarList()->SetVarValue( strVarName, guid );
            }
        }
        break;

        //##同步玩家的杀人数
    case MSG_W2S_SERVER_SYNC_MURDERER_COUNT:
        {
            LONG lSuffererType	= pMsg -> GetLong();
            CGUID SuffererID; pMsg->GetGUID(SuffererID);
            LONG lMurdererType	= pMsg -> GetLong();
            CGUID MurdererID; pMsg->GetGUID(MurdererID);

            CPlayer* pPlayer	= GetGame() -> FindPlayer( MurdererID );
            if( pPlayer ) 
            {
                long lNum = pPlayer -> GetPkValue() + GlobalSetup::GetSetup()->dwPkCountPerKill;
                if (lNum > 65535)
                    lNum = 65535;
                pPlayer->SetAttribute("PkValue",lNum);// etAttribute();
                pPlayer->SetAttribute("PkCount",pPlayer -> GetPkCount() + 1);
                pPlayer -> OnUpdateMurdererSign();
                pPlayer->UpdateAttribute();
            }
            else 
            {
                //##如果找不到怎么办？继续交给WorldServer处理？
                pMsg -> SetType( MSG_S2W_SERVER_REPORT_MURDERER );
                pMsg -> Send();
            }
        }
        break;
        // 返回ping信息
    case MSG_W2S_SERVER_QUEST_PING:
        {
            //CMyNetClient* pNetClient = GetGame()->GetNetClient();
            //if (pNetClient)
            {
                DWORD flag = pMsg->GetDWord();
                CMessage msg(MSG_S2W_SERVER_ANSWER_PING);
                msg.Add(GetGame()->GetPlayerCount());
                msg.Add(flag);
                msg.Send();
            }
        }
        // 返回ping信息
    case MSG_DB2S_SERVER_QUEST_PING:
        {
            //CMyNetClient* pNetClient = GetGame()->GetNetClient();
            //if (pNetClient)
            {
                CMessage msg(MSG_S2DB_SERVER_ANSWER_PING);
                msg.Add(GetGame()->GetPlayerCount());
                msg.Send();
            }
        }
        break;
    case MSG_W2S_SERVER_CREATEMONSTER:
        {
            CHAR szMonsterName[256];
            CHAR szScriptName[256];
            ZeroMemory( szMonsterName, 256 );
            ZeroMemory( szScriptName, 256 );

            CGUID lRegionID;
            pMsg->GetGUID(lRegionID);
            pMsg -> GetStr( szMonsterName, 256 );
            long nCounts 	= pMsg -> GetLong();
            long nLeft		= pMsg -> GetLong();
            long nTop		= pMsg -> GetLong();
            long nRight		= pMsg -> GetLong();
            long nBottom	= pMsg -> GetLong();
            BYTE bHasScript	= pMsg -> GetByte();
            if( bHasScript )
            {
                pMsg -> GetStr( szScriptName, 256 );
            }

            CServerRegion* pRegion	= GetGame() -> FindRegion( lRegionID );
            if( pRegion )
            {
                LONG lNewX = 0; 
                LONG lNewY = 0;
                pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );

                for (int i=0; i < nCounts; i++)
                {
                    pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );
                    CMonster* pMonster = pRegion -> AddMonster( szMonsterName, lNewX, lNewY, -1 );
                    if (pMonster)
                    {
                        pMonster->SetScriptFile(szScriptName);
                    }
                }
            }

        }
        break;
    case MSG_W2S_SERVER_CREATEPHGOODS:
        {
            CHAR szMonsterName[256];
            CHAR szScriptName[256];
            ZeroMemory( szMonsterName, 256 );
            ZeroMemory( szScriptName, 256 );

            BYTE bType		= pMsg->GetByte();
            if (bType == 0)	// monster
            {
                CGUID guid;
                pMsg->GetGUID(guid);
                CGUID lRegionID;
                pMsg->GetGUID(lRegionID);
                pMsg -> GetStr( szMonsterName, 256 );
                long nCounts 	= pMsg -> GetLong();
                long nLeft		= pMsg -> GetLong();
                long nTop		= pMsg -> GetLong();
                long nRight		= pMsg -> GetLong();
                long nBottom	= pMsg -> GetLong();
                long tempID		= pMsg -> GetLong();
                long bHasScript	= pMsg -> GetByte();
                if( bHasScript )
                {
                    pMsg -> GetStr( szScriptName, 256 );
                }

                CServerRegion* pRegion	= GetGame() -> FindRegion( lRegionID );
                if( pRegion )
                {
                    LONG lNewX = 0; 
                    LONG lNewY = 0;
                    pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );

                    for (int i=0; i < nCounts; i++)
                    {
                        pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );
                        CMonster* pMonster = pRegion -> AddMonster( szMonsterName, lNewX, lNewY, -1 );
                        if (pMonster)
                        {
                            pMonster->SetScriptFile(szScriptName);

                            // 加入PHGoods容器
                            CRgnManager::PHGoods tGoods;
                            CRgnManager::PHGoodsMapItr goodsItr = GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap().find(pRegion->GetExID());

                            CRgnManager::vecPHGoods* vecPHGoodsPtr = NULL;

                            if(goodsItr == GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap().end())
                            {
                                GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap()[pRegion->GetExID()] = CRgnManager::vecPHGoods();
                                vecPHGoodsPtr = &GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap()[pRegion->GetExID()];
                            }

                            if(vecPHGoodsPtr != NULL)
                            {
                                CRgnManager::vecPHGoodsItr vecGoodsItr = vecPHGoodsPtr->begin();
                                for(; vecGoodsItr != vecPHGoodsPtr->end(); vecGoodsItr++)
                                {
                                    if(
                                        //	vecGoodsItr->guid == right->guid &&
                                        strcmp((*vecGoodsItr).name, szMonsterName) == 0 &&
                                        (*vecGoodsItr).dir == nCounts &&
                                        (*vecGoodsItr).width == nLeft &&
                                        (*vecGoodsItr).height == nTop &&
                                        (*vecGoodsItr).posX == nRight &&
                                        (*vecGoodsItr).posY == nBottom) // 是同一个GOODS
                                        break;
                                }

                                if(vecGoodsItr == vecPHGoodsPtr->end()) // 未找到
                                {
                                    CRgnManager::PHGoods tGoods;
                                    CGUID::CreateGUID(tGoods.guid);
                                    memcpy(tGoods.name, szMonsterName, sizeof(tGoods.name));
                                    tGoods.dir = nCounts;
                                    tGoods.width = nTop;
                                    tGoods.height = nLeft;
                                    tGoods.posX = (float)nRight;
                                    tGoods.posY = (float)nBottom;
                                    vecPHGoodsPtr->push_back(tGoods);
                                }
                            }

                            char szGUID[128];
                            pMonster->GetExID().tostring(szGUID);
                            char szRgnID[128];
                            lRegionID.tostring(szRgnID);
                           Log4c::Trace(ROOT_MODULE,FormatText("GS_SVRMSG_49", szRgnID, szMonsterName, szGUID));//创建个人房屋副本(ID%s)物品(名字%s,ID%s)!
                        }
                    }
                }
            }
        }
        break;
        // 挂起的脚本函数数据库返回消息
    case MSG_W2S_SERVER_PLAYER_PERSONALHOUSEGUID:
        {
            CGUID scriptID;
            pMsg->GetGUID(scriptID); //Script id
            char szPlayerName[128];
            pMsg->GetStr(szPlayerName, 128); // player name
            char szGuidName[128];
            pMsg->GetStr(szGuidName, 128); // ret guid name string
            CGUID retGUID;
            pMsg->GetGUID(retGUID);

            //wq:2008-7-9,需要修改
            //CVariableList* stackVarList = CScript::GetStackVarListInst();
            //if(stackVarList)
            //{
            //	stackVarList->Release();
            //	stackVarList->AddGuid(szGuidName, retGUID);
            //}
            //ContinueRunScript(scriptID, stackVarList);
            Script *script = GetInst( ScriptSys ).GetScript( scriptID );
            if( script != NULL )
            {
                script->PushRetValue( retGUID );
                GetInst( ScriptSys ).ResumeScript( script, 1 );
            }

        }
        break;
    case MSG_W2S_SERVER_BUSINESS_UPDATE:
        {
            //更新贸易点的商业国家归属
            int i=0;

        }
        break;
    }
}
