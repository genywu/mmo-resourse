#pragma once
#include "afxwin.h"
#include "hintinfo.h"
#include <string>
#include <fstream>
#include <list>
#include "afxext.h"
#include "afxcmn.h"
#include "explorer1.h"
#include "SetupServer.h"
#include "WebUIController.h"
#include "../../PUBLIC/ClientResource.h"
#include "../../PUBLIC/Package.h"
#include "../../PUBLIC/FilesInfo.h"
#include "Interface.h"

using namespace std;

#ifndef  _Version_1_0
#define _Version_1_0
#endif

// CSetupDialog 对话框
class CSetupDialog : public CDialog
{
	DECLARE_DYNAMIC(CSetupDialog)

public:
	CSetupDialog(string& strWorkPath,string& strPackageName,string& strFileInfoName,
		CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetupDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	string		m_strWorkPath;									//工作路径
	string		m_strPackageName;								//包名
	string		m_strFilesInfoName;								//文件信息名字

	string		m_strwebAddress;
//	string		m_strwebAddressEx;
	//string		m_strwebAddressNote;
	string		m_strwebLogoAddress;
	string      m_strwebLink[7];
	CHintinfo	m_webHintInfo;
	//CHintinfo	m_webHintInfo2;
	CClientResource*		m_pClientResource;	

	//CComboBox m_ServerList;
	CBitmapButton m_bnPlayGame;
	CBitmapButton m_bnSysSet;
	CBitmapButton m_bnCheckAllFile;
	CBitmapButton m_bnQuit;
	//CBitmapButton m_bnCheckSysCon;
	/*CBitmapButton m_bnRegister; */
	CBitmapButton m_bnSelectUpdate;  
    CBitmapButton m_bnLoading;
//	CBitmapButton m_bnExit;

   // CBitmapButton m_bnUpBoard;
	//CBitmapButton m_bnLeftBoard;
	//CBitmapButton m_bnDownBoard;
	//CBitmapButton m_bnRightBoard;
	//CBitmapButton m_bnDownBoardEX;

	CBitmapButton m_bnFirstPage; 
	CBitmapButton m_bnNewPlayer; 
	CBitmapButton m_bnGameInfo; 
	CBitmapButton m_bnForum; 
	CBitmapButton m_bnRegist; 
	CBitmapButton m_bnPayment; 
	CBitmapButton m_bnServe; 

    CBitmapButton m_bnServerWire;
    CBitmapButton m_bnServerNet;
	CBitmapButton m_bnServerChoose;
	CBitmapButton m_bnServerChoose2;

	CExplorer1 m_explorer;
	CExplorer1 m_explorerNote;

	CBitmapButton m_bnNEW; 
	CBitmapButton m_bnZhangHao; 
	CBitmapButton m_bnDownLoad; 
	CBitmapButton m_bnKeFu; 
	CBitmapButton m_bnLunTan; 

	// 按图构造对话框形状
	BOOL SetupRgn(CDC* pDC);

	// 进行图形设置的开关变量
	BOOL m_bSetupRgn;

	HICON m_hIconBig;
	HICON m_hIconMin;

    CWebUIController m_webUICtrl;

	/*SetupServer m_SetupServerPage;*/

public:
	bool m_bReadError;														//读取客户端资源是否有错

	void IniHintWeb(void);

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnCbnSelchangeComboServerlist();
	afx_msg void OnBnClickedButtonSysset();
	afx_msg void OnBnClickedButtonCheckallfile();
	afx_msg void OnBnClickedButtonLogin();
//	afx_msg void OnBnClickedButtonChecksyscon();
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	CProgressCtrl m_Progress;
	// 显示信息
	CString m_strDisplayInfo; 
	static CString m_strDisplayInfoEx;
	SetupServer m_SetupServerPage;

protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnCancel();
public:
	bool	m_bIDOK;						//是否完成了
	static int m_Sever;   //0表示选择电信服务器，1表示网通服务器
	CClientResource* GetClientResource(){return m_pClientResource;}
	string&			 GetFilesInfoName()						{return m_strFilesInfoName;	}
	string&			 GetWorkPath()							{return m_strWorkPath;}
	int              LoadSdoLoad();
	void             UnLoadSdoLoad();
	//设置进度信息
	void	OnUpdateDisplay();							//更新显示
	static void SetDisplayInfo(HWND hwnd,CString strText,int nPos,int nMax=-1);
	static void	SetDisplayInfo(HWND hwnd,int nPos,int nMax=-1);
	static bool	 LoadClientResource(CSetupDialog* pSetupDialog);
	static bool  LoadUpdateServerResource(CSetupDialog* pSetupDialog);
	static bool	 GetServerVersionFromOneGameServer(CSetupDialog* pSetupDialog,HINTERNET hSession,
									string& strUrl1,string& strServerName,
									HINTERNET& hUpdateServerConnect,DWORD& dwFileInfoCRC,string& strVersion);
	static bool	 GetServerVersion(HINTERNET hConnect,string& strPathName,string& strLocalVersionName,
									DWORD& dwFileInfoCRC,string& strVersion);
	static bool  GetUpdateFileInfosEx(HINTERNET hConnect,string& strPathName,string& strLocalFileName,
		                              DWORD& dwFileInfoCRC,string &ServerFilesName);

	static unsigned __stdcall LoadInfoProc(void* pParam);
	static unsigned __stdcall LoadVersion(void* pParam);
	static bool  CheckVersion(CSetupDialog* pSetupDialog);

	static BOOL  UpdateClientResource(tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,BYTE*& pBuf,DWORD& dwBuflen,
							CPackage* pPackage,CClientResource* pClientResource,HWND hwnd,int& nTotalSize);
	static BOOL  DownloadFile(const CString& strFileURLInServer, //待下载文件的URL
    const CString & strFileLocalFullPath,HWND hwnd,bool bContinue=true);//存放到本地的路径  继续下载默认打开

	static bool  FindServerIP(CSetupDialog* pSetupDialog,string strFolder,string &strIP,string &strPath);

	bool  ChackPackage();    //检查更新包中是否有LUNCHER.EXE的更新


	static unsigned __stdcall UpdateProc(void* pParam);
	BOOL	GetPatchData();						//得到升级包数据
	BOOL InitialInfo();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonNewplayer();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonGameinfo();
	afx_msg void OnBnClickedButtonForum();
	afx_msg void OnBnClickedButtonRegist();
	afx_msg void OnBnClickedButtonPayment();
	afx_msg void OnBnClickedButtonServe();
	afx_msg void OnBnClickedButtonFirstpage();

	afx_msg void OnBnClickedButtonCacel();
public:
	afx_msg void OnBnClickedButtonWire();
	afx_msg void OnBnClickedButtonNet();
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonZhanghao();
	afx_msg void OnBnClickedButtonDownload();
	afx_msg void OnBnClickedButtonKefu();
	afx_msg void OnBnClickedButtonLuntan();
	afx_msg void OnDestroy();
public:
	CFilesInfo*			m_pPatchFilesInfo;		//升级的文件信息
	CPackage*			m_pPatchPackage;		//升级的包数据
	CClientResource*	m_pPatchClientResource;		//客户端资源

	string				m_strPatchWorkPath;
	string				m_strExePath;			//执行文件目录
	string				m_strPatchFilesInfoName;		//文件信息名字
	static string       m_strPathName;
	string				m_listStrUpdatedVersion;	//被升级的版本号
	string		m_strFilesInfoNameEx;								//文件信息名字

	string				m_strTemptPackageName;	//临时包名
	string				m_strTemptFilesInfoName;//临时文件姓名

	string				m_strUpdatedVersion;	//被升级的版本号

	void LoadBitmapFromFile(CBitmapButton &button,LPCTSTR lpszFilenameNor,LPCTSTR lpszFilenamefoc,LPCTSTR lpszFilenamesel);     //从外部资源读取按钮的BITMAP
	void InitButtonPos(const char* Name,CBitmapButton &button);          //根据配置设置所有按纽的位置
	void InitInterface();      //初始化界面所有BUTTON的显示，位置信息

private:
	CBitmap *m_pBitmapbk;
	CInterfaceXml m_Interface;
	map<string,RECT> m_mapButtonRect;
	RECT c_rectLoading;      //记录下进度条的大小用于绘制进度条
};
