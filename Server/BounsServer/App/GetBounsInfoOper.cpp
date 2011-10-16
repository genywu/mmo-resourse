#include "StdAfx.h"
#include "GetBounsInfoOper.h"

extern long g_lDealLoadMsgNum;				// 处理完的查询消息数量


CGetBounsInfoOper::CGetBounsInfoOper(eOperType eOt)
{
	SetOperType(eOt);
}

CGetBounsInfoOper::~CGetBounsInfoOper(void)
{
	list<tagBounsInfo*>::iterator itor = m_listBounsInfo.begin();
	for(; itor != m_listBounsInfo.end(); ++itor)
	{
 		SAFE_DELETE((*itor));
	}
}

BOOL CGetBounsInfoOper::DoOperator()
{
	m_listBounsInfo.clear();
    HRESULT hRt = GetGame()->GetRsBouns()->LoadBounsInfo(m_cnConnPtr, m_strAccount.c_str(), m_dwAreaServerID,GetGame()->GetSetup()->dwMaxBounsNum, m_listBounsInfo);
    if(hRt != S_OK)
        return FALSE;

	CheckBounsDes();

	CMessage msg(MSG_BDS2S_BOUNS_LOAD_RESPOND);
	msg.Add(m_guidPlayer);
	msg.Add((long)m_listBounsInfo.size());
	DBWriteSet db;
	msg.GetDBWriteSet(db);
	list<tagBounsInfo*>::iterator itor = m_listBounsInfo.begin();
	for(; itor != m_listBounsInfo.end(); ++itor)
	{
		(*itor)->EncodeToDataBlock(db);
	}
	msg.SendToSocket(m_lGSSocketID);
	g_lDealLoadMsgNum++;
	RefeashInfoText();	

	return TRUE;
}

// 检测奖励信息是否完整
void CGetBounsInfoOper::CheckBounsDes()
{
	list<tagBounsInfo*>::iterator itor = m_listBounsInfo.begin();
	for(; itor != m_listBounsInfo.end();)
	{
		if( GetGame()->GetBounsDes()->find((*itor)->dwBounsID) == GetGame()->GetBounsDes()->end())
		{
			// 如果该奖励的资源ID找不到，再到数据库中去加载一次
			tagBounsDes tgBD;
			GetGame()->GetRsBouns()->LoadBounsDesByID(m_cnConnPtr, (*itor)->dwBounsID, tgBD);
			if(tgBD.dwBounsID == (*itor)->dwBounsID)
			{
				GetGame()->AddBounsDes(tgBD);
				// 发送奖励描述资源
				CMessage msg(MSG_BDS2S_BOUNS_DES_UPDATE);
				msg.Add((BYTE)2); //单个更新
				DBWriteSet setWriteDB;
				msg.GetDBWriteSet(setWriteDB);				
				tgBD.EncodeToDataBlock(setWriteDB);		
			//	msg.SendToSocket(m_lGSSocketID);
				msg.SendAll();
			}
			SAFE_DELETE(*itor);
			itor = m_listBounsInfo.erase(itor);
		}
		else
		{
			itor++;
		}
	}
	
}