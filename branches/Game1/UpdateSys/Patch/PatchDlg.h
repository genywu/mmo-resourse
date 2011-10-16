// PatchDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "outputexplorer.h"
struct tagFileInfo;
class CFilesInfo;
class CPackage;
class CClientResource;

// CPatchDlg 对话框
class CPatchDlg : public CDialog
{
// 构造
public:
	CPatchDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CPatchDlg();

// 对话框数据
	enum { IDD = IDD_PATCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CFilesInfo*			m_pPatchFilesInfo;		//升级的文件信息
	CPackage*			m_pPatchPackage;		//升级的包数据
	CClientResource*	m_pClientResource;		//客户端资源

	string				m_strWorkPath;
	string				m_strExePath;			//执行文件目录
	string				m_strFilesInfoName;		//文件信息名字

	string				m_strTemptPackageName;	//临时包名
	string				m_strTemptFilesInfoName;//临时文件姓名

	string				m_listStrUpdatedVersion;	//被升级的版本号

	BOOL	InitialInfo();						//初始化客户端资源信息
	void	OnUpdateDisplay();					//更新显示
	BOOL	SetPatchData();						//设置升级包数据
	BOOL	GetPatchData();						//得到升级包数据
	//BOOL	GetPatchInfo();						//得到升级包数据
	BOOL	CheckGameRunFolder();				//检查是否是游戏运行目录
	afx_msg void OnBnClickedButtonSelectfolder();



	static unsigned __stdcall UpdateProc(void* pParam);
	static BOOL	 LoadClientResource(CPatchDlg* pPatchDlg);
	static BOOL  UpdateClientResource(tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,BYTE*& pBuf,DWORD& dwBuflen,
							CPackage* pPackage,CClientResource* pClientResource,HWND hwnd,int& nTotalSize);

	static void SetDisplayInfo(HWND hwnd,CString strText,int nPos,int nMax=-1);
	static void	SetDisplayInfo(HWND hwnd,int nPos,int nMax=-1);

	
private:
	CString m_strGameRunFolder;
public:
	afx_msg void OnEnChangeEditGamerunfolder();
	afx_msg void OnBnClickedUpdate();
	CString m_strDisplayInfo;
	CProgressCtrl m_Progress;
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedCancel();

	afx_msg void OnStnClickedStaticTextinfo();
	CEdit m_OutPutInfoEdit;
	afx_msg void OnDestroy();
};

void PutDebugStringMsg(const char *msg);
