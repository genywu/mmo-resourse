#include "stdafx.h"
#include "../../Engine/Render.h"
#include "Player.h"
#include "MoveShape.h"
#include "FaceActionList.h"
#include "ClientRegion.h"
#include "../GameClient/Game.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 * 功能: 构造函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.06.16
 */
CFaceActionList::CFaceActionList(void)
{
}



/*
 * 功能: 析构函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.06.16
 */
CFaceActionList::~CFaceActionList(void)
{
	Release();
}



/*
 * 功能: 根据传入的字符串得到玩家仪态ID
 * 摘要: -
 * 参数: pszStr - 仪态配置文件所在的目录
 * 返回值: 成功 - 返回ID
 *		   失败 - 没找到则返回0
 * 作者: lpf
 * 创建日期: 2008.06.16
 * 修改日志:
 */
DWORD CFaceActionList::GetPlayerFaceActionID(const char * pszStr)
{
	for (size_t st = 0; st < m_vecPlayerFaceAction.size(); ++st)
	{
		if (m_vecPlayerFaceAction[st].sCallStr == pszStr)
			return (DWORD)(st + 1);
	}

	return 0;
}



/*
 * 功能: 根据传入的ID得到玩家仪态动作
 * 摘要: -
 * 参数: dwID - 动作ID号1开始
 * 返回值: 成功 - 返回玩家仪态指针
 *		   失败 - 返回NULL
 * 作者: lpf
 * 创建日期: 2008.06.16
 * 修改日志:
*	2009.04.10 - lpf
*		修改了传入ID范围越界判断条件;
 */
tagFaceAction * CFaceActionList::GetPlayerFaceAction(DWORD dwID)
{
	if ((int)dwID < 1 || dwID > m_vecPlayerFaceAction.size())
		return NULL;

	return &(m_vecPlayerFaceAction[dwID - 1]);
}



/*
 * 功能: 根据传入的ID得到怪物仪态动作
 * 摘要: -
 * 参数: dwID - 动作ID号1开始
 * 返回值: 成功 - 返回怪物仪态指针
 *		   失败 - 返回NULL
 * 作者: lpf
 * 创建日期: 2008.06.16
 * 修改日志:
*	2009.04.10 - lpf
*		修改了传入ID范围越界判断条件;
 */
tagFaceAction * CFaceActionList::GetMonsterFaceAction(DWORD dwID)
{
	if ((int)dwID < 1 || dwID > m_vecMonsterFaceAction.size())
		return NULL;

	return &(m_vecMonsterFaceAction[dwID - 1]);
}



/*
 * 功能: 根据传入的动作文件索引得到动作列表
 * 摘要: -
 * 参数: dwIndex - 动作索引0开始
 * 返回值: 成功 - 返回怪物仪态指针
 *		   失败 - 返回NULL
 * 作者: lpf
 * 创建日期: 2008.06.16
 * 修改日志:
*	2009.04.10 - lpf
*		修改了传入索引范围越界判断条件;
 */
tagActionList * CFaceActionList::GetActionList(DWORD dwIndex)
{
	if ((int)dwIndex < 0 || dwIndex >= m_vecPlayerActionList.size())
		return NULL;

	return &m_vecPlayerActionList[dwIndex];
}



/*
 * 功能: 载入仪态配置文件
 * 摘要: 仪态配置文件有两个,一个是关于玩家的,另一个是关于怪物的,需要分别载入
 * 参数: pszFileDir - 仪态配置文件所在的目录
 * 返回值: 成功 - true
 *		   失败 - false
 * 作者: lpf
 * 创建日期: 2008.06.16
 * 修改日志:
 *	2008.06.05 - lpf
 *		修改了表情动作所需要的图片资源文件的载入策略,即不同ID的玩家动作或者怪物动作可以共享同一个表情图片资源文件
 *	2008.11.11 - lpf
 *		根据新的需求,增加了动作列表文件功能,修改了相应的载入规则
 *	2009.03.14 - lpf
 *		修正了载入表情动画时的内存泄露
 */
bool CFaceActionList::LoadFaceAction(const char * pszFileDir)
{
	string				sFileName  = pszFileDir;
	CRFile			  * pRFile	   = NULL;
	stringstream		streamPLayer;
	stringstream		streamMonster;
	tagFaceAction		stPlayerTmp;
	tagFaceAction		stMonsterTmp;
	render::Interface * pInterface = render::Interface::GetInstance();

	// 载入玩家仪态文件
	pRFile = rfOpen((sFileName + "/PlayerAction.ini").c_str());
	if (pRFile == NULL)
	{
		rfClose(pRFile);
		return false;
	}
	pRFile->ReadToStream(streamPLayer);
	rfClose(pRFile);

	// 清空之前载入的图片资源
	//for (size_t st = 0; st < m_vecPlayerFaceAction.size(); ++st)
	//	pInterface->DestroyTexture(m_vecPlayerFaceAction[st].dwFaceFileHandle);

	m_vecPlayerFaceAction.clear();

	// 载入玩家仪态信息
	while (ReadTo(streamPLayer, "*"))
	{
		streamPLayer>>stPlayerTmp.dwID
					>>stPlayerTmp.sCallStr
					>>stPlayerTmp.sNote

					>>stPlayerTmp.sActionFileName
					>>stPlayerTmp.bPlayInWater

					//>>stPlayerTmp.dwFaceCount
					//>>stPlayerTmp.sFaceFileName
					//>>stPlayerTmp.dwFaceStartIndex
					//>>stPlayerTmp.dwFaceEndIndex
					//>>stPlayerTmp.dwFaceInterval

					>>stPlayerTmp.sShowTextTarget
					>>stPlayerTmp.sShowTextNoTarget;

		//if (stPlayerTmp.sAction != "0")
		//	stPlayerTmp.sAction.erase(0, 1);

/*		if (stPlayerTmp.sFaceFileName != "0")
		{
			bool bIsFound = false;

			// 查询是否已经创建了该表情纹理
			for (DWORD dw = 0; dw < m_vecPlayerFaceAction.size(); ++dw)
			{
				if (m_vecPlayerFaceAction[dw].sFaceFileName == stPlayerTmp.sFaceFileName)
				{
					bIsFound = true;
					stPlayerTmp.dwFaceFileHandle = m_vecPlayerFaceAction[dw].dwFaceFileHandle;
					break;
				}
			}

			if (!bIsFound)
				stPlayerTmp.dwFaceFileHandle = pInterface->CreateTexture((string("pictures/face/") + stPlayerTmp.sFaceFileName + ".tga").c_str());
		}else
			stPlayerTmp.dwFaceFileHandle = 0;*/

		stPlayerTmp.dwFaceFileHandle = 0;

		if (stPlayerTmp.sActionFileName != "0")
		{
			bool bIsFound = false;

			// 查询是否已经创建了该动作列表
			for (DWORD dw = 0; dw < m_vecPlayerFaceAction.size(); ++dw)
			{
				if (m_vecPlayerFaceAction[dw].sActionFileName == stPlayerTmp.sActionFileName)
				{
					bIsFound = true;
					stPlayerTmp.iActionIndex = m_vecPlayerFaceAction[dw].iActionIndex;
					break;
				}
			}

			if (!bIsFound)
			{
				string sFileTmp = sFileName + "/";
				sFileTmp += stPlayerTmp.sActionFileName;
				sFileTmp += ".ini";
				if (LoadActionList(sFileTmp))
					stPlayerTmp.iActionIndex = (int)m_vecPlayerActionList.size() - 1;
				else
				{
					stPlayerTmp.sActionFileName = "0";
					stPlayerTmp.iActionIndex = -1;
				}
			}
		}else
			stPlayerTmp.iActionIndex = -1;

		m_vecPlayerFaceAction.push_back(stPlayerTmp);
	}

	// 载入怪物仪态文件
	pRFile = rfOpen((sFileName + "/MonsterAction.ini").c_str());
	if (pRFile == NULL)
	{
		rfClose(pRFile);
		return false;
	}
	pRFile->ReadToStream(streamMonster);
	rfClose(pRFile);

	// 清空之前载入的图片资源
	for (size_t st = 0; st < m_vecMonsterFaceAction.size(); ++st)
		pInterface->DestroyTexture(m_vecMonsterFaceAction[st].dwFaceFileHandle);

	m_vecMonsterFaceAction.clear();

	// 载入仪态信息并创建相应的图片资源
	while (ReadTo(streamMonster, "*"))
	{
		streamMonster>>stMonsterTmp.dwID
					 >>stMonsterTmp.sNote

					 >>stMonsterTmp.sActionFileName

					 >>stMonsterTmp.dwFaceCount
					 >>stMonsterTmp.sFaceFileName
					 >>stMonsterTmp.dwFaceStartIndex
					 >>stMonsterTmp.dwFaceEndIndex
					 >>stMonsterTmp.dwFaceInterval

					 >>stMonsterTmp.sShowTextTarget
					 >>stMonsterTmp.sShowTextNoTarget;

		//if (stMonsterTmp.sActionFileName != "0")
			//stMonsterTmp.sActionFileName.erase(0, 1);

		if (stMonsterTmp.sFaceFileName != "0")
		{
			bool bIsFound = false;

			// 查询是否已经创建了该表情纹理(怪物的表情图片文件可能和玩家一样)
			for (DWORD dw = 0; dw < m_vecPlayerFaceAction.size(); ++dw)
			{
				if (m_vecPlayerFaceAction[dw].sFaceFileName == stMonsterTmp.sFaceFileName)
				{
					bIsFound = true;
					stMonsterTmp.dwFaceFileHandle = m_vecPlayerFaceAction[dw].dwFaceFileHandle;
					break;
				}
			}

			if (!bIsFound)
			{
				// 查询是否已经创建了该表情纹理(然后再搜索怪物表情文件)
				for (DWORD dw = 0; dw < m_vecMonsterFaceAction.size(); ++dw)
				{
					if (m_vecMonsterFaceAction[dw].sFaceFileName == stMonsterTmp.sFaceFileName)
					{
						bIsFound = true;
						stMonsterTmp.dwFaceFileHandle = m_vecMonsterFaceAction[dw].dwFaceFileHandle;
						break;
					}
				}
			}

			if (!bIsFound)
				stMonsterTmp.dwFaceFileHandle = pInterface->CreateTexture((string("pictures/face/") + stMonsterTmp.sFaceFileName + ".tga").c_str());
		}else
			stMonsterTmp.dwFaceFileHandle = 0;

		if (stMonsterTmp.sActionFileName != "0")
		{
			bool bIsFound = false;

			// 查询是否已经创建了该动作列表
			for (DWORD dw = 0; dw < m_vecPlayerFaceAction.size(); ++dw)
			{
				if (m_vecPlayerFaceAction[dw].sActionFileName == stMonsterTmp.sActionFileName)
				{
					bIsFound = true;
					stMonsterTmp.iActionIndex = m_vecPlayerFaceAction[dw].iActionIndex;
					break;
				}
			}

			if (!bIsFound)
			{
				for (DWORD dw = 0; dw < m_vecMonsterFaceAction.size(); ++dw)
				{
					if (m_vecMonsterFaceAction[dw].sActionFileName == stMonsterTmp.sActionFileName)
					{
						bIsFound = true;
						stMonsterTmp.iActionIndex = m_vecMonsterFaceAction[dw].iActionIndex;
						break;
					}
				}
			}

			if (!bIsFound)
			{
				string sFileTmp = sFileName + "/";
				sFileTmp += stMonsterTmp.sActionFileName;
				sFileTmp += ".ini";
				if (LoadActionList(sFileTmp))
					stMonsterTmp.iActionIndex = (int)m_vecPlayerActionList.size() - 1;
				else
				{
					stMonsterTmp.sActionFileName = "0";
					stMonsterTmp.iActionIndex = -1;
				}
			}
		}else
			stMonsterTmp.iActionIndex = -1;

		m_vecMonsterFaceAction.push_back(stMonsterTmp);
	}

	return true;
}



/*
 * 功能: 载入玩家动作配置
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.11.04
 * 修改日志:
 *	2008.11.17 - lpf
 *		判断了如果动作列表配置文件中没有特效ID定位符号"#",则直接返回
 */
bool CFaceActionList::LoadActionList(const string & sFilePath)
{
	stringstream  stream;
	tagActionDec  stActionDec;
	tagActionList stActionList;

	CRFile * pActionFile = rfOpen(sFilePath.c_str());
	if (pActionFile == NULL)
	{
		rfClose(pActionFile);
		return false;
	}

	pActionFile->ReadToStream(stream);
	rfClose(pActionFile);

	// 载入绑定特效信息
	string sLocator;
	if (ReadTo(stream, "#"))
	{
		stream	>>	stActionList.dwIdEffect
				>>	stActionList.dwStartTime
				>>	stActionList.bIsLoop
				>>	sLocator;
	}else
	{
		rfClose(pActionFile);
		return false;
	}

	if (sLocator == "WIND")					//胸口
		stActionList.eLocator = MAKEFOURCC('0','0','0','0');
	else if (sLocator == "HEAD")			//头顶
		stActionList.eLocator = MAKEFOURCC('0','0','0','1');
	else if (sLocator == "FOOTCENTER")		//脚底中心
		stActionList.eLocator = MAKEFOURCC('0','0','0','2');
	else if (sLocator == "LEFTHAND")		//左手
		stActionList.eLocator = MAKEFOURCC('0','0','0','3');
	else if (sLocator == "RIGHTHAND")		//右手
		stActionList.eLocator = MAKEFOURCC('0','0','0','4');
	else									//默认为胸口
		stActionList.eLocator = MAKEFOURCC('0','0','0','0');

	while (ReadTo(stream, "*"))
	{
		stream	>>	stActionDec.sAction
				>>	stActionDec.iLoopCount;

		if (stActionDec.sAction != "0")
			stActionDec.sAction.erase(0, 1);

		stActionList.m_vecActionDecList.push_back(stActionDec);
	}

	m_vecPlayerActionList.push_back(stActionList);

	return true;
}



/*
 * 功能: 释放仪态列表
 * 摘要: 该函数请在注意调用,以释放资源
 *		 在该类的析构函数中也调用了该函数
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.06.16
 * 修改日志:
 *	2008.11.11 - lpf
 *		增加了玩家仪态动作列表的释放
 */
void CFaceActionList::Release()
{
	render::Interface * pInterface = render::Interface::GetInstance();

	// 清空载入的图片资源
	for (size_t st = 0; st < m_vecPlayerFaceAction.size(); ++st)
		pInterface->DestroyTexture(m_vecPlayerFaceAction[st].dwFaceFileHandle);
	m_vecPlayerFaceAction.clear();

	for (size_t st = 0; st < m_vecPlayerActionList.size(); ++st)
		m_vecPlayerActionList[st].m_vecActionDecList.clear();
	m_vecPlayerActionList.clear();

	for (size_t st = 0; st < m_vecMonsterFaceAction.size(); ++st)
		pInterface->DestroyTexture(m_vecMonsterFaceAction[st].dwFaceFileHandle);
	m_vecMonsterFaceAction.clear();
}



/*
 * 功能: 设定仪态动作
 * 摘要: 通过该函数可以自动设定仪态动作播放前的设定,设定的对象则自动会根据设定开始播放仪态动作
 * 参数: dwFaceActionID - 需要播放的仪态动作ID
 *		 pSourceObj		- 源播放者对象
 *		 lSourceObjType - 源播放者对象类型
 *		 pTargetObj		- 目标播放对象
 *		 lTargetObjType - 目标播放对象类型
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.06.16
 * 修改日志:
 *	2008.11.11 - lpf
 *		增加了玩家仪态动作播放规则的限制
 *	2008.11.28 - lpf
 *		增加了玩家吟唱状态不能播放仪态动作播放限制
 *		当玩家点击自己播放仪态动作时，聊天消息被默认为没有目标的情况
 *	2008.12.29 - lpf
 *		增加了玩家战斗状态不能播放仪态动作播放限制
 *	2009.03.25 - lpf
 *		播放限制放在了新的成员函数IsCanPlay()中
 *	2009.04.03 - lpf
 *		在"没有说话消息的判断"中增加了,当玩家处于贩卖状态则不需要聊天消息的添加
 *	2009.07.01 - lk
 *		发送仪态动作和接受仪态动作的对象是玩家，且玩家隐藏了个人信息，则提示名字变成‘匿名者’
 */
void CFaceActionList::SetupFaceAction(DWORD dwFaceActionID, CMoveShape * pSourceObj, long lSourceObjType, CMoveShape * pTargetObj, long lTargetObjType)
{
	string			sChatMsg;
	tagFaceAction * stpFaceAction = NULL;

	// 如果源对象为空则直接返回
	if (pSourceObj == NULL)
		return;

	// 判断并得到需要播放的仪态动作
	if (lSourceObjType == TYPE_PLAYER)
		stpFaceAction = GetPlayerFaceAction(dwFaceActionID);
	else if (lSourceObjType == TYPE_MONSTER)
		stpFaceAction = GetMonsterFaceAction(dwFaceActionID);
	else
		return; 

	// 判断表情动画数据是否得到了
	if (stpFaceAction == NULL)
		return;

	// 设定表情动画
	if (stpFaceAction->dwFaceFileHandle != 0 && lSourceObjType != TYPE_PLAYER)
		pSourceObj->SetupFaceAnim(stpFaceAction);

	// 设定动作
	if (stpFaceAction->sActionFileName != "0")
	{
		//if (pSourceObj->GetAction() == CShape::ACT_STAND || pSourceObj->GetAction() == CShape::ACT_IDLE)
		pSourceObj->SetupActionAnim(stpFaceAction);
	}

	// 处理聊天消息
	if (pTargetObj == NULL || pTargetObj == pSourceObj)
	{
		sChatMsg = stpFaceAction->sShowTextNoTarget;
	}else
		sChatMsg = stpFaceAction->sShowTextTarget;

	// 没有说话消息的判断
	if (sChatMsg == "0" || ((CPlayer *)pSourceObj)->IsPlayerShopOpened())
		return;

	string strSouName = "";
	if(pSourceObj)
		strSouName = pSourceObj->GetName();
	string strTargName = "";
	if(pTargetObj)
		strTargName = pTargetObj->GetName();
	// 发送仪态动作和接受仪态动作的对象是玩家，且玩家隐藏了个人信息，则提示名字变成‘匿名者’
	CPlayer * pMainPlayer = GetGame()->GetMainPlayer();
	if(pMainPlayer)
	{
		if(lSourceObjType == TYPE_PLAYER && pMainPlayer != pSourceObj)
		{
			// 队友
			if(pMainPlayer->IsTeammate(pSourceObj->GetExID()))
			{
				tagWSMemberInfo * stInfo = pMainPlayer->GetWsMemberInfo(pSourceObj->GetExID());
				if(stInfo)
					strSouName = stInfo->szName;
			}
		}
		if(lTargetObjType == TYPE_PLAYER && pMainPlayer != pTargetObj)
		{
			// 队友
			if(pMainPlayer->IsTeammate(pTargetObj->GetExID()))
			{
				tagWSMemberInfo * stInfo = pMainPlayer->GetWsMemberInfo(pTargetObj->GetExID());
				if(stInfo)
					strTargName = stInfo->szName;
			}
		}
	}

	// 重组聊天信息
	while (true)
	{
		size_t st = sChatMsg.find("%");
		if (st == string::npos)
			break;

		if (sChatMsg[st + 1] == 'M')
			sChatMsg.replace(st, 2, strSouName.c_str());
		else if (sChatMsg[st + 1] == 'T')
			sChatMsg.replace(st, 2, strTargName.c_str());
	}

	// 添加聊天信息到主页面聊天列表中
	//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(sChatMsg.c_str());
}



/*
 * 功能: 检查是否可以播放指定的仪态动作
 * 摘要: -
 * 参数: dwFaceActionID - 需要播放的仪态动作ID
 *		 pSourceObj		- 源播放者对象
 *		 lSourceObjType - 源播放者对象类型
 * 返回值: true  - 可以播放
 *		   false - 不能播放
 * 作者: lpf
 * 创建日期: 2009.03.25
 * 修改日志:
*	2009.04.10 - lpf
*		在"得到玩家仪态动作信息"增加了返回指针为空的判断;
 */
bool CFaceActionList::IsCanPlay(DWORD dwFaceActionID, CMoveShape * pSourceObj, long lSourceObjType)
{
	tagFaceAction * stpFaceAction = NULL;

	// 判断并得到需要播放的仪态动作
	if (lSourceObjType == TYPE_PLAYER)
	{
		CPlayer * pPlayer = dynamic_cast<CPlayer *>(pSourceObj);
		if (pPlayer)
		{
			if (pPlayer->GetState() == CShape::STATE_FIGHT)			//战斗状态播放不能
			{
				//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
				//													CStringReading::LoadText(eIDS_CONTROL, eIDS_CONTROL_CANTFACEACTION_AT_FIGHT),
				//													D3DCOLOR_ARGB(255, 255, 0, 0));
				return false;
			}if (pPlayer->IsRide())		//乘骑状态播放不能
			{
				//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
				//													CStringReading::LoadText(eIDS_CONTROL, eIDS_CONTROL_CANTFACEACTION_ON_RIDE),
				//													D3DCOLOR_ARGB(255, 255, 0, 0));
				return false;
			}else if (pPlayer->GetIntonateKey() != 0)		//吟唱状态播放不能
			{
				//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
				//													CStringReading::LoadText(eIDS_CONTROL, eIDS_CONTROL_CANTFACEACTION_AT_CURRENT),
				//													D3DCOLOR_ARGB(255, 255, 0, 0));
				return false;
			}
		}else
			return false;

		// 得到玩家仪态动作信息
		stpFaceAction = GetPlayerFaceAction(dwFaceActionID);
		if (stpFaceAction == NULL)
			return false;

		if (!stpFaceAction->bPlayInWater && (GetGame()->GetRegion()->GetSpaceType() == RST_Warter) )			//水下状态播放判断
		{
			//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
			//													CStringReading::LoadText(eIDS_CONTROL, eIDS_CONTROL_CANTFACEACTION_IN_WATER),
			//													D3DCOLOR_ARGB(255, 255, 0, 0));
			return false;
		}
	}

	return true;
}