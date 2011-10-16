#pragma once

#include <string>
#include <map>

class BuildTree
{
	static LRESULT CALLBACK s_mfWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static DWORD  s_dwSelModelIndex;
	static BOOL	  s_bChangeSel;

	static HWND s_hWnd;
	static HWND s_hWndTV;	
	std::map<std::string,DWORD> m_mapPath2Id;
public:
	void Refresh();
	void CreateTreeView(HWND hwndParent,HINSTANCE hInst,LPSTR lpszFileName);
	void Create(HWND hWndParent,HINSTANCE hWndInstance);

	static BOOL CheckChangeSel();

	static DWORD GetModelIndex();

	BOOL SearchModel(HTREEITEM iItem);
	HTREEITEM FindViewItem(HTREEITEM hParent,const char *szPath);
	HTREEITEM InsertViewItem(HTREEITEM hParent,char *szPath);
	HTREEITEM FindChildItem(HTREEITEM hParent,const char *szItemText);

	BuildTree(void);
	~BuildTree(void);

};
