
#include "stdafx.h"
#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "GameSetting.h"


IMPLEMENT_SINGLETON(Setting);

Setting::Setting(void)
{
	ZeroMemory(m_pszTextureSharFolder, MAX_PATH);
}

Setting::~Setting(void)
{
}

bool Setting::LoadSetting()
{
	//设置默认的
	m_dwBackgroundColor = 0xFF2E374D;
	m_dwLightColor		= 0xFFFFFFFF;

	utility::File file;
	if (file.Open("ModelViewer.config"))
	{
		//读取											  
		utility::Text script;
		script.Create(&file);
		utility::Text::Node *pRoot = script.GetRootNode();

		utility::Text::Variable * pTempVar(NULL);

		m_dwBackgroundColor = pRoot->GetVar("backcolor")->GetHexValue();
		m_dwLightColor	    = pRoot->GetVar("lightcolor")->GetHexValue();
		pTempVar = pRoot->GetVar("TextureFolder");

		if (pTempVar == NULL || strlen(pTempVar->GetStringValue()) == 0)
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
	FILE *fp = fopen("ModelViewer.config","wb");
	if (fp)
	{
		fprintf(fp,"backcolor = %x\n",m_dwBackgroundColor);
		fprintf(fp,"lightcolor = %x\n",m_dwLightColor);
		fprintf(fp, "TextureFolder = %s\n", m_pszTextureSharFolder);
		fclose(fp);
	}
}