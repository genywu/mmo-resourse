#pragma once
#include "..\..\frostengine\ui.h"

class WndViewer;

class WndModelTexture: public ui::Wnd
{
	ui::TreeView  * m_pTextureTree;			//纹理列表
	
	char		  m_pszShaderFileName[128];	//模型SHADER路径文件名
	char		  m_pszModelName[128];		//当前操作的model名字
public:
	WndModelTexture(void);
	~WndModelTexture(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();

	ui::TreeView * GetTextureTreeView() {return m_pTextureTree;}
	void CreateShaderTree(const char * ModelFilePathName);
	virtual void Update();
	virtual void Render();

	void SetShaderFilePathName (const char * pathname) {strcpy(m_pszShaderFileName,pathname);}
	void SetCurrentModelName(const char *name){strcpy(m_pszModelName,name);}
	const char * GetShaderFilePathName()	{return m_pszShaderFileName;}
	void SaveToShaderFile();		//保存tree 到文件
};