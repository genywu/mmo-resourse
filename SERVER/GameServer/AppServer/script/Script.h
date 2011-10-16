#ifndef __SCRIPT__H__
#define __SCRIPT__H__
#pragma once


//	Script System
//	Create in 2002/7/22

/************************************************
*	-= 脚本类 =-								*
*	读入并执行脚本文件							*
*	（１）用以控制NPC的行为						*
*	（２）编写特定的情节						*
*	（３）制作任务								*
*	（４）动态改变游戏中的各种数据［巫师］		*
************************************************/

/********************************************************************************
*	脚本系统的运行机制:															*
*	　　　　　　　　　　　　　													*
*	　　　　　　　　　　　　　　以指令形式通知客户机　							*
*	　　　　　　　　　　　　　└──────────┘							*
*	　│　　　　　　　　　　　　　　　　↑										*
*	┌┴┐　　　　　　　　　　（Ｙ）　　│　　　　　　　　　　　（Ｙ）			*
*	│主│──→下一个脚本实例──→执行一条指令─→脚本是否完成──→释放脚本	*
*	│　│　↑　　　　│　　　　　　　　　　　　　　　　　│　　　　　　│		*
*	│循│　│　　　　│（Ｎ）　　　　　　　　　　　　　　│（Ｎ）　　　│		*
*	│　│←┼────┘　　　　　　　　　　　　　　　　　│　　　　　　│		*
*	│环│　└──────────────────────┘　　　　　　│		*
*	│　│←──────────────────────────────┘		*
*	└┬┘																		*
*	　│																		*
*																				*
*	-= 说明 =-																	*
*																				*
*	　　在服务器有一脚本链表，每个游戏循环周期系统会依次访问一遍这个链表的所有	*
*	元素，并执行一步操作（发送指令、检测上条指令是否完成、清除该脚本实例……）	*
*	　　服务器为每个新的脚本执行请求建立一个脚本实例，并将它加入到脚本链表中。	*
*																				*
********************************************************************************/

#include <string>
#include <map>
using namespace std;

#include "..\public\GUID.h"

#define ERROR_CODE		0x09fffff9	//错误代码编号
#define MAX_OPERATION	23			//操作符数量
#define MAX_VAR			32			//一行语句中的最大元素数量
#define MAX_VAR_LEN		3072		//最大的元素长度

#define MAX_STACK_CMD_LEN	256	//最大的元素长度

class CVariableList;
class CPlayer;
class CNpc;
class CRegion;
class CBaseModule;
class GameFaction;

enum SCRIPT_FUNC_DEF
{
	SR_FUNC_WAIT					= 6,
	SR_FUNC_GAMEMESSAGE				= 100,
	SR_FUNC_TALKBOX					= 2307,
	SR_FUNC_HELP					= 101,
	SR_FUNC_GETPLAYER				= 102,
	SR_FUNC_ISPLAYERONLINE			= 3003,
	SR_FUNC_GETREGIONID				= 103,
	SR_FUNC_LISTBANEDPLAYER			= 104,
	SR_FUNC_GETONLINEPLAYER			= 105,
	SR_FUNC_CREATEPRGN				= 106,
	SR_FUNC_CREATEPHRGN				= 107,
	SR_FUNC_CREATETRGN				= 108,
	SR_FUNC_GETPLAYERPDUPRGNID		= 109,
	SR_FUNC_GETPLAYERPHDUPRGNID		= 110,
	SR_FUNC_GETTEAMDUPRGNID			= 111,
	SR_FUNC_ADDRGNVAR				= 112,
	SR_FUNC_SETRGNVAR				= 113,
	SR_FUNC_DELRGNVAR				= 114,
	SR_FUNC_GETRGNVAR				= 115,
	SC_FUNC_DELETERGN				= 116,
	SC_FUNC_GETMPLATEIDBYGUID		= 117,
	RGN_FUNC_GETDUPTYPEBYGUID		= 118,
	SR_FUNC_GETDUPRGNONWERGUID		= 119,
	SC_FUNC_GETPHRGNGUIDBYONWERNAME	= 120,
	SR_FUNC_GETDUPRGNNUM			= 121,
	SR_FUNC_GETGUIDBYPLAYERNAME		= 123,
	SR_FUNC_GETPGUIDBYPLAYERNAME	= 124,
	SR_FUNC_GETPHGUIDBYPLAYERNAME	= 125,
	SR_FUNC_GETTGUIDBYPLAYERNAME	= 126,
	SC_FUNC_VIEWRGNVAR				= 127,
	SR_FUNC_VIEWDUPRGNDATA			= 128,
	SR_FUNC_HALTTHISSCRIPT			= 129,
	SR_FUNC_VIEWRGNVALIDBYGUID		= 130,
	SR_FUNC_UPDATEVARVALUE			= 131,
	SR_FUNC_VIEWGSPLAYERNUM			= 132,
	SR_FUNC_GETTEAMRGNIDNUM			= 133,
	SR_FUNC_GETTEAMRGNIDBYINDEX		= 134,
	SR_FUNC_GETTEAMRGNTEMPLATIDBYINDEX	= 135,
	SR_FUNC_REFRESHPLAYERDUPRGNLIST = 136,
	SR_FUNC_ADDFACRES				= 137,
	SR_FUNC_ADDFACEXP				= 138,
	SR_FUNC_ADDFACMEMBERCONTRIBUTE	= 139,
	SR_FUNC_USESKILLSYNC			= 140,
	SR_FUNC_USESKILLSYNCONCELL		= 141,
	SR_FUNC_INLVLDIFFERENCE			= 142,
	SR_FUNC_INLVLSPACE				= 143,
	SR_FUNC_GETPLAYERREGION			= 144,
	SR_FUNC_CHANGEPLAYERREGION		= 145,
	SR_FUNC_REGIONISEXIT			= 146,
	SR_FUNC_SILENCEPLAYER			= 147,
	SR_FUNC_UPDATENAME				= 148,
	SR_FUNC_RELIVEWINDOW			= 149,
	SR_FUNC_RELIVEGOODWINDOW		= 150,
	SR_FUNC_ADDFACTICKETS			= 151,
	SR_FUNC_ADDBASE					= 152,
	SR_FUNC_ADDBATTLE				= 153,
	SR_FUNC_ADDARMYINVEST			= 154,
	SR_FUNC_CHANGEJOINBATTLE		= 155,
	SR_FUNC_ADDBASEBATTLE			= 156,
	SR_FUNC_ADDTECHLV				= 157,
	SR_FUNC_ADDENEMY				= 158,
	SR_FUNC_GETRGNCAMPPLAYERNUM		= 159,
	SR_FUNC_ISSENTAISYS				= 160,
};

// 脚本类
// 注：每条语句最大不得超过16个元素，每个元素最大为128字节
class CScript : public BaseMemObj
{
public:
	//脚本执行返回
	enum SCRIPTRETURN
	{
		SR_OK=0,		//正常
		SR_HANG,		//脚本挂起
		SR_END,			//结束
		SR_RETURN,		//return标志
		SR_FORBREAK,   //for结构跳出标志
		SR_FORCONTINUE,//for结构的continue

		SR_ERROR,		//错误	
	};


#ifdef _SCRIPT_INFO_
public:

	DWORD			m_RunTime;
#endif

private:


	// 变量类型定义
	enum VAR_CLASS
	{
		VC_CONST=0,		//常量
		VC_VAR,			//简单变量
		VC_ARRAY,		//数组变量
		VC_STRING,		//字符串变量
		VC_FUNCTION,	//函数
		VC_BRACKET,		//小括号
		VC_GUID,        //GUID变量
	};

	// 操作符结构
	struct stOperation
	{
		char *Name;		//符号
		int Level;		//优先级 ( 0 -> 4 )
	};

	//子脚本类型
	typedef enum	eChildScriptType
	{
		CST_FOR=0,//For结构类型
		CST_CALL,//call调用的子脚本
		CST_CALLEX,//callEx调用的子脚本
	}eCST;
	//记录了脚本调用的栈信息
	struct tagChilScriptInfo : public BaseMemObj
	{
		eCST	eType;//子脚本类型
		char pszPreOp[MAX_STACK_CMD_LEN];//调用子脚本之前的预操作脚本数据
		char pszCon[MAX_STACK_CMD_LEN];	//条件命令,调用子脚本的条件脚本数据
		CScript *pChildScript;//子脚本
	};


	typedef double (CScript::*CallFunc)(const char* CmdStr, char* retStr);

	typedef map<string, int>        STRING2INT;
	typedef map<string, CallFunc>   STRING2PFUNC;

	// 函数结构
	static STRING2PFUNC     *s_FunctionList2;   // 函数列表

	static double m_LoadFunctionNum;						//函数列表读入次数记数

	static double m_MaxFunction;							//函数数量
	static stOperation m_OperationList[MAX_OPERATION];	//运算符号列表


	// 递归返回脚本的变量列表, 只能在CGame::Release()接口释放
	static CVariableList* g_StackVarList;

	//在一个MainLoop周期里标记要删除的脚本
	static list<CGUID>	m_DelScripts;

public:



	static void LoadFunction(const char *, char* pData);			//读入函数列表
	static void ReleaseFunction();							//释放函数列表

	static CVariableList* GetStackVarListInst(void);
	static void ReleaseStackVarListInst(void);

	bool m_bHang;									//是否挂起
	int	m_nHangFunc;							//挂起的函数编号
	DWORD m_nWaitTime;								//等待结束时间
	DWORD m_dwLastTime;								//最后一次计时
	string	m_strRunScriptName;						//计时器满后执行的的脚本名字

	static void PushDelScript(const CGUID& guid);
	//删除标记的脚本
	static void DelScript();

	const char* GetClientUpdateVarName(void) { return m_szClientUpdateVarName; }
	void SetClientUpdateVarName(const char* szName) { if(szName)strcpy_s(m_szClientUpdateVarName, 64, szName); }
private:
	CGUID		m_guIndexID;				//该校本的索引ID
	CRegion*	m_pRegion;				//指向场景的指针（一定存在）
	CGUID		m_guRegionID;				//场景ID
	// [070621 AHC]
	// 事件源对象
	CShape*     p_SrcShape;
	long		m_lSrcType;
	CGUID		m_guSrcID;
	// 事件目的对象
	CShape*     p_DesShape;
	long		m_lDesType;
	CGUID		m_guDesID;

	CGUID		m_PlayerGuid;			//指向Player的ID
	CGUID		m_guUsedItemID;			//使用物品触发脚本，记录该物品的GUID
	
	POINT m_ptDropGoods;			//物品掉落位置。。。。


	tagChilScriptInfo* m_pChildScriptInfo;	//子脚本栈信息(在脚本中产生call,for等调用时,会产生栈信息)

	char		m_szClientUpdateVarName[64]; // 客户端修改的变量名字

	char*		m_szClientOptInfo;
	long		m_lClientOptInfoLen; // m_szClientOptInfo的长度
public:
	void	SetID(const CGUID& guid)			{m_guIndexID=guid;}
	const CGUID&	GetID()				{return m_guIndexID;}
	
	void	SetRegion(CRegion* r);
	CRegion	*GetRegion()				{return m_pRegion; }

	void	SetSkillEx(CBaseModule *pSkill);

	void    SetSrcShape(CShape* shape);
	void	SetSrcShapeEx(CShape* pShape);
	CShape* GetSrcShape(void)           { return p_SrcShape; }
	long	GetSrcType()	{return m_lSrcType;}
	const CGUID&	GetSrcID()			{ return m_guSrcID;}

	void    SetDesShape(CShape* shape);
	CShape* GetDesShape(void)           { return p_DesShape; }

	void ResetShape();//重新设置

	void	SetFireShapeEx(CShape* shape);

	void SetIsHang(bool b)	{m_bHang=b;}
	bool IsHang()			{return m_bHang;}
	void SetHangFunc(int n)	{m_nHangFunc=n;}
	int GetHangFunc()		{return m_nHangFunc;}

	void	SetPlayerID(const CGUID& PlayerGuid)	{ m_PlayerGuid = PlayerGuid; }
	const CGUID&	GetPlayerID()				{return m_PlayerGuid;}

	void	SetUsedItemID(CGUID& guid)	{m_guUsedItemID = guid;}
	CGUID& GetUsedItemID()		{return m_guUsedItemID;}

	bool IsGM();					// 判定当前玩家是否是GM。
	void SetDropGoodsPoint(POINT& pt) {m_ptDropGoods = pt;};
	POINT& GetDropGoodsPoint()	{return m_ptDropGoods;}

	double GetTrueNpcID(double id);		// 返回实际的NPC的ID

	void ResetDataPos(void)
	{
		m_Point = 0;//当前指针
		m_Line=m_Word=0;		//当前行列
	}

	const char* GetClientOptInfo(void) { return m_szClientOptInfo; }
	void SetClientOptInfo(const char* szInfo);
private:
	// -文件-
	char m_FileName[MAX_PATH];		//脚本文件名
	char *m_Data;					//脚本内容
	int m_DataLen;					//脚本长度

	int m_Point;					//当前指向
	int m_Line, m_Word;				//当前指针的行列
	int m_Block;					//程序块 '{' ++   '}' --

	// -指令-
	int m_VarNum;							//变量块数
	enum VAR_CLASS m_VarClass[MAX_VAR];		//变量类型
	string m_Var[MAX_VAR];					//变量名

	//union{

	double m_Value[MAX_VAR];				//变量的值
	//GUID变量指针值(将RunFunction()返回的long指针进行赋值拷贝),用于RunScript返回值

	// 记录变量值的指针和长度
	struct tagStrValueInfo
	{
		tagStrValueInfo(void);
		~tagStrValueInfo(void);
		char* pStrValue;
		long lStrValueLen;
	};

	tagStrValueInfo m_strValueInfo[MAX_VAR];			//变量的值(字符串指针和长度)
	
	CGUID m_guidValue[MAX_VAR];			//GUID变量的值(GUID指针)

	//};
	int m_Operation[MAX_VAR];				//操作符

	CVariableList* m_pSelfVariableList;		//脚本实例拥有的脚本变量
	CVariableList* m_pVariableList;			//脚本运行中使用的脚本变量列表,可以是m_pSelfVariableList
private:
	// -工具-
	int GetValueType(const char* pValue);			//获取一个量的类型(数字,字符串)
	int GetValueClass();		//获取一个变量的类型(数字,字符串,数组)
	static int DumpString(char* &pDest,long lDesLen, const char* pSrc);	//将pSrc字符串复制到pDest,处理内存重新分配等各种问题,返回字符串长度
	double Modulus(long ModulusType,long OccuTypeA,long OccuTypeB, long LevelA, long LevelB);

	int GotoNextLine();						//转到下一行
	int JumpTo(const char *);						//跳到指定的位置
	int JumpToNextBlock();					//跳到下一块 { }

	// -脚本指令-
	void Check(const char *);			//分解
	bool ComputeVar(int);		//计算一个变量的值
	int Count();				//计算

	bool IsOperation(char);		//是否是操作符
	bool IsChar(char);			//是否是变量
	int OperationNum(const char *);	//得到操作符编号

	int GetOperation(int num) { return m_Operation[num]; };	//获取操作符编号
	const char *GetVar(int num) { return m_Var[num].c_str(); };	//获取变量名
	int GetValue(int num) {	return m_Value[num]; };			//获取变量的值

	friend CVariableList;

	int Prew(int num);			//前一个可用的数值变量
	int PrewString(int num);	//前一个可用的字符串变量
	int FindHighest();			//找出最高优先级

	// -函数-
	char *GetFunctionName(const char *, long& lLen);		//获取函数名称
	double GetIntParam(const char *, int);			//获取整形参数
	char *GetStringParam(const char *, int);	//获取字符串参数
	char *GetOperation(const char *, int);		//取得某个分隔符
	bool IsCompart(char);						//是否是分隔符

	//记录进入子脚本
	void EnterChildScript(eCST eType,CScript* pChildScript,const char* pszPreOp,const char* pszCon);
	void LeaveChildScript(CScript* pChildScript);

public:

	// -对外接口-
	CScript(CShape* srcShape=NULL, CRegion *Region=NULL, CShape *desShape=NULL, CGUID& guUsedItemID = CGUID::GUID_INVALID, DWORD dwDiedMonsterIndex=0);	//构建
	virtual ~CScript();									//释放
	void ReleaseChildScript();//释放子脚本
	//初始化子脚本数据
	void InitChildScriptData(CScript *pChildScript);

	char *GetData() { return m_Data; }		//返回文件内容
	bool LoadScript(const char *, char* = NULL);	//读入一个脚本文件
	bool ReadCmd(char *strRet, int MaxLen=MAX_VAR_LEN);	//读入一行指令
	bool ReadForCmd(long& beginPos, long& endPos);	//读入for循环体内的全部脚本
	void RunLine(const char *);					//运行一行脚本
	double RunFunction(const char *, char* retStr=NULL/*返回字符串指针*/);			//运行一条脚本函数
	enum SCRIPTRETURN RunStep();					//执行一步脚本
	enum SCRIPTRETURN ContinueRun(CVariableList* stackVarList);		//异步操作返回后继续执行
	enum SCRIPTRETURN ContinueRunCall(CVariableList* stackVarList);	//异步操作返回后继续执行call子句
	enum SCRIPTRETURN ContinueRunCallEx(CVariableList* stackVarList);	//异步操作返回后继续执行callEx子句
	enum SCRIPTRETURN ContinueRunFor(CVariableList* stackVarList);	//异步操作返回后继续执行for子句
	enum SCRIPTRETURN RunStr(CMoveShape* pDest,const char* pszContent);	//执行一段字符串
	
	CScript* BeginForScript(char* pScriptData = NULL);
	void EndForScript(CScript* forScript);//外部调用脚本
	void ResetChildScriptInfo();
	//long RunScript(CMoveShape* pDest,const char* pszContent);

	char* GetFileName()	{return m_FileName;}
	CVariableList* GetVariableList()	{ return m_pVariableList; }
	CVariableList* GetSelfVariableList()	{ return m_pSelfVariableList; }
	void SetVariableList(CVariableList* pVariableList) {m_pVariableList=pVariableList;}
	void ResetVariableList()	{m_pVariableList = m_pSelfVariableList;}

	double CheckParamStringForArray(const char* CmdStr, long cmdStrSize, char** MainVarName, long MainVarNameSize, char** InnVarName, long InnVarNameSize, long& countPos);

	// 设置变量值
	int SetScriptVarValue(CMoveShape* moveShapePtr/*指定对象的指针*/, const char* varName, const char* value);
	int SetScriptVarValue(CMoveShape* moveShapePtr/*指定对象的指针*/, const char* varName, long lArray=0, double value=0);
	const char* GetScriptVarValue(CMoveShape* moveShapePtr/*指定对象的指针*/, const char* varName);
	double GetScriptVarValue(CMoveShape* moveShapePtr/*指定对象的指针*/, const char* varName, long lArray);

	int SetScriptGuidValue(CMoveShape* moveShapePtr/*指定对象的指针*/, const char* varName, const CGUID& guid);
	const CGUID& GetScriptGuidValue(CMoveShape* moveShapePtr/*指定对象的指针*/, const char* varName);

	// [070608 AHC]
	bool  SetAttr(OBJECT_TYPE type, const CGUID& entityID, std::string& attrName, double value);
	double  GetAttr(OBJECT_TYPE type, const CGUID& entityID, std::string& attrName);
	bool  ChangeAttr(OBJECT_TYPE type, const CGUID& entityID, std::string& attrName, double value);

	// 使用指针设置MoveShape属性
	bool  SetMoveShapeAttrEx(DWORD entityPtr/*指针地址DWORD值*/, std::string& attrName, double value);
	double  GetMoveShapeAttrEx(DWORD entityPtr/*指针地址DWORD值*/, std::string& attrName);
	bool  ChangeMoveShapeAttrEx(DWORD entityPtr/*指针地址DWORD值*/, std::string& attrName, double value);

	int GetVarValue(const char* str,int num);

	//////////////////////////////////////////////////////////////////////////
	//	全局变量
	//////////////////////////////////////////////////////////////////////////
public:
	static CVariableList* GetGeneralVariableList()	{ return m_GeneralVariableList; }

	static void LoadGeneralVariable(BYTE* pByte, long lPos);	// 读入全局变量列表
	static void ReleaseGeneralVariable();					// 释放全局变量列表

	static bool UpdateToWorldServer(const char* strVarName, int pos, double nValue); // 更新"$"到WorldServer
	static bool UpdateToWorldServer(const char* strVarName, const char* szValue);	 // 更新"#"到WorldServer
	static bool UpdateToWorldServer(const char* strVarName, const CGUID& guid);		 // 更新"@"到WorldServer

	static void IniBuffSkillVariableList();

private:
	static CVariableList* m_GeneralVariableList;
	//技能系统用到的的变量，用于战斗计算
	static CVariableList* m_BuffSkillVariableList;

	//[070515 AHC] Function
public:
	//-----------------------------------------------------
	//基本语法函数
	enum SCRIPTRETURN call(const char* cmd, char* retStr=NULL);
	enum SCRIPTRETURN callEx(const char* cmd, char* retStr=NULL);
	enum SCRIPTRETURN  For(const char* cmd, char* retStr=NULL);	
	double Goto(const char* cmd, char* retStr=NULL);
	double If(const char* cmd, char* retStr=NULL);
	double pause(const char* cmd, char* retStr=NULL);
	double wait(const char* cmd, char* retStr=NULL);
	double random(const char* cmd, char* retStr=NULL);
	double Rgb(const char* cmd, char* retStr=NULL);
	double LocalVar(const char* cmd, char* retStr=NULL);	
	double define(const char* cmd, char* retStr=NULL);
	double undefine(const char* cmd, char* retStr=NULL);
	double Ifdef(const char* cmd, char* retStr=NULL);
	double DestDefine(const char* cmd, char* retStr=NULL);
	double DestUndefine(const char* cmd, char* retStr=NULL);
	double IfDestDef(const char* cmd, char* retStr=NULL);
	double Time(const char* cmd, char* retStr=NULL);
	double Year(const char* cmd, char* retStr=NULL);
	double Month(const char* cmd, char* retStr=NULL);
	double Day(const char* cmd, char* retStr=NULL);
	double Hour(const char* cmd, char* retStr=NULL);
	double Minute(const char* cmd, char* retStr=NULL);
	double DayOfWeek(const char* cmd, char* retStr=NULL);
	double HourDiff(const char* cmd, char* retStr=NULL);
	double MinuteDiff(const char* cmd, char* retStr=NULL);
	double Second(const char* cmd, char* retStr=NULL);
	double CheckGuidIsNull(const char* cmd, char* retStr=NULL);
	double GetCurTick(const char* cmd, char *retStr=NULL);
	double TestLog(const char* cmd, char* retStr=NULL);
	double Int(const char* cmd, char* retStr=NULL);
	double RoundNum(const char* cmd, char* retStr=NULL);
	double GetPow(const char* cmd, char* retStr=NULL);
	//! 获取一个字符串的子串
	double GetSubString(const char* cmd, char* retStr=NULL);
	
	//-----------------------------------------------------
	//系统全局函数
	double FormatGUID(const char* cmd, char* retStr=NULL);
	double ListOnlineGM(const char* cmd, char* retStr=NULL);
	double ListSilencePlayer(const char* cmd, char* retStr=NULL);
	double ListBanedPlayer(const char* cmd, char* retStr=NULL);
	double GetOnlinePlayers(const char* cmd, char* retStr=NULL);
	double PostRegionInfo(const char* cmd, char* retStr=NULL);
	double PostRegionInfoByIndex(const char* cmd, char* retStr=NULL);
	double PostWorldInfo(const char* cmd, char* retStr=NULL);
	double PostWorldInfoByIndex(const char* cmd, char* retStr=NULL);
	double PostCountryInfo(const char* cmd, char* retStr=NULL);
	double KickAllPlayer(const char* cmd, char* retStr=NULL);
	double KickRegionPlayer(const char* cmd, char* retStr=NULL);
	double RegisterGlobalTimer(const char* cmd, char* retStr=NULL);
	double UnRegisterGlobalTimer(const char* cmd, char* retStr=NULL);
	double KickPlayer(const char* cmd, char* retStr=NULL);
	double KickAround(const char* cmd, char* retStr=NULL);
	double BanPlayer(const char* cmd, char* retStr=NULL);
	double SilencePlayer(const char* cmd, char* retStr=NULL);
	double GetTextRes		(const char* cmd, char* retStr=NULL);
	double SetPlayerRegion		(const char* cmd, char* retStr=NULL);
	double SetPlayerRegionEx	(const char* cmd, char* retStr=NULL);
	double RollRegionInfo	(const char* cmd, char* retStr=NULL);
	double RollWorldInfo	(const char* cmd, char* retStr=NULL);
	double GetPlayerRegion(const char* cmd, char* retStr=NULL);
	double ChangePlayerRegion(const char* cmd, char* retStr=NULL);
	double ChangePlayerName(const char* cmd, char* retStr=NULL);

	//-----------------------------------------------------
	//所有对象(怪物、采集物、角色)基本使用函数
	double SetAttrInt(const char* cmd, char* retStr=NULL);
	double SetAttrStr(const char* cmd, char* retStr=NULL);
	double SetAttrGuid(const char* cmd, char* retStr=NULL);
	double GetAttr(const char* cmd, char* retStr=NULL);
	double GetScriptAttr(const char* cmd, char* retStr=NULL);
	double ChangeAttr(const char* cmd, char* retStr=NULL);
	double RegisterTimer(const char* cmd, char* retStr=NULL);
	double UnRegisterTimer(const char* cmd, char* retStr=NULL);
	double PlayMotion(const char* cmd, char* retStr=NULL);
	//让目标执行一个脚本
	double DesRunScript(const char* cmd, char* retStr=NULL);
	//------//返回执行者周围一定范围内的玩家数量
	double	GetPlayerNumByAround(const char* cmd, char* retStr=NULL);
	//------//返回执行者周围一定范围内的怪物数量
	double	GetMonsterNumByAround(const char* cmd, char* retStr=NULL);
	//------//返回执行者周围一定范围内的同类怪物数量
	double	GetSameMonsterNumByAround(const char* cmd, char* retStr=NULL);
	double SetDelTime(const char* cmd, char* retStr=NULL);
	double SetDelTimeByGuid(const char* cmd, char* retStr=NULL);
	//取得源的guid
	double GetSrcShapeGUID(const char* cmd, char* retStr=NULL);
	//取得目标guid
	double GetDesShapeGUID(const char* cmd, char* retStr=NULL);
	double ChangeMoveSpeed(const char* cmd, char* retStr=NULL);	
	double ChangeBaseSpeed(const char* cmd, char* retStr=NULL);
	double SetMoveSpeed(const char* cmd, char* retStr=NULL);
	double GetCurrentSpeed(const char* cmd, char* retStr=NULL);
	double GetDestIntVar(const char* cmd, char* retStr=NULL);			//得到目标整型变量的值,$bound默认为0
	double SetDestIntVar(const char* cmd, char* retStr=NULL);		//设置目标整形变量的值,$bound默认为0
	double GetDestStrVar(const char* cmd, char* retStr=NULL);		//得到目标字符串变量的值
	double SetDestStrVar(const char* cmd, char* retStr=NULL);			//设置目标字符串变量的值
	double GetDestGuidVar(const char* cmd, char* retStr=NULL);		//得到目标Guid变量的值
	double SetDestGuidVar(const char* cmd, char* retStr=NULL);			//设置目标变量GUID的值
	double IsShapeValid( const char *cmd, char *retStr = NULL ); // 判断指定GUID对象是否有效

	//-----------------------------------------------------
	//怪物相关函数
	//1、基本函数
	double SetMonsterAttr(const char* cmd, char* retStr=NULL);
	double GetMonsterAttr(const char* cmd, char* retStr=NULL);
	double ChangeMonsterAttr(const char* cmd, char* retStr=NULL);
	double AddHpBar(const char* cmd, char* retStr=NULL);
	double GetHpLevel(const char* cmd, char* retStr=NULL);
	double GetKillerID(const char* cmd, char* retStr=NULL);
	double GetKillerType(const char* cmd, char* retStr=NULL);
	double SetBeneficiary(const char* cmd, char* retStr=NULL);
	double GetBeneficiaryID(const char* cmd, char* retStr=NULL);
	double GetBeneficiaryType(const char* cmd, char* retStr=NULL);
	double GetBeneTeamGuid	(const char* cmd, char* retStr=NULL);
	double DelRegionMonsterByName(const char* cmd, char* retStr=NULL);
	//2、怪物AI相关函数
	double MonsterTalk(const char* cmd, char* retStr=NULL);
	double MonsterTalkByIndex(const char* cmd, char* retStr=NULL);
	double MonsterTalkByGUID(const char* cmd, char* retStr=NULL);
	double MonsterTalkByGuidByIndex(const char* cmd, char* retStr=NULL);
	double UseSkillByID		(const char* cmd, char* retStr=NULL);
	double UseSkillByIDOnCell( const char *cmd, char *retStr=NULL);
	double UseSkillSync( const char *cmd, char *retStr = NULL );
	double UseSkillSyncWithTrace( const char *cmd, char *retStr = NULL );
	double UseSkillOnCellSync( const char *cmd, char *retStr = NULL );
	double MoveToCellOnPeace	(const char* cmd, char* retStr=NULL);
	double MoveToCellOnFight	(const char* cmd, char* retStr=NULL);
	double MoveToCellAround	(const char* cmd, char* retStr=NULL);
	double MoveByStepOnPeace	(const char* cmd, char* retStr=NULL);
	double MoveByStepOnFight	(const char* cmd, char* retStr=NULL);
	double SendExpActOnPeace	(const char* cmd, char* retStr=NULL);
	double SendExpActOnFight	(const char* cmd, char* retStr=NULL);
	double MoveByTimeOnPeace	(const char* cmd, char* retStr=NULL);
	double MovebyTimeOnFight	(const char* cmd, char* retStr=NULL);
	double CallFriendAround	(const char* cmd, char* retStr=NULL);
	double CallMonsterAround	(const char* cmd, char* retStr=NULL);
	double Stand				(const char* cmd, char* retStr=NULL);
	double StandOnce( const char *cmd, char *retStr=NULL );
	double RandomMoveByTime	(const char* cmd, char* retStr=NULL);
	double ChangeAI			(const char* cmd, char* retStr=NULL);
	double StartCycle		(const char* cmd, char* retStr=NULL);
	double EndCycle			(const char* cmd, char* retStr=NULL);
	double EscapeByTimeOnFight(const char* cmd, char* retStr=NULL);
	double DeleteSpring(const char* cmd, char* retStr=NULL);
	double EndEscape(const char* cmd, char* retStr=NULL);
	double EndEscapeByGUID(const char* cmd, char* retStr=NULL);
	double SetBefoolID(const char* cmd, char* retStr=NULL);
	double CleanBefoolID(const char* cmd, char* retStr=NULL);
	double SetForceTarget(const char* cmd, char* retStr=NULL);
	double CleanForceTatget(const char* cmd, char* retStr=NULL);	//
	//脚本接口
	//改变虚弱状态
	double ChangeWeaknessState(const char* cmd, char* retStr=NULL);
	//执行者周围的怪逃跑自己的目标一段时间
	double EscapeByAround(const char* cmd, char* retStr=NULL);
	//怪物执行一个脚本					(ben)
	double	MonsterRunScriptsAround(const char* cmd, char* retStr=NULL);
	//怪物是否是在战斗状态				(ben)
	double	MonsterIsFight(const char* cmd, char* retStr=NULL);
	//设置怪物的状态		//0= 和平， 1= 战斗			(ben)
	double    SetMonsterFight(const char* cmd, char* retStr=NULL);		
	//呼唤周围的同类怪物攻击当前目标		限制状态 0=无限制， 1= 和平， 2= 战斗			(ben)
	double	CallFriendAttack(const char* cmd, char* retStr=NULL);		
	//呼唤周围的所有怪物攻击当前目标		(ben)
	double	CallMonsterAttack(const char* cmd, char* retStr=NULL);			
	//--------//执行者一定范围内的指定原始名怪物攻击指定GUID目标	//@目标GUID可能是玩家也可能是怪物，不填则默认为执行当前脚本的玩家或怪物。(ben)
	double	MonsterAttackObjectByAround(const char* cmd, char* retStr=NULL);

	//指定GUID怪物攻击指定GUID目标	//@攻击者GUID如果填空则默认当前执行者。
	double	MonsterAttackObject(const char* cmd, char* retStr=NULL);
	//改变指定GUID怪物的面向
	double	SetMonsterDir(const char* cmd, char* retStr=NULL);
	//指定GUID怪物进行归位处理
	double	ReLiveMonster(const char* cmd, char* retStr=NULL);
	//返回指定GUID怪物目标的GUID
	double	GetMonsterTarget(const char* cmd, char* retStr=NULL);
	//设置指定GUID怪物的目标
	double	SetMonsterTarget(const char* cmd, char* retStr=NULL);
	// 获取指定怪物的(x,y)坐标
	double GetMonsterPos( const char *cmd, char *retStr = NULL );
	double CreateMonster(const char* cmd, char* retStr=NULL);
	double CreateMonsterEx(const char* cmd, char* retStr=NULL);	
	double DelMonster(const char* cmd, char* retStr=NULL);
	double DelMonsterEx(const char* cmd, char* retStr=NULL);
	double DelMonsterRect(const char* cmd, char* retStr=NULL);
	double KillMonster(const char* cmd, char* retStr=NULL);
	double GetMonsterRefeashTime(const char* cmd, char* retStr=NULL);
	// 返回离自己（怪物）指定距离的玩家GUID
	double GetPlayerByDistance( const char *cmd, char *retStr = NULL );
	// 返回和某GUID玩家之间的距离格数
	double GetDistanceFromPlayer( const char *cmd, char *retStr = NULL );
	// 随机选择一定范围内可攻击的玩家
	double GetAttackablePlayer( const char *cmd, char *retStr = NULL );
	// 随机选择一定范围内指定职业的可攻击玩家
	double ChooseTargetByOccu( const char *cmd, char *retStr = NULL );

	//-----------------------------------------------------
	//采集物相关函数
	double CreateCollectionEx(const char* cmd, char* retStr=NULL);
	double CreateCollection(const char* cmd, char* retStr=NULL);
	double DelCollection(const char* cmd, char* retStr=NULL);
	
	//-----------------------------------------------------
	//建物相关函数
	//对门的操作
	double SetDoor(const char* cmd, char* retStr=NULL);
	double GetDoor(const char* cmd, char* retStr=NULL);
	//-----------------------------------------------------
	//角色相关函数
	//1.基本函数
	double ChangeMe(const char* cmd, char* retStr=NULL);
	double GetMe(const char* cmd, char* retStr=NULL);
	double SetMe(const char* cmd, char* retStr=NULL);
	double ChangeDest(const char* cmd, char* retStr=NULL);
	double GetDest(const char* cmd, char* retStr=NULL);
	double SetDest(const char* cmd, char* retStr=NULL);
	double SetPlayerPos(const char* cmd, char* retStr=NULL);
	double SetPlayerDir(const char* cmd, char* retStr=NULL); 
	double AddAreaCreditByAreaId(const char* cmd, char* retStr=NULL);
	double AddAreaCredit(const char* cmd, char* retStr=NULL);
	double SetAreaCredit(const char* cmd, char* retStr=NULL);
	double SetAreaCreditByAreaId(const char* cmd, char* retStr=NULL);
	double GetAreaCredit(const char* cmd, char* retStr=NULL);
	double GetAreaCreditLevel(const char* cmd,char* retStr=NULL);
	double GetOrganCreditLevel(const char* cmd,char* retstr=NULL);
	double AddOrganCredit(const char* cmd,char* retstr=NULL);
	double SetOrganCredit(const char* cmd,char * retsre=NULL);
	double GetCountry(const char* cmd, char* retStr=NULL);
	double ChangeCountry(const char* cmd, char* retStr=NULL);
	double GetContribute(const char* cmd, char* retStr=NULL);
	double SetContribute(const char* cmd, char* retStr=NULL);
	double IsCharged(const char* cmd, char* retStr=NULL);
	double SetCharged(const char* cmd, char* retStr=NULL);
	double GetName(const char* cmd, char* retStr=NULL);
	double ChangePlayer(const char* cmd, char* retStr=NULL);
	double SetPlayer(const char* cmd, char* retStr=NULL);
	double GetPlayer(const char* cmd, char* retStr=NULL);
	double SetPlayerLevel(const char* cmd, char* retStr=NULL);
	double IsPlayerOnline(const char* cmd, char* retStr=NULL);
	double ChangeMoneyByName(const char* cmd, char* retStr=NULL);
	double GetMoneyByName(const char* cmd, char* retStr=NULL);
	double SetMoneyByName(const char* cmd, char* retStr=NULL);
	double ChangeSilverByName(const char* cmd,char* retStr=NULL);
	double GetSilverByName(const char* cmd,char* retStr=NULL);
	double SetSilverByName(const char* cmd,char* retStr=NULL);
	double AddTicketByName(const char* cmd, char* retStr=NULL);
	double GetTicketByName(const char* cmd, char* retStr=NULL);
	double SetPlayerExp(const char* cmd, char* retStr=NULL);
	double ChangePlayerExp(const char* cmd, char* retStr=NULL);
	double SetPlayerOccuExp(const char* cmd, char* retStr=NULL);
	double ChangePlayerOccuExp(const char* cmd, char* retStr=NULL);
	double GetPlayerOccuExp(const char* cmd, char* retStr=NULL);
	double SetPlayerOccuLvl(const char* cmd, char* retStr=NULL);
	double GetPlayerOccuLvl(const char* cmd, char* retStr=NULL);
	double SetPlayerOccuSp(const char* cmd, char* retStr=NULL);
	double ChangePlayerOccuSp(const char* cmd, char* retStr=NULL);
	double GetPlayerOccuSp(const char* cmd, char* retStr=NULL);
	double ChangePlayerOccuExpSp(const char* cmd, char* retStr=NULL);
	double IsGM(const char* cmd, char* retStr=NULL);
	double ShowPlayerInfo(const char* cmd, char* retStr=NULL);
	double ShowPlayerGoods(const char* cmd, char* retStr=NULL);
	double ShowGeneralVariable(const char* cmd, char* retStr=NULL);
	double DurancePlayer(const char* cmd, char* retStr=NULL);
	double Onstate(const char* cmd, char* retStr=NULL);
	double GetActive(const char* cmd, char* retStr=NULL);
	double AddActive(const char* cmd, char* retStr=NULL);
	double TakeActive(const char* cmd, char* retStr=NULL);
	double ResetActive(const char* cmd, char* retStr=NULL);
	double GetServer(const char* cmd, char* retStr=NULL);
	double CheckReliveByOrigin(const char* cmd, char* retStr=NULL);
	double CheckReliveGoods(const char* cmd, char* retStr=NULL);
	//! 得到结婚二人队伍中对方的GUID
	double GetOtherOneInMarriageTeam(const char* cmd, char* retStr=NULL);
	//! 得到玩家结婚步骤
	double GetPlayerMarriageStep(const char* cmd, char* retStr=NULL);
	//! 设置玩家结婚步骤
	double SetPlayerMarriageStep(const char* cmd, char* retStr=NULL);
	//! 设置结婚对象
	double SetPlayerSpouse(const char* cmd, char* retStr=NULL);
	//! 删除结婚对象
	double DelPlayerSpouse(const char* cmd, char* retStr=NULL);
	//! 得到对象ID
	double GetPlayerSpouse(const char* cmd, char* retStr=NULL);
	//! 得到对象名字
	double GetPlayerSpouseName(const char* cmd, char* retStr=NULL);

	//! 获取元素属性
	double GetElementProperty(const char* cmd, char* retStr=NULL);
	//! 设置元素属性
	double SetElementProperty(const char* cmd, char* retStr=NULL);
	//设置玩家活力值倍率
	double SetActiveRatio(const char* cmd, char* retStr=NULL);
	double GetActiveRatio(const char* cmd, char* retStr=NULL);
    //! 设置玩家稳定挂机时间
    double SetStableHookTime(const char* cmd, char* retStr=NULL);
    double AddStableHookTime(const char* cmd, char* retStr=NULL);
    double GetStableHookTime(const char* cmd, char* retStr=NULL);
	//获取玩家活力值倍率
	//2.-------扩展函数-------
	double GetOccuValueRatio(const char* cmd, char* retStr=NULL);
	double GetOccuLvlCoe(const char* cmd, char* retStr=NULL);
	double ChangeOccu(const char* cmd, char* retStr=NULL);
	double CheckLevel(const char* cmd, char* retStr=NULL);
	double CheckOccuLvl(const char* cmd, char* retStr=NULL);
	double ReLive(const char* cmd, char* retStr=NULL);
	double ScriptIsRun(const char* cmd, char* retStr=NULL);
	double RemoveScript(const char* cmd, char* retStr=NULL);
	double AutoMove(const char* cmd, char* retStr=NULL);
	double MovePlayer(const char* cmd, char* retStr=NULL);
	double GodMode(const char* cmd, char* retStr=NULL);
	double ResidentMode(const char* cmd, char* retStr=NULL);
	
	//-----------------------------------------------------
	//物品相关函数
	double AddGoods(const char* cmd, char* retStr=NULL);
	double AddArmament(const char* cmd, char* retStr=NULL);
	double AddArmamentForGM(const char* cmd, char* retStr=NULL);
	double DelGoods(const char* cmd, char* retStr=NULL);
	double AddGoodsToRgn(const char* cmd, char* retStr=NULL);
	double CheckGoods(const char* cmd, char* retStr=NULL);
	double CheckGoodsInBank(const char* cmd, char* retStr=NULL);
	double OpenGoodsExWindow(const char* cmd, char* retStr=NULL);	//! 打开物品扩展对话窗口
	double BindGoods(const char* cmd, char* retStr=NULL);			//! 绑定（解绑）物品
	double FrostGoods(const char* cmd, char* retStr=NULL);			//! 冻结（解冻）物品
	double CheckSpace(const char* cmd, char* retStr=NULL);
	double GetGoodsNum(const char* cmd, char* retStr=NULL);
	double GetGoodsFree(const char* cmd, char* retStr=NULL);
	double GetEquipIDByPos(const char* cmd, char* retStr=NULL);
	double GetEquipNameByPos(const char* cmd, char* retStr=NULL);	//! 获得指定装备栏位的装备原始名
	double GetPlayerGoodsAddonProperty(const char* cmd, char* retStr=NULL);
	double SetPlayerGoodsAddonProperty(const char* cmd, char* retStr=NULL);
	double GetGoodsBaseType(const char* cmd, char* retStr=NULL);
	double OpenGoodsEnchase(const char* cmd, char* retStr=NULL);
	double DelPlayerGoods(const char* cmd, char* retStr=NULL);
	double DelUsedGoods(const char* cmd, char* retStr=NULL);
	double CheckUsedGoods(const char* cmd, char* retStr=NULL);
	double GetUsedGoodsPropertiy1(const char* cmd, char* retStr=NULL);
	double GetUsedGoodsPropertiy2(const char* cmd, char* retStr=NULL);
	double SetUsedGoodsPropertiy1(const char* cmd, char* retStr=NULL);
	double SetUsedGoodsPropertiy2(const char* cmd, char* retStr=NULL);
	double BuyGoods(const char* cmd, char* retStr=NULL);
	double GetGoodsAddonProperty(const char* cmd,char* retStr=NULL);
	double FullRefine(const char* cmd, char* retStr=NULL);
	double AddMedal			(const char* cmd, char* retStr=NULL);
	double DelMedal			(const char* cmd, char* retStr=NULL);
	double GetMedal			(const char* cmd, char* retStr=NULL);
	double GetMedalNum		(const char* cmd, char* retStr=NULL);
	double GetShowMedalName	(const char* cmd, char* retStr=NULL);
	double IsEquipEnchased(const char* cmd, char* retStr=NULL);
	double GetGoodsDisplayName(const char* cmd, char* retStr=NULL);
	double GetCostYuanBao(const char* cmd, char* retStr=NULL);
	
	double BeginMake(const char* cmd, char* retStr=NULL);			//! 开始一个非副职业制作（将配方发送给玩家）
	double FinishMake(const char* cmd, char* retStr=NULL);			//! 完成一个非副职业制作（按配方合成物品）
	double BackToCity(const char* cmd, char* retStr=NULL);			//回城

	double BeginContainerGoodsOpt(const char* cmd, char* retStr=NULL);		//! 开始一个通用物品容器操作
	double GetOptGoodsInContainer(const char* cmd, char* retStr=NULL);		//! 得到操作物品
	double PutOptGoodsInContainer(const char* cmd, char* retStr=NULL);		//! 放入操作物品到容器中
	double RemoveOptGoodsInContainer(const char* cmd, char* retStr=NULL);	//! 清除容器中的操作物品
	double RefreshContainerTxt(const char* cmd, char* retStr=NULL);			//! 刷新容器文本
	double RefreshContainerGold(const char* cmd, char* retStr=NULL);		//! 刷新容器显示金钱
	double EndContainerGoodsOpt(const char* cmd, char* retStr=NULL);		//! 关闭一个通用物品容器操作

	double GetGoodsOriginalName(const char* cmd, char* retStr=NULL);		//! 根据GUID 得到指定物品（玩家身上）的原始名
	double GetGoodsAddonPropertyValues(const char* cmd, char* retStr=NULL);	//! 根据GUID 得到指定物品（玩家身上）的附加属性值
	double SetGoodsAddonPropertyValues(const char* cmd, char* retStr=NULL);	//! 根据GUID 设置指定物品（玩家身上）的附加属性值
	double ResetGoodsStochastic(const char* cmd, char* retStr=NULL);		//! 根据GUID 重置指定物品（玩家身上）的随机属性值
	double DelGoodsByGuid(const char* cmd, char* retStr=NULL);				//! 根据GUID 删除指定物品（玩家身上）
	double AddGoodsReturnGuid(const char* cmd, char* retStr=NULL);			//! 创建物品，并返回第一个物品的GUID
	double IsFrostGoods(const char* cmd, char* retStr=NULL);				//! 根据GUID 得到指定物品（玩家身上）是否为冻结
	
	double GetUsedGoodsOriginalName(const char* cmd,char* retStr=NULL);		//! 获得正在使用的物品的原始名
	
	double GetUsedGoodsGuid(const char* cmd,char* retStr=NULL);    //! 获得正在使用的物品的GUID
    double UploadGoods(const char* cmd, char* retStr=NULL);        // 上传物品
	
	double SetAddonDesc( const char *cmd, char *retStr = NULL );	/// 设置物品额外描述
	double GetAddonDesc( const char *cmd, char *retStr = NULL );	/// 获取物品额外描述
	
	double AddGoodsToServer(const char *cmd, char *retStr = NULL);                        // 添加一个物品到服务器
	double AddGoodsToClient(const char *cmd, char *retStr = NULL);
	

	//-----------------------------------------------------
	//任务相关函数
	double AddQuest(const char* cmd, char* retStr=NULL);
	double GetCurrentMasterRoleID(const char* cmd, char* retStr=NULL);
	double CompleteQuest(const char* cmd, char* retStr=NULL);
	double DisbandQuest(const char* cmd, char* retStr=NULL);
	double GetQuestState(const char* cmd, char* retStr=NULL);
	double GetValidQuestNum(const char* cmd, char* retStr=NULL);
	double UpdateQuestStep(const char* cmd, char* retStr=NULL);
	double UpdateQuestData(const char* cmd, char* retStr=NULL);
	double UpdateQuestParamData(const char* cmd, char* retStr=NULL);
	//取得佣兵任务id
	double GetMercQuest(const char* cmd, char* retStr=NULL);
	//设置当佣兵兵任务
	double SetMercQuest(const char* cmd, char* retStr=NULL);
	double GetRoleStepNum(const char* cmd, char* retStr=NULL);
	//取得 某一任务某一步骤的Index对应的值
	double GetParam(const char* cmd, char* retStr=NULL);
	//取得 某一任务某一步骤的Index对应的附加属性值
	double GetRoleAccParam(const char* cmd, char* retStr=NULL);
	//设置 某一任务某一步骤的Index对应的附加属性值
	double SetRoleAccParam(const char* cmd, char* retStr=NULL);
	//增加 某一任务某一步骤的Index对应的值
	double AddRoleAccParam(const char* cmd, char* retStr=NULL);
	//取得 某一任务某一步骤的Index对应的场景id
	double GetRoleRegionID(const char* cmd, char* retStr=NULL);
	//设置 某一任务某一步骤的Index对应的场景id
	double SetRoleRegionID(const char* cmd, char* retStr=NULL);
	//取得 某一任务某一步骤的Index对应的场景x坐标
	double GetRoleRegionX(const char* cmd, char* retStr=NULL);
	//设置 某一任务某一步骤的Index对应的场景x坐标
	double SetRoleRegionX(const char* cmd, char* retStr=NULL);
	//取得 某一任务某一步骤的Index对应的场景y坐标
	double GetRoleRegionY(const char* cmd, char* retStr=NULL);
	//设置 某一任务某一步骤的Index对应的场景y坐标
	double SetRoleRegionY(const char* cmd, char* retStr=NULL);
	//取得任务列表的任务数量
	double GetRoleSum(const char* cmd, char* retStr=NULL);
	//取得所在主线任务章节
	double GetChapter(const char* cmd, char* retStr=NULL);
	//设置主线任务章节数
	double SetChapter(const char* cmd, char* retStr=NULL);
	//添加任务记录
	double AddFinishedRoleRecord(const char* cmd, char* retStr=NULL);
	double DelFinishedRoleRecord(const char* cmd, char* retStr=NULL);	
	//取得一个索引的任务的状态（是否完成）
	double GetRoleFinishedRecord(const char* cmd, char* retStr=NULL);
	//取得一个任务的（当前步骤的）对应文本id的文本索引
	double GetStepText(const char* cmd, char* retStr=NULL);
	double GetText(const char* cmd, char* retStr=NULL);
	//取得文本类型
	double GetTextType(const char* cmd, char* retStr=NULL);
	//获取任务的名字
	double GetAttributeRoleName(const char* cmd, char* retStr=NULL);
	//任务是否会被记录
	double GetAttributeBeRecord(const char* cmd, char* retStr=NULL);
	//任务类型
	double GetAttributeRoleType(const char* cmd, char* retStr=NULL);
	//任务接受npc的id
	double GetAttributeEmbracerNPCID(const char* cmd, char* retStr=NULL);
	//是否被显示
	double GetAttributeShowAble(const char* cmd, char* retStr=NULL);
	//是否能反复接
	double GetAttributeRepeated(const char* cmd, char* retStr=NULL);
	//是否受经验倍率影响
	double GetAttributeExpRatio(const char* cmd, char* retStr=NULL);
	//死亡是否失败
	double GetAttributeDeadLost(const char* cmd, char* retStr=NULL);
	//任务失败后能否重新接
	double GetAttributeLostRepeated(const char* cmd, char* retStr=NULL);
	//最小等级限制
	double GetAttributeMinGrade(const char* cmd, char* retStr=NULL);
	//最大等级限制
	double GetAttributeMaxGrade(const char* cmd, char* retStr=NULL);
	//职业限制
	double GetAttributeOccupationRestrict(const char* cmd, char* retStr=NULL);
	//需要佣兵声望
	double GetAttributeEngageArmsRepute(const char* cmd, char* retStr=NULL);
	//章节数
	double GetAttributeChapterNum(const char* cmd, char* retStr=NULL);
	//获取下一个任务ID
	double GetAttributeNextRoleID(const char* cmd, char* retStr =NULL);
	//取得index对应的前置任务
	double GetLabelPrefix(const char* cmd, char* retStr=NULL);
	//取得前置任务数量
	double GetLabelSum(const char* cmd, char* retStr=NULL);
	//目标模板类型
	double GetAimAMTType(const char* cmd, char* retStr=NULL);
	//目标类型
	double GetAimType(const char* cmd, char* retStr=NULL);
	//取得目标id
	double GetAimId(const char* cmd, char* retStr=NULL);
	//取得目标数量
	double GetAimNum(const char* cmd, char* retStr=NULL);
	//取得 变量索引
	double GetAimParameIndex(const char* cmd, char* retStr=NULL);
	//掉落这种物品的生物类型（对物品）
	double GetDropFrom(const char* cmd, char* retStr=NULL);
	//几率
	double GetDropOdds(const char* cmd, char* retStr=NULL);
	//取得目标名字
	double GetAimName(const char* cmd, char* retStr=NULL);
	//取得备用属性
	double GetPrepareParam(const char* cmd, char* retStr=NULL);
	//CQusetEncouragement
	//CQusetEncouragement
	double GetStepEncouragementSum(const char* cmd, char* retStr=NULL);
	double GetQuestEncouragementSum(const char* cmd, char* retStr=NULL);
	//取得奖励的名字
	double GetSetpEncouragementName(const char* cmd, char* retStr=NULL);
	double GetQuestEncouragementName(const char* cmd, char* retStr=NULL);
	//取得奖励类型
	double GetSetpEncouragementType(const char* cmd, char* retStr=NULL);
	double GetQuestEncouragementType(const char* cmd, char* retStr=NULL);
	//奖励的具体id
	double GetSetpEncouragementID(const char* cmd, char* retStr=NULL);
	double GetQuestEncouragementID(const char* cmd, char* retStr=NULL);
	//奖励数量
	double GetSetpEncouragementNum(const char* cmd, char* retStr=NULL);
	double GetQuestEncouragementNum(const char* cmd, char* retStr=NULL);
	//奖励所占的格子数
	double GetStepEncouragementCell(const char* cmd, char* retStr=NULL);
	double GetQuestEncouragementCell(const char* cmd, char* retStr=NULL);
	//执行奖励
	double GetStepSum(const char* cmd, char* retStr=NULL);
	double GetStepAimNum(const char* cmd, char* retStr=NULL);
	double FindRoleIsInSystem(const char* cmd, char* retStr=NULL);
	double FindRoleStepIsInSystem(const char*cmd, char* retStr=NULL);
	//俑兵任务
	//重置俑兵任务刷新时间
	double ResetMercQuestUpdateTime(const char*cmd, char* retStr=NULL);
	//获得俑兵列表ID任务数据
	double AcquireMercQuest(const char*cmd, char* retStr=NULL);
	//发送俑兵任务数据到客户端，$俑兵列表ID==0表示只发送刷新时间
	double SendMercQuestToClient(const char*cmd, char* retStr=NULL);
	//打开俑兵任务界面
	double OpenMercQuestInterface(const char*cmd, char* retStr=NULL);
	//得道俑兵任务数据
	double GetMercQuestData(const char*cmd, char* retStr=NULL);
	//设置俑兵任务数据
	double SetMercQuestData(const char*cmd, char* retStr=NULL);
	//返回任务集结点
	double GetQuestMassPoint(const char*cmd, char* retStr=NULL);
	//重置所有俑兵任务数据
	double ResetAllMercQuestData(const char*cmd, char* retStr=NULL);
	//查看某一任务是否在佣兵任务刷新表里面
	double IsInMercQuestList(const char*cmd, char* retStr=NULL);
	double SetRoleProtect(const char* cmd, char* retStr=NULL);
	double SetMasterQuest(const char* cmd, char* retStr=NULL);
	//赏金任务
	double OpenShareQuestInterface(const char* cmd, char* retStr=NULL);
	double GetTeamCurrQuest(const char* cmd, char* retStr=NULL);
	double SetTeamCurrQuest(const char* cmd, char* retStr=NULL);
	double DelTeamShareQuest(const char* cmd,char* retStr=NULL);
	double IsCompleteQuest(const char* cmd,char* retStr=NULL);
	double GetQuestOdds(const char* cmd,char* retStr=NULL);
	double CanAcceptMercQuest(const char* cmd,char* retStr=NULL);
	//任务引导
	double OpenIndexQuestInterface(const char* cmd, char* retStr=NULL);
	double SetIndexQuestParamNum(const char* cmd, char* retStr=NULL);
	double SetIndexQuestState(const char* cmd, char* retStr=NULL);
	double ResetIndexQuest(const char* cmd, char* retStr=NULL);
	//-----------------------------------------------------
	//副职业相关函数
	double CanCollect(const char* cmd, char* retStr=NULL);
	double CanCheckCollect(const char* cmd, char* retStr=NULL);
	double CheckCollect(const char* cmd, char* retStr=NULL);
	double LockCollection(const char* cmd,char* retStr=NULL);
	double OutlockCollection(const char* cmd, char* retStr=NULL);
	double CanFacture(const char* cmd, char* retStr=NULL);
	double Facture(const char* cmd, char* retStr=NULL);
	double FactureSkillStudyBegin(const char* cmd, char* retStr=NULL);
	double ClearFactureList(const char* cmd, char* retStr=NULL);
	double AddFacture(const char* cmd, char* retStr=NULL);
	double DelFacture(const char* cmd, char* retStr=NULL);
	double FindFacture(const char* cmd, char* retStr=NULL);
	double GetFactureNeedsLevel(const char* cmd, char* retStr=NULL);
	double GetFactureNeedsDOccu(const char* cmd, char* retStr=NULL);
	double UpdateEffect(const char* cmd, char* retStr=NULL);
	double AddFactureExp(const char* cmd,char* retstr=NULL);
	double AddCollectionExp(const char* cmd,char* retstr=NULL);
	//-----------------------------------------------------
	//技能状态相关函数
	//1、基本技能相关的函数
	double AddState(const char* cmd, char* retStr=NULL);		
	double AddSkill(const char* cmd, char* retStr=NULL);
	double DelSkill(const char* cmd, char* retStr=NULL);
	double SetSkillLevel(const char* cmd, char* retStr=NULL);
	double GetSkillLevel(const char* cmd, char* retStr=NULL);
	double CheckStudySkill  (const char* cmd, char* retStr=NULL);
	double GetAllStateNum	(const char* cmd, char* retStr=NULL);
	double DelNewState		(const char* cmd, char* retStr=NULL);
	double DelDeBuff		(const char* cmd, char* retStr=NULL);
	double DelAllBuff		(const char* cmd, char* retStr=NULL);
	double Modulus			(const char* cmd, char* retStr=NULL);

	double AddStateToShape	(const char* cmd, char* retStr=NULL);
	//2、技能脚本中用到的函数
	double AddNewState(const char* cmd, char* retStr=NULL);	
	double RemoverNewState(const char* cmd, char* retStr=NULL);	
	double SetMoveShapeAttrEx(const char* cmd, char* retStr=NULL);
	double GetMoveShapeAttrEx(const char* cmd, char* retStr=NULL);
	double ChangeMoveShapeAttrEx(const char* cmd, char* retStr=NULL);
	double AddDeBuffToObj	(const char* cmd, char* retStr=NULL);
	double SetEventDestObj	(const char* cmd, char* retStr=NULL);
	double GetBaseModuleLv  (const char* cmd, char* retStr=NULL);
	double ReLiveSkill		(const char* cmd, char* retStr=NULL);
	double GetPkResult(const char* cmd, char* retStr=NULL);
	double GetObjectType	(const char* cmd, char* retStr=NULL);
	double GetShieldNO		(const char* cmd, char* retStr=NULL);
	double SendSkillList	(const char* cmd, char* retStr=NULL);
	double GetDistance		(const char* cmd, char* retStr=NULL);
	double GetWeaponStr		(const char* cmd, char* retStr=NULL);	
	double GetDesEnergyRatio(const char* cmd, char* retStr=NULL);
	double GetEnergyRatio	(const char* cmd, char* retStr=NULL);
	double DeductEnergy		(const char* cmd, char* retStr=NULL);
	double SetTarget		(const char* cmd, char* retStr=NULL);	
	double GetSkillParam	(const char* cmd, char* retStr=NULL);
	double GetSkillElement  (const char* cmd, char* retStr=NULL);
	double GetSkillUseType	(const char* cmd, char* retStr=NULL);
	double GetStateParamByID(const char* cmd, char* retStr=NULL);				
	double GetStateParamByIndex(const char* cmd, char* retStr=NULL);
	double CheckStateFirer	(const char* cmd, char* retStr=NULL);
	double BeedAttackEvent	(const char* cmd, char* retStr=NULL);
	double GetSkillLv		(const char* cmd, char* retStr=NULL);
	double ReLiveInScript	(const char* cmd, char* retStr=NULL);
	double GetGuidByParam	(const char* cmd, char* retStr=NULL);
	double RunScriptByDest	(const char* cmd, char* retStr=NULL);
	double BeginUseSkill	(const char* cmd, char* retStr=NULL);
	double GetGoodsAddProperty(const char* cmd, char* retStr=NULL);
	double GetGoodsPlacePos	(const char* cmd, char* retStr=NULL);
	double DelGoodsByPlacePos	(const char* cmd, char* retStr=NULL);
	double SucceedUseItem	(const char* cmd, char* retStr=NULL);
	double GetActModify(const char* cmd, char* retStr=NULL);//取得技能动作修正时间
	double SendUnLockGoodsMsg(const char* cmd, char* retStr=NULL);
	double GetBeneficiaryInfo(const char* cmd, char* retStr=NULL);
	double GetKillerInfo	(const char* cmd, char* retStr=NULL);
	double GetModType		(const char* cmd, char* retStr=NULL);
	double GetAttackerType	(const char* cmd, char* retStr=NULL);
	double GetFunMoHua		(const char* cmd, char* retStr=NULL);
	double GetBModifier		(const char* cmd, char* retStr=NULL);
	double GetPrevent		(const char* cmd, char* retStr=NULL);	
	double GetPartDamageOut	(const char* cmd, char* retStr=NULL);
	double OnCostYuanBao	(const char* cmd, char* retStr=NULL);

	//-----------------------------------------------------
	//定义了和客户端交互，客户端可视相关的脚本函数
	double AddInfo(const char* cmd, char* retStr=NULL); 
	double AddDestInfo(const char* cmd, char* retStr=NULL); 
	double AddInfoByIndex(const char* cmd, char* retStr=NULL);
	double GameMessage(const char* cmd, char* retStr=NULL);
	double TalkBox(const char* cmd, char* retStr=NULL);
	//!	带图片的TalkBox
	double PicTalkBox(const char* cmd, char* retStr=NULL);
	double EndTalkBox(const char* cmd, char* retStr=NULL);
	double TalkBoxByIndex(const char* cmd, char* retStr=NULL);
	double StoryTalkBox(const char* cmd, char* retStr=NULL);
	double StoryTalkBoxByIndex(const char* cmd, char* retStr=NULL);
	double PixyTalkBox(const char* cmd, char* retStr=NULL);
	double EndPixyTalkBox(const char* cmd, char* retStr=NULL);
	double PixyTalkBoxByIndex(const char* cmd, char* retStr=NULL);
	double PlayEffectByIndex(const char* cmd, char* retStr=NULL);
	double PlayerTalk(const char* cmd, char* retStr=NULL);
	double PlayerTalkByIndex(const char* cmd, char* retStr=NULL);
	double PackGlint(const char* cmd, char* retStr=NULL);
	double PostPlayerInfo(const char* cmd, char* retStr=NULL);
	double PostDestPlayerInfo(const char* cmd, char* retStr=NULL);
	double PostPlayerInfoByIndex(const char* cmd, char* retStr=NULL);
	double PostDestPlayerInfoByIndex(const char* cmd, char* retStr=NULL);
	double ShowString(const char* cmd, char* retStr=NULL);
	double Weather(const char* cmd, char* retStr=NULL);
	double PlayEffect(const char* cmd, char* retStr=NULL);
	double PlayAction(const char* cmd, char* retStr=NULL);
	double PlaySound(const char* cmd, char* retStr=NULL);
	double Play3DSound(const char* cmd, char* retStr=NULL);
	double PlayBkMusic(const char* cmd, char* retStr=NULL);
	double StopBkMusic(const char* cmd, char* retStr=NULL);
	double AddGoodToShortCutBar(const char* cmd, char* retStr=NULL);
	double AddSkillToShortCutBar(const char* cmd, char* retStr=NULL);
	double TransforCamera(const char* cmd, char* retStr=NULL);
	double OperaInterface(const char* cmd, char* retStr=NULL);
	double HelpHint(const char* cmd, char* retStr=NULL);
	double InterfaceHint(const char* cmd, char* retStr=NULL);
	double OpenReliveWindow(const char* cmd, char* retStr=NULL);
	double OpenReLiveByGoodsWindow(const char* cmd, char* retStr=NULL);
	// 修改脚本变量
	double UpdateScriptVarValue(const char* cmd, char* retStr=NULL);
	double PlayVideo		(const char* cmd, char* retStr=NULL);
	double StopVideo		(const char* cmd, char* retStr=NULL);
	double SendExpAction	(const char* cmd, char* retStr=NULL);
	double GetPlayerAllProperties(const char* cmd, char* retStr=NULL);
	double GetPlayerAllVariables(const char* cmd, char* retStr=NULL);
	// 显示当前玩家全部脚本变量
	double ViewPlayerAllVariableList(const char* cmd, char* retStr=NULL);
	

	//-----------------------------------------------------
	//队伍帮会组织相关函数


	//1、队伍函数
	double GetTeamNum(const char* cmd, char* retStr=NULL);
	double GetTeamNumInRgn(const char* cmd, char* retStr=NULL);
	double GetTeamerName(const char* cmd, char* retStr=NULL);
	double GetTeamGUID(const char* cmd, char* retStr=NULL);
	double GetPlayerTeamGUID(const char* cmd, char* retStr=NULL);
	double IsTeamCaptain(const char* cmd, char* retStr=NULL);
	//! 检查是有地图上只有等于小于指定数量的队伍
	double CheckRgnOnlyTeam(const char* cmd, char* retStr=NULL);
	double GetTeamCaptainPos(const char* cmd, char* retStr=NULL);
	double SetRegionForTeam(const char* cmd, char* retStr=NULL);
	double SetTeamRegion(const char* cmd, char* retStr=NULL);
	double IsTeammatesAroundMe(const char* cmd, char* retStr=NULL);
	double InLvlDifference(const char* cmd, char* retStr=NULL);
	double InLvlSpace(const char* cmd, char* retStr=NULL);
	double RunScriptForTeam(const char* cmd, char* retStr=NULL);


	//2.帮会组织
	double CreateFaction(const char* cmd, char* retStr=NULL);
	double CreateUnion(const char* cmd, char* retStr=NULL);
	double OpenApplyBoard(const char* cmd, char* retStr=NULL);
	double OpenFacIconBoard(const char* cmd, char* retStr=NULL);
	double SetFactionRecruitState(const char* cmd, char* retStr=NULL);
	double GetFactionLevel(const char* cmd, char* retStr=NULL);	
	double GetFactionIDByPlayerName(const char* cmd, char* retStr=NULL);
	double GetUinionIDByPlayerName(const char* cmd, char* retStr=NULL);
	//! 为玩家所在工会添加资源（减少传入负数，@PlayerGuid可=""，$Ret=1表示执行成功）AddFacRes("@PlayerGuid", #value, $Ret);
	double AddFacRes(const char* cmd, char* retStr=NULL);
	//! 为玩家所在工会添加经验（减少传入负数，@PlayerGuid可=""，$Ret=1表示执行成功）AddFacExp("@PlayerGuid", #value, $Ret);
	double AddFacExp(const char* cmd, char* retStr=NULL);
	//! 为玩家添加工会贡献（减少传入负数，@PlayerGuid可=""，$Ret=1表示执行成功）AddFacMemberContribute("@PlayerGuid", #value, $Ret);
	double AddFacMemberContribute(const char* cmd, char* retStr=NULL);
	//! 获取成员贡献值
	double GetMemContribute(const char* cmd,char* retStr=NULL);
	//! 获取家族资源
	double GetFacRes(const char* cmd,char* retStr=NULL);
	//! 获取家族经验
	double GetFacExp(const char* cmd,char* retStr=NULL);
	//! 查询玩家是否是会长
	double IsFacMaster(const char* cmd, char* retStr=NULL);
	//添加公会的某据点的门票,减少传负数
	double AddFacTickets(const char* cmd,char* retstr=NULL);
	//添加某据点的战功值 ，减少传负数
	double AddFacBaseBattle(const char* cmd,char* retstr=NULL);
	//修改一个据点的信息
	double ChangeBaseInfo(const char* cmd,char* retstr=NULL);
	//增加公会或成员的战功总值
	double AddBattleVal(const char* cmd,char* retstr=NULL);
	//增加公会或成员的军事投资总值
	double AddArmyInvest(const char* cmd,char* retstr=NULL);
	//获取某据点的门票数
	double GetFacTickets(const char* cmd,char* retstr=NULL);
	//获取公会或成员的战功值
	double GetBattleVal(const char* cmd,char* retstr=NULL);
	//获取公会或成员的投资值
	double GetArmyInvest(const char* cmd,char* retstr=NULL);
	//获取成员参与权
	double GetJoinBattle(const char* cmd,char* retstr=NULL);
	//添加科技
	double AddTech(const char* cmd,char* retstr=NULL);
	//得到一个科技的等级
	double GetTechLv(const char* cmd,char* retstr=NULL);
	//根据家族GUID获取家族名字
	double GetFactionName(const char* cmd,char* retstr=NULL);
	//增加或减少敌对家族信息
	double ChangeEnemy(const char* cmd,char* retstr=NULL);
	//获取据点的战功值
	double GetFacBaseBattle(const char* cmd,char* retstr=NULL);
	//发送已更新据点信息数据到客户端
	double SendBaseInfo(const char* cmd,char* retstr=NULL);
	//获取家族等级获科技等级
	double GetFacLevel(const char* cmd,char* retstr=NULL);
	//打开公会界面
	double FacDisplay(const char* cmd,char* retstr=NULL);

	//队伍帮会组织相关函数
	//1、场景相关函数
	double GetRgnAttr(const char* cmd, char* retStr=NULL);
	double SetRgnAttr(const char* cmd, char* retStr=NULL);
	double GetCellType(const char* cmd, char* retStr=NULL);
	double RegionIsExit(const char* cmd, char* retStr=NULL);
	double GetRegionID(const char* cmd, char* retStr=NULL);
	double GetMapInfo(const char* cmd, char* retStr=NULL);
	double GetCountryOwningRegion(const char* cmd, char* retStr=NULL);
	double SetBlock(const char* cmd, char* retStr=NULL);
	double GetRegionRandomPos(const char* cmd, char* retStr=NULL);
	double GetRandomPosInRange(const char* cmd, char* retStr=NULL);
	double GetNextPosByDir( const char *cmd, char *retStr=NULL);
	double GetLineDir( const char *cmd, char *retStr = NULL );
	double GetRegionPlayerNum(const char* cmd, char* retStr=NULL);
	double RunScriptForAllPlayerInRgn(const char* cmd,char* retStr=NULL);
	//1、副本相关函数
	double CreatePRgn			(const char* cmd, char* retStr=NULL);
	double CreatePHRgn			(const char* cmd, char* retStr=NULL);
	double CreateTRgn			(const char* cmd, char* retStr=NULL);
	// 取得当前脚本玩家个人副本GUID	GetCurPlayerPDupRgnID(@Ret);
	double GetCurPlayerPDupRgnID	(const char* cmd, char* retStr=NULL);
	//取得当前脚本玩家个人房屋副本GUID	GetCurPlayerPHDupRgnID(@Ret);
	double GetCurPlayerPHDupRgnID	(const char* cmd, char* retStr=NULL);
	//取得当前脚本队伍副本GUID		GetCurTeamDupRgnID(@Ret);
	//double GetCurTeamDupRgnID		(const char* cmd, char* retStr=NULL);
	//取得玩家个人副本GUID		GetPlayerPDupRgnID(@playerID, @Ret);
	double GetPlayerPDupRgnID	(const char* cmd, char* retStr=NULL);
	//取得玩家个人房屋副本GUID	GetPlayerPHDupRgnID(@playerID, @Ret);
	double GetPlayerPHDupRgnID	(const char* cmd, char* retStr=NULL);
	//取得队伍副本GUID			GetTeamDupRgnID(@playerID, @Ret);
	double GetTeamDupRgnID		(const char* cmd, char* retStr=NULL);
	//取得普通场景GUID				@Ret=GetNormalRgnID($lTemplateRgnID);
	double GetNormalDupRgnID	(const char* cmd, char* retStr=NULL);
	// 切换普通场景by模板id
	double ChangeRgnByTemplateID (const char* cmd, char* retStr=NULL);
	// 添加场景变量
	double AddRgnVar(const char* cmd, char* retStr=NULL);
	// 设置场景变量
	double SetRgnVar(const char* cmd, char* retStr=NULL);
	// 删除场景变量
	double DelRgnVar(const char* cmd, char* retStr=NULL);
	// 取得指定场景变量
	double GetRgnVar(const char* cmd, char* retStr=NULL);
	//删除指定副本ＩＤ的副本。
	double DeleteRgn			(const char* cmd, char* retStr=NULL);
	//返回指定副本ＩＤ的副本的蓝图ＩＤ，如该副本ＩＤ的副本不存在则返回0。
	double GetTemplateIDByGUID	(const char* cmd, char* retStr=NULL);
	//返回当前所在场景的ＩＤ。
	double GetCurRgnGUID		(const char* cmd, char* retStr=NULL);
	//返回指定副本ＩＤ的副本中当前在线人数。（原GetRegionPlayerNum）
	//返回指定副本ＩＤ的副本的类型。
	double GetDupTypeByGUID		(const char* cmd, char* retStr=NULL);
	//返回指定副本ＩＤ的副本的所有者ＩＤ。
	double GetDupRgnOnwerGUID	(const char* cmd, char* retStr=NULL);
	//返回指定副本ＩＤ的副本的人数上限。
	double GetDupRgnPlayerMaxNum		(const char* cmd, char* retStr=NULL);
	//返回指定角色ＩＤ玩家角色的个人房屋副本ＩＤ。（需要该玩家不在线时也可查询）
	double GetPHRgnGUIDByOnwerName	(const char* cmd, char* retStr=NULL);
	//随机返回当前副本中的一个不为阻挡的坐标，并将坐标值保存到指定的2个玩家变量中。（原GetRegionRandomPos）
	//传送到指定副本的指定坐标。（原ChangeRegion）
	//队伍中达成条件的成员全部传送到指定副本的指定坐标。（原SetTeamRegion）
	//在指定副本中指定坐标创建指定原始名、数量、脚本的生物。（原CreateMonster）
	//删除指定副本中所有为指定原始名的生物。
	double DeleteMonster		(const char* cmd, char* retStr=NULL);
	//在指定副本中指定坐标创建指定ＩＤ的地图地物。（暂定，可能公测后开放）
	double CreateDupRgnEntity	(const char* cmd, char* retStr=NULL);
	//以副本为执行者执行一个脚本。（即使该副本中没有玩家脚本也继续执行）
	double RunDupRgnScript		(const char* cmd, char* retStr=NULL);
	//返回目前游戏中存在的副本数量。
	double GetDupRgnNum			(const char* cmd, char* retStr=NULL);
	//返回目前游戏中存在的副本数量上限。
	double GetDupRgnMaxNum		(const char* cmd, char* retStr=NULL);
	//设置指定副本ＩＤ的副本的管理权开放（1／0）。
	double SetDupRgnOperFlag	(const char* cmd, char* retStr=NULL);
	//得到指定副本ＩＤ的副本的管理权开放（1／0）。
	double GetDupRgnOperFlag	(const char* cmd, char* retStr=NULL);
	//返回执行者当前所在队伍中的指定队伍顺序号的角色ＩＤ。
	double GetTeamMaterIDByPos		(const char* cmd, char* retStr=NULL);
	//修改指定副本ＩＤ的永久副本的显示名，弹出一个输入框来修改。命名规则同玩家角色命名规则。
	double EditDupRgnName		(const char* cmd, char* retStr=NULL);
	// 取得本地GSID
	double GetLocalGSID			(const char* cmd, char* retStr=NULL);
	// 通过玩家名字查找GUID, 参数2返回GUID
	double GetGUIDByPlayerName  (const char* cmd, char* retStr=NULL);
	// 通过玩家名字查找其P GUID, 参数2返回GUID
	double GetPGUIDByPlayerName  (const char* cmd, char* retStr=NULL);
	// 通过玩家名字查找其PH GUID, 参数2返回GUID
	double GetPHGUIDByPlayerName  (const char* cmd, char* retStr=NULL);
	// 通过玩家名字查找其T GUID, 参数2返回GUID
	double GetTGUIDByPlayerName  (const char* cmd, char* retStr=NULL);
	// 设置当前脚本玩家个人副本GUID       SetPlayerPDupRgnID("@OwnerID","@RgnID");
	double SetPlayerPDupRgnID (const char* cmd, char* retStr=NULL);					
	// 设置当前脚本玩家个人房屋副本GUID		SetPlayerPHDupRgnID("@OwnerID","@RgnID");	
	double SetPlayerPHDupRgnID (const char* cmd, char* retStr=NULL);		
	// 设置当前脚本队伍副本GUID			AddTeamRgnID("@OwnerID","@RgnID");
	double AddTeamRgnID(const char* cmd, char* retStr=NULL);
	//! 为当前脚本队伍删除副本GUID		DelTeamRgnID("@OwnerID", $templateID);
	double DelTeamRgnID(const char* cmd, char* retStr=NULL);
	//! 为当前脚本队伍清除所有副本GUID	ClearTeamRgnID("@OwnerID");
	double ClearTeamRgnID(const char* cmd, char* retStr=NULL);
	//! 两个玩家交换所有队伍副本GUID	SwapTeamRgnID("@OwnerID", "@AimID");
	double SwapTeamRgnID(const char* cmd, char* retStr=NULL);
	//! 得到玩家所在队伍的队伍副本数			GetTeamRgnIDNum("@OwnerID", $Ret);
	double GetTeamRgnIDNum(const char* cmd, char* retStr=NULL);
	//! 得到玩家所在队伍的第N个队伍副本ID		GetTeamRgnIDByIndex("@OwnerID", $N, "@ReRgnGuid");
	double GetTeamRgnIDByIndex(const char* cmd, char* retStr=NULL);
	//! 得到玩家所在队伍的第N个队伍副本的蓝图ID	GetTeamRgnTemplatIDByIndex("@OwnerID", $N, $Ret);
	double GetTeamRgnTemplatIDByIndex(const char* cmd, char* retStr=NULL);

	// 查看副本变量数据
	double ViewRgnVar(const char* cmd, char* retStr=NULL);
	// 切换副本
	double ChangeDupRgn(const char* cmd, char* retStr=NULL);
	// 切换个人副本
	double ChangeDupRgnByRgnID(const char* cmd, char* retStr=NULL);	
	// 创建个人房屋副本物品(Monster)
	double CreatePHGoods(const char* cmd, char* retStr=NULL);//(const CGUID& ownerID, const char* name, long nCounts, long posX, long posY,
	//	long width, long height, long dir, const char* strScript, const char* strTalk);
	// 删除个人房屋副本物品(Monster)
	double DeletePHGoods(const char* cmd, char* retStr=NULL);//(const CGUID& ownerID, const CGUID& goodsID);
	//取得个人GUID
	double GetPlayerGUID(const char* cmd, char* retStr=NULL);
	//取得目标的GUID
	double GetDestGUID(const char* cmd, char* retStr=NULL);
	// 通过GUID调用该player、monster的脚本
	double CallScriptByGUID(const char* cmd, char* retStr=NULL);
	// 查看指定GUID的Rgn是否存在
	double RgnIsAvlidByGUID(const char* cmd, char* retStr=NULL);
	// 查看指定pos的副本详细数据
	double ViewDupRgnData(const char* cmd, char* retStr=NULL);
	// @变量赋值接口,Param1:@Src   Param2:@Des
	double EvaluateGUID(const char* cmd, char* retStr=NULL);
	// 查看Rgn是否存在Param1:@RgnID, Param2:#RetVar
	double ViewRgnValidByGUID(const char* cmd, char* retStr=NULL);
	// 取得当前脚本调用者的GUID
	double GetCallerGUID(const char* cmd, char* retStr=NULL);
	// 清除当前GS的所有该类型副本
	double ClearCurGSDupRgn(const char* cmd, char* retStr=NULL);
	// 刷新玩家的副本记录列表
	double ReFreshPlayerDupRgnList(const char* cmd, char* retStr=NULL);
	// 改变当前场景的某个据点状态
	double SetRgnStrongPointState (const char* cmd, char* retStr=NULL);
	// 取得当前场景的某个据点状态
	double GetRgnStrongPointState (const char* cmd, char* retStr=NULL);
	// 取得指定地图指定阵营精确人数
	double GetRegionCampPlayerNum(const char* cmd, char* retStr=NULL);

	//其他的脚本相关函数
	//1、国家相关函数
	//........................................................
	//2. 战队系统
	
	//! 战队排队的进入和退出
	double SentaiQueue(const char* cmd, char* retStr=NULL);
	//! 获得玩家的战队积分
	double GetPlayerSentaiPoints(const char* cmd, char* retStr=NULL);
	//! 修改玩家的战队积分
	double ChangePlayerSentaiPoints(const char* cmd, char* retStr=NULL);
	//! 获取队伍的战队积分
	double GetTeamSentaiPoints(const char* cmd, char* retStr=NULL);
	//! 结束比赛
	double EndMatch(const char* cmd, char* retStr=NULL);
	//! 结束一个队伍的比赛
	double EndTeamMatch(const char* cmd, char* retStr=NULL);
	//! 是否在战队系统中
	double IsSentaiSys(const char* cmd, char* retStr=NULL);

	//......................................................
	//3.跑商系统相关函数
	// 开启跑商商店界面：买/卖
	double OpenBusiness			(const char* cmd,char* retStr=NULL);
	double IsBusinessState( const char *cmd, char *retStr = NULL );
	double UseHiggleSkill( const char *cmd, char *retStr = NULL );
	double UseTradeSkill( const char *cmd, char *retStr = NULL );
	double UseCheckingSkill( const char *cmd, char *retStr = NULL );
	double UseChafferSkill		(const char* cmd,char* retStr=NULL);
	double OpenInvest			(const char* cmd,char* retStr=NULL);
	double RemoveBusinessState	(const char* cmd,char* retStr=NULL);
	// 开启家族跑商界面：买（0）/卖（1）
	double OpenFBusiness( const char *cmd, char *retStr = NULL );

	//4.邮件系统函数
	double SendSysMail			(const char* cmd, char* retStr=NULL);
	double ReceiveSysMail		(const char* cmd, char* retStr=NULL);
	double ReceiveMail			(const char* cmd, char* retStr=NULL);
	//5.奖励相关
	double OpenBouns			(const char* cmd, char* retStr=NULL);

	// Fox@20081230 宠物
	double CreatePet(const char* pCmd, char* pRetStr = NULL);
	double DelPet(const char* pCmd, char* pRetStr = NULL);
	double RelivePet(const char* pCmd, char* pRetStr = NULL);
	double AddPetSkill(const char* pCmd, char* pRetStr = NULL);
	double GetFocusPet(const char* pCmd, char* pRetStr = NULL);
	double GetPet(const char* pCmd, char* pRetStr = NULL);

	// 反外挂相关函数 BAIYUN@20090603
	// 改变玩家活越度
	double ChangeActivity(const char *cmd, char *retStr = NULL);
	// 踢玩家下线并限制其第二天才能登录
	double ACKickPlayer(const char *cmd, char *retStr = NULL);
	// 停止外挂检查的脚本函数
	double StopCheckAC(const char *cmd, char *retStr = NULL);
	// 获取玩家反外挂相关属性
	double GetACAttr( const char *cmd, char *retStr = NULL );
	// 开启反外挂定时器
	double StartCheackAC(const char *cmd , char *retStr = NULL);

	// 隐匿功能相关接口
	double GetRegionHideFlagById( const char *cmd, char *retStr = NULL );
	double GetPlayerCanHideByGuid(const char *cmd, char *retStr = NULL);
	double SetPlayerCanHideByGuid(const char *cmd, char *retStr = NULL);
	double GetPlayerHideFlagByGuid(const char *cmd, char *retStr = NULL);
	double SetPlayerHideFlagByGuid(const char *cmd, char *retStr = NULL);
	double GetHideName(const char *cmd, char *retStr = NULL);
	double GMViewHideNameAround(const char *cmd, char *retStr = NULL);
	double GMAddSkill(const char *cmd, char *retStr = NULL);
	double GMDelSkill(const char *cmd, char *retStr = NULL);
	// 定时器功能相关接口
	// 定时器激活，记录定时器信息，返回客户端结果消息
	double SetTimer(const char* cmd, char* retStr = NULL);
	// 定时器关闭，无需返回客户端消息
	double CloseTimer(const char* cmd, char* retStr = NULL);
	// 取得玩家定时器状态
	double GetPlayerTimer(const char* cmd, char* retStr = NULL);
	// 取得定时器剩余时间
	double GetTimer(const char* cmd, char* retStr = NULL);
    //打开彩票界面
	double OpenLotteryPage(const char* cmd, char* retStr = NULL);
	//抽取奖品
	double DoLottery(const char* cmd, char* retStr = NULL);
	//设置世界积分
	double SetWorldCredit(const char* cmd, char* retStr = NULL);
	//获得个人积分
	double GetPersonalCredit(const char* cmd, char* retStr = NULL);
	//获得累计积分
	double GetPersonalCreditTotal(const char* cmd, char* retStr = NULL);
	//设置当前最高积分和玩家名
	double SetCurHighestCredit(const char* cmd, char* retStr = NULL);
	//设置玩家的论盘等级
	double SetWorldCreditVelue(const char* cmd, char* retStr = NULL);
	//获得世界积分上限
	double GetWorldCreditMax(const char* cmd, char* retStr = NULL);
	//向客户端发送世界积分和个人最高积分信息
	double SendWorldCredit(const char* cmd, char* retStr = NULL);
	//发送彩票系统的世界喊话信息
	double SendLotteryChat(const char* cmd, char* retStr = NULL);
	//返回当次抽奖的奖品名
	double GetLotteryRe(const char* cmd, char* retStr = NULL);
	//获得当前玩家的升级经验
	double GetNextTotalExp(const char* cmd, char* retStr = NULL);

    // 结婚日志
    double MarriageLog(const char* cmd, char* retStr = NULL);

};

/************************************************
*	//运行脚本的结构							*
*	struct stRunScript							*
*	{											*
*	CRegion* pRegion;	// 场景					*
*	CPlayer* pPlayer;	// 人物					*
*	CNpc* pNpc;			// NPC					*
*	char strFile[64];	// 脚本文件名			*
*	};											*
************************************************/

//运行脚本的结构
struct stRunScript
{
	CRegion* pRegion;		// 场景
	CShape* srcShape;		// 人物
	CShape* desShape;		// NPC
	CGUID guUsedItemID;		// 使用的道具ID
	DWORD dwDiedMonsterIndex;	// 死亡的怪物Index
	char* pszFileName;			// 脚本文件名

	stRunScript()
	{
		pRegion = NULL;
		srcShape = NULL;
		desShape = NULL;
		pszFileName = NULL;
		dwDiedMonsterIndex = 0;
	}
};

//变量结构，用于给脚本传入参数列表
struct stVarPair
{
	std::string Name; // 变量名
	double NumValue;
	std::string StrValue; // cannot be used in union, so ..
	CGUID GuidValue;
	enum
	{
		VT_NUMBER, VT_STRING, VT_GUID, VT_INVALID
	} ValueType;

	stVarPair( const std::string &name, double value ) : Name( name ), NumValue( value ), ValueType( VT_NUMBER ) { }
	stVarPair( const std::string &name, std::string &value ) : Name( name ), StrValue( value ), NumValue( 0 ), ValueType( VT_STRING ) { }
	stVarPair( const std::string &name, const CGUID &value ) : Name( name ), GuidValue( value ), NumValue( 0 ), ValueType( VT_GUID ) { }
	stVarPair() : NumValue( 0 ), ValueType( VT_INVALID ) { }
};

typedef std::list<stVarPair> VarPairListType;

// 脚本链表
typedef map<CGUID,CScript*,hash_guid_compare>	listScript;
typedef map<CGUID,CScript*,hash_guid_compare>::iterator	itScript;

extern listScript		g_Scripts;			//脚本链表
typedef deque<CScript*> FreeScript;
extern FreeScript g_FreeScriptList;     // 预分配的空闲脚本内存
extern map<CScript*,string> g_UsingScript;     // 正在使用的Script

// [070511 AHC]
CScript* PopFreeScript(const char* pszName);
void	 PushFreeScript(CScript* freeScript);
double RunScript(stRunScript *, char* = NULL, int paramA=0, int paramB=0);//外部调用脚本

// 调用脚本时传入任意多个参数
double RunScript( stRunScript *rs, char *content, const VarPairListType &var_list );

long DelectAllScripByShape(const CGUID& PlayerGuid);
//删除玩家的Talkbox脚本
long DelPlayerTalkBoxScript(const CGUID& PlayerGuid, SCRIPT_FUNC_DEF funcDef);

long DelectPlayerScript(const CGUID &ScriptID,const CGUID& PlayerGuid);	//删除某个ID对应的脚本
bool ScriptIfExit(CPlayer* pPlayer,char* strFile);			//判断某个万家的脚本是否正在执行
long DelectPlayerScript(CPlayer* pPlayer,char* strFile);	//在脚本链表中清除PLAYER对应的脚本
long ContinueRunScript(const CGUID& ScriptID, CVariableList* stackVarList);//继续执行一个脚本
long GetScriptIndex();						//得到一个脚本的全局ID


//输出脚本信息
void OutputScriptInfo();
// 客户端选择有效性判断
bool ClientOptIsValid(const CGUID& scriptid, const char* szClientOpt);
// 清除脚本的客户端选择检查数据
void ClearClientOptData(const CGUID& scriptid);
#endif