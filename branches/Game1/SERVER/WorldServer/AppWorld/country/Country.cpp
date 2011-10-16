#include "StdAfx.h"
#include ".\country.h"

CCountry::CCountry(void)
:m_lID(0), m_lPower(0), m_lTechLevel(0),m_eState(PEACE)
{
}

CCountry::~CCountry(void)
{
}

//克隆保存数据,只是克隆变化了的数据
CCountry* CCountry::CloneSaveData()
{
	CCountry* pCloneCountry= MP_NEW CCountry();
	if( pCloneCountry ) 
	{
		pCloneCountry->m_lID		= m_lID;
		pCloneCountry->m_lPower		= m_lPower;
		pCloneCountry->m_King		= m_King;
		pCloneCountry->m_lTechLevel	= m_lTechLevel;
	}
	return pCloneCountry;
}

//BYTE			m_lID;						// 国家ID  // 这个ID是固定了的。
//											// 可能从配置文件里获取，可能在数据库字典表里获取。
//long			m_lPower;					// 国力 上线为1亿
//long			m_lKing;					// 国王ID
//long			m_lTechLevel;				// 科技等级

//eState			m_eState;					// 国家当前状态

// 添加到CByteArray
bool CCountry::AddToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray, m_lID);
	_AddToByteArray(pByteArray, m_lPower);
	_AddToByteArray(pByteArray, m_King);
	_AddToByteArray(pByteArray, m_lTechLevel);
	_AddToByteArray(pByteArray, (long)m_eState);
	return true;
}

// 解码
bool CCountry::DecordFromByteArray(BYTE* pByte, long& pos)
{
	m_lID = _GetByteFromByteArray(pByte, pos);
	m_lPower = _GetLongFromByteArray(pByte, pos);
	_GetBufferFromByteArray(pByte, pos, m_King);
	m_lTechLevel = _GetLongFromByteArray(pByte, pos);
	m_eState = (eState)_GetLongFromByteArray(pByte, pos);
	return true;
}