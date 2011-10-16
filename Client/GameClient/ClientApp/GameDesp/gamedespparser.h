#pragma once
/*
    功能：解析物品描述配置文件，为界面生成属性描述组合字串
*/
class CGoods;
class GameDespParser
{
public:
    GameDespParser();
    ~GameDespParser();
    enum    SOURCE_UI
    {
        SI_PACKAGE = 1,
        SI_SHOP,
        //....
    };
    struct tagParse
    {
        string  Desp;   //描述字串
        string  strAtt;  //属性枚举
        int     val;    //附加属性取值
        string  ImgInfo;  //图片信息
        tagParse():val(0){}
    };

    // 读取配置文件(data/GoodsDescription.xml)
    bool  LoadDespCfg();

    /*解析物品*/
    /*para：isSelf 是否是mainPlayer的*/
    const char *ParseGoods(CGoods *pGoods,bool isSelf);
    
    /*解析技能*/
    const char *ParseSkill(int skillID, int iskillLv);

    SOURCE_UI   GetSourceUI(){return sourceUI;}
    void        SetSourceUI(SOURCE_UI name){sourceUI = name;}
private:
    const char *AppendImage(const char *ImageFileNamePath,long d_width = 0,long d_height = 0);
    const char *AppendGoodIcon(string &ImageSetProperty);
    const char *AppendText(const char *text);

    string strPARSETEXT;
    vector<tagParse>    tagGoodsVector;
    vector<tagParse>    tagSkillVector;
    //基本属性识别串
    static char BaseAtt[5][64];

    SOURCE_UI           sourceUI;
};