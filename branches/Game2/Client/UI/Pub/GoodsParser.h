
/*[GOOD] 标记解析 2010*5.19*/
#pragma once
namespace
{
    CEGUI::String;
    CEGUI::Window;
    CEGUI::Imageset;
}
class GoodsParser
{
public:
    GoodsParser();
    ~GoodsParser();

    struct GoodsInfo 
    {
        //物品原始名
        CEGUI::String GoodsOriName;
        //物品数量
        DWORD Num;
        //是否能选中
        bool  bCanBeSelect;
        //服务器识别ID
        DWORD ID;
        GoodsInfo():GoodsOriName(""),Num(0),bCanBeSelect(false),ID(0){}
    };

    CEGUI::String Parse(const CEGUI::String &str);

    bool OnGoodsIconClicked(const CEGUI::EventArgs &e);

    static void RemoveGoodInfoFromWindow(CEGUI::Window *Wnd);
private:
    typedef map<CEGUI::Window *, GoodsInfo>    mapGoodInfo;
    static mapGoodInfo     GoodsMap;
    //物品图标选中的外框
    static CEGUI::Imageset *         ImageSel;
};