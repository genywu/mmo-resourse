#pragma once
#include "afxcmn.h"
#include "../../PUBLIC/ClientResource.h"
#include "afxext.h"
#include "hintinfo.h"

#include <string>
#include <fstream>
#include <list>
#include <vector>
#include "afxwin.h"
using namespace std;

// CInitInfoDialog 对话框

class CInitInfoDialog : public CDialog
{
	DECLARE_DYNAMIC(CInitInfoDialog)

public:
	CInitInfoDialog(string& strWorkPath,string& strPackageName,string& strFileInfoName,
		CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInitInfoDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_INITIALIFNO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CClientResource* GetClientResource()					{return m_pClientResource;}
	string&			 GetFilesInfoName()						{return m_strFilesInfoName;	}
	string&			 GetWorkPath()							{return m_strWorkPath;}
	void	OnUpdateDisplay();							//更新显示
	void	SetUpdateServerSessionAndConnect(HINTERNET hSession,HINTERNET hNet)
	{m_hUpdateSession = hSession;m_hUpdateServerConnect=hNet;}
	HINTERNET GetUpdateServerConnect()				{return m_hUpdateServerConnect;	}
	string&   GetFtpServerDire()					{return m_strServerCurDir;}
	string&	  GetNewVersion()						{return m_strnewVersion;}
	void	  SetNewVerion(string& strNewVerions)	{m_strnewVersion=strNewVerions;	}
	void	  Release();
	

	static void SetDisplayInfo(HWND hwnd,CString strText,int nPos,int nMax=-1);
	static void	SetDisplayInfo(HWND hwnd,int nPos,int nMax=-1);

	static bool	 LoadClientResource(CInitInfoDialog* pInfoDialog);
    static bool  LoadUpdateServerResource(CInitInfoDialog* pInfoDialog);
	static bool	 GetServerVersionFromOneGameServer(CInitInfoDialog* pInfoDialog,HINTERNET hSession,
									string& strUrl1,string& strServerName,
									HINTERNET& hUpdateServerConnect,DWORD& dwFileInfoCRC,string& strVersion);
	static bool	 GetServerVersion(HINTERNET hConnect,string& strPathName,string& strLocalVersionName,
									DWORD& dwFileInfoCRC,string& strVersion);
	//static bool	 GetUpdateFileInfos(HINTERNET hConnect,string& strPathName,string& strLocalFileName,
	//								DWORD dwFileInfoCRC,CFilesInfo* pFilesInfo);
	static bool  GetUpdateFileInfosEx(HINTERNET hConnect,string& strPathName,string& strLocalFileName,
		                              string& dwFileInfoCRC,string &ServerFilesName);
	//static bool  CheckGameVersion(string& strOldVersion,string& strNewVerion,string& strRet);

	//static bool	 DownLoadFiles(HINTERNET hConnect,tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,
	//							string& strFtpPath,CClientResource* pResource,HWND hwnd,int& nTotalSize);

	//static bool	 DownLoadFilesEx(HINTERNET hConnect,CPackage* pPckage,BYTE*& pBuf,DWORD& dwBufLen,
	//							tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,
	//							string& strFtpPath,CClientResource* pResource,
	//							int& nTotalSize,HWND hwnd);

	//续传文件
	//static bool ContinueDownLoadOneFile(HINTERNET hConnect,CPackage* pPckage,BYTE* pBuf,DWORD& dwBufLen,
	//							CFilesInfo* pFilesInfo,
	//							string& strFtpPath,CClientResource* pResource,
	//							int& nTotalSize,HWND hwnd);
	//续传文件
	static bool ContinueDownLoadEx(HINTERNET hConnect,string strOldVersion,BYTE* pBuf,DWORD& dwBufLen,
		                        string& strFtpPath,int& nTotalSize,HWND hwnd);
	//下载一个exe更新文件
	static bool DownLoadEXEFile(HINTERNET hConnect,string strOldVersion,BYTE* pBuf,DWORD& dwBufLen,
		                        string& strFtpPath,int& nTotalSize,HWND hwnd);

	//static bool	DownLoadOneFile(HINTERNET hConnect,CPackage* pPckage,BYTE* pBuf,
	//							string& strTemptFileName,string& strServerFilename,
	//							tagFileInfo* ptagFileInfo,
	//							int& nTotalSize,HWND hwnd);

	//static bool DownloadFile(HINTERNET hHttpFile,CPackage* pPckage,BYTE* pBuf,
	//						string& strTemptFileName,
	//						DWORD dwFileSize,DWORD dwCount,
	//						int& nTotalSize,HWND hwnd);
	static unsigned __stdcall LoadInfoProc(void* pParam);
public:
	bool	m_bReadError;					//读取资源是否出错

	bool	m_bIDOK;						//是否完成了
private:
	// 操作进度
	CProgressCtrl m_Progress;
	// 显示信息
	CString m_strDisplayInfo;
	string					m_strServerCurDir;								//服务器当前目录
	string					m_strnewVersion;								//服务器新版本

	string					m_strWorkPath;									//工作路径
	string					m_strPackageName;								//包名
	string					m_strFilesInfoName;								//文件信息名字
	CClientResource*		m_pClientResource;								//客户端资源信息
	HINTERNET				m_hUpdateServerConnect;							//和更新服务器的连接
	HINTERNET				m_hUpdateSession;
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();

	void IniDownloadHintWeb(void);

protected:
	virtual void OnOK();
public:
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
private:
	CButton m_bnCancel;
	string	m_strDownloadHitAddress;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	CHintinfo m_DownloadHintInfo;
};
