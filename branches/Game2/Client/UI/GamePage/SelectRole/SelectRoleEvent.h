#pragma once
#include "../../PageEvent.h"
class CPlayer;
class C3DIconEffect;
class SREvent:public PageEvent
{
public:
    SREvent();
    SREvent(EVENTTYPE type);
    ~SREvent(){}
    virtual void OnPageLoad(GamePage *pPage);
    virtual void OnPageOpen(GamePage *pPage);
    virtual void OnPageClose(GamePage *pPage);
    virtual void OnPageUpdate(GamePage *pPage);
    virtual void OnPageRelease(GamePage *pPage);
    //处理创建角色按钮
    bool OnCreateRoleBtn(const CEGUI::EventArgs &e);

    //删除角色按钮处理
    bool OnDeleteRoleBtn(const CEGUI::EventArgs &e);

    //进入游戏Gameserver
    bool OnEnterGameBtn(const CEGUI::EventArgs &e);

    //回到上页处理
    bool GoBackBtn(const CEGUI::EventArgs &e);

    //模型左旋转
    bool TurnLeftBtn(const CEGUI::EventArgs &e);

    //模型右旋转
    bool TurnRightBtn(const CEGUI::EventArgs &e);

    //停止旋转
    bool StopRotation(const CEGUI::EventArgs &e);

	bool UpdateDelRemainDays(const CEGUI::EventArgs& e);

    void AddPlayer(CPlayer *pPlayer);

    void ClearPlayer();

    size_t GetPlayerCount()   {return m_vecPlayer.size();}

    CPlayer *GetPlayer(ulong index);

    CPlayer *GetSelPlayer();
    
    ulong GetSelectedIndex() {return m_SelectIndex;}
private:

    //当前账号下玩家基本信息容器
    static vector<CPlayer*>     m_vecPlayer;

    //当前选择的角色
    static ulong                m_SelectIndex;

public:
    // 初始物品和装备列表
    struct tagOrginEquip
    {
        BYTE	lOccupation;			// 职业
        WORD	wPlacePos;				// 放置位置
        string	strOrginName;			// 物品原始名
    };
   

    list<tagOrginEquip>     m_listOrginEquip;   // 初始装备列表
};
