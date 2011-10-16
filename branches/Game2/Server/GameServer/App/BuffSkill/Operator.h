//=============================================================================
/**
 *  file: Operator.h
 *
 *  Brief:状态、技能中的基本操作包装，例如属性变化等
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-8-31
 */
//=============================================================================

#ifndef __OPERATOR_H__
#define __OPERATOR_H__
#pragma once

#include "BuffSkillDef.h"

class CMoveShape;
class CScript;
class CExpressions;

//定义操作函数
typedef void OPFUN(CMoveShape* pShape,string& strName,int nValue);

class COperator
{
public:
	COperator(stModuParam *pParam);
	virtual ~COperator(void);
protected:
	//技能参数
	stModuParam	*m_pParam;
	//作用目标
	eObj m_Obj;

	CMoveShape* GetOpShape(CMoveShape* pDest);
public:
	//执行
    virtual void Execute(CMoveShape* pShape) = 0;
			void SetParam(stModuParam *pParam) {m_pParam = pParam;}		
};


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
class COpProperty:public COperator
{
public:
	COpProperty(stModuParam *pParam);
	COpProperty(void* pParam);
	virtual ~COpProperty(void);
private:
	//操作函数
	OPFUN *m_pOpFun;
	//属性名字
	string m_strName;
	int m_nValue;
	//脚本变量名
	string m_strVariaName;
    //直接读取数据
    string m_ParamName;
public:
	//初始化
	void InitBaseData(eObj Obj,OPFUN *OpFun,string strName,int value, string strVariaName,string paramname)
	{
		m_Obj = Obj;
		m_pOpFun = OpFun;
		m_strName = strName;
		m_nValue = value;
		m_strVariaName = strVariaName;
        m_ParamName = paramname;
	};
	//执行
    void Execute(CMoveShape* pShape);
};

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
class COpBuff:public COperator
{
public:
	COpBuff(stModuParam *pParam);
	COpBuff(void* pParam);
	virtual ~COpBuff(void);
private:
	//操作
	eModuleOper m_Op;
    //清除buff类型  
    eSkillAttackType m_SkillType;
	//buff id
	int m_nBuffID;
	//buff lvl
	int m_nBuffLvl;
	//状态数量
	int m_nBuffCount;
    //buf id  string
    string m_strBuffId;
    //buf lvl string
    string m_strBuffLvl;
    //buf num string
    string m_strBuffNum;
public:
	//
	 void InitBaseData(eObj Obj,eModuleOper op,eSkillAttackType Type,int id, int lv,int nCount,string bufid,string buflvl,string bufnum)
	{
		m_Obj = Obj;
		m_Op = op;
        m_SkillType = Type;
		m_nBuffID = id;
		m_nBuffLvl = lv;
		m_nBuffCount = nCount;
        m_strBuffId  = bufid;
        m_strBuffLvl = buflvl;
        m_strBuffNum = bufnum;
	}

	//执行
    void Execute(CMoveShape* pShape);
};


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
class COpGood:public COperator
{
public:
	COpGood(stModuParam *pParam);
	COpGood(void* pParam);
	virtual ~COpGood(void);
private:
	//操作
	eModuleOper m_Op;
	//物品原始名
	string m_strOrigName;
	//值
	int m_nValue;;
	//容器
	int m_nPlace;
	//栏位
	int m_nPos;
	//物品类别
	int m_nGoodType;
	//武器类别
	int m_nWeaponType;
public:
	void InitBaseData(eObj Obj,eModuleOper op,string name,int value,DWORD dwPlace, DWORD dwPos,DWORD dwGoodType, DWORD dwWeaponType)
	{
		m_Obj = Obj;
		m_Op = op;
		m_strOrigName = name;
		m_nValue = value;
		m_nPlace = dwPlace;
		m_nPos = dwPos;
		m_nGoodType = dwGoodType;
		m_nWeaponType = dwWeaponType;
	}

	//执行
    void Execute(CMoveShape* pShape);
};


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//位置改变
class COpPosition:public COperator
{
public:
	COpPosition(stModuParam *pParam);
	COpPosition(void* pParam);
	virtual ~COpPosition(void);
	enum eDirec
	{
		//前边
		DIREC_Front,
		//后边
		DIRECT_Back,
	};
	enum ePosStyle
	{
		//普通位移
		POS_SIMPLE,
		//滑动位移
		POS_SLIP,
	};
private:
	//参考目标
	eObj m_refObj;
	//方向
	eDirec m_eDriect;
	//位移方式
	ePosStyle m_ePosStyle;
	//速度
	float m_fSpeed;
public:
	void InitBaseData(eObj Obj,eObj obj,eObj rObj,string dir,string style,string speed)
	{
		m_Obj = Obj;
		m_refObj = rObj;
		m_eDriect = GetDir(dir);
		m_ePosStyle = GetPosStyle(style);
		m_fSpeed = (float)atof(speed.c_str());
	}
	
	eDirec GetDir(string strDir)
	{
		if (strDir=="DIREC_Front")
		{
			return DIREC_Front;
		}
		else if (strDir=="DIRECT_Back")
		{
			return DIRECT_Back;
		}
		return DIREC_Front;		
	}
	ePosStyle GetPosStyle(string strPosStyle)
	{
		if (strPosStyle=="POS_SIMPLE")
		{
			return POS_SIMPLE;
		} 
		else if (strPosStyle=="POS_SLIP")
		{
			return POS_SLIP;
		}
		return POS_SIMPLE;
	}

	//执行
    void Execute(CMoveShape* pShape);
	void GetRandomPos(CMoveShape *pShape,LONG &x, LONG &y);
};

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//方向改变
class COpDirec:public COperator
{
public:
	COpDirec(stModuParam *pParam);
	COpDirec(void *pParam);
	virtual ~COpDirec(void);

	//指定参考方向
	enum eDirec
	{
		eDir_Mouse=0,
		eDir_Self,
		eDir_Dest,
	};
private:
	//参考方向
	eDirec m_eDir;
public:
	void InitBaseData(eObj Obj,eDirec eDir)
	{
		m_Obj = Obj;
		m_eDir = eDir;
	}	
	//执行
    void Execute(CMoveShape* pShape);
	void SendChangeDir(CMoveShape* pShape);
};

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
class COpTimeChange:public COperator
{
public:
	COpTimeChange(stModuParam *pParam);
	COpTimeChange(void* pParam);
	virtual ~COpTimeChange(void);

	enum Op_Cooldown
	{
		OP_TS_NO=0,
		OP_TS_Act=1, //动作时间
		OP_TS_Self,  //技能本身
		OP_TS_Inton, //吟唱时间
        OP_TS_CDControl,//CD控制时间
	};

private:
	Op_Cooldown m_eOpCd;
	//值
	int m_nValue;
	//脚本变量
	string m_strVariaName;
    //直接从程序读取属性
    string m_ParamName;
public:
	//初始化
	void InitBaseData(eObj Obj,string& type,int value,string strname,string paramname)
	{
		m_Obj = Obj;
		m_eOpCd = GetType(type);
		m_nValue = value;
		m_strVariaName = strname;
        m_ParamName = paramname;
	}

	Op_Cooldown GetType(string strType)
	{
		if (strType=="OP_TS_Act")
		{
			return OP_TS_Act;
		} 
		else if(strType=="OP_TS_Self")
		{
			return OP_TS_Self;
		}
		else if (strType=="OP_TS_Inton")
		{
			return OP_TS_Inton;
		}
        else if (strType=="OP_TS_CDControl")
        {
            return OP_TS_CDControl;
        }
		return OP_TS_NO;
	}
	//得到条件结果
	void Execute(CMoveShape* pShape);
};

//对作用对象列表的操作
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
class COpDestObjs:public COperator
{
public:
	COpDestObjs(stModuParam *pParam);
	COpDestObjs(void *pParam);
	virtual ~COpDestObjs(void);

	enum Op_DestObj
	{
		OP_Dest_Obj_Add,//添加该对象到作用列表
		OP_Dest_Obj_Del,//删除一个目标
		OP_Dest_Obj_Clear,//清除作用对象列表
	};
private:
	Op_DestObj m_Op;
public:
	//初始化
	void InitBaseData(eObj Obj,string strName)
	{
		m_Obj = Obj;
		m_Op = GetDestObj(strName);
	};
	//得到条件结果
	void Execute(CMoveShape* pShape);

	Op_DestObj GetDestObj(string strName)
	{
		if (strName=="OP_Dest_Obj_Add")
		{
			return OP_Dest_Obj_Add;
		} 
		else if(strName=="OP_Dest_Obj_Del")
		{
			return OP_Dest_Obj_Del;
		}
		else if (strName=="OP_Dest_Obj_Clear")
		{
			return OP_Dest_Obj_Clear;
		}
		else
		{
			return OP_Dest_Obj_Add;
		}
		
	}
};
//技能
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class COpSkill:public COperator
{
public:
	COpSkill(stModuParam *pParam);
	COpSkill(void *pParam);
	virtual ~COpSkill(void);
private:
	ModuleType m_eType;
	long	   m_lID;
	long	   m_lLv;
	string	   m_strOperator;
public:
	void InitBaseData(eObj Obj,ModuleType eType,long lID,long lLv,string strOperator)
	{		
		m_Obj = Obj;
		m_eType = eType;
		m_lID = lID;
		m_lLv = lLv;
		m_strOperator = strOperator;
	}
	//技能操作
	void Execute(CMoveShape* pShape);
};


//脚本
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
class COpScript:public COperator
{
public:
	COpScript(stModuParam *pParam);
	COpScript(void *pParam);
	virtual ~COpScript(void);
private:
	//多个脚本程序
	typedef vector<CExpressions*>::iterator itExpress;
	vector<CExpressions*> m_Expressions;
public:
	void InitBaseData(eObj Obj,vector<CExpressions*>& vExpressions)
	{
		m_Obj = Obj;
		m_Expressions = vExpressions;
	}
	//得到条件结果
	void Execute(CMoveShape* pShape);
};

/// 提供一个使用技能的时候附加已学天赋附加状态
class COpGenius:public COperator
{
public:
    COpGenius(stModuParam *pParam);
    COpGenius(void *pParam);
    virtual ~COpGenius();

    void  InitBaseData(string strGenius)
    {
        m_strGenius = strGenius;
    }

    void  Execute(CMoveShape* pShape);

private:
    string  m_strGenius;
};


void AddOper(CMoveShape* pShape,string& strName,int nValue);
void SubOper(CMoveShape* pShape,string& strName,int nValue);
void SetOper(CMoveShape* pShape,string& strName,int nValue);
void BoolOper(CMoveShape* pShape,string& strName,int nValue);
void ResetOper(CMoveShape* pShape,string& strName,int nValue);
void UpdateOper(CMoveShape* pShape,string& strName,int nValue=1);

#endif
