// 增值商店，只允许购买增值道具，不允许卖，不允许修理，而且无税收。
// bugs
#include "StdAfx.h"
#include "ContributeSetup.h"

extern HWND g_hWnd;

long	CContributeSetup::lCombatLevel1;
long	CContributeSetup::lCombatLevel2;

long	CContributeSetup::lContribute1;
long	CContributeSetup::lContribute2;

long	CContributeSetup::lContributeLevel;
long	CContributeSetup::lContributeMultiple;

long	CContributeSetup::lContributeBaseValue;
long	CContributeSetup::lContributeMax;

long	CContributeSetup::lCountryCityContributeModifierLose;
long	CContributeSetup::lCountryCityContributeModifierGain;
long	CContributeSetup::lCountryCityContributeItemModifier;

vector<CContributeSetup::tagContributeItem> CContributeSetup::m_vContributeItems;		// 所有的NPC交易列表 map<NPC原始名，交易列表>

CContributeSetup::CContributeSetup(void)
{
}

CContributeSetup::~CContributeSetup(void)
{
	m_vContributeItems.clear();
}

// 从文件读取列表
BOOL CContributeSetup::LoadContributeSetup(const char* filename)
{
	m_vContributeItems.clear();

	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		char str[256];
		_snprintf(str, 256, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return FALSE;
	}
	
	stringstream stream;
	string strTemp;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	stream	
		>> strTemp >> lCombatLevel1
		>> strTemp >> lCombatLevel2
		>> strTemp >> lContribute1
		>> strTemp >> lContribute2

		>> strTemp >> lContributeLevel
		>> strTemp >> lContributeMultiple

		>> strTemp >> lContributeBaseValue
		>> strTemp >> lContributeMax

		>> strTemp >> lCountryCityContributeModifierLose
		>> strTemp >> lCountryCityContributeModifierGain
		>> strTemp >> lCountryCityContributeItemModifier;


	// read incrementshop list
	while(ReadTo(stream, "#"))
	{
		tagContributeItem stItem;

		stream>>stItem.dwLoValue;

		stream>>stItem.dwHiValue;

		stream>>stItem.strName;
		
		stream>>stItem.dwNum;

		m_vContributeItems.push_back(stItem);
	}
	return true;
}

// 添加到CByteArray
bool CContributeSetup::AddToByteArray(vector<BYTE>* pByteArray)
{

	_AddToByteArray(pByteArray,lCombatLevel1);
	_AddToByteArray(pByteArray,lCombatLevel2);

	_AddToByteArray(pByteArray,lContribute1);
	_AddToByteArray(pByteArray,lContribute2);

	_AddToByteArray(pByteArray,lContributeLevel);
	_AddToByteArray(pByteArray,lContributeMultiple);

	_AddToByteArray(pByteArray,lContributeBaseValue);
	_AddToByteArray(pByteArray,lContributeMax);

	_AddToByteArray(pByteArray,lCountryCityContributeModifierLose);
	_AddToByteArray(pByteArray,lCountryCityContributeModifierGain);
	_AddToByteArray(pByteArray,lCountryCityContributeItemModifier);


	long lNum = (long)m_vContributeItems.size();
	_AddToByteArray(pByteArray, lNum);

	for(vector<tagContributeItem>::iterator it=m_vContributeItems.begin();
		it!=m_vContributeItems.end(); ++it)
	{
		tagContributeItem *pItem = &(*it);
		if( pItem )
		{
			_AddToByteArray(pByteArray,pItem->dwLoValue);
			_AddToByteArray(pByteArray,pItem->dwHiValue);
			_AddToByteArray(pByteArray,pItem->dwNum);
			_AddToByteArray(pByteArray,(char*)pItem->strName.data());
		}
	}
	return true;
}

// 解码
bool CContributeSetup::DecordFromByteArray(BYTE* pByte, long& pos)
{
	char szTemp[256];
	int i;
	long lNum;
	tagContributeItem stItem;
	
	m_vContributeItems.clear();

	lCombatLevel1 = _GetLongFromByteArray(pByte, pos);
	lCombatLevel2 = _GetLongFromByteArray(pByte, pos);

	lContribute1 = _GetLongFromByteArray(pByte, pos);
	lContribute2 = _GetLongFromByteArray(pByte, pos);

	lContributeLevel = _GetLongFromByteArray(pByte, pos);
	lContributeMultiple = _GetLongFromByteArray(pByte, pos);

	lContributeBaseValue = _GetLongFromByteArray(pByte, pos);
	lContributeMax = _GetLongFromByteArray(pByte, pos);

	lCountryCityContributeModifierLose = _GetLongFromByteArray(pByte, pos);
	lCountryCityContributeModifierGain = _GetLongFromByteArray(pByte, pos);
	lCountryCityContributeItemModifier = _GetLongFromByteArray(pByte, pos);
	

	lNum = _GetLongFromByteArray(pByte, pos);
	for(i=0; i<lNum; i++)
	{
		stItem.dwLoValue = _GetLongFromByteArray(pByte, pos);
		stItem.dwHiValue = _GetLongFromByteArray(pByte, pos);
		stItem.dwNum = _GetLongFromByteArray(pByte, pos);
		stItem.strName = _GetStringFromByteArray(pByte, pos, szTemp);

		m_vContributeItems.push_back(stItem);
	}

	return true;
}
