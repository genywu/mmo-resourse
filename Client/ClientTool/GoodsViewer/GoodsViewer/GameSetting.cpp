#include "stdafx.h"
#include "GameSetting.h"

#include "..\..\..\Engine\Console.h"
#include "..\..\..\Engine\Utility.h"



IMPLEMENT_SINGLETON(Setting);

Setting::Setting()
{
	ZeroMemory(m_szTextureSharFolder,MAX_PATH);
}

Setting::~Setting()
{

}


bool Setting::LoadSetting()
{
	//设置默认颜色
	m_BackgroundColor = 0xFF2E374D;
	m_LightColor		= 0xFFFFFFFF;

	utility::File file;
	if(file.Open("GoodsViewer.config"))
	{
		//读取
		utility::Text script;
		script.Create(&file);
		utility::Text::Node *pRoot = script.GetRootNode();

		utility::Text::Variable *pTemVar(NULL);

		m_BackgroundColor = pRoot->GetVar("backcolor")->GetHexValue();
		m_LightColor		= pRoot->GetVar("lightcolor")->GetHexValue();

		pTemVar				= pRoot->GetVar("TextureFolder");
		if(pTemVar == NULL || strlen(pTemVar->GetStringValue())==0)
		{
			MessageBox(NULL,"请配置GoodsViewer.config文件下的TextureFolder路径！\n该路径用于存储所有模型的纹理文件。", "Error", MB_OK);
			file.Close();
			script.Destroy();
			return false;
		}
		else
			strcpy(m_szTextureSharFolder,pRoot->GetVar("TextureFolder")->GetStringValue());

		pTemVar = pRoot->GetVar("GoodsModelFolder");
		if(pTemVar == NULL || strlen(pTemVar->GetStringValue())==0)
		{
			MessageBox(NULL,"请配置GoodsViewer.config文件下的GoodsModelFolder路径！\n该路径用于存储所有物品模型的默认路径。","Error",MB_OK);
			file.Close();
			script.Destroy();
			return false;
		}
		else
			strcpy(m_szGoodsModelFolder,pRoot->GetVar("GoodsModelFolder")->GetStringValue());
		script.Destroy();
	}
	file.Close();
	return true;
}

void Setting::SaveSetting()
{
	FILE *fp = fopen("GoodsViewer.config","wb");
	if (fp)
	{
		fprintf(fp,"backcolor = %x\n",m_BackgroundColor);
		fprintf(fp,"lightcolor = %x\n",m_LightColor);
		fprintf(fp, "TextureFolder = %s\n", m_szTextureSharFolder);
		fprintf(fp,"GoodsModelFolder = %s\n",m_szGoodsModelFolder);
		fclose(fp);
	}
}