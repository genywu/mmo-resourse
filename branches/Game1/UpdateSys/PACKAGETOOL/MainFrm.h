// MainFrm.h : CMainFrame 类的接口
//

struct tagStaDisplay;

#pragma once
class CMainFrame : public CFrameWnd
{
private:
	CString		m_strLastOpenFolder;					//保存上次打开的文件夹
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:

// 操作
public:

	void	OnDisplayStatus(tagStaDisplay* pStaDisplay);
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpenfolder();
	afx_msg void OnUpdateStatuOperator(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatuFileName(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatuTotaNumb(CCmdUI* pCmdUI);
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnFileOpenclientinfo();
	afx_msg void OnFileOpenpatchinfo();
};


