#include "stdafx.h"
#include "RankSystem.h"
#include "../../ClientApp/Player.h"
#include "../../GameClient/Game.h"
#include "../../../Net/Message.h"
#include "../Other/MsgSendTimeLimitMgr.h"

/////////////////////////////////////////////////
// zhaohang  2010/3/23 
// 排行榜消息处理类函数实现
void RankMsgProc::Query(eRankType type,long RankID,const char* QueryPlayerName,const char* QueryContent,long PageOrRank /* = 0 */)
{
	if(!QueryContent)
		return;
	MsgSendTimeLimitMgr& mstlm = GetInst(MsgSendTimeLimitMgr);
	if( mstlm.IsTimeOut(MST_TYPE_RANK_TIME) == false)
		return;
	CMessage msg(MSG_C2S_QUEST_PLAYERRANKS);
	CPlayer* pPlayer = GetGame()->GetMainPlayer();
	if(!pPlayer)
		return;
	msg.Add(pPlayer->GetExID());
	msg.Add((char)type);
	msg.Add((long)RankID);
	if(type == RT_OWNER)
	{	
		msg.Add(QueryPlayerName);
		msg.Add(QueryContent);
	}
	else if( type == RT_OTHER)
	{
		msg.Add(PageOrRank);
	}
	else if( type == RT_RANK)
	{
		msg.Add(PageOrRank);
	}
	msg.Send();
	mstlm.UpdateLastSendTime(MST_TYPE_RANK_TIME);
}
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// zhaohang  2010/3/25 
// 排行榜查询配置
bool RankQueryConfig::ReadConfig()
{
	ifstream stream;

	stream.open("setup/UISetup/RankSet.ini");
	if( stream.is_open() )
	{	
		uint mainMenuNum;
		//读取主菜单数目
		stream >> mainMenuNum;
		
		for(uint i = 0 ; i < mainMenuNum ; ++i)
		{
			tagMenuQueryConfig mqc;
			//读取主菜单文本 和 ID
			stream >>  mqc.strMainMenu >> mqc.mainMenuID;
			//读取子菜单数目
			uint subItemNum;
			stream >> subItemNum;
			for(uint j= 0 ; j < subItemNum ; ++j)
			{
				tagSubItem si;
				//读取子菜单lable，ID 和查询内容
				stream >> si.strItem >> si.itemID >> si.strQueryContent;
				//读取子菜单的显示列表数目
				uint listNum;
				stream >> listNum;
				//读取列表配置
				tagRowInfo ri;
				for(uint k = 0 ; k < listNum ; ++k)
				{
					char temp[256];
					stream >> ri.strRowHeader >> temp;
					//将strRowDataHeader转化为小写
					ri.strRowDataHeader = strlwr(temp);
					si.rowInfo.push_back(ri);
				}
				mqc.vecSubItem.push_back(si);
			}
			vecMQC.push_back(mqc);
		}
		return true;
	}
	else
	{
		MessageBox(g_hWnd, "Can't find file setup/RankSet.ini", "message", MB_OK);
		return false;
	}
}