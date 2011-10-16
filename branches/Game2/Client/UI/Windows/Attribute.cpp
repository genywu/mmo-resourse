#include "stdafx.h"
#include "../UIDef.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/Player.h"

#include "../../ui/UIManager.h"
#include "../../GameClient/ClientApp/Other/CountryList.h"
#include "../../GameClient/ClientApp/Other/PicList.h"
#include "../../ui/UIExtendComponent/3DIconEffect.h"
#include "../../GameClient/ClientApp/MainPlayerHand.h"

//装备栏位表（枚举名 - 配置窗口名）
string strEquipToWin[CPlayer::EQUIP_SUM];
long   lEquipGoodsToIndex[CPlayer::EQUIP_SUM];
//更新界面属性
bool OnUpdateArribute(const CEGUI::EventArgs &args);
// 旋转模型
bool Rot3DModel(const CEGUI::EventArgs &args);
bool StopRotation(const CEGUI::EventArgs &args);
void SetEquipEvent(CPlayer::eEquipIndex index);
void UpdateEquipmentIcon(CPlayer::eEquipIndex index);
bool ChangeEquip(const CEGUI::EventArgs &args);
C3DIconEffect * Att_3DIconEffect = 0; 
CEGUI::Window* InitAttribute()
{
    lEquipGoodsToIndex[CPlayer::EQUIP_HEAD] = EC_E_HEAD;
    lEquipGoodsToIndex[CPlayer::EQUIP_NECKLACE] = EC_E_NECKLACE;
    lEquipGoodsToIndex[CPlayer::EQUIP_WING] = EC_E_WING;
    lEquipGoodsToIndex[CPlayer::EQUIP_BODY] = EC_E_BODY;
    lEquipGoodsToIndex[CPlayer::EQUIP_BACK] = EC_E_BACK;
    lEquipGoodsToIndex[CPlayer::EQUIP_HAND] = EC_E_HAND;
    lEquipGoodsToIndex[CPlayer::EQUIP_SHOULDER] = EC_E_SHOULDER;
    lEquipGoodsToIndex[CPlayer::EQUIP_WRIST] = EC_E_WRIST;
    lEquipGoodsToIndex[CPlayer::EQUIP_LEG] = EC_E_LEG;
    lEquipGoodsToIndex[CPlayer::EQUIP_BOOT] = EC_E_BOOT;
    lEquipGoodsToIndex[CPlayer::EQUIP_LRING] = EC_E_LRING;
    lEquipGoodsToIndex[CPlayer::EQUIP_RRING] = EC_E_RRING;
    lEquipGoodsToIndex[CPlayer::EQUIP_WEAPON] = EC_E_WEAPON;
    lEquipGoodsToIndex[CPlayer::EQUIP_CARD] = EC_E_CARD;
    lEquipGoodsToIndex[CPlayer::EQUIP_GODGOODS] = EC_E_GODGOODS;
    lEquipGoodsToIndex[CPlayer::EQUIP_EFFIGY] = EC_E_EFFIGY;
    lEquipGoodsToIndex[CPlayer::EQUIP_LIFECOWRY] = EC_E_LIFECOWRY;
    lEquipGoodsToIndex[CPlayer::EQUIP_SOULGUARD] = EC_E_SOULGUARD;
    lEquipGoodsToIndex[CPlayer::EQUIP_RIDER] = EC_E_RIDER;
    strEquipToWin[CPlayer::EQUIP_HEAD] = "TabGroup/Tab 1/EquipHead/Drage";
    strEquipToWin[CPlayer::EQUIP_NECKLACE] = "TabGroup/Tab 1/Equipnecklace/Drage";
    strEquipToWin[CPlayer::EQUIP_WING] = "TabGroup/Tab 1/EquipWing/Drage";
    strEquipToWin[CPlayer::EQUIP_BODY] = "TabGroup/Tab 1/EquipArmo/Drage";
    strEquipToWin[CPlayer::EQUIP_BACK] = "TabGroup/Tab 1/EquipBelt/Drage";
    strEquipToWin[CPlayer::EQUIP_HAND] = "TabGroup/Tab 1/EquipGlove/Drage";
    strEquipToWin[CPlayer::EQUIP_BOOT] = "TabGroup/Tab 1/EquipShoe/Drage";
    strEquipToWin[CPlayer::EQUIP_SHOULDER] = "TabGroup/Tab 1/FashionHead/Drage";
    strEquipToWin[CPlayer::EQUIP_WRIST] = "TabGroup/Tab 1/FashionCloth/Drage";
    strEquipToWin[CPlayer::EQUIP_LRING] = "TabGroup/Tab 1/LeftRing/Drage";
    strEquipToWin[CPlayer::EQUIP_RRING] = "TabGroup/Tab 1/RightRing/Drage";
    strEquipToWin[CPlayer::EQUIP_WEAPON] = "TabGroup/Tab 1/EquiWeapon/Drage";
    strEquipToWin[CPlayer::EQUIP_SOULGUARD] = "TabGroup/Tab 1/AssistantWeapon/Drage";
    strEquipToWin[CPlayer::EQUIP_RIDER] = "TabGroup/Tab 1/EquiHorse/Drage";

    CEGUI::Window* wnd = LoadUI("Attribute");
    wnd->setVisible(false);
    wnd->setAlwaysOnTop(true);
    //关联事件
    wnd->subscribeEvent(EVENT_OPEN,CEGUI::Event::Subscriber(OnUpdateArribute));
    wnd->subscribeEvent("UpdateAttribute",CEGUI::Event::Subscriber(OnUpdateArribute));
    CPlayer *pPlayer = GetGame()->GetMainPlayer();
    CEGUI::Window *p3Dwin = GetWndMgr().getWindow("TabGroup/Tab 1/PlayerModel/Model");

    C3DIconEffect * Att_3DIconEffect = new C3DIconEffect(p3Dwin);
    Att_3DIconEffect->SetIconShape(GetGame()->GetMainPlayer());
    Att_3DIconEffect->SetIconPos(0.f,-0.3f,0.f);
    Att_3DIconEffect->SetIconScale(2.5f,1.5f,2.5f);

    UIManager::Add3DIconEffect(p3Dwin,Att_3DIconEffect);
    //加点事件处理注册

    CEGUI::PushButton    *pPowerAddBtn = WPushButton(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/PowerAdd"));

    CEGUI::PushButton    *pPowerSubBtn = WPushButton(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/PowerSub"));

    CEGUI::PushButton    *pLeftRotBtn = WPushButton(GetWndMgr().getWindow("TabGroup/Tab 1/PlayerModel/RotLeft"));
    pLeftRotBtn->subscribeEvent(CEGUI::PushButton::EventMouseButtonDown,CEGUI::Event::Subscriber(Rot3DModel));
    pLeftRotBtn->subscribeEvent(CEGUI::PushButton::EventMouseButtonUp,CEGUI::Event::Subscriber(StopRotation));
    CEGUI::PushButton    *pRightRotBtn = WPushButton(GetWndMgr().getWindow("TabGroup/Tab 1/PlayerModel/RotRight"));
    pRightRotBtn->subscribeEvent(CEGUI::PushButton::EventMouseButtonDown,CEGUI::Event::Subscriber(Rot3DModel));
    pRightRotBtn->subscribeEvent(CEGUI::PushButton::EventMouseButtonUp,CEGUI::Event::Subscriber(StopRotation));
    
    
    for (int i = 0;i<CPlayer::EQUIP_SUM;i++)
        SetEquipEvent((CPlayer::eEquipIndex)i);
    return wnd;
}

bool UpdateMainPlayerHand(const CEGUI::EventArgs &args)
{
    using namespace CEGUI;
    
    const WindowEventArgs& dd_args = static_cast<const WindowEventArgs&>(args);
    //得到当前正在拖拽的物品容器
    DragContainer *pCurDrag = static_cast<DragContainer*>(dd_args.window);
    string wndName = pCurDrag->getName().c_str();
    
    CPlayer::eEquipIndex pos = CPlayer::EQUIP_SUM;
    for(long i= CPlayer::EQUIP_HEAD;i < CPlayer::EQUIP_SUM;i++)
    {
        if (wndName == strEquipToWin[(CPlayer::eEquipIndex)i])
        {
            pos = (CPlayer::eEquipIndex)i;
            break;
        }
    }
    if (pos != CPlayer::EQUIP_SUM)
    {
        CGoods *pEquipGoods = GetGame()->GetMainPlayer()->GetEquip(pos);
        CMainPlayerHand *pHand = GetGame()->GetMainPlayerHand();
        if(pEquipGoods && pHand)
        {
            pHand->SetMainPlayerHandByGoods(false,pEquipGoods->GetNum(),pos,pEquipGoods,PEI_EQUIPMENT);
        }
        else
        {
            pHand->ClearMainPlayerHand();
        }
    }

    return true;
}

bool ChangeEquip(const CEGUI::EventArgs &args)
{
    //处理在装备栏位上鼠标右键点击
    const CEGUI::MouseEventArgs &e_mouse = static_cast<const CEGUI::MouseEventArgs&>(args);
    
    CEGUI::DragContainer *pDrug = static_cast<CEGUI::DragContainer*>(e_mouse.window);
    CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
    CMainPlayerHand *pHand = GetGame()->GetMainPlayerHand();
    if (pMainPlayer->GetState() == CShape::STATE_PEACE)
    {
        long nSPos = -1;
        for (int i=0;i<CPlayer::EQUIP_SUM;i++)
        {
            string name = pDrug->getName().c_str();
            if(strEquipToWin[i] == name)
            {
                nSPos = i;
                break;
            }
        }
        if (e_mouse.button == CEGUI::RightButton)
        { 
            CGoods *pGoods = pMainPlayer->GetEquip(nSPos);
            if (pGoods == NULL)
                return false;
            DWORD dwAmount = pGoods->GetNum();
            long lExtendID = pMainPlayer->GetNotFullPackage();
            int	 iPos = -1;
            if (lExtendID!=-1)
            {
                iPos = pMainPlayer->GetFirstEmptyGrid(lExtendID);
                if (lExtendID==0)
                {
                    lExtendID = PEI_PACKET;
                }else if (lExtendID>=1&&lExtendID<=5)
                {
                    lExtendID = PEI_SUBPACK+lExtendID;
                }
            }

            pHand->SendPutDownMes(TYPE_PLAYER,pMainPlayer->GetExID(),PEI_EQUIPMENT,nSPos,TYPE_PLAYER,pMainPlayer->GetExID(),
                lExtendID,iPos,TYPE_GOODS,pGoods->GetExID(),dwAmount);
        } 
        else
        {
            //检查物品放置的装备栏是否对应。】
            CPlayer::tagGoods *pHandGoods = pHand->GetTGoodsOfMainPlayerHand();
            if(pHandGoods && pHandGoods->pGoods)
            {
                long lEquipType = (long)pHandGoods->pGoods->GetEquipType();
                if (lEquipGoodsToIndex[nSPos] != lEquipType)
                    return false;

                if (pHandGoods && pHandGoods->pGoods)
                {
                    const CEGUI::DragDropEventArgs &e_DragArgs = static_cast<const CEGUI::DragDropEventArgs&>(args);
                    if(e_DragArgs.window->getName() == strEquipToWin[nSPos])
                    {
                        DWORD dwAmount = pHandGoods->pGoods->GetNum();
                        CGUID PlayerGUID = pMainPlayer->GetExID();
                        CGUID GoodsGUID = pHandGoods->pGoods->GetExID();
                        pHand->SendPutDownMes(TYPE_PLAYER,PlayerGUID,pHandGoods->lType,pHandGoods->lPos,TYPE_PLAYER,
                            PlayerGUID,PEI_EQUIPMENT,nSPos,TYPE_GOODS,GoodsGUID,dwAmount);
                    }
                } 
            }
        }
    }
        
    return true;
}


void SetEquipEvent(CPlayer::eEquipIndex index)
{
    const char * wndName = strEquipToWin[index].c_str();
    if(GetWndMgr().isWindowPresent(wndName) == false)
        return;
    CEGUI::Window *pWnd = GetWndMgr().getWindow(wndName);
    if (pWnd->testClassName("DragContainer"))
    {
        pWnd->subscribeEvent(CEGUI::DragContainer::EventDragStarted,CEGUI::Event::Subscriber(UpdateMainPlayerHand));
        pWnd->subscribeEvent(CEGUI::DragContainer::EventMouseButtonDown,CEGUI::Event::Subscriber(ChangeEquip));
        pWnd->subscribeEvent(CEGUI::DragContainer::EventDragDropItemDropped,CEGUI::Event::Subscriber(ChangeEquip));
    }
}

void UpdateEquipmentIcon(CPlayer::eEquipIndex index)
{
    const char * wndName = strEquipToWin[index].c_str();
    if(GetWndMgr().isWindowPresent(wndName) == false)
        return;
    CEGUI::Window *pWnd = GetWndMgr().getWindow(wndName);
    if (pWnd->testClassName("DragContainer"))
    {
        CEGUI::DefaultWindow *deWnd = WDefaultWindow(pWnd->getChildAtIdx(0));
        CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
        if(pMainPlayer == NULL) return;
        CGoods *pEquip = pMainPlayer->GetEquip(index);
        if(pEquip)
        {
            long lGoodIndex = pEquip->GetIndex();
            long lconID = CGoodsList::GetProperty(lGoodIndex)->BaseProperty.dwIconId;
            const char *strIconPath = GetGame()->GetPicList()->GetPicFilePathName(CPicList::PT_GOODS_ICON,lconID);
            char strImageFilePath[128] = "";
            char strImageFileName[128] = "";
            GetFilePath(strIconPath,strImageFilePath);
            GetFileName(strIconPath,strImageFileName);
            CEGUI::String strImagesetName = "GoodIcon/";
            strImagesetName += strImageFileName;
            SetBackGroundImage(deWnd,strImagesetName.c_str(),strImageFilePath,strImageFileName);
        }
        else
            deWnd->setProperty("Image","");
    }
}

bool OnUpdateArribute(const CEGUI::EventArgs &args)
{
    //角色名字
    CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
    CEGUI::Window *p3Dwin = GetWndMgr().getWindow("TabGroup/Tab 1/PlayerModel/Model");
    UIManager::Get3DIconEffect(p3Dwin)->SetIconShape(pMainPlayer);

    CEGUI::DefaultWindow * pNameWin = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/PlayerName"));
    const char * name = pMainPlayer->GetName();
    pNameWin->setText(ToCEGUIString(name));

    //家族
    const char * familyName = pMainPlayer->GetFactionName().c_str();
    CEGUI::DefaultWindow * pFacionNameWin = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/FamilyName"));
    pFacionNameWin->setText(ToCEGUIString(familyName));

    //阵营
    BYTE CountryID = GetGame()->GetMainPlayer()->GetCountry();
    long lconID  = CCountryList::GetCountry(CountryID)->lIconID;
    const char * strIconPath = GetGame()->GetPicList()->GetPicFilePathName(CPicList::PT_COUNTRY_ICON,lconID);

    CEGUI::DefaultWindow *pCampWin = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/CampIcon"));
    char strImageFilePath[128] = "";
    char strImageFileName[128] = "";
    GetFilePath(strIconPath,strImageFilePath);
    GetFileName(strIconPath,strImageFileName);
    CEGUI::String strImagesetName = "CountryIcon/";
    strImagesetName += strImageFileName;
    const CEGUI::Image &pGoodIcon = SetBackGroundImage(pCampWin,strImagesetName.c_str(),strImageFilePath,strImageFileName);
    float width  = pGoodIcon.getSourceTextureArea().getWidth();
    float height = pGoodIcon.getSourceTextureArea().getHeight();
    pCampWin->setSize(CEGUI::UVector2(cegui_absdim(width),cegui_absdim(height)));

    //称号
    const char *strTitle = pMainPlayer->GetTitle();
    CEGUI::Combobox *pTitleCombo = WComboBox(GetWndMgr().getWindow("TabGroup/Tab 1/Title"));
    if(pTitleCombo->getItemCount())
        pTitleCombo->resetList();
    CEGUI::ListboxTextItem *itm = NULL;
    const CEGUI::Image* sel_img = &CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage("MultiListSelectionBrush");
    itm = new CEGUI::ListboxTextItem(strTitle, 0);
	/***********************************************************************/
	/* zhaohang fix 2010-9-3
	/* 修改设置图片方式
	/***********************************************************************/
    itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
    pTitleCombo->addItem(itm);

    /*--------------------装备Icon---------------------*/
    for(long i= CPlayer::EQUIP_HEAD;i < CPlayer::EQUIP_SUM;i++)
        UpdateEquipmentIcon((CPlayer::eEquipIndex)i);  

    /*---------------------基本属性------------------------*/
    char Desp[128] = "";
    //等级
    CEGUI::DefaultWindow *pLevelWin = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/Level"));
    long lLev = pMainPlayer->GetLevel();
    sprintf_s<128>(Desp,"%d",lLev);
    pLevelWin->setText(ToCEGUIString(Desp));

    //道行
    CEGUI::DefaultWindow *pDaoWin = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/Ability"));

    //阵营
    pCampWin = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/CampName"));
    long lLeague  = CCountryList::GetCountry(CountryID)->nLeague;
    const char * Decription = 0;
    if (lLeague == 0)
    {
        Decription = AppFrame::GetText("Player_231"); //联邦
        pCampWin->setText(ToCEGUIString(Decription));
    } 
    else
    {
        Decription = AppFrame::GetText("Player_232");   //帝国
        pCampWin->setText(ToCEGUIString(Decription));
    }
    //阴德
    CEGUI::DefaultWindow *pYindeWin = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/yingde"));

    //流派
    CEGUI::DefaultWindow *pLiuPaiWin = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/LiuPai"));

    //煞气
    CEGUI::DefaultWindow *pShaqiWin = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/ShaQi"));

    //精力
    CEGUI::ProgressBar *pEnergeWin = WProgress(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/energy"));
    DWORD lEnergy = pMainPlayer->GetEnergy();
    DWORD lMaxEnergy = pMainPlayer->GetMaxEnergy();
    float curVal = 0;
    if(lMaxEnergy != 0)
        curVal = (float)lEnergy / (float)lMaxEnergy;
    pEnergeWin->setProgress(curVal);
    sprintf_s<128>(Desp,"%d/%d",lEnergy,lMaxEnergy);
    pEnergeWin->setText(ToCEGUIString(Desp));
    //贡献
    CEGUI::DefaultWindow *pContribute = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/contribute"));
    long lContribute = pMainPlayer->GetCountryContribute();
    sprintf_s<128>(Desp,"%d",lContribute);
    pContribute->setText(ToCEGUIString(Desp));
    //体力
    CEGUI::ProgressBar *pPhysicalWin = WProgress(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/PhysicalForce"));
    long lCurPhysical = pMainPlayer->GetStamina();
    long lMaxPhysical = pMainPlayer->GetMaxStamina();
    curVal = 0;
    if (lMaxPhysical != 0)
    {
        curVal = float(lCurPhysical)/float(lMaxPhysical);
    }
    pPhysicalWin->setProgress(curVal);
    sprintf_s<128>(Desp,"%d - %d",lCurPhysical,lMaxPhysical);
    pPhysicalWin->setText(ToCEGUIString(Desp));
    //恶名
    CEGUI::DefaultWindow *pNotoriety = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/contribute"));
    long lNoteriety = pMainPlayer->GetPVPValue();
    sprintf_s<128>(Desp,"%d",lNoteriety);
    pNotoriety->setText(ToCEGUIString(Desp));
    //气血
    CEGUI::ProgressBar *pQiXue= WProgress(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/QiXue"));

    //法力
    CEGUI::ProgressBar *pMagic= WProgress(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/Magic"));
    long lMp    = pMainPlayer->GetMp();
    long lMaxMp = pMainPlayer->GetMaxMp();
    sprintf_s<128>(Desp,"%d/%d",lMp,lMaxMp);
    curVal = 0;
    if(lMaxMp!=0)
        curVal = float(lMp) / float(lMaxMp);
    pMagic->setProgress(curVal);
    pMagic->setText(ToCEGUIString(Desp));

    //内力
    CEGUI::ProgressBar *pNeili= WProgress(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/Neili"));

    //怒气
    CEGUI::ProgressBar *pNuqi= WProgress(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/Nuqi"));

    /*加点属性*/
    //力量
    CEGUI::DefaultWindow *pPower = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/Power"));
    long lPower = pMainPlayer->GetStr();
    sprintf_s<128>(Desp,"%d",lPower);
    pPower->setText(ToCEGUIString(Desp));

    CEGUI::DefaultWindow *pPowerPoint = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/PowerPoint"));

    //身法
    CEGUI::DefaultWindow *pShenfa = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/shenfa"));

    CEGUI::DefaultWindow *pShenfaPoint = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/shenfaPoint"));

    //根骨
    CEGUI::DefaultWindow *pGengu= WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/GenGu"));

    CEGUI::DefaultWindow *pGenguPoint = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/GenGuPoint"));
    //意志
    CEGUI::DefaultWindow *pWill= WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/Will"));

    CEGUI::DefaultWindow *pWillPoint = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/WillPoint"));

    //灵性
    CEGUI::DefaultWindow *pLingXin= WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/lingxin"));

    CEGUI::DefaultWindow *pLingXinPoint = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/lingxinPoint"));
    //潜力
    CEGUI::DefaultWindow *pQianli= WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/qianli"));

    /*攻防属性*/
    //物伤
    CEGUI::DefaultWindow *pPhyHurt = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/PhyHurt"));
    long PhyMinAtt = pMainPlayer->GetMinAtk();
    long PhyMaxAtt = pMainPlayer->GetMaxAtk();
    sprintf_s<128>(Desp,"%d - %d",PhyMinAtt,PhyMaxAtt);
    pPhyHurt->setText(ToCEGUIString(Desp));

    //物理防御
    CEGUI::DefaultWindow *pPhyDef = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/phyDef"));
    long PhyDef = pMainPlayer->GetDef();
    sprintf_s<128>(Desp,"%d",PhyDef);
    pPhyDef->setText(ToCEGUIString(Desp));

    //法术伤害
    CEGUI::DefaultWindow *pMagicHurt = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/magicHurt"));
    long lMagicHurt = pMainPlayer->GetMAtk();
    sprintf_s<128>(Desp,"%d",lMagicHurt);
    pMagicHurt->setText(ToCEGUIString(Desp));
    //法术防御
    CEGUI::DefaultWindow *pMagicDef = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/MagicDef"));
    long lMagixDef = pMainPlayer->GetMdef();
    sprintf_s<128>(Desp,"%d",lMagixDef);
    pMagicDef->setText(ToCEGUIString(Desp));
    //命中
    CEGUI::DefaultWindow *pHitRate = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/HitRate"));
    long lHitRate = pMainPlayer->GetHit();
    sprintf_s<128>(Desp,"%d",lHitRate);
    pHitRate->setText(ToCEGUIString(Desp));
    //闪避
    CEGUI::DefaultWindow *pDodge = WDefaultWindow(GetWndMgr().getWindow("TabGroup/Tab 1/BaseBalueTab/Property/dodge"));
    long lPdodge = pMainPlayer->GetDodge();
    sprintf_s<128>(Desp,"%d",lPdodge);
    pDodge->setText(ToCEGUIString(Desp));

    //经验值
    CEGUI::ProgressBar *pExpBar= WProgress(GetWndMgr().getWindow("TabGroup/Tab 1/LevBar"));
    long lMaxExp = pMainPlayer->GetUpgradeExp();
    long lCurExp = pMainPlayer->GetExp();
    curVal = 0;
    if(lMaxExp!=0)
        curVal = (float)lCurExp/(float)lMaxExp;
    pExpBar->setProgress(curVal);
    sprintf_s(Desp,"%d/%d",lCurExp,lMaxExp);
    pExpBar->setText(ToCEGUIString(Desp));
    //修为
    CEGUI::ProgressBar *pXiuwei= WProgress(GetWndMgr().getWindow("TabGroup/Tab 1/XiuWei"));
    return true;
}


// 模型旋转
bool Rot3DModel(const CEGUI::EventArgs &args)
{
    CEGUI::Window *pClickBtn = static_cast<const CEGUI::WindowEventArgs&>(args).window;
    CEGUI::Window *p3Dwin = GetWndMgr().getWindow("TabGroup/Tab 1/PlayerModel/Model");
    C3DIconEffect *p3DIcon = UIManager::Get3DIconEffect(p3Dwin);
    float RotAngle = p3DIcon->GetRotAngle();
    if(pClickBtn->getName() == "TabGroup/Tab 1/PlayerModel/RotLeft")
    {
        p3DIcon->SetLeftRot(true);
        p3DIcon->SetRightRot(false);
    }
    else
    {
        p3DIcon->SetLeftRot(false);
        p3DIcon->SetRightRot(true);
    }
    return true;
}

bool StopRotation(const CEGUI::EventArgs &args)
{
    CEGUI::Window *p3Dwin = GetWndMgr().getWindow("TabGroup/Tab 1/PlayerModel/Model");
    C3DIconEffect *p3DIcon = UIManager::Get3DIconEffect(p3Dwin);
    p3DIcon->SetLeftRot(false);
    p3DIcon->SetRightRot(false);
    return true;
}