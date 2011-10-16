#include "StdAfx.h"
#include ".\creationplayerlist.h"



//CCreationPlayerList::CCreationPlayerList(void)
//{
//}
//
//CCreationPlayerList::~CCreationPlayerList(void)
//{
//}
CCreationPlayerList *CCreationPlayerList::instance = NULL;

CCreationPlayerList* CCreationPlayerList::GetInstance()
{
	if(!instance)
	{
		instance = new CCreationPlayerList();
	}
	return instance;
}

void CCreationPlayerList::Release()
{
	if(instance)
	{
		delete instance;
		instance = NULL;
	}
}

/*
* 追加一个Player到PlayerList
*/
void CCreationPlayerList::AppendPlayer(CPlayer *pPlayer, BYTE nLocation)
{
	// player==NULL OR location==0 THEN exit
	assert(pPlayer);
	for(list<tagCreationPlayer>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		// 如果列表中存在该玩家
		if( pPlayer->GetID() == (*it).pPlayer->GetID() ) return;
	}
	tagCreationPlayer CreationPlayer = {pPlayer, nLocation};
	GetPlayerList()->push_back(CreationPlayer);
}
void CCreationPlayerList::DeletePlayer(CPlayer *pPlayer)
{
	if (!pPlayer) return;

	for(list<tagCreationPlayer>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		if (pPlayer->GetID() == (*it).pPlayer->GetID())
		{
			CPlayer* pTemp = (*it).pPlayer;
			SAFE_DELETE(pTemp);
			GetPlayerList()->erase(it);
			DelPlayerList(pPlayer->GetName());
		}
	}
}
/*
 * 检测列表中该Name是否被使用了
 */
bool CCreationPlayerList::IsNameExist(LPCSTR szName)
{
	// Name 不能 NULL
	assert(szName);

	for(list<tagCreationPlayer>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		// 找到有使用过的名字
		if( !strcmpi((*it).pPlayer->GetName(), szName) ) 
		{
			return true;
		}
	}
	return false;
}

/*
// 0x 0 1 2 3 ... 7
//    1 2 3 reserve
// 1，2，3表示可否角色创建的位子，对正一个字节（8bit），1是已经使用，0没有使用
// res从低到高第0个1bit表示位子1，1表示位子2，2表示位子3，3-7保留
// 例如： 0x00       0x01        0x02         0x05        0x06
//     0000 0000  0000 0001   0000 0010    0000 0101   0000 0111
//     全空位     位子1不空   位子2不空    位子1，3空  全满
 */
BYTE CCreationPlayerList::GetLocationStateInCdkey(LPCSTR szCdkey)
{
	assert(szCdkey);
	
	// res | (1/2/4)  001 010 100
	BYTE res = 0x00;

	// 获取一个cdkey下所以的待创建角色
	for(list<tagCreationPlayer>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		if( !strcmpi((*it).pPlayer->GetAccount(), szCdkey) )
		{
			//if( (*it).nLocation >0 || (*it).nLocation<4 )
			switch((*it).nLocation) 
			{
			case 1:
				res |= 1;
				break;
			case 2:
				res |= 2;
				break;
			case 3:
				res |= 4;
				break;
			default:
				res &= 0x07; // 0000 0111
			}
		}
	}
	return res;
}

/*
 * 获取同一帐户下角色的数量
 */
BYTE CCreationPlayerList::GetPlayerCountInCdkey(LPCSTR szCdkey)
{
	// 一个帐户里的玩家个数是0到3
	assert(szCdkey);

	UINT8 res = 0;
	for(list<tagCreationPlayer>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		if( !stricmp((*it).pPlayer->GetAccount(), szCdkey) )
		{
			res++;
		}
	}
	return res;
}

/*
 * 返回一个帐户下所以角色数组（size:0-3)
 */
vector<CCreationPlayerList::tagCreationPlayer> CCreationPlayerList::GetPlayerVectorByCdkey(LPCSTR szCdkey)
{
	vector<tagCreationPlayer> vCreationPlayer;
	for(list<tagCreationPlayer>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		if( !stricmp((*it).pPlayer->GetAccount(), szCdkey) )
		{
			vCreationPlayer.push_back(*it);
		}
	}
	return vCreationPlayer;
}

/*
* 通过PlayerID在PlayerList返回一个Player
*/
CPlayer* CCreationPlayerList::GetPlayerByID(UINT nID)
{
	CPlayer* pPlayer = NULL;
	for(list<tagCreationPlayer>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		if( (*it).pPlayer->GetID() == nID )
		{
			pPlayer = (*it).pPlayer;
			break;
		}
	}
	return pPlayer;
}

/*
* 通过PlayerID复制一个Player
*/
CPlayer* CCreationPlayerList::ClonePlayer(UINT nID)
{
	CPlayer* pClonePlayer = NULL;
	for(list<tagCreationPlayer>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		if( (*it).pPlayer->GetID() == nID )
		{
			pClonePlayer = new CPlayer;
			long lPos=0;
			CPlayer* pPlayer = (*it).pPlayer;
			vector<BYTE> vBuf;
			pPlayer->AddToByteArray(&vBuf);
			pClonePlayer->DecordFromByteArray(&vBuf[0],lPos);
			break;
		}
	}
    return pClonePlayer;
}