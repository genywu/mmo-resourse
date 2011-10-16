
#include "StdAfx.h"
#include "PlayerDepot.h"
#include "Player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

PlayerDepot::PlayerDepot(void)
:
m_pPlayer(NULL),
m_bIsFree(false),
m_lInputPwdErrCount(0),
m_lFirstErrTime(0),
m_lThawDate(0),
m_uSessionRadius(0),
m_lSessionPosX(0),
m_lSessionPosY(0),
m_GoldDepot(GoodsSetup::GetGoldCoinIndex()),
m_GemDepot(GoodsSetup::GetSilverIndex())
{
	m_Primary.SetContainerVolume(PRIMARY_SIZE);
	//m_Primary2.SetContainerVolume(PRIMARY2_SIZE);
	memset(m_szPassword, 0, DEPOT_PASSWORD_ARRAY_LEN + 1);
	memset(m_arrHlodSubpack, 0, DEPOT_SUBPACK_NUM * sizeof(bool));
}

PlayerDepot::~PlayerDepot(void)
{
	Release();
}




///////////////////////////////////////////////////////////////////////////////////////////////
//! 编码
void PlayerDepot::AddToByteArray_ForServer(vector<BYTE> *pByteArray)
{
	//! 密码
	_AddToByteArray(pByteArray, m_szPassword, DEPOT_PASSWORD_ARRAY_LEN);
	//! 扩展栏位标识
	long lFlag = 0;
	for (long i = 0; i < DEPOT_SUBPACK_NUM; ++i)
	{
		if(m_arrHlodSubpack[i])
			lFlag |= (1 << i);
	}
	_AddToByteArray(pByteArray, lFlag);
	//! 冻结时间
	_AddToByteArray(pByteArray, m_lThawDate);

	//! 物品容器
	m_Primary.Serialize( pByteArray, true);
	//m_Primary2.Serialize( pByteArray, true);
	m_Secondary.Serialize( pByteArray, true);
	m_GoldDepot.Serialize(pByteArray,true);
	m_GemDepot.Serialize(pByteArray,true);
}

//! 解码
void PlayerDepot::GetFormByteArray_FromServer(BYTE *pByteArray, long &lPos)
{
	//! 密码
	_GetBufferFromByteArray(pByteArray, lPos, m_szPassword, DEPOT_PASSWORD_ARRAY_LEN);
	//! 扩展栏位标识
	long lFlag = _GetLongFromByteArray(pByteArray, lPos);
	for (long i = 0; i < DEPOT_SUBPACK_NUM; ++i)
	{
		m_arrHlodSubpack[i] = (0 != (lFlag & (1 << i)));
	}
	//! 冻结时间
	m_lThawDate = _GetLongFromByteArray(pByteArray, lPos);

#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(7831);
#endif
	//! 物品容器
	Clear();
	m_Primary.Unserialize(pByteArray, lPos);
//#ifdef __MEMORY_LEAK_CHECK__
//		CBaseObject::SetConstructFlag(7832);
//#endif
//	m_Primary2.Unserialize(pByteArray, lPos);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(7833);
#endif
	m_Secondary.Unserialize(pByteArray, lPos);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(7834);
#endif
	m_GoldDepot.Unserialize(pByteArray, lPos);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(7835);
#endif
	m_GemDepot.Unserialize(pByteArray, lPos);
}

//! 设置数据解码
void PlayerDepot::GetSetupFormByteArray(BYTE *pByteArray, long &lPos)
{
	//! 密码
	_GetBufferFromByteArray(pByteArray, lPos, m_szPassword, DEPOT_PASSWORD_ARRAY_LEN);
	//! 扩展栏位标识
	long lFlag = _GetLongFromByteArray(pByteArray, lPos);
	for (long i = 0; i < DEPOT_SUBPACK_NUM; ++i)
	{
		m_arrHlodSubpack[i] = (0 != (lFlag & (1 << i)));
	}
	//! 冻结时间
	m_lThawDate = _GetLongFromByteArray(pByteArray, lPos);
}

//!		编码
void PlayerDepot::CodeToDataBlock_ForServer(DBWriteSet& setWriteDB)
{
	//! 密码
	setWriteDB.AddToByteArray(m_szPassword, DEPOT_PASSWORD_ARRAY_LEN);
	//! 扩展栏位标识
	long lFlag = 0;
	for (long i = 0; i < DEPOT_SUBPACK_NUM; ++i)
	{
		if(m_arrHlodSubpack[i])
			lFlag |= (1 << i);
	}
	setWriteDB.AddToByteArray(lFlag);
	//! 冻结时间
	setWriteDB.AddToByteArray(m_lThawDate);

	//! 物品容器
	m_Primary.Serialize( setWriteDB, true);
	m_Secondary.Serialize( setWriteDB, true);
	m_GoldDepot.Serialize(setWriteDB,true);
	m_GemDepot.Serialize(setWriteDB,true);
}

//!		解码
void PlayerDepot::DecodeFormDataBlock_FromServer(DBReadSet& setReadDB)
{
	//! 密码
	setReadDB.GetBufferFromByteArray( m_szPassword, DEPOT_PASSWORD_ARRAY_LEN);
	//! 扩展栏位标识
	long lFlag = setReadDB.GetLongFromByteArray();
	for (long i = 0; i < DEPOT_SUBPACK_NUM; ++i)
	{
		m_arrHlodSubpack[i] = (0 != (lFlag & (1 << i)));
	}
	//! 冻结时间
	m_lThawDate = setReadDB.GetLongFromByteArray( );

#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(7831);
#endif
	//! 物品容器
	Clear();
	m_Primary.Unserialize(setReadDB);
//#ifdef __MEMORY_LEAK_CHECK__
//		CBaseObject::SetConstructFlag(7832);
//#endif
//	m_Primary2.Unserialize(pByteArray, lPos);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(7833);
#endif
	m_Secondary.Unserialize(setReadDB);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(7834);
#endif
	m_GoldDepot.Unserialize(setReadDB);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(7835);
#endif
	m_GemDepot.Unserialize(setReadDB);
}
//!		设置数据解码
void PlayerDepot::DecodeSetupFromDataBlock(DBReadSet& setReadDB)
{
	//! 密码
	setReadDB.GetBufferFromByteArray(m_szPassword, DEPOT_PASSWORD_ARRAY_LEN);
	//! 扩展栏位标识
	long lFlag =setReadDB.GetLongFromByteArray();
	for (long i = 0; i < DEPOT_SUBPACK_NUM; ++i)
	{
		m_arrHlodSubpack[i] = (0 != (lFlag & (1 << i)));
	}
	//! 冻结时间
	m_lThawDate = setReadDB.GetLongFromByteArray( );
}

//! 重置对象
void PlayerDepot::Clear(void)
{
	m_Primary.Release();
	m_Primary.SetContainerVolume(PRIMARY_SIZE);
	//m_Primary2.Release();
	m_Secondary.ReleaseAllGoods();
	m_GoldDepot.Release();
	m_GemDepot.Release();
}

//! 释放对象
void PlayerDepot::Release(void)
{
	m_pPlayer = NULL;
	Init();

	m_Primary.Release();
	//m_Primary2.Release();
	m_Secondary.Release();
	m_GoldDepot.Release();
	m_GemDepot.Release();
}

//! 初始化对象
void PlayerDepot::Init(void)
{
	m_pPlayer			= NULL;
	m_bIsFree			= false;
	m_lInputPwdErrCount = 0;
	m_lFirstErrTime		= 0;
	m_lThawDate			= 0;
	m_uSessionRadius	= 0;
	m_lSessionPosX		= 0;
	m_lSessionPosY		= 0;

	m_Primary.SetContainerVolume(PRIMARY_SIZE);
	//m_Primary2.SetContainerVolume(PRIMARY2_SIZE);
	memset(m_szPassword, 0, DEPOT_PASSWORD_ARRAY_LEN + 1);
	memset(m_arrHlodSubpack, 0, DEPOT_SUBPACK_NUM * sizeof(bool));
}
//! 绑定角色
void PlayerDepot::BindPlayer(CPlayer *pPlayer)
{
	m_pPlayer = pPlayer;
	if (NULL != pPlayer)
	{
	}
}

//! 得到容器列表
void PlayerDepot::GetGoodsContainerList(GoodsContainerList &ContainerList)
{
	ContainerList.clear();
	pair<CGoodsContainer*, long> tmpNode;

	tmpNode.first = &m_Primary;
	tmpNode.second = eDCT_Primary;
	ContainerList.push_back(tmpNode);

	//tmpNode.first = &m_Primary2;
	//tmpNode.second = eDCT_Primary2;
	//ContainerList.push_back(tmpNode);

	tmpNode.first = FindContainer(eDCT_Secondary1);
	if (NULL != tmpNode.first)
	{
		tmpNode.second = eDCT_Secondary1;
		ContainerList.push_back(tmpNode);
	}

	tmpNode.first = FindContainer(eDCT_Secondary2);
	if (NULL != tmpNode.first)
	{
		tmpNode.second = eDCT_Secondary2;
		ContainerList.push_back(tmpNode);
	}

	tmpNode.first = FindContainer(eDCT_Secondary3);
	if (NULL != tmpNode.first)
	{
		tmpNode.second = eDCT_Secondary3;
		ContainerList.push_back(tmpNode);
	}

	tmpNode.first = FindContainer(eDCT_Secondary4);
	if (NULL != tmpNode.first)
	{
		tmpNode.second = eDCT_Secondary4;
		ContainerList.push_back(tmpNode);
	}

	tmpNode.first = &m_Secondary;
	tmpNode.second = eDCT_Secondary;
	ContainerList.push_back(tmpNode);

	tmpNode.first = &m_GoldDepot;
	tmpNode.second = eDCT_Gold;
	ContainerList.push_back(tmpNode);

	tmpNode.first = &m_GemDepot;
	tmpNode.second = eDCT_Silver;
	ContainerList.push_back(tmpNode);
}


//! 得到密码
const char* PlayerDepot::GetPassword(void)
{
	return m_szPassword;
}

//! 设置密码
void PlayerDepot::SetPassword(const char* pPassword)
{
	if(NULL == pPassword) return;
	long lPwsSize = (long)strlen(pPassword);
	lPwsSize = (lPwsSize < DEPOT_PASSWORD_ARRAY_LEN) ? lPwsSize : DEPOT_PASSWORD_ARRAY_LEN;
	memset(m_szPassword, 0, DEPOT_PASSWORD_ARRAY_LEN);
	memmove(m_szPassword, pPassword, lPwsSize);
}

//! 得到子背包购买标识
long PlayerDepot::GetSubpackFlag(void)
{
	long lFlag = 0;
	for (long i = 0; i < DEPOT_SUBPACK_NUM; ++i)
	{
		if (m_arrHlodSubpack[i])
		{
			lFlag |= (1 << i);
		}
	}
	return lFlag;
}

//! 设置子背包购买与否
void PlayerDepot::SetSubpackFlag(long lFlag)
{
	for (long i = 0; i < DEPOT_SUBPACK_NUM; ++i)
	{
		m_arrHlodSubpack[i] = ((lFlag & (1 << i)) != 0);
	}
}

//! 得到解冻时间
long PlayerDepot::GetThawdate(void)
{
	return m_lThawDate;
}
//! 得到解冻时间
void PlayerDepot::SetThawdate(long lThawdate)
{
	m_lThawDate = lThawdate;
}














////////////////////////////////////////

tagDepotSetup PlayerDepot::c_DepotSetup;

bool PlayerDepot::LoadSetup(string filename)
{
	//! ----------------------------根节点 此区间内的错误都应返回1~10----------------------------
	TiXmlNode *pNode = NULL; 		

	CRFile *prfile = rfOpen(filename.c_str());
	if(NULL == prfile)
	{
		//输出错误信息
		return false;
	}

	TiXmlDocument m_Tdoc(filename.c_str());
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	pNode=m_Tdoc.FirstChild("PlayerDepot");
	if (pNode==NULL)
	{
		return false;
	}

	TiXmlElement* pDepot = pNode->ToElement();//获取node 的指针
	if (pDepot==NULL) 
	{
		return false;
	}
	//! ----------------------------获取子栏位价格----------------------------
	bool bRe = true;
	TiXmlElement* pSubDepotPrice = pDepot->FirstChildElement("SubDepotPrice");
	if (NULL == pSubDepotPrice)
	{
		return false;
	}

	const char *pTmpString = NULL;

	pTmpString = pSubDepotPrice->Attribute("Pos1_Price");
	if (NULL != pTmpString)
	{
		c_DepotSetup.arr_lSubpackPrice[0] = atoi(pTmpString);
	}
	else
	{
		bRe = false;
	}

	pTmpString = pSubDepotPrice->Attribute("Pos2_Price");
	if (NULL != pTmpString)
	{
		c_DepotSetup.arr_lSubpackPrice[1] = atoi(pTmpString);
	}
	else
	{
		bRe = false;
	}

	pTmpString = pSubDepotPrice->Attribute("Pos3_Price");
	if (NULL != pTmpString)
	{
		c_DepotSetup.arr_lSubpackPrice[2] = atoi(pTmpString);
	}
	else
	{
		bRe = false;
	}

	pTmpString = pSubDepotPrice->Attribute("Pos4_Price");
	if (NULL != pTmpString)
	{
		c_DepotSetup.arr_lSubpackPrice[3] = atoi(pTmpString);
	}
	else
	{
		bRe = false;
	}

	//! ----------------------------冻结设置----------------------------
	TiXmlElement* pFrostSetup = pDepot->FirstChildElement("FrostSetup");
	if (NULL == pFrostSetup)
	{
		return false;
	}

	pTmpString = pFrostSetup->Attribute("MaxErrCount");
	if (NULL != pTmpString)
	{
		c_DepotSetup.lMaxPwdErrCount = atoi(pTmpString);
	}
	else
	{
		bRe = false;
	}

	pTmpString = pFrostSetup->Attribute("FrostTime");
	if (NULL != pTmpString)
	{
		c_DepotSetup.lFrostTime = atoi(pTmpString) * 60;
	}
	else
	{
		bRe = false;
	}

	//! ----------------------------清除对象----------------------------
	return true;
}

const	tagDepotSetup* PlayerDepot::GetDepotSetup(void)
{
	return &c_DepotSetup;
}











////////////////////////////////////////
//! 找到容器对象
CGoodsContainer* PlayerDepot::FindContainer(long eContainerId)
{
	switch(eContainerId)
	{
	case eDCT_Primary:
		return &m_Primary;
	//case eDCT_Primary2:
	//	return &m_Primary2;
	case eDCT_Secondary:
		return &m_Secondary;
	case eDCT_Secondary1:
		return m_Secondary.GetSubpack(0)->pSubpackContainer;
		break;
	case eDCT_Secondary2:
		return m_Secondary.GetSubpack(1)->pSubpackContainer;
		break;
	case eDCT_Secondary3:
		return m_Secondary.GetSubpack(2)->pSubpackContainer;
		break;
	case eDCT_Secondary4:
		return m_Secondary.GetSubpack(3)->pSubpackContainer;
		break;
	case eDCT_Gold:
		return &m_GoldDepot;
	case eDCT_Silver:
		return &m_GemDepot;
	}

	return NULL;
}