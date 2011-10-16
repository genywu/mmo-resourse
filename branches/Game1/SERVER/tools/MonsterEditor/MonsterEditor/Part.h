#pragma once


// CPart 对话框

class CPart : public CDialog
{
	DECLARE_DYNAMIC(CPart)

public:
	//设置对话框中记录数
	void SetData(vector<tagMonster>*vecMonster,int n,int m);
	CPart(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPart();

// 对话框数据
	enum { IDD = IDD_PART };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int nfirstA;
	int nfirstB;
	int nallrecords;
	afx_msg void OnPart();
	int nallskillrecords;
};
