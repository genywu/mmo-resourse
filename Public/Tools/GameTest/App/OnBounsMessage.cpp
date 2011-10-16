


#include "stdafx.h"
#include "GameClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CGameClient::OnBounsMessage(CMessage* pMsg)
{
	static long lQueryTimes = 0;
	switch(pMsg->GetType())
	{
	case MSG_S2C_BOUNS_INFO_RESPOND:
		{
			++lQueryTimes;
			char szInfo[128];
			sprintf(szInfo,"Query:%d\n",lQueryTimes);
			OutputDebugString(szInfo);

			long lSize = pMsg->GetLong();

			DBReadSet dbRead;
			pMsg->GetDBReadSet(dbRead);

			tagBounsInfo * bounsInfo;
			tagBounsDes pDB;
			for(long i=0;i<lSize;++i)
			{
				bounsInfo = new tagBounsInfo;
				bounsInfo->DecodeFromDataBlock(dbRead);
				pDB.DecodeFromDataBlock(dbRead);

				m_pMainPlayer->AddBounsInfo(bounsInfo);
			}
			if( lSize == 0 )
			{
				sprintf(szInfo,"%s:Info Size:%d\n",m_pMainPlayer->GetName(),0);
				OutputDebugString(szInfo);
			}
		}
		break;
	case MSG_S2C_BOUNS_GET_RESULT:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			bool bSucced = pMsg->GetLong() != 1 ? false : true;

			m_pMainPlayer->OnGetBounsResult(guid,bSucced);
		}
	}
}