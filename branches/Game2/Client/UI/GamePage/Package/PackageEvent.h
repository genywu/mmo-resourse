#pragma once

#include "../../PageEvent.h"
#include "../../../../Public/Module/GUID.h"
#include "../../../GameClient/ClientApp/Player.h"

#define GRID_H_ITEM 5							//横排5个道具格子
#define GRID_V_ITEM 4							//竖排4个道具格子
#define GRIDITEM	GRID_H_ITEM*GRID_V_ITEM		//主背包的格子的总数
#define GRIDPACK	5							//子背包栏总数
#define GRID_V_SUBPACK	6						//子背包最大行数

#define GRID_H_INTEG 8							//整合背包界面的横排格子数
#define GRID_V_INTEGMAXLINE	15					//整合背包的最大行数
#define GRIDINTEGMAXNUM  GRID_H_INTEG*GRID_V_INTEGMAXLINE	//整合背包的最大格子数
namespace CEGUI
{
    class DragContainer;
}

class PackageEvent:public PageEvent
{
public:
    PackageEvent(){}
    PackageEvent(EVENTTYPE type);
    ~PackageEvent();
    virtual void OnPageLoad(GamePage *pPage);
    virtual void OnPageOpen(GamePage *pPage);
    virtual void OnPageClose(GamePage *pPage);
    virtual void OnPageUpdate(GamePage *pPage);
    virtual void OnPageRelease(GamePage *pPage);

    void SetUpdatePos(int lPos){m_UpdateGoodsPos = lPos;}

    void SendChangeEquipMSG(int nIndex,int nSPos,int nExtendID,int nSExtendID);
    
    int  GetGuidMouseHover() {return m_UpdateGoodsPos;}
private:
    /*
    当前需要更新的物品位置
    */
    int m_UpdateGoodsPos;

private:
    void SendPutDownMes(long SorceType/*源类型*/,const CGUID& SorceGUID/*源对象的GUID*/,long lSorceObjectExtendID,long lSorceObjectPos,
        long DesType,const CGUID& DesGUID,long lDesObjectExtendID,long lDesObjectPos,long lObjectType,const CGUID& ObjectGUID,long lObjectNum);

    //处理右键点击
    bool ProcessMouseRBtnDown(const CEGUI::EventArgs &args);

    //得到当前操作的的格子索引
    long GetCurGridPos(CEGUI::DragContainer *pDrag);

	/// 右键响应物品输入
	bool OnRBClickGoodsForOtherUIPages(CPlayer::tagGoods* pstGoods);

    bool handle_GoodsDropped(const CEGUI::EventArgs& args); //放下物品

    bool handle_GoodsPickUp(const CEGUI::EventArgs& args);  //拾取物品

    bool handle_PageClose(const CEGUI::EventArgs& args);    //关闭界面

	bool handle_OpenPlayerShop(const CEGUI::EventArgs& args);	// 打开个人商店

    bool updateMoney(const CEGUI::EventArgs & args);        //刷新金钱

    bool OnMouseEnter(const CEGUI::EventArgs & args);       //鼠标移动到背包格子UI上

    bool OnMouseLeave(const CEGUI::EventArgs & args);       //鼠标移动到背包格子UI上
};

