
#include "stdafx.h"

#include "../Engine/console.h"
#include "../Engine/utility.h"
#include "../Engine/render.h"

#include "gamemodel.h"
#include "textureprojective.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//CLASS____(GameModel::Group::Part)_____
void GameModel::Locator::Create(utility::Text::Node * pNode,const char *szResourcePath)
{
	//名称
	m_dwName = 0;
	m_eLoad = Load_Not;
	strncpy((char*)&m_dwName,pNode->GetName(),4);

	//读取文件
	static char szTemp[_MAX_PATH];
	m_pMesh = NULL;
	utility::Text::Variable *pTempVar = pNode->GetVar("file");
	_snprintf(szTemp,_MAX_PATH,"%s\\%s",szResourcePath,pTempVar->GetStringValue());
	m_pMesh = new model::Mesh;
	m_pMesh->Create(szTemp);

	//建立buffer            
	m_pBuffer = new model::Buffer;

	m_pBuffer->Create(m_pMesh);
	m_pBuffer->SetGpuCaculate(false);

}

void GameModel::Locator::Create(utility::Text::Node * pNode)
{
	//名称
	m_dwName = 0;
	strncpy((char*)&m_dwName,pNode->GetName(),4);

	//读取文件
	utility::Text::Variable *pTempVar = pNode->GetVar("file");
	_snprintf(m_pszFileName,_MAX_PATH,"%s",pTempVar->GetStringValue());
	m_pMesh = NULL;
	m_pBuffer = NULL;
}
BOOL GameModel::PickMesh::Create(CRFile *pFile)
{
	if(NULL == pFile)	return FALSE;
	m_pMesh = new model::Mesh;
	m_pMesh->Create(pFile);
	//建立buffer            
	m_pBuffer = new model::Buffer;
	m_pBuffer->Create(m_pMesh);
	m_pBuffer->SetGpuCaculate(false);
	if(m_pMesh->GetGroupCount() == 0)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL GameModel::Locator::Create(CRFile *pFile)
{
	if(NULL == pFile)	return FALSE;
	m_pMesh = NULL;
	m_pBuffer = NULL;
	m_pMesh = new model::Mesh;
	m_pMesh->Create(pFile);
	//建立buffer            
	m_pBuffer = new model::Buffer;
	m_pBuffer->Create(m_pMesh);
	m_pBuffer->SetGpuCaculate(false);
	if(m_pMesh->GetGroupCount() == 0)
	{
		return FALSE;
	}
	return TRUE;
}
GameModel::PickMesh::PickMesh()
{
	m_pMesh = NULL;
	m_pBuffer = NULL;
}
GameModel::PickMesh::~PickMesh()
{
	m_pMesh = NULL;
	m_pBuffer = NULL;
}
void GameModel::PickMesh::Destroy()
{
	SAFEDESTROY(m_pMesh);
	SAFEDESTROY(m_pBuffer);
}
void GameModel::Locator::Destroy()
{
	SAFEDESTROY(m_pBuffer);
	SAFEDESTROY(m_pMesh);
}

GameModel::Group::Part::Part()
{
	m_dwSelectLevel = 0;//至少有一级
	m_dwRenderMeshCount = 0;
	m_dwLevelCount = 0;
}

GameModel::Group::Part::~Part()
{


}

//BOOL GameModel::Group::Part::Create(utility::Text::Node *pPartNode,char *szResourcePath)
//{
//	static char szError[1024];
//	static char szTemp[_MAX_PATH];
//	//Make name code
//	*((DWORD*)szTemp) = 0;
//	strncpy(szTemp,pPartNode->GetName(),4);
//	m_dwName = MAKEFOURCC(szTemp[0],szTemp[1],szTemp[2],szTemp[3]);
//	utility::Text::Variable *pTempVar(NULL);
//
//	pTempVar = pPartNode->GetVar("file");
//	ASSERT(pTempVar);
//	_snprintf(szTemp,_MAX_PATH,"%s\\%s",szResourcePath,pTempVar->GetStringValue());	
//
//	//打开part文件
//	utility::File	file;
//	if (!file.Open(szTemp))
//	{
//		OutputConsole("error: Group::Part::Create((\"%s\" failed.\n",szTemp);
//		MessageBox(NULL,szTemp,"打开文件错误",MB_OK);
//		ASSERT(0);
//		return FALSE;
//	}
//
//	utility::Text script;
//	script.Create(&file);
//	file.Close();
//	utility::Text::Node *pRootNode = script.GetRootNode();
//
//	//读取meshlist
//	utility::Text::Node *pMeshArrayNode = pRootNode->GetChildNode("mesharray");
//	ASSERT(pMeshArrayNode && "没有模型数组");
//	if (pMeshArrayNode)
//	{
//		m_dwRenderMeshCount = pMeshArrayNode->GetChildCount();
//		m_pRenderMeshArray = new tagRenderMesh[m_dwRenderMeshCount];
//		for(DWORD n = 0; n < m_dwRenderMeshCount; n ++)
//		{
//			utility::Text::Node *pMeshNode = pMeshArrayNode->GetChildNode(n);
//
//			tagRenderMesh * pRenderMesh = &m_pRenderMeshArray[n];
//			pTempVar = pMeshNode->GetVar("mesh");//一定要有文件
//			if (!pTempVar)
//			{
//				MessageBox(NULL,pPartNode->GetName(),"没有模型文件定义",MB_OK);
//			}
//			if (pTempVar)
//			{
//				_snprintf(szTemp,_MAX_PATH,"%s\\%s",szResourcePath,pTempVar->GetStringValue());
//
//				pRenderMesh->pMesh = new model::Mesh;
//				if (!pRenderMesh->pMesh->Create(szTemp))		
//				{
//					sprintf(szError,"error: 读取模型文件\"%s\" failed.\n",szTemp);
//					OutputConsole("error: Read mesh\"%s\" failed.\n",szTemp);
//					MessageBox(NULL,szError,"错误",MB_OK);
//				}
//			}
//			//读取相关数据
//			//1.读取显示方式
//			pTempVar = pMeshNode->GetVar("alphamode");
//			pRenderMesh->dwAlphaMode = 0;
//			if (pTempVar)
//			{
//				const char *szFilterString[] = {"test","blend"};//,"topblend"};
//				pRenderMesh->dwAlphaMode = pTempVar->GetEnumValue(szFilterString,2);
//			}
//
//			//2.读取billboard属性
//			pTempVar = pMeshNode->GetVar("billboard");
//			if (pTempVar)
//			{
//				pRenderMesh->bBillboard = pTempVar->GetBOOLValue();
//			}
//			else
//				pRenderMesh->bBillboard = FALSE;
//
//			pTempVar = pMeshNode->GetVar("y_billboard");
//			if (pTempVar)
//			{
//				pRenderMesh->bAroundYBillboard = pTempVar->GetBOOLValue();
//			}
//			else
//				pRenderMesh->bAroundYBillboard = FALSE;
//
//			pTempVar = pMeshNode->GetVar("animuv");
//			if (pTempVar)
//			{
//				pRenderMesh->bAnimUV = pTempVar->GetBOOLValue();
//			}
//			else
//				pRenderMesh->bAnimUV = FALSE;
//
//			pTempVar = pMeshNode->GetVar("animcolor");
//			if (pTempVar)
//			{
//				pRenderMesh->bAnimColor = pTempVar->GetBOOLValue();
//			}
//			else
//				pRenderMesh->bAnimColor = FALSE;
//
//			pTempVar = pMeshNode->GetVar("animvisibility");
//			if (pTempVar)
//			{
//				pRenderMesh->bAnimVisibility = pTempVar->GetBOOLValue();
//			}
//			else
//				pRenderMesh->bAnimVisibility = FALSE;
//
//			pTempVar = pMeshNode->GetVar("animtexture");
//			if (pTempVar)
//			{
//				pRenderMesh->bAnimTexture = pTempVar->GetBOOLValue();
//			}
//			else
//				pRenderMesh->bAnimTexture = FALSE;
//
//			pTempVar = pMeshNode->GetVar("effect_by_light");
//			if (pTempVar)
//			{
//				pRenderMesh->bEffectByLight = pTempVar->GetBOOLValue();
//			}
//			else
//				pRenderMesh->bEffectByLight = TRUE;
//
//			pTempVar = pMeshNode->GetVar("specular");
//			if (pTempVar)
//			{
//				pRenderMesh->bSpecular= pTempVar->GetBOOLValue();
//			}
//			else
//				pRenderMesh->bSpecular = FALSE;
//
//			pTempVar = pMeshNode->GetVar("environmentmaps");
//			if (pTempVar)
//			{
//				pRenderMesh->bEnvironmentMaps = pTempVar->GetBOOLValue();
//			}
//			else
//				pRenderMesh->bEnvironmentMaps = FALSE;
//
//			//建立缓冲
//			pRenderMesh->pBuffer = new model::Buffer;
//			pRenderMesh->pBuffer->Create(pRenderMesh->pMesh);
//			//建立渲染缓冲
//			pRenderMesh->pRenderBuffer = new model::Buffer;
//			pRenderMesh->pRenderBuffer->Create(pRenderMesh->pMesh);
//
//		}
//	}
//	//级别数据
//	utility::Text::Node *pLevelArrayNode = pRootNode->GetChildNode("levelarray");
//	ASSERT(pLevelArrayNode && "没有模型数组");
//	if (pLevelArrayNode)
//	{
//		m_dwLevelCount = pLevelArrayNode->GetChildCount();
//		m_pLevelArray  = new Level[m_dwLevelCount];
//		for(DWORD n = 0; n < m_dwLevelCount; n ++)
//		{
//			utility::Text::Node *pLevelNode = pLevelArrayNode->GetChildNode(n);
//			Level *pLevel = &m_pLevelArray[n];
//			if (!pLevel->Create(pLevelNode,szResourcePath))
//			{
//				//错误
//				MessageBox(NULL,"读取LevelNode失败!","错误",MB_OK);
//			}
//		}
//	}
//
//	script.Destroy();
//	return TRUE;
//}

BOOL GameModel::Group::Part::Create(utility::Text::Node *pPartNode,const char *szModelPath)
{
	static char szTemp[_MAX_PATH];
	static char szErrorInfo[1024];
	//Make name code
	*((DWORD*)szTemp) = 0;
	strncpy(szTemp,pPartNode->GetName(),4);
	m_dwName = MAKEFOURCC(szTemp[0],szTemp[1],szTemp[2],szTemp[3]);
	utility::Text::Variable *pTempVar(NULL);

	pTempVar = pPartNode->GetVar("file");

	//没有file节点
	if(NULL == pTempVar)
	{
		Log4c::Error(ROOT_MODULE,"%-15s model file(%s) Part(%s) config error。",__FUNCTION__,szModelPath,pPartNode->GetName());
		return FALSE;
	}
	_snprintf(m_strFileName,MAX_PATH,"%s",pTempVar->GetStringValue());

	//表示未装载
	m_eLoad = Load_Not;
	m_pLevelArray = NULL;
	m_pRenderMeshArray = NULL;
	return TRUE;
}



/*
* 功能: 部件创建
* 摘要: -
* 参数: pFile - 配置文件指针
* 返回值: -
* 作者: lpf
* 创建日期: -
* 修改日志:
*	2009.05.21 - lpf
*		修改了"读取模型索引列表"时"Part level中Mesh索引数量大于Mesh实际数量",错误的处理,目前为继续循环(原来是返回FALSE);
*		增加了_TOOL的宏控制判断,如果是工具类型的应用(如模型观察器),才输出错误消息提示框;
*/
BOOL GameModel::Group::Part::Create(CRFile * pFile)
{
	if(NULL == pFile)	return FALSE;

	static char szTemp[_MAX_PATH];

	utility::Text script;
	script.Create(pFile);
	utility::Text::Node *pRootNode = script.GetRootNode();
	utility::Text::Variable *pTempVar(NULL);
	//读取meshlist
	utility::Text::Node *pMeshArrayNode = pRootNode->GetChildNode("mesharray");
	ASSERT(pMeshArrayNode && "没有模型数组");
	if (pMeshArrayNode)
	{
		m_dwRenderMeshCount = pMeshArrayNode->GetChildCount();
		m_pRenderMeshArray = new tagRenderMesh[m_dwRenderMeshCount];
		for(DWORD n = 0; n < m_dwRenderMeshCount; n ++)
		{
			utility::Text::Node *pMeshNode = pMeshArrayNode->GetChildNode(n);
			tagRenderMesh * pRenderMesh = &m_pRenderMeshArray[n];
			pRenderMesh->pMesh = NULL;
			pRenderMesh->pBuffer=NULL;
			pRenderMesh->dwRendMeshIndex = 0xffffffff;

			pRenderMesh->wIndex = (WORD)n;
			pRenderMesh->SetLoadState(Load_Not);
			pTempVar = pMeshNode->GetVar("mesh");//一定要有文件
			if (!pTempVar)
			{
				MessageBox(NULL,m_strFileName,"没有模型文件定义",MB_OK);
			}
			if (pTempVar)
			{
				_snprintf(pRenderMesh->pszMeshFileName,_MAX_PATH,"%s",pTempVar->GetStringValue());

				//pRenderMesh->pMesh = new model::Mesh;
				//if (!pRenderMesh->pMesh->Create(szTemp))		
				//{
				//	sprintf(szError,"error: 读取模型文件\"%s\" failed.\n",szTemp);
				//	OutputConsole("error: Read mesh\"%s\" failed.\n",szTemp);
				//	MessageBox(NULL,szError,"错误",MB_OK);
				//}
			}
			//读取相关数据
			//1.读取显示方式
			pTempVar = pMeshNode->GetVar("alphamode");
			pRenderMesh->dwAlphaMode = 0;
			if (pTempVar)
			{
				const char *szFilterString[] = {"test","blend"};//,"topblend"};
				pRenderMesh->dwAlphaMode = pTempVar->GetEnumValue(szFilterString,2);
				if (pRenderMesh->dwAlphaMode == 0xffffffff)
				{
					pRenderMesh->dwAlphaMode = 0;
					//	MessageBox(NULL,m_strFileName,"alphamodel配置错误",MB_OK);
				}
			}
			//是否被投影
			pTempVar = pMeshNode->GetVar("tobeprojected");

			pRenderMesh->bProjected = pTempVar?pTempVar->GetBOOLValue():TRUE;


			//2.读取billboard属性
			pTempVar = pMeshNode->GetVar("billboard");
			if (pTempVar)
			{
				pRenderMesh->bBillboard = pTempVar->GetBOOLValue();
			}
			else
				pRenderMesh->bBillboard = FALSE;

			pTempVar = pMeshNode->GetVar("y_billboard");
			if (pTempVar)
			{
				pRenderMesh->bAroundYBillboard = pTempVar->GetBOOLValue();
			}
			else
				pRenderMesh->bAroundYBillboard = FALSE;

			pTempVar = pMeshNode->GetVar("animuv");
			if (pTempVar)
			{
				pRenderMesh->bAnimUV = pTempVar->GetBOOLValue();
			}
			else
				pRenderMesh->bAnimUV = FALSE;

			pTempVar = pMeshNode->GetVar("animcolor");
			if (pTempVar)
			{
				pRenderMesh->bAnimColor = pTempVar->GetBOOLValue();
			}
			else
				pRenderMesh->bAnimColor = FALSE;

			pTempVar = pMeshNode->GetVar("animvisibility");
			if (pTempVar)
			{
				pRenderMesh->bAnimVisibility = pTempVar->GetBOOLValue();
			}
			else
				pRenderMesh->bAnimVisibility = FALSE;

			pTempVar = pMeshNode->GetVar("animtexture");
			if (pTempVar)
			{
				pRenderMesh->bAnimTexture = pTempVar->GetBOOLValue();
			}
			else
				pRenderMesh->bAnimTexture = FALSE;

			pTempVar = pMeshNode->GetVar("effect_by_light");
			if (pTempVar)
			{
				pRenderMesh->bEffectByLight = pTempVar->GetBOOLValue();
			}
			else
				pRenderMesh->bEffectByLight = TRUE;

			pTempVar = pMeshNode->GetVar("effect_by_gridcolor");
			if (pTempVar)
			{
				pRenderMesh->bEffectByGridColor = pTempVar->GetBOOLValue();
			}
			else
				pRenderMesh->bEffectByGridColor = TRUE;

			pTempVar = pMeshNode->GetVar("specular");
			if (pTempVar)
			{
				pRenderMesh->bSpecular= pTempVar->GetBOOLValue();
			}
			else
				pRenderMesh->bSpecular = FALSE;

			pTempVar = pMeshNode->GetVar("environmentmaps");
			if (pTempVar)
			{
				pRenderMesh->bEnvironmentMaps = pTempVar->GetBOOLValue();
			}
			else
				pRenderMesh->bEnvironmentMaps = FALSE;

			////建立缓冲
			//pRenderMesh->pBuffer = new model::Buffer;
			//pRenderMesh->pBuffer->Create(pRenderMesh->pMesh);
			////建立渲染缓冲
			//pRenderMesh->pRenderBuffer = new model::Buffer;
			//pRenderMesh->pRenderBuffer->Create(pRenderMesh->pMesh);

		}
	}
	//级别数据
	utility::Text::Node *pLevelArrayNode = pRootNode->GetChildNode("levelarray");
	ASSERT(pLevelArrayNode && "没有模型数组");
	if (pLevelArrayNode)
	{
		m_dwLevelCount = pLevelArrayNode->GetChildCount();
		m_pLevelArray  = new Level[m_dwLevelCount];
		for(DWORD n = 0; n < m_dwLevelCount; n ++)
		{
			utility::Text::Node *pLevelNode = pLevelArrayNode->GetChildNode(n);
			Level *pLevel = &m_pLevelArray[n];

			utility::Text::Node *pMeshIndexNode = pLevelNode->GetChildNode("mesh_index");
			if (pMeshIndexNode)
			{
				LevelMeshes &RenderMeshes = pLevel->GetRMArray();
				//读取模型索引列表
				for(DWORD m = 0; m < pMeshIndexNode->GetVarCount(); m++)
				{
					pTempVar = pMeshIndexNode->GetVar(m);
					ASSERT(pTempVar && "mesh_index 变量不正确");
					int nRMIndex = pTempVar->GetIntValue();
					if (int(nRMIndex) > int(m_dwRenderMeshCount) - 1)
					{
#ifdef _TOOL
						MessageBox(NULL,"Part level中Mesh索引数量大于Mesh实际数量!","错误!",MB_OK);
#endif
						continue;
					}
					tagRenderMesh *pRenderMesh = &m_pRenderMeshArray[nRMIndex];
					RenderMeshes.push_back(pRenderMesh);
				}
			}

			if (!pLevel->Create(pLevelNode))
			{
				//错误
				MessageBox(NULL,"读取LevelNode失败!","错误",MB_OK);
			}
		}
	}
	script.Destroy();

	//已经装载了
	return TRUE;
}



void GameModel::Group::Part::Destroy()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();

	DWORD n = 0;
	for(; n < m_dwRenderMeshCount;n++)
	{
		tagRenderMesh *pRenderMesh = &m_pRenderMeshArray[n];
		SAFEDESTROY(pRenderMesh->pMesh);
		SAFEDESTROY(pRenderMesh->pBuffer);
		pLayer3D->GetRendMeshManager()->ReleaseRendMesh(pRenderMesh->dwRendMeshIndex);

	}

	SAFEDELETEARRAY(m_pRenderMeshArray);
	m_dwRenderMeshCount = 0;

	for(n = 0; n < m_dwLevelCount;n++)
	{
		Level *pLevel = &m_pLevelArray[n];
		pLevel->Destroy();
	}
	SAFEDELETEARRAY(m_pLevelArray);
	m_dwLevelCount = 0;
}

void GameModel::Group::Part::Dump()
{
	OutputConsole("		Part Name: %c%c%c%c\n",m_dwName >> 24 , m_dwName >> 16 & 0xff,m_dwName >> 8 & 0xff,m_dwName & 0xff);
}




//CLASS____(GameModel::Group)_____
GameModel::Group::Group():
m_pCurSelected(NULL),m_bVisible(FALSE)
{

}


GameModel::Group::~Group()
{
}

//BOOL GameModel::Group::Create(utility::Text::Node *pGroupNode,char *szResourcePath)
//{
//	//Make name code
//	DWORD dwCode = 0;
//	strncpy((char*)&dwCode,pGroupNode->GetName(),4);
//	m_dwName = dwCode;
//
//	//读取部件列表
//	DWORD dwPartCount = pGroupNode->GetChildCount();
//
//	if (dwPartCount)
//	{
//		for(DWORD n = 0; n < dwPartCount; n++)
//		{
//			utility::Text::Node *pPartNode = pGroupNode->GetChildNode(n);
//			ASSERT(pPartNode);
//
//			Part *pPart = new Part;
//
//			//read part date
//			BOOL bResult = pPart->Create(pPartNode,szResourcePath);
//			ASSERT(bResult);
//
//			extern bool g_bDebug;
//			if (g_bDebug)
//			{
//				if (m_mapPartArray.find(pPart->GetName()) != m_mapPartArray.end())
//				{
//					char str[256];
//					sprintf(str, "%s 的 PartArray: %s %c%c%c%c 重复了", szResourcePath, pGroupNode->GetName(),
//						LOBYTE(LOWORD(pPart->GetName())),
//						HIBYTE(LOWORD(pPart->GetName())),
//						LOBYTE(HIWORD(pPart->GetName())),
//						HIBYTE(HIWORD(pPart->GetName())));
//
//					MessageBox(NULL,str,"error",MB_OK);
//				}
//			}
//			m_mapPartArray.insert(PART_PAIR(pPart->GetName(),pPart));
//		}
//	}
//
//	return TRUE;
//}

BOOL GameModel::Group::Create(utility::Text::Node *pGroupNode,const char *szModelPath)
{
	static char szErrorInfo[1024];
	//Make name code
	DWORD dwCode = 0;
	strncpy((char*)&dwCode,pGroupNode->GetName(),4);
	m_dwName = dwCode;

	//读取部件列表
	DWORD dwPartCount = pGroupNode->GetChildCount();
	if (dwPartCount)
	{
		for(DWORD n = 0; n < dwPartCount; n++)
		{
			utility::Text::Node *pPartNode = pGroupNode->GetChildNode(n);
			ASSERT(pPartNode);

			Part *pPart = new Part;

			//read part date
			BOOL bResult = pPart->Create(pPartNode,szModelPath);
			if(!bResult)
			{
				Log4c::Error(ROOT_MODULE,"%-15s model(%s) Group(%s) config error",__FUNCTION__,szModelPath,pGroupNode->GetName());
			}
			ASSERT(bResult);

			extern bool g_bDebug;
			if (g_bDebug)
			{
				if (m_mapPartArray.find(pPart->GetName()) != m_mapPartArray.end())
				{
					char str[256];
					sprintf(str, "PartArray: %s %c%c%c%c error(repeat)",pGroupNode->GetName(),
						LOBYTE(LOWORD(pPart->GetName())),
						HIBYTE(LOWORD(pPart->GetName())),
						LOBYTE(HIWORD(pPart->GetName())),
						HIBYTE(HIWORD(pPart->GetName())));

					MessageBox(NULL,str,"error",MB_OK);
				}
			}
			m_mapPartArray.insert(PART_PAIR(pPart->GetName(),pPart));
		}
	}
	return TRUE;
}

void GameModel::Group::Destroy()
{
	//destroy part group
	for(std::map<DWORD,Part *>::iterator itPart = m_mapPartArray.begin(); itPart != m_mapPartArray.end();itPart++)
	{
		Part *pPart = itPart->second;
		SAFEDESTROY(pPart);	
	}
	m_mapPartArray.clear();
}

void GameModel::Group::SelectPart(DWORD dwName)
{
	//查找部件
	std::map<DWORD,Part*>::iterator itFind;
	itFind = m_mapPartArray.find(dwName);
	if (itFind != m_mapPartArray.end())
	{
		m_pCurSelected = itFind->second;
	}
}

void GameModel::Group::SelectNull()
{
	m_pCurSelected = NULL;
}

GameModel::Group::Part * GameModel::Group::GetPart(DWORD dwName)
{
	std::map<DWORD,Part*>::iterator itFind;
	itFind = m_mapPartArray.find(dwName);
	if (itFind != m_mapPartArray.end())
	{
		return itFind->second;
	}

	return NULL;
}


void GameModel::Group::Dump(void)
{
	OutputConsole("Dump: %c%c%c%c\n",m_dwName >> 24 , m_dwName >> 16 & 0xff,m_dwName >> 8 & 0xff,m_dwName & 0xff);
}

GameModel::Group::Part * GameModel::Group::GetSelectPart()
{
	return m_pCurSelected;
}

//CLASS____(GameModel::Action)____        
GameModel::Action::Action()
{
	m_pAnimJoint = NULL;
}
GameModel::Action::~Action()
{
	ASSERT(!m_pAnimJoint);
}
BOOL GameModel::Action::Create(utility::Text::Node *pActionNode,char *szResourcePath)
{
	//Make name code
	m_dwName = 0;
	strncpy((char*)&m_dwName,pActionNode->GetName(),4);

	//Read anim file
	static char szTemp[_MAX_PATH];	
	utility::Text::Variable *pTempVar(NULL);

	pTempVar = pActionNode->GetVar("file");
	ASSERT(pTempVar);
	_snprintf(szTemp,_MAX_PATH,"%s\\%s",szResourcePath,pTempVar->GetStringValue());	
	m_pAnimJoint = new model::AnimJoint;
	VERIFY(m_pAnimJoint->Create(szTemp));

	utility::Text::Node *pPVNode = pActionNode->GetChildNode("particle_parameter");
	if (pPVNode)
	{
		for(DWORD n = 0; n < pPVNode->GetChildCount();n++)
		{
			utility::Text::Node *pEmitterNode = pPVNode->GetChildNode(n);

			DWORD dwName(0);
			strncpy((char*)&dwName,pEmitterNode->GetName(),4);
			tagParticleParameter tPP;

			pTempVar = pEmitterNode->GetVar("visible");
			tPP.bVisibile = pTempVar ? pTempVar->GetBOOLValue() : TRUE;

			pTempVar = pEmitterNode->GetVar("start");
			tPP.dwStartTime = pTempVar ? (DWORD)pTempVar->GetIntValue() : 0;

			pTempVar = pEmitterNode->GetVar("end");
			tPP.dwEndTime = pTempVar ? (DWORD)pTempVar->GetIntValue() : 0xffffffff;

			m_mapParticleVisible.insert(PARTICLEVISIBLE_PAIR(dwName,tPP));
		}
	}

	utility::Text::Node *pTVNode = pActionNode->GetChildNode("trail_visible");
	if (pTVNode)
	{
		for(DWORD n = 0; n < pTVNode->GetVarCount();n++)
		{
			pTempVar = pTVNode->GetVar(n);
			ASSERT(pTempVar);
			DWORD dwName(0);
			strncpy((char*)&dwName,pTempVar->GetName(),4);

			m_mapTrailVisible.insert(TRAILVISIBLE_PAIR(dwName,pTempVar->GetBOOLValue()));
		}
	}
	m_bvisibility = pActionNode->GetVar("visibility")?pActionNode->GetVar("visibility")->GetBOOLValue():TRUE;
	return TRUE;
}

BOOL GameModel::Action::Create(utility::Text::Node * pActionNode)
{
	//Make name code
	m_dwName = 0;
	strncpy((char*)&m_dwName,pActionNode->GetName(),4);

	//Read anim file
	static char szTemp[_MAX_PATH];	
	utility::Text::Variable *pTempVar(NULL);

	pTempVar = pActionNode->GetVar("file");
	ASSERT(pTempVar);
	_snprintf(m_strFileName,_MAX_PATH,"%s",pTempVar->GetStringValue());	
	//稍后读取动画信息
	m_pAnimJoint = NULL;
	m_eLoad = Load_Not;

	utility::Text::Node *pPVNode = pActionNode->GetChildNode("particle_parameter");
	if (pPVNode)
	{
		for(DWORD n = 0; n < pPVNode->GetChildCount();n++)
		{
			utility::Text::Node *pEmitterNode = pPVNode->GetChildNode(n);

			DWORD dwName(0);
			strncpy((char*)&dwName,pEmitterNode->GetName(),4);
			tagParticleParameter tPP;

			pTempVar = pEmitterNode->GetVar("visible");
			tPP.bVisibile = pTempVar ? pTempVar->GetBOOLValue() : TRUE;

			pTempVar = pEmitterNode->GetVar("start");
			tPP.dwStartTime = pTempVar ? (DWORD)pTempVar->GetIntValue() : 0;

			pTempVar = pEmitterNode->GetVar("end");
			tPP.dwEndTime = pTempVar ? (DWORD)pTempVar->GetIntValue() : 0xffffffff;

			m_mapParticleVisible.insert(PARTICLEVISIBLE_PAIR(dwName,tPP));
		}
	}

	utility::Text::Node *pTVNode = pActionNode->GetChildNode("trail_visible");
	if (pTVNode)
	{
		for(DWORD n = 0; n < pTVNode->GetVarCount();n++)
		{
			pTempVar = pTVNode->GetVar(n);
			ASSERT(pTempVar);
			DWORD dwName(0);
			strncpy((char*)&dwName,pTempVar->GetName(),4);

			m_mapTrailVisible.insert(TRAILVISIBLE_PAIR(dwName,pTempVar->GetBOOLValue()));
		}
	}
	m_bvisibility = pActionNode->GetVar("visibility")?pActionNode->GetVar("visibility")->GetBOOLValue():TRUE;
	return TRUE;
}

void GameModel::Action::Destroy()
{
	SAFEDESTROY(m_pAnimJoint);
	m_mapParticleVisible.clear();
	m_mapTrailVisible.clear();
}

GameModel::Action::tagParticleParameter * GameModel::Action::GetParticleParameter(DWORD dwIndex)
{
	std::map<DWORD,tagParticleParameter>::iterator itFind;	
	itFind = m_mapParticleVisible.find(dwIndex);
	if (itFind != m_mapParticleVisible.end())
	{
		return &itFind->second;
	}
	return NULL;
}

BOOL GameModel::Action::GetTrailVisible(DWORD dwIndex)
{
	std::map<DWORD,BOOL>::iterator itFind;	
	itFind = m_mapTrailVisible.find(dwIndex);
	if (itFind != m_mapTrailVisible.end())
	{
		return itFind->second;
	}
	return TRUE;
}



void GameModel::Action::Dump(void)
{
	//dump this
	OutputConsole("Action: %c%c%c%c\n",m_dwName >> 24 , m_dwName >> 16 & 0xff,m_dwName >> 8 & 0xff,m_dwName & 0xff);
}

//CLASS____(GameModel)_____
BOOL GameModel::Create(const char *pszFileName,DWORD dwTextureProcision)
{
	//设置GameModel对应的文件名
	strncpy(m_pszFileName,pszFileName,MAX_PATH);
	//读取建立texture Shade 列表
	utility::File	file;
	if (!file.Open(pszFileName))
	{
		OutputConsole("error: GameModel::Create(\"%s\" failed.\n",pszFileName);
		MessageBox(NULL,pszFileName,"打开文件错误",MB_OK);
		ASSERT(0);
		return FALSE;
	}
	utility::Text script;
	script.Create(&file);
	file.Close();

	utility::Text::Node *pRootNode = script.GetRootNode();
	//整理出 texture ,sahder 送到layer3d里面建立shaderhandle

	char szResourcePath[_MAX_PATH];
	utility::File::GetFilePath(pszFileName,szResourcePath);

	//shader[ texture shader]
	utility::Text::Node *pShaderNode = pRootNode->GetChildNode("shader");
	ASSERT(pShaderNode);
	//read file
	char szShaderFileName[_MAX_PATH];
	_snprintf(szShaderFileName,_MAX_PATH,"%s\\%s",szResourcePath,pShaderNode->GetVar("file")->GetStringValue());

	utility::File fileShader;	
	if (!fileShader.Open(szShaderFileName))
	{
		OutputConsole("error: open \"%s\" failed.\n",szShaderFileName);
		MessageBox(NULL,szShaderFileName,"文件打开失败",MB_OK);
		return FALSE;
	}

	char *pszShaderText = new char [fileShader.GetLength() + 1];
	memcpy(pszShaderText,fileShader.GetData(),fileShader.GetLength());
	pszShaderText[fileShader.GetLength()] = 0;	
	fileShader.Close();
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();
	//get shader group handle
	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroup(pszShaderText,szResourcePath,dwTextureProcision);

	SAFEDELETE(pszShaderText);

	//read partgroup
	utility::Text::Node *pGroupListNode = pRootNode->GetChildNode("group");
	if (pGroupListNode)
	{
		DWORD dwGroupCount = pGroupListNode->GetChildCount();
		if (dwGroupCount > GAMEMODEL_MAXGROUP)
		{
			MessageBox(NULL,"模型组件超过最大数目","注意",MB_OK);
			dwGroupCount = GAMEMODEL_MAXGROUP - 1;
		}
		for(DWORD n = 0;n < dwGroupCount;n++)
		{
			Group *pGroup = new Group;
			utility::Text::Node *pGroupNode = pGroupListNode->GetChildNode(n);
			ASSERT(pGroupNode);
			pGroup->Create(pGroupNode,szResourcePath);

			extern bool g_bDebug;
			if (g_bDebug)
			{
				if (m_mapGroupArray.find(pGroup->GetName()) != m_mapGroupArray.end())
				{
					char str[256];
					sprintf(str, "%s 的 group: %04d 重复了", pszFileName, n-1);
					MessageBox(NULL,str,"error",MB_OK);
				}
			}

			m_mapGroupArray.insert(GROUP_PAIR(pGroup->GetName(),pGroup));
		}
	}

	//读取全局时钟
	utility::Text::Node *pTimerNode = pRootNode->GetChildNode("timer");
	if (pTimerNode)
	{
		utility::Text::Variable *pTempVar = pTimerNode->GetVar("animcolorcircle");
		if (pTempVar)
		{
			m_dwAnimColorCircle = (DWORD)pTempVar->GetIntValue();
		}
		else
			m_dwAnimColorCircle = 5000;

		pTempVar = pTimerNode->GetVar("animuvcircle");
		if (pTempVar)
		{
			m_dwAnimUVCircle = (DWORD)pTempVar->GetIntValue();
		}
		else
			m_dwAnimUVCircle = 5000;

		pTempVar = pTimerNode->GetVar("animvisibilitycircle");
		if (pTempVar)
		{
			m_dwAnimVisibilityCircle= (DWORD)pTempVar->GetIntValue();
		}
		else
			m_dwAnimVisibilityCircle = 5000;

		pTempVar = pTimerNode->GetVar("animtexturecircle");
		if (pTempVar)
		{
			m_dwAnimTextureCircle = (DWORD)pTempVar->GetIntValue();
		}
		else
			m_dwAnimTextureCircle = 5000;
	}

	//读取动作列表
	utility::Text::Node *pActionListNode = pRootNode->GetChildNode("action");
	if (pActionListNode)
	{
		for(DWORD n = 0 ; n < pActionListNode->GetChildCount(); n++)
		{
			Action *pAction =  new Action;
			utility::Text::Node *pActionNode = pActionListNode->GetChildNode(n);
			ASSERT(pActionNode);
			pAction->Create(pActionNode,szResourcePath);
			m_mapActionArray.insert(ACTION_PAIR(pAction->GetName(),pAction));
		}
	}

	//读取邦定文件
	utility::Text::Node *pBindPoseNode = pRootNode->GetChildNode("bindpose");
	if (pBindPoseNode)
	{
		//插入表中
		static char szTemp[_MAX_PATH];
		utility::Text::Variable *pTempVar = pBindPoseNode->GetVar("file");
		_snprintf(szTemp,_MAX_PATH,"%s\\%s",szResourcePath,pTempVar->GetStringValue());
		m_pBindPose = new model::BindPose;
		m_pBindPose->Create(szTemp);
	}
	else
	{
		if (m_mapActionArray.size()!=0)
		{
			OutputError("model 未指定 绑定文件. \"%s\".\n",pszFileName);
			MessageBox(NULL,pszFileName,"错误: 动画模型未指定绑定文件.\n",MB_OK);
		}
	}

	//读取定位器
	utility::Text::Node *pLocatorNode = pRootNode->GetChildNode("locator");
	if (pLocatorNode)
	{
		for(DWORD n =0 ; n < pLocatorNode->GetChildCount(); n++)
		{
			Locator *pLocator = new Locator;
			pLocator->Create(pLocatorNode->GetChildNode(n),szResourcePath);
			m_mapLocatorArray.insert(LOCATOR_PAIR(pLocator->GetName(),pLocator));
		}
	}

	//读取发射器
	utility::Text::Node *pParticleEmitterNode = pRootNode->GetChildNode("particle_emitter");
	if (pParticleEmitterNode)
	{
		DWORD dwCount = pParticleEmitterNode->GetChildCount();
		if (dwCount > GAMEMODEL_MAXPARTICLEEMITTER)
		{
			dwCount = GAMEMODEL_MAXPARTICLEEMITTER;
			MessageBox(NULL,pszFileName,"粒子发射器数目达到上限,超过的将被忽略",MB_OK);
		}

		for(DWORD n =0 ; n < dwCount; n++)
		{

			ParticleEmitter *pEmitter = new ParticleEmitter;
			pEmitter->Create(pParticleEmitterNode->GetChildNode(n),szResourcePath);
			m_mapParticleEmitterArray.insert(PARTICLEEMITTER_PAIR(pEmitter->GetName(),pEmitter));
		}
	}
	//读取纹理投影
	utility::Text::Node *ptextureprojective = pRootNode->GetChildNode("textureprojective");
	if (ptextureprojective)
	{
		/*DWORD dwCount = ptextureprojective->GetChildCount();
		if (dwCount > GAMEMODEL_MAXTEXTUREPROJECTIVE)
		{
		dwCount = GAMEMODEL_MAXTEXTUREPROJECTIVE;
		MessageBox(NULL,pszFileName,"投影纹理数目达到上限,超过的将被忽略",MB_OK);
		}

		for(DWORD n =0 ; n < dwCount; n++)
		{
		TextureProjective *ptexprj = new TextureProjective;
		ptexprj->Create(ptextureprojective->GetChildNode(n),szResourcePath);
		m_maptextureprojectivearray.insert(TEXTUREPROJECTIVE_PAIR(ptexprj->GetName(),ptexprj));
		}*/
		TextureProjective *ptexprj = new TextureProjective;
		ptexprj->Create(ptextureprojective);
		m_maptextureprojectivearray.insert(TEXTUREPROJECTIVE_PAIR(ptexprj->GetName(),ptexprj));

	}
	//读取尾巴效果
	utility::Text::Node *pTrailEmitterNode = pRootNode->GetChildNode("trail_emitter");
	if (pTrailEmitterNode )
	{
		DWORD dwCount = pTrailEmitterNode ->GetChildCount();
		if (dwCount > GAMEMODEL_MAXTRAILEMITTER)
		{
			dwCount = GAMEMODEL_MAXTRAILEMITTER;
			MessageBox(NULL,pszFileName,"尾迹发射器数目达到上限,超过的将被忽略",MB_OK);
		}
		for(DWORD n =0 ; n < dwCount; n++)
		{
			TrailEmitter *pTrailEmitter = new TrailEmitter;
			pTrailEmitter->Create(pTrailEmitterNode ->GetChildNode(n),szResourcePath);
			m_mapTrailEmitterArray.insert(TRAILEMITTER_PAIR(pTrailEmitter->GetName(),pTrailEmitter));
		}													  
	}
	/*********************************************************************
	读取尺寸盒
	**********************************************************************/
	D3DXVECTOR3 vMin(0.0f,0.0f,0.0f),vMax(1.0f,1.0f,1.0f);	
	utility::Text::Node *pSizeBoxNode = pRootNode->GetChildNode("sizebox");
	m_pSizeBox = new render::BoundingBox;
	if (pSizeBoxNode)
	{
		utility::Text::Node *pMinNode = pSizeBoxNode->GetChildNode("min");
		vMin.x = pMinNode->GetVar("x")->GetFloatValue();
		vMin.y = pMinNode->GetVar("y")->GetFloatValue();
		vMin.z = pMinNode->GetVar("z")->GetFloatValue();
		utility::Text::Node *pMaxNode = pSizeBoxNode->GetChildNode("max");
		vMax.x = pMaxNode->GetVar("x")->GetFloatValue();
		vMax.y = pMaxNode->GetVar("y")->GetFloatValue();
		vMax.z = pMaxNode->GetVar("z")->GetFloatValue();
	}
	else
	{	
		//默认 //求出最大包围框
		//根据显示组件表取得组件，渲染
		BOOL bInit = FALSE;
		for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
		{
			GameModel::Group *Group = itGroup->second;
			std::map<DWORD,GameModel::Group::Part*> *pPartArray = Group->GetPartArray();	
			for(std::map<DWORD,GameModel::Group::Part*>::iterator itPart = pPartArray->begin();itPart != pPartArray->end(); itPart++)
			{
				GameModel::Group::Part * pPart = itPart->second;

				GameModel::Group::Part::tagRenderMesh *pRenderMeshArray = pPart->GetRenderMeshArray();

				for(DWORD n = 0; n < pPart->GetRenderMeshArrayCount();n++)
				{
					model::Mesh *pMesh = pRenderMeshArray[n].pMesh;
					D3DXVECTOR3 *pPosBuffer = pMesh->GetPosBuffer();
					for(DWORD i = 0; i < pMesh->GetPNCCount(); i++)
					{
						if (!bInit)
						{
							vMin = pPosBuffer[i];
							vMax = pPosBuffer[i];
							bInit = TRUE;
						}
						//calc bounding box

						if (pPosBuffer[i].x < vMin.x) vMin.x = pPosBuffer[i].x;
						if (pPosBuffer[i].y < vMin.y) vMin.y = pPosBuffer[i].y;
						if (pPosBuffer[i].z < vMin.z) vMin.z = pPosBuffer[i].z;
						if (pPosBuffer[i].x > vMax.x) vMax.x = pPosBuffer[i].x;
						if (pPosBuffer[i].y > vMax.y) vMax.y = pPosBuffer[i].y;
						if (pPosBuffer[i].z > vMax.z) vMax.z = pPosBuffer[i].z;
					}
				}
			}
		}
	}
	m_pSizeBox->Set(vMin,vMax);


	//读取选择盒
	utility::Text::Node *pPickBoxNode = pRootNode->GetChildNode("pickbox");
	//读取选择模型,求出范围盒
	m_pPickBox = new render::BoundingBox;
	if (pPickBoxNode)
	{
		utility::Text::Node *pMinNode = pPickBoxNode->GetChildNode("min");
		vMin.x = pMinNode->GetVar("x")->GetFloatValue();
		vMin.y = pMinNode->GetVar("y")->GetFloatValue();
		vMin.z = pMinNode->GetVar("z")->GetFloatValue();
		utility::Text::Node *pMaxNode = pPickBoxNode->GetChildNode("max");
		vMax.x = pMaxNode->GetVar("x")->GetFloatValue();
		vMax.y = pMaxNode->GetVar("y")->GetFloatValue();
		vMax.z = pMaxNode->GetVar("z")->GetFloatValue();
	} 
	else
	{
		//使用sizebox
	}

	m_pPickBox->Set(vMin,vMax);


	script.Destroy();
	return TRUE;
}



/*
* 功能: 根据CRFile创建模型
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2009.10.20 - lpf
*		增加了读取水面效果
*/
BOOL GameModel::Create(CRFile* pFile)
{
	if(NULL == pFile)	return FALSE;

	utility::Text script;
	script.Create(pFile);
	utility::Text::Node *pRootNode = script.GetRootNode();

	//shader[ texture shader]
	utility::Text::Node *pShaderNode = pRootNode->GetChildNode("shader");
	ASSERT(pShaderNode);
	//read file
	_snprintf(m_pszShaderFileName,_MAX_PATH,"%s",pShaderNode->GetVar("file")->GetStringValue());

	//read partgroup
	utility::Text::Node *pGroupListNode = pRootNode->GetChildNode("group");
	if (pGroupListNode)
	{
		DWORD dwGroupCount = pGroupListNode->GetChildCount();
		if (dwGroupCount > GAMEMODEL_MAXGROUP)
		{
			MessageBox(NULL,"模型组件超过最大数目","注意",MB_OK);
			dwGroupCount = GAMEMODEL_MAXGROUP - 1;
		}
		for(DWORD n = 0;n < dwGroupCount;n++)
		{
			Group *pGroup = new Group;
			utility::Text::Node *pGroupNode = pGroupListNode->GetChildNode(n);
			ASSERT(pGroupNode);
			pGroup->Create(pGroupNode,pFile->GetFileName());

			extern bool g_bDebug;
			if (g_bDebug)
			{
				if (m_mapGroupArray.find(pGroup->GetName()) != m_mapGroupArray.end())
				{
					char str[256];
					sprintf(str, "group: %04d 重复了",n-1);
					MessageBox(NULL,str,"error",MB_OK);
				}
			}
			m_mapGroupArray.insert(GROUP_PAIR(pGroup->GetName(),pGroup));
		}
	}

	//读取全局时钟
	utility::Text::Node *pTimerNode = pRootNode->GetChildNode("timer");
	if (pTimerNode)
	{
		utility::Text::Variable *pTempVar = pTimerNode->GetVar("animcolorcircle");
		if (pTempVar)
		{
			m_dwAnimColorCircle = (DWORD)pTempVar->GetIntValue();
		}
		else
			m_dwAnimColorCircle = 5000;

		pTempVar = pTimerNode->GetVar("animuvcircle");
		if (pTempVar)
		{
			m_dwAnimUVCircle = (DWORD)pTempVar->GetIntValue();
		}
		else
			m_dwAnimUVCircle = 5000;

		pTempVar = pTimerNode->GetVar("animvisibilitycircle");
		if (pTempVar)
		{
			m_dwAnimVisibilityCircle= (DWORD)pTempVar->GetIntValue();
		}
		else
			m_dwAnimVisibilityCircle = 5000;

		pTempVar = pTimerNode->GetVar("animtexturecircle");
		if (pTempVar)
		{
			m_dwAnimTextureCircle = (DWORD)pTempVar->GetIntValue();
		}
		else
			m_dwAnimTextureCircle = 5000;
	}

	//读取动作列表
	utility::Text::Node *pActionListNode = pRootNode->GetChildNode("action");
	if (pActionListNode)
	{
		for(DWORD n = 0 ; n < pActionListNode->GetChildCount(); n++)
		{
			Action *pAction =  new Action;
			utility::Text::Node *pActionNode = pActionListNode->GetChildNode(n);
			ASSERT(pActionNode);
			pAction->Create(pActionNode);
			if (m_mapActionArray.find(pAction->GetName()) != m_mapActionArray.end())
			{
				static char str[255];
				sprintf(str,"%s动画名有重复\n",pActionNode->GetName());
				OutputDebugStr(str);
				SAFEDESTROY(pAction);

			}
			else
			{
				m_mapActionArray.insert(ACTION_PAIR(pAction->GetName(),pAction));
			}
		}
	}


	//读取邦定文件
	utility::Text::Node *pBindPoseNode = pRootNode->GetChildNode("bindpose");
	if (pBindPoseNode)
	{
		//插入表中
		static char szTemp[_MAX_PATH];
		utility::Text::Variable *pTempVar = pBindPoseNode->GetVar("file");
		_snprintf(szTemp,_MAX_PATH,"%s",pTempVar->GetStringValue());
		m_pBindPose = new model::BindPose;
		m_pBindPose->SetFileName(szTemp);
		//m_pBindPose->Create(szTemp);
	}
	else
	{
		if (m_mapActionArray.size()!=0)
		{
			OutputError("model 未指定 绑定文件.\n");
			MessageBox(NULL,"","错误: 动画模型未指定绑定文件.\n",MB_OK);
		}
	}

	//读取定位器
	utility::Text::Node *pLocatorNode = pRootNode->GetChildNode("locator");
	if (pLocatorNode)
	{
		for(DWORD n =0 ; n < pLocatorNode->GetChildCount(); n++)
		{
			Locator *pLocator = new Locator;
			pLocator->Create(pLocatorNode->GetChildNode(n));
			m_mapLocatorArray.insert(LOCATOR_PAIR(pLocator->GetName(),pLocator));
		}
	}

	//读取发射器
	utility::Text::Node *pParticleEmitterNode = pRootNode->GetChildNode("particle_emitter");
	if (pParticleEmitterNode)
	{
		DWORD dwCount = pParticleEmitterNode->GetChildCount();
		if (dwCount > GAMEMODEL_MAXPARTICLEEMITTER)
		{
			dwCount = GAMEMODEL_MAXPARTICLEEMITTER;
			MessageBox(NULL,"","粒子发射器数目达到上限,超过的将被忽略",MB_OK);
		}

		for(DWORD n =0 ; n < dwCount; n++)
		{

			ParticleEmitter *pEmitter = new ParticleEmitter;
			pEmitter->Create(pParticleEmitterNode->GetChildNode(n));
			m_mapParticleEmitterArray.insert(PARTICLEEMITTER_PAIR(pEmitter->GetName(),pEmitter));
		}
	}
	//读取纹理投影
	utility::Text::Node *ptextureprojective = pRootNode->GetChildNode("textureprojective");
	if (ptextureprojective)
	{
		/*DWORD dwCount = ptextureprojective->GetChildCount();
		if (dwCount > GAMEMODEL_MAXTEXTUREPROJECTIVE)
		{
		dwCount = GAMEMODEL_MAXTEXTUREPROJECTIVE;
		MessageBox(NULL,pszFileName,"投影纹理数目达到上限,超过的将被忽略",MB_OK);
		}

		for(DWORD n =0 ; n < dwCount; n++)
		{
		TextureProjective *ptexprj = new TextureProjective;
		ptexprj->Create(ptextureprojective->GetChildNode(n),szResourcePath);
		m_maptextureprojectivearray.insert(TEXTUREPROJECTIVE_PAIR(ptexprj->GetName(),ptexprj));
		}*/
		TextureProjective *ptexprj = new TextureProjective;
		ptexprj->Create(ptextureprojective);
		m_maptextureprojectivearray.insert(TEXTUREPROJECTIVE_PAIR(ptexprj->GetName(),ptexprj));

	}
	m_bRendShadow = pRootNode->GetVar("rendshadow")?pRootNode->GetVar("rendshadow")->GetBOOLValue():TRUE;
	//读取尾巴效果
	utility::Text::Node *pTrailEmitterNode = pRootNode->GetChildNode("trail_emitter");
	if (pTrailEmitterNode )
	{
		DWORD dwCount = pTrailEmitterNode ->GetChildCount();
		if (dwCount > GAMEMODEL_MAXTRAILEMITTER)
		{
			dwCount = GAMEMODEL_MAXTRAILEMITTER;
			MessageBox(NULL,"","尾迹发射器数目达到上限,超过的将被忽略",MB_OK);
		}
		for(DWORD n =0 ; n < dwCount; n++)
		{
			TrailEmitter *pTrailEmitter = new TrailEmitter;
			pTrailEmitter->Create(pTrailEmitterNode ->GetChildNode(n));
			m_mapTrailEmitterArray.insert(TRAILEMITTER_PAIR(pTrailEmitter->GetName(),pTrailEmitter));
		}													  
	}
	// 读取水面效果
	utility::Text::Node * pWater = pRootNode->GetChildNode("water");
	if (pWater != NULL)
	{
		utility::Text::Variable * pTempVar = pWater->GetVar("file");
		if (pTempVar != NULL)
		{
			m_pWater = new Water::CWater();
			m_pWater->SetWaterFileName(pTempVar->GetStringValue());
			Water::CManager::GetInstance()->AddWater(m_pWater);
		}
	}
	/*********************************************************************
	读取尺寸盒
	**********************************************************************/
	D3DXVECTOR3 vMin(0.0f,0.0f,0.0f),vMax(1.0f,1.0f,1.0f);
	utility::Text::Node *pSizeBoxNode = pRootNode->GetChildNode("sizebox");
	m_pSizeBox = new render::BoundingBox;
	if (pSizeBoxNode)
	{
		utility::Text::Node *pMinNode = pSizeBoxNode->GetChildNode("min");
		vMin.x = pMinNode->GetVar("x")->GetFloatValue();
		vMin.y = pMinNode->GetVar("y")->GetFloatValue();
		vMin.z = pMinNode->GetVar("z")->GetFloatValue();
		utility::Text::Node *pMaxNode = pSizeBoxNode->GetChildNode("max");
		vMax.x = pMaxNode->GetVar("x")->GetFloatValue();
		vMax.y = pMaxNode->GetVar("y")->GetFloatValue();
		vMax.z = pMaxNode->GetVar("z")->GetFloatValue();
	}
	else
	{
		m_bSizeBoxAutoMake = true;
	}
	m_pSizeBox->Set(vMin,vMax);
	//读取选择盒
	utility::Text::Node *pPickBoxNode = pRootNode->GetChildNode("pickbox");
	//读取选择模型,求出范围盒
	m_pPickBox = new render::BoundingBox;
	if (pPickBoxNode)
	{
		utility::Text::Node *pMinNode = pPickBoxNode->GetChildNode("min");
		vMin.x = pMinNode->GetVar("x")->GetFloatValue();
		vMin.y = pMinNode->GetVar("y")->GetFloatValue();
		vMin.z = pMinNode->GetVar("z")->GetFloatValue();
		utility::Text::Node *pMaxNode = pPickBoxNode->GetChildNode("max");
		vMax.x = pMaxNode->GetVar("x")->GetFloatValue();
		vMax.y = pMaxNode->GetVar("y")->GetFloatValue();
		vMax.z = pMaxNode->GetVar("z")->GetFloatValue();
	} 
	else
	{
		m_bPickBoxAutoMake = true;
	}
	m_pPickBox->Set(vMin,vMax);
	//读取选择MESH
	utility::Text::Node *pPickMeshNode = pRootNode->GetChildNode("pickmesh");
	if (pPickMeshNode)
	{
		utility::Text::Variable *pTempVar  = pPickMeshNode->GetVar("mesh");
		static char szTemp[_MAX_PATH];
		_snprintf(szTemp,_MAX_PATH,"%s",pTempVar->GetStringValue());
		m_pPickMesh = new GameModel::PickMesh;
		m_pPickMesh->SetFileName(szTemp);
	} 
	else
	{
		m_pPickMesh = NULL;
	}
	script.Destroy();
	return TRUE;
}


void GameModel::AutoMakeBoundingBox()
{
	if(m_bSizeBoxAutoMake)
	{
		D3DXVECTOR3 vMin(0.0f,0.0f,0.0f),vMax(1.0f,1.0f,1.0f);	
		//默认 //求出最大包围框
		//根据显示组件表取得组件，渲染
		BOOL bInit = FALSE;
		for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
		{
			GameModel::Group *Group = itGroup->second;
			std::map<DWORD,GameModel::Group::Part*> *pPartArray = Group->GetPartArray();	
			for(std::map<DWORD,GameModel::Group::Part*>::iterator itPart = pPartArray->begin();itPart != pPartArray->end(); itPart++)
			{
				GameModel::Group::Part * pPart = itPart->second;
				if(pPart->GetLoadState() != Load_Did) continue;

				GameModel::Group::Part::tagRenderMesh *pRenderMeshArray = pPart->GetRenderMeshArray();
				for(DWORD n = 0; n < pPart->GetRenderMeshArrayCount();n++)
				{
					if(pRenderMeshArray[n].GetLoadState() != Load_Did) continue;

					model::Mesh *pMesh = pRenderMeshArray[n].pMesh;
					if(NULL == pMesh)	continue;

					D3DXVECTOR3 *pPosBuffer = pMesh->GetPosBuffer();
					for(DWORD i = 0; i < pMesh->GetPNCCount(); i++)
					{
						if (!bInit)
						{
							vMin = pPosBuffer[i];
							vMax = pPosBuffer[i];
							bInit = TRUE;
						}
						//calc bounding box

						if (pPosBuffer[i].x < vMin.x) vMin.x = pPosBuffer[i].x;
						if (pPosBuffer[i].y < vMin.y) vMin.y = pPosBuffer[i].y;
						if (pPosBuffer[i].z < vMin.z) vMin.z = pPosBuffer[i].z;
						if (pPosBuffer[i].x > vMax.x) vMax.x = pPosBuffer[i].x;
						if (pPosBuffer[i].y > vMax.y) vMax.y = pPosBuffer[i].y;
						if (pPosBuffer[i].z > vMax.z) vMax.z = pPosBuffer[i].z;
					}
				}
			}
		}
		m_pSizeBox->Set(vMin,vMax);

		if(m_bPickBoxAutoMake)
			m_pPickBox->Set(vMin,vMax);
	}
}

BOOL GameModel::CreateShaderGroup(CRFile* pFile,DWORD dwTextureProcision)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();
	//get shader group handle
	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroup(pFile,dwTextureProcision);
	return TRUE;
}


void GameModel::Destroy()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();
	for(std::map<DWORD,Group *>::iterator itGroup = m_mapGroupArray.begin(); itGroup != m_mapGroupArray.end();itGroup++)
	{
		Group *pGroup = itGroup->second;
		SAFEDESTROY(pGroup);	
	}
	m_mapGroupArray.clear();

	for(std::map<DWORD,TrailEmitter *>::iterator itTrailEmitter = m_mapTrailEmitterArray.begin(); itTrailEmitter != m_mapTrailEmitterArray.end();itTrailEmitter++)
	{
		TrailEmitter *pTrailEmitter = itTrailEmitter->second;
		SAFEDESTROY(pTrailEmitter);	
	}
	m_mapTrailEmitterArray.clear();

	for(std::map<DWORD,ParticleEmitter *>::iterator itEmitter = m_mapParticleEmitterArray.begin(); itEmitter != m_mapParticleEmitterArray.end();itEmitter++)
	{
		ParticleEmitter *pEmitter = itEmitter->second;
		SAFEDESTROY(pEmitter);	
	}
	m_mapParticleEmitterArray.clear();



	for(std::map<DWORD,TextureProjective *>::iterator ittexproj = m_maptextureprojectivearray.begin(); ittexproj != m_maptextureprojectivearray.end();ittexproj++)
	{
		TextureProjective *ptexproj = ittexproj->second;
		SAFEDESTROY(ptexproj);	
	}
	m_maptextureprojectivearray.clear();

	for(std::map<DWORD,Locator *>::iterator itLocator = m_mapLocatorArray.begin(); itLocator != m_mapLocatorArray.end();itLocator++)
	{
		Locator *pLocator = itLocator->second;
		SAFEDESTROY(pLocator);	
	}
	m_mapLocatorArray.clear();

	//destroy shader group handle
	pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);

	//destroy part group


	//destroy action array
	for(std::map<DWORD,Action *>::iterator itAction = m_mapActionArray.begin(); itAction != m_mapActionArray.end(); itAction++)
	{
		Action *pAction = itAction->second;
		SAFEDESTROY(pAction);
	}
	m_mapActionArray.clear();

	//释放动画
	itAnimColor itAnimColor =  m_mapAnimColor.begin();
	for(;itAnimColor != m_mapAnimColor.end();itAnimColor++)
	{
		SAFEDESTROY((*itAnimColor).second);
	}
	m_mapAnimColor.clear();

	itAnimUV itAnimUV = m_mapAnimUV.begin();
	for(;itAnimUV != m_mapAnimUV.end();itAnimUV++)
	{
		SAFEDESTROY((*itAnimUV).second);
	}
	m_mapAnimUV.clear();

	itAnimVisi itAnimV = m_mapAnimVisi.begin();
	for(;itAnimV != m_mapAnimVisi.end();itAnimV++)
	{
		SAFEDESTROY((*itAnimV).second);
	}
	m_mapAnimVisi.clear();

	itAnimTex itAnimTex =  m_mapAnimTexture.begin();
	for(;itAnimTex != m_mapAnimTexture.end();itAnimTex++)
	{
		SAFEDESTROY((*itAnimTex).second);
	}
	m_mapAnimTexture.clear();
	SAFEDELETE(m_pSizeBox);
	SAFEDESTROY(m_pPickMesh);
	SAFEDELETE(m_pPickBox);
	SAFEDESTROY(m_pBindPose);

}

GameModel::TrailEmitter* GameModel::GetTrailEmitter(DWORD dwName)
{
	map<DWORD,TrailEmitter*>::iterator it = m_mapTrailEmitterArray.find(dwName);
	if(it != m_mapTrailEmitterArray.end())
		return (*it).second;
	return NULL;
}

GameModel::ParticleEmitter* GameModel::GetParticleEmitter(DWORD dwName)
{
	map<DWORD,ParticleEmitter*>::iterator it = m_mapParticleEmitterArray.find(dwName);
	if(it != m_mapParticleEmitterArray.end())
		return (*it).second;
	return NULL;
}

void GameModel::SetGroupVisible(DWORD dwGroup,BOOL bVisible)
{
	std::map<DWORD,Group*>::iterator itFind;

	itFind = m_mapGroupArray.find(dwGroup);

	if (itFind != m_mapGroupArray.end())
	{
		Group *pGroup = itFind->second;
		pGroup->SetVisible(bVisible);
	}
}

void GameModel::SelectPart(DWORD dwGroup,DWORD dwPart)
{
	std::map<DWORD,Group*>::iterator itFind;

	itFind = m_mapGroupArray.find(dwGroup);

	if (itFind != m_mapGroupArray.end())
	{
		Group *pGroup = itFind->second;
		pGroup->SelectPart(dwPart);
	}
}

void GameModel::ProcessAnimJoint(AnimInfo *pAnimInfo)
{
	//当前激活的动画  
	AnimInfo::tagActionInfo *pActionInfo = pAnimInfo->GetActionInfo();
	DWORD  dwCurrentTime = pAnimInfo->GetCurrentTime();

	if (pActionInfo->bInterpolation)
	{
		//插值
		pActionInfo->dwInterpolationElapseTime = dwCurrentTime - pActionInfo->dwInterpolationStartTime;
		if (pActionInfo->dwInterpolationElapseTime > pActionInfo->dwInterpolationTimeLength)
		{
			//插值完成
			pActionInfo->bInterpolation = FALSE;
			pActionInfo->dwCurActionStartTime  = dwCurrentTime;
			pActionInfo->dwCurActionLoopCount  = 0 ;
			pActionInfo->dwCurActionElapseTime = 0 ;
		}
	}
	else
	{
		Action *pCurAction = GetAction(pActionInfo->dwCurAction);
		if (pCurAction)//有动画则显示
		{
			//设置当前动画数据的时间
			model::AnimJoint *pAnimJoint = pCurAction->GetAnimJoint();	
			if (pAnimJoint)
			{
				//根据开始，结束时间设定帧
				DWORD dwCurActionTimeElapse = dwCurrentTime - pActionInfo->dwCurActionStartTime;

				DWORD dwTimeLength = pAnimJoint->GetTimeLength();

				if (dwCurActionTimeElapse >= dwTimeLength)
				{
					//循环一次
					dwCurActionTimeElapse = dwTimeLength;
					if (pActionInfo->bCurActionLooped)
					{
						pActionInfo->dwCurActionLoopCount +=1;
						pActionInfo->dwCurActionStartTime  = dwCurrentTime;
					}
					else
						pActionInfo->dwCurActionLoopCount = 1;
				}
				pActionInfo->dwCurActionElapseTime = dwCurActionTimeElapse;
			}
		}
	}
	const D3DXMATRIX *pWorldMatrix = pAnimInfo->GetWorldMatrix();
	const D3DXMATRIX *pBillboard = pAnimInfo->GetBillboard();
	const D3DXMATRIX *pBillboardY = pAnimInfo->GetBillboardY();

	Action *pOldAction = GetAction(pActionInfo->dwOldAction);
	Action *pCurAction = GetAction(pActionInfo->dwCurAction);	

	model::AnimJoint *pOldAnimJoint = (pOldAction)?  pOldAction->GetAnimJoint() : NULL;	
	model::AnimJoint *pCurAnimJoint = (pCurAction)?  pCurAction->GetAnimJoint() : NULL;	

	DWORD dwCurActionTimeElapse = pActionInfo->dwCurActionElapseTime;
	DWORD dwOldActionTimeElapse = pActionInfo->dwOldActionTimeElapse;
	DWORD dwInterplationTimeElapse = pActionInfo->dwInterpolationElapseTime;

	float fRatio = (float)dwInterplationTimeElapse / (float)pActionInfo->dwInterpolationTimeLength;

	//根据显示组件表取得组件计算拷贝缺省数据
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second;
		GameModel::Group::Part * pPart = pGroup->GetSelectPart();
		if (pPart)
		{
			pPart->ProcessAnimJoint(pAnimInfo,pOldAnimJoint,pCurAnimJoint,dwOldActionTimeElapse,dwCurActionTimeElapse,m_pBindPose,fRatio);

		}
	}

}

void GameModel::ProcessParticleEmitter(AnimInfo *pAnimInfo)
{
	//刷新粒子
	const D3DXMATRIX *pWorldMatrix = pAnimInfo->GetWorldMatrix();
	const D3DXMATRIX *pViewMatrix = pAnimInfo->GetViewMatrix();
	const D3DXMATRIX *pProjMatrix = pAnimInfo->GetProjectedMatrix();
	const D3DXMATRIX *pBillboard = pAnimInfo->GetBillboard();
	const D3DXMATRIX *pBillboardY = pAnimInfo->GetBillboardY();

	const AnimInfo::tagActionInfo *pActionInfo = pAnimInfo->GetActionInfo();

	Action *pOldAction = GetAction(pActionInfo->dwOldAction);
	Action *pCurAction = GetAction(pActionInfo->dwCurAction);	

	model::AnimJoint *pCurAnimJoint = (pCurAction)? pCurAction->GetAnimJoint() : NULL;
	model::AnimJoint *pOldAnimJoint = (pOldAction)?  pOldAction->GetAnimJoint() : NULL;	

	DWORD dwCurActionTimeElapse = pActionInfo->dwCurActionElapseTime;
	DWORD dwOldActionTimeElapse = pActionInfo->dwOldActionTimeElapse;

	DWORD dwInterplationTimeElapse = pActionInfo->dwInterpolationElapseTime;

	float fRatio = (float)dwInterplationTimeElapse / (float)pActionInfo->dwInterpolationTimeLength;

	DWORD dwCurrentTime = pAnimInfo->GetCurrentTime();

	//发射器处理
	DWORD dwParticleEmitterTimeIndex = 0;
	for(std::map<DWORD ,ParticleEmitter *>::iterator itEmitter = m_mapParticleEmitterArray.begin(); itEmitter != m_mapParticleEmitterArray.end();itEmitter++)
	{
		ParticleEmitter *pEmitter = itEmitter->second;
		//这里只刷新粒子时间		
		if (pEmitter->GetParticleEmitter()->UpdateTime(pAnimInfo->m_dwParticleEmitterLastTimeArray[dwParticleEmitterTimeIndex],dwCurrentTime))
		{	
			pAnimInfo->m_dwParticleEmitterLastTimeArray[dwParticleEmitterTimeIndex] = dwCurrentTime;
		}
		dwParticleEmitterTimeIndex++;
		//
		Action::tagParticleParameter *pPP = pCurAction ? pCurAction->GetParticleParameter(pEmitter->GetName()) : NULL;
		DWORD dwStartTime(0),dwEndTime(0xFFFFFFFF);

		if (pPP)
		{
			if (!pPP->bVisibile)
			{
				//不可见
				continue;
			}
			dwStartTime = pPP->dwStartTime;
			dwEndTime   = pPP->dwEndTime;
		}
		if (pEmitter->IsEnable())
		{
			//检查是否是开始时间和结束
			if (dwCurActionTimeElapse < dwStartTime || dwCurActionTimeElapse > dwEndTime)
			{
				continue;
			}

			model::Mesh   * pMesh      = pEmitter->GetMesh();
			model::Buffer * pBuffer   = pEmitter->GetBuffer();
			D3DXVECTOR3   * pPosBuffer = pBuffer->GetPosBuffer();	

			if (pActionInfo->bInterpolation && pOldAnimJoint && pCurAnimJoint&&pMesh->GetGroupCount())
				pMesh->ProcessAnimJointInterpolation(pBuffer,pWorldMatrix,pViewMatrix,pProjMatrix,m_pBindPose,pOldAnimJoint,dwOldActionTimeElapse,pActionInfo->fOldActionSpeedRate,pCurAnimJoint,0,1.0f,fRatio); 
			else
				if (pCurAnimJoint&&pMesh->GetGroupCount())
					pMesh->ProcessAnimJoint(pBuffer,pWorldMatrix,pViewMatrix,pProjMatrix,m_pBindPose,pCurAnimJoint,dwCurActionTimeElapse,pActionInfo->fCurActionSpeedRate);
				else
					pMesh->CopyPosition(pBuffer,pWorldMatrix,pViewMatrix,pProjMatrix);

			particle::Emitter * pParticleEmitter = pEmitter->GetParticleEmitter();
			pParticleEmitter->SetBillboard(pBillboard); 
			pParticleEmitter->SetBillboardY(pBillboardY);
			D3DXMATRIX mattemp = *pWorldMatrix;
			mattemp._41 = mattemp._42 = mattemp._43 = 0.0f;
			pParticleEmitter->SetNowMatrix(&mattemp);

			for(DWORD i = 0; i < pEmitter->GetEmitteNumber(); i++)
			{

				switch(pEmitter->GetType())
				{
				case 0://POINT
					{
						int iIndex = rand() % pBuffer->GetPNCCount();  		
						//发射

						pParticleEmitter->SetPosition(&pPosBuffer[iIndex]);
					}
					break;
				case 1://LINE
					{
						//随机位置及下一个位置
						int iPosCount = pBuffer->GetPNCCount();
						int iIndexStart = rand() % iPosCount;
						int iIndexEnd   = (iIndexStart + 1) % iPosCount;

						D3DXVECTOR3 &pos0 = pPosBuffer[iIndexStart];
						D3DXVECTOR3 &pos1 = pPosBuffer[iIndexEnd];

						//插值
						D3DXVECTOR3 vNormal = pos1 - pos0;
						float fLength = D3DXVec3Length(&vNormal);
						D3DXVec3Normalize(&vNormal,&vNormal);
						float fCur = (float)rand() / RAND_MAX * fLength;
						D3DXVECTOR3 vCur = pos0 + vNormal * fCur;
						pParticleEmitter->SetPosition(&vCur);
					}
					break;
				}
				pParticleEmitter->Emission();
			}
		}
	}    


}
void GameModel::ProcessTextureProjective(AnimInfo *pAnimInfo)
{
	const D3DXMATRIX* matworld = pAnimInfo->GetWorldMatrix();
	TextureProjective* ptexprj;
	for (std::map<DWORD,TextureProjective*>::iterator it = m_maptextureprojectivearray.begin();it != m_maptextureprojectivearray.end();it++)
	{
		ptexprj = it->second;
		if (ptexprj->IsEnable())
		{
			ptexprj->GetTextureProjective()->SetPosDirAlphaNowtime(matworld->_41,0,matworld->_43,0.0f,-1.0f,pAnimInfo->GetCurrentTime() - pAnimInfo->GetTextureProjectiveTime());
		}
		//ptexprj->GetTextureProjective()->SetNowTime(pAnimInfo->GetCurrentTime());
		//ptexprj->GetTextureProjective()->UpdateMapArray();
		//ptexprj->GetTextureProjective()->Rend();
		// GetGame()->GetRegion->GetGameMap()->GetTagMapBuffer(matworld->_41,matworld->_43,ptexprj->GetTextureProjective()->GetSize(),ptexprj->GetTextureProjective()->GetBuffer());

	}
}
void GameModel::ProcessVisibility(AnimInfo *pAnimInfo)
{
	DWORD dwCurrentTime = pAnimInfo->GetCurrentTime();
	//刷新模型组上的颜色动画效果
	DWORD dwTimeElapse = dwCurrentTime - pAnimInfo->m_dwAnimVisibilityStartTime;
	if (dwTimeElapse >= m_dwAnimVisibilityCircle)
	{
		//循环一次
		dwTimeElapse = m_dwAnimVisibilityCircle;
		pAnimInfo->m_dwAnimVisibilityStartTime = dwCurrentTime;
	}
	//Visibility效果
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		//根据开始，结束时间设定帧
		Group *pGroup = itGroup->second;
		if (pGroup->IsVisible())
		{
			Group::Part *pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				pPart->ProcessLevelAnimVisibility(dwTimeElapse,m_dwAnimVisibilityCircle);
			}
		}
	}
}

void GameModel::ProcessAnimTexture(AnimInfo *pAnimInfo)
{
	//动画纹理效果
	DWORD dwCurrentTime = pAnimInfo->GetCurrentTime();

	//根据开始，结束时间设定帧
	DWORD dwTimeElapse = dwCurrentTime - pAnimInfo->m_dwAnimTextureStartTime;
	if (dwTimeElapse >= m_dwAnimTextureCircle)
	{
		//循环一次
		dwTimeElapse = m_dwAnimTextureCircle;
		pAnimInfo->m_dwAnimTextureStartTime = dwCurrentTime;
	}

	//动画纹理效果
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		//根据开始，结束时间设定帧
		Group *pGroup = itGroup->second;
		if (pGroup->IsVisible())
		{
			Group::Part *pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				pPart->ProcessLevelAnimTexture(dwTimeElapse,m_dwAnimTextureCircle);
			}
		}
	}
}

void GameModel::ProcessTrailEmitter(AnimInfo *pAnimInfo)
{
	//计算尾迹效果
	const GameModel::AnimInfo::tagActionInfo *pActionInfo = pAnimInfo->GetActionInfo();
	//计算动作
	Action *pCurAction = GetAction(pActionInfo->dwCurAction);
	if (!pCurAction)
	{
		//无动作无法拖尾
		return;
	}

	model::AnimJoint *pAnimJoint = 	pCurAction->GetAnimJoint();

	//这里没有内插，应该修改之
	DWORD dwCurrentTime = pAnimInfo->GetCurrentTime();
	DWORD dwAnimTimeElapse = pAnimInfo->GetActionInfo()->dwCurActionElapseTime;

	if (pAnimJoint)
	{
		DWORD dwTCIndex = 0;
		for(std::map<DWORD,TrailEmitter *>::iterator itEmitter = m_mapTrailEmitterArray.begin(); itEmitter != m_mapTrailEmitterArray.end();itEmitter++)
		{
			TrailEmitter *pEmitter = itEmitter->second;
			if (!pCurAction->GetTrailVisible(pEmitter->GetName()))
			{
				continue;
			}
			if (!pEmitter->IsEnable())
			{
				continue;
			}
			//
			model::Mesh   *pMesh      = pEmitter->GetMesh();
			model::Buffer *pBuffer    = pEmitter->GetBuffer();
			D3DXVECTOR3   *pPosBuffer = pBuffer->GetPosBuffer();

			DWORD dwTimeElapse = dwCurrentTime - pActionInfo->dwCurActionStartTime;
			if (dwTimeElapse > pAnimJoint->GetTimeLength())
			{
				dwTimeElapse = pAnimJoint->GetTimeLength();
			}
			if (pMesh->GetGroupCount())
			{
				pMesh->ProcessAnimJoint(pBuffer,pAnimInfo->GetWorldMatrix(),pAnimInfo->GetViewMatrix(),pAnimInfo->GetProjectedMatrix(),m_pBindPose,pAnimJoint,dwAnimTimeElapse,pActionInfo->fCurActionSpeedRate);
			}


			GameModel::TrailContainer::tagTrail *pTrail = pAnimInfo->m_pTrailContainerArray[dwTCIndex].GetFreeTrail();//array of 6 	

			// [0] = a[i]  [1] = b[i] [2] = b[i+1]
			pTrail->vPos[0]     = pPosBuffer[0];
			pTrail->vPos[1]     = pPosBuffer[1];
			pTrail->dwStartTime = pAnimInfo->GetCurrentTime();
			pTrail->dwTimeLength= pEmitter->GetLifeTimeLength(); 
			unsigned __int64 qwColor = pEmitter->GetColor();
			pTrail->dwTopColor	= (DWORD)(qwColor >> 32);
			pTrail->dwBottomColor = (DWORD)(qwColor & 0xFFFFFFFF);
			//OutputConsole("%64x %x %x\n",pTrail->dwTopColor,pTrail->dwBottomColor);

			pAnimInfo->m_pTrailContainerArray[dwTCIndex].Update(dwCurrentTime);
			pAnimInfo->m_pTrailContainerArray[dwTCIndex].Render(m_dwShaderGroupHandle,pMesh->GetShaderIndex());
			dwTCIndex++;
		}
	}  
}

void GameModel::RenderModel()
{
	//根据显示组件表取得组件，渲染
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second;
		if (pGroup->IsVisible())
		{
			GameModel::Group::Part * pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				pPart->Render(m_dwShaderGroupHandle);
			}
		}
	}
}
void GameModel::RenderModelTransparent()
{
	//根据显示组件表取得组件，渲染
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second;
		if (pGroup->IsVisible())
		{
			GameModel::Group::Part * pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				pPart->RenderTransparent(m_dwShaderGroupHandle,2);
			}
		}
	}
}
void GameModel::_DEBUG_RenderAction(const GameModel::AnimInfo *pAnimInfo)
{
	const AnimInfo::tagActionInfo *pActionInfo = pAnimInfo->GetActionInfo();

	Action *pCurAction = GetAction(pActionInfo->dwCurAction);

	DWORD dwActionTimeElapse = pAnimInfo->GetActionInfo()->dwCurActionElapseTime;
	if (pCurAction)//有动作才渲染
	{
		//测试渲染skeleton
		model::AnimJoint *pAnimJoint = pCurAction->GetAnimJoint();	
		pAnimJoint->_DEBUG_Render(pAnimInfo->GetWorldMatrix(),dwActionTimeElapse);  
	}
}

void GameModel::_DEBUG_RenderNormal()
{
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second;
		if (pGroup->IsVisible())
		{
			GameModel::Group::Part * pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				pPart->_DEBUG_RenderNormal();
			}
		}
	}
}

void GameModel::_DEBUG_RenderSizeBox(const D3DXMATRIX *pWroldMatrix,D3DCOLOR dwColor)
{
	//渲染显示组件表取得组件，渲染
	if (m_pSizeBox)
		m_pSizeBox->_DEBUG_Render(pWroldMatrix,dwColor);
}

void GameModel::_DEBUG_RenderPickBox(const D3DXMATRIX *pWroldMatrix,D3DCOLOR dwColor)
{
	//渲染显示组件表取得组件，渲染
	if (m_pPickBox)
		m_pPickBox->_DEBUG_Render(pWroldMatrix,dwColor);
}

void GameModel::Dump(void)
{
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		Group *pGroup = itGroup->second;
		pGroup->Dump();
	}
}

void GameModel::SetGroupVisible(bool bVisible)
{
	for(std::map<DWORD,Group *>::iterator itGroup = m_mapGroupArray.begin(); itGroup != 
		m_mapGroupArray.end(); itGroup ++)
	{
		Group *pGroup = itGroup->second;
		pGroup->SetVisible(bVisible);
	}
}

void GameModel::SetGroupLevel(int iLevel)
{
	for(std::map<DWORD,Group *>::iterator itGroup = m_mapGroupArray.begin(); itGroup != 
		m_mapGroupArray.end(); itGroup ++)
	{
		Group *pGroup = itGroup->second;
		Group::Part *pPart = pGroup->GetSelectPart();
		if (pPart)
		{
			pPart->SetLevel(iLevel);
		}
	}
}

void GameModel::ProcessPointLight(const D3DXVECTOR3 *pLightPosition,float fIntensity,DWORD dwColor)
{
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second;

		GameModel::Group::Part * pPart = pGroup->GetSelectPart();
		if (pPart)
		{
			pPart->ProcessPointLight(pLightPosition,fIntensity,dwColor);
		}
	}
}

void GameModel::ProcessDirectionalLight(const D3DXVECTOR3 *pCameraPosition,const D3DXVECTOR3 *pLightDirection,DWORD dwColor)
{
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second; 

		GameModel::Group::Part * pPart = pGroup->GetSelectPart();
		if (pPart)
		{
			pPart->ProcessDirectionalLight(pCameraPosition,pLightDirection,dwColor);
		}
	}
}


DWORD GameModel::GetVisibleTriangleCount(void)
{
	int dwTriangleCount = 0;
	std::map<DWORD,GameModel::Group *>::iterator it;

	for(it = m_mapGroupArray.begin(); it != m_mapGroupArray.end(); it++)
	{
		GameModel::Group *pGroup = it->second;
		if (pGroup->IsVisible())
		{
			GameModel::Group::Part *pPart = pGroup->GetSelectPart();

			if (pPart)
			{
				GameModel::Group::Part::Level *pLevel = pPart->GetSelectLevel();
				if (pLevel)
				{
					GameModel::Group::Part::LevelMeshes& vMeshes = pLevel->GetRMArray();
					for(int i = 0; i < pLevel->GetRMCount(); i++)
					{
						model::Mesh *pMesh = vMeshes[i]->pMesh;

						dwTriangleCount += pMesh->GetTriangleCount();
					}
				}
			}
		}
	}

	return dwTriangleCount;
}

void GameModel::SelectDefaultPart(DWORD dwGroup)
{
	std::map<DWORD,Group*>::iterator itFind;

	itFind = m_mapGroupArray.find(dwGroup);

	if (itFind != m_mapGroupArray.end())
	{
		Group *pGroup = itFind->second;
		pGroup->SelectDefault();		
	}
}

void GameModel::SelectNullPart(DWORD dwGroup)
{
	std::map<DWORD,Group*>::iterator itFind;

	itFind = m_mapGroupArray.find(dwGroup);

	if (itFind != m_mapGroupArray.end())
	{
		Group *pGroup = itFind->second;
		pGroup->SelectNull();		
	}
}

void GameModel::Group::SelectDefault()
{
	if (m_mapPartArray.size())
	{
		m_pCurSelected = m_mapPartArray.begin()->second; 
	}
}

//得到缺省part的名字
DWORD GameModel::Group::GetDefaultPartName(void)
{
	if (m_mapPartArray.size())
	{
		return m_mapPartArray.begin()->second->GetName(); 
	}
	return 0;
}



void GameModel::SelectDefaultPart(void)
{
	std::map<DWORD,Group*>::iterator it;	
	for(it = m_mapGroupArray.begin(); it != m_mapGroupArray.end(); it++)
	{
		Group *pGroup = it->second;
		pGroup->SelectDefault();		
	}
}

model::AnimColor* GameModel::GetAnimColor(const char* pszFileName)
{
	itAnimColor it = m_mapAnimColor.find(pszFileName);
	if(it != m_mapAnimColor.end())
		return (*it).second;
	return NULL;
}

void GameModel::SetAnimColor(const char* pszFileName,model::AnimColor* pAnimColor)
{
	itAnimColor it = m_mapAnimColor.find(pszFileName);
	if(it != m_mapAnimColor.end())
		return;
	m_mapAnimColor[pszFileName] = pAnimColor;
}

model::AnimUV* GameModel::GetAnimUV(const char* pszFileName)
{
	itAnimUV it = m_mapAnimUV.find(pszFileName);
	if(it != m_mapAnimUV.end())
		return (*it).second;
	return NULL;
}

void GameModel::SetAnimUV(const char* pszFileName,model::AnimUV* pAnimUV)
{
	itAnimUV it = m_mapAnimUV.find(pszFileName);
	if(it != m_mapAnimUV.end())
		return;
	m_mapAnimUV[pszFileName] = pAnimUV;
}

model::AnimVisibility* GameModel::GetAnimVisi(const char* pszFileName)
{
	itAnimVisi it = m_mapAnimVisi.find(pszFileName);
	if(it != m_mapAnimVisi.end())
		return (*it).second;
	return NULL;
}

void GameModel::SetAnimVisi(const char* pszFileName,model::AnimVisibility* pAnimVisi)
{
	itAnimVisi it = m_mapAnimVisi.find(pszFileName);
	if(it != m_mapAnimVisi.end())
		return;
	m_mapAnimVisi[pszFileName] = pAnimVisi;
}


model::AnimTexture* GameModel::GetAnimTexture(const char* pszFileName)
{
	itAnimTex it = m_mapAnimTexture.find(pszFileName);
	if(it != m_mapAnimTexture.end())
		return (*it).second;
	return NULL;
}

void GameModel::SetAnimTexture(const char* pszFileName,model::AnimTexture* pANim)
{
	itAnimTex it = m_mapAnimTexture.find(pszFileName);
	if(it != m_mapAnimTexture.end())
		return;
	m_mapAnimTexture[pszFileName]=pANim;
}


//渲染步骤: 1 设置动作 2刷新时间 3处理数据 4送入渲染管道
GameModel::Action * GameModel::GetAction(DWORD dwName)
{
	std::map<DWORD,Action *>::iterator itFind;
	itFind = m_mapActionArray.find(dwName);
	if (itFind != m_mapActionArray.end())
	{
		//找到
		return itFind->second;
	}

	return NULL;
}


GameModel::Group * GameModel::GetGroup(DWORD dwName)
{
	std::map<DWORD,Group*>::iterator itFind;

	itFind = m_mapGroupArray.find(dwName);

	if (itFind != m_mapGroupArray.end())
	{
		return itFind->second;
	}
	return NULL;
}
void GameModel::_DEBUG_RenderLocator(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();

	//根据显示组件表取得组件，渲染

	for(std::map<DWORD,Locator *>::iterator itLocator = m_mapLocatorArray.begin(); itLocator != m_mapLocatorArray.end();itLocator++)
	{
		GameModel::Locator *pLocator = itLocator->second;
		model::Buffer *pBuffer = pLocator->GetBuffer();
		pBuffer->Render(m_dwShaderGroupHandle,0);
	}	
}



GameModel::GameModel()
{
	m_bSizeBoxAutoMake = FALSE;
	m_pSizeBox		   = NULL;
	m_bPickBoxAutoMake = FALSE;
	m_pPickBox		   = NULL;
	m_pPickMesh		   = NULL;
	m_pBindPose		   = NULL;
	m_dwShaderGroupHandle = 0;

	m_pWater = NULL;
}

GameModel::~GameModel()
{
	ASSERT(!m_pPickBox);
	ASSERT(!m_pSizeBox);
	ASSERT(!m_pBindPose);

	m_pWater = NULL;
}


void GameModel::SetModelColor(DWORD dwColor)
{
	//根据显示组件表取得组件
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second;

		GameModel::Group::Part * pPart = pGroup->GetSelectPart();
		if (pPart)
		{
			pPart->SetColor(dwColor);			
		}
	}
}

void GameModel::RenderModelBlend(void)
{
	//根据显示组件表取得组件，渲染
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second;
		if (pGroup->IsVisible())
		{
			GameModel::Group::Part * pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				pPart->RenderBlend(m_dwShaderGroupHandle);
			}
		}
	}
}

void GameModel::DrawModel(void)
{
	//根据显示组件表取得组件，渲染
	std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin();
	for( ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second;
		if (pGroup->IsVisible())
		{
			GameModel::Group::Part * pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				pPart->DrawAlphaTestOnly(m_dwShaderGroupHandle);
			}
		}
	}
	//根据显示组件表取得组件，渲染混合效果
	for(itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second;
		if (pGroup->IsVisible())
		{
			GameModel::Group::Part * pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				pPart->DrawAlphaBlendOnly(m_dwShaderGroupHandle);
				//pPart->DrawTransparent(m_dwShaderGroupHandle);
			}
		}
	}
}
void GameModel::DrawModelTransparent(void)
{
	//根据显示组件表取得组件，渲染
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second;
		if (pGroup->IsVisible())
		{
			GameModel::Group::Part * pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				pPart->DrawTransparent(m_dwShaderGroupHandle);
			}
		}
	}
}

//CLASS____(GameModel::AnimInfo)____
GameModel::AnimInfo::AnimInfo()
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matBillboard);
	D3DXMatrixIdentity(&m_matBillboardY);

	m_dwAnimColorStartTime = 0;
	ZeroMemory(&m_tActionInfo,sizeof(tagActionInfo));
	ZeroMemory(&m_dwParticleEmitterLastTimeArray,GAMEMODEL_MAXPARTICLEEMITTER);

	m_tActionInfo.fCurActionSpeedRate = 1.0f;
	m_tActionInfo.fOldActionSpeedRate = 1.0f;

	//容器
	m_pTrailContainerArray = new TrailContainer[GAMEMODEL_MAXTRAILEMITTER];
}

GameModel::AnimInfo::~AnimInfo()
{
	SAFEDELETEARRAY(m_pTrailContainerArray);
}

//容器,接受发射器的粒子
GameModel::TrailContainer::TrailContainer()
{
	m_pTrailUsed = NULL;
	m_pTrailFree = NULL;
}

GameModel::TrailContainer::~TrailContainer()
{
	//TODo
	tagTrail *pTrail,*p;

	pTrail = m_pTrailUsed;	
	while(pTrail)
	{
		p = pTrail;
		pTrail = pTrail->pNext;
		SAFEDELETE(p);
	}

	pTrail = m_pTrailFree;	
	while(pTrail)
	{
		p = pTrail;
		pTrail = pTrail->pNext;
		SAFEDELETE(p);
	}
}

void GameModel::TrailContainer::Update(DWORD dwCurrentTime)
{
	//取得刷新时间
	m_pTrailRenderHead = NULL;

	tagTrail *pTrail,**ppTrail = &m_pTrailUsed;
	while(*ppTrail)
	{
		pTrail = *ppTrail;
		//计算过去的时间累加

		DWORD dwTimeElapse = dwCurrentTime - pTrail->dwStartTime;

		//减少浓度,匀速运动
		pTrail->fFade = 1.0f - (float)dwTimeElapse / (float)pTrail->dwTimeLength;

		if (dwTimeElapse > pTrail->dwTimeLength)
		{
			//死亡
			*ppTrail = pTrail->pNext;
			pTrail->pNext = m_pTrailFree;
			m_pTrailFree = pTrail;
		}
		else
		{
			//装入渲染链
			pTrail->pNextRender = m_pTrailRenderHead;
			m_pTrailRenderHead = pTrail;

			//呃,来人啦，下一个
			ppTrail = &pTrail->pNext;
		}
	}
}

void GameModel::TrailContainer::Clear()
{
	//取得刷新时间
	m_pTrailRenderHead = NULL;

	tagTrail *pTrail,**ppTrail = &m_pTrailUsed;
	while(*ppTrail)
	{
		pTrail = *ppTrail;
		*ppTrail = pTrail->pNext;
		pTrail->pNext = m_pTrailFree;
		m_pTrailFree = pTrail;
	}
}

void GameModel::TrailContainer::Render(DWORD dwShaderGroupHandle,DWORD dwShaderIndex)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	static D3DXVECTOR3 vPosBuffer[6];
	static D3DXVECTOR2 vTexBuffer[6];
	static DWORD color[6];   	

	tagTrail *pTrailPre = m_pTrailRenderHead;
	if (!pTrailPre)
		return ;

	tagTrail *pTrail = pTrailPre->pNextRender;

	while(pTrail)
	{
		//颜色浓度运算
		float fTA = (float)( (pTrail->dwTopColor & 0xFF000000) >> 24);
		float fTR = (float)( (pTrail->dwTopColor & 0x00FF0000) >> 16);
		float fTG = (float)( (pTrail->dwTopColor & 0x0000FF00) >>  8);
		float fTB = (float)(  pTrail->dwTopColor & 0x000000FF);

		DWORD c = (DWORD)( fTA * pTrail->fFade);
		DWORD dwTopColor = c << 24;
		c = (DWORD)( fTR * pTrail->fFade); 
		dwTopColor |= c << 16;
		c = (DWORD)( fTG * pTrail->fFade);
		dwTopColor |= c << 8;
		c = (DWORD)( fTB * pTrail->fFade);
		dwTopColor |= c;

		//颜色浓度运算
		float fBA = (float)( (pTrail->dwBottomColor & 0xFF000000) >> 24);
		float fBR = (float)( (pTrail->dwBottomColor & 0x00FF0000) >> 16);
		float fBG = (float)( (pTrail->dwBottomColor & 0x0000FF00) >>  8);
		float fBB = (float)(  pTrail->dwBottomColor & 0x000000FF);

		c = (DWORD)( fBA * pTrail->fFade);
		DWORD dwBottomColor = c << 24;
		c = (DWORD)( fBR * pTrail->fFade); 
		dwBottomColor |= c << 16;
		c = (DWORD)( fBG * pTrail->fFade);
		dwBottomColor |= c << 8;
		c = (DWORD)( fBB * pTrail->fFade);
		dwBottomColor |= c;

		color[0] = color[1] = color[3] = dwBottomColor;
		color[2] = color[4] = color[5] = dwTopColor;

		vTexBuffer[0].x = 0.0f;
		vTexBuffer[0].y = 1.0f;

		vTexBuffer[1].x = 1.0f;
		vTexBuffer[1].y = 1.0f;

		vTexBuffer[2].x = 1.0f;
		vTexBuffer[2].y = 0.0f;

		vTexBuffer[3].x = 0.0f;
		vTexBuffer[3].y = 1.0f;

		vTexBuffer[4].x = 1.0f;
		vTexBuffer[4].y = 0.0f;

		vTexBuffer[5].x = 0.0f;
		vTexBuffer[5].y = 0.0f;

		vPosBuffer[0] = pTrailPre->vPos[0];
		vPosBuffer[1] = pTrail->vPos[0];
		vPosBuffer[2] = pTrailPre->vPos[1];

		vPosBuffer[3] = pTrail->vPos[0];
		vPosBuffer[4] = pTrail->vPos[1];
		vPosBuffer[5] = pTrailPre->vPos[1];

		pLayer3D->SendTriangleToCache(dwShaderGroupHandle,dwShaderIndex,2,vPosBuffer,vTexBuffer,color,1);

		pTrailPre = pTrail;
		pTrail = pTrail->pNextRender;
	}
}

GameModel::TrailContainer::tagTrail *GameModel::TrailContainer::GetFreeTrail()
{
	tagTrail *pNewTrail = NULL;

	//检查是否有空余的
	if (m_pTrailFree)
	{
		pNewTrail = m_pTrailFree;
		m_pTrailFree = m_pTrailFree->pNext;
	}
	else
	{
		//没有
		pNewTrail = new tagTrail;
	}

	//加入活动列表
	pNewTrail->pNext = m_pTrailUsed;
	m_pTrailUsed = pNewTrail;    
	return pNewTrail;
}

//Locator
GameModel::Locator * GameModel::GetLocator(DWORD dwLocatorName)
{
	std::map<DWORD,Locator*>::iterator itFind;

	itFind = m_mapLocatorArray.find(dwLocatorName);

	if (itFind != m_mapLocatorArray.end())
	{
		return itFind->second;
	}
	return NULL;
}


void GameModel::EnableParticleEmitter(BOOL bValue)
{
	for(std::map<DWORD,ParticleEmitter *>::iterator itEmitter = m_mapParticleEmitterArray.begin(); itEmitter != m_mapParticleEmitterArray.end();itEmitter++)
	{
		ParticleEmitter *pEmitter = itEmitter->second;
		pEmitter->Enable(bValue);
	}
}

void GameModel::EnableParticleEmitter(DWORD dwName,BOOL bValue)
{
	std::map<DWORD,ParticleEmitter*>::iterator itFind;

	itFind = m_mapParticleEmitterArray.find(dwName);

	if (itFind != m_mapParticleEmitterArray.end())
	{
		ParticleEmitter *pEmitter = itFind->second;
		pEmitter->Enable(bValue);
	}
}
BOOL GameModel::TextureProjective::IsEnable()
{
	return m_ptexproj->IsVisable();
}
void GameModel::TextureProjective::Enable(BOOL bvalue)
{
	m_ptexproj->SetVisiable(bvalue);
}
void GameModel::EnableTextureProjective(BOOL bValue)
{
	for(std::map<DWORD,TextureProjective*>::iterator it = m_maptextureprojectivearray.begin();it != m_maptextureprojectivearray.end();it++)
	{
		TextureProjective *ptp = it->second;
		ptp->Enable(bValue);
	}
}



void GameModel::EnableTrailEmitter(BOOL bValue)
{
	for(std::map<DWORD,TrailEmitter *>::iterator itEmitter = m_mapTrailEmitterArray.begin(); itEmitter != m_mapTrailEmitterArray.end();itEmitter++)
	{
		TrailEmitter *pEmitter = itEmitter->second;
		pEmitter->Enable(bValue);
	}
}

void GameModel::EnableTrailEmitter(DWORD dwName,BOOL bValue)
{
	std::map<DWORD,TrailEmitter*>::iterator itFind;

	itFind = m_mapTrailEmitterArray.find(dwName);

	if (itFind != m_mapTrailEmitterArray.end())
	{
		TrailEmitter *pEmitter = itFind->second;
		pEmitter->Enable(bValue);
	}
}


//CLASS____(GameModel::ParticleEmitter)____
void GameModel::ParticleEmitter::Create(utility::Text::Node *pNode,const char *szResourcePath)
{
	m_eLoad = Load_Not;
	m_bEnable = TRUE;
	//Make name code
	m_dwName = 0;
	strncpy((char*)&m_dwName,pNode->GetName(),4);

	//读取定位器名字

	static const char * szTypeString[] = { "POINT","LINE"};
	m_dwType = (DWORD)pNode->GetVar("type")->GetEnumValue(szTypeString,2);
	m_dwEmittNumber = pNode->GetVar("number")? pNode->GetVar("number")->GetIntValue() : 1;

	//读取粒子发射器模型
	static char szTemp[_MAX_PATH];
	utility::Text::Variable *pTempVar = pNode->GetVar("shape");
	ASSERT(pTempVar && "发射器模型文件未指定");
	sprintf(szTemp,"%s\\%s",szResourcePath,pTempVar->GetStringValue());	
	m_pMesh = new model::Mesh;
	if (!m_pMesh->Create(szTemp))
	{
		OutputConsole("error: Mesh 文件 建立失败: %s\n",szTemp);
		ASSERT(0);
	}

	m_pBuffer = new model::Buffer;
	m_pBuffer->Create(m_pMesh);

	pTempVar = pNode->GetVar("particle");
	ASSERT(pTempVar && "发射器粒子文件未指定");
	sprintf(szTemp,"%s\\%s",szResourcePath,pTempVar->GetStringValue());	
	m_pParticleEmitter = new particle::Emitter;
	m_pParticleEmitter->CreateFromFile(szTemp);	

}

void GameModel::ParticleEmitter::Create(utility::Text::Node *pNode)
{
	m_bEnable = TRUE;
	//Make name code
	m_dwName = 0;
	strncpy((char*)&m_dwName,pNode->GetName(),4);
	//读取定位器名字

	static const char * szTypeString[] = { "POINT","LINE"};
	m_dwType = (DWORD)pNode->GetVar("type")->GetEnumValue(szTypeString,2);
	m_dwEmittNumber = pNode->GetVar("number")? pNode->GetVar("number")->GetIntValue() : 1;

	//读取粒子发射器模型
	utility::Text::Variable *pTempVar = pNode->GetVar("shape");
	ASSERT(pTempVar && "发射器模型文件未指定");
	sprintf(m_pszMeshFileName,"%s",pTempVar->GetStringValue());

	m_pMesh = NULL;
	m_pBuffer = NULL;

	pTempVar = pNode->GetVar("particle");
	ASSERT(pTempVar);
	sprintf(m_pszEmitterFileName,"%s",pTempVar->GetStringValue());	
	m_pParticleEmitter = new particle::Emitter;
	particle::Manager::GetInstance()->AddSourcelist(m_pParticleEmitter);
	m_pParticleEmitter->SetLoadState(Load_Not);
}

void GameModel::ParticleEmitter::CreateMesh(CRFile *pFile)
{
	m_pMesh = new model::Mesh;
	if (!m_pMesh->Create(pFile))
	{
		OutputConsole("error: Mesh 文件 建立失败: %s\n",pFile->GetFileName());
		ASSERT(0);
	}
	m_pBuffer = new model::Buffer;
	m_pBuffer->Create(m_pMesh);
	m_pBuffer->SetGpuCaculate(false);
}
void GameModel::ParticleEmitter::CloneEmitter(ParticleEmitter* pParticleEmitter)
{
	m_dwName = pParticleEmitter->m_dwName;
	m_dwType = pParticleEmitter->m_dwType;
	m_dwEmittNumber = pParticleEmitter->m_dwEmittNumber;

	strncpy(m_pszMeshFileName,pParticleEmitter->m_pszMeshFileName,MAX_PATH);
	m_pMesh = pParticleEmitter->m_pMesh;
	m_pBuffer = pParticleEmitter->m_pBuffer;

	//粒子
	strncpy(m_pszEmitterFileName,pParticleEmitter->m_pszEmitterFileName,MAX_PATH);
	m_pParticleEmitter = new particle::Emitter;
	m_pParticleEmitter->CloneEmitter(pParticleEmitter->GetParticleEmitter()); 
	//打开
	m_bEnable = pParticleEmitter->m_bEnable;
	m_eLoad = pParticleEmitter->m_eLoad;


}	
void GameModel::ParticleEmitter::Destroy()
{
	//销毁
	SAFEDESTROY(m_pMesh);
	SAFEDESTROY(m_pBuffer);

	m_pParticleEmitter->SetReadyToDie();
}					   
void GameModel::TextureProjective::Create(utility::Text::Node *pNode)
{
	//m_bEnable = TRUE;
	//Make name code
	//static char szTemp[_MAX_PATH];
	m_dwName = 0;
	//strncpy((char*)&m_dwName,pNode->GetName(),4);
	utility::Text::Variable *pTempVar = pNode->GetVar("texproj");
	ASSERT(pTempVar && "投影纹理文件未指定");
	sprintf(m_pszTextureProjFileName,"%s",pTempVar->GetStringValue());
	m_ptexproj = CTextureProjective::CreateTextureProjctive();
	m_ptexproj->SetLoadState(Load_Not);

}

void GameModel::TextureProjective::Destroy()
{
	//销毁
	m_ptexproj->SetAlive(false);
}
//CLASS____(GameModel::TrailEmitter)____
BOOL GameModel::TrailEmitter::Create(utility::Text::Node *pNode,const char *szResourcePath)
{
	m_bEnable = TRUE;

	strncpy(( char*)&m_dwName,pNode->GetName(),4);
	m_bEnable = FALSE;
	m_dwCurColorIndex  = MAKEFOURCC('0','0','0','0');

	char szTemp[_MAX_PATH];
	utility::Text::Variable *pTempVar;

	pTempVar = pNode->GetVar("shape");
	ASSERT(pTempVar && "shape 未指定");
	sprintf(szTemp,"%s\\%s",szResourcePath,pTempVar->GetStringValue());	
	//读取边界形状
	m_pMesh = new model::Mesh;
	m_pMesh->Create(szTemp);

	m_pBuffer = new model::Buffer;
	m_pBuffer->Create(m_pMesh);

	//默认数据
	pTempVar = pNode->GetVar("trail");
	ASSERT(pTempVar && "trail文件未指定");
	sprintf(szTemp,"%s\\%s",szResourcePath,pTempVar->GetStringValue());	

	utility::File file;
	if (file.Open(szTemp))
	{
		utility::Text script;
		script.Create(&file);

		m_dwLifeTimeLength = (DWORD)script.GetRootNode()->GetVar("timelength")->GetIntValue();
		utility::Text::Node *pColorNode=  script.GetRootNode()->GetChildNode("color");
		if (pColorNode)
		{
			typedef std::pair<DWORD,__int64> COLOR_PAIR;
			for(DWORD n = 0; n < pColorNode->GetChildCount();n++)
			{
				utility::Text::Node *pTempNode = pColorNode->GetChildNode(n);
				unsigned __int64 qwColor = pTempNode->GetVar("top_color")->GetHexValue();
				qwColor = (qwColor << 32) | pTempNode->GetVar("bottom_color")->GetHexValue();
				m_mapColor.insert(COLOR_PAIR(*(DWORD*)pTempNode->GetName(),qwColor));
			}
		}
		script.Destroy();
		file.Close();
		return TRUE;
	}

	return FALSE;
}

BOOL   GameModel::TrailEmitter::Create(utility::Text::Node *pNode)
{
	m_eLoad = Load_Not;

	m_bEnable = TRUE;

	strncpy(( char*)&m_dwName,pNode->GetName(),4);
	m_bEnable = FALSE;
	m_dwCurColorIndex  = MAKEFOURCC('0','0','0','0');

	utility::Text::Variable *pTempVar;

	pTempVar = pNode->GetVar("shape");
	ASSERT(pTempVar);
	sprintf(m_pszMeshFileName,"%s",pTempVar->GetStringValue());	
	//读取边界形状
	m_pMesh = NULL;
	m_pBuffer = NULL;
	//默认数据
	pTempVar = pNode->GetVar("trail");
	ASSERT(pTempVar);
	sprintf(m_pszColorFileName,"%s",pTempVar->GetStringValue());	
	return TRUE;
}

void   GameModel::TrailEmitter::CreateMesh(CRFile *pFile)
{
	if(NULL == pFile)	return;
	//读取边界形状
	m_pMesh = new model::Mesh;
	m_pMesh->Create(pFile);

	m_pBuffer = new model::Buffer;
	m_pBuffer->Create(m_pMesh);
}

BOOL   GameModel::TrailEmitter::CreateColor(CRFile* pFile)
{
	utility::Text script;
	script.Create(pFile);

	m_dwLifeTimeLength = (DWORD)script.GetRootNode()->GetVar("timelength")->GetIntValue();
	utility::Text::Node *pColorNode=  script.GetRootNode()->GetChildNode("color");
	if (pColorNode)
	{
		typedef std::pair<DWORD,__int64> COLOR_PAIR;
		for(DWORD n = 0; n < pColorNode->GetChildCount();n++)
		{
			utility::Text::Node *pTempNode = pColorNode->GetChildNode(n);
			unsigned __int64 qwColor = pTempNode->GetVar("top_color")->GetHexValue();
			qwColor = (qwColor << 32) | pTempNode->GetVar("bottom_color")->GetHexValue();
			m_mapColor.insert(COLOR_PAIR(*(DWORD*)pTempNode->GetName(),qwColor));
		}
	}
	script.Destroy();
	return TRUE;
}

unsigned __int64 GameModel::TrailEmitter::GetColor()
{
	std::map<DWORD,unsigned __int64>::iterator itFind;
	itFind = m_mapColor.find(m_dwCurColorIndex);
	if (itFind == m_mapColor.end())
	{
		return 0xFFFFFFFFFFFFFFFF;
	}
	return itFind->second;
}

void GameModel::TrailEmitter::Destroy()
{
	//销毁
	SAFEDESTROY(m_pMesh);
	SAFEDESTROY(m_pBuffer);
	m_mapColor.clear();
}

void GameModel::MultiplyModelColor(DWORD dwColor)
{
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second;

		GameModel::Group::Part * pPart = pGroup->GetSelectPart();
		if (pPart)
		{
			pPart->MultiplyColor(dwColor);            
		}
	}
}

void GameModel::ProcessLocator(const GameModel::AnimInfo *pAnimInfo)
{
	//当前激活的动画  
	const AnimInfo::tagActionInfo *pActionInfo = pAnimInfo->GetActionInfo();
	DWORD  dwCurrentTime = pAnimInfo->GetCurrentTime();

	const D3DXMATRIX *pWorldMatrix = pAnimInfo->GetWorldMatrix();
	const D3DXMATRIX *pBillboard = pAnimInfo->GetBillboard();
	const D3DXMATRIX *pBillboardY = pAnimInfo->GetBillboardY();

	Action *pOldAction = GetAction(pActionInfo->dwOldAction);
	Action *pCurAction = GetAction(pActionInfo->dwCurAction);	

	model::AnimJoint *pOldAnimJoint = (pOldAction)?  pOldAction->GetAnimJoint() : NULL;	
	model::AnimJoint *pCurAnimJoint = (pCurAction)?  pCurAction->GetAnimJoint() : NULL;	

	DWORD dwCurActionTimeElapse = pActionInfo->dwCurActionElapseTime;
	DWORD dwOldActionTimeElapse = pActionInfo->dwOldActionTimeElapse;

	DWORD dwInterplationTimeElapse = pActionInfo->dwInterpolationElapseTime;

	float fRatio = (float)dwInterplationTimeElapse / (float)pActionInfo->dwInterpolationTimeLength;

	//根据显示组件表取得组件计算拷贝缺省数据
	for(std::map<DWORD,Locator *>::iterator itLocator = m_mapLocatorArray.begin(); itLocator != m_mapLocatorArray.end();itLocator++)
	{
		Locator *pLocator = itLocator->second;
		D3DXMATRIX matnowworld,matnowmodel;
		model::Mesh   *pMesh   = pLocator->GetMesh();
		model::Buffer *pBuffer = pLocator->GetBuffer();
		if (pActionInfo->bInterpolation && pOldAnimJoint && pCurAnimJoint&&pMesh->GetGroupCount())
		{
			pMesh->ProcessAnimJointInterpolation(pBuffer,pAnimInfo->GetWorldMatrix(),pAnimInfo->GetViewMatrix(),pAnimInfo->GetProjectedMatrix(),m_pBindPose,pOldAnimJoint,dwOldActionTimeElapse,pActionInfo->fOldActionSpeedRate,pCurAnimJoint,0,1.0f,fRatio,matnowworld,matnowmodel); 
		}
		else
		{
			if (pCurAnimJoint&&pMesh->GetGroupCount())
			{
				pMesh->ProcessAnimJoint(pBuffer,pAnimInfo->GetWorldMatrix(),pAnimInfo->GetViewMatrix(),pAnimInfo->GetProjectedMatrix(),m_pBindPose,pCurAnimJoint,dwCurActionTimeElapse,pActionInfo->fCurActionSpeedRate,matnowworld,matnowmodel);
			}	
			else
			{
				D3DXMatrixIdentity(&matnowworld);
				D3DXMatrixIdentity(&matnowmodel);
				pMesh->CopyPosition(pBuffer,pAnimInfo->GetWorldMatrix(),pAnimInfo->GetViewMatrix(),pAnimInfo->GetProjectedMatrix());
			}
		}
		pLocator->SetNowWorldMatrix(matnowworld);
		pLocator->SetNowModelMatrix(matnowmodel);
	}
}

void GameModel::Group::Part::Render(DWORD dwShaderGroupHandle)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
		for(int i = 0; i <pLevel->GetRMCount(); i++)
		{
			tagRenderMesh *pRenderMesh = vRenderMeshes[i];

			model::Buffer *pBuffer = pRenderMesh->pBuffer;

			pBuffer->Render(dwShaderGroupHandle,pRenderMesh->dwAlphaMode);
		}
	}
}
void GameModel::Group::Part::RenderTransparent(DWORD dwShaderGroupHandle, DWORD dwMode)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
		for(int i = 0; i <pLevel->GetRMCount(); i++)
		{
			tagRenderMesh *pRenderMesh = vRenderMeshes[i];

			model::Buffer *pBuffer = pRenderMesh->pBuffer;
			pBuffer->RenderTransparent(dwShaderGroupHandle,dwMode);
		}
	}
}
void GameModel::Group::Part::_DEBUG_RenderNormal()
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
		for(int i = 0; i <pLevel->GetRMCount(); i++)
		{
			tagRenderMesh *pRenderMesh = vRenderMeshes[i];
			pRenderMesh->pBuffer->_DEBUG_RenderNormal();
		}
	}
}

void GameModel::Group::Part::RenderBlend(DWORD dwShaderGroupHandle)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
		for(int i = 0; i <pLevel->GetRMCount(); i++)
		{
			tagRenderMesh *pRenderMesh = vRenderMeshes[i];

			model::Buffer *pBuffer = pRenderMesh->pBuffer;

			pBuffer->Render(dwShaderGroupHandle,1);
		}
	}
}

//void GameModel::Group::Part::Draw(DWORD dwShaderGroupHandle)
//{
//	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
//	{
//		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
//		for(int i = 0; i < pLevel->GetRMIndexCount(); i++)
//		{
//			DWORD dwIndex = pLevel->GetRMIndex(i);
//			tagRenderMesh *pRenderMesh = &m_pRenderMeshArray[dwIndex];
//			model::Buffer *pBuffer = pRenderMesh->pBuffer;
//			model::Buffer *pRenderBuffer= pRenderMesh->pRenderBuffer;
//			*pRenderBuffer = *pBuffer;
//			pRenderBuffer->Draw(dwShaderGroupHandle,pRenderMesh->dwAlphaMode);
//
//		}
//	}
//}

void GameModel::Group::Part::DrawAlphaTestOnly(DWORD dwShaderGroupHandle)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
		for(int i = 0; i <pLevel->GetRMCount(); i++)
		{
			tagRenderMesh *pRenderMesh = vRenderMeshes[i];
			if (pRenderMesh->dwAlphaMode == 0)
			{
				model::Buffer *pBuffer = pRenderMesh->pBuffer;

				pBuffer->Draw(dwShaderGroupHandle,0);
			}
		}
	}
}

void GameModel::Group::Part::DrawAlphaBlendOnly(DWORD dwShaderGroupHandle)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
		for(int i = 0; i <pLevel->GetRMCount(); i++)
		{
			tagRenderMesh *pRenderMesh = vRenderMeshes[i];
			if (pRenderMesh->dwAlphaMode == 1)
			{
				model::Buffer *pBuffer = pRenderMesh->pBuffer;

				pBuffer->Draw(dwShaderGroupHandle,1);
			}
		}
	}
}

void GameModel::Group::Part::DrawTransparent(DWORD dwShaderGroupHandle)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
		for(int i = 0; i <pLevel->GetRMCount(); i++)
		{
			tagRenderMesh *pRenderMesh = vRenderMeshes[i];
			if (pRenderMesh->dwAlphaMode)
			{
				continue;
			}
			model::Buffer *pBuffer = pRenderMesh->pBuffer;

			pBuffer->DrawTransparent(dwShaderGroupHandle);
		}
	}
}


void GameModel::Group::Part::ProcessAnimJoint(const GameModel::AnimInfo *pAnimInfo,model::AnimJoint * pOldAnimJoint,model::AnimJoint *pCurAnimJoint,DWORD dwOldActionTimeElapse,DWORD dwCurActionTimeElapse,model::BindPose *pBindPose,float fRatio)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		const GameModel::AnimInfo::tagActionInfo *pActionInfo = pAnimInfo->GetActionInfo();

		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
		for(int i = 0; i <pLevel->GetRMCount(); i++)
		{
			tagRenderMesh *pRenderMesh = vRenderMeshes[i];

			model::Mesh   *pMesh   = pRenderMesh->pMesh;
			model::Buffer *pBuffer = pRenderMesh->pBuffer;
			if (pActionInfo->bInterpolation && pOldAnimJoint && pCurAnimJoint&&pMesh->GetGroupCount())
				pMesh->ProcessAnimJointInterpolation(pBuffer,pAnimInfo->GetWorldMatrix(),pAnimInfo->GetViewMatrix(),pAnimInfo->GetProjectedMatrix(),pBindPose,pOldAnimJoint,dwOldActionTimeElapse,pActionInfo->fOldActionSpeedRate,pCurAnimJoint,0,1.0f,fRatio); 
			else
				if (pCurAnimJoint&&pMesh->GetGroupCount())
					pMesh->ProcessAnimJoint(pBuffer,pAnimInfo->GetWorldMatrix(),pAnimInfo->GetViewMatrix(),pAnimInfo->GetProjectedMatrix(),pBindPose,pCurAnimJoint,dwCurActionTimeElapse,pActionInfo->fCurActionSpeedRate);
				else
					pMesh->CopyPosition(pBuffer,pAnimInfo->GetWorldMatrix(),pAnimInfo->GetViewMatrix(),pAnimInfo->GetProjectedMatrix());

			pMesh->CopyColor(pBuffer);
			pMesh->CopyUV(pBuffer);  

			if (pRenderMesh->bBillboard)
			{
				if (pRenderMesh->bAroundYBillboard)
				{
					CalculateBillboard(pMesh,pBuffer,pAnimInfo->GetBillboardY());
				}
				else
					CalculateBillboard(pMesh,pBuffer,pAnimInfo->GetBillboard());
			}
		}
	}
}

void GameModel::Group::Part::CalculateBillboard(const model::Mesh *pMesh,model::Buffer * pBuffer,const D3DXMATRIX *pBillboarMatrix)
{
	D3DXMATRIX matBillBoard;
	memcpy(&matBillBoard,pBillboarMatrix,sizeof(D3DXMATRIX));
	float x(0),y(0),z(0);
	float cx(0),cy(0),cz(0);

	D3DXMATRIX matTransform;
	D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();
	D3DXVECTOR3 *pMeshPosBuffer = pMesh->GetPosBuffer();

	DWORD n = 0;
	for(; n < pBuffer->GetPNCCount(); n++)
	{
		x += pPosBuffer[n].x;
		y += pPosBuffer[n].y;
		z += pPosBuffer[n].z;

		cx += pMeshPosBuffer[n].x;
		cy += pMeshPosBuffer[n].y;
		cz += pMeshPosBuffer[n].z;
	}

	float reciprocal = 1.0f / (float)n;

	x *= reciprocal;
	y *= reciprocal;
	z *= reciprocal;

	cx *= - reciprocal;
	cy *= - reciprocal;
	cz *= - reciprocal;

	matBillBoard._41 = x;
	matBillBoard._42 = y;
	matBillBoard._43 = z;

	D3DXMatrixTranslation(&matTransform,cx,cy,cz);
	D3DXMatrixMultiply(&matTransform,&matTransform,&matBillBoard);

	for(n = 0; n < pBuffer->GetPNCCount(); n++)
	{
		D3DXVECTOR3 &vb = pPosBuffer[n];	
		D3DXVECTOR3 &vm = pMeshPosBuffer[n];
		D3DXVec3TransformCoord(&vb,&vm,&matTransform);
	}
}

void GameModel::Group::Part::ProcessPointLight(const D3DXVECTOR3 * pLightPosition, float fIntensity, DWORD dwColor)
{
	float fR = (float)( (dwColor & 0x00FF0000) >> 16) / 255.0f;
	float fG = (float)( (dwColor & 0x0000FF00) >>  8) / 255.0f;
	float fB = (float)(  dwColor & 0x000000FF)        / 255.0f;

	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
		for(int i = 0; i <pLevel->GetRMCount(); i++)
		{
			tagRenderMesh *pRenderMesh = vRenderMeshes[i];
			model::Buffer *pBuffer = pRenderMesh->pBuffer;
			D3DXVECTOR3   *pPosBuffer  = pBuffer->GetPosBuffer();
			D3DXVECTOR3   *pNmlBuffer  = pBuffer->GetNmlBuffer();
			D3DCOLOR	  *pColBufferDiffuse  = pBuffer->GetColBufferDiffuse();

			DWORD dwColCount = pBuffer->GetPNCCount();

			for(DWORD n = 0; n < dwColCount;n++)
			{
				DWORD &dwDestColor = pColBufferDiffuse[n];

				D3DXVECTOR3 L = pPosBuffer[n] - *pLightPosition;
				float fLightDistance = D3DXVec3Length(&L);		//光源到vertex的距离
				if (fLightDistance == 0.0f)
					fLightDistance = 0.001f;

				D3DXVec3Normalize(&L,&L);
				float DOT = - D3DXVec3Dot(&L,&pNmlBuffer[n]);// L . N

				if (DOT > 0)//光线在面的正面
				{	
					float fValue = fIntensity / fLightDistance * DOT;

					DWORD r = (dwDestColor & 0x00FF0000) >> 16;
					DWORD g = (dwDestColor & 0x0000FF00) >> 8;
					DWORD b = (dwDestColor & 0x000000FF);

					r = r + (DWORD)(( fR * fValue) * 255.0f);  	
					if (r > 255) r = 0x000000FF;
					g = g + (DWORD)(( fG * fValue) * 255.0f);  	
					if (g > 255) g = 0x000000FF;
					b = b + (DWORD)(( fB * fValue) * 255.0f);  	
					if (b > 255) b = 0x000000FF;
					dwDestColor = (dwDestColor & 0xFF000000) | (r << 16) | (g << 8) | b;
				}                                    
			}
		}
	}
}

void GameModel::Group::Part::MultiplyColor(DWORD dwColor)
{
	if (m_dwSelectLevel >= m_dwLevelCount)
		return;

	DWORD  dwR = (dwColor & 0x00FF0000) >> 16;
	DWORD  dwG = (dwColor & 0x0000FF00) >>  8;
	DWORD  dwB = dwColor & 0x000000FF;

	Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
	LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
	for(int i = 0; i <pLevel->GetRMCount(); i++)
	{
		tagRenderMesh *pRenderMesh = vRenderMeshes[i];

		if (pRenderMesh->bEffectByLight)
		{
			model::Buffer *pBuffer = pRenderMesh->pBuffer;

			D3DXVECTOR3   *pPosBuffer  = pBuffer->GetPosBuffer();
			D3DXVECTOR3   *pNmlBuffer  = pBuffer->GetNmlBuffer();
			D3DCOLOR	  *pColBuffer  = pBuffer->GetColBufferDiffuse();

			DWORD dwColCount = pBuffer->GetPNCCount();

			for(DWORD n = 0; n < dwColCount;n++)
			{
				DWORD &dwDestColor = pColBuffer[n];

				DWORD dwRD =  (dwDestColor & 0x00FF0000) >> 16;
				DWORD dwGD =  (dwDestColor & 0x0000FF00) >>  8;
				DWORD dwBD =  (dwDestColor & 0x000000FF);

				DWORD r = (dwR * dwRD ) >> 8;  	
				DWORD g = (dwG * dwGD ) >> 8;  	
				DWORD b = (dwB * dwBD ) >> 8;  	
				dwDestColor = (dwDestColor & 0xFF000000) | (r << 16) | (g << 8) | b;
			}
		}
	}
}

void GameModel::Group::Part::SetColor(DWORD dwColor)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{

		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
		for(int i = 0; i <pLevel->GetRMCount(); i++)
		{
			tagRenderMesh *pRenderMesh = vRenderMeshes[i];

			//if (pRenderMesh->dwAlphaMode == 0 && pRenderMesh->bEffectByLight)
			if (pRenderMesh->bEffectByLight)
			{
				model::Buffer *pBuffer = pRenderMesh->pBuffer;

				D3DCOLOR	  *pColBuffer = pBuffer->GetColBufferDiffuse();

				DWORD dwColCount = pBuffer->GetPNCCount();

				_asm
				{
					mov edi,[pColBuffer]
					mov eax,dwColor
						cld 
						mov ecx,dwColCount
						rep stosd
				}
			}
		}
	}
}

void GameModel::Group::Part::SetLevel(DWORD dwLevel)
{
	if (dwLevel >= m_dwLevelCount)
		m_dwSelectLevel = m_dwLevelCount - 1;
	else
		m_dwSelectLevel = dwLevel;
}



/*
* 功能: 得到模型部件的等级信息
* 摘要: 实现了得到指定等级和随机索引等级功能
* 参数: dwLevel - 需要得到的等级索引(从0开始,如果等于0xffffffff则返回的是随机索引的等级信息,注意最终得到的等级信息也从这里返回)
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.09.12 - lpf
*		增加了得到随机索引的等级信息功能
*	2008.10.25 - lpf
*		修改了参数dwLevel类型，如果传入的是0xffffffff，则该参数在函数执行后会返回随机等级的结果
*/
GameModel::Group::Part::Level * GameModel::Group::Part::GetLevel(DWORD & dwLevel)
{
	if (dwLevel < m_dwLevelCount)
		return &m_pLevelArray[dwLevel];
	else if (dwLevel == 0xffffffff)
	{
		dwLevel = (DWORD)random(m_dwLevelCount);
		if (dwLevel < m_dwLevelCount)
			return &m_pLevelArray[dwLevel];
	}

	return NULL;
}



GameModel::Group::Part::tagRenderMesh *GameModel::Group::Part::GetRenderMesh(DWORD dwID)
{
	if(dwID >= m_dwRenderMeshCount) return NULL;
	return &m_pRenderMeshArray[dwID];
}


void GameModel::Group::Part::GetRenderMeshes(DWORD dwLevel,Level *&pLevel,LevelMeshes& RenderMeshes)
{
}

//void GameModel::Group::Part::ProcessDirectionalLight(const D3DXVECTOR3 *pLightDirection,DWORD dwColor)
//{
//	float fR = (float)( (dwColor & 0x00FF0000) >> 16) / 255.0f;
//	float fG = (float)( (dwColor & 0x0000FF00) >>  8) / 255.0f;
//	float fB = (float)(  dwColor & 0x000000FF)        / 255.0f;
//
//	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
//	{
//		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
//		for(int i = 0; i < pLevel->GetRMIndexCount(); i++)
//		{
//			DWORD dwIndex = pLevel->GetRMIndex(i);
//			tagRenderMesh *pRenderMesh = &m_pRenderMeshArray[dwIndex];
//
//			if (!pRenderMesh->bEffectByLight)
//			{
//				continue;
//			}
//
//			model::Buffer *pBuffer     = pRenderMesh->pBuffer;
//			D3DXVECTOR3   *pPosBuffer  = pBuffer->GetPosBuffer();
//			D3DXVECTOR3   *pNmlBuffer  = pBuffer->GetNmlBuffer();
//			D3DCOLOR	  *pColBuffer  = pBuffer->GetColBuffer();
//
//			DWORD dwColCount = pBuffer->GetPNCCount();
//
//			for(DWORD n = 0; n < dwColCount;n++)
//			{
//				DWORD &dwDestColor = pColBuffer[n];
//				
//				float DOT = - D3DXVec3Dot(pLightDirection,&pNmlBuffer[n]);// L . N
//
//				if (DOT > 0)//光线在面的正面
//				{	
//					float fValue = 2.0f * DOT;
//
//					DWORD r = (dwDestColor & 0x00FF0000) >> 16;
//					DWORD g = (dwDestColor & 0x0000FF00) >> 8;
//					DWORD b = (dwDestColor & 0x000000FF);
//
//					r = r + (DWORD)(( fR * fValue) * 255.0f);  	
//					if (r > 255) r = 0x000000FF;
//					g = g + (DWORD)(( fG * fValue) * 255.0f);  	
//					if (g > 255) g = 0x000000FF;
//					b = b + (DWORD)(( fB * fValue) * 255.0f);  	
//					if (b > 255) b = 0x000000FF;
//					dwDestColor = 0xFF000000 | (r << 16) | (g << 8) | b;
//				}                                    
//			}
//		}
//	}
//}

void GameModel::Group::Part::ProcessDirectionalLight(const D3DXVECTOR3 *pCameraPosition,const D3DXVECTOR3 *pLightDirection,DWORD dwColor)
{
	if (m_dwSelectLevel >= m_dwLevelCount)
		return ;

	DWORD dwR = ( (dwColor & 0x00FF0000) >> 16) ;
	DWORD dwG = ( (dwColor & 0x0000FF00) >>  8) ;
	DWORD dwB = (  dwColor & 0x000000FF)        ;

	Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
	LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
	for(int i = 0; i <pLevel->GetRMCount(); i++)
	{
		tagRenderMesh *pRenderMesh = vRenderMeshes[i];
		if (!pRenderMesh->bEffectByLight)
		{
			continue;
		}
		//
		model::Buffer *pBuffer     = pRenderMesh->pBuffer;
		D3DXVECTOR3   *pPosBuffer  = pBuffer->GetPosBuffer();
		D3DXVECTOR3   *pNmlBuffer  = pBuffer->GetNmlBuffer();
		D3DCOLOR	  *pColBufferDiffuse  = pBuffer->GetColBufferDiffuse();

		DWORD dwColCount = pBuffer->GetPNCCount();
		//计算Diffuse光
		DWORD n = 0;
		for(; n < dwColCount;n++)
		{
			DWORD &dwDestColor = pColBufferDiffuse[n];

			DWORD r = (dwDestColor & 0x00FF0000) >> 16;
			DWORD g = (dwDestColor & 0x0000FF00) >> 8;
			DWORD b = (dwDestColor & 0x000000FF);

			float DOT = - D3DXVec3Dot(pLightDirection,&pNmlBuffer[n]);// L . N

			if (DOT > 0)//光线在面的正面
			{	
				DWORD dwValue = (DWORD)(2.0f * DOT * 256.0f);

				r = r + dwR * dwValue / 256;  	
				if (r > 255) r = 0x000000FF;
				g = g + dwG * dwValue / 256;  	
				if (g > 255) g = 0x000000FF;
				b = b + dwB * dwValue / 256;  	
				if (b > 255) b = 0x000000FF;
			}                                    

			dwDestColor = 0xFF000000 | (r << 16) | (g << 8) | b;
		}


	}
}


void GameModel::Group::Part::ProcessLevelAnimColor(DWORD dwTimeElapse,DWORD dwTimeCircle)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		if (pLevel->GetAnimColor())
		{
			LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
			for(int i = 0; i <pLevel->GetRMCount(); i++)
			{
				tagRenderMesh *pRenderMesh = vRenderMeshes[i];
				if (pRenderMesh->bAnimColor)	
				{
					model::Mesh   *pMesh   = pRenderMesh->pMesh;
					model::Buffer *pBuffer = pRenderMesh->pBuffer;
					pMesh->ProcessAnimColor(pBuffer,pLevel->GetAnimColor(),dwTimeElapse,dwTimeCircle);	
				}
			}
		}
	}
}

void GameModel::Group::Part::ProcessLevelAnimUV(DWORD dwTimeElapse,DWORD dwTimeCircle)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		if (pLevel->GetAnimUV())
		{
			LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
			for(int i = 0; i <pLevel->GetRMCount(); i++)
			{
				tagRenderMesh *pRenderMesh = vRenderMeshes[i];
				if (pRenderMesh->bAnimUV)	
				{
					model::Mesh   *pMesh   = pRenderMesh->pMesh;
					model::Buffer *pBuffer = pRenderMesh->pBuffer;
					pMesh->ProcessAnimUV(pBuffer,pLevel->GetAnimUV(),dwTimeElapse,dwTimeCircle);	
				}
			}
		}
	}
}

void GameModel::Group::Part::ProcessLevelAnimVisibility(DWORD dwTimeElapse,DWORD dwAnimVisibilityCircle)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		if (pLevel->GetAnimVisibility())
		{
			LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
			for(int i = 0; i <pLevel->GetRMCount(); i++)
			{
				tagRenderMesh *pRenderMesh = vRenderMeshes[i];
				if (pRenderMesh->bAnimVisibility)
				{
					model::Mesh   *pMesh   = pRenderMesh->pMesh;
					model::Buffer *pBuffer = pRenderMesh->pBuffer;
					pMesh->ProcessAnimVisibility(pBuffer,pLevel->GetAnimVisibility(),dwTimeElapse,dwAnimVisibilityCircle);	
				}
			}
		}
	}
}

void GameModel::Group::Part::ProcessLevelAnimTexture(DWORD dwTimeElapse,DWORD dwTimeCircle)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		if (pLevel->GetAnimTexture())
		{
			LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
			for(int i = 0; i <pLevel->GetRMCount(); i++)
			{
				tagRenderMesh *pRenderMesh = vRenderMeshes[i];

				if (pRenderMesh->bAnimTexture)
				{
					model::Mesh   *pMesh   = pRenderMesh->pMesh;
					model::Buffer *pBuffer = pRenderMesh->pBuffer;
					pMesh->ProcessAnimTexture(pBuffer,pLevel->GetAnimTexture(),dwTimeElapse,dwTimeCircle);	
				}
			}
		}
	}
}

GameModel::Group::Part::Level::Level():
m_pAnimColor(NULL),
m_pAnimUV(NULL),
m_pAnimVisibility(NULL),
m_pAnimTexture(NULL)
{
}
GameModel::Group::Part::Level::~Level()
{
}
BOOL GameModel::Group::Part::Level::Create(utility::Text::Node *pLevelNode,const char *szResourcePath)
{
	utility::Text::Variable *pTempVar(NULL);

	utility::Text::Node *pMeshIndexNode = pLevelNode->GetChildNode("mesh_index");
	if (pMeshIndexNode)
	{
		//读取模型索引列表
		for(DWORD m = 0; m < pMeshIndexNode->GetVarCount(); m++)
		{
			pTempVar = pMeshIndexNode->GetVar(m);
			ASSERT(pTempVar && "mesh_index 变量不正确");
			//m_vRMIndexArray.push_back(pTempVar->GetIntValue());
		}
	}
	//读取animColor文件
	char szTemp[_MAX_PATH];
	pTempVar = pLevelNode->GetVar("animcolor");
	if (pTempVar)
	{
		_snprintf(szTemp,_MAX_PATH,"%s\\%s",szResourcePath,pTempVar->GetStringValue());
		m_pAnimColor = new model::AnimColor;
		if (!m_pAnimColor->Create(szTemp))
		{
			OutputConsole("error: Create\"%s\" failed!\n",szTemp);
			ASSERT(0);
			MessageBox(NULL,szTemp,"读取文件失败",MB_OK);			
			return FALSE;
		}
	}
	//读取animUV文件
	pTempVar = pLevelNode->GetVar("animuv");
	if (pTempVar)
	{
		_snprintf(szTemp,_MAX_PATH,"%s\\%s",szResourcePath,pTempVar->GetStringValue());
		m_pAnimUV = new model::AnimUV;
		if (!m_pAnimUV->Create(szTemp))
		{
			OutputConsole("error: Create\"%s\" failed!\n",szTemp);
			MessageBox(NULL,szTemp,"读取文件失败",MB_OK);			
			ASSERT(0);
			return FALSE;
		}
	}
	//读取animVisibility文件
	pTempVar = pLevelNode->GetVar("animvisibility");
	if (pTempVar)
	{
		_snprintf(szTemp,_MAX_PATH,"%s\\%s",szResourcePath,pTempVar->GetStringValue());
		m_pAnimVisibility = new model::AnimVisibility;
		if (!m_pAnimVisibility->Create(szTemp))
		{
			OutputConsole("error: Create\"%s\" failed!\n",szTemp);
			MessageBox(NULL,szTemp,"读取文件失败",MB_OK);			
			ASSERT(0);
			return FALSE;
		}
	}
	//读取animtexture文件
	pTempVar = pLevelNode->GetVar("animtexture");
	if (pTempVar)
	{
		_snprintf(szTemp,_MAX_PATH,"%s\\%s",szResourcePath,pTempVar->GetStringValue());
		m_pAnimTexture = new model::AnimTexture;
		if (!m_pAnimTexture->Create(szTemp))
		{
			OutputConsole("error: Create\"%s\" failed!\n",szTemp);
			MessageBox(NULL,szTemp,"读取文件失败",MB_OK);			
			ASSERT(0);
			return FALSE;
		}
	}
	utility::Text::Node *particlenode = pLevelNode->GetChildNode("particle_index");
	if (particlenode)
	{
		for (DWORD i = 0;i < particlenode->GetVarCount();i++)
		{
			m_vParticleEnable.push_back(particlenode->GetVar(i)->GetIntValue());
		}
	}
	return TRUE;
}

BOOL GameModel::Group::Part::Level::Create(utility::Text::Node *pLevelNode)
{
	utility::Text::Variable *pTempVar(NULL);
	//读取animColor文件
	pTempVar = pLevelNode->GetVar("animcolor");
	m_pAnimColor = NULL;
	if (pTempVar)	
		_snprintf(m_pszColorFileName,_MAX_PATH,"%s",pTempVar->GetStringValue());
	else
		_snprintf(m_pszColorFileName,_MAX_PATH,"%s","");
	//读取animUV文件
	pTempVar = pLevelNode->GetVar("animuv");
	m_pAnimUV = NULL;
	if (pTempVar)
		_snprintf(m_pszUVFileName,_MAX_PATH,"%s",pTempVar->GetStringValue());
	else
		_snprintf(m_pszUVFileName,_MAX_PATH,"%s","");
	//读取animVisibility文件
	pTempVar = pLevelNode->GetVar("animvisibility");
	m_pAnimVisibility = NULL;
	if (pTempVar)
		_snprintf(m_pszVisiFileName,_MAX_PATH,"%s",pTempVar->GetStringValue());
	else
		_snprintf(m_pszVisiFileName,_MAX_PATH,"%s","");
	//读取animtexture文件
	pTempVar = pLevelNode->GetVar("animtexture");
	m_pAnimTexture = NULL;
	if (pTempVar)
		_snprintf(m_pszTexFileName,_MAX_PATH,"%s",pTempVar->GetStringValue());
	else
		_snprintf(m_pszTexFileName,_MAX_PATH,"%s","");
	utility::Text::Node *particlenode = pLevelNode->GetChildNode("particle_index");
	if (particlenode)
	{
		static char str[32];
		for (DWORD i = 0;i < particlenode->GetVarCount();i++)
		{

			sprintf(str, "%04d", particlenode->GetVar(i)->GetIntValue());
			m_vParticleEnable.push_back(MAKEFOURCC(str[0],str[1],str[2],str[3]));
		}
	}
	m_eLoad = Load_Not;
	return TRUE;
}

void GameModel::Group::Part::Level::Destroy()
{
	/*SAFEDESTROY(m_pAnimUV);
	SAFEDESTROY(m_pAnimColor);
	SAFEDESTROY(m_pAnimTexture);
	SAFEDESTROY(m_pAnimVisibility);*/
}

void GameModel::TrailEmitter::SelectColor(DWORD dwIndex)
{
	m_dwCurColorIndex = dwIndex;
}

void GameModel::ProcessAnimColor(GameModel::AnimInfo *pAnimInfo)
{
	DWORD dwCurrentTime = pAnimInfo->GetCurrentTime();
	//刷新模型组上的颜色动画效果
	DWORD dwAnimColorTimeElapse = dwCurrentTime - pAnimInfo->m_dwAnimColorStartTime;
	if (dwAnimColorTimeElapse >= m_dwAnimColorCircle)
	{
		//循环一次
		dwAnimColorTimeElapse = m_dwAnimColorCircle;
		pAnimInfo->m_dwAnimColorStartTime = dwCurrentTime;
	}
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		//根据开始，结束时间设定帧
		Group *pGroup = itGroup->second;
		if (pGroup->IsVisible())
		{
			Group::Part *pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				Group::Part::Level *pLevel = pPart->GetSelectLevel();
				model::AnimColor *pAnimColor = pLevel->GetAnimColor();
				if (pAnimColor)
				{
					pPart->ProcessLevelAnimColor(dwAnimColorTimeElapse,m_dwAnimColorCircle);
				}
			}
		}
	}
}

void GameModel::ProcessAnimUV(GameModel::AnimInfo *pAnimInfo)
{
	DWORD dwCurrentTime = pAnimInfo->GetCurrentTime();
	//刷新模型组上的颜色动画效果
	DWORD dwTimeElapse = dwCurrentTime - pAnimInfo->m_dwAnimUVStartTime;
	if (dwTimeElapse >= m_dwAnimUVCircle)
	{
		//循环一次
		dwTimeElapse = m_dwAnimUVCircle;
		pAnimInfo->m_dwAnimUVStartTime = dwCurrentTime;
	}
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		//根据开始，结束时间设定帧
		Group *pGroup = itGroup->second;
		if (pGroup->IsVisible())
		{
			Group::Part *pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				pPart->ProcessLevelAnimUV(dwTimeElapse,m_dwAnimUVCircle);
			}
		}
	}
}

void GameModel::AnimInfo::ClearTrailContainer(void)
{
	for(DWORD n = 0; n < GAMEMODEL_MAXTRAILEMITTER;n++)
	{
		m_pTrailContainerArray[n].Clear();
	}
}

bool GameModel::CheckStatic(void)
{
	//检查有没有动态效果以决定是否需要时钟
	//动作
	if ( m_mapActionArray.size())
		return false;
	//if ( m_mapParticleEmitterArray.size())
	//	return FALSE;
	//if ( m_mapTrailEmitterArray.size())
	//	return FALSE;

	////根据显示组件表取得组件，渲染
	//for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	//{
	//	GameModel::Group *pGroup = itGroup->second;
	//	for(std::map<DWORD,GameModel::Group::Part *>::iterator itPart = pGroup->GetPartArray()->begin();
	//		itPart != pGroup->GetPartArray()->end(); itPart++)
	//	{
	//		GameModel::Group::Part *pPart = itPart->second;
	//		GameModel::Group::Part::Level * pLevelArray = pPart->GetLevelArray();
	//		for(DWORD n = 0; n < pPart->GetLevelCount();n++)
	//		{
	//			GameModel::Group::Part::Level *pLevel = &pLevelArray[n];
	//			if (pLevel->GetAnimColor() || pLevel->GetAnimTexture() || pLevel->GetAnimUV() ||
	//				pLevel->GetAnimVisibility())
	//			{
	//				return FALSE;
	//			}
	//		}
	//	}
	//}

	return true;
}

void GameModel::Group::Part::ProcessEnvironmentMaps(const D3DXMATRIX *pViewMatrix)
{
	if (m_dwSelectLevel >= 0 && m_dwSelectLevel < m_dwLevelCount)
	{
		Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
		LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
		for(int i = 0; i <pLevel->GetRMCount(); i++)
		{
			tagRenderMesh *pRenderMesh = vRenderMeshes[i];

			if (pRenderMesh->bEnvironmentMaps)
			{
				model::Mesh   *pMesh   = pRenderMesh->pMesh;
				model::Buffer *pBuffer = pRenderMesh->pBuffer;
				pMesh->ProcessEnvironmentMaps(pBuffer,pViewMatrix);	
			}
		}

	}
}
void GameModel::ProcessEnvironmentMaps(const D3DXMATRIX *pViewMatrix)
{
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		GameModel::Group *pGroup = itGroup->second; 
		if (pGroup->IsVisible())
		{
			GameModel::Group::Part * pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				pPart->ProcessEnvironmentMaps(pViewMatrix);
			}
		}
	}
}

void GameModel::MultiplyModelAlpha(float fRatio)
{
	//Visibility效果
	for(std::map<DWORD,Group*>::iterator itGroup = m_mapGroupArray.begin() ; itGroup != m_mapGroupArray.end(); itGroup++)
	{
		//根据开始，结束时间设定帧
		Group *pGroup = itGroup->second;
		if (pGroup->IsVisible())
		{
			Group::Part *pPart = pGroup->GetSelectPart();
			if (pPart)
			{
				pPart->MultiplyAlpha(fRatio);
			}
		}
	}
}

void GameModel::Group::Part::MultiplyAlpha(float fRatio)
{
	if (m_dwSelectLevel >= m_dwLevelCount)
		return;

	DWORD  dwRatio = (DWORD)(fRatio * 255.0f) & 0xFF;

	Level *pLevel = &m_pLevelArray[m_dwSelectLevel];
	LevelMeshes& vRenderMeshes = pLevel->GetRMArray();
	for(int i = 0; i <pLevel->GetRMCount(); i++)
	{
		tagRenderMesh *pRenderMesh = vRenderMeshes[i];

		model::Buffer *pBuffer = pRenderMesh->pBuffer;

		D3DCOLOR	  *pColBuffer  = pBuffer->GetColBufferDiffuse();

		DWORD dwColCount = pBuffer->GetPNCCount();

		for(DWORD n = 0; n < dwColCount;n++)
		{
			DWORD &dwDestColor = pColBuffer[n];

			dwDestColor &= 0X00FFFFFF;
			dwDestColor |= (dwRatio << 24);
		}
	}
}