#include "stdafx.h"

#include "../../../Net/Message.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnFriendHuntMessage(CMessage * pMsg)
{
	//
	//vector<CFriendHuntPageEx::tagHuntedInfo> *pHuntedFriendVector = pFriendHuntPage->GetVecHuntedFriend();
	//pHuntedFriendVector->clear();
	//CGUID	guid;
	//pMsg->GetGUID(guid);
	//long lMsgSize = pMsg->GetLong();
	//if (lMsgSize == 0)
	//{
	//	CMessageBoxPageEx *pMsgBoxPage =  GetGame()->GetCGuiEx()->GetMessageBoxPageEx();
	//	pMsgBoxPage->Open(pFriendHuntPage,"没有找到符合条件的玩家",CMessageBoxPageEx::MSG_OK,true);
	//	return;
	//}
	//char strName[64] = "";
	//char strFactionName[64] = "";
	//char strUnionName[64] = "";
	//for(int i = 0; i < lMsgSize; i++)
	//{
	//	CFriendHuntPageEx::tagHuntedInfo tagInfo;
	//	pMsg->GetStr(strName,64);
	//	pMsg->GetStr(strFactionName,64);
	//	pMsg->GetStr(strUnionName,64);
	//	long lOccupation = pMsg->GetLong();
	//	long lSex		= pMsg->GetLong();
	//	long lLevel		= pMsg->GetLong();
	//	long lCountryID = pMsg->GetLong();
	//	long lRegionID  = pMsg->GetLong();
	//	long lConstle   = pMsg->GetLong();
	//	tagInfo.strPlayerName	= strName;
	//	tagInfo.strFactionName	= strFactionName;
	//	tagInfo.strUnion		= strUnionName;
	//	tagInfo.lOccupation     = lOccupation;
	//	tagInfo.lSex			= lSex;
	//	tagInfo.lLevel			= lLevel;
	//	tagInfo.lCountryID		= lCountryID;
	//	tagInfo.lRegionID		= lRegionID;
	//	tagInfo.lConstellation  = lConstle;
	//	pHuntedFriendVector->push_back(tagInfo);
	//}
	//pFriendHuntPage->ProcessHuntedFriendList();
}