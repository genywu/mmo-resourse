//NPC对话框逻辑层
#pragma once

class NpcTalkBox
{
public:
    struct ButtonInfo
    {
        string BtnName;
        bool   isEnable;
        ButtonInfo():BtnName(""),isEnable(true){}
    };
    struct PicInfo
    {
        ulong   lID;
        string  fileName;
        PicInfo():lID(0),fileName(""){}
    };
    
    NpcTalkBox():strTalkContext(""),PicID(0),PicPosX(0),PicPosY(0),SoundID(0),BKMusicID(0){LoadTalkBoxImageRes();}
    ~NpcTalkBox(){}

    void SetTalkContext(const char * strText) {strTalkContext = strText;}

    const char * GetTalkContext()   {return strTalkContext.c_str();}

    void  SetScriptID(CGUID guid)    {ScriptID = guid;}

    CGUID  &GetScriptGUID()          {return ScriptID;}

    void SetPicID(DWORD id)        {PicID = id;}

    DWORD GetPicID()                {return PicID;}

    void  SetPicPosX(DWORD x)       {PicPosX = x;}

    DWORD GetPicPosX()              {return PicPosX;}

    void SetPicPosY(DWORD y)       {PicPosY = y;}

    DWORD GetPicPosY()              {return PicPosY;}

    void SetSoundID(DWORD id)       {SoundID = id;}

    DWORD GetSoundID()              {return SoundID;}

    void SetBKMusicID(DWORD id)       {BKMusicID = id;}

    DWORD GetBKMusicID()            {return BKMusicID;}

    ButtonInfo &GetBtnInfo(int index)   {return tagBtnInfo[index];}

    //读取图片资源
    void LoadTalkBoxImageRes();

    vector<PicInfo> &GetPicResList()    {return PicResList;}

    void SetGoodsSelectedIndex(BYTE index)     {SelectGoodsIndex = index;}

    BYTE GetGoodsSelectedIndex()        {return SelectGoodsIndex;}
private:
    //内容
    string strTalkContext;
    //脚本ID
    CGUID  ScriptID;
    //图片ID
    DWORD  PicID;
    //显示坐标
    DWORD  PicPosX;

    DWORD  PicPosY;
    //音效ID
    DWORD  SoundID;
    //背景音乐
    DWORD  BKMusicID;
    //物品选择ID
    BYTE  SelectGoodsIndex;
    //按钮
    ButtonInfo tagBtnInfo[2];
    //解析字符串的图片集
    vector<PicInfo>     PicResList;
};