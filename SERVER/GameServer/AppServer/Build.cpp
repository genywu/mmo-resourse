////////////////////////////////////////////////////////////////////////////
//build.cpp///////////////////////////////////////////////////////////////////
//建筑类，此物体有生命和防御，无AI
//Author:Joe
//Create Time:2004.721
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\build.h"
#include "ServerRegion.h"
#include "script/Script.h"
#include "Player.h"
#include "Monster.h"


class CPlayer;
CBuild::CBuild(void)
{
	InitNameValueMap();
	m_lIndex=0;	
	m_fPosX=0.0;
	m_fPosY=0.0;					
	m_lMaxHP=0;				
	m_lHP=0;			
	m_lDef=0;			
	m_lMagicDef=0;				
	m_lNameColor=0;			
	m_lLenth=0;			
	m_lWidth=0;		
	m_lLevel=0;		
	m_strAttack="0";		
	m_strDestory="0";		
	m_strRefresh="0";		
	//##添加基本的战斗技能
	CMoveShape::InitSkills();
}

CBuild::~CBuild(void)
{
}

// 添加到CByteArray
bool CBuild::CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData)
{
	CMoveShape::CodeToDataBlock_ForClient( setWriteDB, bExData );
	setWriteDB.AddToByteArray(m_lIndex);
	setWriteDB.AddToByteArray(m_lMaxHP);
	setWriteDB.AddToByteArray(m_lHP);
	setWriteDB.AddToByteArray(m_lDef);
	setWriteDB.AddToByteArray(m_lMagicDef);
	setWriteDB.AddToByteArray(m_lNameColor);
	setWriteDB.AddToByteArray(m_lLenth);
	setWriteDB.AddToByteArray(m_lWidth);
	setWriteDB.AddToByteArray(m_lLevel);
	setWriteDB.AddToByteArray(m_strHitSound.c_str());
	setWriteDB.AddToByteArray(m_strDestorySound.c_str());
	return true;
}

// 解码
bool CBuild::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData )
{
	CMoveShape::DecodeFromDataBlock(setReadDB, bExData);
	m_lIndex=setReadDB.GetLongFromByteArray();
	m_lMaxHP=setReadDB.GetLongFromByteArray();
	m_lHP=setReadDB.GetLongFromByteArray();
	m_lDef=setReadDB.GetLongFromByteArray();
	m_lMagicDef=setReadDB.GetLongFromByteArray();
	m_lNameColor=setReadDB.GetLongFromByteArray();
	m_lLenth=setReadDB.GetLongFromByteArray();
	m_lWidth=setReadDB.GetLongFromByteArray();
	m_lLevel=setReadDB.GetLongFromByteArray();
	char str[1024];
	setReadDB.GetStringFromByteArray(str,1024);
	m_strHitSound=str;
	setReadDB.GetStringFromByteArray(str,1024);
	m_strDestorySound=str;
	return true;
}

void CBuild::SetAction(WORD l)
{
	//if( m_wAction != l )
	m_lPos = 0;
	m_wAction = l;

	// 如果死亡，不阻挡
	switch(m_wAction)
	{
	case ACT_IDLE:
	case ACT_STAND:
		{
			SetBlock(GetTileX(), GetTileY(), CRegion::BLOCK_NO);
			SetBlock(GetTileX(), GetTileY(), GetBlockType());
		}
		break;
	case ACT_OPEN:
	case ACT_DIED:
		{
			SetBlock(GetTileX(), GetTileY(), GetBlockType());
			SetBlock(GetTileX(), GetTileY(), CRegion::BLOCK_NO);
		}
		break;
	default:
		break;
	}
}





// 被击毁时处理
void CBuild::OnDied()
{
	CServerRegion* pRegion = (CServerRegion*)GetFather();
	if(pRegion==NULL) return;

	// 最后杀死自己的对象
	if(GetKilledMeType()==TYPE_PLAYER)
	{
		CPlayer* pKilledMe = (CPlayer*)pRegion->FindChildObject(TYPE_PLAYER, GetKilledMeID());

		if(pKilledMe)
		{
			CMessage msg( MSG_S2C_SHAPE_ATK_DIED );
			msg.Add( GetType());
			msg.Add(GetExID());
			msg.Add( GetType() );
			msg.Add( GetExID() );
			msg.SendToAround(this);
			// 执行脚本
			
		}
	}
	else
	{
		CMessage msg( MSG_S2C_SHAPE_ATK_DIED );
		msg.Add( GetType());
		msg.Add(GetExID());
		msg.Add( GetType() );
		msg.Add( GetExID() );
		msg.SendToAround(this);
	}
}



VOID CBuild::OnBeenMurdered( LONG lMurdererType, const CGUID& MurdererGuid )
{
	CServerRegion* pRegion = (CServerRegion*)GetFather();
	if(pRegion==NULL) return;
}

// 可否攻击
bool CBuild::IsAttackAble(CMoveShape* pAttackShape)
{
	if(GetState() == STATE_DIED || IsDied()) return false;

	CServerRegion* pRegion = (CServerRegion*)GetFather();
	if(pRegion)
		return true;
	return false;
}

// 根据方向得到体形
WORD CBuild::GetFigure(eDIR dir)
{
	if(GetDir()==DIR_DOWN||GetDir()==DIR_UP)
	{
		if(dir==DIR_UP || dir== DIR_DOWN)
		{
			return m_lWidth/2;
		}
		else if(dir==DIR_LEFT || dir==DIR_RIGHT)
		{
			return m_lLenth/2;
		}
	}
	else if(GetDir()==DIR_RIGHT||GetDir()==DIR_LEFT)
	{
		if(dir==DIR_UP || dir== DIR_DOWN)
		{
			return m_lLenth/2;
		}
		else if(dir==DIR_LEFT || dir==DIR_RIGHT)
		{
			return m_lWidth/2;
		}
	}
	return 0;
}

long CBuild::GetAttackerDir(long lAttackerX,long lAttackerY,long ldx,long ldy)
{
	long lDistance = 10000000;
	long lDir = 0;
	long lTileX = GetTileX();
	long lTileY = GetTileY();
	GetBeAttackedPoint(lAttackerY,lAttackerY,lTileX,lTileY);

	lDir = GetDestDir(lAttackerY,lAttackerY,lTileX,lTileY);
	return lDir;
}

void	CBuild::GetBeAttackedPoint(long lAttackerX,long lAttackerY,long& lTileX,long& lTileY)
{
	long lDistance = 10000000;
	long lDir = 0;
	lTileX = GetTileX();
	lTileY = GetTileY();
	for (long i=GetTileX()-GetFigure(DIR_RIGHT); i<=GetTileX()+GetFigure(DIR_RIGHT); i++)
	{
		for (long j=GetTileY()-GetFigure(DIR_UP); j<=GetTileY()+GetFigure(DIR_UP); j++)
		{
			if(lDistance>Distance(i,j,lAttackerX,lAttackerY))
			{
				lDistance=Distance(i,j,lAttackerX,lAttackerY);
				lDir = GetDestDir(lAttackerX,lAttackerY,i,j);
				lTileX = i;lTileY = j;
			}
			else if( lDistance==Distance(i,j,lAttackerX,lAttackerY) &&
				(lDir%2==1) && (GetDestDir(lAttackerX,lAttackerY,i,j)%2==0) )
			{
				lDistance=Distance(i,j,lAttackerX,lAttackerY);
				lDir = GetDestDir(lAttackerX,lAttackerY,i,j);
				lTileX = i;lTileY = j;
			}
		}
	}
	
}

void CBuild::SetHP(long lhp)
{
	if (0==lhp)
	{
		m_lHP=lhp;
		SetAttribute("Action",ACT_DIED);
		SetAttribute("State",STATE_DIED);
		SetAttribute("Hp",0);
		OnDied();
	}
	else
	{
		SetAttribute("Hp",lhp<m_lMaxHP?lhp:m_lMaxHP);
	}
}

void CBuild::SetPosXY(float fX, float fY)
{
	long lOldX = GetTileX();
	long lOldY = GetTileY();
	long lNewTileX = static_cast<long>(fX);
	long lNewTileY = static_cast<long>(fY);
	CMoveShape::SetPosXY(fX,fY);
	//取消老阻挡
	SetBlock(lOldX,lOldY,CRegion::BLOCK_NO);
	//设置怪物阻挡
	if( !IsDied())
		SetBlock(lNewTileX,lNewTileY,CRegion::BLOCK_SHAPE);
}

void CBuild::SetBlock(int x, int y, unsigned short block)
{
	CShape::SetBlock(x,y,block);
}



void CBuild::InitNameValueMap()
{
	CMoveShape::InitNameValueMap();
	m_mapNameValue.clear();
	long minValue[7] = { 0x0, 0, 0, (long)-99999999.999f, -0x7FFFFFFF, -0x7FFF, 0};
	long maxValue[7] = { 0xFF, 0xFFFF, 0x7FFFFFFF, (long)99999999.999f, 0x7FFFFFFF, 0x7FFF, 1};

	tagPty st;// 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool

	st.lType=4; st.pValue=&m_lType;         st.minValue = minValue[4];st.maxValue = maxValue[4];	   st.attrEnumValue= B_ATTR_TYPE;			m_mapNameValue["Type"] = st;		
	st.lType=4; st.pValue=&m_lGraphicsID;	st.minValue = minValue[4];st.maxValue = maxValue[4];		st.attrEnumValue=B_ATTR_GRAPHICSID;		m_mapNameValue["GraphicsID"]=st;	
	st.lType=4; st.pValue=&m_lRegionID;		st.minValue = minValue[4];st.maxValue = maxValue[4];		 st.attrEnumValue=B_ATTR_REGIONID;		m_mapNameValue["RegionID"]=st;
	st.lType=3;	st.pfValue=&m_fPosX;		st.minValue = minValue[3];st.maxValue = maxValue[3];		 st.attrEnumValue=B_ATTR_TILEX;			m_mapNameValue["TileX"]=st;			
	st.lType=3;	st.pfValue=&m_fPosY;		st.minValue = minValue[3];st.maxValue = maxValue[3];       st.attrEnumValue=B_ATTR_TILEY;			m_mapNameValue["TileY"]=st;	
	st.lType=3;	st.pfValue=&m_fDir;			st.minValue = minValue[3];st.maxValue = maxValue[3];         st.attrEnumValue=B_ATTR_DIR;			m_mapNameValue["Dir"]=st;			
	st.lType=4;	st.pValue=&m_lPos;			st.minValue = minValue[4];st.maxValue = maxValue[4];         st.attrEnumValue=B_ATTR_POS;			m_mapNameValue["Pos"]=st;			
	st.lType=1;	st.pwValue=&m_wState;		st.minValue = minValue[1];st.maxValue = maxValue[1];		 st.attrEnumValue=B_ATTR_STATE;			m_mapNameValue["State"]=st;			
	st.lType=1;	st.pwValue=&m_wAction;		st.minValue = minValue[1];st.maxValue = maxValue[1];		 st.attrEnumValue=B_ATTR_ACTION;		m_mapNameValue["Action"]=st;		

	//建物属性
	st.lType=4;	st.pValue = &m_lIndex;		st.minValue = minValue[4];st.maxValue = maxValue[4];		st.attrEnumValue=B_ATTR_INDEX;			m_mapNameValue["Index"]=st;	
	st.lType=4;	st.pValue = &m_lHP;			st.minValue = minValue[4];st.maxValue = maxValue[4];   		st.attrEnumValue=B_ATTR_HP;		    	m_mapNameValue["Hp"]=st;	
	st.lType=4;	st.pValue = &m_lNameColor;	st.minValue = minValue[4];st.maxValue = maxValue[4];        st.attrEnumValue=B_ATTR_NAMECOLOR;		m_mapNameValue["NameColor"]=st;	
	st.lType=4;	st.pValue = &m_lLevel;		st.minValue = minValue[4];st.maxValue = maxValue[4];		st.attrEnumValue=B_ATTR_LEVEL;			m_mapNameValue["Level"]=st;	
	st.lType=4;	st.pValue = &m_lMaxHP;		st.minValue = minValue[4];st.maxValue = maxValue[4];		st.attrEnumValue=B_ATTR_MAXHP;			m_mapNameValue["MaxHp"]=st;	
	st.lType=4;	st.pValue = &m_lDef;		st.minValue = minValue[4];st.maxValue = maxValue[4];		st.attrEnumValue=B_ATTR_DEF;			m_mapNameValue["E_Def"]=st;
	st.lType=4;	st.pValue = &m_lMagicDef;	st.minValue = minValue[4];st.maxValue = maxValue[4];		st.attrEnumValue=B_ATTR_MDEF;			m_mapNameValue["E_MDef"]=st;
	
	//基本属性
	st.lType=4;	st.pValue = &m_lMaxHP;		st.minValue = minValue[4];st.maxValue = maxValue[4];		m_mapNameValue["B_MaxHp"]=st;				//生命上限
	st.lType=4;	st.pValue = &m_lDef;		st.minValue = minValue[4];st.maxValue = maxValue[4];		m_mapNameValue["B_Def"]=st;
	st.lType=4;	st.pValue = &m_lMagicDef;	st.minValue = minValue[4];st.maxValue = maxValue[4];		m_mapNameValue["B_MDef"]=st;	//魔法防御
}

long CBuild::GetValue(const char* strName)
{
	long value = INVILID_VALUE;
	value = CMoveShape::GetValue(strName);
	if(INVILID_VALUE != value)
		return value;

	mapNameValueItr itr = m_mapNameValue.begin();
	itr = m_mapNameValue.find(strName);
	if (itr != m_mapNameValue.end())
	{
		switch (itr->second.lType)
		{
		case 0:	// byte
			return *(itr->second.plValue);

		case 1:	// word
			return *(itr->second.pwValue);

		case 2:	// dword
			return *(itr->second.pdwValue);

		case 3:	// float
			return *(itr->second.pfValue);

		case 4:	// long
			return *(itr->second.pValue);

		case 5:	// short
			return *(itr->second.psValue);

		case 6:	// bool
			return *(itr->second.pbValue);
		}
	}
	return value;
}

long CBuild::SetValue(const char* strName, long dwValue)
{
	long value = INVILID_VALUE;
	value = CMoveShape::SetValue(strName, dwValue);
	if(INVILID_VALUE != value)
		return value;

	mapNameValueItr itr = m_mapNameValue.begin();
	itr = m_mapNameValue.find(strName);
	if (itr != m_mapNameValue.end())
	{
		if(dwValue < itr->second.minValue)
			dwValue = itr->second.minValue;
		if(dwValue > itr->second.maxValue)
			dwValue = itr->second.maxValue;

		switch (itr->second.lType)
		{
		case 0:	// byte
			*(itr->second.plValue) = (BYTE)dwValue;
			return dwValue;

		case 1:	// word
			*(itr->second.pwValue) = (WORD)dwValue;
			return dwValue;

		case 2:	// dword
			*(itr->second.pdwValue) = (DWORD)dwValue;
			return dwValue;

		case 3:	// float
			*(itr->second.pfValue) = (float)dwValue;
			return dwValue;

		case 4:	// long
			*(itr->second.pValue) = (long)dwValue;
			return dwValue;

		case 5:	// short
			*(itr->second.psValue) = (short)dwValue;
			return dwValue;

		case 6:	// bool
			*(itr->second.pbValue) = dwValue==0?false:true;
			return dwValue;
		}
	}
	return value;
}

//添加变量名到胀数据
void  CBuild::AddAttributeToDirty(string attrName)
{
	if(attrName == "")	return;
	if( (attrName[0] == 'E' && attrName[1] == '_')
		|| (attrName[0] == 'B' && attrName[1] == '_') )
		attrName[0] = 'C';
	m_mapDirtyAttrData.insert(attrName);
}
bool  CBuild::SetAttribute(std::string& attrName, long value)
{
	bool bFlag = CMoveShape::SetAttribute(attrName, value);
	if(!bFlag)
	{
		// 脏属性数据标记
		if(SetValue(attrName.c_str(), value) != INVILID_VALUE)
		{
			bFlag = true;
		}
	}
	if(bFlag)
		AddAttributeToDirty(attrName);
	return false;
}
bool  CBuild::SetAttribute(char* attrName, long value)
{
	if(NULL == attrName)
		return false;

	bool bFlag = CMoveShape::SetAttribute(attrName, value);
	if(!bFlag)
	{
		// 脏属性数据标记
		if(SetValue(attrName, value) != INVILID_VALUE)
		{			
			bFlag = true;
		}
	}
	if(bFlag)
		AddAttributeToDirty(string(attrName));
	return false;
}
long CBuild::GetAttribute(std::string attrName)
{
	long value = INVILID_VALUE;

	value = CMoveShape::GetAttribute(attrName);
	if(value != INVILID_VALUE)
		return value;

	if(attrName.size() > 2 && attrName[0] == 'C' && attrName[1] == '_') // 取得当前值
	{
		// 取得改变值
		attrName[0] = 'E';
		long evalue = GetValue(attrName.c_str());
		// 取得基本值
		attrName[0] = 'B';
		long bvalue = GetValue(attrName.c_str());

		// 取得当前值的合法值
		map<string, tagPty>::iterator vMoveShapeItr = m_mapShapeNameValue.find(attrName);
		if(vMoveShapeItr == m_mapShapeNameValue.end())
		{
			map<string, tagPty>::iterator vMonsterItr = m_mapNameValue.find(attrName);
			if(vMonsterItr != m_mapNameValue.end())
			{
				tagPty& tagValue = vMonsterItr->second;
				value = GetCurrentValue(tagValue, evalue, bvalue);
			}
		}
		else
		{
			tagPty& tagValue = vMoveShapeItr->second;
			value = GetCurrentValue(tagValue, evalue, bvalue);
		}
	}
	else
		value = GetValue(attrName.c_str());

	return value;
}

// 取得当前值的合法值
long CBuild::GetCurrentValue(const tagPty& tagValue, long evalue/*改变值*/, long bvalue/*基本值*/)
{
	long retValue = evalue+bvalue;
	if( retValue > tagValue.maxValue )
		retValue = tagValue.maxValue;

	if( retValue < tagValue.minValue )
		retValue = tagValue.minValue;

	return retValue;
}

bool  CBuild::ChangeAttribute(std::string& attrName, long value)
{
	bool bFlag = CMoveShape::ChangeAttribute(attrName, value);
	if(!bFlag)
	{
		long curValue = 0;
		long rightValue = GetValue(attrName.c_str());

		curValue = rightValue + value;
		mapNameValueItr itr = m_mapNameValue.begin();
		itr = m_mapNameValue.find(attrName);
		if (itr != m_mapNameValue.end())
		{
			if(attrName[0] != 'E') // 改变值可以为负数
			{
				if( rightValue + value <  itr->second.minValue ) 
					curValue = itr->second.minValue;
				if( rightValue + value > itr->second.maxValue ) 
					curValue = itr->second.maxValue;
			}

			if(SetValue(attrName.c_str(), curValue) != INVILID_VALUE)
			{
				bFlag = true;
			}
		}
	}
	if(bFlag)
		AddAttributeToDirty(attrName);
	return bFlag;
}
long  CBuild::UpdateAttributeToClient()
{
	if( m_mapDirtyAttrData.size() == 0)
		return false;

	CMessage msg( MSG_S2C_SHAPE_CHANGE_PROPERTIES );
	msg.Add(GetType());
	msg.Add(GetExID());
	DBWriteSet setWriteDB;
	msg.GetDBWriteSet(setWriteDB);
	CodeChangeAttrToDataBlock(setWriteDB);
	// 发送msg
	msg.SendToAround(this);
	return true;
}

long  CBuild::UpdateAttribute(BOOL bImmediately)
{
	if(m_mapDirtyAttrData.size() > 0)
	{
		RegisterUpdateAttrTimer();
	}
	return true;
}
long  CBuild::OnUpdateAttributeTimeOut()
{
	UpdateAttributeToClient();
	return true;
}



void CBuild::CodeChangeAttrToDataBlock(DBWriteSet& setWriteDB)
{
	long lShapeNum = 0;
	long lBuildNum = 0;
	vector<BYTE>	byShapeDirtyData;byShapeDirtyData.clear();
	vector<BYTE>	byBuildDirtyData;byBuildDirtyData.clear();
	//在m_mapShapeDirtyAttrData中查找,是否修改CMoveShape中的属性
	set<string>::iterator itDirty = m_mapDirtyAttrData.begin();
	for (;itDirty!=m_mapDirtyAttrData.end();itDirty++)
	{
		string name = *itDirty;
		mapShapeNameValueItr sitr = m_mapShapeNameValue.find(name);
		if (sitr != m_mapShapeNameValue.end())
		{
			lShapeNum++;
			//msg.Add((BYTE)saitr->second); // 属性类型
			::_AddToByteArray(&byShapeDirtyData, (WORD)sitr->second.attrEnumValue);
			switch (sitr->second.lType)
			{
			case 0:	// byte
				::_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_BYTE);
				::_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(BYTE));
				::_AddToByteArray(&byShapeDirtyData, (BYTE)*(BYTE*)(sitr->second.plValue));
				break;
			case 1:	// word
				::_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_WORD);
				::_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(WORD));
				::_AddToByteArray(&byShapeDirtyData, (WORD)*(WORD*)(sitr->second.pwValue));
				break;
			case 2:	// dword
				::_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_DWORD);
				::_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(DWORD));
				::_AddToByteArray(&byShapeDirtyData, (DWORD)*(DWORD*)(sitr->second.pdwValue));
				break;
			case 3:	// float
				::_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_FLOAT);
				::_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(float));
				::_AddToByteArray(&byShapeDirtyData, (float)*(float*)(sitr->second.pfValue));
				break;
			case 4:	// long
				::_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_LONG);
				::_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(long));
				::_AddToByteArray(&byShapeDirtyData, (long)*(long*)(sitr->second.pValue));
				break;
			case 5:	// short
				::_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_SHORT);
				::_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(short));
				::_AddToByteArray(&byShapeDirtyData, (short)*(short*)(sitr->second.psValue));
				break;
			case 6:	// bool
				::_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_BOOL);
				::_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(bool));
				::_AddToByteArray(&byShapeDirtyData, (char)*(bool*)(sitr->second.pbValue));
				break;
			default:
				break;
			}
		}

		//查找修改的Build属性
		long baseValue = 0;
		if(name[0] == 'C' && name[1] == '_')
		{	
			// 先找出基本值
			name[0] = 'B';
			baseValue = GetValue(name.c_str());
			// 再找出改变值
			name[0] = 'E';
		}
		mapNameValueItr itr = m_mapNameValue.find(name);
		if (itr!=m_mapNameValue.end())
		{
			lBuildNum++;
			// 属性类型
			::_AddToByteArray(&byBuildDirtyData, (WORD)itr->second.attrEnumValue);
			long sendValue = INVILID_VALUE;

			switch (itr->second.lType)
			{
			case 0:	// byte
				::_AddToByteArray(&byBuildDirtyData, (BYTE)ATTR_DATA_BYTE);
				::_AddToByteArray(&byBuildDirtyData, (WORD)sizeof(BYTE));
				if((*itDirty)[0] == 'C' && (*itDirty)[1] == '_') // 最终值 
				{
					BYTE EValue = *(BYTE*)(itr->second.plValue);
					sendValue = GetCurrentValue(itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(BYTE*)(itr->second.plValue);

				::_AddToByteArray(&byBuildDirtyData, (BYTE)sendValue);

				break;
			case 1:	// word 
				::_AddToByteArray(&byBuildDirtyData, (BYTE)ATTR_DATA_WORD);
				::_AddToByteArray(&byBuildDirtyData, (WORD)sizeof(WORD));
				if(((*itDirty)[0] == 'C' && (*itDirty)[1] == '_')) // 改变值) // 改变值
				{
					WORD EValue = *(WORD*)(itr->second.pwValue);
					sendValue = GetCurrentValue(itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(WORD*)(itr->second.pwValue);
				::_AddToByteArray(&byBuildDirtyData, (WORD)sendValue);
				break;
			case 2:	// dword
				::_AddToByteArray(&byBuildDirtyData, (BYTE)ATTR_DATA_DWORD);
				::_AddToByteArray(&byBuildDirtyData, (WORD)sizeof(DWORD));
				if(((*itDirty)[0] == 'C' && (*itDirty)[1] == '_')) // 改变值) // 改变值
				{
					DWORD EValue = *(DWORD*)(itr->second.pdwValue);
					sendValue = GetCurrentValue(itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(DWORD*)(itr->second.pdwValue);
				::_AddToByteArray(&byBuildDirtyData, (DWORD)sendValue);
				break;
			case 3:	// float
				::_AddToByteArray(&byBuildDirtyData, (BYTE)ATTR_DATA_FLOAT);
				::_AddToByteArray(&byBuildDirtyData, (WORD)sizeof(float));
				if(((*itDirty)[0] == 'C' && (*itDirty)[1] == '_')) // 改变值) // 改变值
				{
					float EValue = *(float*)(itr->second.pfValue);
					sendValue = GetCurrentValue(itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(float*)(itr->second.pfValue);
				::_AddToByteArray(&byBuildDirtyData, (float)sendValue);
				break;
			case 4:	// long			
				::_AddToByteArray(&byBuildDirtyData, (BYTE)ATTR_DATA_LONG);
				::_AddToByteArray(&byBuildDirtyData, (WORD)sizeof(long));
				if(((*itDirty)[0] == 'C' && (*itDirty)[1] == '_')) // 改变值) // 改变值
				{
					long EValue = (long)*(long*)(itr->second.pValue);
					sendValue = GetCurrentValue(itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(long*)(itr->second.pValue);
				::_AddToByteArray(&byBuildDirtyData, (long)sendValue);
				break;
			case 5:	// short
				::_AddToByteArray(&byBuildDirtyData, (BYTE)ATTR_DATA_SHORT);
				::_AddToByteArray(&byBuildDirtyData, (WORD)sizeof(short));
				if(((*itDirty)[0] == 'C' && (*itDirty)[1] == '_')) // 改变值) // 改变值
				{
					short EValue = *(short*)(itr->second.psValue);
					sendValue = GetCurrentValue(itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(short*)(itr->second.psValue);
				::_AddToByteArray(&byBuildDirtyData, (short)sendValue);
				break;
			case 6:	// bool
				::_AddToByteArray(&byBuildDirtyData, (BYTE)ATTR_DATA_BOOL);
				::_AddToByteArray(&byBuildDirtyData, (WORD)sizeof(bool));
				if(((*itDirty)[0] == 'C' && (*itDirty)[1] == '_')) // 改变值) // 改变值
				{
					bool EValue = *(bool*)(itr->second.pbValue);
					sendValue = GetCurrentValue(itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(bool*)(itr->second.pbValue);
				::_AddToByteArray(&byBuildDirtyData, (BYTE)sendValue);
				break;
			default:
				break;
			}
		}	

	}
	setWriteDB.AddToByteArray((WORD)lShapeNum);
	if(byShapeDirtyData.size() > 0)
		setWriteDB.AddToByteArray(&byShapeDirtyData[0],byShapeDirtyData.size());
	setWriteDB.AddToByteArray((WORD)lBuildNum);
	if(byBuildDirtyData.size() > 0)
		setWriteDB.AddToByteArray(&byBuildDirtyData[0],byBuildDirtyData.size());
	m_mapDirtyAttrData.clear();
	m_mapShapeDirtyAttrData.clear();
}

void CBuild::SetBuild(WORD l)
{
	switch (l)
	{
	case STATE_DESTORY:			//设置损毁状态
		{
			if (m_wState!=CShape::STATE_DIED)
			{
				SetAttribute("Action",ACT_DIED);
				SetAttribute("State",STATE_DIED);
				SetAttribute("Hp",0);
				SetAction(ACT_DIED);
				OnDied();
			}
		}
		break;
	case STATE_REFRESH:			//设置为重刷状态
		{

			SetAttribute("Hp",GetMaxHP());
			MoveAroundPlayer();
			SetAttribute("Action",ACT_STAND);
			SetAttribute("State",STATE_FIGHT);
			SetAction(ACT_STAND);			
		}
		break;
	default:
		break;
	}
}

void CBuild::MoveAroundPlayer()
{
	CShape *pShape=dynamic_cast<CShape*>(this);
	vector<CPlayer*> vOut;
	dynamic_cast<CServerRegion*>(this->GetFather())->FindAroundPlayer(pShape,vOut);
	long lx=GetTileX();
	long ly=GetTileY();
	//移开在门区域中的其他shape,设置阻挡

	for (int i=lx-GetFigure(DIR_RIGHT); i<=lx+GetFigure(DIR_RIGHT); i++)
	{
		for (int j=ly-GetFigure(DIR_UP); j<=ly+GetFigure(DIR_UP); j++)
		{
			vector<CPlayer*>::iterator iter=vOut.begin();
			for (;iter!=vOut.end();iter++)
			{
				if ((*iter)->GetTileX()==i && (*iter)->GetTileY()==j)
				{
					(*iter)->SetPosition(lx+GetWidth(),ly+GetWidth());
				}
			}
		}
	}	
}