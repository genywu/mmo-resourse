////////////////////////////////////////////////////////////////////////////
//build.cpp///////////////////////////////////////////////////////////////////
//建筑类，此物体有生命和防御，无AI
//Author:Joe
//Create Time:2004.721
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Build.h"
#include "../../EngineExtend/DisplayModel.h"
#include "../ClientApp/Other/AudioList.h"
#include "../../../Public/Common/BaseDef.h"
#include "../../../Public/Common/Public.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBuild::CBuild(void)
{
	memset(&m_Property,0,sizeof(tagProperty));
	m_lType=TYPE_BUILD;
	m_Switch = 0;

	InitNameValueMap();
}

CBuild::~CBuild(void)
{
}

// 初始化变量名到值得影射
void CBuild::InitNameValueMap()
{
	CMoveShape::InitNameValueMap();
    tagPty st;// 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool

    st.lType=4; st.pValue=&m_lType;             m_PropertyEnumToName[B_ATTR_TYPE]="Type";				m_mapNameValue["Type"] = st;		
    st.lType=4; st.pValue=&m_lGraphicsID;		m_PropertyEnumToName[B_ATTR_GRAPHICSID]="GraphicsID";	m_mapNameValue["GraphicsID"]=st;	
    st.lType=4; st.pValue=&m_lRegionID;		    m_PropertyEnumToName[B_ATTR_REGIONID]="RegionID";		m_mapNameValue["RegionID"]=st;
	st.lType=3;	st.pfValue=&m_fPosX;		    m_PropertyEnumToName[B_ATTR_TILEX]="TileX";				m_mapNameValue["TileX"]=st;			
    st.lType=3;	st.pfValue=&m_fPosY;		    m_PropertyEnumToName[B_ATTR_TILEY]="TileY";				m_mapNameValue["TileY"]=st;	
    st.lType=3;	st.pfValue=&m_fDir;			    m_PropertyEnumToName[B_ATTR_DIR]="Dir";					m_mapNameValue["Dir"]=st;			
    st.lType=1;	st.pwValue=&m_wState;			m_PropertyEnumToName[B_ATTR_STATE]="State";				m_mapNameValue["State"]=st;			
    st.lType=1;	st.pwValue=&m_wAction;		    m_PropertyEnumToName[B_ATTR_ACTION]="Action";			m_mapNameValue["Action"]=st;		
																																	   

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*城门属性*/
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool
	st.lType=4;	st.pValue = &m_Property.lIndex;				 m_PropertyEnumToName[B_ATTR_INDEX]="Index"; 			        m_mapNameValue["Index"]=st;				   																																																 	        																																											 
	st.lType=4;	st.pValue = &m_Property.lHP;                 m_PropertyEnumToName[B_ATTR_HP]="Hp";						    m_mapNameValue["Hp"]=st;
																																			 
    st.lType=4;	st.pValue = &m_Property.lMaxHP;		         m_PropertyEnumToName[B_ATTR_MAXHP]="MAXHP";				    m_mapNameValue["MAXHP"]=st;
	st.lType=4;	st.pValue = &m_Property.lDef;		         m_PropertyEnumToName[B_ATTR_DEF]="C_DEF";				        m_mapNameValue["C_DEF"]=st;
    st.lType=4;	st.pValue = &m_Property.lMagicDef;		     m_PropertyEnumToName[B_ATTR_MDEF]="C_MAGICDEF";			    m_mapNameValue["C_MAGICDEF"]=st;
	st.lType=4;	st.pValue = &m_Property.lNameColor;		     m_PropertyEnumToName[B_ATTR_NAMECOLOR]="NAMECOLOR";			m_mapNameValue["NAMECOLOR"]=st;
	st.lType=1;	st.pwValue = &m_Property.wLevel;		     m_PropertyEnumToName[B_ATTR_LEVEL]="LEVEL";			        m_mapNameValue["LEVEL"]=st;
	st.lType=4; st.pValue = &m_Switch;                       m_PropertyEnumToName[B_ATTR_STATE]="SWITCH";			        m_mapNameValue["SWITCH"]=st;

}


// 解码
bool CBuild::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	CMoveShape::DecordFromByteArray(pByte, pos, bExData);
	_GetBufferFromByteArray(pByte, pos, &m_Property, (long)sizeof(m_Property));

	char strVal[256];
	_GetStringFromByteArray(pByte, pos, strVal);
	strHitSound=strVal;
	_GetStringFromByteArray(pByte, pos, strVal);
	strDestorySound=strVal;

	_GetStringFromByteArray(pByte, pos, strVal);//开门音效
    strOpenSound = strVal;
	_GetStringFromByteArray(pByte, pos, strVal);//关门音效
	strCloseSound = strVal;

    
	return true;
}

bool CBuild::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData)
{
	CMoveShape::DecodeFromDataBlock(setReadDB, bExData);
	setReadDB.GetBufferFromByteArray( &m_Property, (long)sizeof(m_Property));

	char strVal[256];
	setReadDB.GetStringFromByteArray( strVal,256);
	strHitSound=strVal;
	setReadDB.GetStringFromByteArray( strVal,256);
	strDestorySound=strVal;

	setReadDB.GetStringFromByteArray( strVal,256);//开门音效
    strOpenSound = strVal;
	setReadDB.GetStringFromByteArray( strVal,256);//关门音效
	strCloseSound = strVal;
	return true;
}

void CBuild::SetAction(WORD l)
{

	if( m_wAction != l )
	{
		m_wAction = l;
		OnActionChange();
	}
}

void CBuild::OnActionChange()
{
	static char str[32];

	AnimInfo::tagActionInfo *pActionInfo = m_pAnimInfo->GetActionInfo();
	//---------------------------------------------
	pActionInfo->dwCurActionStartTime = GetCurTickCount();
	pActionInfo->dwCurActionLoopCount = 0;

	if (m_wAction == CShape::ACT_STAND || m_wAction == CShape::ACT_INTONATE
		|| m_wAction == CShape::ACT_WALK || m_wAction == CShape::ACT_RUN 
		|| m_wAction == CShape::ACT_MEGRIM || m_wAction == CShape::ACT_ALREADYDIED )
	{
		pActionInfo->bCurActionLooped = TRUE;
	}
	else
	{
		pActionInfo->bCurActionLooped = FALSE;
	}

	//如果是打开动作且当前选择的对象是城门，则清除选择
	if(m_wAction == ACT_OPEN||m_wAction == ACT_ALREADYOPEN)
	{
		//if(GetGame()->GetGameControl()->GetSelectedShape()==this)
		//{
		//	GetGame()->GetGameControl()->SetSelectedShape(NULL);
		//	GetGame()->GetCGuiEx()->GetCObjectPageEx()->Close();
		//}
	}

	// 如果死亡，不阻挡
	if(  m_wAction == CShape::ACT_DIED ||
		m_wAction == CShape::ACT_ALREADYDIED ||m_wAction == CShape::ACT_ALREADYOPEN ||
		m_wAction == CShape::ACT_OPEN )

	{
		SetBlock(GetTileX(), GetTileY(), CRegion::BLOCK_NO);
	}
	else
	{
		SetBlock(GetTileX(), GetTileY(), CRegion::BLOCK_SHAPE);
	}

	// 进入非站立动作
	if (m_wAction!=ACT_STAND)
	{
		m_lIdleCount = 0;
	}

	static DWORD dwSTND = MAKEFOURCC('C','L','O','1'); 
	static DWORD dwDIED = MAKEFOURCC('D','I','E','D');
	static DWORD dwOPEN = MAKEFOURCC('O','P','E','N');
	static DWORD dwHURT = MAKEFOURCC('H','U','R','T');
	static DWORD dwALREADYDIED = MAKEFOURCC('D','I','E','1');
	static DWORD dwALREADYOPEN = MAKEFOURCC('O','P','E','1');
	static DWORD dwCLOSE = MAKEFOURCC('C','L','O','S'); 
	switch (m_wAction)
	{
	case CShape::ACT_STAND:
		pActionInfo->dwCurAction = dwSTND;
		SetState(STATE_FIGHT);
		break;

	case CShape::ACT_DIED:
		pActionInfo->dwCurAction= dwDIED;
		SetState(STATE_DIED);
		break;

	case CShape::ACT_BEATTACK:
		pActionInfo->dwCurAction= dwHURT;
        SetState(STATE_FIGHT);
		break;

	case CShape::ACT_OPEN:
		pActionInfo->dwCurAction = dwOPEN;
        SetState(STATE_PEACE);
		break;

	case CShape::ACT_ALREADYDIED:
		pActionInfo->dwCurAction= dwALREADYDIED;
		SetState(STATE_DIED);
		break;

	case CShape::ACT_ALREADYOPEN:
		pActionInfo->dwCurAction= dwALREADYOPEN;
		SetState(STATE_PEACE);
		break;

	case CShape::ACT_IDLE:
		pActionInfo->dwCurAction= dwCLOSE;
		SetState(STATE_PEACE);
		break;	
	}
}

void CBuild::AI(void)
{
	CShape::AI();

	switch( GetAction() )
	{
	case ACT_STAND:
	case ACT_IDLE:
		if( AI_Stand() == RET_AI_BREAK ) return;
		break;
	case ACT_BEATTACK:
		if( AI_BeAttack() == RET_AI_BREAK ) return;
		break;

	case ACT_DIED:
		if( AI_Died() == RET_AI_BREAK ) return;
		break;

	case ACT_OPEN:
	case ACT_ALREADYOPEN:
		if(AI_Open() ==  RET_AI_BREAK) return;
		break;
	}

	// 跳血数字的颜色和高度控制
	for(list<tagBloodText>::iterator it=m_listBloodText.begin(); it!=m_listBloodText.end();)
	{
		tagBloodText *pst = &(*it);

		pst->lAlpha-=4;
		pst->fPos +=.02f;
		if (pst->lAlpha <= 0)
		{
			m_listBloodText.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

bool CBuild::Display(void)
{
	CMoveShape::Display();
	return true;
}
// 建筑静止AI
long CBuild::AI_Stand()
{
	if( OnAttackEvent() == RET_AI_BREAK )
	{
		return RET_AI_BREAK;
	}
	return RET_AI_CONTINUE;
}

// 被攻擊AI
long CBuild::AI_BeAttack()
{
	if( OnAttackEvent() == RET_AI_BREAK )
	{
	    return RET_AI_BREAK;
	}
	return RET_AI_CONTINUE;
}

// 死亡AI
long CBuild::AI_Died()
{
	return RET_AI_CONTINUE;
}

// 开门AI
long CBuild::AI_Open()
{
	if( OnAttackEvent() == RET_AI_BREAK )
	{
	   return RET_AI_BREAK;
	}
	return RET_AI_CONTINUE;
}

// 根据方向得到体形
BYTE CBuild::GetFigure(eDIR dir)
{
    if(GetDir()==DIR_DOWN||GetDir()==DIR_UP)
    {
        if(dir==DIR_UP || dir== DIR_DOWN)
        {
            return (BYTE)(m_Property.lHeightInc/2);
        }
        else if(dir==DIR_LEFT || dir==DIR_RIGHT)
        {
            return (BYTE)(m_Property.lWidthInc/2);
        }
    }
    else if(GetDir()==DIR_RIGHT||GetDir()==DIR_LEFT)
    {
        if(dir==DIR_UP || dir== DIR_DOWN)
        {
            return (BYTE)(m_Property.lWidthInc/2);
        }
        else if(dir==DIR_LEFT || dir==DIR_RIGHT)
        {
            return (BYTE)(m_Property.lHeightInc/2);
        }
    }
    return 0;
}

DWORD  CBuild::SetValue(const char* strName, DWORD dwValue)
{
	if(strcmp(strName,"HP") == 0)
	{
        SetHp(dwValue);
		//GetGame()->GetCGuiEx()->GetCObjectPageEx()->UpdataObjInfo();
	}
    else if(strcmp(strName,"MAXHP") == 0)
	{
        SetMaxHp(dwValue);
		if(dwValue<GetHp())
		{
           SetHp(dwValue);
		}

		//GetGame()->GetCGuiEx()->GetCObjectPageEx()->UpdataObjInfo();
	}
    else if(strcmp(strName,"SWITCH") == 0)
	{
		//SetSwitch((bool)dwValue);
	}
    else if(strcmp(strName,"REFRESH") == 0)
	{
		SetHp(dwValue);
		//GetGame()->GetCGuiEx()->GetCObjectPageEx()->UpdataObjInfo();
	}
	return 1;
}

void CBuild::UpdateProperty(CMessage* pMsg)
{
	DBReadSet setReadDB;
	pMsg->GetDBReadSet(setReadDB);
	CBuild::DecodeChangePropertyFromDataBlock(setReadDB);
}

void  CBuild::DecordChangeProperty(BYTE* pByte, long& pos)
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
		if(data==CShape::ACT_DIED||data==CShape::STATE_DIED)
			return;

		//设置相应属性值
		itProETN it = m_PropertyEnumToName.find(pro_type);
		if(it != m_PropertyEnumToName.end())
		{			
			if(strcmp((*it).second.c_str(),"Action")==0&&(long)GetAction()!=data)
			{
               SetAction((WORD)data);
			}

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


void  CBuild::DecodeChangePropertyFromDataBlock(DBReadSet& setReadDB)
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
		if(data==CShape::ACT_DIED||data==CShape::STATE_DIED)
			return;

		//设置相应属性值
		itProETN it = m_PropertyEnumToName.find(pro_type);
		if(it != m_PropertyEnumToName.end())
		{			
			if(strcmp((*it).second.c_str(),"Action")==0&&(long)GetAction()!=data)
			{
               SetAction((WORD)data);
			}

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