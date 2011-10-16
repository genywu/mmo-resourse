#include "StdAfx.h"
#include "countryHandler.h"
#include "Country.h"
//#include "..\dbaccess\worlddb\DBCountry.h"


CCountryHandler *CCountryHandler::instance = NULL;

CCountryHandler::CCountryHandler(void)
{
}

CCountryHandler::~CCountryHandler(void)
{
	//map<long,tagCountryList*>::iterator it=m_mpContryList.begin();
	//for (;it!=m_mpContryList.end();++it)
	//{
	//	SAFE_DELETE((*it).second);
	//}
	//m_mpContryList.clear();
}


bool CCountryHandler::Initialize(void)
{
	Load();
	return true;
}


bool CCountryHandler::Load()
{
	//m_mpContryList.clear();

	char filename[] = "data/countrylist.ini";

	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		AddLogText("file '%s' can't found!", filename);
		return false;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);
	
	while(ReadTo(stream, "#"))
	{
		tagCountryList ptgCountryList;
		long lID;
		stream >>lID
			   >>ptgCountryList.lID
			   >>ptgCountryList.strName
			   >>ptgCountryList.strDesc;
		CCountry *pCountry = MP_NEW CCountry;
		pCountry->SetID(lID);
		GetInstance()->Append(pCountry);
		
	}

	return true;
}


//释放资源
void CCountryHandler::Release(void)
{
	for(map<BYTE,CCountry*>::iterator it = m_pCountrys.begin(); it != m_pCountrys.end(); ++it)
	{
		MP_DELETE(it->second);
	}

	SAFE_DELETE(instance);
}

// 生成存盘数据
void CCountryHandler::GenerateSaveData(void)
{
/*	map<BYTE,CCountry*>::iterator it;
	for( it=m_pCountrys.begin(); it!=m_pCountrys.end(); ++it)
	{
		if( it->second )
		{
			CCountry* pCloneCountry = it->second->CloneSaveData();
			if(pCloneCountry)
			{
				GetGame()->AppendDBCountry(pCloneCountry);
			}
		}

	}*/
}

bool CCountryHandler::Append( CCountry* c )
{
	if(!c) return false;

	m_pCountrys[c->GetID()] = c;
	char strInfo[256]="";
	_snprintf(strInfo,256,"添加一个国家(ID:%d).",c->GetID());
	AddLogText(strInfo);
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
		MP_DELETE(it->second);
	}

	//BYTE id;
	long s;//, power, king, techlevel, state;

	s = _GetLongFromByteArray(pByte, pos);
	for(int i = 0 ; i < s ; ++i)
	{
		CCountry *pCountry = MP_NEW CCountry;
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