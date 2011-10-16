#include "StdAfx.h"
#include "CreditSystem.h"

CCreditSystem::CCreditSystem(void)
{
}

CCreditSystem::~CCreditSystem(void)
{
}



/*
* 功能: -
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2010.02.03 - lpf
*		错误信息修改为英文
*/
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
		MessageBox(NULL,"Load entrepot config error!","Error",MB_OK);
		rfClose(prFile);
		return FALSE;
	}
	rfClose(prFile);
	pNode=m_Tdoc.FirstChild("TradeArea");
	if (pNode==NULL)
	{
		MessageBox(NULL,"Load entrepot config error!","Error",MB_OK);
		return FALSE;
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
			area._dwAreaID=(DWORD)atoi(pAreaElement->Attribute("ID"));
			const char* strName=pAreaElement->Attribute("NAME");
			strcpy(area._cName,strName);
			m_mapTradeAreas[area._dwAreaID]=area;
		}		
	}
		char Text[100]="";
		sprintf(Text,"AreaNum：%d",m_mapTradeAreas.size());
        PutDebugString(Text);

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
			tglevel._dwLevelID=(DWORD)atoi(pCreditElement->Attribute("ID"));
			tglevel._dwValue=(DWORD)atoi(pCreditElement->Attribute("VAL"));
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
	dbWriteSet.AddToByteArray((DWORD)m_mapTradeAreas.size());
	map<DWORD,AREA>::iterator it=m_mapTradeAreas.begin();
	for (;it!=m_mapTradeAreas.end();++it)
	{
		dbWriteSet.AddToByteArray(&(it->second),sizeof(AREA));
	}

	dbWriteSet.AddToByteArray((DWORD)m_vecCreditLevel.size());
	for (int i=0;i<m_vecCreditLevel.size();i++)
	{
		dbWriteSet.AddToByteArray(&(m_vecCreditLevel[i]),sizeof(CREDITLEVEL));
	}

	dbWriteSet.AddToByteArray((DWORD)m_mapOrganCredit.size());
	ORGANITR itOragn=m_mapOrganCredit.begin();
	for (;itOragn!=m_mapOrganCredit.end();++itOragn)
	{
		dbWriteSet.AddToByteArray(itOragn->second._dwOrganizeID);
		dbWriteSet.AddToByteArray(itOragn->second._cName);
		dbWriteSet.AddToByteArray(itOragn->second._cDirtyName);
		dbWriteSet.AddToByteArray((DWORD)itOragn->second._vecLevel.size());
		for (int i=0;i<itOragn->second._vecLevel.size();i++)
		{
			dbWriteSet.AddToByteArray(&(itOragn->second._vecLevel[i]),sizeof(CREDITLEVEL));
		}
	}
	return true;
}

bool CCreditSystem::DecodeToByteArray(DBReadSet& dbReadSet)
{
	DWORD dwsize=dbReadSet.GetDwordFromByteArray();	
	AREA area;
	for (int i=0;i<dwsize;++i)
	{
		dbReadSet.GetBufferFromByteArray(&area,sizeof(AREA));
		m_mapTradeAreas[area._dwAreaID]=area;
	}


	dwsize=dbReadSet.GetDwordFromByteArray();
	CREDITLEVEL tglevel;
	for (int i=0;i<dwsize;++i)
	{	
		dbReadSet.GetBufferFromByteArray(&tglevel,sizeof(CREDITLEVEL));
		m_vecCreditLevel.push_back(tglevel);
	}
	
	
	dwsize=dbReadSet.GetDwordFromByteArray();
	ORGANCREDIT tgOrganCredit;
	for (int i=0;i<dwsize;++i)
	{
		tgOrganCredit._vecLevel.clear();

		tgOrganCredit._dwOrganizeID=dbReadSet.GetDwordFromByteArray();
		dbReadSet.GetStringFromByteArray(tgOrganCredit._cName,MAXAREANAMESIZE);
		dbReadSet.GetStringFromByteArray(tgOrganCredit._cDirtyName,MAXAREANAMESIZE);
		DWORD vecsize=dbReadSet.GetDwordFromByteArray();
		for (int j=0;j<vecsize;++j)
		{
			dbReadSet.GetBufferFromByteArray(&tglevel,sizeof(CREDITLEVEL));
			tgOrganCredit._vecLevel.push_back(tglevel);
		}
		m_mapOrganCredit[tgOrganCredit._dwOrganizeID]=tgOrganCredit;
	}
	return true;
}

CREDITLEVEL* CCreditSystem::GetLevel(DWORD dwValue,vector<CREDITLEVEL>& vecCreditLevel)
{
	DWORD dwsize=vecCreditLevel.size();
	if(dwsize==0)
		return NULL;
	long level=0;
	for (int i=0;i<dwsize;++i)
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

long CCreditSystem::CreditLevel(DWORD dwValue)
{
	
	CREDITLEVEL* tglevel=GetLevel(dwValue,m_vecCreditLevel);
	if (tglevel==NULL)
		return 0;
	return tglevel->_dwLevelID;
}

const char* CCreditSystem::CreditName(DWORD dwValue)
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
		DWORD size=(DWORD)m_vecCreditLevel.size();
		if(size==0)
			return 0;
		return m_vecCreditLevel[size-1]._cName;
	}
}


CREDITLEVEL* CCreditSystem::GetOrganLevel(DWORD dwOrganID,DWORD dwValue)
{
	ORGANITR it=m_mapOrganCredit.find(dwOrganID);
	CREDITLEVEL* level=NULL;
	if (it!=m_mapOrganCredit.end())
	{	
		level = GetLevel(dwValue,it->second._vecLevel);	
	}
	return level;
}

long CCreditSystem::MaxOrganValue(DWORD dwOrganID,DWORD dwLevel)
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


const char* CCreditSystem::CreditOrganName(DWORD dwOrganID,DWORD dwValue)
{
	//获得该组织声望的最大值
	long lMaxOrganValue = GetMaxCreditByOrgan(dwOrganID);
	if(dwValue<=lMaxOrganValue)
	{
	CREDITLEVEL* tglevel=GetOrganLevel(dwOrganID,dwValue);
	if (tglevel==NULL)
		return NULL;
	return tglevel->_cName;
	}else{
		ORGANITR it=m_mapOrganCredit.find(dwOrganID);
		if (it!=m_mapOrganCredit.end())
		{
			DWORD dwSize = it->second._vecLevel.size();
			if(dwSize==0)
				return 0;
			return it->second._vecLevel[dwSize-1]._cName;
		}
		return 0;
	}

}

long CCreditSystem::CreditOrganLevel(DWORD dwOrganID,DWORD dwValue)
{
	CREDITLEVEL* tglevel=GetOrganLevel(dwOrganID,dwValue);
	if (tglevel==NULL)
		return 0;
	return tglevel->_dwLevelID;
}

long CCreditSystem::CreditOrganLevelByName(const char* strName,DWORD dwValue)
{
	long id=GetOrganID(strName);
	if (id==0)
		return 0;
	return CreditOrganLevel(id,dwValue);

}

long CCreditSystem::CreditOrganLeveyByDirtyName(const char* strDirtyName,DWORD dwValue)
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
	DWORD size=(DWORD)m_vecCreditLevel.size();
	if(size==0)
		return 0;
	return m_vecCreditLevel[size-1]._dwValue;
}

long CCreditSystem::MaxAreaValue(DWORD dwLevel)
{
	DWORD size=(DWORD)m_vecCreditLevel.size();
	if(size==0)
		return 0;

	for(int i=0;i<size;i++)
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
	map<DWORD,AREA>::iterator it=m_mapTradeAreas.begin();
	for (;it!=m_mapTradeAreas.end();++it)
	{
		if(it->second._iIndex == index)
		{
			return it->second._cName;
		}
	}
	return NULL;
}

const char* CCreditSystem::CreditAreaNameByID(DWORD dwID)
{
	map<DWORD,AREA>::iterator it=m_mapTradeAreas.begin();
	for (;it!=m_mapTradeAreas.end();++it)
	{
		if(it->second._dwAreaID == dwID)
		{
			return it->second._cName;
		}
	}
	return NULL;
}

int CCreditSystem::GetCreditAreaSize()
{
	return 	m_mapTradeAreas.size();
}

int CCreditSystem::GetCreditOrganSize()
{
	return 	m_mapOrganCredit.size();
}

const char* CCreditSystem::GetDirtyName(DWORD dwOrganID)
{
	map<DWORD,ORGANCREDIT>::iterator it=m_mapOrganCredit.find(dwOrganID);
	if (it!=m_mapOrganCredit.end())
	{
		return it->second._cDirtyName;
	}
	return NULL;
}

const char* CCreditSystem::GetOrganName(DWORD dwOrganID)
{
	map<DWORD,ORGANCREDIT>::iterator it=m_mapOrganCredit.find(dwOrganID);
	if (it!=m_mapOrganCredit.end())
	{
		return it->second._cName;
	}
	return NULL;
}

long CCreditSystem::MaxAreaCreditLevel()
{
	DWORD size=(DWORD)m_vecCreditLevel.size();
	if(size==0)
		return 0;
	return m_vecCreditLevel[size-1]._dwLevelID;
}

long CCreditSystem::GetMaxCreditByOrgan(DWORD dwOrganID)
{
	ORGANITR it=m_mapOrganCredit.find(dwOrganID);
	if (it!=m_mapOrganCredit.end())
	{
		DWORD dwSize = it->second._vecLevel.size();
		if(dwSize==0)
			return 0;
		return it->second._vecLevel[dwSize-1]._dwValue;
	}
	return 0;
}

long CCreditSystem::GetMaxCreditLevelByOrgan(DWORD dwOrganID)
{
	ORGANITR it=m_mapOrganCredit.find(dwOrganID);
	if (it!=m_mapOrganCredit.end())
	{
		DWORD dwSize = it->second._vecLevel.size();
		if(dwSize==0)
			return 0;
		return it->second._vecLevel[dwSize-1]._dwLevelID;
	}
	return 0;
}