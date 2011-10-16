#pragma once
#include "afxcmn.h"
#include "afxext.h"


// CCheckFilesDialog 对话框

class CCheckFilesDialog : public CDialog
{
	DECLARE_DYNAMIC(CCheckFilesDialog)

public:
	CCheckFilesDialog(string& strWorkPath,string& strPackageName,string& strFileInfoName,
		CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCheckFilesDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_CHECKALLFILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CProgressCtrl m_Progress;
	CString m_strDisplayInfo;

	string					m_strServerCurDir;								//服务器当前目录
	string					m_strnewVersion;								//服务器新版本

	string					m_strWorkPath;									//工作路径
	string					m_strPackageName;								//包名
	string					m_strFilesInfoName;								//文件信息名字
	CClientResource*		m_pClientResource;								//客户端资源信息
	HINTERNET				m_hUpdateServerConnect;							//和更新服务器的连接
	HINTERNET				m_hUpdateSession;

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


	static void SetDisplayInfo(HWND hwnd,CString strText,int nPos,int nMax=-1);
	static void	SetDisplayInfo(HWND hwnd,int nPos,int nMax=-1);

	static bool	 LoadClientResource(CCheckFilesDialog* pInfoDialog);
	static bool	 LoadCheckServerResource(CCheckFilesDialog* pInfoDialog);
	static bool	 GetUpdateFileInfos(HINTERNET hConnect,string& strPathName,string& strLocalFileName,CFilesInfo* pFilesInfo);
	static bool  CheckGameVersion(string& strOldVersion,string& strNewVerion,string& strRet);

	static bool	 DownLoadFiles(HINTERNET hConnect,tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,string& strFtpPath,CClientResource* pResource,
		HWND hwnd,int& nTotalSize);
	static unsigned __stdcall LoadInfoProc(void* pParam);
	virtual BOOL OnInitDialog();
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnCancel();
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	CBitmapButton m_bnCancel;
};
