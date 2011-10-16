//********************************************************************************
//	created:	2004/30/12 14:21:01
//	file name: 	e:\Program\test\MyAdoBase.cpp
//	file path:	e:\Program\test
//	file base:	MyAdoBase
//	file ext:	cpp
//	author:		
//
//	purpose:	静态ADO类，只要是简化ADO操作。
//********************************************************************************
#include "StdAfx.h"
#include "MyAdoBase.h"
#include "../../Appframe/AppFrameDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef _DEBUG
int CMyAdoBase::rsCounter = 0;
int CMyAdoBase::cnCounter = 0;
#endif // _DEBUG



/* 定义静态成员变量 */
//_ConnectionPtr CMyAdoBase::m_Connection = NULL;
//_ConnectionPtr CMyAdoBase::m_cnOpenPlayerBase = NULL;
//_ConnectionPtr CMyAdoBase::m_cnLoadPlayerData = NULL;

string CMyAdoBase::m_strConnectionString;

string CMyAdoBase::m_strProvider;
string CMyAdoBase::m_strDataSource;
string CMyAdoBase::m_strInitialCatalog;
string CMyAdoBase::m_strUserID;
string CMyAdoBase::m_strPassword;
string CMyAdoBase::m_strConnectTimeout;		// default 0
string CMyAdoBase::m_strIntegratedSecurity; // default SSPI

/* 定义静态成员函数 */
//com初始化和连接对象的初始化
bool CMyAdoBase::Initialize(string strProvider, string strDataSource, string strInitialCatalog,
							string strUserID, string strPassword, string strConnectTime, 
							string strIntegratedSecurity)
{
	m_strProvider			= strProvider;
	m_strDataSource			= strDataSource;
	m_strInitialCatalog		= strInitialCatalog;
	m_strUserID				= strUserID;
	m_strPassword			= strPassword;
	m_strConnectTimeout 	= strConnectTime;
	m_strIntegratedSecurity	= strIntegratedSecurity;

	// 构造Connection String
	m_strConnectionString.clear();
	//	m_strConnectionString = "Driver={SQL Server};Server=192.168.0.90,1433;Database=Bugs_GameDB;UID=sa;PWD=123";



	m_strConnectionString = "Provider=" + m_strProvider +
		"; Server=" + m_strDataSource + 
		"; Database=" + m_strInitialCatalog + 
		"; UID=" + m_strUserID + 
		"; PWD=" + m_strPassword + 
		"; OLE DB Services=-1; Driver={SQL Server}";

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

//	CreateCn(m_Connection);
//	CreateCn(m_cnOpenPlayerBase);
//	CreateCn(m_cnLoadPlayerData);


//	OpenCn(m_Connection);
//	OpenCn(m_cnOpenPlayerBase);
//	OpenCn(m_cnLoadPlayerData);

	return true;
}

bool CMyAdoBase::Initialize(void)
{
	m_strProvider			= "";			
	m_strDataSource			= "";		
	m_strInitialCatalog		= "";
	m_strUserID				= "";		
	m_strPassword			= "";		
	m_strConnectTimeout		= "";	
	m_strIntegratedSecurity = "";

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	return true;
}

//释放数据库连接
bool CMyAdoBase::Uninitalize()
{
//	CloseCn(m_Connection);
//	CloseCn(m_cnOpenPlayerBase);
//	CloseCn(m_cnLoadPlayerData);

//	ReleaseCn(m_Connection);
//	ReleaseCn(m_cnOpenPlayerBase);
//	ReleaseCn(m_cnLoadPlayerData);

	CoUninitialize();
	return true;
}

//--------------------------------------------------------------------------------
//  Function:   void CMyAdoBase::PrintErr(LPCSTR msg, _com_error &e)
//  Parameters: 
//      LPCSTR msg				消息
//		_com_error &e			错误对象
//  Return:     
//
//  Purpose:    
//  [2004/12/30]    
//--------------------------------------------------------------------------------
void CMyAdoBase::PrintErr(LPCSTR msg, _com_error &e)
{
	char buf[1024];
	string strErr="";

	if (e.Error() == 0x80004005)
	{
		wsprintf( buf, "%s (一般性网络错误): 描述\r\n:%s", msg, (LPCSTR)((_bstr_t)e.Description()));
	}
	else
	{
		wsprintf( buf, "%s\r\n \
				   Code = %08lx\r\n \
				   Cnode meaning = %d\r\n \
				   Source = %s\r\n \
				   Description = %s\r\n",
				   msg ,e.Error(), e.ErrorMessage(), (LPCSTR)((_bstr_t)e.Source()), (LPCSTR)((_bstr_t)e.Description()));
		strErr=buf;
		strErr=strErr.substr(30);
	}

#ifdef USE_SCC_LOG_TOOLS
	LogTools::OutErrLog(buf);
#else
   Log4c::Warn(ROOT_MODULE,"%s", buf);
#endif
	
}

void CMyAdoBase::PrintErr(LPCSTR msg)
{
#ifdef USE_SCC_LOG_TOOLS
	LogTools::OutErrLog("Connetcion Executing : %s",msg);
#else
   Log4c::Warn(ROOT_MODULE, "Connetcion Executing : %s",msg);
#endif
}

//--------------------------------------------------------------------------------
//  Function:   bool CMyAdoBase::CreateRs(IN _RecordsetPtr rs)
//  Parameters: 
//      IN _RecordsetPtr rs 
//  Return:     
//		bool
//  Purpose:	创建一个_RecordSetPtr指针到rs    
//  [2004/12/30]    
//--------------------------------------------------------------------------------
long CMyAdoBase::CreateRs(IN _RecordsetPtr &rs)
{
	return rs.CreateInstance(__uuidof(Recordset));
}

//--------------------------------------------------------------------------------
//  Function:   bool CMyAdoBase::ReleaseRs(IN _RecordsetPtr rs)
//  Parameters: 
//      IN _RecordsetPtr rs
//  Return:     
//		bool
//  Purpose:    
//  [2004/12/30]    释放指定rs指针
//--------------------------------------------------------------------------------
long CMyAdoBase::ReleaseRs(IN _RecordsetPtr &rs)
{
	HRESULT hRet = CloseRs(rs);
	if(hRet == S_OK)
	{
		if(rs) rs.Release();
	}
	return hRet;
}



/* 定义构造函数 */
CMyAdoBase::CMyAdoBase()
{
}

/* 定义析构函数 */
CMyAdoBase::~CMyAdoBase(void)
{
}


//--------------------------------------------------------------------------------
//  Function:   bool CMyAdoBase::OpenCn()
//  Parameters: 
//      
//  Return:     
//		bool
//  Purpose:    打开连接（调用）
//  [2004/12/30]    
//--------------------------------------------------------------------------------
long CMyAdoBase::OpenCn(_ConnectionPtr &cn)
{
	return cn->Open(m_strConnectionString.c_str(), "", "", adConnectUnspecified);
}

long CMyAdoBase::OpenCn(_ConnectionPtr &cn, string strSQL)
{
	return cn->Open(strSQL.c_str(), "", "", adConnectUnspecified);
}

//--------------------------------------------------------------------------------
//  Function:   bool CMyAdoBase::CloseCn(_ConnectionPtr &cn)
//  Parameters: 
//      _ConnectionPtr &cn
//  Return:     
//		bool
//  Purpose:    
//  [2004/12/30]    
//--------------------------------------------------------------------------------
long CMyAdoBase::CloseCn(_ConnectionPtr &cn)
{
	return cn->Close();
}

//--------------------------------------------------------------------------------
//  Function:   
//  Parameters: 
//      
//  Return:     
//
//  Purpose:    
//  [2005/1/13]    
//--------------------------------------------------------------------------------
long CMyAdoBase::ExecuteCn(LPCSTR sql, _ConnectionPtr& cn)
{
	return cn->Execute(sql, NULL, adCmdText);
}

//--------------------------------------------------------------------------------
//  Function:   
//  Parameters: 
//      
//  Return:     
//
//  Purpose:    
//  [2005/1/13]    
//--------------------------------------------------------------------------------
//bool CMyAdoBase::ReConnect(IN _ConnectionPtr& cn)
//{
//	try
//	{
//		if(cn->GetState() == adStateClosed)
//		{
//			if(!OpenCn(cn)) return false;
//		}
//		return true;
//	}
//	catch(_com_error &e)
//	{
//		PrintErr("CMyAdoBase ReConnection Failed", e);
//		return false;
//	}
//}


//--------------------------------------------------------------------------------
//  Function:   bool CMyAdoBase::OpenRs()
//  Parameters: 
//      
//  Return:     
//		bool
//  Purpose:    打开连接（调用）
//  [2004/12/30]    
//--------------------------------------------------------------------------------
long CMyAdoBase::OpenRs(LPCSTR sql, _RecordsetPtr &rs, _ConnectionPtr& cn, 
						CursorTypeEnum ct, LockTypeEnum lt, long op)
{
	return rs->Open(_bstr_t(sql), cn.GetInterfacePtr(), ct, lt, op);
}


//--------------------------------------------------------------------------------
//  Function:   bool CMyAdoBase::CloseCn(_ConnectionPtr &cn)
//  Parameters: 
//      _ConnectionPtr &cn
//  Return:     
//		bool
//  Purpose:    
//  [2004/12/30]    
//--------------------------------------------------------------------------------
long CMyAdoBase::CloseRs(_RecordsetPtr &rs)
{
	return rs->Close();
}


long CMyAdoBase::CreateCn(_ConnectionPtr &cn)
{
	return cn.CreateInstance(__uuidof(Connection));
}


long CMyAdoBase::ReleaseCn(_ConnectionPtr &cn)
{
	HRESULT hRet = CloseCn(cn);
	if(hRet == S_OK)
	{
		if(cn) cn.Release();
	}
	return hRet;
}


//修正单引号问题
char* CMyAdoBase::FixSingleQuotes(char *sstr,char *dstr)
{
	char *tmp = dstr;
	while( *sstr )
	{
		if(*sstr == '\'')
		{
			*tmp = '\'';
			tmp++;
		}
		*tmp = *sstr;
		tmp++;
		sstr++;
	}
	*tmp = '\0';
	return dstr;
}

bool CMyAdoBase::HasSingleQuotes(const char *str)
{
	while( *str )
	{
		if( *str == '\'' )
		{
			return true;
		}
		str++;
	}
	return false;
}

//--------------------------------------------------------------------------------
// Function: GenerateGuidString(OUT char* GuidString);
// Parameters: 出口参数 GuidString
//
// Return:
// true
// false
// Purpose: GoodsPropertyID string = 40 (included '\0')
// Guid format:"{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}"
// [2004/12/21]
//--------------------------------------------------------------------------------
char* CMyAdoBase::GenerateGuidString(OUT char* GuidString)
{
	//if(FAILED(::CoInitialize(NULL))) return false;
	GUID guid;
	if(FAILED(CoCreateGuid(&guid))) return false;
	sprintf(GuidString,"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3, guid.Data4[0],guid.Data4[1],
		guid.Data4[2],guid.Data4[3],guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]);
	//::CoUninitialize();
	return GuidString;
}

char* CMyAdoBase::GetTimeString(OUT char *szTime)
{		
	SYSTEMTIME Time;
	GetLocalTime(&Time);
	sprintf(szTime,"%d-%d-%d %d:%d:%d",Time.wYear,Time.wMonth,Time.wDay,Time.wHour,Time.wMinute,Time.wSecond);
	return szTime;
}