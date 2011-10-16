#pragma once

#include "../../PageEvent.h"
class CPlayer;
class GameScene;
class CREvent:public PageEvent
{
public:
    CREvent();
    CREvent(EVENTTYPE type);
    ~CREvent();
    virtual void OnPageLoad(GamePage *pPage);
    virtual void OnPageOpen(GamePage *pPage);
    virtual void OnPageClose(GamePage *pPage);
    virtual void OnPageUpdate(GamePage *pPage);
    virtual void OnPageRelease(GamePage *pPage);
	//显示模型
	void Display();

    bool OnCreateRoleBtn(const CEGUI::EventArgs &e);    //创建角色按钮逻辑
    bool GoBackBtn(const CEGUI::EventArgs &e);          //回到上页
    static void SetRandCountryRange(BYTE lRange);       //设定国家随机范围
	static void SetRandCountry( BYTE nCountry ){ m_nRandCountry=nCountry; }		//设置随机国家
	static void SetSelectSex(short chSex);                              // 设置角色性别
	void SetConstellation(bool bLeft);			//设定星座
	static void SetFace(DWORD dwIndex);         //设定脸型
	static void SetHairStyle(DWORD dwIndex);    //设定发型
	static void SetHairColor(DWORD dwIndex);    //设定发色 
	static void SetSelectCountry(BYTE country){nCountry = country;}	            // 设置被选中的国家

	static short GetSelectSex(){return m_chSex;}					    // 获得角色性别
	static short GetHairColorNum(WORD wSexIndex,WORD wHairIndex);   //获取人物的发色数量（男0、女1）
	//获取人物的面部特征数量（男0、女1）
	static short GetFaceNum(WORD wSexIndex);
	static short GetHairNum(WORD wSexIndex);
	static BYTE GetConstellation()	{ return m_nConstellation;}		                        //得到当前用户选择的星座信息     
	static BYTE GetHair()			{ return (m_byHairStyle << 4) | m_byHairColor; }		//得到当前用户选择的头发信息
	static BYTE GetFace()			{ return m_byFace; }										//得到当前用户选择的脸型信息

	bool          m_bRoleLeftRotate;    //向左旋转
	bool          m_bRoleRightRotate;   //向右旋转

private:
    static short m_wFaceNum[2];		// 人物的面部特征数量（男0、女1）
    static short m_wHairNum[2];		// 人物的发型数量（男0、女1）
    static short nSelected;         //选中Player索引
    static BYTE nCountry;          //国家ID
    static short m_chSex;           //性别
    static BYTE  m_byHairStyle;     //发型
    static BYTE  m_byHairColor;     //发色
    static BYTE  m_byFace;           //脸型
    static BYTE  m_nConstellation;  //星座
    static CPlayer *m_pPlayer;      //待创建人物角色
    static BYTE  m_nRandCountry;    //随即国家ID

    static map<WORD,short> m_mapHairColorNum[2];//人物的发色数量（男0、女1）
    static vector<BYTE> m_vecRandCountryRange;//国家随机范围

    static bool LoadFaceHairIni();			// 读取面部特征、头发及发色的配置文件
 
    static bool CheckName(const char* strName);					                //检查人物名字是否满足命名规则
    static short GetSelectedNum(){return nSelected;}					        // 获得要被创建的角色
    static BYTE GetSelectCountry(){return nCountry;}					        // 获得选中的国家
    static BYTE GetRandCountry(){return m_nRandCountry;}				        // 获得随机的国家
    

    static void RandomChoseDetails();           //随机人物细节
    static void RandomChoseCountry();			//随机选择国家

	static float  s_RolePos[4];		// 站的位置
	static int    m_CurrentAction;		// 当前动作
	static int    m_NextAction;           // 下一个动作
	GameScene*	  m_SelectSence;   
};
