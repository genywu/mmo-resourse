#include "stdafx.h"
#include "WSEvent.h"
#include "../Login/LoginPage.h"
#include "WorldServerPage.h"
#include "../../UIDef.h"
#include "../../../GameClient/Game.h"
map<long,WSEvent::tagWorldInfo>		WSEvent::m_WorldInfos;
map<long,string>                    WSEvent::WS_stateToImageSetName;
#define  WSSTATECONFIG "datafiles/imagesets/selectWS/ws_StateToImageSetName.xml"
WSEvent::WSEvent(EVENTTYPE type)
{
    SetEventType(type);
}

void WSEvent::OnPageLoad(GamePage *pPage)
{
    pPage->LoadPageWindow();
    //创建界面的资源组
    CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>
                                        (CEGUI::System::getSingleton().getResourceProvider());
    
    rp->setResourceGroupDirectory(UIRes, "datafiles/imagesets/selectWS");
   
    //创建服务器状态图片资源
    LoadServerImageSet();
}

void WSEvent::OnPageOpen(GamePage *pPage)
{
    CEGUI::Window *pWsPageWin = pPage->GetPageWindow();

    CEGUI::PushButton *pBackBtn = static_cast<CEGUI::PushButton*>(pWsPageWin->getChild("BackToLogin"));
    pBackBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&WSEvent::OnBackToLoginBtn, this));
    pBackBtn->setText(ToCEGUIString(AppFrame::GetText("SelctRole_2")));
    CEGUI::PushButton *pEnterBtn = static_cast<CEGUI::PushButton*>(pWsPageWin->getChild("EnterSelectRole"));
    pEnterBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&WSEvent::OnEnterBtnDown, this));
    pEnterBtn->setText(ToCEGUIString(AppFrame::GetText("Login_1")));
    CEGUI::Listbox    *pServerList = static_cast<CEGUI::Listbox*>(pWsPageWin->getChild("ServerList"));
    pServerList->subscribeEvent(CEGUI::Listbox::EventMouseDoubleClick,CEGUI::Event::Subscriber(&WSEvent::OnEnterBtnDown, this));
}

void WSEvent::OnPageClose(GamePage *pPage)
{
    
}

void WSEvent::OnPageUpdate(GamePage *pPage)
{
    CEGUI::Window *pWsWindow = pPage->GetPageWindow();
    CEGUI::Listbox *pWsList = static_cast<CEGUI::Listbox*>(pWsWindow->getChild("ServerList"));
    //选中项背景条图片
    //const CEGUI::Image* sel_img = &CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage("MultiListSelectionBrush");
    const CEGUI::Image* cov_img = &CEGUI::ImagesetManager::getSingleton().get(IMAGES_FILE_NAME).getImage(CHECKBOXMARK);
    //先清除列表中所以的项目
    pWsList->resetList();
    char  strParseImage[256];
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    wsIter iter = m_WorldInfos.begin();
    long lListWidth = (long)pWsList->getListRenderArea().getWidth();
    for (int i = 0;iter != m_WorldInfos.end(); ++iter,i++)
    {
        CEGUI::String ItemText = iter->second.strWSName;
        long lStateID = iter->second.lState;

        long ImageWidth = (long)CEGUI::ImagesetManager::getSingleton().get(
            WS_stateToImageSetName[lStateID]).getImage("full_image").getWidth();
        long lPaddingLeft = lListWidth - ImageWidth - 40;
        sprintf_s(strParseImage,"[left-padding = '%d'][image = 'set:%s image:full_image']",
            lPaddingLeft,WS_stateToImageSetName[lStateID].c_str());
        //创建一个列表中的ITEM
        CEGUI::ListboxTextItem *pItem = new CEGUI::ListboxTextItem(ItemText+=strParseImage,i);
        //设置选中时的背景条
        pItem->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME); 
        //pItem->setCoverBrushImage(cov_img);
        //添加到列表中
        pWsList->addItem(pItem);
        
    }
   // CEGUI::System::getSingleton().signalRedraw();
}

void WSEvent::OnPageRelease(GamePage *pPage)
{
    
}

void WSEvent::AddWorldInfo(long lID,char* strWorldName,long lState)
{
    if(strWorldName == NULL)
        return;
    tagWorldInfo WorldInfo = {strWorldName,lState};
    m_WorldInfos[lID] = WorldInfo;
}

bool WSEvent::OnBackToLoginBtn(const CEGUI::EventArgs &e)
{
    GetGame()->SetGameState(GS_LOGIN);
    GetGame()->CloseNetClient();
    m_WorldInfos.clear();
    return true;
}
bool WSEvent::OnEnterBtnDown(const CEGUI::EventArgs &e)
{
    const static float DELAYTIME = 2000;
    static DWORD lastConnetTime = 0;
    if (GetCurTickCount() - lastConnetTime < DELAYTIME)
        return false;
    else
        lastConnetTime = GetCurTickCount();
    CEGUI::Window *pWsWindow = GetInst(WorldServerPage).GetPageWindow();
    CEGUI::Listbox *pWsList = static_cast<CEGUI::Listbox*>(pWsWindow->getChild("ServerList"));
    long lID = -1;
    for(size_t i = 0;i< pWsList->getItemCount();i++)
    {
        if(pWsList->isItemSelected(i))
        {
            lID = (long)i;
            break;
        }
    }
    if (lID != -1)
    {
        map<long,tagWorldInfo>::iterator it = m_WorldInfos.find(lID);
        string wsName = it->second.strWSName;
        GetGame()->C2L_PlayerBase_Send(wsName.c_str(),GetInst(WorldServerPage).GetCDkey());
        return true;
    }
    return false;
}

void WSEvent::LoadServerImageSet()
{
    WS_stateToImageSetName.clear();

    // 解析 XML 配置文件
    TiXmlNode *pNode = NULL;
    TiXmlDocument m_Tdoc;

    CRFile *prfile= rfOpen(WSSTATECONFIG);
    if(prfile==NULL)
    {
        MessageBox( 0, "Load ws_StateToImageSetName.xml file error!", "selectWS", MB_OK );
        return;
    }
    if(!m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()))
    {
        MessageBox( 0, "Load ws_StateToImageSetName.xml file error!", "selectWS", MB_OK );
        return;
    }
    rfClose(prfile);

    // 找到根节点，如果无节点，输出错误提示
    if( !( pNode = m_Tdoc.FirstChild("WS_State")))
    {
        MessageBox( 0, "Can't find the WS_State node in ws_StateToImageSetName.xml file!", "selectWS", MB_OK );
        return;
    }
    // 遍历子节点
    TiXmlElement* pElem = pNode->ToElement();
    for( TiXmlElement *pChildElem = pElem->FirstChildElement(); pChildElem;
        pChildElem = pChildElem->NextSiblingElement() )
    {
        long lStateID = CStrConverter::parseInt( pChildElem->Attribute( "StateID" ) );
        string ImageSetName = pChildElem->Attribute( "ImageSetName" );
        string FileName = pChildElem->Attribute( "ImageFileName" );
        WS_stateToImageSetName[lStateID] = ImageSetName;
        CEGUI::ImagesetManager::getSingleton().createFromImageFile
            (ImageSetName, FileName,UIRes).setAutoScalingEnabled(false);
    }
}