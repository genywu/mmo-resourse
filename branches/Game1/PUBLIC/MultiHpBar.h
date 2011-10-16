////////////////////////////////////////////////////////////
//     文件名   MultiHpBar.h
//	   作者		heliangchen
//	   时间		08/8/25
//	   功能介绍： 新添怪物多血条功能，实现可添加多个血条，
//				  每个血条分别有hp值， 颜色值， 和触发脚本（服务器用）
///////////////////////////////////////////////////////////
#pragma once
class CMultiHpBar
{
	struct HpBar
	{
		//记录血条范围
		int HpRange;
		//触发状态
		BOOL  SpringState;
		//脚本名字（服务器），该血条消失时被触发
		string ScriptName;
		HpBar();
	};
public:
	struct HpBarInfo
	{
		//血条显示率
		float Scale;
		//当前血条的总雪量
		int HpRange;
		//hp在多血条所处的等级
		int Level;
		//触发脚本名
		const char* ScriptName;
		HpBarInfo(float scale,int hprange,int lev,const char*csript)
			:Scale(scale),HpRange(hprange),Level(lev),ScriptName(csript)
		{}
	};
public:
	CMultiHpBar();
	~CMultiHpBar();
	//添加一个血条
	bool Clean();
	bool AddHpBar(int AddHp, const char* scriptname);
	bool AddDatatoByteArray(vector<BYTE>* pByteArray);
	bool DecordDataFromByteArray(BYTE* pByte, long& pos);

	bool CodeToDataBlock(DBWriteSet& setWriteDB);
	bool DecodeFromDataBlock(DBReadSet& setReadDB);

	int  GetMaxHp();
public:
	//获取当前的状态： 传入参数 Hp 当前hp； 获值参数level 当前等级； 获取参数collor 当前颜色； state 触发状态
	//返回值 当前触发的脚本函数名，没有返回NULL；
	HpBarInfo GetCurrentInfo(int Hp,int ChangeHp=0);
	void SetBaseMaxHp(int BaseMaxHp);
private:
	//血条列表
	long m_BaseMaxHp;
	list<HpBar> m_listHpBar;
};