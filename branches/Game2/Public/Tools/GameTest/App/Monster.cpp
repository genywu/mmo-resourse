#include "StdAfx.h"
#include "..\GameTest\resource.h"

#include "..\GameTest\GameTestDlg.h"
#include ".\monster.h"
//#include "..\Net\Message.h"
#include "..\Script\ScriptSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMonster::CMonster(void)
{
	SetType(TYPE_MONSTER);
	//memset(&m_BaseProperty.dwHP, 0, sizeof(tagMPro)-sizeof(string));
	SetHp(0);

	//FADE IN
	//--------------07.12.6liuke------------------
	//m_bIsCanTalk = FALSE;
	m_strTitle="";				// Monster的称号
	m_strOrigName = "";			// 原始名
	m_lCreatorType = 0;			// 创建关联者的类型

	m_State[STATE_STEP].iState = STATE_STEP;				// 步骤
	m_State[STATE_TRACE].iState = STATE_TRACE;				// 追踪
	m_State[STATE_QUEST].iState = STATE_QUEST;				// 任务

	m_CreatorGuid = CGUID::GUID_INVALID;

	InitNameValueMap();
}

CMonster::~CMonster(void)
{
}
// 初始化变量名到值得影射
void CMonster::InitNameValueMap()
{
	CMoveShape::InitNameValueMap();
	//tagPty st;// 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool

	//st.lType=4; st.pValue=&m_lType;             m_PropertyEnumToName[M_ATTR_TYPE]="Type";				m_mapNameValue["Type"] = st;		
	//st.lType=4; st.pValue=0/*&m_lGraphicsID*/;		m_PropertyEnumToName[M_ATTR_GRAPHICSID]="GraphicsID";	m_mapNameValue["GraphicsID"]=st;	
	//st.lType=4; st.pValue=&m_lRegionID;		    m_PropertyEnumToName[M_ATTR_REGIONID]="RegionID";		m_mapNameValue["RegionID"]=st;
	//st.lType=3;	st.pfValue=&m_fPosX;		    m_PropertyEnumToName[M_ATTR_TILEX]="TileX";				m_mapNameValue["TileX"]=st;			
	//st.lType=3;	st.pfValue=&m_fPosY;		    m_PropertyEnumToName[M_ATTR_TILEY]="TileY";				m_mapNameValue["TileY"]=st;	
	//st.lType=3;	st.pfValue=&m_fDir;			    m_PropertyEnumToName[M_ATTR_DIR]="Dir";					m_mapNameValue["Dir"]=st;			
	//st.lType=1;	st.pwValue=&m_wState;			m_PropertyEnumToName[M_ATTR_STATE]="State";				m_mapNameValue["State"]=st;			
	//st.lType=1;	st.pwValue=&m_wAction;		    m_PropertyEnumToName[M_ATTR_ACTION]="Action";			m_mapNameValue["Action"]=st;		


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///*怪物属性*/
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool
	//st.lType=2;	st.pdwValue = &m_BaseProperty.dwIndex;					m_PropertyEnumToName[M_ATTR_INDEX]="Index"; 			m_mapNameValue["Index"]=st;		
	//st.lType=2;	st.pdwValue	= &m_BaseProperty.dwPicLevel;				m_PropertyEnumToName[M_ATTR_PICLEVEL]="PicLevel";		m_mapNameValue["PicLevel"]=st;			   
	//st.lType=2;	st.pdwValue	= &m_BaseProperty.dwNameColor;				m_PropertyEnumToName[M_ATTR_NAMECOLOR]="NameColor";		m_mapNameValue["NameColor"]=st;			

	//st.lType=2;	st.pdwValue	= &m_BaseProperty.dwHpBarColor;			m_PropertyEnumToName[M_ATTR_HPBARCOLOR]="HpBarColor";		m_mapNameValue["HpBarColor"]=st;		
	//st.lType=2;	st.pdwValue	= &m_BaseProperty.dwSoundID;				m_PropertyEnumToName[M_ATTR_SOUNDID]="SoundID";			m_mapNameValue["SoundID"]=st;			
	//st.lType=1;	st.pwValue  = &m_BaseProperty.wLevel;					m_PropertyEnumToName[M_ATTR_LEVEL]="Level";				m_mapNameValue["Level"]=st;							
	//st.lType=2; st.pdwValue  = &m_BaseProperty.dwExp;					m_PropertyEnumToName[M_ATTR_EXP]="Exp";					m_mapNameValue["Exp"]=st;				
	//st.lType=2; st.pdwValue  = &m_BaseProperty.dwOccuPoint;			m_PropertyEnumToName[M_ATTR_OCCUPOINT]="OccuPoint";			m_mapNameValue["OccuPoint"]=st;				
	//st.lType=4;	st.plValue	= &m_BaseProperty.byCountry;				m_PropertyEnumToName[M_ATTR_COUNTRY]="byCountry";		m_mapNameValue["byCountry"]=st;			
	//st.lType=4;	st.plValue	= &m_BaseProperty.byTamable;				m_PropertyEnumToName[M_ATTR_TAMABLE]="byTamable";		m_mapNameValue["byTamable"]=st;			
	//st.lType=2; st.pdwValue	= &m_BaseProperty.dwMaxTameCount;			m_PropertyEnumToName[M_ATTR_MAXTAMECOUNT]="MaxTameCount"; m_mapNameValue["MaxTameCount"]=st;		

	//st.lType=1; st.pwValue	= &m_BaseProperty.wCountryContriValue;	m_PropertyEnumToName[M_ATTR_COUNTRYCONTRIVALUE]="CountryContriValue";	m_mapNameValue["CountryContriValue"]=st;
	//st.lType=1;	st.pwValue = &m_BaseProperty.wSPContriValue;			m_PropertyEnumToName[M_ATTR_SPCONTRIVALUE]="SPContriValue";				m_mapNameValue["SPContriValue"]=st;		
	//st.lType=2;	st.pdwValue  = (DWORD*)&m_BaseProperty.eEType;		m_PropertyEnumToName[M_ATTR_ELETYPE_]="EleType";   						m_mapNameValue["EleType"]=st;	        
	//st.lType=2;	st.pdwValue = (DWORD*)&m_BaseProperty.eNType;		m_PropertyEnumToName[M_ATTR_NPCTYPE_]="NpcType";  						m_mapNameValue["NpcType"]=st;         
	//st.lType=2;	st.pdwValue = (DWORD*)&m_BaseProperty.eClass_;					m_PropertyEnumToName[M_ATTR_CLASS]="Class"; 							m_mapNameValue["Class"]=st;				
	//st.lType=1;	st.pwValue = &m_BaseProperty.wRace;                   m_PropertyEnumToName[M_ATTR_RACE]="Race";     							m_mapNameValue["Race"]=st;	            
	//st.lType=1; st.pwValue  = &m_BaseProperty.wMoveRandomValue;       m_PropertyEnumToName[M_ATTR_MOVERANDOMVALUE]="MoveRandomValue"; 			m_mapNameValue["MoveRandomValue"]=st;	
	//st.lType=1; st.pwValue = &m_BaseProperty.wStopTime;                m_PropertyEnumToName[M_ATTR_STOPTIME]="StopTime"; 						m_mapNameValue["StopTime"]=st;	        
	//st.lType=1; st.pwValue  = &m_BaseProperty.wAIHangupType;	        m_PropertyEnumToName[M_ATTR_AIHANGUPTYPE]="AIHangupType";				m_mapNameValue["AIHangupType"]=st;		
	//st.lType=1; st.pwValue = &m_BaseProperty.wFigure;                 m_PropertyEnumToName[M_ATTR_FIGURE]="Figure";    							m_mapNameValue["Figure"]=st;	        

	//st.lType=4;	st.pdwValue	= &m_BaseProperty.dwHp;                   m_PropertyEnumToName[M_ATTR_HP]="Hp";										m_mapNameValue["Hp"]=st;				
	//st.lType=2;	st.pdwValue	= &m_BaseProperty.dwAIType;                m_PropertyEnumToName[M_ATTR_AITYPE]="AIType";							m_mapNameValue["AIType"]=st;			

	//st.lType=3;	st.pfValue = &m_BaseProperty.fHurtProportion;         m_PropertyEnumToName[M_ATTR_HURTPROPORTION]="fHurtProportion";			m_mapNameValue["fHurtProportion"]=st;	
	//st.lType=2; st.pdwValue = &m_BaseProperty.dwGuardTime;            m_PropertyEnumToName[M_ATTR_GUARDTIME]="GuardTime";						m_mapNameValue["GuardTime"]=st;			
	//st.lType=3; st.pfValue = &m_BaseProperty.fEscapePoint;             m_PropertyEnumToName[M_ATTR_ESCAPEPOINT]="EscapePoint";					m_mapNameValue["EscapePoint"]=st;		
	//st.lType=1; st.pwValue = &m_BaseProperty.wSkillProportion;        m_PropertyEnumToName[M_ATTR_SKILLPROPORTION]="SkillProportion";			m_mapNameValue["SkillProportion"]=st;		
	//st.lType=2; st.pdwValue = &m_BaseProperty.dwSkillSpace;            m_PropertyEnumToName[M_ATTR_SKILLSPACE]="SkillSpace";					m_mapNameValue["SkillSpace"]=st;		
	//st.lType=3;	st.pfValue	= &m_BaseProperty.fMoveSpeed;	;			m_PropertyEnumToName[M_ATTR_MOVESPEED]="MoveSpeed";						m_mapNameValue["MoveSpeed"]=st;		
	//st.lType=3;	st.pfValue	= &m_BaseProperty.fFightMoveSpeed;			m_PropertyEnumToName[M_ATTR_FIGHTMOVESPEED]="FightMoveSpeed";			m_mapNameValue["FightMoveSpeed"]=st;		
	//st.lType=1; st.pwValue = &m_BaseProperty.wCanTalk;             m_PropertyEnumToName[M_ATTR_CANTALK]="Occu";					m_mapNameValue["CanTalk"]=st;

	////for(i=0; i<MONSTER_AI_REFERRENCE_NUM; ++i)
	//{
	//	st.lType=2;	st.pdwValue	= &m_BaseProperty.dwAIReferrence[0]; m_PropertyEnumToName[M_ATTR_AIREFERRENCE0]="AIReferrence0";				m_mapNameValue["AIReferrence0"]=st; 
	//	st.lType=2;	st.pdwValue	= &m_BaseProperty.dwAIReferrence[1]; m_PropertyEnumToName[M_ATTR_AIREFERRENCE1]="AIReferrence1";				m_mapNameValue["AIReferrence1"]=st; 
	//	st.lType=2;	st.pdwValue	= &m_BaseProperty.dwAIReferrence[2]; m_PropertyEnumToName[M_ATTR_AIREFERRENCE2]="AIReferrence2";				m_mapNameValue["AIReferrence2"]=st; 
	//	st.lType=2;	st.pdwValue	= &m_BaseProperty.dwAIReferrence[3]; m_PropertyEnumToName[M_ATTR_AIREFERRENCE3]="AIReferrence3";				m_mapNameValue["AIReferrence3"]=st; 
	//	st.lType=2;	st.pdwValue	= &m_BaseProperty.dwAIReferrence[4]; m_PropertyEnumToName[M_ATTR_AIREFERRENCE4]="AIReferrence4";				m_mapNameValue["AIReferrence4"]=st; 
	//}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///*战斗属性*/ 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//st.lType=4;	st.pValue	= &m_FightProperty.dwMaxHp;		m_PropertyEnumToName[M_ATTR_MAXHP]="C_MaxHp";						m_mapNameValue["C_MaxHp"]=st;			
	//st.lType=5;	st.psValue	= &m_FightProperty.wHpRecoverSpeed;m_PropertyEnumToName[M_ATTR_HPRECOVERSPEED]="C_HpRecoverSpeed";	m_mapNameValue["C_HpRecoverSpeed"]=st;	
	//st.lType=5;	st.psValue	= &m_FightProperty.wMinAtk;		m_PropertyEnumToName[M_ATTR_MINATK]="C_MinAtk";						m_mapNameValue["C_MinAtk"]=st;			
	//st.lType=5;	st.psValue	= &m_FightProperty.wMaxAtk;		m_PropertyEnumToName[M_ATTR_MAXATK]="C_MaxAtk";						m_mapNameValue["C_MaxAtk"]=st;			
	//st.lType=5;	st.psValue	= &m_FightProperty.wMinMAtk;		m_PropertyEnumToName[M_ATTR_MINMATK]="C_MinMAtk";				m_mapNameValue["C_MinMAtk"]=st;			
	//st.lType=5;	st.psValue	= &m_FightProperty.wMaxMAtk;		m_PropertyEnumToName[M_ATTR_MAXMATK]="C_MaxMAtk";				m_mapNameValue["C_MaxMAtk"]=st;			
	//st.lType=5;	st.psValue	= &m_FightProperty.wHit;			m_PropertyEnumToName[M_ATTR_HIT]="C_Hit";						m_mapNameValue["C_Hit"]=st;				
	//st.lType=5;	st.psValue	= &m_FightProperty.wDef;			m_PropertyEnumToName[M_ATTR_DEF]="C_Def";						m_mapNameValue["C_Def"]=st;		
	//st.lType=5;	st.psValue	= &m_FightProperty.wDef;			m_PropertyEnumToName[M_ATTR_MDEF]="C_MDef";						m_mapNameValue["C_MDef"]=st;		
	//st.lType=5;	st.psValue	= &m_FightProperty.wDodge;			m_PropertyEnumToName[M_ATTR_DODGE]="C_Dodge";					m_mapNameValue["C_Dodge"]=st;			
	//st.lType=5;	st.psValue	= &m_FightProperty.wBlock;			m_PropertyEnumToName[M_ATTR_BLOCK]="C_Block";					m_mapNameValue["C_Block"]=st;			
	//st.lType=5;	st.psValue	= &m_FightProperty.wParry;			m_PropertyEnumToName[M_ATTR_PARRY]="C_Parry";					m_mapNameValue["C_Parry"]=st;			

	//st.lType=5;	st.psValue	= &m_FightProperty.wFarAtkDis;		m_PropertyEnumToName[M_ATTR_FARATKDIS]="Occu";					m_mapNameValue["C_FarAtkDis"]=st;		
	//st.lType=5;	st.psValue	= &m_FightProperty.wNeaAtkDis;		m_PropertyEnumToName[M_ATTR_NEAATKDIS]="Occu";					m_mapNameValue["C_NeaAtkDis"]=st;		
	//st.lType=5;	st.psValue	= &m_FightProperty.wGuardRange;	m_PropertyEnumToName[M_ATTR_GUARDRANGE]="Occu";						m_mapNameValue["C_GuardRange"]=st;		
	//st.lType=5;	st.psValue	= &m_FightProperty.wChaseRange;	m_PropertyEnumToName[M_ATTR_CHASERANGE]="Occu";						m_mapNameValue["C_ChaseRange"]=st;		
	//st.lType=5;	st.psValue	= &m_FightProperty.wWillDef;		m_PropertyEnumToName[M_ATTR_WILLDEF]="Occu";					m_mapNameValue["C_WillDef"]=st;			
	//st.lType=5;	st.psValue	= &m_FightProperty.wConstiDef;		m_PropertyEnumToName[M_ATTR_CONSTIDEF]="Occu";					m_mapNameValue["C_ConstiDef"]=st;		
	//st.lType=5;	st.psValue	= &m_FightProperty.wCri;			m_PropertyEnumToName[M_ATTR_CRI]="Occu";						m_mapNameValue["C_Cri"]=st;				
	//st.lType=5;	st.psValue	= &m_FightProperty.wCriDef;		m_PropertyEnumToName[M_ATTR_CRIDEF]="Occu";							m_mapNameValue["C_CriDef"]=st;			
	//st.lType=5;	st.psValue	= &m_FightProperty.wMCriDef;		m_PropertyEnumToName[M_ATTR_MCRIDEF]="Occu";					m_mapNameValue["C_MCriDef"]=st;     


	//{
	//	st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DFT_Bled];	m_PropertyEnumToName[M_ATTR_DEBUFFDEF_BLED]="C_DeBuffDef0";				m_mapNameValue["C_DeBuffDef0"]=st;      
	//	st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Dizzy];m_PropertyEnumToName[M_ATTR_DEBUFFDEF_DIZZY]="C_DeBuffDef1";				m_mapNameValue["C_DeBuffDef1"]=st;	    
	//	st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Lull];	m_PropertyEnumToName[M_ATTR_DEBUFFDEF_LULL]="C_DeBuffDef2";				m_mapNameValue["C_DeBuffDef2"]=st;	    
	//	st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Hypnosis];;	m_PropertyEnumToName[M_ATTR_DEBUFFDEF_HYPNOSIS]="C_DeBuffDef3";		m_mapNameValue["C_DeBuffDef3"]=st;  
	//	st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Poison];	m_PropertyEnumToName[M_ATTR_DEBUFFDEF_POSION]="C_DeBuffDef4";		m_mapNameValue["C_DeBuffDef4"]=st;	
	//	st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Fix];	m_PropertyEnumToName[M_ATTR_DEBUFFDEF_FIX]="C_DeBuffDef5";				m_mapNameValue["C_DeBuffDef5"]=st;	    
	//}																														   

	//{
	//	st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Fire];m_PropertyEnumToName[M_ATTR_ELEMDEF_FIRE]="C_ElemDef0";		m_mapNameValue["C_ElemDef0"]=st;	
	//	st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Water];m_PropertyEnumToName[M_ATTR_ELEMDEF_WATER]="C_ElemDef1";	m_mapNameValue["C_ElemDef1"]=st;	
	//	st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Earth];m_PropertyEnumToName[M_ATTR_ELEMDEF_EARTH]="C_ElemDef2";	m_mapNameValue["C_ElemDef2"]=st;	
	//	st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Wind];m_PropertyEnumToName[M_ATTR_ELEMDEF_WIND]="C_ElemDef3";		m_mapNameValue["C_ElemDef3"]=st;	
	//	st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Dark];m_PropertyEnumToName[M_ATTR_ELEMDEF_DARK]="C_ElemDef4";		m_mapNameValue["C_ElemDef4"]=st;	
	//	st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Holy];m_PropertyEnumToName[M_ATTR_ELEMDEF_HOLY]="C_ElemDef5";		m_mapNameValue["C_ElemDef5"]=st;	
	//}

	//{
	//	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Human];m_PropertyEnumToName[M_ATTR_CLASSDAM_HUMAN]="C_ClassDam0";			m_mapNameValue["C_ClassDam0"]=st;		
	//	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Element];m_PropertyEnumToName[M_ATTR_CLASSDAM_ELEMENT]="C_ClassDam1";		m_mapNameValue["C_ClassDam1"]=st;	
	//	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Alien];m_PropertyEnumToName[M_ATTR_CLASSDAM_ALIEN]="C_ClassDam2";			m_mapNameValue["C_ClassDam2"]=st;		
	//	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Animal];m_PropertyEnumToName[M_ATTR_CLASSDAM_ANIMAL]="C_ClassDam3";		m_mapNameValue["C_ClassDam3"]=st;		
	//	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Undead];m_PropertyEnumToName[M_ATTR_CLASSDAM_UNDEAD]="C_ClassDam4";		m_mapNameValue["C_ClassDam4"]=st;		
	//	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Evil];m_PropertyEnumToName[M_ATTR_CLASSDAM_EVIL]="C_ClassDam5";			m_mapNameValue["C_ClassDam5"]=st;		
	//	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Hexapods];m_PropertyEnumToName[M_ATTR_CLASSDAM_HEXAPODS]="C_ClassDam6";	m_mapNameValue["C_ClassDam6"]=st;	
	//}																														  

	//{
	//	st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Fire];m_PropertyEnumToName[M_ATTR_ELEMDAM_FIRE]="C_ElemDam0";		m_mapNameValue["C_ElemDam0"]=st;  
	//	st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Water];m_PropertyEnumToName[M_ATTR_ELEMDAM_WATER]="C_ElemDam1";	m_mapNameValue["C_ElemDam1"]=st; 
	//	st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Earth];m_PropertyEnumToName[M_ATTR_ELEMDAM_EARTH]="C_ElemDam2";	m_mapNameValue["C_ElemDam2"]=st; 
	//	st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Wind];m_PropertyEnumToName[M_ATTR_ELEMDAM_WIND]="C_ElemDam3";		m_mapNameValue["C_ElemDam3"]=st;  
	//	st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Dark];m_PropertyEnumToName[M_ATTR_ELEMDAM_DARK]="C_ElemDam4";		m_mapNameValue["C_ElemDam4"]=st;  
	//	st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Holy];m_PropertyEnumToName[M_ATTR_ELEMDAM_HOLY]="C_ElemDam5";		m_mapNameValue["C_ElemDam5"]=st;  

	//}
}


void CMonster::SetPosXY(float x, float y)
{
	// 将之前位置的阻挡去除
	if(GetFather())
		SetBlock(GetTileX(), GetTileY(), CRegion::BLOCK_NO);
	CShape::SetPosXY(x,y);
	// 设置新的阻挡位置
	if(GetFather())
		SetBlock(GetTileX(), GetTileY(), GetBlockType());
}
// 解码
bool CMonster::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	// 	WORD	wClass = _GetWordFromByteArray(pByte,pos);
	// 	WORD	wFigure = _GetWordFromByteArray(pByte,pos);
	// 	DWORD	dwSounID = _GetDwordFromByteArray(pByte,pos);
	// 	DWORD	dwPicLevel = _GetDwordFromByteArray(pByte,pos);
	// 	DWORD	dwNameColor = _GetDwordFromByteArray(pByte,pos);
	// 	DWORD	dwHpBarColor = _GetDwordFromByteArray(pByte,pos);
	// 	WORD	wLevel = _GetWordFromByteArray(pByte,pos);
	// 
	// 	//m_BaseProperty.eClass_ = (eClass)wClass;
	// 	//m_BaseProperty.wFigure = wFigure;
	// 	//m_BaseProperty.dwSoundID = dwSounID;
	// 	//m_BaseProperty.dwPicLevel = dwPicLevel;
	// 	//m_BaseProperty.dwNameColor = dwNameColor;
	// 	//m_BaseProperty.dwHpBarColor = dwHpBarColor;
	// 	//m_BaseProperty.wLevel = wLevel;
	// 	//////////////////////////////////////////////////////////////////////////
	// 	CMoveShape::DecordFromByteArray(pByte, pos, bExData);
	// 	//
	// 	long lMonsterType = -1;
	// 	WORD wIsCanTalk = 0;
	// 	char str[512];
	// 	memset(str,'\0',512);
	// 	//原始名
	// 	m_strOrigName = _GetStringFromByteArray(pByte,pos,str);
	// 	lMonsterType = _GetLongFromByteArray(pByte,pos);
	// 	wIsCanTalk = _GetWordFromByteArray(pByte,pos);
	// 	/*wIcon = */_GetWordFromByteArray(pByte,pos);
	// 	/*wColor = */_GetWordFromByteArray(pByte,pos);
	// 	m_strTitle = _GetStringFromByteArray(pByte,pos,str);
	// 	//
	// 	BYTE byBlockType = _GetByteFromByteArray(pByte,pos);
	// 	SetBlockType(CRegion::eBLOCK(byBlockType));
	// 	//
	// 	m_lCreatorType = _GetLongFromByteArray(pByte,pos);
	// 	_GetBufferFromByteArray(pByte,pos,m_CreatorGuid);
	// 	//
	// 	//SetCountry((BYTE)_GetByteFromByteArray(pByte,pos));
	// 	_GetByteFromByteArray(pByte,pos);
	// 	//转身速度
	// 	_GetFloatFromByteArray(pByte,pos);
	// 	// BOSS等级
	// 	WORD wBossLevel = _GetWordFromByteArray(pByte,pos);
	// //	SetBossLevel(wBossLevel);
	// 	SeteNpcType(eNpcType(lMonsterType));
	// 	SetIsCanTalk(wIsCanTalk);
	// 	//////////////////////////////////////////////////////////////////////////
	// 	//怪物多重血条
	// 			long lBaseMaxHp = _GetLongFromByteArray(pByte,pos);
	// 			long lSize = _GetLongFromByteArray(pByte,pos);
	// 			for( int i=0;i<lSize;i++)
	// 			{
	// 				long lHpRange = _GetLongFromByteArray(pByte,pos);
	// 			}
	// 	//////////////////////////////////////////////////////////////////////////
	// 	//
	// 	//
	// 	DecordChangeProperty(pByte,pos);
	//
	/*BYTE bIsPet = _GetByteFromByteArray(pByte,pos);
	if( bIsPet == 1)
	{
	_GetLongFromByteArray(pByte,pos);
	CGUID guid;
	_GetBufferFromByteArray(pByte,pos,guid);
	char str[128];
	_GetStringFromByteArray(pByte,pos,str);
	_GetDwordFromByteArray(pByte,pos);
	_GetDwordFromByteArray(pByte,pos);
	}
	else if( bIsPet == 2)
	{
	_GetLongFromByteArray(pByte,pos);
	CGUID guid;
	_GetBufferFromByteArray(pByte,pos,guid);
	char str[128];
	_GetStringFromByteArray(pByte,pos,str);
	}

	if( bIsPet != 1)
	memset(&m_tPetInfo,0,sizeof(tagPetInfo));

	if( bIsPet != 2 )
	memset(& m_tCarriageInfo,0,sizeof(tagCarriageInfo));
	*/
	return true;
}

bool CMonster::DecordFromDataBlock(DBReadSet& readDB,bool bExData /* = true */)
{
	CMoveShape::DecordFromDataBlock(readDB, bExData);

	DWORD	dwHp = readDB.GetDwordFromByteArray();
	DWORD	dwMaxHp = readDB.GetDwordFromByteArray();
	WORD	wClass = readDB.GetWordFromByteArray();
	WORD	wFigure = readDB.GetWordFromByteArray();
	DWORD	dwSounID = readDB.GetDwordFromByteArray();
	DWORD	dwNameColor = readDB.GetDwordFromByteArray();
	DWORD	dwHpBarColor = readDB.GetDwordFromByteArray();
	WORD	wLevel = readDB.GetWordFromByteArray();
	/*m_dwCollectID		= */readDB.GetDwordFromByteArray();
	//if( m_dwCollectID != 0 )
	//{

	//}

	/*m_bIsBeenCollect        = */(readDB.GetByteFromByteArray() == 0)?false : true;
	/*m_bCanBeenCollect		= */(readDB.GetByteFromByteArray() == 0)?false : true;

	//	SetMaxHp(dwMaxHp);
	SetHp(dwHp);
	//	SetClass((eClass)wClass);
	//	SetFigure(wFigure);
	//	SetLevel(wLevel);


	long lMonsterType = -1;
	WORD wIsCanTalk = 0;
	char str[512];
	memset(str,'\0',512);
	//原始名
	m_strOrigName = readDB.GetStringFromByteArray( str,512);
	lMonsterType = readDB.GetWordFromByteArray();
	wIsCanTalk = readDB.GetWordFromByteArray();
	/*wIcon = */readDB.GetWordFromByteArray();
	/*wColor = */readDB.GetWordFromByteArray();
	m_strTitle = readDB.GetStringFromByteArray( str,512);
	// 设置阻挡类型
	BYTE bBlockType = readDB.GetByteFromByteArray();
	SetBlockType(CRegion::eBLOCK(bBlockType));
	// 任务保护信息(类型和GUID)
	m_lCreatorType = readDB.GetLongFromByteArray();
	readDB.GetBufferFromByteArray(m_CreatorGuid);
	//
	// 国家ID
	//SetCountry(readDB.GetByteFromByteArray());
	readDB.GetByteFromByteArray();
	//转身速度
	readDB.GetFloatFromByteArray();
	// BOSS等级
	WORD wBossLevel = readDB.GetWordFromByteArray();
	//	SetBossLevel(wBossLevel);

	SeteNpcType(eNpcType(lMonsterType));
	SetIsCanTalk(wIsCanTalk);
	//////////////////////////////////////////////////////////////////////////
	//怪物多重血条
	long lBaseMaxHp = readDB.GetLongFromByteArray();
	long lSize = readDB.GetLongFromByteArray();
	for( int i=0;i<lSize;i++)
	{
		long lHpRange = readDB.GetLongFromByteArray();
	}

	//	this->m_bAppearanceSoundPlayed = false;
	return true;
}

void  CMonster::DecordChangeProperty(BYTE* pByte, long& pos)
{
	CMoveShape::DecordChangeProperty(pByte,pos);
	WORD lNum = _GetWordFromByteArray(pByte,pos);
	WORD pro_type = 0;
	BYTE data_type = 0;
	WORD data_length = 0;
	long data = 0;
	BYTE* byte_Array = NULL;
	WORD* word_Array = NULL;
	DWORD* dword_Array = NULL;
	float* flaot_Array = NULL;
	long*  long_Array = NULL;
	short* short_Array = NULL;
	char*  char_Array = NULL;
	bool*  bool_Array = NULL;

	for (int i = 0;i<lNum;i++)
	{
		pro_type = _GetWordFromByteArray(pByte,pos);
		data_type = _GetByteFromByteArray(pByte, pos);
		data_length = _GetWordFromByteArray(pByte,pos);		
		if (data_length<=0) continue;

		//根据数据类型获取具体的数据值
		switch(data_type)
		{
		case ATTR_DATA_BYTE:
			data = _GetByteFromByteArray(pByte, pos);
			break;
		case ATTR_DATA_WORD:
			data = _GetWordFromByteArray(pByte,pos);
			break;
		case ATTR_DATA_DWORD:
			data = _GetDwordFromByteArray(pByte,pos);
			break;
		case ATTR_DATA_FLOAT:
			data = _GetFloatFromByteArray(pByte,pos);
			break;
		case ATTR_DATA_LONG:
			data = _GetLongFromByteArray(pByte,pos);
			break;
		case ATTR_DATA_SHORT:
			data = _GetShortFromByteArray(pByte,pos);
			break;
		case ATTR_DATA_BOOL:
			data = _GetByteFromByteArray(pByte, pos);
			break;
		case ATTR_DATA_BYTEARRAY:
			byte_Array = new BYTE[data_length];
			if (!byte_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				byte_Array[i] = _GetByteFromByteArray(pByte, pos);
			}
			break;
		case ATTR_DATA_WORDARRAY:
			word_Array = new WORD[data_length];
			if (!word_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				word_Array[i] = _GetWordFromByteArray(pByte,pos);
			}
			break;
		case ATTR_DATA_DWORDARRAY:
			dword_Array = new DWORD[data_length];
			if (!dword_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				dword_Array[i] = _GetDwordFromByteArray(pByte,pos);
			}
			break;
		case ATTR_DATA_FLOATARRAY:
			flaot_Array = new float[data_length];
			if (!flaot_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				flaot_Array[i] = _GetFloatFromByteArray(pByte,pos);
			}
			break;
		case ATTR_DATA_LONGARRAY:
			long_Array = new long[data_length];
			if (!long_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				long_Array[i] = _GetLongFromByteArray(pByte,pos);
			}
			break;
		case ATTR_DATA_SHORTARRAY:
			short_Array = new short[data_length];
			if (!short_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				short_Array[i] = _GetShortFromByteArray(pByte,pos);
			}
			break;
		case ATTR_DATA_CHARARRAY:
			char_Array = new char[data_length];
			if (!char_Array) return;
			_GetStringFromByteArray(pByte, pos, char_Array);
			break;
		case ATTR_DATA_BOOLARRAY:
			bool_Array = new bool[data_length];
			if (!bool_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				data = _GetByteFromByteArray(pByte, pos);
				if (data!=0)
				{
					bool_Array[i] = true;
				}else
					bool_Array[i] = false;
			}
			break;
		default:
			break;
		}

		//设置相应属性值
		//itProETN it = m_PropertyEnumToName.find(pro_type);
		//if(it != m_PropertyEnumToName.end())
		//{
		//	SetProperty( (*it).second.c_str(),data);
		//}
		SAFE_DELETE_ARRAY(byte_Array);
		SAFE_DELETE_ARRAY(word_Array);
		SAFE_DELETE_ARRAY(dword_Array);
		SAFE_DELETE_ARRAY(flaot_Array);
		SAFE_DELETE_ARRAY(long_Array);
		SAFE_DELETE_ARRAY(short_Array);
		SAFE_DELETE_ARRAY(char_Array);
		SAFE_DELETE_ARRAY(bool_Array);
	}
}

void CMonster::UpdateProperty(CMessage* pMsg)
{
	DBReadSet db_r;
	pMsg->GetDBReadSet(db_r);
	CMonster::DecordChangePropertyFromDataBlock(db_r);
}

bool CMonster::OnPropertyChanged(const char* attrName)
{
	CMoveShape::OnPropertyChanged(attrName);
	return true;
}

const char* CMonster::GetPropertyStringValue(const char* pszPropertyName)
{
	return CMoveShape::GetPropertyStringValue(pszPropertyName);
}

void CMonster::DecordChangePropertyFromDataBlock(DBReadSet& readDB)
{
	CMoveShape::DecordChangePropertyFromDataBlock(readDB);
	WORD lNum = readDB.GetWordFromByteArray();
	WORD pro_type = 0;
	BYTE data_type = 0;
	WORD data_length = 0;
	long data = 0;
	BYTE* byte_Array = NULL;
	WORD* word_Array = NULL;
	DWORD* dword_Array = NULL;
	float* flaot_Array = NULL;
	long*  long_Array = NULL;
	short* short_Array = NULL;
	char*  char_Array = NULL;
	bool*  bool_Array = NULL;

	for (int i = 0;i<lNum;i++)
	{
		pro_type = readDB.GetWordFromByteArray();
		data_type = readDB.GetByteFromByteArray();
		data_length = readDB.GetWordFromByteArray();		
		if (data_length<=0) continue;

		//根据数据类型获取具体的数据值
		switch(data_type)
		{
		case ATTR_DATA_BYTE:
			data = readDB.GetByteFromByteArray();
			break;
		case ATTR_DATA_WORD:
			data = readDB.GetWordFromByteArray();
			break;
		case ATTR_DATA_DWORD:
			data = readDB.GetDwordFromByteArray();
			break;
		case ATTR_DATA_FLOAT:
			data = readDB.GetFloatFromByteArray();
			break;
		case ATTR_DATA_LONG:
			data = readDB.GetLongFromByteArray();
			break;
		case ATTR_DATA_SHORT:
			data = readDB.GetShortFromByteArray();
			break;
		case ATTR_DATA_BOOL:
			data = readDB.GetByteFromByteArray();
			break;
		case ATTR_DATA_BYTEARRAY:
			byte_Array = new BYTE[data_length];
			if (!byte_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				byte_Array[i] = readDB.GetByteFromByteArray();
			}
			break;
		case ATTR_DATA_WORDARRAY:
			word_Array = new WORD[data_length];
			if (!word_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				word_Array[i] = readDB.GetWordFromByteArray();
			}
			break;
		case ATTR_DATA_DWORDARRAY:
			dword_Array = new DWORD[data_length];
			if (!dword_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				dword_Array[i] = readDB.GetDwordFromByteArray();
			}
			break;
		case ATTR_DATA_FLOATARRAY:
			flaot_Array = new float[data_length];
			if (!flaot_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				flaot_Array[i] = readDB.GetFloatFromByteArray();
			}
			break;
		case ATTR_DATA_LONGARRAY:
			long_Array = new long[data_length];
			if (!long_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				long_Array[i] = readDB.GetLongFromByteArray();
			}
			break;
		case ATTR_DATA_SHORTARRAY:
			short_Array = new short[data_length];
			if (!short_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				short_Array[i] = readDB.GetShortFromByteArray();
			}
			break;
		case ATTR_DATA_CHARARRAY:
			char_Array = new char[data_length];
			if (!char_Array) return;
			readDB.GetStringFromByteArray( char_Array,data_length);
			break;
		case ATTR_DATA_BOOLARRAY:
			bool_Array = new bool[data_length];
			if (!bool_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				data = readDB.GetByteFromByteArray();
				if (data!=0)
				{
					bool_Array[i] = true;
				}else
					bool_Array[i] = false;
			}
			break;
		default:
			break;
		}

		//设置相应属性值
		// 		itProETN it = m_PropertyEnumToName.find(pro_type);
		// 		if(it != m_PropertyEnumToName.end())
		// 		{
		// 			SetProperty( (*it).second.c_str(),data);
		// 		}

		SAFE_DELETE_ARRAY(byte_Array);
		SAFE_DELETE_ARRAY(word_Array);
		SAFE_DELETE_ARRAY(dword_Array);
		SAFE_DELETE_ARRAY(flaot_Array);
		SAFE_DELETE_ARRAY(long_Array);
		SAFE_DELETE_ARRAY(short_Array);
		SAFE_DELETE_ARRAY(char_Array);
		SAFE_DELETE_ARRAY(bool_Array);
	}
}