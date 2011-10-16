#include "StdAfx.h"
#include "LinkmanGroupSetup.h"
#include "../Common/Public.h"

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
bool LinkmanGroupSetup::LoadSetup(TiXmlElement* pGroupElement)
{
	if (NULL == pGroupElement) return false;

	bool bRe = true;
	const char *pSetupStr = NULL;
	//! ----------------------------基本----------------------------
	TiXmlElement* pBase = pGroupElement->FirstChildElement("Base");
	if (NULL == pBase) return false;

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
		bRe = false;
	}

	//! 本组成员的留言优先级（eLeaveWordPRI）
	m_uLeaveWordPRI = atoi(pBase->Attribute("LeaveWordPRI"));
	if (eLWPRI_UnSteadyest < m_uLeaveWordPRI) bRe = false;


	//! 密聊频道类型（eSecretTalkChannel）
	m_uSecretTalkChannelType = atoi(pBase->Attribute("SecretTalkChannelType"));
	if (eSTC_BaseValue >= m_uSecretTalkChannelType || eSTC_MaxValue <= m_uSecretTalkChannelType) bRe = false;

	//! 启用互加区别
	m_bPartMutual = false;
	if (NULL != (pSetupStr = pBase->Attribute("PartMutual")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_bPartMutual = true;
	}
	else
		bRe = false;
	


	//! ----------------------------行为----------------------------
	TiXmlElement* pActive = pGroupElement->FirstChildElement("Active");
	if (NULL == pActive) return false;

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
		bRe = false;
	}

	//! 即时显示信息（eTimelyInfoType）
	m_uTimelyInfo = eTIT_Hide_All;
	if (NULL != (pSetupStr = pActive->Attribute("TimelyInfo")))
	{	
		if (0 == stricmp(pSetupStr, "eTIT_Show_All"))	m_uTimelyInfo = eTIT_Show_All;
	}
	else
		bRe = false;

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
		bRe = false;
	}


	//! ----------------------------成员操作----------------------------
	TiXmlElement* pMemberOpt = pGroupElement->FirstChildElement("MemberOpt");
	if (NULL == pMemberOpt) return false;

	//! 提供用户添加操作
	m_bUserAdd = false;
	if (NULL != (pSetupStr = pMemberOpt->Attribute("UserAdd")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_bUserAdd = true;
	}
	else
		bRe = false;
	
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
		bRe = false;
	}

	//! 添加时是否请求目标确认
	m_bAddAimAffirm = false;
	if (NULL != (pSetupStr = pMemberOpt->Attribute("AddAimAffirm")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_bAddAimAffirm = true;
	}
	else
		bRe = false;

	//! 提供用户删除操作
	m_bUserDel = false;
	if (NULL != (pSetupStr = pMemberOpt->Attribute("UserDel")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_bUserDel = true;
	}
	else
		bRe = false;

	//! 操作由谁完成（eGroupOperatorType）
	m_uOperater = eGOT_Client;
	if (NULL != (pSetupStr = pMemberOpt->Attribute("Operater")))
	{	
		if (0 == stricmp(pSetupStr, "eGOT_Server"))	m_uOperater = eGOT_Server;
	}
	else
		bRe = false;

	//! 成员是否具有唯一性
	m_bMemberIsOnly = false;
	if (NULL != (pSetupStr = pMemberOpt->Attribute("MemberIsOnly")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_bMemberIsOnly = true;
	}
	else
		bRe = false;

	//! 成员移动优先级（eOnlyPRT）
	m_uOnlyPRT = atoi(pMemberOpt->Attribute("OnlyPRT"));
	if (eOPRI_Steadyest > m_uOnlyPRT || eOPRI_UnSteadyest < m_uOnlyPRT) bRe = false;

	//! 同级删除
	m_bSameDel = false;
	if (NULL != (pSetupStr = pMemberOpt->Attribute("SameDel")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_bSameDel = true;
	}
	else
		bRe = false;



	//! ----------------------------提示----------------------------
	TiXmlElement* pNotify = pGroupElement->FirstChildElement("Notify");
	if (NULL == pNotify) return false;

//#ifndef USE_IN_SERVER
	//! 上线提示信息
	m_NotifySetup.uMessage_Login = false;
	if (NULL != (pSetupStr = pNotify->Attribute("Message_Login")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_NotifySetup.uMessage_Login = true;
	}
	else
		bRe = false;

	//! 下线提示信息
	m_NotifySetup.uMessage_Logout = false;
	if (NULL != (pSetupStr = pNotify->Attribute("Message_Logout")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_NotifySetup.uMessage_Logout = true;
	}
	else
		bRe = false;

	//! 上线提示音
	if (NULL != (pSetupStr = pNotify->Attribute("Music_Login")))
	{
		ulong lCpySize = (MAX_FILE_PATH_SIZE > strlen(pSetupStr)) ? (ulong)strlen(pSetupStr) : MAX_FILE_PATH_SIZE;
		memmove(m_NotifySetup.szMusic_Login, pSetupStr, lCpySize);
	}
	else
		bRe = false;

	//! 下线提示音
	if (NULL != (pSetupStr = pNotify->Attribute("Music_Logout")))
	{
		ulong lCpySize = (MAX_FILE_PATH_SIZE > strlen(pSetupStr)) ? (ulong)strlen(pSetupStr) : MAX_FILE_PATH_SIZE;
		memmove(m_NotifySetup.szMusic_Logout, pSetupStr, lCpySize);
	}
	else
		bRe = false;

	//! 收到密聊提示音
	if (NULL != (pSetupStr = pNotify->Attribute("Music_SecretTalk")))
	{
		ulong lCpySize = (MAX_FILE_PATH_SIZE > strlen(pSetupStr)) ? (ulong)strlen(pSetupStr) : MAX_FILE_PATH_SIZE;
		memmove(m_NotifySetup.szMusic_SecretTalk, pSetupStr, lCpySize);
	}
	else
		bRe = false;

//#endif //! USE_IN_SERVER
	
	//! 向被添加的目标发送提示信息
	m_NotifySetup.bMessage_AddAim = false;
	if (NULL != (pSetupStr = pNotify->Attribute("Message_AddAim")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_NotifySetup.bMessage_AddAim = true;
	}
	else
		bRe = false;

	//! 向被删除的目标发送提示信息
	m_NotifySetup.bMessage_DelAim = false;
	if (NULL != (pSetupStr = pNotify->Attribute("Message_DelAim")))
	{	
		if (0 == stricmp(pSetupStr, "YES"))	m_NotifySetup.bMessage_DelAim = true;
	}
	else
		bRe = false;

	//! ----------------------------外观----------------------------
//#ifndef USE_IN_SERVER
	TiXmlElement* pAppearance = pGroupElement->FirstChildElement("Appearance");
	if (NULL == pAppearance) return false;

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
		ulong lCpySize = (MAX_APPELLATIVE_SIZE > strlen(pSetupStr)) ? (ulong)strlen(pSetupStr) : MAX_APPELLATIVE_SIZE;
		memmove(m_szMemberAppellative, pSetupStr, lCpySize);
	}
	else
		bRe = false;

	//! 本组文件名
	if (NULL != (pSetupStr = pAppearance->Attribute("FileName")))
	{
		ulong lCpySize = (MAX_APPELLATIVE_SIZE > strlen(pSetupStr)) ? (ulong)strlen(pSetupStr) : MAX_APPELLATIVE_SIZE;
		memmove(m_szFileName, pSetupStr, lCpySize);
	}
	else
		bRe = false;
//#endif //! USE_IN_SERVER

	return true;
}
















//! 配置管理类
///////////////////////////////////////////////////////////////////////////////////////////////////
LGSManage::LGSManage(void)
{
	m_bLoadSucceed = false;
}
LGSManage::~LGSManage(void)
{
}

//! 读取配置
bool LGSManage::LoadSetup (const char *pFilename)
{
	//! 如果配置载入成功，则不允许再对其修改
	if (m_bLoadSucceed) return false;

	bool bRe = true;
	//! ----------------------------根节点 此区间内的错误都应返回1~10----------------------------
	TiXmlNode *pNode = NULL; 		

	CRFile *prfile = rfOpen(pFilename);
	if(NULL == prfile)
	{
		//输出错误信息
		return false;
	}

	TiXmlDocument m_Tdoc(pFilename);
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		rfClose(prfile);
		return false;
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
		set<long> idSet;

		TiXmlElement* pGroup = pLinkmanSetup->FirstChildElement("Group");
		for (;pGroup != NULL; pGroup = pGroup->NextSiblingElement("Group"))
		{
			tmpLinkmanGroupSetup.Reset();
			if(!tmpLinkmanGroupSetup.LoadSetup(pGroup)) bRe = false;
			m_LinkmanGroupSetup.push_back(tmpLinkmanGroupSetup);
			idSet.insert(tmpLinkmanGroupSetup.m_uID);
		}

		if(m_LinkmanGroupSetup.size() > idSet.size()) bRe = false;
		
	} while(false);

	m_bLoadSucceed = (bRe) ? true : false;
	return bRe;
}


//! 得到配置的好友组数量
ulong LGSManage::GetGroupNum(void)
{
	if (m_bLoadSucceed) 
		return (ulong)m_LinkmanGroupSetup.size();
	return 0;
}

//! 按指定的顺序得到一个组的配置
const LinkmanGroupSetup* LGSManage::GetLinkmanGroupSetup_ByOrder(ulong lOrder)
{
	if (lOrder < m_LinkmanGroupSetup.size() && m_bLoadSucceed)
			return &(m_LinkmanGroupSetup[lOrder]);
	return NULL;
}

//! 通过ID得到组所在顺序
ulong LGSManage::GetIndexByID(ulong uID)
{
	for (ulong i = 0; i < m_LinkmanGroupSetup.size(); ++i)
	{
		if (m_LinkmanGroupSetup[i].m_uID == uID)
		{
			return i;
		}
	}
	return -1;
}

//! 通过组类型得到组所在顺序
ulong LGSManage::GetIndexByType(eLinkmanGroupType eRelationType)
{
	for (ulong i = 0; i < m_LinkmanGroupSetup.size(); ++i)
	{
		if (m_LinkmanGroupSetup[i].m_uGroupType == (long)eRelationType)
		{
			return i;
		}
	}
	return -1;
}



//!----------------------------------------------------------------------------------------------------------
//#ifdef USE_IN_SERVER

//! 编码
void LGSManage::AddToByteArray(vector<uchar> *pVecData)
{
	_AddToByteArray(pVecData, (long)m_bLoadSucceed);

	long lSize = (long)m_LinkmanGroupSetup.size();
	_AddToByteArray(pVecData, lSize);
	for (long i = 0; i < lSize; ++i)
	{
		_AddToByteArray(pVecData, &(m_LinkmanGroupSetup[i]), sizeof(LinkmanGroupSetup));
	}
}

//! 解码
void LGSManage::DecordFromByteArray(uchar* pByte, long& pos)
{
	m_bLoadSucceed = _GetLongFromByteArray(pByte, pos)>0 ? true:false;
	if (m_bLoadSucceed)
	{
		m_LinkmanGroupSetup.clear();
		long lSize = _GetLongFromByteArray(pByte, pos);
		for (long i = 0; i < lSize; ++i)
		{
			LinkmanGroupSetup tmpLinkmanGroupSetup;
			_GetBufferFromByteArray(pByte, pos, &tmpLinkmanGroupSetup, sizeof(LinkmanGroupSetup));
			m_LinkmanGroupSetup.push_back(tmpLinkmanGroupSetup);
		}
	}
}

//#endif //! USE_IN_SERVER
//!----------------------------------------------------------------------------------------------------------