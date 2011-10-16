
#include "StdAfx.h"

#include "LinkmanGroupSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//! 配置类
///////////////////////////////////////////////////////////////////////////////////////////////////
LinkmanGroupSetup::LinkmanGroupSetup(void)
{
	Reset();
}

LinkmanGroupSetup::~LinkmanGroupSetup(void)
{
}

//! 重置对象
void LinkmanGroupSetup::Reset(void)
{
	memset(this, 0, sizeof(LinkmanGroupSetup));
}

//! 从传入的XML节点中获取配置信息
BOOL LinkmanGroupSetup::LoadSetup(TiXmlElement* pGroupElement)
{
	if (NULL == pGroupElement) return FALSE;

	BOOL bRe = TRUE;
	const char *pSetupStr = NULL;
	//! ----------------------------基本----------------------------
	TiXmlElement* pBase = pGroupElement->FirstChildElement("Base");
	if (NULL == pBase) return FALSE;

	//! 配置定义的唯一ID
	m_uID = atoi(pBase->Attribute("ID"));

	//! 类型(eLinkmanGroupType)
	if (NULL != (pSetupStr = pBase->Attribute("GroupType")))
	{
		if		(0 == stricmp(pSetupStr, "eLGT_Friend"))	m_uGroupType = eLGT_Friend;
		else if (0 == stricmp(pSetupStr, "eLGT_Spouse"))	m_uGroupType = eLGT_Spouse;
		else if (0 == stricmp(pSetupStr, "eLGT_Stranger"))	m_uGroupType = eLGT_Stranger;
		else if (0 == stricmp(pSetupStr, "eLGT_Hellion"))	m_uGroupType = eLGT_Hellion;
		else if (0 == stricmp(pSetupStr, "eLGT_Blacklist"))	m_uGroupType = eLGT_Blacklist;
		else												m_uGroupType = eLGT_Invalid;
	}
	else
	{
		m_uGroupType = eLGT_Invalid;
		bRe = FALSE;
	}

	//! 本组成员的留言优先级（eLeaveWordPRI）
	m_uLeaveWordPRI = atoi(pBase->Attribute("LeaveWordPRI"));
	if (eLWPRI_UnSteadyest < m_uLeaveWordPRI) bRe = FALSE;


	//! 密聊频道类型（eSecretTalkChannel）
	m_uSecretTalkChannelType = atoi(pBase->Attribute("SecretTalkChannelType"));
	if (eSTC_BaseValue >= m_uSecretTalkChannelType || eSTC_MaxValue <= m_uSecretTalkChannelType) bRe = FALSE;

	//! 启用互加区别
	m_bPartMutual = FALSE;
	if (NULL != (pSetupStr = pBase->Attribute("PartMutual")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_bPartMutual = TRUE;
	}
	else
		bRe = FALSE;
	


	//! ----------------------------行为----------------------------
	TiXmlElement* pActive = pGroupElement->FirstChildElement("Active");
	if (NULL == pActive) return FALSE;

	//! 最大人数限制
	m_uMaxMemberNum = atoi(pActive->Attribute("MaxMemberNum"));
	//! 存储方式（eKeepWay）
	if (NULL != (pSetupStr = pActive->Attribute("KeepWay")))
	{
		if		(0 == stricmp(pSetupStr, "eKW_ServerKeep"))	m_uKeepWay = eKW_ServerKeep;
		else if (0 == stricmp(pSetupStr, "eKW_ClientKeep"))	m_uKeepWay = eKW_ClientKeep;
		else												m_uKeepWay = eKW_NotKeep;
	}
	else
	{
		m_uKeepWay = eKW_NotKeep;
		bRe = FALSE;
	}

	//! 即时显示信息（eTimelyInfoType）
	m_uTimelyInfo = eTIT_Hide_All;
	if (NULL != (pSetupStr = pActive->Attribute("TimelyInfo")))
	{	
		if (0 == stricmp(pSetupStr, "eTIT_Show_All"))	m_uTimelyInfo = eTIT_Show_All;
	}
	else
		bRe = FALSE;

	//! 屏蔽功能（eFunctionFiltrateType）
	if (NULL != (pSetupStr = pActive->Attribute("FunctionFiltrate")))
	{	
		if		(0 == stricmp(pSetupStr, "eFFT_UuFiltrate_All"))	m_uFunctionFiltrate = eFFT_UuFiltrate_All;
		else if (0 == stricmp(pSetupStr, "eFFT_Filtrate_Chat"))		m_uFunctionFiltrate = eFFT_Filtrate_Chat;
		else if (0 == stricmp(pSetupStr, "eFFT_Filtrate_Team"))		m_uFunctionFiltrate = eFFT_Filtrate_Team;
		else														m_uFunctionFiltrate = eFFT_Filtrate_All;
	}
	else
	{
		m_uFunctionFiltrate = eFFT_Filtrate_All;
		bRe = FALSE;
	}


	//! ----------------------------成员操作----------------------------
	TiXmlElement* pMemberOpt = pGroupElement->FirstChildElement("MemberOpt");
	if (NULL == pMemberOpt) return FALSE;

	//! 提供用户添加操作
	m_bUserAdd = FALSE;
	if (NULL != (pSetupStr = pMemberOpt->Attribute("UserAdd")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_bUserAdd = TRUE;
	}
	else
		bRe = FALSE;
	
	//! 自动添加办法（eAutoAddType）、、
	if (NULL != (pSetupStr = pMemberOpt->Attribute("AutoAddWay")))
	{
		if	(0 == stricmp(pSetupStr, "eAAT_SecretTalk"))	m_uAutoAddWay = eAAT_SecretTalk;
		else if (0 == stricmp(pSetupStr, "eAAT_Attacker"))	m_uAutoAddWay = eAAT_Attacker;
		else if (0 == stricmp(pSetupStr, "eAAT_Spouse"))	m_uAutoAddWay = eAAT_Spouse;
		else												m_uAutoAddWay = eAAT_NotAdd;
	}
	else
	{
		m_uAutoAddWay = eAAT_NotAdd;
		bRe = FALSE;
	}

	//! 添加时是否请求目标确认
	m_bAddAimAffirm = FALSE;
	if (NULL != (pSetupStr = pMemberOpt->Attribute("AddAimAffirm")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_bAddAimAffirm = TRUE;
	}
	else
		bRe = FALSE;

	//! 提供用户删除操作
	m_bUserDel = FALSE;
	if (NULL != (pSetupStr = pMemberOpt->Attribute("UserDel")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_bUserDel = TRUE;
	}
	else
		bRe = FALSE;

	//! 操作由谁完成（eGroupOperatorType）
	m_uOperater = eGOT_Client;
	if (NULL != (pSetupStr = pMemberOpt->Attribute("Operater")))
	{	
		if (0 == stricmp(pSetupStr, "eGOT_Server"))	m_uOperater = eGOT_Server;
	}
	else
		bRe = FALSE;

	//! 成员是否具有唯一性
	m_bMemberIsOnly = FALSE;
	if (NULL != (pSetupStr = pMemberOpt->Attribute("MemberIsOnly")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_bMemberIsOnly = TRUE;
	}
	else
		bRe = FALSE;

	//! 成员移动优先级（eOnlyPRT）
	m_uOnlyPRT = atoi(pMemberOpt->Attribute("OnlyPRT"));
	if (eOPRI_Steadyest > m_uOnlyPRT || eOPRI_UnSteadyest < m_uOnlyPRT) bRe = FALSE;

	//! 同级删除
	m_bSameDel = FALSE;
	if (NULL != (pSetupStr = pMemberOpt->Attribute("SameDel")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_bSameDel = TRUE;
	}
	else
		bRe = FALSE;



	//! ----------------------------提示----------------------------
	TiXmlElement* pNotify = pGroupElement->FirstChildElement("Notify");
	if (NULL == pNotify) return FALSE;

#ifndef USE_IN_SERVER
	//! 上线提示信息
	m_NotifySetup.uMessage_Login = FALSE;
	if (NULL != (pSetupStr = pNotify->Attribute("Message_Login")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_NotifySetup.uMessage_Login = TRUE;
	}
	else
		bRe = FALSE;

	//! 下线提示信息
	m_NotifySetup.uMessage_Logout = FALSE;
	if (NULL != (pSetupStr = pNotify->Attribute("Message_Logout")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_NotifySetup.uMessage_Logout = TRUE;
	}
	else
		bRe = FALSE;

	//! 上线提示音
	if (NULL != (pSetupStr = pNotify->Attribute("Music_Login")))
	{
		LONG lCpySize = (MAX_FILE_PATH_SIZE > strlen(pSetupStr)) ? strlen(pSetupStr) : MAX_FILE_PATH_SIZE;
		memmove(m_NotifySetup.szMusic_Login, pSetupStr, lCpySize);
	}
	else
		bRe = FALSE;

	//! 下线提示音
	if (NULL != (pSetupStr = pNotify->Attribute("Music_Logout")))
	{
		LONG lCpySize = (MAX_FILE_PATH_SIZE > strlen(pSetupStr)) ? strlen(pSetupStr) : MAX_FILE_PATH_SIZE;
		memmove(m_NotifySetup.szMusic_Logout, pSetupStr, lCpySize);
	}
	else
		bRe = FALSE;

	//! 收到密聊提示音
	if (NULL != (pSetupStr = pNotify->Attribute("Music_SecretTalk")))
	{
		LONG lCpySize = (MAX_FILE_PATH_SIZE > strlen(pSetupStr)) ? strlen(pSetupStr) : MAX_FILE_PATH_SIZE;
		memmove(m_NotifySetup.szMusic_SecretTalk, pSetupStr, lCpySize);
	}
	else
		bRe = FALSE;

#endif //! USE_IN_SERVER
	
	//! 向被添加的目标发送提示信息
	m_NotifySetup.bMessage_AddAim = FALSE;
	if (NULL != (pSetupStr = pNotify->Attribute("Message_AddAim")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_NotifySetup.bMessage_AddAim = TRUE;
	}
	else
		bRe = FALSE;

	//! 向被删除的目标发送提示信息
	m_NotifySetup.bMessage_DelAim = FALSE;
	if (NULL != (pSetupStr = pNotify->Attribute("Message_DelAim")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_NotifySetup.bMessage_DelAim = TRUE;
	}
	else
		bRe = FALSE;

	//! ----------------------------外观----------------------------
#ifndef USE_IN_SERVER
	TiXmlElement* pAppearance = pGroupElement->FirstChildElement("Appearance");
	if (NULL == pAppearance) return FALSE;

	//! 在线色
	m_ColorSetup.uNameColor_Online			= strtoul(pAppearance->Attribute("NameColor_Online"), NULL, 0);

	//! 离线色
	m_ColorSetup.uNameColor_Leave			= strtoul(pAppearance->Attribute("NameColor_Leave"), NULL, 0);

	//! 互加关系在线色
	m_ColorSetup.uNameColor_Online_Mutual	= strtoul(pAppearance->Attribute("NameColor_Online_Mutual"), NULL, 0);

	//! 角色已删除色
	m_ColorSetup.uNameColor_Player_Deleted	= strtoul(pAppearance->Attribute("NameColor_Leave_Mutual"), NULL, 0);

	//! 密聊显示色
	m_ColorSetup.uSecretTalkColor			= strtoul(pAppearance->Attribute("SecretTalkColor"), NULL, 0);

	//! 本组角色称呼
	if (NULL != (pSetupStr = pAppearance->Attribute("MemberAppellative")))
	{
		LONG lCpySize = (MAX_APPELLATIVE_SIZE > strlen(pSetupStr)) ? strlen(pSetupStr) : MAX_APPELLATIVE_SIZE;
		memmove(m_szMemberAppellative, pSetupStr, lCpySize);
	}
	else
		bRe = FALSE;

	//! 本组文件名
	if (NULL != (pSetupStr = pAppearance->Attribute("FileName")))
	{
		LONG lCpySize = (MAX_APPELLATIVE_SIZE > strlen(pSetupStr)) ? strlen(pSetupStr) : MAX_APPELLATIVE_SIZE;
		memmove(m_szFileName, pSetupStr, lCpySize);
	}
	else
		bRe = FALSE;
#endif //! USE_IN_SERVER

	return TRUE;
}
















//! 配置管理类
///////////////////////////////////////////////////////////////////////////////////////////////////
LGSManage::LGSManage(void)
{
	m_bLoadSucceed = FALSE;
}
LGSManage::~LGSManage(void)
{
}

//! 读取配置
BOOL LGSManage::LoadSetup (const char *pFilename)
{
	//! 如果配置载入成功，则不允许再对其修改
	if (m_bLoadSucceed) return FALSE;

	BOOL bRe = TRUE;
	//! ----------------------------根节点 此区间内的错误都应返回1~10----------------------------
	TiXmlNode *pNode = NULL; 		

	CRFile *prfile = rfOpen(pFilename);
	if(NULL == prfile)
	{
		//输出错误信息
		return FALSE;
	}

	TiXmlDocument m_Tdoc(pFilename);
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		rfClose(prfile);
		return FALSE;
	}
	rfClose(prfile);

	do 
	{
		pNode=m_Tdoc.FirstChild("LinkmanSetup");
		if (pNode==NULL) break;

		TiXmlElement* pLinkmanSetup = pNode->ToElement();//获取node 的指针
		if (pLinkmanSetup==NULL) break;

		//! 开始读取
		m_LinkmanGroupSetup.clear();
		LinkmanGroupSetup tmpLinkmanGroupSetup;

		//! ID重复验证用的集合
		set<LONG> idSet;

		TiXmlElement* pGroup = pLinkmanSetup->FirstChildElement("Group");
		for (;pGroup != NULL; pGroup = pGroup->NextSiblingElement("Group"))
		{
			tmpLinkmanGroupSetup.Reset();
			if(!tmpLinkmanGroupSetup.LoadSetup(pGroup)) bRe = FALSE;
			m_LinkmanGroupSetup.push_back(tmpLinkmanGroupSetup);
			idSet.insert(tmpLinkmanGroupSetup.m_uID);
		}

		if(m_LinkmanGroupSetup.size() > idSet.size()) bRe = FALSE;
		
	} while(FALSE);

	m_bLoadSucceed = (bRe) ? TRUE : FALSE;
	return bRe;
}


//! 得到配置的好友组数量
LONG LGSManage::GetGroupNum(void)
{
	if (m_bLoadSucceed) 
		return (LONG)m_LinkmanGroupSetup.size();
	return 0;
}

//! 按指定的顺序得到一个组的配置
const LinkmanGroupSetup* LGSManage::GetLinkmanGroupSetup_ByOrder(LONG lOrder)
{
	if (-1 < lOrder && lOrder < m_LinkmanGroupSetup.size() && m_bLoadSucceed)
			return &(m_LinkmanGroupSetup[lOrder]);
	return NULL;
}

//! 通过ID得到组所在顺序
LONG LGSManage::GetIndexByID(ULONG uID)
{
	for (LONG i = 0; i < m_LinkmanGroupSetup.size(); ++i)
	{
		if (m_LinkmanGroupSetup[i].m_uID == uID)
		{
			return i;
		}
	}
	return -1;
}

//! 通过组类型得到组所在顺序
LONG LGSManage::GetIndexByType(eLinkmanGroupType eRelationType)
{
	for (LONG i = 0; i < m_LinkmanGroupSetup.size(); ++i)
	{
		if (m_LinkmanGroupSetup[i].m_uGroupType == (LONG)eRelationType)
		{
			return i;
		}
	}
	return -1;
}



//!----------------------------------------------------------------------------------------------------------
#ifdef USE_IN_SERVER

//! 编码
void LGSManage::AddToByteArray(vector<BYTE> *pVecData)
{
	_AddToByteArray(pVecData, (LONG)m_bLoadSucceed);

	LONG lSize = m_LinkmanGroupSetup.size();
	_AddToByteArray(pVecData, lSize);
	for (LONG i = 0; i < lSize; ++i)
	{
		_AddToByteArray(pVecData, &(m_LinkmanGroupSetup[i]), sizeof(LinkmanGroupSetup));
	}
}

//! 解码
void LGSManage::DecordFromByteArray(BYTE* pByte, LONG& pos)
{
	m_bLoadSucceed = _GetLongFromByteArray(pByte, pos);
	if (m_bLoadSucceed)
	{
		m_LinkmanGroupSetup.clear();
		LONG lSize = _GetLongFromByteArray(pByte, pos);
		for (LONG i = 0; i < lSize; ++i)
		{
			LinkmanGroupSetup tmpLinkmanGroupSetup;
			_GetBufferFromByteArray(pByte, pos, &tmpLinkmanGroupSetup, sizeof(LinkmanGroupSetup));
			m_LinkmanGroupSetup.push_back(tmpLinkmanGroupSetup);
		}
	}
}

#endif //! USE_IN_SERVER
//!----------------------------------------------------------------------------------------------------------