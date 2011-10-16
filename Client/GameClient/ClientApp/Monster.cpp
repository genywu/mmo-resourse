#include "stdafx.h"
#include "monster.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/ClientRegion.h"
#include "../../GameClient/ClientApp/Other/AudioList.h"
#include "../../GameClient/ClientApp/Other/DeputyOccuSystemClient.h"
#include "../../GameClient/ClientApp/Other/PicList.h"
#include "../../Public/Common/Public.h"
#include "Player.h"
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
	this->m_bAppearanceSoundPlayed=false;

	//FADE IN
	//--------------07.12.6liuke------------------
	//m_bIsCanTalk = FALSE;
	m_strTitle="";				// Monster的称号
	m_wTitleColor = 0;			// 称号的颜色
	m_wIconID = 0;				// Monster的图标ID
	m_strOrigName = "";			// 原始名
	m_lCreatorType = 0;			// 创建关联者的类型
	m_CreatorID = CGUID::GUID_INVALID;
	m_lCurQuestEffectID = 0;
	m_fTurnSpeed = 0.f;
	//--------------------------------------------
	m_bFade = TRUE;
	m_bFadeIn = TRUE;
	m_dwFadeStartTime = timeGetTime();
	m_dwFadeTimeLegth = 2500;

	m_bDeadStatus = FALSE;

	m_dwPetLastDiedTime=m_dwPetLastRefreshTime=0;

	m_State[STATE_STEP].iState = STATE_STEP;				// 步骤
	m_State[STATE_TRACE].iState = STATE_TRACE;				// 追踪
	m_State[STATE_QUEST].iState = STATE_QUEST;				// 任务
	for (int i = 0;i<EFFECT_LEVEL; ++i)
		m_lEffectType[i] = 0;

	InitNameValueMap();

	//
	m_timeLeaveFight = 0;
	m_timeLeaveFightStart = GetCurTickCount();
}

CMonster::~CMonster(void)
{
}

// 初始化变量名到值得影射
void CMonster::InitNameValueMap()
{
	CMoveShape::InitNameValueMap();
    tagPty st;// 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool

    st.lType=4; st.pValue=&m_lType;             m_PropertyEnumToName[M_ATTR_TYPE]="Type";				m_mapNameValue["Type"] = st;		
    st.lType=4; st.pValue=&m_lGraphicsID;		m_PropertyEnumToName[M_ATTR_GRAPHICSID]="GraphicsID";	m_mapNameValue["GraphicsID"]=st;	
    st.lType=4; st.pValue=&m_lRegionID;		    m_PropertyEnumToName[M_ATTR_REGIONID]="RegionID";		m_mapNameValue["RegionID"]=st;
	st.lType=3;	st.pfValue=&m_fPosX;		    m_PropertyEnumToName[M_ATTR_TILEX]="TileX";				m_mapNameValue["TileX"]=st;			
    st.lType=3;	st.pfValue=&m_fPosY;		    m_PropertyEnumToName[M_ATTR_TILEY]="TileY";				m_mapNameValue["TileY"]=st;	
    st.lType=3;	st.pfValue=&m_fDir;			    m_PropertyEnumToName[M_ATTR_DIR]="Dir";					m_mapNameValue["Dir"]=st;			
    st.lType=1;	st.pwValue=&m_wState;			m_PropertyEnumToName[M_ATTR_STATE]="State";				m_mapNameValue["State"]=st;			
    st.lType=1;	st.pwValue=&m_wAction;		    m_PropertyEnumToName[M_ATTR_ACTION]="Action";			m_mapNameValue["Action"]=st;		
																																	   

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*怪物属性*/
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool
    st.lType=2;	st.pdwValue = &m_BaseProperty.dwIndex;					m_PropertyEnumToName[M_ATTR_INDEX]="Index"; 			m_mapNameValue["Index"]=st;		
    st.lType=2;	st.pdwValue	= &m_BaseProperty.dwPicLevel;				m_PropertyEnumToName[M_ATTR_PICLEVEL]="PicLevel";		m_mapNameValue["PicLevel"]=st;			   
    st.lType=2;	st.pdwValue	= &m_BaseProperty.dwNameColor;				m_PropertyEnumToName[M_ATTR_NAMECOLOR]="NameColor";		m_mapNameValue["NameColor"]=st;			
																																												
    st.lType=2;	st.pdwValue	= &m_BaseProperty.dwHpBarColor;			m_PropertyEnumToName[M_ATTR_HPBARCOLOR]="HpBarColor";		m_mapNameValue["HpBarColor"]=st;		
	st.lType=2;	st.pdwValue	= &m_BaseProperty.dwSoundID;				m_PropertyEnumToName[M_ATTR_SOUNDID]="SoundID";			m_mapNameValue["SoundID"]=st;			
	st.lType=1;	st.pwValue  = &m_BaseProperty.wLevel;					m_PropertyEnumToName[M_ATTR_LEVEL]="Level";				m_mapNameValue["Level"]=st;							
	st.lType=2; st.pdwValue  = &m_BaseProperty.dwExp;					m_PropertyEnumToName[M_ATTR_EXP]="Exp";					m_mapNameValue["Exp"]=st;				
	st.lType=2; st.pdwValue  = &m_BaseProperty.dwOccuPoint;			m_PropertyEnumToName[M_ATTR_OCCUPOINT]="OccuPoint";			m_mapNameValue["OccuPoint"]=st;				
	st.lType=4;	st.plValue	= &m_BaseProperty.byCountry;				m_PropertyEnumToName[M_ATTR_COUNTRY]="byCountry";		m_mapNameValue["byCountry"]=st;			
	st.lType=4;	st.plValue	= &m_BaseProperty.byCatchable;				m_PropertyEnumToName[M_ATTR_TAMABLE]="byCatchable";		m_mapNameValue["byCatchable"]=st;			
	st.lType=2; st.pdwValue	= &m_BaseProperty.dwMaxCatchFastness;			m_PropertyEnumToName[M_ATTR_MAXTAMECOUNT]="MaxCatchFastness"; m_mapNameValue["MaxCatchFastness"]=st;		
																																												 
	st.lType=1; st.pwValue	= &m_BaseProperty.wCountryContriValue;	m_PropertyEnumToName[M_ATTR_COUNTRYCONTRIVALUE]="CountryContriValue";	m_mapNameValue["CountryContriValue"]=st;
	st.lType=1;	st.pwValue = &m_BaseProperty.wSPContriValue;			m_PropertyEnumToName[M_ATTR_SPCONTRIVALUE]="SPContriValue";				m_mapNameValue["SPContriValue"]=st;		
	st.lType=2;	st.pdwValue  = (DWORD*)&m_BaseProperty.eEType;		m_PropertyEnumToName[M_ATTR_ELETYPE_]="EleType";   						m_mapNameValue["EleType"]=st;	        
	st.lType=2;	st.pdwValue = (DWORD*)&m_BaseProperty.eNType;		m_PropertyEnumToName[M_ATTR_NPCTYPE_]="NpcType";  						m_mapNameValue["NpcType"]=st;         
	st.lType=2;	st.pdwValue = (DWORD*)&m_BaseProperty.eClass_;					m_PropertyEnumToName[M_ATTR_CLASS]="Class"; 							m_mapNameValue["Class"]=st;				
	st.lType=1;	st.pwValue = &m_BaseProperty.wRace;                   m_PropertyEnumToName[M_ATTR_RACE]="Race";     							m_mapNameValue["Race"]=st;	            
	st.lType=1; st.pwValue  = &m_BaseProperty.wMoveRandomValue;       m_PropertyEnumToName[M_ATTR_MOVERANDOMVALUE]="MoveRandomValue"; 			m_mapNameValue["MoveRandomValue"]=st;	
	st.lType=1; st.pwValue = &m_BaseProperty.wStopTime;                m_PropertyEnumToName[M_ATTR_STOPTIME]="StopTime"; 						m_mapNameValue["StopTime"]=st;	        
	st.lType=1; st.pwValue  = &m_BaseProperty.wAIHangupType;	        m_PropertyEnumToName[M_ATTR_AIHANGUPTYPE]="AIHangupType";				m_mapNameValue["AIHangupType"]=st;		
	st.lType=1; st.pwValue = &m_BaseProperty.wFigure;                 m_PropertyEnumToName[M_ATTR_FIGURE]="Figure";    							m_mapNameValue["Figure"]=st;	        
																																												 
    st.lType=4;	st.pdwValue	= &m_BaseProperty.dwHp;                   m_PropertyEnumToName[M_ATTR_HP]="Hp";										m_mapNameValue["Hp"]=st;				
    st.lType=2;	st.pdwValue	= &m_BaseProperty.dwAIType;                m_PropertyEnumToName[M_ATTR_AITYPE]="AIType";							m_mapNameValue["AIType"]=st;			
																																												 
    st.lType=3;	st.pfValue = &m_BaseProperty.fHurtProportion;         m_PropertyEnumToName[M_ATTR_HURTPROPORTION]="fHurtProportion";			m_mapNameValue["fHurtProportion"]=st;	
    st.lType=2; st.pdwValue = &m_BaseProperty.dwGuardTime;            m_PropertyEnumToName[M_ATTR_GUARDTIME]="GuardTime";						m_mapNameValue["GuardTime"]=st;			
    st.lType=3; st.pfValue = &m_BaseProperty.fEscapePoint;             m_PropertyEnumToName[M_ATTR_ESCAPEPOINT]="EscapePoint";					m_mapNameValue["EscapePoint"]=st;		
    st.lType=1; st.pwValue = &m_BaseProperty.wSkillProportion;        m_PropertyEnumToName[M_ATTR_SKILLPROPORTION]="SkillProportion";			m_mapNameValue["SkillProportion"]=st;		
    st.lType=2; st.pdwValue = &m_BaseProperty.dwSkillSpace;            m_PropertyEnumToName[M_ATTR_SKILLSPACE]="SkillSpace";					m_mapNameValue["SkillSpace"]=st;		
	st.lType=3;	st.pfValue	= &m_BaseProperty.fMoveSpeed;	;			m_PropertyEnumToName[M_ATTR_MOVESPEED]="MoveSpeed";						m_mapNameValue["MoveSpeed"]=st;		
	st.lType=3;	st.pfValue	= &m_BaseProperty.fFightMoveSpeed;			m_PropertyEnumToName[M_ATTR_FIGHTMOVESPEED]="FightMoveSpeed";			m_mapNameValue["FightMoveSpeed"]=st;		
	st.lType=1; st.pwValue = &m_BaseProperty.wCanTalk;             m_PropertyEnumToName[M_ATTR_CANTALK]="Occu";					m_mapNameValue["CanTalk"]=st;

    //for(i=0; i<MONSTER_AI_REFERRENCE_NUM; ++i)
    {
		st.lType=2;	st.pdwValue	= &m_BaseProperty.dwAIReferrence[0]; m_PropertyEnumToName[M_ATTR_AIREFERRENCE0]="AIReferrence0";				m_mapNameValue["AIReferrence0"]=st; 
		st.lType=2;	st.pdwValue	= &m_BaseProperty.dwAIReferrence[1]; m_PropertyEnumToName[M_ATTR_AIREFERRENCE1]="AIReferrence1";				m_mapNameValue["AIReferrence1"]=st; 
		st.lType=2;	st.pdwValue	= &m_BaseProperty.dwAIReferrence[2]; m_PropertyEnumToName[M_ATTR_AIREFERRENCE2]="AIReferrence2";				m_mapNameValue["AIReferrence2"]=st; 
		st.lType=2;	st.pdwValue	= &m_BaseProperty.dwAIReferrence[3]; m_PropertyEnumToName[M_ATTR_AIREFERRENCE3]="AIReferrence3";				m_mapNameValue["AIReferrence3"]=st; 
		st.lType=2;	st.pdwValue	= &m_BaseProperty.dwAIReferrence[4]; m_PropertyEnumToName[M_ATTR_AIREFERRENCE4]="AIReferrence4";				m_mapNameValue["AIReferrence4"]=st; 
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*战斗属性*/ 
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
	st.lType=4;	st.pValue	= &m_FightProperty.dwMaxHp;		m_PropertyEnumToName[M_ATTR_MAXHP]="C_MaxHp";						m_mapNameValue["C_MaxHp"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wHpRecoverSpeed;m_PropertyEnumToName[M_ATTR_HPRECOVERSPEED]="C_HpRecoverSpeed";	m_mapNameValue["C_HpRecoverSpeed"]=st;	
	st.lType=5;	st.psValue	= &m_FightProperty.wMinAtk;		m_PropertyEnumToName[M_ATTR_MINATK]="C_MinAtk";						m_mapNameValue["C_MinAtk"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wMaxAtk;		m_PropertyEnumToName[M_ATTR_MAXATK]="C_MaxAtk";						m_mapNameValue["C_MaxAtk"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wMinMAtk;		m_PropertyEnumToName[M_ATTR_MINMATK]="C_MinMAtk";				m_mapNameValue["C_MinMAtk"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wMaxMAtk;		m_PropertyEnumToName[M_ATTR_MAXMATK]="C_MaxMAtk";				m_mapNameValue["C_MaxMAtk"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wHit;			m_PropertyEnumToName[M_ATTR_HIT]="C_Hit";						m_mapNameValue["C_Hit"]=st;				
	st.lType=5;	st.psValue	= &m_FightProperty.wDef;			m_PropertyEnumToName[M_ATTR_DEF]="C_Def";						m_mapNameValue["C_Def"]=st;		
	st.lType=5;	st.psValue	= &m_FightProperty.wDef;			m_PropertyEnumToName[M_ATTR_MDEF]="C_MDef";						m_mapNameValue["C_MDef"]=st;		
	st.lType=5;	st.psValue	= &m_FightProperty.wDodge;			m_PropertyEnumToName[M_ATTR_DODGE]="C_Dodge";					m_mapNameValue["C_Dodge"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wBlock;			m_PropertyEnumToName[M_ATTR_BLOCK]="C_Block";					m_mapNameValue["C_Block"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wParry;			m_PropertyEnumToName[M_ATTR_PARRY]="C_Parry";					m_mapNameValue["C_Parry"]=st;			
																																												
	//st.lType=5;	st.psValue	= &m_FightProperty.wFarAtkDis;		m_PropertyEnumToName[M_ATTR_FARATKDIS]="Occu";					m_mapNameValue["C_FarAtkDis"]=st;		
	//st.lType=5;	st.psValue	= &m_FightProperty.wNeaAtkDis;		m_PropertyEnumToName[M_ATTR_NEAATKDIS]="Occu";					m_mapNameValue["C_NeaAtkDis"]=st;		
	//st.lType=5;	st.psValue	= &m_FightProperty.wGuardRange;	m_PropertyEnumToName[M_ATTR_GUARDRANGE]="Occu";						m_mapNameValue["C_GuardRange"]=st;		
	//st.lType=5;	st.psValue	= &m_FightProperty.wChaseRange;	m_PropertyEnumToName[M_ATTR_CHASERANGE]="Occu";						m_mapNameValue["C_ChaseRange"]=st;		
	st.lType=5;	st.psValue	= &m_FightProperty.wWillDef;		m_PropertyEnumToName[M_ATTR_WILLDEF]="Occu";					m_mapNameValue["C_WillDef"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wConstiDef;		m_PropertyEnumToName[M_ATTR_CONSTIDEF]="Occu";					m_mapNameValue["C_ConstiDef"]=st;		
	st.lType=5;	st.psValue	= &m_FightProperty.wCri;			m_PropertyEnumToName[M_ATTR_CRI]="Occu";						m_mapNameValue["C_Cri"]=st;				
	st.lType=5;	st.psValue	= &m_FightProperty.wCriDef;		m_PropertyEnumToName[M_ATTR_CRIDEF]="Occu";							m_mapNameValue["C_CriDef"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wMCriDef;		m_PropertyEnumToName[M_ATTR_MCRIDEF]="Occu";					m_mapNameValue["C_MCriDef"]=st;     


    {
		st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DFT_Bled];	m_PropertyEnumToName[M_ATTR_DEBUFFDEF_BLED]="C_DeBuffDef0";				m_mapNameValue["C_DeBuffDef0"]=st;      
		st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Dizzy];m_PropertyEnumToName[M_ATTR_DEBUFFDEF_DIZZY]="C_DeBuffDef1";				m_mapNameValue["C_DeBuffDef1"]=st;	    
		st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Lull];	m_PropertyEnumToName[M_ATTR_DEBUFFDEF_LULL]="C_DeBuffDef2";				m_mapNameValue["C_DeBuffDef2"]=st;	    
		st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Hypnosis];;	m_PropertyEnumToName[M_ATTR_DEBUFFDEF_HYPNOSIS]="C_DeBuffDef3";		m_mapNameValue["C_DeBuffDef3"]=st;  
		st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Poison];	m_PropertyEnumToName[M_ATTR_DEBUFFDEF_POSION]="C_DeBuffDef4";		m_mapNameValue["C_DeBuffDef4"]=st;	
		st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Fix];	m_PropertyEnumToName[M_ATTR_DEBUFFDEF_FIX]="C_DeBuffDef5";				m_mapNameValue["C_DeBuffDef5"]=st;	    
	}																														   

    {
		st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Fire];m_PropertyEnumToName[M_ATTR_ELEMDEF_FIRE]="C_ElemDef0";		m_mapNameValue["C_ElemDef0"]=st;	
		st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Water];m_PropertyEnumToName[M_ATTR_ELEMDEF_WATER]="C_ElemDef1";	m_mapNameValue["C_ElemDef1"]=st;	
		st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Earth];m_PropertyEnumToName[M_ATTR_ELEMDEF_EARTH]="C_ElemDef2";	m_mapNameValue["C_ElemDef2"]=st;	
		st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Wind];m_PropertyEnumToName[M_ATTR_ELEMDEF_WIND]="C_ElemDef3";		m_mapNameValue["C_ElemDef3"]=st;	
		st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Dark];m_PropertyEnumToName[M_ATTR_ELEMDEF_DARK]="C_ElemDef4";		m_mapNameValue["C_ElemDef4"]=st;	
		st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Holy];m_PropertyEnumToName[M_ATTR_ELEMDEF_HOLY]="C_ElemDef5";		m_mapNameValue["C_ElemDef5"]=st;	
    }

    {
		st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Human];m_PropertyEnumToName[M_ATTR_CLASSDAM_HUMAN]="C_ClassDam0";			m_mapNameValue["C_ClassDam0"]=st;		
		st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Element];m_PropertyEnumToName[M_ATTR_CLASSDAM_ELEMENT]="C_ClassDam1";		m_mapNameValue["C_ClassDam1"]=st;	
		st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Alien];m_PropertyEnumToName[M_ATTR_CLASSDAM_ALIEN]="C_ClassDam2";			m_mapNameValue["C_ClassDam2"]=st;		
		st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Animal];m_PropertyEnumToName[M_ATTR_CLASSDAM_ANIMAL]="C_ClassDam3";		m_mapNameValue["C_ClassDam3"]=st;		
		st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Undead];m_PropertyEnumToName[M_ATTR_CLASSDAM_UNDEAD]="C_ClassDam4";		m_mapNameValue["C_ClassDam4"]=st;		
		st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Evil];m_PropertyEnumToName[M_ATTR_CLASSDAM_EVIL]="C_ClassDam5";			m_mapNameValue["C_ClassDam5"]=st;		
		st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Hexapods];m_PropertyEnumToName[M_ATTR_CLASSDAM_HEXAPODS]="C_ClassDam6";	m_mapNameValue["C_ClassDam6"]=st;	
    }																														  

    {
		st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Fire];m_PropertyEnumToName[M_ATTR_ELEMDAM_FIRE]="C_ElemDam0";		m_mapNameValue["C_ElemDam0"]=st;  
		st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Water];m_PropertyEnumToName[M_ATTR_ELEMDAM_WATER]="C_ElemDam1";	m_mapNameValue["C_ElemDam1"]=st; 
		st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Earth];m_PropertyEnumToName[M_ATTR_ELEMDAM_EARTH]="C_ElemDam2";	m_mapNameValue["C_ElemDam2"]=st; 
		st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Wind];m_PropertyEnumToName[M_ATTR_ELEMDAM_WIND]="C_ElemDam3";		m_mapNameValue["C_ElemDam3"]=st;  
		st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Dark];m_PropertyEnumToName[M_ATTR_ELEMDAM_DARK]="C_ElemDam4";		m_mapNameValue["C_ElemDam4"]=st;  
		st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Holy];m_PropertyEnumToName[M_ATTR_ELEMDAM_HOLY]="C_ElemDam5";		m_mapNameValue["C_ElemDam5"]=st;  
																														 
    }
}


void CMonster::RunLeaveFightAI( long xDest, long yDest, long time )
{
	m_xLeaveFightDest = xDest;
	m_yLeaveFightDest = yDest;

	// 加一点延迟时间
	m_timeLeaveFight = time + 1000;

	m_bRestorColor = true;

	m_timeLeaveFightStart = GetCurTickCount();
}


// 怪物逃离战斗特效
void CMonster::AILeaveFight()
{
	DWORD dwCurTime = GetCurTickCount();

	DWORD dwLeaveTime = dwCurTime - m_timeLeaveFightStart;
	if( (long)dwLeaveTime < m_timeLeaveFight )
	{
		// 渐隐
		D3DXCOLOR		colorModel;
		D3DXCOLOR		colorSrc = 0xffffffff;
		D3DXCOLOR		colorDest = 0x00ffffff;
		D3DXColorLerp( &colorModel, &colorSrc, &colorDest, (float)dwLeaveTime / (float)m_timeLeaveFight );
		this->SetModelColor( colorModel );
	}
	else
	{
		if( m_bRestorColor )
		{
			this->SetModelColor( 0xffffffff );
			m_bRestorColor = false;
		}
	}
}


void CMonster::AI(void)
{
	// 退出战斗 AI特效
	this->AILeaveFight();

	//
	CMoveShape::AI();
	//Fade In Effect
	DWORD dwCurrentTime = timeGetTime();
	if (m_bFade)
	{
		DWORD dwTimeElapse =  dwCurrentTime - m_dwFadeStartTime;
		if ( dwTimeElapse> m_dwFadeTimeLegth)
		{
			dwTimeElapse = m_dwFadeTimeLegth;
			m_bFade = FALSE;
		}
		m_fFadeRatio = (float)dwTimeElapse / (float)m_dwFadeTimeLegth; 
		if (!m_bFadeIn)
		{
			m_fFadeRatio = 1.0f - m_fFadeRatio;
		}

	}

	if(!this->m_bAppearanceSoundPlayed)
	{
		D3DXVECTOR2 pos1(this->GetPosX(),this->GetPosY());
		D3DXVECTOR2 pos2(GetGame()->GetMainPlayer()->GetPosX(),GetGame()->GetMainPlayer()->GetPosY());
		pos1=pos1-pos2;
		float dist=D3DXVec2Length(&pos1);
		if(dist<=18)
		{
			CClientRegion *pRegion=(CClientRegion *)GetFather();
			pRegion->GetRegionCamera()->Update();
			GetGame()->GetAudioList()->Play(this, TYPE_MONSTER, 0xFFFF, Vector3(GetPosY(), GetHeight(), GetPosX()));
			this->m_bAppearanceSoundPlayed=true;
		}
		else
		{
			this->m_bAppearanceSoundPlayed=false;
		}
	}

	if( GetState() == STATE_DIED )
		return;
	extern long AREA_WIDTH,AREA_HEIGHT;
	long mx=GetGame()->GetMainPlayer()->GetTileX()/AREA_WIDTH,my=GetGame()->GetMainPlayer()->GetTileY()/AREA_HEIGHT;
	long dx=GetTileX()/AREA_WIDTH,dy=GetTileY()/AREA_HEIGHT;
	if( (abs(dx-mx) <= 1 && abs(dy-my) <= 1) )
		return;
	if( GetCurTickCount()-m_dwPetLastRefreshTime>2000 )
	{
		m_dwPetLastRefreshTime=GetCurTickCount();

	}
	if(  GetCurTickCount()-m_dwPetLastDiedTime<10000 )
	{
		return;
	}
	m_dwPetLastDiedTime=GetCurTickCount();
}

// 解码
bool CMonster::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	return true;
}

bool CMonster::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData)
{
	CMoveShape::DecodeFromDataBlock(setReadDB, bExData);

	DWORD	dwHp = setReadDB.GetDwordFromByteArray();
	DWORD	dwMaxHp = setReadDB.GetDwordFromByteArray();
	WORD	wClass = setReadDB.GetWordFromByteArray();
	WORD	wFigure = setReadDB.GetWordFromByteArray();
	DWORD	dwSounID = setReadDB.GetDwordFromByteArray();
	DWORD	dwNameColor = setReadDB.GetDwordFromByteArray();
	DWORD	dwHpBarColor = setReadDB.GetDwordFromByteArray();
	WORD	wLevel = setReadDB.GetWordFromByteArray();
	m_dwCollectID		= setReadDB.GetDwordFromByteArray();
	if (m_dwCollectID!=0)
	{
		const CCollectionLayout* CollectLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
		if (CollectLayout!=NULL)
		{
			m_strCollectionName = CollectLayout->GetCollectionName();
		}
	}
	m_bIsBeenCollect        = (setReadDB.GetByteFromByteArray() == 0)?false : true;
	m_bCanBeenCollect		= (setReadDB.GetByteFromByteArray() == 0)?false : true;

	SetMaxHp(dwMaxHp);
	SetHp(dwHp);
	SetClass((eClass)wClass);
	SetFigure(wFigure);
	SetSoundID(dwSounID);
	SetNameColor(dwNameColor);
	SetHpBarColor(dwHpBarColor);
	SetLevel(wLevel);

	//--------------07.10.23刘科----------
	// 如果该对象是monster，需要获得它的内部类型
	long lMonsterType = -1;
	WORD wIsCanTalk = 0;
	WORD wIcon = 0;
	WORD wColor = 0;
	BYTE bCountryID = 0;
	char str[512];
	memset(str,'\0',512);
	// 原始名
	m_strOrigName = setReadDB.GetStringFromByteArray( str,512);
	lMonsterType = setReadDB.GetWordFromByteArray();
   	wIsCanTalk = setReadDB.GetWordFromByteArray();
	wIcon = setReadDB.GetWordFromByteArray();
	wColor = setReadDB.GetWordFromByteArray();
	m_strTitle = setReadDB.GetStringFromByteArray( str,512);
	// 设置阻挡类型
	BYTE bBlockType = setReadDB.GetByteFromByteArray();
	SetBlockType(CRegion::eBLOCK(bBlockType));
	// 任务保护信息(类型和GUID)
	m_lCreatorType = setReadDB.GetLongFromByteArray();
	setReadDB.GetBufferFromByteArray(m_CreatorID);
	// 国家ID
	bCountryID = setReadDB.GetByteFromByteArray();
	SetCountry(bCountryID);
	// 转身速度
	m_fTurnSpeed = setReadDB.GetFloatFromByteArray();

	// BOSS等级
	WORD wBossLevel = setReadDB.GetWordFromByteArray();
	SetBossLevel(wBossLevel);

	SeteNpcType(eNpcType(lMonsterType));
	SetIsCanTalk(wIsCanTalk);
	SetMonsterIconID(wIcon);
	SetTitleColor(wColor);

	// 接压怪物的多重血条属性
	if(wBossLevel != 0 && wBossLevel != 1)
	{
		m_MultiHpBar.DecodeFromDataBlock(setReadDB);
	}

	//------------------------------------
	//DecodeChangePropertyFromDataBlock(setReadDB);

	this->m_bAppearanceSoundPlayed=false;
	return true;
}

void CMonster::SetAction(WORD l)
{
	//如果从跑步动作到其他动作，修正怪物的位置
	if( (GetAction() == CShape::ACT_RUN || GetAction() == CShape::ACT_WALK) &&
		(l != CShape::ACT_RUN && l != CShape::ACT_WALK) )
	{
		if(GetTileX() != (long)GetDestX() || GetTileY() != (long)GetDestY())
		{
			Log4c::Trace(ROOT_MODULE,"from the walk or run action to the other action,corrected the monster postion!");
			SetPosXY( GetDestX(),GetDestY() );
		}
	}

	CMoveShape::SetAction(l);
	//if (l == ACT_DIED)
	//{
	//	m_bDeadStatus = TRUE;
	//	m_dwDeadStartTime = timeGetTime();
	//	//if( m_tPetInfo.MasterID==GetGame()->GetMainPlayer()->GetExID() )
	//	//{
	//	//	GetGame()->GetMainPlayer()->GetPetIDSet()->erase( GetExID() );
	//	//}
	//}
}

// 设置位置
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
			data = (long)_GetFloatFromByteArray(pByte,pos);
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
		itProETN it = m_PropertyEnumToName.find(pro_type);
		if(it != m_PropertyEnumToName.end())
		{
			SetProperty( (*it).second.c_str(),data);
		}

		SAFEDELETE(byte_Array);
		SAFEDELETE(word_Array);
		SAFEDELETE(dword_Array);
		SAFEDELETE(flaot_Array);
		SAFEDELETE(long_Array);
		SAFEDELETE(short_Array);
		SAFEDELETE(char_Array);
		SAFEDELETE(bool_Array);
	}
}


void CMonster::DecodeChangePropertyFromDataBlock(DBReadSet& setReadDB)
{
	CMoveShape::DecodeChangePropertyFromDataBlock(setReadDB);
	WORD lNum = setReadDB.GetWordFromByteArray();
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
		pro_type = setReadDB.GetWordFromByteArray();
		data_type = setReadDB.GetByteFromByteArray();
		data_length = setReadDB.GetWordFromByteArray();		
		if (data_length<=0) continue;

		//根据数据类型获取具体的数据值
		switch(data_type)
		{
		case ATTR_DATA_BYTE:
			data = setReadDB.GetByteFromByteArray();
			break;
		case ATTR_DATA_WORD:
			data = setReadDB.GetWordFromByteArray();
			break;
		case ATTR_DATA_DWORD:
			data = setReadDB.GetDwordFromByteArray();
			break;
		case ATTR_DATA_FLOAT:
			data = (long)setReadDB.GetFloatFromByteArray();
			break;
		case ATTR_DATA_LONG:
			data = setReadDB.GetLongFromByteArray();
			break;
		case ATTR_DATA_SHORT:
			data = setReadDB.GetShortFromByteArray();
			break;
		case ATTR_DATA_BOOL:
			data = setReadDB.GetByteFromByteArray();
			break;
		case ATTR_DATA_BYTEARRAY:
			byte_Array = new BYTE[data_length];
			if (!byte_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				byte_Array[i] = setReadDB.GetByteFromByteArray();
			}
			break;
		case ATTR_DATA_WORDARRAY:
			word_Array = new WORD[data_length];
			if (!word_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				word_Array[i] = setReadDB.GetWordFromByteArray();
			}
			break;
		case ATTR_DATA_DWORDARRAY:
			dword_Array = new DWORD[data_length];
			if (!dword_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				dword_Array[i] = setReadDB.GetDwordFromByteArray();
			}
			break;
		case ATTR_DATA_FLOATARRAY:
			flaot_Array = new float[data_length];
			if (!flaot_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				flaot_Array[i] = setReadDB.GetFloatFromByteArray();
			}
			break;
		case ATTR_DATA_LONGARRAY:
			long_Array = new long[data_length];
			if (!long_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				long_Array[i] = setReadDB.GetLongFromByteArray();
			}
			break;
		case ATTR_DATA_SHORTARRAY:
			short_Array = new short[data_length];
			if (!short_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				short_Array[i] = setReadDB.GetShortFromByteArray();
			}
			break;
		case ATTR_DATA_CHARARRAY:
			char_Array = new char[data_length];
			if (!char_Array) return;
			setReadDB.GetStringFromByteArray( char_Array,data_length);
			break;
		case ATTR_DATA_BOOLARRAY:
			bool_Array = new bool[data_length];
			if (!bool_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				data = setReadDB.GetByteFromByteArray();
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
		itProETN it = m_PropertyEnumToName.find(pro_type);
		if(it != m_PropertyEnumToName.end())
		{
			SetProperty( (*it).second.c_str(),data);
		}

		SAFEDELETE(byte_Array);
		SAFEDELETE(word_Array);
		SAFEDELETE(dword_Array);
		SAFEDELETE(flaot_Array);
		SAFEDELETE(long_Array);
		SAFEDELETE(short_Array);
		SAFEDELETE(char_Array);
		SAFEDELETE(bool_Array);
	}
}

void CMonster::UpdateProperty(CMessage* pMsg)
{
	DBReadSet setReadDB;
	pMsg->GetDBReadSet(setReadDB);
	CMonster::DecodeChangePropertyFromDataBlock(setReadDB);
}


// -------------NPC的相关任务状态标识（刘科07.10.11添加）--------------
// 设置该NPC对象任务相关的指定状态(lTraceIndex==-1标识不是因为追踪引起的)
void CMonster::SetStateByQuest(int iState, long lPicOK, long lPicNO, bool bIsAdd, long lQuestID, long lStep, long lTraceIndex)
{
	if(lPicOK <= 0 && lPicNO <= 0)
		return;
	if(iState >= STATE_STEP && iState < STATE_NUM)
	{
		bool ret = false;
		tagQuestInfo quest;
		// 找到指定状态下任务列表中的具体任务
		for(size_t i=0; i<m_State[iState].vec_QuestInfo.size(); ++i)
		{
			quest = m_State[iState].vec_QuestInfo[i];
			// 根据加入的状态来添加NPC的任务状态列表
			// 只和任务相关的状态
			if(iState == STATE_QUEST)
			{
				// 找到该任务后，设置NPC对应的状态，然后返回
				if(lQuestID == quest.lQuestID)
				{
					if(quest.bIsOK == bIsAdd)
						return;
					quest.bIsOK = bIsAdd;
					quest.lPicOK = lPicOK;
					quest.lPicNO = lPicNO;
					m_State[iState].vec_QuestInfo[i] = quest;
					ret = true;
					break;
				}
			}
			// 和该任务某一步骤相关的状态
			else if(iState == STATE_STEP)
			{
				// 找到该任务的指定步骤后，设置NPC对应的状态，然后返回
				if(lStep != -1 && lQuestID == quest.lQuestID && lStep == quest.lStepIndex)
				{
					if(quest.bIsOK == bIsAdd)
						return;
					quest.bIsOK = bIsAdd;
					quest.lPicOK = lPicOK;
					quest.lPicNO = lPicNO;
					m_State[iState].vec_QuestInfo[i] = quest;
					ret = true;
					break;
				}
			}
			// 和该任务某一步骤的某一个追踪相关的状态
			else if(iState == STATE_TRACE)
			{
				// 找到该任务的指定步骤的指定追踪后，设置NPC对应的状态，然后返回
				if(lTraceIndex != -1 && lStep != -1 && lQuestID == quest.lQuestID 
					&& lStep == quest.lStepIndex && lTraceIndex == quest.lTraceIndex)
				{
					if(quest.bIsOK == bIsAdd)
						return;
					quest.bIsOK = bIsAdd;
					quest.lPicOK = lPicOK;
					quest.lPicNO = lPicNO;
					m_State[iState].vec_QuestInfo[i] = quest;
					ret = true;
					break;
				}
			}
		}
		// 如果没有在该NPC的任务列表中找到需要设置的任务，那么构造一个任务信息，添加到他的任务信息列表中去
		if(!ret)
		{
			quest.bIsOK = bIsAdd;
			quest.lPicOK = lPicOK;
			quest.lPicNO = lPicNO;
			quest.lQuestID = lQuestID;
			quest.lStepIndex = lStep;
			quest.lTraceIndex = lTraceIndex;
			m_State[iState].vec_QuestInfo.push_back(quest);
		}
		// 获得对应的优先级
		long lOkindex = GetGame()->GetMonsterQuestEffectLevel(quest.lPicOK);
		long lNoindex = GetGame()->GetMonsterQuestEffectLevel(quest.lPicNO);
		if(quest.bIsOK)
		{
			// 固定的优先级
			if(lOkindex >= 0 && lOkindex < EFFECT_LEVEL)
			{
				++m_lEffectType[lOkindex];
				if(ret && m_lEffectType[lNoindex] > 0)
					--m_lEffectType[lNoindex];
			}
			else if(quest.lPicOK > 0)
			{
				m_lsOtherQuestEffectID.push_back(quest.lPicOK);
				if(ret)
					DelOtherQuestEffectID(quest.lPicNO);
				m_lsOtherQuestEffectID.sort();
			}
		}
		else
		{
			// 固定的优先级
			if(lNoindex >= 0 && lNoindex < EFFECT_LEVEL)
			{
				++m_lEffectType[lNoindex];
				if(ret && m_lEffectType[lOkindex] > 0)
					--m_lEffectType[lOkindex];
			}
			else if(quest.lPicNO > 0)
			{
				m_lsOtherQuestEffectID.push_back(quest.lPicNO);
				if(ret)
					DelOtherQuestEffectID(quest.lPicOK);
				m_lsOtherQuestEffectID.sort();
			}
		}
	}
}

// 从任务信息表中移出指定任务ID的任务信息
void CMonster::RemoveStateByQuest(int iState, long lQuestID, long lStep, long lTraceIndex)
{
	bool ret = false;
	bool bIsOK = false;
	long lOk = 0;
	long lNo = 0;
	vector<tagQuestInfo>::iterator it;
	vector<tagQuestInfo>::iterator itend;
	if(iState >= STATE_STEP && iState < STATE_NUM)
	{
		it = m_State[iState].vec_QuestInfo.begin();
		itend = m_State[iState].vec_QuestInfo.end();
		// 找到指定状态下任务列表中的具体任务
		for(; it!=itend; ++it)
		{
			// 根据加入的状态来添加NPC的任务状态列表
			// 只和任务相关的状态
			if(iState == STATE_QUEST)
			{
				// 找到该任务后，设置NPC对应的状态，然后返回
				if(lQuestID == (*it).lQuestID)
				{
					ret = true;
					bIsOK = (*it).bIsOK;
					lOk = (*it).lPicOK;
					lNo = (*it).lPicNO;
					m_State[iState].vec_QuestInfo.erase(it);
					break;
				}
			}
			// 和该任务某一步骤相关的状态
			else if(iState == STATE_STEP)
			{
				// 找到该任务的指定步骤后，设置NPC对应的状态，然后返回
				if(lStep != -1 && lQuestID == (*it).lQuestID && lStep == (*it).lStepIndex)
				{
					ret = true;
					bIsOK = (*it).bIsOK;
					lOk = (*it).lPicOK;
					lNo = (*it).lPicNO;
					m_State[iState].vec_QuestInfo.erase(it);
					break;
				}
			}
			// 和该任务某一步骤的某一个追踪相关的状态
			else if(iState == STATE_TRACE)
			{
				// 找到该任务的指定步骤的指定追踪后，设置NPC对应的状态，然后返回
				if(lTraceIndex != -1 && lStep != -1 && lQuestID == (*it).lQuestID 
					&& lStep == (*it).lStepIndex && lTraceIndex == (*it).lTraceIndex)
				{
					ret = true;
					bIsOK = (*it).bIsOK;
					lOk = (*it).lPicOK;
					lNo = (*it).lPicNO;
					m_State[iState].vec_QuestInfo.erase(it);
					break;
				}
			}
		}
		// 有删除的数据，将这些特效从本怪物身上删除
		if(ret)
		{
			if(lOk)
				DelEffect(lOk);
			if(lNo)
				DelEffect(lNo);
			// 获得对应的优先级
			long lOkindex = GetGame()->GetMonsterQuestEffectLevel(lOk);
			long lNoindex = GetGame()->GetMonsterQuestEffectLevel(lNo);
			if(bIsOK)
			{
				if(lOkindex >= 0  && lOkindex < EFFECT_LEVEL)
				{
					if(m_lEffectType[lOkindex] > 0)
						--m_lEffectType[lOkindex];
				}
				else
					DelOtherQuestEffectID(lOk);
			}
			else
			{
				if(lNoindex >= 0  && lNoindex < EFFECT_LEVEL)
				{
					if(m_lEffectType[lNoindex] > 0)
						--m_lEffectType[lNoindex];
				}
				else
					DelOtherQuestEffectID(lNo);
			}
		}
	}
}

// 获得当前NPC对象需要显示的任务相关特效
long CMonster::GetStateByQuest()
{
	long index = -1;
	for(long i = 0; i < EFFECT_LEVEL; ++i)
	{
		if(m_lEffectType[i] > 0)
		{
			index = GetGame()->GetMonsterQuestEffectID(i);
			return index;
		}
	}
	if(!m_lsOtherQuestEffectID.empty())
		index = m_lsOtherQuestEffectID.front();
	return index;
}

// 删除指定ID的其它任务特效
void CMonster::DelOtherQuestEffectID(long lID)
{
	list<long>::iterator it = m_lsOtherQuestEffectID.begin();
	for (; it!=m_lsOtherQuestEffectID.end(); ++it)
	{
		if((*it) == lID)
		{
			m_lsOtherQuestEffectID.erase(it);
			return;
		}
	}
}

// 获得指定状态应该绘制的标志（完成/未完成，或者可接，将接）
long CMonster::GetPicIndexByNpc(int iState)
{
	//if(iState >= STATE_STEP && iState < STATE_NUM)
	//{
	//	for(int i=0; i<m_State[iState].vec_QuestInfo.size(); ++i)
	//	{
	//		// 只要有一个完成或可接，返回
	//		if(m_State[iState].vec_QuestInfo[i].bIsOK)
	//			return m_State[iState].vec_QuestInfo[i].lPicOK;
	//	}
	//	// 返回未完成或不可接图标
	//	return m_State[iState].vec_QuestInfo[0].lPicNO;
	//}
	return -1;
}



/*
* 功能: 设置当前任务相关的特效
* 摘要: -
* 参数: 略
* 返回值: -
* 作者: 刘科
* 创建日期: -
* 修改日志:
*	2008.12.10 - lpf
*		添加任务相关特效时,传入了拾取盒子高度修正值;
*	2008.12.23 - lpf
*		增加了DisplayModel是否为空的判断，如果为空直接返回;
*/
void CMonster::SetCurQuestEffect(long lEffectID)
{
	// 身上没有此特效，装载
	if(FindEffect(lEffectID) == NULL)
	{
		// 删除之前的任务特效
		if(m_lCurQuestEffectID > 0)
			DelEffect(m_lCurQuestEffectID);

		// 加载新的任务特效
		m_lCurQuestEffectID = lEffectID;

		if (GetDisplayModel() == NULL)
			return;
		else if(GetDisplayModel()->GetPickBox() == NULL)
			return;


		AddEffectLocator(m_lCurQuestEffectID,this,CShape::LOCATOR_FOOTCENTER,true, 0.0f, 0, true, GetPickBoxHeight() + 0.5f);
	}
}



/*
* 功能: 显示Monster称号
* 摘要: 添加，显示Monster称号
* 参数: 略
* 返回值: -
* 作者: 刘科
* 创建日期: 2007.12.06
* 修改日志:
*	2008.08.26 - lpf
*		修改了变换高度取的参考点坐标高度值，可与玩家名字在缩放镜头时保持同样的位置;
*		取消了超出屏幕界限的处理;
*	2008.10.07 - lpf
*		怪物称号(包括NPC)称号显示的高度由外部传进来;
*/
void CMonster::DisplayMasterTitle(const char* str, long lIconID, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, D3DCOLOR bkBarColor,bool bShoWName)
{
	D3DXVECTOR3 pos(GetPosY(), GetPickBoxHeight() + GetHeight(), GetPosX());
	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);
	pos2.y = fPos;

	// 显示称号图标
	// 有任务相关的状态，则不显示怪物的称谓图标
	if(lIconID != 0 && GetStateByQuest() == -1)
	{
		if(!bShoWName)
			pos2.y += GetGame()->GetFontHeight(-1);
		CPicList::tagBitmap * pIconBitmap = GetGame()->GetPicList()->Load(CPicList::PT_MONSTER_ICON, lIconID);
		if(pIconBitmap == NULL || pIconBitmap->pBitmap == NULL)
			return;
		CBitmapX * pBitmap = pIconBitmap->pBitmap;
		pBitmap->Draw((long)pos2.x - (pBitmap->GetWidth() >> 1), 
			(int)(pos2.y - (GetGame()->GetFontHeight(-1) * 3.5f)));
	}
	if(bShoWName && str != NULL && *str != '0')
	{
		int len = lstrlen(str);
		// 称号的正中间位置
		long x = (long)pos2.x - GetGame()->GetFontWidth(str) / 2;

		// 称号显示的高度
		long y = (long)pos2.y - GetGame()->GetFontHeight(-1) - 4;

		//if (x<0)
		//	x = 0;
		//if (y<0)
		//	y = 0;
		//if (x > SCREEN_WIDTH-len * GetGame()->GetFontWidth(-1))
		//	x = SCREEN_WIDTH-len * GetGame()->GetFontWidth(-1);
		//if (y > SCREEN_HEIGHT-63-GetGame()->GetFontHeight(-1))
		//	y = SCREEN_HEIGHT-63-GetGame()->GetFontHeight(-1);

		// 文字的起始显示位置
		long lPosX = x;
		// 显示称号文字
		if (bkBarColor)
			GetGame()->DrawSolidQuadRect(lPosX-2, y-2, lPosX+ GetGame()->GetFontWidth(str) +2, y+GetGame()->GetFontHeight(-1)+2, bkBarColor);

		//GetGame()->OutPutText(lPosX, y-1, str, len, bkcolor);
		//GetGame()->OutPutText(lPosX-1, y, str, len, bkcolor);
		//GetGame()->OutPutText(lPosX, y+1, str, len, bkcolor);
		GetGame()->OutPutText(lPosX+1, y+1, str, len, bkcolor);
		GetGame()->OutPutText(lPosX, y, str, len, color);
	}
}
//---------------------------------------------------------------------



//当属性值改变以后出发
bool  CMonster::OnPropertyChanged(const char* attrName)
{
	CMoveShape::OnPropertyChanged(attrName);
	return true;
}

//得到属性值的字符串表示
const char* CMonster::GetPropertyStringValue(const char* pszPropertyName)
{
	return CMoveShape::GetPropertyStringValue(pszPropertyName);
}