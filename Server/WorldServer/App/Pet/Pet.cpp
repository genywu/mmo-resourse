#include "StdAfx.h"
#include "Pet.h"
#include "../Player.h"
#include "../Monster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPet::CPet(void)
{
	SetType(TYPE_PET);
}

CPet::~CPet(void)
{
}

// WS==>GS编码
bool CPet::CodeToDataBlock(DBWriteSet& setWriteDB,
							 bool bExData)
{
	CMoveShape::CodeToDataBlock(setWriteDB, bExData);

	setWriteDB.AddToByteArray(&m_DiffAttrs, sizeof(m_DiffAttrs));
	setWriteDB.AddToByteArray(m_HostID);		// 主人
	long lNum = (long)m_Skills.size();
	setWriteDB.AddToByteArray(lNum);
	ITR_SKILL_LIST itrList = m_Skills.begin();
	for( ; itrList != m_Skills.end(); ++itrList )
	{
		setWriteDB.AddToByteArray(&itrList->second, sizeof(tagSkill));
	}

	return true;
}

// GS==>WS解码
bool CPet::DecodeFromDataBlock(DBReadSet& setReadDB,
							   bool bExData)
{
	CMoveShape::DecodeFromDataBlock(setReadDB, bExData);

	setReadDB.GetBufferFromByteArray(&m_DiffAttrs, sizeof(m_DiffAttrs));
	setReadDB.GetBufferFromByteArray(m_HostID);		// 主人
	long lSkillNum = setReadDB.GetLongFromByteArray();
	tagSkill tSkill;
	for( long l=0; l<lSkillNum; ++l )
	{
		setReadDB.GetBufferFromByteArray(&tSkill, sizeof(tSkill));
		m_Skills[tSkill.lPos] = tSkill;
	}
	
	SetName(m_DiffAttrs.szName);

	return true;
}