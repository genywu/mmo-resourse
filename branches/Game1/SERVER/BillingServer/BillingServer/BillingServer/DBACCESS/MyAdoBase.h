#ifndef _INC_MYADOBASE
#define _INC_MYADOBASE
#pragma once

#import "msado15.dll" no_namespace rename("EOF","adoEOF")
#include <string>
using namespace std;

class CMyAdoBase
{
public:
	// 初始#
	static bool Initialize(string strProvider, 
		string strDataSource, 
		string strInitialCatalog,
		string strUserID, 
		string strPassword, 
		string strConnectTime = string("0"), 
		string strIntegratedSecurity = string("SSPI"));

	// 释放#
	static bool Uninitalize();

	/*
	* Exception
	*/

	// 测试函数 如有异常则抛出#
	static void TESTHR (bool _b) { if(!_b) _com_issue_error (E_FAIL);} 
	static void TESTHR (HRESULT _hr){ if(FAILED(_hr)) _com_issue_error (E_FAIL);}
	static void TESTHR (BOOL _b) {if(!_b) _com_issue_error(E_FAIL);}

public:

	/*
	* Connection object
	*/
//	static _ConnectionPtr m_Connection;
//	static _ConnectionPtr m_cnOpenPlayerBase;
//	static _ConnectionPtr m_cnLoadPlayerData;

	// 创建一个连接对象
	static bool CreateCn(_ConnectionPtr &cn);
	// 释放连接
	static void ReleaseCn(_ConnectionPtr &cn);
	// 打开连接
	static bool OpenCn(_ConnectionPtr &cn);
	// 关闭连接
	static bool CloseCn(_ConnectionPtr &cn);

	static bool ExecuteCn(LPCSTR sql, _ConnectionPtr cn);

	/* Construction */
	CMyAdoBase();

	/* Destruction */
	virtual ~CMyAdoBase(void);

	/*
	 * Recordset object
	 */

	// 打开记录对象
	static bool OpenRs(LPCSTR sql, _RecordsetPtr &rs, _ConnectionPtr cn,
		CursorTypeEnum ct = adOpenForwardOnly,
		LockTypeEnum lt = adLockReadOnly,
		LONG op = adCmdText);
	// 创建记录对象#
	static bool CreateRs(IN _RecordsetPtr &rs);
	// 释放记录对象#
	static bool ReleaseRs(IN _RecordsetPtr &rs);
	// 关闭记录对象
	static bool CloseRs(_RecordsetPtr &rs);
	

	//修正单引号问题
	static char* FixSingleQuotes(char *sstr, char *dstr);


	// 生成一个GUID. Format: "{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}"  40位字符
	static char *GenerateGuidString(OUT char* GuidString);

	static char *GetTimeString(OUT char *szTime);

	static bool HasSingleQuotes(const char *str);
protected:

	// 主数据库连接对象
	//static _ConnectionPtr m_cn;

	// Connection String
	static string m_strConnectionString;


	// 如果连接断开，重新连接。
	//static bool ReConnect(IN _ConnectionPtr cn = NULL);

	/*
	 * Print error
	 */

	// 错误输入#
	static void PrintErr(LPCSTR msg, _com_error &e);
	static void PrintErr(LPCSTR msg);
private:

	/*
	 * Connetion string members
	 */

	static string m_strProvider;
	static string m_strDataSource;
	static string m_strInitialCatalog;
	static string m_strUserID;
	static string m_strPassword;
	static string m_strConnectTimeout;
	static string m_strIntegratedSecurity; // SSPI

#ifdef _DEBUG
	static int rsCounter;
	static int cnCounter;
#endif // _DEBUG
	

};

#endif // _INC_MYADOBASE