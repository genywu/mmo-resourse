#include "stdafx.h"
#include "GoodsParser.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/Other/NpcTalkBox.h"
#include "../../GameClient/ClientApp/Other/PicList.h"
#include "../../GameClient/ClientApp/Goods/GoodsList.h"
GoodsParser::mapGoodInfo GoodsParser::GoodsMap;
CEGUI::Imageset * GoodsParser::ImageSel = 0;
GoodsParser::GoodsParser()
{
    if (NULL == ImageSel)
    {
        CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>
            (CEGUI::System::getSingleton().getResourceProvider());
        CEGUI::String ResGroup = "talkbox";
        rp->setResourceGroupDirectory(ResGroup, "datafiles/imagesets/Public");

        //创建服务器状态图片资源
        ImageSel = &CEGUI::ImagesetManager::getSingleton().createFromImageFile("GoodsIconSel", "icon_00A2FF.tga",ResGroup);
    }
}

GoodsParser::~GoodsParser()
{

}

CEGUI::String GoodsParser::Parse(const CEGUI::String &str)
{
    size_t parentWinPos,originNamePos,NumPos,SelPos,IDPos,endPos;
    parentWinPos = originNamePos = NumPos = SelPos = IDPos = endPos = CEGUI::String::npos;
    char ParentWinName[128] = "";
    char originGoodsName[128] = "";
    char canbeSelect[32] = "";
    char Num[32] = "";
    char ID[32] = "";
    static CEGUI::String temp;
    parentWinPos = str.find("WIN:");
    originNamePos = str.find("ORINAME:");
    NumPos = str.find("NUM:");
    SelPos = str.find("SEL:");
    IDPos  = str.find("ID:");
    CEGUI::String wndName = "Goods_Static_image";
    static DWORD GoodCounter = 0;
    wndName += CEGUI::PropertyHelper::intToString(GoodCounter++);
    CEGUI::Window *pStaticImage = NULL;
    GoodsInfo Goods;
    //如果无此window，就创建staticimage类型的。
    if(CEGUI::WindowManager::getSingleton().isWindowPresent(wndName) == false)
    {
        pStaticImage = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage",wndName);
        //解析父窗口
        if (parentWinPos != CEGUI::String::npos)
        {
            temp = str.substr(parentWinPos+5);

            endPos = temp.find("'");
            strcpy_s<128>(ParentWinName,temp.substr(0,endPos).c_str());
            CEGUI::Window *pParentWnd = CEGUI::WindowManager::getSingleton().getWindow(ParentWinName);
            pParentWnd->addChildWindow(pStaticImage);
            
            //解析物品原始名
            if (originNamePos != CEGUI::String::npos)
            {
                temp = str.substr(originNamePos+9);
                endPos = temp.find("'");
                strcpy_s<128>(originGoodsName,temp.substr(0,endPos).c_str());
                Goods.GoodsOriName = originGoodsName;
                DWORD dwIconId = CGoodsList::GetProperty(originGoodsName)->BaseProperty.dwIconId;
                const char *strIconPath = GetGame()->GetPicList()->GetPicFilePathName(CPicList::PT_GOODS_ICON, dwIconId);
                char strImageFilePath[128] = "";
                char strImageFileName[128] = "";
                GetFilePath(strIconPath,strImageFilePath);
                GetFileName(strIconPath,strImageFileName);
                CEGUI::String strImagesetName = "GoodIcon/";
                strImagesetName += strImageFileName;
                //设置物品图片背景
                //static_cast<CEGUI::GUISheet*>(pStaticImage)->SetBackgoundImage("GoodIcon",strImagesetName.c_str(),strImageFilePath,strImageFileName);
            }
            //解析是否能被选择
            if (SelPos != CEGUI::String::npos)
            {
                temp = str.substr(SelPos+4);
                endPos = temp.find(" ");
                strcpy_s<32>(canbeSelect,temp.substr(0,endPos).c_str());
                if(strcmp(canbeSelect,"1") == 0)
                {
                    Goods.bCanBeSelect = true;
                    //如果能被选择，则需要在pStaticImage组件上再创建一个用于点中后显示外框的组件
                    CEGUI::String selWndName = wndName+ "Sel";
                    CEGUI::Window *pSelWnd = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage",selWndName);
                    pStaticImage->addChildWindow(pSelWnd);
                    pSelWnd->setSize(pStaticImage->getSize());
                    pSelWnd->setProperty("FrameEnabled", "false");
                    pSelWnd->setProperty("BackgroundEnabled", "false");
                    pSelWnd->subscribeEvent(CEGUI::Window::EventMouseClick,CEGUI::Event::Subscriber(&GoodsParser::OnGoodsIconClicked,this));
                }
            }
            //解析文字描述
            if (NumPos != CEGUI::String::npos)
            {
                temp = str.substr(NumPos+4);
                endPos = temp.find(" ");
                strcpy_s<32>(Num,temp.substr(0,endPos).c_str());
                pStaticImage->setText(Num);
                Goods.Num = CEGUI::PropertyHelper::stringToInt(Num);
            }
            //解析物品ID
            if(IDPos != CEGUI::String::npos)
            {
                temp = str.substr(IDPos+3);
                endPos = temp.find(" ");
                strcpy_s<32>(ID,temp.substr(0,endPos).c_str());
                Goods.ID = CEGUI::PropertyHelper::stringToInt(ID);
            }
        }
        GoodsMap[pStaticImage] = Goods;
    }
    return wndName;
}

bool GoodsParser::OnGoodsIconClicked(const CEGUI::EventArgs &e)
{
    CEGUI::Window *pWnd = static_cast<const CEGUI::WindowEventArgs&>(e).window;
    CEGUI::Window *pParent = pWnd->getParent();
    CEGUI::String img_property = pWnd->getProperty("Image");
    NpcTalkBox *pTalkBox = GetGame()->GetNpcTalkBox();
    if (img_property == "")
    {
        pWnd->setProperty("Image","set:GoodsIconSel image:full_image");
        pTalkBox->SetGoodsSelectedIndex((BYTE)GoodsMap[pParent].ID);
    }
    else
    {
        pWnd->setProperty("Image","");
        pTalkBox->SetGoodsSelectedIndex(0);
    }
    return true;
}

void GoodsParser::RemoveGoodInfoFromWindow(CEGUI::Window *Wnd)
{
    mapGoodInfo::iterator iterInfo = GoodsMap.begin();
    while(iterInfo != GoodsMap.end())
    {
        CEGUI::Window *pChildWin = iterInfo->first;

        if (Wnd == pChildWin->getParent())
        {
            pChildWin->destroy();
            iterInfo = GoodsMap.erase(iterInfo);
            continue;
        }
        ++iterInfo;
    }
}