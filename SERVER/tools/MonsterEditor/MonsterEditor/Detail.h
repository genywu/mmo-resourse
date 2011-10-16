#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDetail 对话框

class CDetail : public CDialog
{
	DECLARE_DYNAMIC(CDetail)

public:
	//设置控件值
	void SetData(vector<tagMonster>*vecMonster,vector<tagMonster>::iterator iter);
	//在listcontrol填充技能记录
	void FillData();
	//保存文件
	bool Save();
	//新建记录时设计默认值
	bool SetNewData(vector<tagMonster>*vecMonster,size_t n);
	//判断是否有重复记录
	bool IsSimple();
	//获取脚本文件路径
	CString GetScript();
	CDetail(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDetail();

// 对话框数据
	enum { IDD = IDD_DETAIL };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeCombo7();
	afx_msg void OnClose();
	int nIDCLevel;	//等级编辑框
	CListCtrl m_MonSkillList;//技能列表
	DWORD dwIDCExp;	//经验编辑框
	DWORD dwIDCWorkPoint;//职业点数
	DWORD dwIDCMaxHp;//最大HP编辑框
	DWORD dwIDCMinAttack;//最小攻击力
	DWORD dwIDCMaxAttack;//最大攻击力
	DWORD dwIDCMinMagicAttack;//最小魔法攻击
	DWORD dwIDCMaxMagicAttack;//最大魔法攻击
	DWORD dwIDCHitRating;//命中率
	DWORD dwIDCStormHit;//物理暴击
	DWORD dwIDCMagicStorm;//魔法暴击
	DWORD dwIDCDefense;//防御力
	DWORD dwIDCMagicDefense;//魔法防御力
	DWORD dwIDCBlock;//格档
	DWORD dwIDCParry;//招架
	DWORD dwIDCDodge;//闪避
	float fIDCMoveSpd;//移动速度
	float fIDCBatMoveSpd;//战斗移动速度
	CString strIDCMonRace;//怪物种族
	CString strIDCElem;//元素类型
	DWORD dwIDCVitality;//活力
	DWORD dwIDCHpRecoverSpd;//HP恢复速度
	DWORD dwIDCMpRecoverSpd;//MP恢复速度


	CString strIDCAIModel;//AI模式
	CString strIDCBlock;//是否阻挡
	CString strIDCSuper;//是否无敌
	CString strIDCTame;//可否驯服
	CString bIDCTalk;//可否对话
	CString strIDCReturn;//是否飞回
	CString strIDCAllRecover;//是否完全恢复
	DWORD dwIDCReDistance;//飞回距离
	DWORD dwIDCReTime;//返回时间
	double fIDCInjurRate;//重伤比率
	DWORD dwIDCAlertTime;//警戒时间
	double fIDCEscape;//逃跑底线
	DWORD dwIDCSkillRate;//技能概率
	DWORD dwIDCSkillInter;//技能间隔
	DWORD dwIDCTameTimes;//最大驯服次数
	DWORD dwIDCMinStayTimes;//最小停留时间
	DWORD dwIDCMaxStayTimes;//最大停留时间
	DWORD dwIDCMinWalkTimes;//最小行走次数
	DWORD dwIDCMaxWalkTimes;//最大行走次数
	DWORD dwIDCAlertRange;//警戒范围
	DWORD dwIDCPurRange;//追击范围
	DWORD dwIDCPeaMoveRange;//和平移动范围
	DWORD dwIDCStayTime;//停留时间
	CString strIDCAIWay;//AT挂起方式
	CString strIDCNO;//自动号
	DWORD dwIDCIndex;//索引
	CString strIDCOrigName;//原始名
	CString strIDCName;//怪物名
	CString strIDCCollection;//采集物名
	CString strIDCTitle;//称谓
	DWORD dwIDCTileColor;//称谓颜色
	DWORD dwIDCTitlePicture;//称谓图片
	CString strIDCCountryID;//国家id
	CString strIDCNpcSort;//NPC类别
	DWORD dwIDCPicID;//图片id
	DWORD dwIDCPicLevel;//图片等级
	DWORD dwIDCNameColorID;//称谓颜色id
	DWORD dwIDCLevelColorID;//等级颜色

	DWORD dwIDCHpColor;//血条颜色
	DWORD dwIDCSoundID;//声音id
	DWORD dwIDCHurtHuman;//对人形生物伤害
	DWORD dwIDCHurtElem;//对元素生物伤害
	DWORD dwIDCHurtDiff;//对异族生物伤害
	DWORD dwIDCHurtAnimal;//对动物伤害
	DWORD dwIDCHurtDead;//对亡灵伤害
	DWORD dwIDCHurtDevil;//对魔物伤害
	DWORD dwIDCHurtInsect;//对昆虫伤害
	DWORD dwIDCStormHurt;//物理暴击伤害
	DWORD dwIDCMagicStormHurt;//魔法暴击伤害
	DWORD dwIDCHurtFire;//对火系怪物伤害
	DWORD dwIDCHurtWater;//对水系怪物伤害
	DWORD dwIDCHurtGround;//对土系怪物伤害
	DWORD dwIDCHurtWind;//对风系怪物伤害
	DWORD dwIDCHurtSacre;//对神圣系怪物伤害
	DWORD dwIDCHurtDark;//对黑暗系怪物伤害
	DWORD dwIDCHurtNotElem;//对无元素系怪物伤害
	DWORD dwIDCGroundAnti;//地元素抗性
	DWORD dwIDCWaterAnti;//水元素抗性
	DWORD dwIDCFireAnti;//火元素抗性
	DWORD dwIDCWindAnti;//水元素抗性
	DWORD dwIDCSacreAnti;//神圣抗性
	DWORD dwIDCDarkAnti;//黑暗抗性
	DWORD dwIDCNotElemAnti;//无元素抗性
	DWORD dwIDCStormAnti;//物理暴击抗性
	DWORD dwIDCMagicStormAnti;//魔法暴击抗性
	DWORD dwIDCWillAnti;//意志抗性
	DWORD dwIDCBodyAnti;//体质抗性
	DWORD dwIDCBloodAnti;//流血抗性
	DWORD dwIDCDuskAnti;//击晕抗性
	DWORD dwIDCNumbAnti;//麻痹抗性
	DWORD dwIDCSleepAnti;//睡眠抗性
	DWORD dwIDCDrugAnti;//中毒抗性
	DWORD dwIDCNotMoveAnti;//定身抗性
	CString strIDCRefe1;//ai参考1
	CString strIDCRefe2;//ai参考2
	CString strIDCRefe3;
	CString strIDCRefe4;
	CString strIDCRefe5;
	CString strIDCBirthSct;//出生脚本
	CString strIDCBattleSct;//战斗脚本
	CString strIDCInjurSct;//重伤脚本
	CString strIDCDeadSct;//死亡脚本
	CString strIDCCycleSct;//周期脚本
	CString strIDCChoseSkillSct;//选择技能脚本
	CString strIDCFindTgtSct;//寻找目标脚本
	CString strIDCKillTgtSct;//杀死目标脚本
	CString strIDCShape;//体形
	//添加技能
	afx_msg void OnAppendSkill();

	afx_msg void OnEnChangeEdit26();
	// 技能类型
	CString strIDCSkillType;
	// 技能ID
	DWORD dwIDCSkillID;
	// 技能等级
	DWORD dwIDCSkillLevel;
	// 技能间隔，基本技能、普通技能没有，则为0
	DWORD dwIDCInterTime;
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_combox;
	afx_msg void OnUpdateSkill();
	afx_msg void OnDeleteSkill();
	//添加脚本的按钮
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();
};
