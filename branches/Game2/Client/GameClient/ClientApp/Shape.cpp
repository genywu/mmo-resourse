#include "stdafx.h"

#include "Shape.h"
#include "ModelTools.h"
#include "other/effectsetup.h"
#include "../../Public/Common/Public.h"
#include "../../EngineExtend\Gamemap.h"
#include "../../EngineExtend\DisplayModel.h"
#include "../../EngineExtend\GameModelManager.h"

#include "Region.h"
#include "ClientRegion.h"
#include "Effect.h"
#include "Monster.h"

#include "PetSystem/Pet.h"
//#include "PetSystem/CFairyPet.h"
//#include "PetSystem/CFighterPet.h"
//#include "PetSystem/CSummonPet.h"
//#include "PetSystem/CTrap.h"
//#include "PetSystem/OrnaPet.h"
#include "CollectionGoods.h"
#include "Player.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/GameControl.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

D3DCOLOR CShape::s_colorModelDefault =  0xFFFFFFFF;	// 原始模型颜色
long CShape::lEffectID = 1;

CShape::CShape(void)
:m_pDisplayModel(NULL)
{
	SetRegionID(0);
	SetPosX(0);
	SetPosY(0);
	SetDir(DIR_UP);
	SetSpeed(2000.f);
	SetState(STATE_PEACE);

	m_bIsSelected = false;
	m_wAction = 0xFFFF;

	SetHeight(0.f);

	m_lAppearTime = 0;
	m_lAlphaFadeInStartTime = timeGetTime();
	m_lAlphaFadeInTimeLength = 0;//5000;
	m_bIsDeleted = false;
	m_bIsNeedAlphaFade = false;
	m_bIsAddAlphaFaded = false;
	m_fAlphaValue = 0.0f;
	m_eAlphaFadeMode = CAlphaFadeObjectList::ALPHA_FADE_NONE;

	m_fDir = 0.0f;
	m_fNowDir = 0.f;

	m_colorModel = s_colorModelDefault;
	m_dwCurrentModelCol = 0;

	//初始化动作列表
	m_pAnimInfo = new AnimInfo();
	ZeroMemory(m_pAnimInfo->GetActionInfo(),sizeof(AnimInfo::tagActionInfo));
	m_pAnimInfo->SetAnimTime(GetCurTickCount());
	m_pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('S','T','N','D');
	m_pAnimInfo->GetActionInfo()->dwCurActionStartTime = GetCurTickCount();
	m_pAnimInfo->SetTextureProjectiveTime(timeGetTime());
	m_cAttackAction = 0;
	
	m_bDisplayFinish = false;
	m_bInViewRange = true;

	m_iActionCount = -1;

}



/*
 * 功能: 析构函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.10.13 - lpf
 *		释放显示模型是根据当前对象是否需要渐变操作而定;
 */
CShape::~CShape(void)
{
	DelAllEffect();
	SAFE_DELETE(m_pAnimInfo);
	CGameModelManager::GetInstance()->ReleaseDisplayModel(m_pDisplayModel);
	m_pDisplayModel = NULL;
}



/*
 * 功能: 初始化显示模型
 * 摘要: -
 * 参数: bAsyn - 是否同步创建显示模型(默认为异步创建)
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.03.20 - lpf
 *		为该函数增加了同步创建的参数;
 */
bool CShape::SetDisplayModel(bool bAsyn)
{
	long lType,lGraphicsID;
	GetCurGraphicTypeID(lType,lGraphicsID);

	char pszFileName[MAX_PATH];
	GetModelFormatString(lType,lGraphicsID,pszFileName);
	if(!GetGame()->IsFileExist(pszFileName))
	{
		Log4c::Warn(ROOT_MODULE,"model文件%s不存在。",pszFileName);
		OutputError("Model ID : %d %d 不存在\n",lType,lGraphicsID);
		return false;
	}
	
	CGameModelManager* pModelManager = CGameModelManager::GetInstance();
	if(m_pDisplayModel)
	{
		char szResourcePath[_MAX_PATH];
		CRFile::GetFilePath(pszFileName,szResourcePath);
		//如果相同则退出
		if(m_pDisplayModel->GetGameModelID() ==
			pModelManager->GetGameModelID(pszFileName))
			return true;
	}
	//释放
	pModelManager->ReleaseDisplayModel(m_pDisplayModel);
	//创建
	m_pDisplayModel = pModelManager->CreateDisplayModel(pszFileName, bAsyn);
	//SetAction(ACT_STAND);
	return true;
}

//初始化显示模型
void CShape::SetDisplayModelGroup(bool bAsyn)
{
	if(m_pDisplayModel)
	{
		m_pDisplayModel->ShowAllGroup(bAsyn);
		m_pDisplayModel->EnableParticleEmitter(TRUE);
		m_pDisplayModel->EnableTrailEmitter(FALSE);
	}
}



/*
* 功能: 更新动画
* 摘要: 在客户端程序最小化是调用该函数,仅仅处理模型的动画信息
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.03.07
* 修改日志:
*/
void CShape::UpdateAnimation()
{
	if (m_pDisplayModel == NULL ||
		m_pDisplayModel->GetLoadState() != Load_Did)
		return;

	m_pAnimInfo->SetCurrentTime(timeGetTime());
	m_pDisplayModel->ProcessAnimLoop(m_pAnimInfo);
}



CShape::eDIR CShape::GetDir()
{
	return RadianDirTo8Dir(m_fDir);
}

void CShape::SetDir(CShape::eDIR dir)
{
	if( dir>= DIR_UP && dir<= DIR_LEFTUP)
	{
		m_fDir = D3DX_PI/4*dir-D3DX_PI/2;
	}
}

void CShape::SetPosXY(float x, float y)
{ 
    long lOldTileX = static_cast<long>(m_fPosX);
    long lOldTileY = static_cast<long>(m_fPosY);
    m_fPosX = x; m_fPosY = y;
    long lNewTileX = static_cast<long>(m_fPosX);
    long lNewTileY = static_cast<long>(m_fPosY);

    if(lOldTileX != lNewTileX || lOldTileY != lNewTileY)
    {
        if( GetType() == TYPE_PLAYER ||
            GetType() == TYPE_MONSTER ||
            GetType() == TYPE_PET )
        {
            CRegion *pRegion = dynamic_cast<CRegion*>(GetFather());
            if(pRegion)
            {
                pRegion->DecCellObjNum(lOldTileX,lOldTileY);
                pRegion->IncCellObjNum(lNewTileX,lNewTileY);
            }
        }
    }

    // 计算高度
    CClientRegion *pRegion = GetGame()->GetRegion();
    if (pRegion)
    {
        //render::Camera *pCamera = pRegion->GetCamera();
        GameMap *pGameMap = pRegion->GetGameMap();
        SetHeight(pGameMap->CalculateHeight(GetPosY(),GetPosX()));
    }
}



/*
 * 功能: 得到模型拾取盒局部坐标系绝对高度
 * 摘要: -
 * 参数: -
 * 返回值: 拾取盒局部坐标系绝对高度
 * 作者: lpf
 * 创建日期: 2007.12.17
 */
float CShape::GetPickBoxHeight()
{
	D3DXVECTOR3 vMax;
	D3DXVECTOR3 vMin;

	if (!m_pDisplayModel)
		return 2.0f;

	render::BoundingBox * pBoundingBox = m_pDisplayModel->GetPickBox();
	if (pBoundingBox != NULL)
	{
		pBoundingBox->Get(vMin, vMax);
		return (vMax.y - vMin.y);
	}else
		return 2.0f;
}



/*
 * 功能: 得到模型尺寸盒局部坐标系绝对高度
 * 摘要: -
 * 参数: -
 * 返回值: 尺寸盒局部坐标系绝对高度
 * 作者: lpf
 * 创建日期: 2007.12.17
 */
float CShape::GetSizeBoxHeight()
{
	D3DXVECTOR3 vMax;
	D3DXVECTOR3 vMin;

	if (!m_pDisplayModel)
		return 2.0f;

	render::BoundingBox * pBoundingBox = m_pDisplayModel->GetSizeBox();
	if (pBoundingBox != NULL)
	{
		pBoundingBox->Get(vMin, vMax);
		return (vMax.y - vMin.y);
	}else
		return 2.0f;
}



/*
 * 功能: 得到模型拾取盒中心点的屏幕坐标
 * 摘要: 中心点在世界坐标系里
 * 参数: -
 * 返回值: 中心点的屏幕坐标
 * 作者: lpf
 * 创建日期: 2009.02.05
 */
POINT & CShape::GetPickBoxCenterOfScreenCoord(POINT & ptScreen)
{
	D3DXVECTOR3 vPos(GetPosY(), (GetPickBoxHeight() * 0.5f) + GetHeight(), GetPosX());
	D3DXMATRIX  mFinal;

	GetGame()->GetRegion()->CalculateFinalTransformMatrix(mFinal);
	D3DXVec3TransformCoord(&vPos, &vPos, &mFinal);

	ptScreen.x = (long)vPos.x;
	ptScreen.y = (long)vPos.y;

	return ptScreen;
}



/*
 * 功能: 得到模型尺寸盒中心点的屏幕坐标
 * 摘要: 中心点在世界坐标系里
 * 参数: -
 * 返回值: 中心点的屏幕坐标
 * 作者: lpf
 * 创建日期: 2009.02.05
 */
POINT & CShape::GetSizeBoxCenterOfScreenCoord(POINT & ptScreen)
{
	D3DXVECTOR3 vPos(GetPosY(), (GetSizeBoxHeight() * 0.5f) + GetHeight(), GetPosX());
	D3DXMATRIX  mFinal;

	GetGame()->GetRegion()->CalculateFinalTransformMatrix(mFinal);
	D3DXVec3TransformCoord(&vPos, &vPos, &mFinal);

	ptScreen.x = (long)vPos.x;
	ptScreen.y = (long)vPos.y;

	return ptScreen;
}



long CShape::GetTileX()
{
	return (long)GetPosX();
}

long CShape::GetTileY()
{
	return (long)GetPosY();
}

// 设置阻挡
void CShape::SetBlock(int x, int y, unsigned short block)
{
	assert(GetFather());

	if (GetType()==TYPE_GOODS || GetType()==TYPE_EFFECT || GetType()==TYPE_SKILL || GetType()==TYPE_SUMMON)
		return;

	switch(block) 
	{
	case CRegion::BLOCK_SHAPE:
		for (int i=x-GetFigure(DIR_RIGHT); i<=x+GetFigure(DIR_RIGHT); i++)
		{
			for (int j=y-GetFigure(DIR_UP); j<=y+GetFigure(DIR_UP); j++)
			{
				CRegion::tagCell* pCell = ((CRegion*)GetFather())->GetCell(i,j);
				if( pCell && (pCell->bBlock == CRegion::BLOCK_NO || pCell->bBlock == CRegion::BLOCK_AIM))
				{
					pCell->bBlock = CRegion::BLOCK_SHAPE;
				}
			}
		}
		break;

	case CRegion::BLOCK_NO:
		for (int i=x-GetFigure(DIR_RIGHT); i<=x+GetFigure(DIR_RIGHT); i++)
		{
			for (int j=y-GetFigure(DIR_UP); j<=y+GetFigure(DIR_UP); j++)
			{
				CRegion::tagCell* pCell = ((CRegion*)GetFather())->GetCell(i,j);
				if( pCell && pCell->bBlock == CRegion::BLOCK_SHAPE )
				{
					pCell->bBlock = CRegion::BLOCK_NO;
				}
			}
		}
		break;
	}
}


// 添加到CByteArray
bool CShape::AddShapeToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray, GetRegionID());
	_AddToByteArray(pByteArray,GetRegionGUID());
	_AddToByteArray(pByteArray, GetPosX());
	_AddToByteArray(pByteArray, GetPosY());
	_AddToByteArray(pByteArray, (long)GetDir());
	_AddToByteArray(pByteArray, (long)0);
	_AddToByteArray(pByteArray, GetSpeed());
	_AddToByteArray(pByteArray, GetState());
	_AddToByteArray(pByteArray, GetAction());
	return true;
}

// 解码
bool CShape::DecordShapeFromByteArray(BYTE* pByte, long& pos)
{
	m_lCurRgnType = _GetLongFromByteArray(pByte,pos);
	m_lRegionID = _GetLongFromByteArray(pByte, pos);
	_GetBufferFromByteArray(pByte, pos,m_RegionID);
	m_fPosX = _GetFloatFromByteArray(pByte, pos);
	m_fPosY = _GetFloatFromByteArray(pByte, pos);
	SetDir((eDIR)_GetLongFromByteArray(pByte, pos));
	int lPos = _GetLongFromByteArray(pByte, pos);
	m_fSpeed = _GetFloatFromByteArray(pByte, pos);
	m_wState = _GetWordFromByteArray(pByte, pos);
	m_wAction = _GetWordFromByteArray(pByte, pos);

	SetNowDir(GetDirEx());

	SetPosX( GetPosX() );
	SetPosY( GetPosY() );
	SetSpeed(m_fSpeed);
	return true; 
}

// 添加到CByteArray
bool CShape::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	CBaseObject::AddToByteArray(pByteArray, bExData);
	AddShapeToByteArray(pByteArray);
	return true;
}

// 解码
bool CShape::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	CBaseObject::DecordFromByteArray(pByte, pos, bExData);
	DecordShapeFromByteArray(pByte, pos);
	return true; 
}

// 解码
bool CShape::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData )
{
	CBaseObject::DecodeFromDataBlock(setReadDB,bExData);
	m_lCurRgnType = (eRgnType)setReadDB.GetByteFromByteArray();
	m_lRegionID = setReadDB.GetLongFromByteArray();
	setReadDB.GetBufferFromByteArray(m_RegionID);
	m_fPosX = setReadDB.GetFloatFromByteArray();
	m_fPosY = setReadDB.GetFloatFromByteArray();
	SetDir((eDIR)setReadDB.GetLongFromByteArray());
	int lPos = setReadDB.GetLongFromByteArray();
	m_fSpeed = setReadDB.GetFloatFromByteArray();
	m_wState = setReadDB.GetWordFromByteArray();
	m_wAction = setReadDB.GetWordFromByteArray();

	SetNowDir(GetDirEx());

	SetPosX( GetPosX() );
	SetPosY( GetPosY() );
	SetSpeed(m_fSpeed);
	return true;
}



/*
 * 功能: Shape对象的AI
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.04.10 - lpf
 *		在"判断播放动作结束"处,增加了对动作循环次数的判断(主要用于支持仪态动作的结束判断)
 *	2008.12.26 - lpf
 *		在"判断播放动作结束"处,当前仪态动作是否播放完毕的判断,如果播放完毕,也要设定为站立动作
 */
void CShape::AI()
{
	// 特效AI
	for(list<CEffect*>::iterator it=m_listEffect.begin(); it!=m_listEffect.end();)
	{
		CEffect* pEffect = (*it);

		if (pEffect->GetIsDeleted())
		{
            m_listEffect.erase(it++);
			SAFE_DELETE(pEffect);
		}
		else
		{
			char str[1024] = "";
		#ifdef _RUNSTACKINFO_
			sprintf_s(str,"%d:EffectAI() Start.",pEffect->GetGraphicsID());
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
		#endif
			pEffect->AI();
			++it;
			//判断结束
			AnimInfo::tagActionInfo *pActionInfo = pEffect->GetAnimInfo()->GetActionInfo();
			pEffect->m_bDisplayFinish = (pActionInfo->dwCurActionLoopCount > 0);
		#ifdef _RUNSTACKINFO_
			sprintf_s(str,"%d:EffectAI() End.",pEffect->GetGraphicsID());
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
		#endif
		}
	}

	// 平滑转向
	if (IsNeedDisplay())
	{
		float fValue = 0.f;
		if(GetType() == TYPE_MONSTER)
		{
			CMonster *pMonster = (CMonster *)this;
			fValue = pMonster->GetTurnSpeed();
			if(fValue == 0.f)
				fValue = 4.f;
		}
		else
			fValue = 2.f;
		if(fValue > 0.f)
		{
		float fAngle = 0.314159f * fValue;
		float fDestDir = GetDirEx();
		float fNowDir = GetNowDir();

		if (fDestDir<0)
			fDestDir += D3DX_PI*2;

		if (fNowDir<0)
			fNowDir += D3DX_PI*2;

		float fDistanc = fDestDir - fNowDir;

		if (abs(fDistanc)<fAngle)
		{
			SetNowDir(fDestDir);
		}
		else
		{
			if ( (fDistanc > 0 && fDistanc < D3DX_PI) || fDistanc < -D3DX_PI)	// 顺时针
			{
				fNowDir += fAngle;
				if (fNowDir>=D3DX_PI*2)
					fNowDir = 0;
			}
			else	// 逆时针
			{
				fNowDir -= fAngle;
				if (fNowDir<0.f)
					fNowDir += D3DX_PI*2;
			}

			SetNowDir(fNowDir);
		}
	}

	}

	// 判断播放动作结束	
	if (m_iActionCount == -1)
		m_bDisplayFinish = (m_pAnimInfo->GetActionInfo()->dwCurActionLoopCount > 0 && !m_pAnimInfo->GetActionInfo()->bCurActionLooped);
	else
		m_bDisplayFinish = (m_pAnimInfo->GetActionInfo()->dwCurActionLoopCount > 0 && m_pAnimInfo->GetActionInfo()->dwCurActionLoopCount >= m_iActionCount);
	if (m_bDisplayFinish)
	{
		if (GetAction()==ACT_ATTACK || GetAction()==ACT_BEATTACK || GetAction()==ACT_IDLE ||
			GetAction()==ACT_BLOCK || GetAction()==ACT_PARRY || GetAction()==ACT_MISS ||
			GetAction()>=ACT_PERFORM||GetAction()==ACT_PICK)
		{
			// 判断是否为仪态动作
			if (GetAction() >= ACT_FACEACTION && GetAction() <= ACT_FACEACTIONMAX)
			{
				// 判断仪态动作是否播放完毕
				CMoveShape * pMoveShape = dynamic_cast<CMoveShape *>(this);
				if (pMoveShape && !pMoveShape->GetPlayStateOfActionAnim())
				{
					SetAction(ACT_STAND);
					m_iActionCount = -1;
				}
			}else
			{
				SetAction(ACT_STAND);
				m_iActionCount = -1;
			}
		}
	}
}

bool CShape::Display(void)
{
	// 显示
	/*if (IsNeedDisplay())
	{
		CClientRegion *pRegion = GetGame()->GetRegion();
		render::Camera *pCamera = pRegion->GetCamera();
		m_pAnimInfo->SetupAnimInfo(GetPosX(),GetPosY(),GetHeight(),GetNowDir(),pCamera);
		if(m_pDisplayModel)
		{
			m_pDisplayModel->Display(m_pAnimInfo);
		}
	}*/
	char str[1024] = "";
#ifdef _RUNSTACKINFO_
	sprintf_s(str,"CShape::Display Start.%s,ID:%d",GetName(),GetGraphicsID());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
	if( IsNeedDisplay() )
	{
		DisplayEffect();
	}
#ifdef _RUNSTACKINFO_
	sprintf_s(str,"CShape::Display End.%s,ID:%d",GetName(),GetGraphicsID());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
	return true;
}

void CShape::Display(render::Camera * pCamera)
{
}

// 显示特效
void CShape::DisplayEffect()
{
	if (IsInViewRange())
	{
		for(list<CEffect*>::iterator it=m_listEffect.begin(); it!=m_listEffect.end(); it++)
		{
			if ((*it)->GetIsDeleted()==false)
			{
				//(*it)->SetPosXY(GetPosX(), GetPosY());
				//(*it)->SetHeight(GetHeight());
				(*it)->Display();
			}
		}
	}
}
// 该shape是否需要显示
bool CShape::IsNeedDisplay()
{
	if (GetType() == TYPE_SKILL || GetType() == TYPE_SUMMON)
		return false;
	return true;
}

// 获取定位点坐标
BOOL CShape::GetLocatorPos(eLOCATOR eLocator, float &x, float &y, float &fHeight,DWORD dwFrame)
{
	CClientRegion *pRegion = GetGame()->GetRegion();
	render::Camera *pCamera = pRegion->GetCamera();
	if (eLocator == LOCATOR_FOOTCENTER)
	{
		x = m_fPosX;
		y = m_fPosY;
		fHeight = m_fHeight;
		return TRUE;
	}

	if(m_pDisplayModel)
	{
		if (GetType() == TYPE_PLAYER)
		{
			CPlayer* player = (CPlayer*)this;
			if (player->IsRide())
			{
				if (player->GetHorse()->GetDisplayModel()&&player->GetHorse()->GetDisplayModel()->GetLoadState() == Load_Did)
				{
					m_pDisplayModel->GetAnimInfobyLocator(m_pAnimInfo,player->GetHorse()->GetDisplayModel(),player->GetHorse()->GetAnimInfo(),CShape::LOCATOR_BACK);
				}
			}
			else
			{
				m_pAnimInfo->SetupAnimInfo(GetPosX(),GetPosY(),GetHeight(),GetNowDir(),pCamera);

			}
		}
		else
		{
			m_pAnimInfo->SetupAnimInfo(GetPosX(),GetPosY(),GetHeight(),GetNowDir(),pCamera);		
		}
		
		return m_pDisplayModel->GetLocatorPos(m_pAnimInfo,eLocator,x,y,fHeight,dwFrame);
	}
	return FALSE;
}



/*
 * 功能: 画分段字符窜
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2009.12.03 - lpf
 *		注释了字体拖屏显示功能
 */
void CShape::DrawSubsectionText(const char* str,D3DXVECTOR3 pos2,D3DCOLOR color,D3DCOLOR bkcolor,D3DCOLOR bkBarColor)
{
	static short count = 1;
	static short sLevel = 0;
	int sublen = lstrlen(str);
	long sublPos = (long)strcspn(str, "/");
	//if (sublPos==sublen) return;
	
	/////////////////////
	long x = 0,y = 0;
	char strFirst[256], strSecond[256];
	strcpy(strFirst, str);
	strFirst[sublPos] = 0;
	strcpy(strSecond, &str[sublPos+1]);

	// second
	sublPos = (long)strcspn(strSecond, "/");
	sublen = (long)lstrlen(strSecond);
	if (sublen==sublPos)
	{
		sLevel = 1;
		// first
		sublen = lstrlen(strFirst);
		x = (long)pos2.x - GetGame()->GetFontWidth(strFirst) / 2;
		y = (long)pos2.y;

		//if (x<0)
		//	x = 0;
		//if (y<0)
		//	y = 0;
		//if (x > SCREEN_WIDTH-sublen * GetGame()->GetFontWidth(-1))
		//	x = SCREEN_WIDTH-sublen * GetGame()->GetFontWidth(-1);
		//if (y > SCREEN_HEIGHT-63-GetGame()->GetFontHeight(-1))
		//	y = SCREEN_HEIGHT-63-GetGame()->GetFontHeight(-1);

			y -= sLevel*(GetGame()->GetFontHeight(-1) + 2);


        // 宠物代码 注释  Bugs 2008-9-12
		//if(GetType()==TYPE_MONSTER)
		//{
		//	CMonster *pMonster=(CMonster*)this;
		//	if(pMonster->GetPetInfo()->MasterID!=CGUID::GUID_INVALID &&
		//		pMonster->GetPetInfo()->lMasterType==TYPE_PLAYER)
		//	{
		//		y-=17;
		//	}
		//}

		if (bkBarColor)
			GetGame()->DrawSolidQuadRect(x-2, y-2, x+ GetGame()->GetFontWidth(strFirst) +2, y+GetGame()->GetFontHeight(-1)+2, bkBarColor);

		GetGame()->OutPutText(x, y-1, strFirst, sublen, bkcolor);
		GetGame()->OutPutText(x-1, y, strFirst, sublen, bkcolor);
		GetGame()->OutPutText(x, y+1, strFirst, sublen, bkcolor);
		GetGame()->OutPutText(x+1, y, strFirst, sublen, bkcolor);

		GetGame()->OutPutText(x, y, strFirst, sublen, color);
		
		sublen = lstrlen(strSecond);
		x = (long)pos2.x - sublen * GetGame()->GetFontWidth(strFirst) / 2;
		y = (long)pos2.y;

		//if (x<0)
		//	x = 0;
		//if (y<0)
		//	y = 0;
		//if (x > SCREEN_WIDTH-sublen * GetGame()->GetFontWidth(-1))
		//	x = SCREEN_WIDTH-sublen * GetGame()->GetFontWidth(-1);
		//if (y > SCREEN_HEIGHT-63-GetGame()->GetFontHeight(-1))
		//	y = SCREEN_HEIGHT-63-GetGame()->GetFontHeight(-1);

		if (bkBarColor)
			GetGame()->DrawSolidQuadRect(x-2, y-2, x+ GetGame()->GetFontWidth(strSecond) +2, y+GetGame()->GetFontHeight(-1)+2, bkBarColor);


		//if(GetType()==TYPE_MONSTER)
		//{
		//	CMonster *pMonster=(CMonster*)this;
		//	if(pMonster->GetPetInfo()->MasterID!=CGUID::GUID_INVALID &&
		//		pMonster->GetPetInfo()->lMasterType==TYPE_PLAYER)
		//	{
		//		y-=17;
		//	}
		//}

		GetGame()->OutPutText(x, y-1, strSecond, sublen, bkcolor);
		GetGame()->OutPutText(x-1, y, strSecond, sublen, bkcolor);
		GetGame()->OutPutText(x, y+1, strSecond, sublen, bkcolor);
		GetGame()->OutPutText(x+1, y, strSecond, sublen, bkcolor);

		GetGame()->OutPutText(x, y, strSecond, sublen, color);
		
	}else
	{
		count++;
		DrawSubsectionText(strSecond,pos2,color,bkcolor,bkBarColor);		
		sLevel++;
		// first
		sublen = lstrlen(strFirst);
		x = (long)pos2.x - GetGame()->GetFontWidth(strFirst)/ 2;
		y = (long)pos2.y;

		//if (x<0)
		//	x = 0;
		//if (y<0)
		//	y = 0;
		//if (x > SCREEN_WIDTH-sublen * GetGame()->GetFontWidth(-1))
		//	x = SCREEN_WIDTH-sublen * GetGame()->GetFontWidth(-1);
		//if (y > SCREEN_HEIGHT-63-GetGame()->GetFontHeight(-1))
		//	y = SCREEN_HEIGHT-63-GetGame()->GetFontHeight(-1);

		y -= sLevel*(GetGame()->GetFontHeight(-1) + 2);

		//if(GetType()==TYPE_MONSTER)
		//{
		//	CMonster *pMonster=(CMonster*)this;
		//	if(pMonster->GetPetInfo()->MasterID!=CGUID::GUID_INVALID &&
		//		pMonster->GetPetInfo()->lMasterType==TYPE_PLAYER)
		//	{
		//		y-=17;
		//	}
		//}

		if (bkBarColor)
			GetGame()->DrawSolidQuadRect(x-2, y-2, x+GetGame()->GetFontWidth(strFirst)+2, y+GetGame()->GetFontHeight(-1)+2, bkBarColor);

		GetGame()->OutPutText(x, y-1, strFirst, sublen, bkcolor);
		GetGame()->OutPutText(x-1, y, strFirst, sublen, bkcolor);
		GetGame()->OutPutText(x, y+1, strFirst, sublen, bkcolor);
		GetGame()->OutPutText(x+1, y, strFirst, sublen, bkcolor);

		GetGame()->OutPutText(x, y, strFirst, sublen, color);
		count--;
		if (count==1)
		{
			sLevel = 0;
		}
	}
}



/*
 * 功能: 对象信息显示
 * 摘要: -
 * 参数: str			 - 需要显示的文字信息
 *		 color			 - 字体颜色
 *		 bkcolor		 - 字体背景颜色
 *		 fPos			 - 绘制起始高度
 *		 bkBarColor 	 - 绘制背景条颜色
 *		 bIsDisplayHPBar - 是否已经显示了血条
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.07.07 - lpf
 * 		增加了"名字行箭头显示"
 *	2008.07.17 - lpf
 * 		调整了选中对象时的箭头显示位置
 *	2008.08.04 - lpf
 * 		对象时头顶文字信息显示高度,原来的显示,当视距拉远时,头顶信息会遮挡头顶(主要是NPC)
*	2008.12.25 - lpf
*		调整了"名字行箭头显示",向左移动2两个像素;
*	2009.11.13 - lpf
*		增加了一个参数,用于记录外部是否已经显示了血条,目前主要应用于玩家显示;
 */
void CShape::DisplayText(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, D3DCOLOR bkBarColor, bool bIsDisplayHPBar)
{
	D3DXVECTOR3 pos(GetPosY(), GetPickBoxHeight() + 0.2f + GetHeight(), GetPosX());
	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	int len = lstrlen(str);
	long lFontLen = GetGame()->GetFontWidth(str);
	long x=(long)pos2.x - lFontLen / 2, y=(long)fPos;


    long lPos = (long)strcspn(str, "/");
    if (lPos==len)
    {
        //x = (long)pos2.x - lFontLen / 2;
        //y = (long)fPos;

        //if(GetType()==TYPE_MONSTER)
        //{
        //	CMonster *pMonster=(CMonster*)this;
        //	if(pMonster->GetPetInfo()->MasterID!=CGUID::GUID_INVALID &&
        //		pMonster->GetPetInfo()->lMasterType==TYPE_PLAYER)
        //	{
        //		y-=17;
        //	}
        //}

        if (bkBarColor)
            GetGame()->DrawSolidQuadRect(x-2, y-2, x+GetGame()->GetFontWidth(str)+2, y+GetGame()->GetFontHeight(-1)+2, bkBarColor);

        GetGame()->OutPutText(x + 1, y + 1, str, len, bkcolor);
        GetGame()->OutPutText(x, y, str, len, color);
    }
    else
    {
        DrawSubsectionText(str,pos2,color,bkcolor,bkBarColor);
    }

	// 名字行箭头显示
	if (m_bIsSelected)
	{
		CBitmapX * pSelectShape = GetGame()->m_pSelectShape;
		DWORD dwW = pSelectShape->GetWidth();
		DWORD dwH = pSelectShape->GetHeight();
		RECT rc = { 0, 0, dwW / 2, dwH };
		GetGame()->m_pSelectShape->Draw(x - dwW / 2 - 10, y, rc);
		rc.left = dwW / 2;
		rc.right = dwW;
		GetGame()->m_pSelectShape->Draw(x + lFontLen + 8, y, rc);
	}
}



/*
 * 功能: 对象信息显示
 * 摘要: -
 * 参数: str		- 需要显示的文字信息
 *		 color		- 字体颜色
 *		 bkcolor	- 字体背景颜色
 *		 fPos		- 绘制起始高度
 *		 bkBarColor - 绘制背景条颜色
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.07.17 - lpf
 * 		增加了选中对象时的箭头显示,并取消了绘制坐标超出屏幕时的判断;
 *	2008.09.30 - lpf
 * 		修改了当前绘制高度由传进来的参数决定;
 */
void CShape::DisplayMasterName(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, D3DCOLOR bkBarColor)
{
	D3DXVECTOR3 pos(GetPosY(),GetPickBoxHeight() + 0.2f + GetHeight(),GetPosX());
	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	int len = lstrlen(str);

	long x = (long)pos2.x - GetGame()->GetFontWidth(str) / 2;
	long y = (long)fPos;

	//if (x<0)
	//	x = 0;
	//if (y<0)
	//	y = 0;
	//if (x > SCREEN_WIDTH-len * GetGame()->GetFontWidth(-1))
	//	x = SCREEN_WIDTH-len * GetGame()->GetFontWidth(-1);
	//if (y > SCREEN_HEIGHT-63-GetGame()->GetFontHeight(-1))
	//	y = SCREEN_HEIGHT-63-GetGame()->GetFontHeight(-1);

	if (bkBarColor)
		GetGame()->DrawSolidQuadRect(x-2, y-2, x+GetGame()->GetFontWidth(str)+2, y+GetGame()->GetFontHeight(-1)+2, bkBarColor);

	//GetGame()->OutPutText(x, y-1, str, len, bkcolor);
	//GetGame()->OutPutText(x-1, y, str, len, bkcolor);
	//GetGame()->OutPutText(x, y+1, str, len, bkcolor);
	GetGame()->OutPutText(x+1, y+1, str, len, bkcolor);

	GetGame()->OutPutText(x, y, str, len, color);

	// 名字行箭头显示
	if (m_bIsSelected)
	{
		CBitmapX * pSelectShape = GetGame()->m_pSelectShape;
		DWORD dwW = pSelectShape->GetWidth();
		DWORD dwH = pSelectShape->GetHeight();
		RECT rc = { 0, 0, dwW / 2, dwH };
		GetGame()->m_pSelectShape->Draw(x - dwW / 2 - 8, y, rc);
		rc.left = dwW / 2;
		rc.right = dwW;
		GetGame()->m_pSelectShape->Draw(x + GetGame()->GetFontWidth(str) + 8, y, rc);
	}
}

void CShape::DisplayChat(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, D3DCOLOR bkBarColor)
{
	D3DXVECTOR3 pos(GetPosY(),2.5f + GetHeight(),GetPosX());
	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	const long LINEWIDTH = 27 * 14;
	const long LINEHEIGHT = GetGame()->GetFontHeight();
	const long ROW = 3;
	string strText = str;
	int len = (int)strText.length();
	if(len > LINEWIDTH * ROW)
	{
		long i = LINEWIDTH * ROW-2;
		// 汉字
		if((unsigned char)strText[i]>127 )
		{
			strText = strText.substr(0,i-1);
			--len;
		}
		else
		{
			int index = 0;
			index = (int)strText.find("\\");
			// 处于表情代码之中
			// 有表情代码
			while(string::npos != index && index < i)
			{
				if(index + 3 > i)
				{
					i = index;
					break;
				}
				index = (int)strText.find("\\",index+1);
			}
			strText = strText.substr(0,i);
		}
		strText += "...";
	}
	int lines = GetGame()->GetStringLine((char*)strText.c_str(), LINEWIDTH);
	if (len>LINEWIDTH)
		len=LINEWIDTH;

	int nZoneWidth = GetGame()->GetFontWidth(strText.c_str());
	int nZoneHeight = lines * LINEHEIGHT;

	long x = (long)pos2.x - nZoneWidth / 2;
	long y = (long)pos2.y - nZoneHeight / 2;
	y += (long)fPos;

	if(lines > 1)
    {
		y -= LINEHEIGHT *(lines-1);
    }
	GetGame()->DrawStringEx(x, y, (char*)strText.c_str(), color, bkcolor, bkBarColor, LINEWIDTH, LINEHEIGHT);
}

// 在shape身上显示减血伤害数字
void CShape::DisplayDamage(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX, float fY, float fScale)
{
	D3DXVECTOR3 pos;
	if (fX==-1.f && fY ==-1.f)
		pos = D3DXVECTOR3(GetPosY(),fPos+GetHeight(),GetPosX());
	else
		pos = D3DXVECTOR3(fY, fPos+GetHeight(), fX);

	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	int len = lstrlen(str);

	int iW = (int)((float)GetGame()->GetNumberWidth() * fScale);
	long x = (long)pos2.x - len * iW / 2;
	int iH = (int)((float)GetGame()->GetNumberHeight() * fScale);
	long y = (long)pos2.y - iH / 2;

	GetGame()->NumberOut(x, y-2, str, len, bkcolor, fScale, false);
	GetGame()->NumberOut(x-2, y, str, len, bkcolor, fScale, false);
	GetGame()->NumberOut(x, y+2, str, len, bkcolor, fScale, false);
	GetGame()->NumberOut(x+2, y, str, len, bkcolor, fScale, false);

	GetGame()->NumberOut(x, y, str, len, color, fScale, false);
}



/*
 * 功能: 在shape身上显示吸收图片和受伤数字
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.12.03 - lpf
 * 		显示字符图片,仅仅采用单一图片显示模式,不再由程序进行描边显示;
 */
void CShape::DisplaySuck(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX, float fY, float fScale, bool bBlastAttck)
{
	D3DXVECTOR3 pos;
	if (fX==-1.f && fY ==-1.f)
		pos = D3DXVECTOR3(GetPosY(),fPos+GetHeight(),GetPosX());
	else
		pos = D3DXVECTOR3(fY, fPos+GetHeight(), fX);

	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	int len = lstrlen(str);

	int iW = (int)((float)GetGame()->GetNumberWidth() * fScale);
	long x = (long)pos2.x - len * iW / 2;
	int iH = (int)((float)GetGame()->GetNumberHeight() * fScale);
	long y = (long)pos2.y - iH / 2;

	if (len > 0)		// 如果有伤害数字就显示伤害数字
	{
		GetGame()->NumberOut(x, y-2, str, len, bkcolor, fScale, false);
		GetGame()->NumberOut(x-2, y, str, len, bkcolor, fScale, false);
		GetGame()->NumberOut(x, y+2, str, len, bkcolor, fScale, false);
		GetGame()->NumberOut(x+2, y, str, len, bkcolor, fScale, false);

		GetGame()->NumberOut(x, y, str, len, color, fScale, false);
	}
	else			// 显示吸收图片
	{
		RECT rect = {0, 0, GetGame()->GetSuckBitmap()->GetWidth(), GetGame()->GetSuckBitmap()->GetHeight()};
		float fW = rect.right * fScale;
		float fH = rect.bottom * fScale;
		x = x + GetGame()->GetNumberWidth();
		DWORD dwColor = (color & 0xff000000) | 0x00ffffff;

		if(bBlastAttck)
		{
			//GetGame()->GetBlastSuckBitmap()->Draw(x, y-2, fW, fH, rect, bkcolor);
			//GetGame()->GetBlastSuckBitmap()->Draw(x-2, y, fW, fH, rect, bkcolor);
			//GetGame()->GetBlastSuckBitmap()->Draw(x, y+2, fW, fH, rect, bkcolor);
			//GetGame()->GetBlastSuckBitmap()->Draw(x+2, y, fW, fH, rect, bkcolor);

			GetGame()->GetBlastSuckBitmap()->Draw(x, y, fW, fH, rect, dwColor);
		}
		else
		{
			//GetGame()->GetSuckBitmap()->Draw(x, y-2, fW, fH, rect, bkcolor);
			//GetGame()->GetSuckBitmap()->Draw(x-2, y, fW, fH, rect, bkcolor);
			//GetGame()->GetSuckBitmap()->Draw(x, y+2, fW, fH, rect, bkcolor);
			//GetGame()->GetSuckBitmap()->Draw(x+2, y, fW, fH, rect, bkcolor);

			GetGame()->GetSuckBitmap()->Draw(x, y, fW, fH, rect, dwColor);
		}
	}
}



/*
 * 功能:  在shape身上显示受伤数字和爆击图片
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.12.01 - lpf
 * 		修改了暴击时暴击图片的显示模式,从原来的与数字同排显示修改为现在的图片叠加在数字下面;
 *	2008.12.04 - lpf
 * 		修改暴击图片显示模式,增加渐出扩散效果,并为该函数增加了新的参数float fScale2;
 */
void CShape::DisplayNumber(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX, float fY, float fScale, float fScale2, bool bBlastAttack)
{
	D3DXVECTOR3 pos;
	if (fX==-1.f && fY ==-1.f)
		pos = D3DXVECTOR3(GetPosY(),fPos+GetHeight(),GetPosX());
	else
		pos = D3DXVECTOR3(fY, fPos+GetHeight(), fX);

	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	int len = lstrlen(str);

	int iW = (int)((float)GetGame()->GetNumberWidth() * fScale);
	long x = (long)pos2.x - len * iW / 2;
	int iH = (int)((float)GetGame()->GetNumberHeight() * fScale);
	long y = (long)pos2.y - iH / 2;

	// 爆击
	if (bBlastAttack)
	{
		RECT rect = {0, 0, GetGame()->GetBlastAttackBitmap()->GetWidth(), GetGame()->GetBlastAttackBitmap()->GetHeight()};
		float fW = rect.right * fScale;
		float fH = rect.bottom * fScale;
		long x2 = (long)(pos2.x - (fW / 2));
		long y2 = (long)(pos2.y - fH + iH / 2);

		DWORD dwColorAlpha = (color & 0xff000000);
		DWORD dwColor = dwColorAlpha | 0x00ffffff;

		float fW2 = rect.right * fScale2;
		float fH2 = rect.bottom * fScale2;

		dwColorAlpha = dwColorAlpha / 2;
		if (dwColorAlpha < 0x00000000)
			dwColorAlpha = 0x00000000;
		DWORD dwColor2 = dwColorAlpha | 0x00ffffff;

		GetGame()->GetBlastAttackBitmap()->Draw((int)(x2 - (fW2 - fW) * 0.5f), (int)(y2 - (fH2 - fH) * 0.5f), fW2, fH2, rect, dwColor2);
		GetGame()->GetBlastAttackBitmap()->Draw(x2, y2, fW, fH, rect, dwColor);
	}

	GetGame()->NumberOut(x, y-2, str, len, bkcolor, fScale, false);
	GetGame()->NumberOut(x-2, y, str, len, bkcolor, fScale, false);
	GetGame()->NumberOut(x, y+2, str, len, bkcolor, fScale, false);
	GetGame()->NumberOut(x+2, y, str, len, bkcolor, fScale, false);

	GetGame()->NumberOut(x, y, str, len, color, fScale, false);
}



/*
 * 功能:  在shape身上显示格档受伤数字和格档图片
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.12.01 - lpf
 * 		修改了暴击和普通时暴击图片的显示模式,从原来的与数字同排显示修改为现在的图片叠加在数字下面;
 */
void CShape::DisplayBlock(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX, float fY, float fScale, bool bBlastAttack)
{
	D3DXVECTOR3 pos;
	if (fX==-1.f && fY ==-1.f)
		pos = D3DXVECTOR3(GetPosY(),fPos+GetHeight(),GetPosX());
	else
		pos = D3DXVECTOR3(fY, fPos+GetHeight(), fX);

	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	int len = lstrlen(str);

	int iW = (int)((float)GetGame()->GetNumberWidth() * fScale);
	long x = (long)pos2.x - len * iW / 2;
	int iH = (int)((float)GetGame()->GetNumberHeight() * fScale);
	long y = (long)pos2.y - iH / 2;

	// 爆击
	if (bBlastAttack)
	{
		RECT rect = {0, 0, GetGame()->GetBlastBlock()->GetWidth(), GetGame()->GetBlastBlock()->GetHeight()};
		float fW = rect.right * fScale;
		float fH = rect.bottom * fScale;
		long x2 = (long)(pos2.x - (fW / 2));
		long y2 = (long)(pos2.y - fH + iH / 2);

		DWORD dwColor = (color & 0xff000000) | 0x00ffffff;
		GetGame()->GetBlastBlock()->Draw(x2, y2, fW, fH, rect, dwColor);
	}
	else	// 普通
	{
		RECT rect = {0, 0, GetGame()->GetBlock()->GetWidth(), GetGame()->GetBlock()->GetHeight()};
		float fW = rect.right * fScale;
		float fH = rect.bottom * fScale;
		long x2 = (long)(pos2.x - (fW / 2));
		long y2 = (long)(pos2.y - fH + iH / 2);

		DWORD dwColor = (color & 0xff000000) | 0x00ffffff;
		GetGame()->GetBlock()->Draw(x2, y2, fW, fH, rect, dwColor);
	}

	GetGame()->NumberOut(x, y-2, str, len, bkcolor, fScale, false);
	GetGame()->NumberOut(x-2, y, str, len, bkcolor, fScale, false);
	GetGame()->NumberOut(x, y+2, str, len, bkcolor, fScale, false);
	GetGame()->NumberOut(x+2, y, str, len, bkcolor, fScale, false);

	GetGame()->NumberOut(x, y, str, len, color, fScale, false);
}



/*
 * 功能: 在shape身上显示反弹受伤数字和反弹图片
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.12.03 - lpf
 * 		取消了反弹的字符图片显示,仅仅显示数字;
 */
void CShape::DisplayRebound(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX, float fY, float fScale, bool bBlastAttack)
{
	D3DXVECTOR3 pos;
	if (fX==-1.f && fY ==-1.f)
		pos = D3DXVECTOR3(GetPosY(),fPos+GetHeight(),GetPosX());
	else
		pos = D3DXVECTOR3(fY, fPos+GetHeight(), fX);

	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	int len = lstrlen(str);

	int iW = (int)((float)GetGame()->GetNumberWidth() * fScale);
	long x = (long)pos2.x - len * iW / 2;
	int iH = (int)((float)GetGame()->GetNumberHeight() * fScale);
	long y = (long)pos2.y - iH / 2;

	GetGame()->NumberOut(x, y-2, str, len, bkcolor, fScale, false);
	GetGame()->NumberOut(x-2, y, str, len, bkcolor, fScale, false);
	GetGame()->NumberOut(x, y+2, str, len, bkcolor, fScale, false);
	GetGame()->NumberOut(x+2, y, str, len, bkcolor, fScale, false);

	GetGame()->NumberOut(x, y, str, len, color, fScale, false);

	//RECT rect = {0, 0, GetGame()->GetRebound()->GetWidth(), GetGame()->GetRebound()->GetHeight()};
	//float fW = rect.right * fScale;
	//float fH = rect.bottom * fScale;
	//x = x + len * GetGame()->GetNumberWidth();
	//// 爆击
	//if (bBlastAttack)
	//{
	//	GetGame()->GetBlastRebound()->Draw(x, y-2, fW, fH, rect, bkcolor);
	//	GetGame()->GetBlastRebound()->Draw(x-2, y, fW, fH, rect, bkcolor);
	//	GetGame()->GetBlastRebound()->Draw(x, y+2, fW, fH, rect, bkcolor);
	//	GetGame()->GetBlastRebound()->Draw(x+2, y, fW, fH, rect, bkcolor);

	//	GetGame()->GetBlastRebound()->Draw(x, y, fW, fH, rect, color);
	//}
	//else	// 普通
	//{
	//	GetGame()->GetRebound()->Draw(x, y-2, fW, fH, rect, bkcolor);
	//	GetGame()->GetRebound()->Draw(x-2, y, fW, fH, rect, bkcolor);
	//	GetGame()->GetRebound()->Draw(x, y+2, fW, fH, rect, bkcolor);
	//	GetGame()->GetRebound()->Draw(x+2, y, fW, fH, rect, bkcolor);

	//	GetGame()->GetRebound()->Draw(x, y, fW, fH, rect, color);
	//}
}



/*
 * 功能: 在shape身上显示闪避图片
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.12.03 - lpf
 * 		显示字符图片,仅仅采用单一图片显示模式,不再由程序进行描边显示;
 */
void CShape::DisplayMiss(D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX, float fY, float fScale, bool bBlastAttack)
{
	D3DXVECTOR3 pos;
	if (fX==-1.f && fY ==-1.f)
		pos = D3DXVECTOR3(GetPosY(),fPos+GetHeight(),GetPosX());
	else
		pos = D3DXVECTOR3(fY, fPos+GetHeight(), fX);

	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	RECT rect = {0, 0, GetGame()->GetFullMissBitmap()->GetWidth(), GetGame()->GetFullMissBitmap()->GetHeight()};
	float fW = rect.right * fScale;
	float fH = rect.bottom * fScale;
	long x = (long)(pos2.x - fW / 2);
	long y = (long)(pos2.y - fH / 2);
	DWORD dwColor = (color & 0xff000000) | 0x00ffffff;

	// 爆击
	if(bBlastAttack)
	{
		//GetGame()->GetBlastFullMiss()->Draw(x, y-2, fW, fH, rect, bkcolor);
		//GetGame()->GetBlastFullMiss()->Draw(x-2, y, fW, fH, rect, bkcolor);
		//GetGame()->GetBlastFullMiss()->Draw(x, y+2, fW, fH, rect, bkcolor);
		//GetGame()->GetBlastFullMiss()->Draw(x+2, y, fW, fH, rect, bkcolor);

		GetGame()->GetBlastFullMiss()->Draw(x, y, fW, fH, rect, dwColor);
	}
	else	// 普通
	{
		//GetGame()->GetFullMissBitmap()->Draw(x, y-2, fW, fH, rect, bkcolor);
		//GetGame()->GetFullMissBitmap()->Draw(x-2, y, fW, fH, rect, bkcolor);
		//GetGame()->GetFullMissBitmap()->Draw(x, y+2, fW, fH, rect, bkcolor);
		//GetGame()->GetFullMissBitmap()->Draw(x+2, y, fW, fH, rect, bkcolor);

		GetGame()->GetFullMissBitmap()->Draw(x, y, fW, fH, rect, dwColor);
	}	
}



/*
 * 功能: 在shape身上显示招架图片
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.12.03 - lpf
 * 		显示字符图片,仅仅采用单一图片显示模式,不再由程序进行描边显示;
 */
void CShape::DisplayParry(D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX, float fY, float fScale, bool bBlastAttack)
{
	D3DXVECTOR3 pos;
	if (fX==-1.f && fY ==-1.f)
		pos = D3DXVECTOR3(GetPosY(),fPos+GetHeight(),GetPosX());
	else
		pos = D3DXVECTOR3(fY, fPos+GetHeight(), fX);

	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	RECT rect = {0, 0, GetGame()->GetParry()->GetWidth(), GetGame()->GetParry()->GetHeight()};
	float fW = rect.right * fScale;
	float fH = rect.bottom * fScale;
	long x = (long)(pos2.x - fW / 2);
	long y = (long)(pos2.y - fH / 2);
	DWORD dwColor = (color & 0xff000000) | 0x00ffffff;

	// 爆击
	if(bBlastAttack)
	{
		//GetGame()->GetBlastParry()->Draw(x, y-2, fW, fH, rect, bkcolor);
		//GetGame()->GetBlastParry()->Draw(x-2, y, fW, fH, rect, bkcolor);
		//GetGame()->GetBlastParry()->Draw(x, y+2, fW, fH, rect, bkcolor);
		//GetGame()->GetBlastParry()->Draw(x+2, y, fW, fH, rect, bkcolor);

		GetGame()->GetBlastParry()->Draw(x, y, fW, fH, rect, dwColor);
	}
	else	// 普通
	{
		//GetGame()->GetParry()->Draw(x, y-2, fW, fH, rect, bkcolor);
		//GetGame()->GetParry()->Draw(x-2, y, fW, fH, rect, bkcolor);
		//GetGame()->GetParry()->Draw(x, y+2, fW, fH, rect, bkcolor);
		//GetGame()->GetParry()->Draw(x+2, y, fW, fH, rect, bkcolor);

		GetGame()->GetParry()->Draw(x, y, fW, fH, rect, dwColor);
	}
}



/*
 * 功能: 在shape身上显示抵抗图片
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.12.03 - lpf
 * 		显示字符图片,仅仅采用单一图片显示模式,不再由程序进行描边显示;
 */
void CShape::DisplayResist(D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX, float fY, float fScale, bool bBlastAttack)
{
	D3DXVECTOR3 pos;
	if (fX==-1.f && fY ==-1.f)
		pos = D3DXVECTOR3(GetPosY(),fPos+GetHeight(),GetPosX());
	else
		pos = D3DXVECTOR3(fY, fPos+GetHeight(), fX);

	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	RECT rect = {0, 0, GetGame()->GetResist()->GetWidth(), GetGame()->GetResist()->GetHeight()};
	float fW = rect.right * fScale;
	float fH = rect.bottom * fScale;
	long x = (long)(pos2.x - fW / 2);
	long y = (long)(pos2.y - fH / 2);
	DWORD dwColor = (color & 0xff000000) | 0x00ffffff;

	// 爆击
	if(bBlastAttack)
	{
		//GetGame()->GetBlastResist()->Draw(x, y-2, fW, fH, rect, bkcolor);
		//GetGame()->GetBlastResist()->Draw(x-2, y, fW, fH, rect, bkcolor);
		//GetGame()->GetBlastResist()->Draw(x, y+2, fW, fH, rect, bkcolor);
		//GetGame()->GetBlastResist()->Draw(x+2, y, fW, fH, rect, bkcolor);

		GetGame()->GetBlastResist()->Draw(x, y, fW, fH, rect, dwColor);
	}
	else	// 普通
	{
		//GetGame()->GetResist()->Draw(x, y-2, fW, fH, rect, bkcolor);
		//GetGame()->GetResist()->Draw(x-2, y, fW, fH, rect, bkcolor);
		//GetGame()->GetResist()->Draw(x, y+2, fW, fH, rect, bkcolor);
		//GetGame()->GetResist()->Draw(x+2, y, fW, fH, rect, bkcolor);

		GetGame()->GetResist()->Draw(x, y, fW, fH, rect, dwColor);
	}
}



/*
 * 功能: 在shape身上显示丢失图片
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.12.03 - lpf
 * 		显示字符图片,仅仅采用单一图片显示模式,不再由程序进行描边显示;
 */
void CShape::DisplayLost(D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX, float fY, float fScale, bool bBlastAttck)
{
	D3DXVECTOR3 pos;
	if (fX==-1.f && fY ==-1.f)
		pos = D3DXVECTOR3(GetPosY(),fPos+GetHeight(),GetPosX());
	else
		pos = D3DXVECTOR3(fY, fPos+GetHeight(), fX);

	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	RECT rect = {0, 0, GetGame()->GetResist()->GetWidth(), GetGame()->GetResist()->GetHeight()};
	float fW = rect.right * fScale;
	float fH = rect.bottom * fScale;
	long x = (long)(pos2.x - fW / 2);
	long y = (long)(pos2.y - fH / 2);
	DWORD dwColor = (color & 0xff000000) | 0x00ffffff;

	//GetGame()->GetLost()->Draw(x, y-2, fW, fH, rect, bkcolor);
	//GetGame()->GetLost()->Draw(x-2, y, fW, fH, rect, bkcolor);
	//GetGame()->GetLost()->Draw(x, y+2, fW, fH, rect, bkcolor);
	//GetGame()->GetLost()->Draw(x+2, y, fW, fH, rect, bkcolor);

	GetGame()->GetLost()->Draw(x, y, fW, fH, rect, dwColor);
}



/*
 * 功能: 绘制血条
 * 摘要: -
 * 参数: pBitmap	- 血槽图片资源
 *		 lHpBarLong	- 血槽长度
 *		 lHp		- 角色当前HP
 *		 lMaxHp		- 角色最大HP
 *		 fPos		- 绘制高度
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2007.11.27 - lpf
 *		当前血条绘制高度依据显示对象的PickBox高度
 *	2008.03.01 - lpf
 *		调整了绘制血槽的背景X坐标,以修正背景与血条的错位问题
 *	2008.06.19 - lpf
 *		将血槽的绘制坐标向上提高了10个像素
 *	2008.06.20 - lpf
 *		将血槽的绘制高度坐标受传入参数影响
*	2008.10.17 - lpf
*		增加了局部变量赋予初始值;
*	2008.12.25 - lpf
*		调整了血条绘制坐标为血条图片的一半(计算而来);
 */
void CShape::DisplayHpBar(CBitmapX * pBitmap, long lHpBarLong, long lHp, long lMaxHp, float fPos)
{
	D3DXMATRIX	matFinal;
	D3DXVECTOR3 pos(GetPosY(), GetPickBoxHeight() + GetHeight(), GetPosX());
	D3DXVECTOR3 pos2;

	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	if (lMaxHp < lHp)
		lMaxHp = lHp;

	long x = (long)pos2.x - GetGame()->GetHpBarBackBitmap()->GetWidth() / 2;
	long y = (long)fPos;

	RECT rect  = { 0, 0, lHpBarLong, 7 };
	rect.right =(long)((float)lHp / (float)lMaxHp * (float)lHpBarLong);

	if (lHp > 0 && rect.right < 2)
		rect.right = 2;

	// 绘制血条的背景
	if (GetGame()->GetHpBarBackBitmap())
		GetGame()->GetHpBarBackBitmap()->Draw(x - 2, y - 3);

	if(pBitmap)
		pBitmap->Draw(x, y, rect);
}



// 获取面对的坐标
void CShape::GetFacePos(long& x, long& y)
{
	static int pos[8][2] = {0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1};
	long lDir = GetDir();
	x = GetTileX() + pos[lDir][0];
	y = GetTileY() + pos[lDir][1];
}

//获取面对的目标
CShape* CShape::GetFaceShape()
{
	long lTileX = 0;
	long lTileY = 0;
	GetFacePos(lTileX,lTileY);
	return ((CClientRegion*)GetFather())->GetShape(lTileX,lTileY);
}

// 获取dir方向的坐标
void CShape::GetDirPos(long dir, long& x, long& y)
{
	static int pos[8][2] = {0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1};
	x = x + pos[dir][0];
	y = y + pos[dir][1];
}
/***********************************************************************/
/*  zhaohang 2010-11-15 add*/
void CShape::GetDirPos(long dir,float& x,float& y)
{
	static float pos[8][2] = {0.0f,-1.0f,
		1.0f,-1.0f, 
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,
		-1.0f,1.0f,
		-1.0f,0.0f,
		-1.0f,-1.0f};
	x = x + pos[dir][0];
	y = y + pos[dir][0];
}
/***********************************************************************/
void CShape::GetDirPosFromDistance(long dir, long& x, long& y, DWORD lCell)
{
	if (lCell==0)
	{
		MessageBox(g_hWnd,"Distance can not be set Zero！","Error",MB_OK);
		return;
	}
	static int pos[8][2] ={0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1};
	x = x + pos[dir][0] * lCell;
	y = y + pos[dir][1] * lCell;
}

// 获取面对的坐标
long CShape::GetFaceDir()
{

	static int dir[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };
	return dir[GetDir()];
}

void CShape::GetPosFromCameraView(eDIR Dir, long &x,long &y)
{
	CClientRegion *pRegion = GetGame()->GetRegion();
	if(pRegion == NULL)	return;
	render::Camera *pCamera = pRegion->GetCamera();
	if(pCamera == NULL) return;

	D3DXVECTOR3 vecPlayerPos = D3DXVECTOR3(m_fPosX,m_fHeight,m_fPosY);
	D3DXVECTOR3	vecCameraPos = *(pCamera->GetPosition());
	vecCameraPos = D3DXVECTOR3(vecCameraPos.z,vecCameraPos.y,vecCameraPos.x);
	float dd = sqrtf( pow((vecCameraPos.x - m_fPosX),2) + pow((vecCameraPos.z - m_fPosY),2));
	if(dd >= 4.0f)
		GetGame()->GetGameControl()->m_fParameterOfDisFromCamara = 1.125f;
	float fDx = 0.0f,fDy = 0.0f,fd = 0.0f;
	// 空间上两点间的直线方程
	// P = p1 + t * (p2 - p1)
	switch (Dir)
	{
	case DIR_UP:
		do 
		{
			fDx = vecCameraPos.x + GetGame()->GetGameControl()->m_fParameterOfDisFromCamara * (vecPlayerPos.x - vecCameraPos.x);
			fDy = vecCameraPos.z + GetGame()->GetGameControl()->m_fParameterOfDisFromCamara * (vecPlayerPos.z - vecCameraPos.z);
			//判断目标点的位置和下一步的距离必须大于1小于2
			fd = sqrtf(pow((fDx - m_fPosX),2) + pow((fDy - m_fPosY),2));
			if (fd < 1.1f)
				GetGame()->GetGameControl()->m_fParameterOfDisFromCamara += 0.025f;
			else if(fd > 2.0f)
				GetGame()->GetGameControl()->m_fParameterOfDisFromCamara -= 0.025f;
		} while (fd < 1.1f || fd > 2.0f);
		x = (long)fDx;
		y = (long)fDy;
		break;
	case DIR_LEFT:break;
	case DIR_RIGHT:break;
	case DIR_DOWN:break;
	case DIR_LEFTUP:break;
	case DIR_LEFTDOWN:break;
	case DIR_RIGHTUP:break;
	case DIR_RIGHTDOWN:break;
	default:break;
	}
}

//相对于自身面对方向上，获取Dir方向上的坐标
void CShape::GetPosFromRelativeDir(CShape::eDIR Dir,long &x, long &y,DWORD lDis)
{
	int lRelatvieDir = GetDir() + Dir;
	if (lRelatvieDir > 7)
		lRelatvieDir -= 8;
	GetDirPos(lRelatvieDir,x,y);
}
/***********************************************************************/
/*  zhaohang 2010-11-15 add*/
void CShape::GetPosFromRelativeDir(eDIR Dir,float &x,float &y,DWORD lDis /* = 1 */)
{
	int lRelatvieDir = GetDir() + Dir;
	if (lRelatvieDir > 7)
		lRelatvieDir -= 8;
	GetDirPos(lRelatvieDir,x,y);
}
/***********************************************************************/
// 求两个Shape间的距离（单位：格子）
long CShape::Distance(CShape* pShape)
{
	//return max( abs(GetTileX() - pShape->GetTileX()), abs(GetTileY() - pShape->GetTileY()) ) - GetFigure(DIR_UP) - pShape->GetFigure(DIR_UP);
	return max( abs(GetTileX() - pShape->GetTileX()) - GetFigure(DIR_RIGHT) - pShape->GetFigure(DIR_RIGHT),
		abs(GetTileY() - pShape->GetTileY()) - GetFigure(DIR_UP) - pShape->GetFigure(DIR_UP) );
}

// 求x,y方向上的最小距离（单位：格子）
long CShape::MinXYDistance(CShape* pShape)
{
	return min( abs(GetTileX() - pShape->GetTileX()) - GetFigure(DIR_RIGHT) - pShape->GetFigure(DIR_RIGHT),
		abs(GetTileY() - pShape->GetTileY()) - GetFigure(DIR_UP) - pShape->GetFigure(DIR_UP) );
}

// 与服务器的RealDistance( FLOAT fX, FLOAT fY )函数统一
FLOAT CShape::GetRealDistance( FLOAT fX, FLOAT fY )
{
	FLOAT fXOffset = fX - GetTileX();
	FLOAT fYOffset = fY - GetTileY();
	return sqrt( fYOffset * fYOffset + fXOffset * fXOffset );
}

FLOAT CShape::RealDistance( FLOAT fX, FLOAT fY )
{
	FLOAT fXOffset = fX - GetPosX();
	FLOAT fYOffset = fY - GetPosY();
	return sqrt( fYOffset * fYOffset + fXOffset * fXOffset );
}

LONG CShape::RealDistance( LONG lX, LONG lY )
{
	LONG lResult = 0x7FFFFFFF;

	FLOAT fDistance = RealDistance( static_cast<FLOAT>(lX), static_cast<FLOAT>(lY) );

	FLOAT fDecimalPart = fDistance - static_cast<LONG>(fDistance);
	if( fDecimalPart > 0.5 )
	{
		lResult = static_cast<LONG>( fDistance ) + 1;
	}
	else 
	{
		lResult = static_cast<LONG>( fDistance );
	}
	return lResult;
}

LONG CShape::RealDistance( CShape* pTarget )
{
	if(this == pTarget)
		return 0;
	if(pTarget->GetTileX() == this->GetTileX() && pTarget->GetTileY() == this->GetTileY())
		return 0;
	LONG lResult = 0x7FFFFFFF;
	if( pTarget )
	{
		FLOAT fResult = GetRealDistance( static_cast<FLOAT>( pTarget -> GetTileX() ), static_cast<FLOAT>( pTarget -> GetTileY() ) );

		FLOAT fDeltaX = abs( pTarget -> GetTileX() - static_cast<FLOAT>( GetTileX() ) );
		FLOAT fDeltaY = abs( pTarget -> GetTileY() - static_cast<FLOAT>( GetTileY() ) );

		LONG lDeltaX = static_cast<LONG>( fDeltaX ) - pTarget -> GetFigure( DIR_RIGHT ) - GetFigure( DIR_RIGHT );
		LONG lDeltaY = static_cast<LONG>( fDeltaY ) - pTarget -> GetFigure( DIR_UP ) - GetFigure( DIR_UP );

		if( lDeltaX >= lDeltaY )
		{
			fResult = fResult * ( lDeltaX / fDeltaX );
		}
		else
		{
			fResult = fResult * ( lDeltaY / fDeltaY );
		}

		FLOAT fDecimalPart = fResult - static_cast<LONG>(fResult);
		if( fDecimalPart > 0.5 )
		{
			lResult = static_cast<LONG>( fResult ) + 1;
		}
		else 
		{
			lResult = static_cast<LONG>( fResult );
		}
	}
	return lResult;
}

// 查找是否有特效
CEffect* CShape::FindEffect(long lGraphicsID, long lID)
{
	for (list<CEffect*>::iterator it = m_listEffect.begin(); it!=m_listEffect.end(); it++)
	{
		if ((*it)->GetGraphicsID() == lGraphicsID && (*it)->GetEffectID() == lID)
		{
			return *it;
		}
	}
	return NULL;
}

// 查找是否有特效
CEffect* CShape::FindEffectByNum(long lNum)
{
	long n=0;
	for (list<CEffect*>::iterator it = m_listEffect.begin(); it!=m_listEffect.end(); it++)
	{
		if (n++==lNum)
			return *it;
	}
	return NULL;
}

// 播放特效
const CGUID& CShape::AddEffect(long lGraphicsID, float fx, float fy, float fHeight,
							   bool bLoop, float fDir, long lID,bool bAsyn)
{
	if (lGraphicsID == 0)
		return CGUID::GUID_INVALID;

	if (fx==-99999.f)
		fx = GetPosX();

	if (fy==-99999.f)
		fy = GetPosY();

	if (fHeight==-99999.f)
		fHeight = GetHeight();

	
	CEffect* pEffect = new CEffect;
	pEffect->SetGraphicsID(lGraphicsID);
	pEffect->SetDisplayModel(bAsyn);
	pEffect->SetEffectID(lID);
	//设置模型显示的Group
	pEffect->SetDisplayModelGroup(bAsyn);
	pEffect->SetPosXY(fx, fy);
	pEffect->SetHeight(fHeight);
	pEffect->SetLoop(bLoop);
	CGUID guid;
	CGUID::CreateGUID(guid);
	pEffect->SetExID(guid);
	// 特效ID计数+1；
	/*if(lID != 0)
	{
		pEffect->SetEffectID(lEffectID);
		++lEffectID;
	}
	else
		*/
	
	map<DWORD,CEffectSetup::tagEffectSetup>::iterator iter=CEffectSetup::GetEffectSetup()->find(pEffect->GetGraphicsID());
	if(iter!=CEffectSetup::GetEffectSetup()->end())
	{
		if (iter->second.bEnable)
		{
			pEffect->SetLight(iter->second.dwColor,iter->second.dwType,iter->second.fRadius,iter->second.dwFadeStartTime,iter->second.dwFadeLength);
		}
	}

	pEffect->SetDirEx(fDir);
	pEffect->SetNowDir(fDir);

	m_listEffect.push_back(pEffect);
	return pEffect->GetExID();
}



/*
* 功能: 播放特效(定位点)(如果lID==0表示默认特效ID)
* 摘要: -
* 参数: 略
* 返回值: -
* 作者: 刘科
* 创建日期: -
* 修改日志:
*	2008.12.10 - lpf
*		添加高度修正值的设定;
*/
const CGUID& CShape::AddEffectLocator(long lGraphicsID, CShape* pHoster, eLOCATOR eLocator,
									  bool bLoop, float fDir, long lID,bool bAsyn, float fHeightAdjust)
{
	CEffect* pEffect = new CEffect;
	pEffect->SetGraphicsID(lGraphicsID);
	pEffect->SetDisplayModel(bAsyn);
	pEffect->SetEffectID(lID);
	pEffect->SetDisplayModelGroup(bAsyn);
	pEffect->SetPosXY(GetPosX(), GetPosY());
	pEffect->SetHeight(GetHeight());
	pEffect->SetLoop(bLoop);
	CGUID guid;
	CGUID::CreateGUID(guid);
	pEffect->SetExID(guid);
	pEffect->SetHeightAdjust(fHeightAdjust);

	// 特效ID计数+1；
	/*if(lID != 0)
	{
		pEffect->SetEffectID(lEffectID);
		++lEffectID;
	}
	else
		*/
	
	map<DWORD,CEffectSetup::tagEffectSetup>::iterator iter=CEffectSetup::GetEffectSetup()->find(pEffect->GetGraphicsID());
	if(iter!=CEffectSetup::GetEffectSetup()->end())
	{
		if (iter->second.bEnable)
		{
			pEffect->SetLight(iter->second.dwColor,iter->second.dwType,iter->second.fRadius,iter->second.dwFadeStartTime,iter->second.dwFadeLength);
		}
	}

	pEffect->SetDirEx(fDir);
	pEffect->SetNowDir(fDir);

	if (pHoster)
	{
		long lHosterType, lHosterID;
		pHoster->GetCurGraphicTypeID(lHosterType, lHosterID);
		pEffect->SetHost(pHoster->GetType(), lHosterType, lHosterID, pHoster->GetExID(), eLocator);
	}

	m_listEffect.push_back(pEffect);
	return pEffect->GetExID();
}

// 删除技能
void CShape::DelEffect(long lGrahpicsID,long lID)
{
	for (list<CEffect*>::iterator it = m_listEffect.begin(); it!=m_listEffect.end(); it++)
	{
		if ((*it)->GetGraphicsID() == lGrahpicsID && (*it)->GetEffectID() == lID)
		{
			//CEffect* pEffect = (*it);
			SAFE_DELETE((*it));
			m_listEffect.erase(it);
		//	pEffect->SetIsDeleted(true);
			return;
		}
	}
}

// 删除技能
void CShape::DelAllEffect()
{
	for (list<CEffect*>::iterator it = m_listEffect.begin(); it!=m_listEffect.end(); it++)
	{
		CEffect* pEffect = (*it);
		SAFE_DELETE(pEffect);
	//	pEffect->SetIsDeleted(true);
	}
	m_listEffect.clear();
}

// 根据索引获取定位点的值
CShape::eLOCATOR CShape::GetLocatorName(DWORD dwIndex)
{
	switch(dwIndex)
	{
	case 0:
		return LOCATOR_WIND;
	case 1:
		return LOCATOR_HEAD;
	case 2:
		return LOCATOR_FOOTCENTER;
	case 3:
		return LOCATOR_LEFTHAND;
	case 4:
		return LOCATOR_RIGHTHAND;
	case 5:
		return LOCATOR_LEFTFOOT;
	case 6:
		return LOCATOR_RIGHTFOOT;
	default:
		return LOCATOR_WIND;
	}
}

// 获取从(sx,sy)到(dx,dy)的方向(0-PI)
float CShape::GetLineDirF(float sx, float sy, float dx, float dy, float fdir)
{
	D3DXVECTOR3 vSour(sy, 0.f, sx);
	D3DXVECTOR3 vDest(dy, 0.f, dx);

	D3DXVECTOR3 dir = vSour - vDest;

	float fDirection = fdir;
	D3DXVec3Normalize(&dir,&dir);
	if( dir.x != 0 )
	{
		float fDir =  atanf( dir.z/dir.x );
		if( dir.x < 0 )
			fDir += D3DX_PI;
		fDirection = - fDir-D3DX_PI/2;
	}
	else
	{
		if(dir.z != 0 )
			fDirection = - dir.z/fabsf(dir.z)*D3DX_PI/2-D3DX_PI/2 ;
	}

	return fDirection;
}

//从幅度方向值转换到8个枚举值方向
CShape::eDIR CShape::RadianDirTo8Dir(float fRadianDir)
{
	float fDir = fRadianDir;
	if(fDir<0)
	fDir += 2*D3DX_PI;
	eDIR dir = CShape::DIR_UP;

	if(D3DX_PI/4 - fDir > 0.4 )
	dir = CShape::DIR_RIGHT;
	else if(D3DX_PI/2 - fDir >0.4  )
	dir = CShape::DIR_RIGHTDOWN;
	else if( D3DX_PI*3/4 - fDir  > 0.4 )
	dir = CShape::DIR_DOWN;
	else if(D3DX_PI  - fDir > 0.4 )
	dir = CShape::DIR_LEFTDOWN;
	else if(  D3DX_PI*5/4 -fDir  > 0.4 )
	dir = CShape::DIR_LEFT;
	else if( D3DX_PI*3/2 - fDir  > 0.4 )
	dir = CShape::DIR_LEFTUP;
	else if( D3DX_PI*7/4 - fDir  > 0.4 )
	dir = CShape::DIR_UP;
	else if(2 * D3DX_PI - fDir > 0.4)
	dir = CShape::DIR_RIGHTUP;
	else 
	dir = CShape::DIR_RIGHT;
	return dir;
}



/*
 * 功能: 设定是否为删除对象
 * 摘要: -
 * 参数: bIs - 是否为删除对象
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.10.09 - lpf
 * 		增加了对当前对象是否处于渐变状态的判断;
 *	2008.10.21 - lpf
 * 		增加了对当前对象是否已经加入到渐变列表的判断;
 */         
void CShape::SetIsDeleted(bool bIs)
{
	if (bIs)
	{
		if (m_bIsNeedAlphaFade)
		{
  			if (!m_bIsAddAlphaFaded)
			{
				CAlphaFadeObjectList & ObjectList = GetGame()->GetRegion()->GetAlphaFadeObjectList();
				ObjectList.AddFadeOutObject(this);
			}
		}

		m_bIsDeleted = true;
	}else
	{
		if (m_bIsNeedAlphaFade)
			m_eAlphaFadeMode = CAlphaFadeObjectList::ALPHA_FADE_IN;

		m_bIsDeleted = false;
	}
}



/*
 * 功能: 得到是否为删除对象
 * 摘要: -
 * 参数: -
 * 返回值: 是否为删除对象
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.10.09 - lpf
 * 		增加了对当前对象是否处于渐变状态的判断;
 */
bool CShape::GetIsDeleted()
{
	return m_bIsDeleted;
}



/*
 * 功能: 是否需要Alpha渐变
 * 摘要: -
 * 参数: -
 * 返回值: 是否为删除对象
 * 作者: lpf
 * 创建日期: 2008.10.13
 * 修改日志:
 */
void CShape::SetIsNeedAlphaFade(bool bIs)
{
	m_bIsNeedAlphaFade = bIs;
}



/*
 * 功能: 选中对象信息显示
 * 摘要: -
 * 参数: dwColor - 类型颜色
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.07.07 - lpf
 * 		增加了选中对象,则显示其信息,并删除了原头顶居中图片显示
 *	2008.08.26 - lpf
 * 		修改了高度参考坐标取值;
 *	2008.10.07 - lpf
 *		调整了所有关于称号高度的显示
 *		增加了"显示个人商店或者其他的头顶上的文字"的显示
*	2008.10.17 - lpf
*		增加了局部变量赋予初始值;
 */
void CShape::DisplayHoverArrow(DWORD dwColor)
{
	CClientRegion *pRegion = GetGame()->GetRegion();
	render::Camera *pCamera = pRegion->GetCamera();

	D3DXVECTOR3 pos(GetPosY(),GetHeight() + GetPickBoxHeight(),GetPosX());
	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	long x = (long)pos2.x - 12;
	long y = (long)pos2.y - 12;

	// 如果选中对象,则显示其信息
	m_bIsSelected = true;

//	if (GetGame()->GetGameControl()->GetMouseShape() == this)
		float fPos = pRegion->DisplayShapeName(this, true, true);
//	else
//		pRegion->DisplayShapeName(this, true, false);

	CMonster * pMonster = dynamic_cast<CMonster *>(this);
	if (pMonster)
		pRegion->DisplayMonsterTitle(pMonster, true, fPos);

	// 显示其他头顶信息
	CPlayer * pPlayer = dynamic_cast<CPlayer*>(this);
	if (pPlayer != NULL)
	{				
		if (pPlayer->IsPlayerShopOpened())
			pRegion->ShowShopName(pPlayer, fPos);
		else if (pPlayer->IsRecruitingTeam())
			pRegion->ShowPlayerRecruiting(pPlayer, fPos);

		// 其他玩家的头顶提示文字
		if (pPlayer != GetGame()->GetMainPlayer() && pPlayer->GetPlayerInfo().size() > 0)
			pRegion->ShowPlayerInfo(pPlayer);
	}

	m_bIsSelected = false;

	// 普通目标
	if (dwColor==1 && GetGame()->m_pMouseShapeFoot->GetDisplayModel())
	{
		GetGame()->m_pMouseShapeFoot->GetAnimInfo()->SetupAnimInfo(GetPosX(), GetPosY(), GetHeight()+0.1f,GetNowDir(),pCamera);
		GetGame()->m_pMouseShapeFoot->GetDisplayModel()->Display(GetGame()->m_pMouseShapeFoot->GetAnimInfo());
	}
	else if (dwColor==2 && GetGame()->m_pMouseTeammerFoot->GetDisplayModel() )	// 队友
	{
		GetGame()->m_pMouseTeammerFoot->GetAnimInfo()->SetupAnimInfo(GetPosX(), GetPosY(), GetHeight()+0.1f,GetNowDir(), pCamera);
		GetGame()->m_pMouseTeammerFoot->GetDisplayModel()->Display(GetGame()->m_pMouseTeammerFoot->GetAnimInfo());
	}
	else if (dwColor==3 && GetGame()->m_pLockAttackShapeFoot->GetDisplayModel())	// 锁定攻击目标
	{
		GetGame()->m_pLockAttackShapeFoot->GetAnimInfo()->SetupAnimInfo(GetPosX(), GetPosY(), GetHeight()+0.1f,GetNowDir(), pCamera);
		GetGame()->m_pLockAttackShapeFoot->GetDisplayModel()->Display(GetGame()->m_pLockAttackShapeFoot->GetAnimInfo());
	}
}



//================07.12.19liuke==================
// 查找指定索引的特效
CEffect* CShape::FindEffectByIndex(CGUID guid)
{
	if(guid == CGUID::GUID_INVALID)
		return NULL;
	for (list<CEffect*>::iterator it = m_listEffect.begin(); it!=m_listEffect.end(); it++)
	{
		if ((*it)->GetExID() == guid)
		{
			return *it;
		}
	}
	return NULL;
}

// 删除指定索引的特效
void CShape::DelEffectByIndex(CGUID guid)
{
	if(guid == CGUID::GUID_INVALID)
		return;
	for (list<CEffect*>::iterator it = m_listEffect.begin(); it!=m_listEffect.end(); it++)
	{
		if ((*it)->GetExID() == guid)
		{
			CEffect* pEffect = (*it);
			SAFE_DELETE(pEffect);
			m_listEffect.erase(it);
			return;
		}
	}
}

// 将指定索引的特效从列表中移出
void CShape::RemoveEffectByIndex(CGUID guid)
{
	if(guid == CGUID::GUID_INVALID)
		return;
	for (list<CEffect*>::iterator it = m_listEffect.begin(); it!=m_listEffect.end(); it++)
	{
		if ((*it)->GetExID() == guid)
		{
			CEffect* pEffect = (*it);
			m_listEffect.erase(it);
			return;
		}
	}
}
//===============================================

void CShape::SetActionToLastFrame()
{
	m_pAnimInfo->GetActionInfo()->bStayToLastFrame = true;
	
}



/*
 * 功能: 拷贝当前对象的显示信息到新的对象中
 * 摘要: -
 * 参数: pOutObj - 输出新的对象指针的引用
 * 返回值: 返回创建新的对象指针
 * 作者: lpf
 * 创建日期: 2008.10.15
 * 修改日志:
 *	2008.10.21 - lpf
 * 		增加了对当前对象是玩家时,对其显示装备数据的拷贝;
 *	2008.10.22- lpf
 * 		增加了对采集物当前显示模型等级数据的设定;
 *	2008.10.23- lpf
 * 		增加了对坐骑显示数据的设定;
 *	2008.10.29- lpf
 * 		修正了在拷贝AnimInfo时的内存泄露;
 *	2008.10.31- lpf
 * 		修正了坐骑数据拷贝不全面,下线后坐骑无法显示动作,且不是消失时的动作;
 *	2009.04.10- lpf
 * 		修正了玩家时装秀时显示不对,没有拷贝披风装备;
 */
bool CShape::CopyShapeDisplayInfo(CShape *& pOutObj)
{
	if (pOutObj != NULL)
		return false;

	long lType = GetType();
	switch (lType)
	{
	case TYPE_PLAYER:
		{
			pOutObj = new CPlayer();
			pOutObj->SetType(TYPE_PLAYER);

			CPlayer * pNewPlayer = dynamic_cast<CPlayer *>(pOutObj);
			CPlayer * pOldPlayer = dynamic_cast<CPlayer *>(this);

			pNewPlayer->SetHairPic(pOldPlayer->GetHairPic());
			pNewPlayer->SetFacePic(pOldPlayer->GetFacePic());

			//===========================
			// -- 左边装备 
			//pNewPlayer->SetEquipIndex(CPlayer::EQUIP_HEAD, nHead);
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_NECKLACE, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_NECKLACE));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_WING,  pOldPlayer->GetEquipIndex(CPlayer::EQUIP_WING));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_BODY, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_BODY));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_BACK, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_BACK));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_HAND, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_HAND));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_SHOULDER, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_SHOULDER));

			// -- 右边装备
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_WRIST, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_WRIST));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_LEG, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_LEG));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_BOOT,  pOldPlayer->GetEquipIndex(CPlayer::EQUIP_BOOT));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_LRING, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_LRING));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_RRING, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_RRING));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_WEAPON, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_CARD, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_CARD));

			// -- 下边装备
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_GODGOODS, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_GODGOODS));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_WEAPON, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_EFFIGY, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_EFFIGY));
			pNewPlayer->SetEquipIndex(CPlayer::EQUIP_LIFECOWRY, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_LIFECOWRY));
            pNewPlayer->SetEquipIndex(CPlayer::EQUIP_SOULGUARD, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_SOULGUARD));
            pNewPlayer->SetEquipIndex(CPlayer::EQUIP_RIDER, pOldPlayer->GetEquipIndex(CPlayer::EQUIP_RIDER));

			// --装备等级
			// -- 左边装备
			pNewPlayer->SetEquipLevel(CPlayer::EQUIP_HEAD, pOldPlayer->GetEquipLevel(CPlayer::EQUIP_HEAD));
			pNewPlayer->SetEquipLevel(CPlayer::EQUIP_NECKLACE, pOldPlayer->GetEquipLevel(CPlayer::EQUIP_NECKLACE));
			pNewPlayer->SetEquipLevel(CPlayer::EQUIP_WING,  pOldPlayer->GetEquipLevel(CPlayer::EQUIP_WING));
			pNewPlayer->SetEquipLevel(CPlayer::EQUIP_BODY, pOldPlayer->GetEquipLevel(CPlayer::EQUIP_BODY));
			pNewPlayer->SetEquipLevel(CPlayer::EQUIP_BACK, pOldPlayer->GetEquipLevel(CPlayer::EQUIP_BACK));
			pNewPlayer->SetEquipLevel(CPlayer::EQUIP_HAND, pOldPlayer->GetEquipLevel(CPlayer::EQUIP_HAND));
			pNewPlayer->SetEquipLevel(CPlayer::EQUIP_SHOULDER, pOldPlayer->GetEquipLevel(CPlayer::EQUIP_SHOULDER));

            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_WRIST, (BYTE)pOldPlayer->GetEquipIndex(CPlayer::EQUIP_WRIST));
            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_LEG,(BYTE) pOldPlayer->GetEquipIndex(CPlayer::EQUIP_LEG));
            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_BOOT, (BYTE) pOldPlayer->GetEquipIndex(CPlayer::EQUIP_BOOT));
            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_LRING, (BYTE)pOldPlayer->GetEquipIndex(CPlayer::EQUIP_LRING));
            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_RRING, (BYTE)pOldPlayer->GetEquipIndex(CPlayer::EQUIP_RRING));
            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_WEAPON, (BYTE)pOldPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON));
            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_CARD, (BYTE)pOldPlayer->GetEquipIndex(CPlayer::EQUIP_CARD));

            // -- 下边装备
            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_GODGOODS, (BYTE)pOldPlayer->GetEquipIndex(CPlayer::EQUIP_GODGOODS));
            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_WEAPON, (BYTE)pOldPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON));
            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_EFFIGY, (BYTE)pOldPlayer->GetEquipIndex(CPlayer::EQUIP_EFFIGY));
            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_LIFECOWRY, (BYTE)pOldPlayer->GetEquipIndex(CPlayer::EQUIP_LIFECOWRY));
            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_SOULGUARD, (BYTE)pOldPlayer->GetEquipIndex(CPlayer::EQUIP_SOULGUARD));
            pNewPlayer->SetEquipLevel(CPlayer::EQUIP_RIDER,(BYTE) pOldPlayer->GetEquipIndex(CPlayer::EQUIP_RIDER));

			if (pOldPlayer->IsRide())
			{
				pNewPlayer->SetIsRide(!pOldPlayer->IsRide());
				pNewPlayer->GetHorse()->Copy(pOldPlayer->GetHorse());
				pNewPlayer->GetHorse()->SetHorseType(pOldPlayer->GetHorse()->GetHorseType());
				pNewPlayer->GetHorse()->SetVisible(true);

				AnimInfo * pAnimInfo = pNewPlayer->GetHorse()->GetAnimInfo();
				pAnimInfo->DeleteTrailContainer();
				memcpy(pAnimInfo, pOldPlayer->GetHorse()->GetAnimInfo(), sizeof(AnimInfo));
				pAnimInfo->SetTrailContainerArray(NULL);
				pAnimInfo->EnableProcessSelfAnimJoint(false);

				pNewPlayer->GetHorse()->SetDisplayModel();
			}

			pNewPlayer->UpdateWeaponShow(CPlayer::EQUIP_WEAPON);
		//	pNewPlayer->UpdateWeaponShow(CPlayer::EQUIP_WEAPON2);
			pNewPlayer->UpdateWeaponShow(CPlayer::EQUIP_WING);

			// 设定时装秀
			pNewPlayer->SetShowFashion(pOldPlayer->GetShowFashion());
		}
		break;
	case TYPE_PET:
		{
			CPet *pPet = (CPet*)this;
			long lPetType = pPet->GetPetType();
			switch (lPetType)
			{
			case PET_TYPE_SERVANT:
				pOutObj = new CPet;
				break;
			}
			pOutObj->SetType(TYPE_PET);
		}
		break;
	case TYPE_MONSTER:
		{
			pOutObj = new CMonster();
			pOutObj->SetType(TYPE_MONSTER);
		}
		break;
	case TYPE_EFFECT:
		{
			pOutObj = new CEffect();
			pOutObj->SetType(TYPE_EFFECT);

			CEffect * pNewEffect = dynamic_cast<CEffect *>(pOutObj);
			CEffect * pOldEffect = dynamic_cast<CEffect *>(this);
			pNewEffect->SetHost(pOldEffect->GetHType(), pOldEffect->GetHostType(), pOldEffect->GetHostGraphicsID(), pOldEffect->GetHostID(), pOldEffect->GetHostLocator());
		}
		break;
	case TYPE_COLLECTION:
		{
			pOutObj = new CCollectionGoods();
			pOutObj->SetType(TYPE_COLLECTION);
		}
		break;
	}

	if (pOutObj == NULL)
		return false;

	// 基本数据
	pOutObj->SetGraphicsID(GetGraphicsID());
	pOutObj->SetName(GetName());
	pOutObj->SetPosX(GetPosX());
	pOutObj->SetPosY(GetPosY());
	pOutObj->SetHeight(GetHeight());
	pOutObj->SetNowDir(GetNowDir());

	// 渐变操作相关
	pOutObj->SetAlphaFadeInStartTime(GetAlphaFadeInStartTime());
	pOutObj->SetAlphaFadeInTimeLength(GetAlphaFadeInTimeLength());
	pOutObj->SetIsNeedAlphaFade(GetIsNeedAlphaFade());
	pOutObj->SetAlphaFadeMode(GetAlphaFadeMode());
	pOutObj->SetAlphaValue(GetAlphaValue());

	// 显示模型数据
	AnimInfo * pAnimInfo = pOutObj->GetAnimInfo();
	pAnimInfo->DeleteTrailContainer();
	memcpy(pAnimInfo, m_pAnimInfo, sizeof(AnimInfo));
	pAnimInfo->SetTrailContainerArray(NULL);
	pAnimInfo->EnableProcessSelfAnimJoint(false);

	pOutObj->SetDisplayModel(false);
	pOutObj->SetDisplayModelGroup();

	if (lType == TYPE_PLAYER)
	{
		// 设定时装秀
		CPlayer * pPlayer = dynamic_cast<CPlayer *>(pOutObj);
		pPlayer->SetManteauVisible(pPlayer->GetShowFashion() & FASHION_TYPE_MANTEAU);
		pPlayer->SetGreatcoatVisible(pPlayer->GetShowFashion() & FASHION_TYPE_GREATCOAT);
	}else if (lType == TYPE_COLLECTION)
	{
		CDisplayModel::CDisplayGroup * pGroup = (*GetDisplayModel()->GetGroups().find(MAKEFOURCC('0','0','0','0'))).second;
		if (pGroup != NULL)
			pOutObj->GetDisplayModel()->ShowGroup(MAKEFOURCC('0','0','0','0'), MAKEFOURCC('m','e','s','h'), pGroup->GetPartLevel(), false);
	}

	return true;
}

/*
* 功能: 拷贝模型信息
* 摘要: -
* 参数: pShape - 拷贝参考对象
* 返回值: -
* 作者: 刘科
* 创建日期: 2009.11.10
* 修改日志:
*/ 
void CShape::CopyModelInfo(CShape * pShape)
{
	if (pShape)
	{
		SetType(pShape->GetType());
		SetGraphicsID(pShape->GetGraphicsID());
		// 复制身上的装备信息
		CopyAllEquip(pShape);
		SetDisplayModel(false);
		SetDisplayModelGroup(false);
		SetState(STATE_FIGHT);
	}
}