
#include "StdAfx.h"
#include "OfficialRankConfig.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

vector<COffciailRankConfig::tagOfficialRankConfig>	COffciailRankConfig::m_vecOfficialRackConfig;	//保存国家官阶的配置信息	
COffciailRankConfig::COffciailRankConfig(void)
{
	m_vecOfficialRackConfig.clear();
}

COffciailRankConfig::~COffciailRankConfig(void)
{
	m_vecOfficialRackConfig.clear();
}

// 从文件读取
void COffciailRankConfig::LoadOfficialRankConfig()
{
	CRFile* prfile = rfOpen("data/officalRank.ini");
	if( prfile )
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);
		while (ReadTo(stream, "#"))
		{
			COffciailRankConfig::tagOfficialRankConfig tagOffRank;
			stream >> tagOffRank.CreditMinValue >> tagOffRank.CreditMaxValue >> tagOffRank.strFighterOfficialRank >> tagOffRank.strHunterOfficialRank >> tagOffRank.strDaoshiOfficialRank 
				>> tagOffRank.textColorA >> tagOffRank.textColorR >> tagOffRank.textColorG >> tagOffRank.textColorB 
				>> tagOffRank.textBkColorA >> tagOffRank.textBkColorR >> tagOffRank.textBkColorG >> tagOffRank.textBkColorB;;
			m_vecOfficialRackConfig.push_back(tagOffRank);
		}
	}
}

