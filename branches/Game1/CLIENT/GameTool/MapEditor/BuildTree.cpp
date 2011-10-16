#include "StdAfx.h"
#include "resource.h"
#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "BuildTree.h"

HWND BuildTree::s_hWnd = NULL;
HWND BuildTree::s_hWndTV = NULL;	
DWORD BuildTree::s_dwSelModelIndex = 0;
BOOL  BuildTree::s_bChangeSel = TRUE;

BOOL  BuildTree::CheckChangeSel()
{
	BOOL r = s_bChangeSel;
	s_bChangeSel = FALSE;
	return r;
}

DWORD BuildTree::GetModelIndex()
{
	return s_dwSelModelIndex;
}

LRESULT CALLBACK BuildTree::s_mfWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	BOOL bActive;

	switch (message) 
	{
	case WM_ACTIVATE:
		bActive = LOWORD(wParam);  
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// 分析菜单选择:
		//switch (wmId)
		//{
		//case IDM_EXIT:
		//	DestroyWindow(hWnd);
		//	break;
		//default:
		//	return DefWindowProc(hWnd, message, wParam, lParam);
		//}
		break;
	case WM_NOTIFY:
		{
			NMHDR *pHDR = (NMHDR*)lParam;
			if (pHDR->hwndFrom == s_hWndTV)
			{
				switch (((LPNMHDR)lParam)->code) 
				{
				case TVN_SELCHANGED:
					HTREEITEM hItem = TreeView_GetSelection(s_hWndTV);
					TV_ITEM tvi = {0};
					tvi.mask = TVIF_PARAM;
					tvi.hItem = hItem;
					TreeView_GetItem(s_hWndTV,&tvi);
					if (tvi.lParam != -1)
					{
                        s_dwSelModelIndex = (DWORD)tvi.lParam;
						s_bChangeSel = TRUE;
					}
					break;
				}
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

#define ICON_FOLDER   0
#define ICON_FILE	  1

BuildTree::BuildTree(void)
{
	
}

BuildTree::~BuildTree(void)
{
}

// CreateATreeView - creates a tree-view control. 
// Returns the handle to the new control if successful,
// or NULL otherwise. 
// hwndParent - handle to the control's parent window. 
// lpszFileName - name of the file to parse for tree-view items.

void BuildTree::CreateTreeView(HWND hwndParent,HINSTANCE hInst,LPSTR lpszFileName) 
{ 
	RECT rcClient;
    
    InitCommonControls(); 
    GetClientRect(hwndParent, &rcClient); 
    s_hWndTV = CreateWindowEx(0,
                            WC_TREEVIEW,
                            "Tree View",
							WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS, 
                            0, 
                            0, 
                            rcClient.right, 
                            rcClient.bottom,
                            hwndParent, 
                            (HMENU)0, 
                            hInst, 
                            NULL); 

	::ShowWindow(s_hWndTV,SW_SHOW);

	HIMAGELIST himl;  // handle to image list 
    HBITMAP hbmp;     // handle to bitmap 

    // Create the image list. 
	himl = ImageList_Create(16,16,ILC_COLOR32,2, 0);
    

    // Add the open file, closed file, and document bitmaps. 
    hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_BUILD)); 
    ImageList_Add(himl, hbmp, (HBITMAP)NULL); 
    DeleteObject(hbmp); 

    hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_FOLDER)); 
    ImageList_Add(himl, hbmp, (HBITMAP)NULL); 
    DeleteObject(hbmp); 

	int ic = ImageList_GetImageCount(himl);

    // Associate the image list with the tree-view control. 
    TreeView_SetImageList(s_hWndTV, himl, TVSIL_NORMAL); 

} 
// testRender.cpp : 定义应用程序的入口点。
//

void BuildTree::Create(HWND hWndParent,HINSTANCE hWndInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)BuildTree::s_mfWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;//hInstance;
	wcex.hIcon			= NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_MODELCOMPOSER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "szWindowClass";
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	::RegisterClassEx(&wcex);

	RECT rcParent;

	::GetWindowRect(hWndParent, &rcParent); 

	// 执行应用程序初始化:
	s_hWnd = CreateWindowEx( WS_EX_TOOLWINDOW ,"szWindowClass", "Build", WS_CAPTION,
		rcParent.right,rcParent.top,220,rcParent.bottom - rcParent.top, hWndParent, NULL, NULL, NULL);

	ShowWindow(s_hWnd, SW_SHOW);
    UpdateWindow(s_hWnd);

	CreateTreeView(s_hWnd,(HINSTANCE) hWndInstance,"A");

	Refresh();
}


void BuildTree::Refresh()
{
	TreeView_DeleteAllItems(s_hWndTV);

	utility::File file;
	if (!file.Open("model\\build\\build.idx"))
	{
		MessageBox(NULL,"build Define File Do Not Found!","Error",MB_OK | MB_ICONERROR);
		OutputConsole("build Define File Do Not Found!\n");
		ASSERT(0);
	}
	
	utility::Text theText;
	theText.Create(&file);
	file.Close();

	utility::Text::Node *pRoot = theText.GetRootNode();
	ASSERT(pRoot);

	for(DWORD n = 0; n < pRoot->GetChildCount(); n++)
	{
		utility::Text::Node *pBuildNode = pRoot->GetChildNode(n);

		const char *pModelPath = pBuildNode->GetVar("model")->GetStringValue();
		int id = pBuildNode->GetVar("id")->GetIntValue();
		m_mapPath2Id[pModelPath] = id;
	}	

	TVINSERTSTRUCT tvi = {0};
	tvi.hParent = TVI_ROOT;
	tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvi.item.lParam = -1;
	tvi.item.iImage = ICON_FOLDER;
	tvi.item.iSelectedImage = ICON_FOLDER;
	tvi.item.pszText = "Build";

	HTREEITEM hModelItem = TreeView_InsertItem(s_hWndTV,&tvi);

	char szTempPath[_MAX_PATH]; 
	for(DWORD n = 0; n < pRoot->GetChildCount(); n++)
	{
		utility::Text::Node *pTempNode = pRoot->GetChildNode(n);

		utility::File::GetFilePath(pTempNode->GetVar("model")->GetStringValue(),szTempPath);

		HTREEITEM hItem = FindViewItem(hModelItem,szTempPath);
		if (!hItem)
			hItem = InsertViewItem(hModelItem,szTempPath);

		char *pFileName = utility::File::GetFileName(pTempNode->GetVar("model")->GetStringValue());
		
		TVINSERTSTRUCT tvi = {0};
		tvi.hParent = hItem;
		tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		tvi.item.lParam = pTempNode->GetVar("id")->GetIntValue();
		tvi.item.iImage = ICON_FILE;
		tvi.item.iSelectedImage = ICON_FILE;
		tvi.item.pszText = pFileName;

		TreeView_InsertItem(s_hWndTV,&tvi);
	}	

	theText.Destroy();

}

HTREEITEM BuildTree::FindViewItem(HTREEITEM hParent,const char *szPath)
{
	//找到就OK
	const char *ps = szPath;
	const char *pf(NULL);
	char szTemp[_MAX_PATH];
	pf = strchr(ps,'\\');
	if (pf)
	{
		ZeroMemory(szTemp,_MAX_PATH);
		memcpy(szTemp,ps,(int)(pf - ps));
		
		HTREEITEM hItem = FindChildItem(hParent,szTemp);
		if (hItem)
            return FindViewItem(hItem,pf + 1);
		else 
			return NULL;
	
	}

	return FindChildItem(hParent,szPath);	
}

HTREEITEM BuildTree::InsertViewItem(HTREEITEM hParentItem,char *szPath)
{
	//找到就OK
	char *ps = szPath;
	char *pf(NULL);
	char szTemp[_MAX_PATH];
	pf = strchr(ps,'\\');
	if (pf)
	{
		ZeroMemory(szTemp,_MAX_PATH);
		memcpy(szTemp,ps,(int)(pf - ps));

		HTREEITEM hItem = FindChildItem(hParentItem,szTemp);

		if (!hItem)
		{
			TV_INSERTSTRUCT ti = {0};
			ti.hParent = hParentItem;
			ti.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
			ti.item.lParam  = -1;
			ti.item.iImage  = ICON_FOLDER;
			ti.item.iSelectedImage = ICON_FOLDER;
			ti.item.pszText = szTemp;
			
			hItem = TreeView_InsertItem(s_hWndTV,&ti);
		}

		return InsertViewItem(hItem,pf + 1);
	}
	
	HTREEITEM hItem = FindChildItem(hParentItem,szTemp);

	if (!hItem)
	{
		TV_INSERTSTRUCT ti = {0};
		ti.hParent = hParentItem;
		ti.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		ti.item.lParam  = -1;
		ti.item.iImage  = ICON_FOLDER;
		ti.item.iSelectedImage = ICON_FOLDER;
		ti.item.pszText = szPath;

		hItem = TreeView_InsertItem(s_hWndTV,&ti);
	}

	return hItem;
}

HTREEITEM BuildTree::FindChildItem(HTREEITEM hParent,const char *szItemText)
{
	HTREEITEM hItem = TreeView_GetChild(s_hWndTV,hParent);

	char szTemp[256];
	while(hItem != NULL)
	{
		TV_ITEM tvi = {0};
		tvi.mask = TVIF_TEXT;
		tvi.pszText = szTemp;
		tvi.cchTextMax = 255;
		tvi.hItem = hItem;
		TreeView_GetItem(s_hWndTV,&tvi);

		if (stricmp(tvi.pszText,szItemText) == 0)
		{
			return hItem;
		}
		hItem = TreeView_GetNextSibling(s_hWndTV,hItem);
	}
	return hItem;
}


