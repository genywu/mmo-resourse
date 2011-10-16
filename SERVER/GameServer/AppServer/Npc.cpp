
#include "StdAfx.h"
/*
#include ".\npc.h"
#include "ServerRegion.h"
#include "Player.h"
#include "Goods\CGoods.h"
#include "..\..\nets\netserver\message.h"
#include "..\setup\playerlist.h"
#include "other\chat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNpc::CNpc(void)
{
	SetType(TYPE_NPC);
	m_bShowList = true;
}

CNpc::~CNpc(void)
{
}

bool CNpc::AddToByteArray(vector<BYTE>* pByteArray, bool bExData )
{// 添加到CByteArray
	return CShape::AddToByteArray(pByteArray, bExData);
	
}

bool CNpc::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{	// 解码
	return true;
}

void CNpc::Talk( const char* strContent )
{
	BEGIN_LOOP_AROUND_SHAPE((CServerRegion*)GetFather(),GetArea()->GetX(),GetArea()->GetY())
	{
		if( pShape->GetType() == TYPE_PLAYER )
		{
			CPlayer* p = (CPlayer*)pShape;

			// 同屏玩家
			if( abs( (long)(p->GetTileX() - GetTileX()) ) < AREA_WIDTH && abs( (long)(p->GetTileY() - GetTileY()) ) < AREA_HEIGHT )
			{
				CMessage msg(MSG_S2C_OTHER_TALK);
				msg.Add(long(CHAT_NORMAL));
				msg.Add(static_cast<long>(TYPE_NPC));
				msg.Add(GetExID());
				msg.Add(const_cast<char*>(GetName()));
				msg.Add(const_cast<char*>(strContent) );
				msg.SendToPlayer( p->GetExID());
			}
		}
	}
	END_LOOP_AROUND_SHAPE
}
*/