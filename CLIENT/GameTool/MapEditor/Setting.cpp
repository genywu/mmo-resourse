
#include "stdafx.h"
#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "setting.h"

IMPLEMENT_SINGLETON(Setting);

Setting::Setting(void)
{
	ZeroMemory(m_pszTextureSharFolder, MAX_PATH);
}

Setting::~Setting(void)
{
}

BOOL Setting::LoadSetting()
{
	//设置默认的
	m_fViewDistance   = 35.0f;
	m_fHieghtMapRatio = 10.0f;
	utility::File file;
	if (file.Open("MapEditor.config"))
	{
		//读取											  
		utility::Text script;
		script.Create(&file);
		utility::Text::Node *pRoot = script.GetRootNode();
		utility::Text::Variable *pVar(NULL);

		m_fViewDistance = pRoot->GetVar("viewdistance")->GetFloatValue();

		m_fHieghtMapRatio = pRoot->GetVar("height_ratio")->GetFloatValue();

		pVar = pRoot->GetVar("fog_enable");
		if (pVar)
		{	
			m_bFogEnable = pVar->GetBOOLValue();
		}
		else
			m_bFogEnable = FALSE;

		pVar = pRoot->GetVar("fog_color");
		if (pVar)
		{
			m_dwFogColor = pVar->GetHexValue();
		}
		else
			m_dwFogColor = 0xFFFFFFFF;
		pVar = pRoot->GetVar("TextureFolder");

		if (pVar == NULL || strlen(pVar->GetStringValue()) == 0)
		{
			MessageBox(NULL, "请配置ModelViewer.config文件下的TextureFolder路径！\n该路径用于存储所有模型的纹理文件。", "Error", MB_OK);
			file.Close();
			script.Destroy();
			return false;
		}else
			strcpy(m_pszTextureSharFolder, pRoot->GetVar("TextureFolder")->GetStringValue());

		script.Destroy();
	}
	file.Close();
	return true;
}

void Setting::SaveSetting()
{
	FILE *fp = fopen("MapEditor.config","wb");
	if (fp)
	{
		fprintf(fp,"viewdistance = %f\n",60.0f);
		fprintf(fp,"height_ratio = %f\n",m_fHieghtMapRatio);
		fprintf(fp,"fog_enable = %s\n",m_bFogEnable? "TRUE":"FALSE");
		fprintf(fp,"fog_color  = %X\n",m_dwFogColor);
		fprintf(fp, "TextureFolder = %s\n", m_pszTextureSharFolder);
		fclose(fp);
	}
}