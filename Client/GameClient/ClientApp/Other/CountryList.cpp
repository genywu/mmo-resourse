#include "StdAfx.h"
#include "CountryList.h"
#include <sstream>
#include "../../../Public/Common/CStrConverter.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

map<BYTE, CCountryList::tagCountryList>		CCountryList::m_mpCountryList;
D3DCOLOR CCountryList::m_dwMyCountryColor = -1;
D3DCOLOR CCountryList::m_dwOtherCountryColor = -1;
D3DCOLOR CCountryList::m_dwNoCountryColor = -1;
CCountryList::CCountryList(void)
{
}

CCountryList::~CCountryList(void)
{
	m_mpCountryList.clear();	
}

// 从文件读取
BOOL CCountryList::Load(const char* filename)
{
	m_mpCountryList.clear();

	// 解析 XML 配置文件
	TiXmlNode *pNode = NULL;
	TiXmlDocument m_Tdoc;

	//if( ! m_Tdoc.LoadFile( filename ) )
	//{
	//	MessageBox( GetActiveWindow(), "Load CountryList.xml file error!", "WangFeng", MB_OK );
	//	return FALSE;
	//} 
	CRFile *prfile= rfOpen(filename);
	if(prfile==NULL)
	{
		MessageBox( GetActiveWindow(), "Load CountryList.xml file error!", "WangFeng", MB_OK );
		return FALSE;
	}
	if(!m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()))
	{
		MessageBox( GetActiveWindow(), "Load CountryList.xml file error!", "WangFeng", MB_OK );
		return FALSE;
	}
	rfClose(prfile);

	// 找到根节点，如果无节点，输出错误提示
	if( !( pNode = m_Tdoc.FirstChild( "CountryList" ) ) )
	{
		MessageBox( GetActiveWindow(), "Can't find the CountryList node in CountryList.xml file!", "WangFeng", MB_OK );
		return FALSE;
	}

	// 遍历子节点
	TiXmlElement* pElem = pNode->ToElement();
	for( TiXmlElement *pChildElem = pElem->FirstChildElement(); pChildElem; pChildElem = pChildElem->NextSiblingElement() )
	{
		// 颜色配置
		if( !strcmp( pChildElem->Value(), "Color" )
			&& pChildElem->Attribute( "Country" )
			&& pChildElem->Attribute( "Alliance" )
			&& pChildElem->Attribute( "DiffrenAlliance" ) )
		{
			m_dwMyCountryColor = CStrConverter::parseStringToDec( pChildElem->Attribute( "Country" ) );
			m_dwOtherCountryColor = CStrConverter::parseStringToDec( pChildElem->Attribute( "Alliance" ) );
			m_dwNoCountryColor = CStrConverter::parseStringToDec( pChildElem->Attribute( "DiffrenAlliance" ) );
		}
		// 国家同盟配置
		else if( !strcmp( pChildElem->Value(), "Alliance" )
			&& pChildElem->Attribute( "AllianceType" )
			&& pChildElem->Attribute( "CountryNum" )
			&& pChildElem->Attribute( "AllianceDesc" ) )
		{
			tagCountryList stList;

			// 阵营
			stList.nLeague = (BYTE) CStrConverter::parseLong( pChildElem->Attribute( "AllianceType" ) );
			stList.strAllianceDesc	= pChildElem->Attribute( "AllianceDesc" );

			// 获取同盟国
			for( TiXmlElement *pCountryElem = pChildElem->FirstChildElement(); pCountryElem; pCountryElem = pCountryElem->NextSiblingElement() )
			{
				if( !strcmp( pCountryElem->Value(), "Country" )
					&& pCountryElem->Attribute( "CountryID" )
					&& pCountryElem->Attribute( "CountryIconID" )
					&& pCountryElem->Attribute( "CountryName" )
					&& pCountryElem->Attribute( "CountryDesc" ) )
				{
					stList.nID		= (BYTE) CStrConverter::parseLong( pCountryElem->Attribute( "CountryID" ) );
					stList.lIconID	= CStrConverter::parseLong( pCountryElem->Attribute( "CountryIconID" ) );
					stList.strName	= pCountryElem->Attribute( "CountryName" );
					stList.strDesc	= pCountryElem->Attribute( "CountryDesc" );

					// 存入 MAP
					m_mpCountryList[stList.nID] = stList;
				}
			}
		}
	}

	return TRUE;
}

CCountryList::tagCountryList* CCountryList::GetCountry(BYTE nID)
{
	if (m_mpCountryList.find(nID) != m_mpCountryList.end())
	{
		return &m_mpCountryList[nID];
	}
	return NULL;
}

const char* CCountryList::GetCountryName(BYTE nID)
{
	tagCountryList* cl = GetCountry(nID);
	if(cl)
		return cl->strName.c_str();
	else 
		return NULL;
}