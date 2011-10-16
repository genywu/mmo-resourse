// ServerUpdateDlg.h : 头文件
//

#pragma once

#include "../../PUBLIC/ClientResource.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "wininet.h"

// CServerUpdateDlg 对话框
class CServerUpdateDlg : public CDialog
{
// 构造
public:
	CServerUpdateDlg(string& strCurDirectory,string& strServerAdress,
					string& strServerFolder,string &strFileName,
					string& strServerTitle,CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERVERUPDATE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	void	  Release();
	static void SetProgressMaxLen(HWND hwnd,int nNum);
	static void SetProgressCunPos(HWND hwnd,int nPos);
	static void SetDisplayText(HWND hwnd,CString strInfo);

	static unsigned __stdcall UpdateProc(void* pParam);
	static bool GetLastFileInfo(HINTERNET hSession,string& strCurDirectory,
							string& strUrl,string& strServerFolder,
							HINTERNET& hUpdateConnect,DWORD& dwFileInfoCRC,string& strVersion);
	static bool GetLastVersion(HINTERNET hConnect,string& strPathName,string& strLocalVersionName,
									DWORD& dwFileInfoCRC,string& strVersion);
	static bool GetLastFileInfos(HINTERNET hConnect,string& strPathName,string& strLocalFileName,
									DWORD dwFileInfoCRC,CFilesInfo* pFilesInfo);

	static bool	 DownLoadFilesEx(HINTERNET hConnect,CPackage* pPckage,BYTE*& pBuf,DWORD& dwBufLen,
								tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,
								string& strFtpPath,CClientResource* pResource,
								int& nTotalSize,HWND hwnd);

	static bool	DownLoadOneFile(HINTERNET hConnect,CPackage* pPckage,BYTE* pBuf,
								string& strTemptFileName,string& strServerFilename,
								tagFileInfo* ptagFileInfo,
								int& nTotalSize,HWND hwnd);
	static bool DownloadFile(HINTERNET hHttpFile,CPackage* pPckage,BYTE* pBuf,
							string& strTemptFileName,
							DWORD dwFileSize,DWORD dwCount,
							int& nTotalSize,HWND hwnd);
private:
	string m_strWorkPath;									//工作路径
	string m_strServerAddress;
	string m_strServerFolder;
	string m_strFilesInfoName;								//文件信息名字
	CClientResource* m_pClientResouce;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_UpdateProgress;
	CStatic m_UpdateInfo;
	CButton m_StartServer;
	CButton m_Exit;
protected:
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

public:
	afx_msg void OnBnClickedButtonStartserver();
	afx_msg void OnBnClickedCancel();
};
