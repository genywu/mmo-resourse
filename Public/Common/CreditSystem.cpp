#include "StdAfx.h"
#include "CreditSystem.h"

CCreditSystem::CCreditSystem(void)
{
}

CCreditSystem::~CCreditSystem(void)
{
}


bool CCreditSystem::LoadAreaCredit()
{
	m_vecCreditLevel.clear();
	m_mapTradeAreas.clear();
	m_mapOrganCredit.clear();
	TiXmlNode *pNode = NULL;
	TiXmlDocument m_Tdoc;	
	CRFile* prFile=rfOpen("data\\area.xml");
	if (prFile==NULL)
		return false;
	if(!m_Tdoc.LoadData(prFile->GetData(),prFile->GetDatalen()))
	{		
		rfClose(prFile);
		return false;
	}
	rfClose(prFile);
	pNode=m_Tdoc.FirstChild("TradeArea");
	if (pNode==NULL)
	{		
		return false;
	}

	//区域声望
	TiXmlElement* pElement=pNode->ToElement();	
	TiXmlElement* pChildElement=pElement->FirstChildElement("Area");
	for(TiXmlElement* pAreaElement=pChildElement->FirstChildElement();pAreaElement!=NULL;
		pAreaElement=pAreaElement->NextSiblingElement())
	{
		if(strcmp(pAreaElement->Value(),"AreaItem")==0)
		{
			AREA area;
			area._iIndex=(int)atoi(pAreaElement->Attribute("Index"));
			area._dwAreaID=(ulong)atoi(pAreaElement->Attribute("ID"));
			const char* strName=pAreaElement->Attribute("NAME");
			strcpy(area._cName,strName);
			m_mapTradeAreas[area._dwAreaID]=area;
		}		
	}
    Log4c::Warn(ROOT_MODULE, "AreaNum：%d",m_mapTradeAreas.size());

	//区域等级
	pChildElement=pChildElement->NextSiblingElement("CreditLevel");
	LoadCreditLevel(pChildElement,m_vecCreditLevel);
	//组织等级
	pChildElement=pChildElement->NextSiblingElement("Organization");
	LoadOrganizeCredit(pChildElement);

	return true;
}

bool CCreditSystem::LoadCreditLevel(TiXmlElement* p,vector<CREDITLEVEL>& vecCreditLevel)
{
	for(TiXmlElement* pCreditElement=p->FirstChildElement();pCreditElement!=NULL;
		pCreditElement=pCreditElement->NextSiblingElement())
	{
		if(strcmp(pCreditElement->Value(),"Level")==0)
		{
			CREDITLEVEL tglevel;
			tglevel._dwLevelID=(ulong)atoi(pCreditElement->Attribute("ID"));
			tglevel._dwValue=(ulong)atoi(pCreditElement->Attribute("VAL"));
			strcpy(tglevel._cName,pCreditElement->Attribute("NAME"));
			vecCreditLevel.push_back(tglevel);
		}
	}
	return true;
}

bool CCreditSystem::LoadOrganizeCredit(TiXmlElement* p)
{
	ORGANCREDIT tgOrganCredit;
	for(TiXmlElement* pOrganElement=p->FirstChildElement();pOrganElement!=NULL;
		pOrganElement=pOrganElement->NextSiblingElement())
	{
		if(strcmp(pOrganElement->Value(),"Organ")==0)
		{
			pOrganElement->Attribute("id",(int*)&tgOrganCredit._dwOrganizeID);
			strcpy(tgOrganCredit._cDirtyName,pOrganElement->Attribute("DirtyName"));
			strcpy(tgOrganCredit._cName,pOrganElement->Attribute("NAME")); 
			TiXmlElement* pLevelValue=pOrganElement->FirstChildElement();
			tgOrganCredit._vecLevel.clear();
			LoadCreditLevel(pLevelValue,tgOrganCredit._vecLevel);
		}
		m_mapOrganCredit[tgOrganCredit._dwOrganizeID]=tgOrganCredit;
	}
	return true;
}

bool CCreditSystem::CodeToByteArray(DBWriteSet& dbWriteSet)
{
	dbWriteSet.AddToByteArray((ulong)m_mapTradeAreas.size());
	map<ulong,AREA>::iterator it=m_mapTradeAreas.begin();
	for (;it!=m_mapTradeAreas.end();++it)
	{
		dbWriteSet.AddToByteArray(&(it->second),sizeof(AREA));
	}

	dbWriteSet.AddToByteArray((ulong)m_vecCreditLevel.size());
	for (size_t i=0;i<m_vecCreditLevel.size();i++)
	{
		dbWriteSet.AddToByteArray(&(m_vecCreditLevel[i]),sizeof(CREDITLEVEL));
	}

	dbWriteSet.AddToByteArray((ulong)m_mapOrganCredit.size());
	ORGANITR itOragn=m_mapOrganCredit.begin();
	for (;itOragn!=m_mapOrganCredit.end();++itOragn)
	{
		dbWriteSet.AddToByteArray(itOragn->second._dwOrganizeID);
		dbWriteSet.AddToByteArray(itOragn->second._cName);
		dbWriteSet.AddToByteArray(itOragn->second._cDirtyName);
		dbWriteSet.AddToByteArray((ulong)itOragn->second._vecLevel.size());
		for (size_t i=0;i<itOragn->second._vecLevel.size();i++)
		{
			dbWriteSet.AddToByteArray(&(itOragn->second._vecLevel[i]),sizeof(CREDITLEVEL));
		}
	}
	return true;
}

bool CCreditSystem::DecodeToByteArray(DBReadSet& dbReadSet)
{
	ulong dwsize=dbReadSet.GetDwordFromByteArray();	
	AREA area;
	for (size_t i=0;i<dwsize;++i)
	{
		dbReadSet.GetBufferFromByteArray(&area,sizeof(AREA));
		m_mapTradeAreas[area._dwAreaID]=area;
	}


	dwsize=dbReadSet.GetDwordFromByteArray();
	CREDITLEVEL tglevel;
	for (size_t i=0;i<dwsize;++i)
	{	
		dbReadSet.GetBufferFromByteArray(&tglevel,sizeof(CREDITLEVEL));
		m_vecCreditLevel.push_back(tglevel);
	}
	
	
	dwsize=dbReadSet.GetDwordFromByteArray();
	ORGANCREDIT tgOrganCredit;
	for (size_t i=0;i<dwsize;++i)
	{
		tgOrganCredit._vecLevel.clear();

		tgOrganCredit._dwOrganizeID=dbReadSet.GetDwordFromByteArray();
		dbReadSet.GetStringFromByteArray(tgOrganCredit._cName,MAXAREANAMESIZE);
		dbReadSet.GetStringFromByteArray(tgOrganCredit._cDirtyName,MAXAREANAMESIZE);
		ulong vecsize=dbReadSet.GetDwordFromByteArray();
		for (size_t j=0;j<vecsize;++j)
		{
			dbReadSet.GetBufferFromByteArray(&tglevel,sizeof(CREDITLEVEL));
			tgOrganCredit._vecLevel.push_back(tglevel);
		}
		m_mapOrganCredit[tgOrganCredit._dwOrganizeID]=tgOrganCredit;
	}
	return true;
}

CREDITLEVEL* CCreditSystem::GetLevel(ulong dwValue,vector<CREDITLEVEL>& vecCreditLevel)
{
	size_t dwsize=vecCreditLevel.size();
	if(dwsize==0)
		return NULL;
	long level=0;
	for (size_t i=0;i<dwsize;++i)
	{	
		if (dwValue < vecCreditLevel[i]._dwValue)
		{
			return &vecCreditLevel[i];
		}
		//与该等级声望值相等,则返回下一级
		if (dwValue == vecCreditLevel[i]._dwValue)
		{
			//还未达到最高等级
			if (i+1<dwsize)
			{
				return &vecCreditLevel[i+1];
			}
			//已经是所有等级的最大值
			else
				return &vecCreditLevel[dwsize-1];
		}
	}	
	if (dwValue > vecCreditLevel[dwsize-1]._dwValue)
	{
		return &vecCreditLevel[dwsize-1];
	}

	//超过最大声望值
	return NULL;
}

long CCreditSystem::CreditLevel(ulong dwValue)
{
	
	CREDITLEVEL* tglevel=GetLevel(dwValue,m_vecCreditLevel);
	if (tglevel==NULL)
		return 0;
	return tglevel->_dwLevelID;
}

const char* CCreditSystem::CreditName(ulong dwValue)
{
	//获取区域的最大声望
	long lMaxAreaValue = MaxAreaCredit();
	if((long)dwValue<=lMaxAreaValue)
	{
	CREDITLEVEL* tglevel=GetLevel(dwValue,m_vecCreditLevel);
	if (tglevel==NULL)
		return NULL;
	return tglevel->_cName;
	}else{
		ulong size=(ulong)m_vecCreditLevel.size();
		if(size==0)
			return 0;
		return m_vecCreditLevel[size-1]._cName;
	}
}


CREDITLEVEL* CCreditSystem::GetOrganLevel(ulong dwOrganID,ulong dwValue)
{
	ORGANITR it=m_mapOrganCredit.find(dwOrganID);
	CREDITLEVEL* level=NULL;
	if (it!=m_mapOrganCredit.end())
	{	
		level = GetLevel(dwValue,it->second._vecLevel);	
	}
	return level;
}

long CCreditSystem::MaxOrganValue(ulong dwOrganID,ulong dwLevel)
{
	ORGANITR it=m_mapOrganCredit.find(dwOrganID);
	if (it!=m_mapOrganCredit.end())
	{	
		if (dwLevel>0&&dwLevel <= it->second._vecLevel.size())
		{
			return it->second._vecLevel[dwLevel-1]._dwValue;	
		}	
	}
	return 0;
}


const char* CCreditSystem::CreditOrganName(ulong dwOrganID,ulong dwValue)
{
	//获得该组织声望的最大值
	long lMaxOrganValue = GetMaxCreditByOrgan(dwOrganID);
	if( (long)dwValue <= lMaxOrganValue )
	{
	CREDITLEVEL* tglevel=GetOrganLevel(dwOrganID,dwValue);
	if (tglevel==NULL)
		return NULL;
	return tglevel->_cName;
	}else{
		ORGANITR it=m_mapOrganCredit.find(dwOrganID);
		if (it!=m_mapOrganCredit.end())
		{
			size_t dwSize = it->second._vecLevel.size();
			if(dwSize==0)
				return 0;
			return it->second._vecLevel[dwSize-1]._cName;
		}
		return 0;
	}

}

long CCreditSystem::CreditOrganLevel(ulong dwOrganID,ulong dwValue)
{
	CREDITLEVEL* tglevel=GetOrganLevel(dwOrganID,dwValue);
	if (tglevel==NULL)
		return 0;
	return tglevel->_dwLevelID;
}

long CCreditSystem::CreditOrganLevelByName(const char* strName,ulong dwValue)
{
	long id=GetOrganID(strName);
	if (id==0)
		return 0;
	return CreditOrganLevel(id,dwValue);

}

long CCreditSystem::CreditOrganLeveyByDirtyName(const char* strDirtyName,ulong dwValue)
{
	long id=GetOrganIDByDirtyName(strDirtyName);
	if (id==0)
		return 0;
	return CreditOrganLevel(id,dwValue);
}

long CCreditSystem::GetOrganID(const char* Name)
{
	ORGANITR it=m_mapOrganCredit.begin();
	for (;it!=m_mapOrganCredit.end();++it)
	{
		if (0 == stricmp(it->second._cName,Name))
		{
			return it->first;
		}
	}
	return 0;
}

long CCreditSystem::GetOrganIDByDirtyName(const char* DirtyName)
{
	ORGANITR it=m_mapOrganCredit.begin();
	for (;it!=m_mapOrganCredit.end();++it)
	{
		if (0 == stricmp(it->second._cDirtyName,DirtyName))
		{
			return it->first;
		}
	}
	return 0;
}


long CCreditSystem::MaxAreaCredit()
{
	ulong size=(ulong)m_vecCreditLevel.size();
	if(size==0)
		return 0;
	return m_vecCreditLevel[size-1]._dwValue;
}

long CCreditSystem::MaxAreaValue(ulong dwLevel)
{
	ulong size=(ulong)m_vecCreditLevel.size();
	if(size==0)
		return 0;

	for(size_t i=0;i<size;i++)
	{
		if(m_vecCreditLevel[i]._dwLevelID==dwLevel)
			return m_vecCreditLevel[i]._dwValue;
	}
	return 0;
}

long CCreditSystem::MaxOrganValue(const char* strName)
{
	ORGANITR it=m_mapOrganCredit.begin();
	for (;it!=m_mapOrganCredit.end();++it)
	{
		if (0 == stricmp(it->second._cName,strName))
		{
			vector<CREDITLEVEL>& vecLevel=it->second._vecLevel;
			//越界判定
			if (vecLevel.size()>0)
			{
				return vecLevel[vecLevel.size()-1]._dwValue;
			}
		}
	}
	return 0;
	
}

long CCreditSystem::MaxOrganValByDirtyName(const char* strDirtyName)
{
	ORGANITR it=m_mapOrganCredit.begin();
	for (;it!=m_mapOrganCredit.end();++it)
	{
		if (0 == stricmp(it->second._cDirtyName,strDirtyName))
		{
			vector<CREDITLEVEL>& vecLevel=it->second._vecLevel;
			//越界判定
			if (vecLevel.size()>0)
			{
				return vecLevel[vecLevel.size()-1]._dwValue;
			}
		}
	}
	return 0;
}

const char* CCreditSystem::CreditAreaNameByIndex(int index)
{
	map<ulong,AREA>::iterator it=m_mapTradeAreas.begin();
	for (;it!=m_mapTradeAreas.end();++it)
	{
		if(it->second._iIndex == index)
		{
			return it->second._cName;
		}
	}
	return NULL;
}

const char* CCreditSystem::CreditAreaNameByID(ulong dwID)
{
	map<ulong,AREA>::iterator it=m_mapTradeAreas.begin();
	for (;it!=m_mapTradeAreas.end();++it)
	{
		if(it->second._dwAreaID == dwID)
		{
			return it->second._cName;
		}
	}
	return NULL;
}

size_t CCreditSystem::GetCreditAreaSize()
{
	return 	m_mapTradeAreas.size();
}

size_t CCreditSystem::GetCreditOrganSize()
{
	return 	m_mapOrganCredit.size();
}

const char* CCreditSystem::GetDirtyName(ulong dwOrganID)
{
	map<ulong,ORGANCREDIT>::iterator it=m_mapOrganCredit.find(dwOrganID);
	if (it!=m_mapOrganCredit.end())
	{
		return it->second._cDirtyName;
	}
	return NULL;
}

const char* CCreditSystem::GetOrganName(ulong dwOrganID)
{
	map<ulong,ORGANCREDIT>::iterator it=m_mapOrganCredit.find(dwOrganID);
	if (it!=m_mapOrganCredit.end())
	{
		return it->second._cName;
	}
	return NULL;
}

long CCreditSystem::MaxAreaCreditLevel()
{
	ulong size=(ulong)m_vecCreditLevel.size();
	if(size==0)
		return 0;
	return m_vecCreditLevel[size-1]._dwLevelID;
}

long CCreditSystem::GetMaxCreditByOrgan(ulong dwOrganID)
{
	ORGANITR it=m_mapOrganCredit.find(dwOrganID);
	if (it!=m_mapOrganCredit.end())
	{
		size_t dwSize = it->second._vecLevel.size();
		if(dwSize==0)
			return 0;
		return it->second._vecLevel[dwSize-1]._dwValue;
	}
	return 0;
}

long CCreditSystem::GetMaxCreditLevelByOrgan(ulong dwOrganID)
{
	ORGANITR it=m_mapOrganCredit.find(dwOrganID);
	if (it!=m_mapOrganCredit.end())
	{
		size_t dwSize = it->second._vecLevel.size();
		if(dwSize==0)
			return 0;
		return it->second._vecLevel[dwSize-1]._dwLevelID;
	}
	return 0;
}