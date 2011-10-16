#include "StdAfx.h"
#include ".\playerlist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HWND g_hWnd;

//string CPlayerList::s_strOccupation[CPlayerList::OC_NUMS] = {"勇士", "猎手", "道人"};
string CPlayerList::s_strOccupation[OCC_Max] = 
{"初心者", "战士", "法师","行者","骑士","斗士","斥候","艺人","术士","牧师","圣骑士","暗骑士",
"角斗士","狂战士","巡守","刺客","剑舞者","吟游诗人","巫师","亡灵巫师","主教","贤者"};
string CPlayerList::s_strSex[CPlayerList::SEX_NUMS] = {"男", "女"};


vector<CPlayerList::tagLevelUp>					CPlayerList::m_vectorPlayerExp;		// 玩家升级经验值
vector<CPlayerList::tagOccuLevelUp>				CPlayerList::m_vcOccuLvlExp[OCC_Max];//每个职业等级经验列表
DWORD											CPlayerList::m_dwLevelPoint = 0;	// 每升一级给玩家分配的点数
list<CPlayerList::tagOrginEquip>				CPlayerList::m_listOrginEquip;		// 初始装备列表

long CPlayerList::m_lBasePropertyValue;//基本属性值
float CPlayerList::m_fPropertyLvlParam;//属性等级系数
map<DWORD, CPlayerList::tagOccupModifyProperty>	CPlayerList::m_mapOccuModifyPropertyList;   //职业修正表
map<DWORD,CPlayerList::tagOccuDefParam>	CPlayerList::m_mapOccuDefParam;//各职业的防御修正值
map<DWORD,CPlayerList::tagOccuAtkParam>	CPlayerList::m_mapOccuAtkParam;//各职业的攻击修正值

CPlayerList::CPlayerList(void)
{
}

CPlayerList::~CPlayerList(void)
{
	m_dwLevelPoint = 0;
	//	m_mapPlayerList.clear();
	m_vectorPlayerExp.clear();
	m_listOrginEquip.clear();
	m_mapOccuModifyPropertyList.clear();
}


// 从文件读取exp列表
bool CPlayerList::LoadPlayerExpList(const char* filename)
{
	m_vectorPlayerExp.clear();

	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		char str[256];
		_snprintf(str, 256, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return FALSE;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	string strTemp;
	ReadTo(stream, "*");
	stream >> strTemp >> m_dwLevelPoint;

	tagLevelUp st;
	while( ReadTo(stream, "#") )
	{
		stream >> st.dwExp >> st.dwExp >> st.dwExpMultiple >> st.lPresentExpPerMinute >> st.lMaxPresentExp >> st.fFacBoonExpScale >> st.lBaseCP;
		m_vectorPlayerExp.push_back(st);
	}

	return true;
}


//装载职业等级经验列表
bool CPlayerList::LoadOccuLvlExpList()
{
	char pszFileName[256]="";
	for(int i=0;i<OCC_Max;i++)
	{
		m_vcOccuLvlExp[i].clear();
		_snprintf(pszFileName,256,"data/LevelExp/Occu%02dExp.ini",i);
		CRFile* prfile = rfOpen(pszFileName);
		if(prfile == NULL)
		{
			char str[256];
			_snprintf(str, 256, "file '%s' can't found!", pszFileName);
			MessageBox(g_hWnd, str, "error", MB_OK);
			return false;
		}
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);
		tagOccuLevelUp stOccuLvl;
		long lLvl=0;
		while( ReadTo(stream, "#") )
		{
			stream >> lLvl >> stOccuLvl.dwExp;
			m_vcOccuLvlExp[i].push_back(stOccuLvl);
			if(m_vcOccuLvlExp[i].size() != lLvl)
			{
				char str[256];
				_snprintf(str, 256, "file '%s' config error!", pszFileName);
				MessageBox(g_hWnd, str, "error", MB_OK);
				return false;
			}
		}
	}
	return true;
}

// 保存到文件exp列表
bool CPlayerList::SavePlayerExpList(const char* filename)
{
	return true;
}

bool CPlayerList::AddToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray,(long)m_dwLevelPoint);     //发送每一等级的技能点
	
	long lLevelNum =(long)m_vectorPlayerExp.size();       /////发送玩家经验表
	_AddToByteArray(pByteArray, lLevelNum);
	for(int i=0; i<lLevelNum; i++)
	{
		_AddToByteArray(pByteArray, &m_vectorPlayerExp[i], (long)sizeof(tagLevelUp));
	}

	for(int i=0;i<OCC_Max;i++)
	{
		long lLevelNum =(long)m_vcOccuLvlExp[i].size();       /////发送职业经验表
		_AddToByteArray(pByteArray, lLevelNum);
		for(int j=0; j<lLevelNum; j++)
		{
			_AddToByteArray(pByteArray, &m_vcOccuLvlExp[i][j], (long)sizeof(tagOccuLevelUp));
		}
	}

	_AddToByteArray(pByteArray,m_lBasePropertyValue);
	_AddToByteArray(pByteArray,m_fPropertyLvlParam);
	_AddToByteArray(pByteArray, (long)m_mapOccuModifyPropertyList.size() );//发送等级修正
	map<DWORD, CPlayerList::tagOccupModifyProperty>::iterator itUpgrade = m_mapOccuModifyPropertyList.begin();
	for( ; itUpgrade != m_mapOccuModifyPropertyList.end(); itUpgrade ++ )
	{
		tagOccupModifyProperty &tPoint = (*itUpgrade).second;
		_AddToByteArray(pByteArray, &tPoint,(long)sizeof(tagOccupModifyProperty));
	}

	map<DWORD,tagOccuDefParam>::iterator it = m_mapOccuDefParam.begin();
	_AddToByteArray(pByteArray,(long)m_mapOccuDefParam.size());
	for(;it != m_mapOccuDefParam.end();it++)
	{
		tagOccuDefParam &OccuDefParam = (*it).second;
		_AddToByteArray(pByteArray,(*it).first);
		_AddToByteArray(pByteArray, &OccuDefParam, (long)sizeof(OccuDefParam));
	}

	map<DWORD,tagOccuAtkParam>::iterator itAtk = m_mapOccuAtkParam.begin();
	_AddToByteArray(pByteArray,(long)m_mapOccuAtkParam.size());
	for(;itAtk != m_mapOccuAtkParam.end();itAtk++)
	{
		tagOccuAtkParam &OccuAtkParam = (*itAtk).second;
		_AddToByteArray(pByteArray,(*itAtk).first);
		_AddToByteArray(pByteArray, &OccuAtkParam, (long)sizeof(OccuAtkParam));
	}
	
	return true;
}

bool CPlayerList::DecordFromByteArray(BYTE* pByte, long& pos)
{
	m_vectorPlayerExp.clear();
	m_mapOccuModifyPropertyList.clear();
	m_mapOccuDefParam.clear();
	m_mapOccuAtkParam.clear();

	m_dwLevelPoint = _GetLongFromByteArray(pByte, pos);     //每一等级增加的点数
	
	long lLevelNum = _GetLongFromByteArray(pByte, pos);      //玩家经验表
	for(int i=0; i<lLevelNum; i++)
	{
		tagLevelUp st;
		_GetBufferFromByteArray(pByte, pos, &st, sizeof(tagLevelUp));
		m_vectorPlayerExp.push_back(st);
	}

	for(int i=0;i<OCC_Max;i++)
	{
		m_vcOccuLvlExp[i].clear();
		long lLevelNum = _GetLongFromByteArray(pByte, pos);      //玩家经验表
		tagOccuLevelUp st;
		for(int j=0; j<lLevelNum; j++)
		{
			_GetBufferFromByteArray(pByte, pos, &st, sizeof(tagOccuLevelUp));
			m_vcOccuLvlExp[i].push_back(st);
		}
	}

	m_lBasePropertyValue = _GetLongFromByteArray(pByte, pos);
	m_fPropertyLvlParam = _GetFloatFromByteArray(pByte, pos);
	tagOccupModifyProperty tPoint;                            //获取等级修正表
	lLevelNum = _GetLongFromByteArray(pByte, pos);     
	for(int i=0; i<lLevelNum; i++)
	{
		_GetBufferFromByteArray(pByte, pos, &tPoint, sizeof(tagOccupModifyProperty));
		m_mapOccuModifyPropertyList[i] = tPoint;
	}

	DWORD dwOccuLevel = 0;
	tagOccuDefParam OccuDefParam;
	long lSize = _GetLongFromByteArray(pByte, pos);
	for(int i=0;i<lSize;i++)
	{
		 dwOccuLevel = _GetDwordFromByteArray(pByte, pos);
		 _GetBufferFromByteArray(pByte, pos, &OccuDefParam, sizeof(OccuDefParam));
		 m_mapOccuDefParam[dwOccuLevel] = OccuDefParam;
	}

	tagOccuAtkParam OccuAtkParam;
	lSize = _GetLongFromByteArray(pByte, pos);
	for(int i=0;i<lSize;i++)
	{
		 dwOccuLevel = _GetDwordFromByteArray(pByte, pos);
		 _GetBufferFromByteArray(pByte, pos, &OccuAtkParam, sizeof(OccuAtkParam));
		 m_mapOccuAtkParam[dwOccuLevel] = OccuAtkParam;
	}
	return true;
}


CPlayerList::tagOccupModifyProperty*	CPlayerList::GetOccuModifyProperty(WORD level)         //
{
	return &m_mapOccuModifyPropertyList[level];
}


BOOL CPlayerList::LoadOccuModifyProperty(const char* filename)            //玩家职业修正属性表
{
	m_mapOccuModifyPropertyList.clear ();
	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		char str[256];
		_snprintf(str, 256, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return FALSE;
	}
	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	short nOccup;
	tagOccupModifyProperty tProperty;
	string strTempt;
	
	stream>>strTempt>>m_lBasePropertyValue;
	stream>>strTempt>>m_fPropertyLvlParam;
	while( ReadTo(stream,"*"))
	{
		stream>> nOccup 
		>>strTempt
		>> tProperty.fStrenth
		>> tProperty.fAgility
		>> tProperty.fConsti
		>> tProperty.fIntell
		>> tProperty.fWisdom
		>> tProperty.fSpirit

		>> tProperty.fStrAttRatio
		>> tProperty.fAgiAttRatio
		>> tProperty.fConDefRatio

		>> tProperty.fBlockStrRatio
		>> tProperty.fBlockValueRatio

		>> tProperty.fParryStrRatio
		>> tProperty.fParryAgiRatio
		>> tProperty.fParryValueRatio

		>> tProperty.fDodgeAgiRatio
		>> tProperty.fDodgeValueRatio

		>> tProperty.fCriAgiRatio
		>> tProperty.fCriValueRatio

		>> tProperty.fHitAgiRatio
		>> tProperty.fHitValueRatio

		>> tProperty.fBaseHit
		>> tProperty.fBaseHp
		>> tProperty.fLvlUpHp
		>> tProperty.fBaseMp
		>> tProperty.fLvlUpMp

		>> tProperty.fCriDamageStrRatio
		>> tProperty.fCriDamageValueRatio
		>> tProperty.fCriDefConstiRatio
		>> tProperty.fCriDefValueRatio
		>> tProperty.fConstiDefRatio
		>> tProperty.fConstiDefValueRatio
		>> tProperty.fConstiHpRatio
		>> tProperty.fMAtkIntellRatio
		>> tProperty.fMCriDamageIntellRatio
		>> tProperty.fMCriDamageValueRatio
		>> tProperty.fWillDefSpiritRatio
		>> tProperty.fWillMpRatio
		>> tProperty.fWillDefValueRatio
		>> tProperty.fMCriDefSpiritRatio
		>> tProperty.fMCriDefValueRatio
		>> tProperty.fMCriWisdomRatio
		>> tProperty.fMCriValueRatio
		>> tProperty.fMDefSpiritRatio
		>> tProperty.fMDefValueRatio
		>> tProperty.fAbsorbWisdomRatio
		>> tProperty.fAbsorbValueRatio
		>> tProperty.fBuffPowerIntellRatio
		>> tProperty.fBuffPowerValueRatio
		>> tProperty.fMAtkSpeedWisdomRatio
		>> tProperty.fMAtkSpeedValueRatio
		>> tProperty.fMaxBaseEnergy
		>> tProperty.fBaseEnergyRecoverValue
		>> tProperty.fBaseEnergyRecoverTime
		>> tProperty.fFightEnergyRecoverValue
		>> tProperty.fFightEnergyRecoverTime
		>> tProperty.fAttackEnergyValue
		>> tProperty.fBlastEnertyValue
		>> tProperty.fBeenAttackEnergyValue
		>> tProperty.fBeenBlastEnergyValue
		>> tProperty.lBasePKCP
		>> tProperty.lLvlupAddPKCP ;

		m_mapOccuModifyPropertyList[nOccup]=tProperty;
	}
	return true;
}

// 从文件读取职业防御修正
bool CPlayerList::LoadOccuDefParam(const char* filename)
{
	m_mapOccuDefParam.clear ();
	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		char str[256];
		_snprintf(str, 256, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return false;
	}
	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	short nOccuLevel=0;
	tagOccuDefParam OccuDefParam;
	while( ReadTo(stream,"*"))
	{
		stream >> nOccuLevel;
		for(int i=0;i<OCC_Max;i++)
		{
			stream >> OccuDefParam.Def[i];
			stream >> OccuDefParam.MDef[i];
		}
		m_mapOccuDefParam[nOccuLevel] = OccuDefParam;
	}
	return true;
}

// 从文件读取职业攻击修正
bool CPlayerList::LoadOccuAtkParam(const char* filename)
{
	m_mapOccuAtkParam.clear ();
	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		char str[256];
		_snprintf(str, 256, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return false;
	}
	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	short nOccuLevel=0;
	tagOccuAtkParam OccuAtkParam;
	while( ReadTo(stream,"*"))
	{
		stream >> nOccuLevel;
		for(int i=0;i<OCC_Max;i++)
		{
			stream >> OccuAtkParam.Atk[i];
			stream >> OccuAtkParam.MAtk[i];
		}
		m_mapOccuAtkParam[nOccuLevel] = OccuAtkParam;
	}
	return true;
}

BOOL CPlayerList::SaveOccuModifyProperty(const char* filename)
{
	return false;
}

float CPlayerList::GetOccuLvlCoe(eOccupation byOccu,WORD wOccuLevel)
{
	float fRet = 0;
	switch(byOccu)
	{
		//初心者
		case OCC_Novice:
			fRet = wOccuLevel;
			break;
		//战士、法师、行者
		case OCC_Fighter:
		case OCC_Mystic:
		case OCC_Rogue:
			fRet = wOccuLevel+9;
			break;
		//骑士、斗士、斥侯、艺人、术士、牧师
		case OCC_Knight:
		case OCC_Warrior:
		case OCC_Scout:
		case OCC_Craftsman:
		case OCC_Warlock:
		case OCC_Cleric:
			fRet = wOccuLevel+39;
			break;
		//圣骑士、暗骑士、剑士、狂战士、巡守、刺客、剑舞者、吟游诗人、巫师、死灵法师、主教、贤者
		case OCC_Paladin:
		case OCC_DarkKnight:
		case OCC_Gladiator:
		case OCC_Berserker:
		case OCC_Ranger:
		case OCC_Assassin:
		case OCC_BladeDancer:
		case OCC_Bard:
		case OCC_Sorcerer:
		case OCC_Necromancer:
		case OCC_Bishop:
		case OCC_Prophet:
			fRet = wOccuLevel+79;
			break;
	};
	return fRet;
}

void CPlayerList::GetDefParam(eOccupation byOccu,WORD wLeve,float& fDef,float& fMDef)
{
	if(byOccu < 0 || byOccu >= OCC_Max)	return;

	map<DWORD,tagOccuDefParam>::iterator it = m_mapOccuDefParam.find(wLeve);
	if(it != m_mapOccuDefParam.end())
	{
		fDef = (*it).second.Def[byOccu];
		fMDef = (*it).second.MDef[byOccu];
	}
	else
	{
		/*char str[500]="";
		_snprintf(str,500,"Don't find the defence modified property of level(%d) Occu(%d)!",wLeve,byOccu);
		PutDebugString(str);*/
	}
}

void CPlayerList::GetAtkParam(eOccupation byOccu,WORD wLeve,float& fAtk,float& fMAtk)
{
	if(byOccu < 0 || byOccu >= OCC_Max)	return;

	map<DWORD,tagOccuAtkParam>::iterator it = m_mapOccuAtkParam.find(wLeve);
	if(it != m_mapOccuAtkParam.end())
	{
		fAtk = (*it).second.Atk[byOccu];
		fMAtk = (*it).second.MAtk[byOccu];
	}
	else
	{
		/*char str[500]="";
		_snprintf(str,500,"Don't find the attck modified property of level(%d) Occu(%d)!",wLeve,byOccu);
		PutDebugString(str);*/
	}
}
void CPlayerList::SetBaseFightProperty(tagPPro *pPPro,tagPFPro *pPFPro)
{
	if(NULL != pPPro && NULL!=pPFPro)
	{
		//设置升级需要的经验值
		if(pPPro->wLevel <= 0 || pPPro->wLevel > m_vectorPlayerExp.size())
		{
			if(pPPro->wLevel <= 0)
				pPPro->wLevel = 1;
			else if(pPPro->wLevel > m_vectorPlayerExp.size() )
				pPPro->wLevel = m_vectorPlayerExp.size();
			PutDebugString("Set Player Property,Level Error.");
		}

		WORD lLvl = pPPro->wLevel;
		if(lLvl>m_vectorPlayerExp.size())
		{
			pPPro->dwUpgradeExp = 0xFFFFFFFF;
			pPPro->dwUpgradeExpMultiple = 0xFFFFFFFF;
		}
		else
		{
			pPPro->dwUpgradeExp = m_vectorPlayerExp[lLvl-1].dwExp;
			pPPro->dwUpgradeExpMultiple = m_vectorPlayerExp[lLvl-1].dwExpMultiple;
		}

		//设置职业升级所需要的经验
		//当前职业
		eOccupation	byCurOccu = pPPro->byOccu;
		if(pPPro->byOccuLvl[byCurOccu] <= 0 || pPPro->byOccuLvl[byCurOccu] > m_vcOccuLvlExp[byCurOccu].size())
		{
			if(pPPro->byOccuLvl[byCurOccu] <= 0)
				pPPro->byOccuLvl[byCurOccu] = 1;
			else if(pPPro->byOccuLvl[byCurOccu] > m_vcOccuLvlExp[byCurOccu].size() )
				pPPro->byOccuLvl[byCurOccu] = m_vcOccuLvlExp[byCurOccu].size();
			PutDebugString("Set Player Property,Occu Level Error.");
		}
		BYTE byLevel = pPPro->byOccuLvl[byCurOccu];
		pPPro->dwCurOccuUpgradeExp = m_vcOccuLvlExp[byCurOccu][byLevel-1].dwExp;

		//设置战斗参数
		map<DWORD, tagOccupModifyProperty>::iterator it =
			m_mapOccuModifyPropertyList.find(pPPro->byOccu);
		if(it != m_mapOccuModifyPropertyList.end())
		{
			tagOccupModifyProperty &OccuProperty = (*it).second;
			//得到职业等级系数
			float fOccuLvlFactor = GetOccuLvlCoe(pPPro->byOccu,pPPro->byOccuLvl[pPPro->byOccu]);

			memset(pPFPro,0,sizeof(tagPFPro));
			//HP回复比例初始值定为100
			pPFPro->wHpRecoverRatio = 100;
			/*人物基本属性（力量、敏捷、体质、智力、智慧、精神）
				==系数A+职业等级系数*POWER(系数2，2)*基本配置表中该职业相关属性的值
			*/
			//float fPowValue = pow((float)m_fPropertyLvlParam,2);
			//力量
			pPFPro->wStrenth = fOccuLvlFactor*OccuProperty.fStrenth+0.5f;
			//敏捷
			pPFPro->wAgility = fOccuLvlFactor*OccuProperty.fAgility+0.5f;
			//体质
			pPFPro->wConsti = fOccuLvlFactor*OccuProperty.fConsti+0.5f;
			//智利
			pPFPro->wIntell = fOccuLvlFactor*OccuProperty.fIntell+0.5f;
			//智慧
			pPFPro->wWisdom = fOccuLvlFactor*OccuProperty.fWisdom+0.5f;
			//精神
			pPFPro->wSpirit = fOccuLvlFactor*OccuProperty.fSpirit+0.5f;
		}
		else
		{
			char str[500]="";
			_snprintf(str, 500, "未找到%d职业的配置属性!",pPPro->byOccu);
			PutDebugString(str);
		}
	}
}


void CPlayerList::SetFightProperty(tagPPro *pPPro,tagPFPro *pBasePFPro,tagPFPro *pExPFPro)
{
	if(NULL != pPPro && NULL != pBasePFPro && NULL!=pExPFPro)
	{
		//得到职业等级系数
		float fOccuLvlFactor = GetOccuLvlCoe(pPPro->byOccu,pPPro->byOccuLvl[pPPro->byOccu]);
		//得到物理防御和魔法防御修正值
		float fDefParam = 0.0f;
		float fMDefParam = 0.0f;
		GetDefParam(pPPro->byOccu,fOccuLvlFactor,fDefParam,fMDefParam);
		float fAtkParam = 0.0f;
		float fMAtkParam = 0.0f;
		GetAtkParam(pPPro->byOccu,fOccuLvlFactor,fAtkParam,fMAtkParam);
		//设置战斗参数
		map<DWORD, tagOccupModifyProperty>::iterator it =
			m_mapOccuModifyPropertyList.find(pPPro->byOccu);
		if(it != m_mapOccuModifyPropertyList.end())
		{
			tagOccupModifyProperty &OccuProperty = (*it).second;			
			//总力量
			long lTotalStrenth = pBasePFPro->wStrenth+pExPFPro->wStrenth;
			if(lTotalStrenth < 0)	lTotalStrenth = 0;
			//总敏捷
			long lTotalAgility = pBasePFPro->wAgility+pExPFPro->wAgility;
			if(lTotalAgility < 0)	lTotalAgility = 0;
			//总体质
			long lTotalConsti = pBasePFPro->wConsti+pExPFPro->wConsti;
			if(lTotalConsti<0) lTotalConsti=0;
			//总智利
			long lTotalIntell = pBasePFPro->wIntell+pExPFPro->wIntell;
			if(lTotalIntell < 0)	lTotalIntell = 0;
			//总智慧
			long lTotalWisdom = pBasePFPro->wWisdom+pExPFPro->wWisdom;
			if(lTotalWisdom < 0)	lTotalWisdom = 0;
			//总精神
			long lTotalSpiri = pBasePFPro->wSpirit+pExPFPro->wSpirit;
			if(lTotalSpiri < 0) lTotalSpiri = 0;
			//格挡
			pBasePFPro->wFightBlock = (double)(lTotalStrenth)/OccuProperty.fBlockStrRatio;
			//招架
			pBasePFPro->wParry = (double)(lTotalStrenth)/OccuProperty.fParryStrRatio +
							(double)(lTotalAgility)/OccuProperty.fParryAgiRatio;
			//闪避
			pBasePFPro->wDodge = (double)(lTotalAgility)/OccuProperty.fDodgeAgiRatio;
			//基本物理攻击
			pBasePFPro->wMinAtk = pBasePFPro->wMaxAtk = (double)(lTotalStrenth)*OccuProperty.fStrAttRatio+
												(double)(lTotalAgility)*OccuProperty.fAgiAttRatio + fAtkParam;
			//物理防御
			pBasePFPro->wDef=(double)(lTotalConsti)*OccuProperty.fConDefRatio+fDefParam;
			//物理爆击值
			pBasePFPro->wCri = (double)(lTotalAgility)/OccuProperty.fCriAgiRatio;
			//命中值
			pBasePFPro->wHit = (double)(lTotalAgility)/OccuProperty.fHitAgiRatio;
			//专注值
			pBasePFPro->wAbsorb = (double)(lTotalWisdom)/OccuProperty.fAbsorbWisdomRatio;
			//物爆伤害值
			pBasePFPro->wCriDamage = (double)(lTotalStrenth)/OccuProperty.fCriDamageStrRatio;
			//物爆抗性值
			pBasePFPro->wCriDef = (double)(lTotalConsti)/OccuProperty.fCriDefConstiRatio;
			//体质抗性值
			pBasePFPro->wConstiDef = (double)(lTotalConsti)/OccuProperty.fConstiDefRatio;
			//魔法攻击
			pBasePFPro->wMAtk = (double)(lTotalIntell)*OccuProperty.fMAtkIntellRatio+fMAtkParam;
			//魔法爆击伤害
			pBasePFPro->wMCriDamage = (double)(lTotalIntell)/OccuProperty.fMCriDamageIntellRatio;
			//意志抗性值
			pBasePFPro->wWillDef = (double)(lTotalSpiri)/OccuProperty.fWillDefSpiritRatio;
			//魔法爆击抗性值
			pBasePFPro->wMCriDef = (double)(lTotalSpiri)/OccuProperty.fMCriDefSpiritRatio;
			//魔法抗性值
			pBasePFPro->wMdef = (double)(lTotalSpiri)*OccuProperty.fMDefSpiritRatio+fMDefParam;
			//魔法爆击值
			pBasePFPro->wMCri = (double)(lTotalWisdom)/OccuProperty.fMCriWisdomRatio;
			//状态攻击强度
			pBasePFPro->wBuffPower = (double)(lTotalIntell)/OccuProperty.fBuffPowerIntellRatio;
			//施法速度值
			pBasePFPro->wMAtkSpeed = (double)(lTotalWisdom)/OccuProperty.fMAtkSpeedWisdomRatio;
			//计算生命和法力
			//计算
			//float fPowValue = pow(fOccuLvlFactor/5,(float)1.5);
			short shLvlDir = pPPro->wLevel-fOccuLvlFactor;
			if(shLvlDir < 0) shLvlDir = 0;
			//生命值
			pBasePFPro->dwMaxHp = ( OccuProperty.fBaseHp+(pPPro->wLevel)*OccuProperty.fLvlUpHp+lTotalConsti*OccuProperty.fConstiHpRatio )*0.34f;
			
			long lTotalMaxHp = pBasePFPro->dwMaxHp+pExPFPro->dwMaxHp;
			if(lTotalMaxHp <= 1)	lTotalMaxHp = 1;

			if(pPPro->dwHp == 0 || pPPro->dwHp > lTotalMaxHp )
				pPPro->dwHp = lTotalMaxHp;
			//法力值
			pBasePFPro->wMaxMp = OccuProperty.fBaseMp+(pPPro->wLevel)*OccuProperty.fLvlUpMp+lTotalSpiri*OccuProperty.fWillMpRatio;
			
			long lTotalMaxMp = pBasePFPro->wMaxMp+pExPFPro->wMaxMp;
			if(lTotalMaxMp <= 1)	lTotalMaxMp = 1;

			if(pPPro->wMp == 0 || pPPro->wMp > lTotalMaxMp )
				pPPro->wMp = lTotalMaxMp;
			//Hp恢复速度值	每5秒生命恢复=当前体质*（1-职业等级系数/220）
			float fTemp = pow((float)(1.0/pPPro->wLevel),(float)0.1);
			pBasePFPro->wHpRecoverSpeed = (double)((((lTotalConsti*(1-pPPro->wLevel/220) + lTotalConsti)*(3-pPPro->wLevel/200))/5)*2)*fTemp;
			//Mp恢复速度值
			pBasePFPro->wMpRecoverSpeed = (double)((((lTotalSpiri*(1-pPPro->wLevel/220) + lTotalSpiri)*(3-pPPro->wLevel/200))/5)*2)*fTemp;
			//能量值
			pBasePFPro->wMaxEnergy = OccuProperty.fMaxBaseEnergy;				

			pBasePFPro->wEnerRecoverSpeed = OccuProperty.fBaseEnergyRecoverValue;
			pBasePFPro->wFightEnerRecoverSpeed = OccuProperty.fFightEnergyRecoverValue;

			// PK CP值
			pBasePFPro->lMaxPKCP = OccuProperty.lBasePKCP + pPPro->wLevel * OccuProperty.lLvlupAddPKCP;
			long lTotalPKCP = pBasePFPro->lMaxPKCP + pExPFPro->lMaxPKCP;
			if(  pPPro->lCurPKCP > lTotalPKCP )
			{
				pPPro->lCurPKCP = lTotalPKCP;
			}
		}
	}
}

double CPlayerList::GetOccuValueRatio(const char* pszName,int nOccu)
{
	double dbRet = 0;
	if(NULL == pszName )	return dbRet;
	map<DWORD, tagOccupModifyProperty>::iterator it =
		m_mapOccuModifyPropertyList.find(nOccu);
	if(it != m_mapOccuModifyPropertyList.end())
	{
		tagOccupModifyProperty &OccuProperty = (*it).second;
		if(0 == strcmp(pszName,"block"))
			dbRet = OccuProperty.fBlockValueRatio;
		else if(0 == strcmp(pszName,"parry"))
			dbRet = OccuProperty.fParryValueRatio;
		else if(0 == strcmp(pszName,"dodge"))
			dbRet = OccuProperty.fDodgeValueRatio;
		else if(0 == strcmp(pszName,"cri"))
			dbRet = OccuProperty.fCriValueRatio;
		else if(0 == strcmp(pszName,"hit"))
			dbRet = OccuProperty.fHitValueRatio;
		else if(0 == strcmp(pszName,"basehit"))
			dbRet = OccuProperty.fBaseHit;
		else if(0 == strcmp(pszName,"cridamage"))
			dbRet = OccuProperty.fCriDamageValueRatio;
		else if(0 == strcmp(pszName,"cridef"))
			dbRet = OccuProperty.fCriDefValueRatio;
		else if(0 == strcmp(pszName,"condef"))
			dbRet = OccuProperty.fConstiDefValueRatio;
		else if(0 == strcmp(pszName,"mcridamage"))
			dbRet = OccuProperty.fMCriDamageValueRatio;
		else if(0 == strcmp(pszName,"willdef"))
			dbRet = OccuProperty.fWillDefValueRatio;
		else if(0 == strcmp(pszName,"mcridef"))
			dbRet = OccuProperty.fMCriDefValueRatio;
		else if(0 == strcmp(pszName,"mcri"))
			dbRet = OccuProperty.fMCriValueRatio;
		else if(0 == strcmp(pszName,"mdef"))
			dbRet = OccuProperty.fMDefValueRatio;
		else if(0 == strcmp(pszName,"absorb"))
			dbRet = OccuProperty.fAbsorbValueRatio;
		else if(0 == strcmp(pszName,"buffatkpower"))
			dbRet = OccuProperty.fBuffPowerValueRatio;
		else if(0 == strcmp(pszName,"matkspeed"))
			dbRet = OccuProperty.fMAtkSpeedValueRatio;
		else if(0 == strcmp(pszName,"baseenergyrecovertime"))
			dbRet = OccuProperty.fBaseEnergyRecoverTime;
		else if (0 == strcmp(pszName,"fightenergyrecovertime"))
			dbRet = OccuProperty.fFightEnergyRecoverTime;
		else if(0 == strcmp(pszName,"attackenergyvalue"))
			dbRet = OccuProperty.fAttackEnergyValue;
		else if(0 == strcmp(pszName,"blastenertyvalue"))
			dbRet = OccuProperty.fBlastEnertyValue;
		else if(0 == strcmp(pszName,"beenattackenergyvalue"))
			dbRet = OccuProperty.fBeenAttackEnergyValue;
		else if (0 == strcmp(pszName,"beenblastenergyvalue"))
			dbRet = OccuProperty.fBeenBlastEnergyValue;
		else if( 0 == strcmp( pszName, "basepkcp" ) )
			dbRet = OccuProperty.lBasePKCP;
		else if( 0 == strcmp( pszName, "lvlupaddpkcp" ) )
			dbRet = OccuProperty.lLvlupAddPKCP;

		
		
	}
	return dbRet;
}

