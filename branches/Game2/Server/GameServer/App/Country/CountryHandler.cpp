#include "StdAfx.h"
#include "CountryHandler.h"
#include "Country.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCountryHandler *CCountryHandler::instance = NULL;

CCountryHandler::CCountryHandler(void)
{
}

CCountryHandler::~CCountryHandler(void)
{
}

CCountryHandler* CCountryHandler::GetInstance(void)
{
	if(instance == NULL)
	{
		instance = OBJ_CREATE(CCountryHandler);
	}
	return instance;
}

bool CCountryHandler::Initialize(void)
{
	return true;
}


//释放资源
void CCountryHandler::Release(void)
{
	map<BYTE,CCountry*>::iterator it;
	for(it = m_pCountrys.begin(); it != m_pCountrys.end(); ++it)
	{
		OBJ_RELEASE(CCountry, it->second);
	}

	OBJ_RELEASE(CCountryHandler, instance);
}



bool CCountryHandler::Append( CCountry* c )
{
	if(!c) return false;

	m_pCountrys[c->GetID()] = c;
	return true;
}

// 添加到CByteArray
bool CCountryHandler::AddToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray, (long)m_pCountrys.size());

	for(map<BYTE,CCountry*>::iterator it = m_pCountrys.begin(); it != m_pCountrys.end(); ++it)
	{

		it->second->AddToByteArray(pByteArray);

		//_AddToByteArray(pByteArray, it->second->GetID());
		//_AddToByteArray(pByteArray, it->second->GetPower());
		//_AddToByteArray(pByteArray, it->second->GetKing());
		//_AddToByteArray(pByteArray, it->second->GetTechLevel());
		//_AddToByteArray(pByteArray, (long)(it->second->GetState()));
	}

	return true;
}

// 解码
bool CCountryHandler::DecordFromByteArray(BYTE* pByte, long& pos)
{
	map<BYTE,CCountry*>::iterator it;
	for(it = m_pCountrys.begin(); it != m_pCountrys.end(); ++it)
	{
		OBJ_RELEASE(CCountry, it->second);
	}

	//BYTE id;
	long s;//, power, king, techlevel, state;

	s = _GetLongFromByteArray(pByte, pos);
	for(int i = 0 ; i < s ; ++i)
	{
		CCountry *pCountry = OBJ_CREATE(CCountry);
		if( !pCountry) return false;

		pCountry->DecordFromByteArray(pByte, pos);


		//id = _GetByteFromByteArray(pByte, pos);
		//power = _GetLongFromByteArray(pByte, pos);
		//king = _GetLongFromByteArray(pByte, pos);
		//techlevel = _GetLongFromByteArray(pByte, pos);
		//state = _GetLongFromByteArray(pByte, pos);

		//pCountry->SetID(id);
		//pCountry->SetPower(power);
		//pCountry->SetKing(king);
		//pCountry->SetTechLevel(techlevel);
		//pCountry->SetState((CCountry::eState)state);

		Append(pCountry);
	}
	return true;
}