///////////////////////////////////////////////////////////////////////////////
///  StateFactory.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\WorldServer\AppWorld\State
///
///  CREATED: 06/07/2007 12:50:01 PM by 陈先进
///
///  PURPOSE:   状态工厂类
///
///  COPYRIGHT NOTICE:   Copyright (C) Aurora Game
///
///  LAST CHANGED: $Date$
///
///  REVISION HISTORY:
///  $Log$
/// 

// StateFactory.h: interface for the CStateFactory class.
//
//////////////////////////////////////////////////////////////////////
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_STATE_FACTORY_INCLUDED
#define _INC_STATE_FACTORY_INCLUDED
#include "../../../../public/tinyxml.h"
#include "State.h"
#include <map>
using namespace std;


class CState;

class CStateFactory
{

private:
	//状态的临时列表
	static map<DWORD, CState*>					g_mStateMap;

public:

	//读取状态配置文件
	static HRESULT			LoadConfigration	( const string p );
	//通过节点载入
	static HRESULT			LoadConfigration	( TiXmlElement *pElem );
	//载入各个等级的状态
	static HRESULT			LoadLevelConfigration( TiXmlElement *pElem, CState *pState );
	//载入条件判断
	static HRESULT			LoadConditionConfigration( TiXmlElement *pElem, CState *pState );
	//载入开始基本状态配置
	static HRESULT			LoadPropertyConfigration(TiXmlElement *pElem, CState::tagState *ptgState);
	//载入开始状态配置
	static HRESULT			LoadBeginConfig(TiXmlElement *pElem,CState::tagState *ptgState);
	//载入事件驱动脚本
	static HRESULT		    LoadEventConfig(TiXmlElement *pElem,CState::tagState *ptgState);

	//载入基本状态配置
	static CState::tagFettle *LoadBaseProperty(TiXmlElement *pElem);//,CState::tagState *ptgState
	//霸入更新状态配置
	static HRESULT		    LoadTimeOutConfig(TiXmlElement *pElem,CState::tagState *ptgState);
	//霸入结束状态配置
	static HRESULT		    LoadEndConfig(TiXmlElement *pElem,CState::tagState *ptgState);


	//载入开始脚本
	static HRESULT			LoadScriptConfigration(TiXmlElement *pElem, CState::tagState *ptgState);
	//清除状态缓冲
	static void				ClearStateCache		();								
	//载入状态缓冲
	static bool				LoadStateCache		();
	//编码
	static bool				CodeState			( vector<BYTE>& vRet );	

};

#endif//_INC__STATE_FACTORY_INCLUDED