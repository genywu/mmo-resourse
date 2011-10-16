#pragma once

class EditMap;

class VisibleSet
{

	DWORD   m_dwCurFlagValue;

	DWORD * m_pGridFlagArray;

	DWORD * m_pGridIndexArray;//索引数组

	DWORD	m_dwVisibleIndexCount;

	DWORD   m_dwGridCount;// = m_dwMapWidth * m_dwMapHeight;
	
	//Map Info

	DWORD   m_dwMapWidth;
	DWORD   m_dwMapDepth;
	

	//1.测试标志
	//2.放入链表

public:

	VisibleSet(void);
	~VisibleSet(void);

	void Create(EditMap * pMap);
	void Destroy();

	void SetVisible(int iGridIndex);
	
	DWORD * GetVisibleIndexArray() const { return m_pGridIndexArray;}
	DWORD   GetVisibleIndexCount() const { return m_dwVisibleIndexCount;}


	void Reset();
	void _DEBUG_Render(void);

	BOOL GetOpenFileDialog(const char *szFilter,char *szFile );
};
