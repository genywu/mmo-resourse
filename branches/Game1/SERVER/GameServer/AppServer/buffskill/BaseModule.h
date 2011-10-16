//=============================================================================
/**
*  file: BaseModule.h
*
*  Brief:封装了状态、技能的基本模块功能
*
*  Authtor:wangqiao
*	
*	Datae:2007-8-31
*/
//=============================================================================

#ifndef __BASE_MODULE_H__
#define __BASE_MODULE_H__

#pragma once

#include "BuffSkillDef.h"

class CMoveShape;
class CScript;
class CProcedure;

class CBaseModule : public BaseMemObj
{
public:
	CBaseModule(stModuParam *pParam,stStaticModuParam *pStatiParam=NULL);
	virtual ~CBaseModule(void);
private:
	//ID和等级
	long m_nID;
	int  m_nLvl;
	//动态参数
	stModuParam	*m_pParam;
	//静态参数
	stStaticModuParam *m_pStatiParam;
	//操作工序集合
	typedef vector<CProcedure*>::iterator itPro;
	vector<CProcedure*>	m_Procedures;	
private:	
	//设置施放者
	void SetFireObjs();
	//处理工序的返回结果
	eModuleRet NextProcedure();
	//连续执行所有工序 
	eModuleRet DoSome(eModuleRet eRet);

public:

	//初始化数据
	void Initial();	

	//获取ID
	long				GetID()				{return m_nID;}		
	//获取等级	
	int					GetLv()				{return m_nLvl;}	
	
	//静态参数
	stStaticModuParam *GetStaticParam()			{return m_pStatiParam;}			
	void			   SetStaticParam(stStaticModuParam* pParam)	{m_pStatiParam=pParam;}
	//动态参数
	stModuParam		   *GetParam()				{return m_pParam;}	
	void			   SetParam(stModuParam *pParam)				{m_pParam= pParam;}

	void ProcNumSub();	
		
	//初始化基本数据
	void InitBaseData(vector<CProcedure*> &vProcedure);
						
	// 添加到CByteArray
	bool AddToByteArray(vector<BYTE>* pByteArray);
	// 添加到CByteArray为发送到客户段
	bool AddToByteArrayForClient(vector<BYTE>* pByteArray,ModuleType eType);
	// 解码
	bool DecordFromByteArray(long lProceNum,list<stTimerParam*>& Timers);

	// 添加到DataBlock
	bool CodeToDataBlock(DBWriteSet& setWriteDB);
	// 到添加到DataBlock为客户端
	bool CodeToDataBlockForClient(DBWriteSet& setWriteDB,ModuleType eType);
	// 解码
	bool DecodeFromDataBlock(long lProceNum,list<stTimerParam*>& Timers);
	
	//功能接口///////////////////////////////////////////////////
	//开始
	eModuleRet Begin(long lMouseX=0,long lMouseY=0,long lMouseShapeType=0,const CGUID& MouseShapeID=NULL_GUID,long lCreateID=0);
	//事件处理接口
	eModuleRet Proc(DWORD type,const tagProcVar *pVar);
	//超时处理接口
	eModuleRet OnTimeOut(DWORD timerid,DWORD curtime,const void* var);
	//结束调用接口
	eModuleRet End();
};

#endif