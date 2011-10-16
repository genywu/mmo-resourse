#pragma once


// CStickQuest 对话框

class CStickQuest : public CDialog
{
	DECLARE_DYNAMIC(CStickQuest)

public:
	CStickQuest(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStickQuest();

// 对话框数据
	enum { IDD = IDD_DIALOG6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    long m_QuestID;
};
