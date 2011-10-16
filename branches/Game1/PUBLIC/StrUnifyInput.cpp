#include "StdAfx.h"
#include "StrUnifyInput.h"
#include "public.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "tools.h"
struct tagString;
map<DWORD,tagString*>  CStringReading::m_mapUniFile;
//默认构造函数
CStringReading::CStringReading()
{

}
//默认析构函数
CStringReading::~CStringReading()
{

}
//字符串初始化
bool CStringReading::InitUniString(string strFileName,string strNode,string strType,DWORD dwID)
{	
	if( strFileName.empty())
	{	
		return false;
	}

	CRFile *prfile = rfOpen(strFileName.c_str());
	if(NULL == prfile)	return FALSE;

	TiXmlNode *pNode = NULL;
	TiXmlDocument doc(strFileName.c_str());

	//doc.LoadFile(); 
	if( !doc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		char pszInfo[512]="";
		_snprintf(pszInfo,512,"%s 配置错误!",strFileName.c_str());
		MessageBox(NULL,pszInfo,"",MB_OK);
		rfClose(prfile);
		return FALSE;
	}
	rfClose(prfile);

	//载入技能配置文件
	//doc.LoadData(prfile->GetData(),prfile->GetDatalen());
	//rfClose(prfile);

	//取得第一个节点
	pNode = doc.FirstChild(strNode.c_str());	

	if (NULL == pNode)
	{
		return false;
	}

	TiXmlElement *pElem = pNode->ToElement();

	if (strType!="")
	{
		LoadStringConfigration(pElem,dwID);
	} 
	else
	{
		LoadStringConfigration(pElem,strType,dwID);
	}
	
	return true;

}

DWORD CStringReading::GetDwordAttribute(TiXmlElement *pElem,string strName)
{
	double dValue;
	if(pElem->QueryDoubleAttribute(strName.c_str(),&dValue))
	{
		dValue = 0;
	}
	return dValue;
}
void CStringReading::LoadStringConfigration(TiXmlElement *pElem,DWORD dwID)
{
	map<DWORD,tagString*>::iterator it = m_mapUniFile.find(dwID);
	if (it==m_mapUniFile.end())
	{
		tagString *ptgString = NULL;
		ptgString = new tagString;		
		for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
			pChildElem=pChildElem->NextSiblingElement())
		{
			if (!stricmp(pChildElem->Value(),"String"))
			{
				DWORD dwStrID = GetDwordAttribute(pChildElem,"id");
				string strInput = pChildElem->Attribute("text");
				map<DWORD,string>::iterator it= ptgString->m_mapUniString.find(dwStrID);
				if (it!=ptgString->m_mapUniString.end())
				{
					char strTemp[256];
					_snprintf(strTemp, 256, LoadString(2,1),dwID,dwStrID);
					PutDebugString(strTemp);
				}
				else
				{
					ptgString->m_mapUniString[dwStrID]=strInput;;
				}			
			}
		}	
		m_mapUniFile[dwID]=ptgString;
	}
	else if (it!=m_mapUniFile.end())
	{
		for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
			pChildElem=pChildElem->NextSiblingElement())
		{
			if (!stricmp(pChildElem->Value(),"String"))
			{
				DWORD dwStrID = GetDwordAttribute(pChildElem,"id");
				string strInput = pChildElem->Attribute("text");
				(*it).second->m_mapUniString[dwStrID] = strInput;				
			}
		}			
	}
}
//读取字符串配置文件
void CStringReading::LoadStringConfigration(TiXmlElement *pElem,string strType, DWORD dwID)
{	
	//if (dwID!=0 )
	{	
			for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
				pChildElem=pChildElem->NextSiblingElement())
			{
			
				if (!stricmp(pChildElem->Value(),"FileTable"))
				{
					DWORD dwID = GetDwordAttribute(pChildElem,"typeid");
					string strType = pChildElem->Attribute("name");
					string strFileName = pChildElem->Attribute("file");
					string strNode = "StringTable";
					InitUniString(strFileName,strNode,strType,dwID);
				}
			}		
	}	
}

//通过ID获取字符串
char* CStringReading::LoadString(DWORD dwTypeID,DWORD dwID)
{
	static char pszNoStr[256] = "";
	itUniFile it=m_mapUniFile.find(dwTypeID);
	if (it!=m_mapUniFile.end())
	{
		map<DWORD,string>::iterator sit=(*it).second->m_mapUniString.find(dwID);
		if (sit!=(*it).second->m_mapUniString.end())
		{
			return (char*)(*sit).second.c_str();
		}

	}

	_snprintf(pszNoStr,256,"Don't config text resource type%d,id%d",dwTypeID,dwID);
	return pszNoStr;
}
//根据参数获取对应字符串个数
long CStringReading::GetStringCount(string strType)
{
	long lStringCount=0;
	//for (itUniFile it=m_mapUniFile.begin();
	//	it!=m_mapUniFile.end();++it)
	//{
	//	string str = (*it).second->strTypeName.substr(0,6);
	//	if (str==strType)
	//	{
	//		++lStringCount;			
	//	}
	//}
	return lStringCount;
}
//编码
void CStringReading::AddToByteArray(vector<BYTE>* pByteArray)
{	
	_AddToByteArray(pByteArray,(long)m_mapUniFile.size());

	for (itUniFile it=m_mapUniFile.begin();it!=m_mapUniFile.end();++it)
	{			
			_AddToByteArray(pByteArray,(*it).first);
			
			_AddToByteArray(pByteArray,(long)(*it).second->m_mapUniString.size());
			for (map<DWORD,string>::iterator sit=(*it).second->m_mapUniString.begin();
				sit!=(*it).second->m_mapUniString.end();++sit)
			{
				_AddToByteArray(pByteArray,(*sit).first);
				_AddToByteArray(pByteArray,(*sit).second.c_str());
			}
	}
	
}
//解码
void CStringReading::DecordFromByteArray(BYTE* pByte, long& pos)
{
	long lFCount = _GetLongFromByteArray(pByte,pos);
	tagString *ptgString = NULL;
	for (long lIndex=0; lIndex<lFCount; ++lIndex)
	{
		DWORD dwFileID = _GetDwordFromByteArray(pByte,pos);
		long lStrCount = _GetLongFromByteArray(pByte,pos);
		map<DWORD,tagString*>::iterator it = m_mapUniFile.find(dwFileID);
		if (lStrCount>0 &&  it==m_mapUniFile.end())
		{
			ptgString = new tagString;
			for (long lCount=0; lCount<lStrCount;++lCount)
			{
				char strTemp[102400];
				DWORD dwStrID = _GetDwordFromByteArray(pByte,pos);
				_GetStringFromByteArray(pByte,pos,strTemp);		
				ptgString->m_mapUniString.insert(make_pair(dwStrID,strTemp));
			}
			m_mapUniFile.insert(make_pair(dwFileID,ptgString));	
		}
		else if (lStrCount>0 && it!=m_mapUniFile.end())
		{
			for (long lCount=0; lCount<lStrCount;++lCount)
			{
				char strTemp[102400];
				DWORD dwStrID = _GetDwordFromByteArray(pByte,pos);
				_GetStringFromByteArray(pByte,pos,strTemp);						
				(*it).second->m_mapUniString[dwStrID] = strTemp;
			}			
		}
	}
}
//释放
void CStringReading::Release()
{
	itUniFile it=m_mapUniFile.begin();
	for (;it!=m_mapUniFile.end();++it)
	{
		SAFE_DELETE((*it).second);
	}
	m_mapUniFile.clear();
}