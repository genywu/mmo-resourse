#include "stdafx.h"
#include "Shape.h"
#include "Effect.h"
#include "Player.h"
#include "ClientRegion.h"
#include "AlphaFadeObjectList.h"
#include "../../EngineExtend/TextureProjective.h"
#include "../GameClient/Game.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
* 功能: 构造函数
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2008.10.13
*/
CAlphaFadeObjectList::CAlphaFadeObjectList(void)
{
}


/*
* 功能: 析构函数
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2008.10.13
*/
CAlphaFadeObjectList::~CAlphaFadeObjectList(void)
{
	ReleaseAllObject();
}



/*
* 功能: 释放所有对象
* 摘要: 该函数请在注意调用,以释放资源
*		 在该类的析构函数中也调用了该函数
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2008.10.13
* 修改日志:
*/
void CAlphaFadeObjectList::ReleaseAllObject()
{
	vector<CShape *>::iterator it = m_vecObjectList.begin();
	while (it != m_vecObjectList.end())
	{
		SAFE_DELETE(*it);
		++it;
	}

	m_vecObjectList.clear();
}



/*
* 功能: 释放指定索引对象
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2008.10.13
* 修改日志:
*/
void CAlphaFadeObjectList::ReleaseObjectById(DWORD dwId)
{
	if ((int)dwId < 0 || dwId > m_vecObjectList.size())
		return;

	CShape * pShape = m_vecObjectList[dwId];
	SAFE_DELETE(pShape);

	m_vecObjectList.erase(m_vecObjectList.begin() + dwId);
}



/*
* 功能: 增加一个渐出操作对象
* 摘要: 通过该函数可以自动添加一个渐变对象模型
* 参数: pShape	- 对象指针
* 返回值: -
* 作者: lpf
* 创建日期: 2008.10.13
* 修改日志:
*	2009.05.07 - lpf
*		由于特效渐出效果不大,且容易造成莫名的当机,故禁止了特效渐出效果;
*	2009.05.21 - lpf
*		由于修改了一些模型底层错误,故现将特效渐出效果恢复;
*/
void CAlphaFadeObjectList::AddFadeOutObject(CShape * pShape)
{
	if (pShape->GetDisplayModel() == NULL || pShape->GetAnimInfo() == NULL/* ||
																		  pShape->GetType() == TYPE_EFFECT*/)
																		  return;

	CShape * pTmp = NULL;
	pShape->CopyShapeDisplayInfo(pTmp);
	if (pTmp == NULL)
		return;

	pTmp->SetAlphaFadeInStartTime(timeGetTime());
	pTmp->SetAlphaFadeInTimeLength(pShape->GetAlphaFadeInTimeLength());
	pTmp->SetAlphaFadeMode(ALPHA_FADE_OUT);
	pTmp->SetAlphaValue(1.0f);

	m_vecObjectList.push_back(pTmp);
}

/*
* 功能: 更新列表中的对象
* 摘要: 完成渐出操作的对象需要删除
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2008.10.13
* 修改日志:
*/
void CAlphaFadeObjectList::Update()
{
	long lElapseTime = 0;

	vector<CShape *>::iterator it = m_vecObjectList.begin();
	while (it != m_vecObjectList.end())
	{
		CShape * pTmp = (*it);
		lElapseTime = timeGetTime() - pTmp->GetAlphaFadeInStartTime();

		if (lElapseTime <= pTmp->GetAlphaFadeInTimeLength())
		{
			pTmp->SetAlphaValue((float)(pTmp->GetAlphaFadeInTimeLength() - lElapseTime) / (float)pTmp->GetAlphaFadeInTimeLength());
			++it;
		}else
		{
			SAFE_DELETE(pTmp);
			it = m_vecObjectList.erase(it);
		}
	}
}



/*
* 功能: 渲染列表中的对象
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2008.10.13
* 修改日志:
*	2008.10.21 - lpf
*		增加了在渲染特效时,对其进行纹理动画,可见动画,UV流动等等的处理;
*	2008.10.23 - lpf
*		增加了对坐骑显示的处理;
*	2008.10.31 - lpf
*		增加了处理渐出效果时,对投影纹理的渐出;
*	2008.12.12 - lpf
*		取消了渐出时的阴影渲染;
*	2008.12.30 - lpf
*		增加了对渐出模型列表中,显示模型是否读取完毕的判断,避免由此带来的当机问题;
*	2009.04.24 - lpf
*		玩家显示时,传入对象指针采用了dynamic_cast形式,检查错误的转换;
*/
void CAlphaFadeObjectList::Render()
{
	CClientRegion * pRegion = GetGame()->GetRegion();
	if (pRegion == NULL)
		return;

	for (size_t st = 0; st < m_vecObjectList.size(); ++st)
	{
		CShape * pTmp = m_vecObjectList[st];

		if (pTmp->GetDisplayModel() == NULL || pTmp->GetDisplayModel()->GetLoadState() != Load_Did)
			continue;
		pTmp->GetDisplayModel()->SetDirLightEnable(GetGame()->GetSetup()->lLight>0);
		pTmp->GetAnimInfo()->SetViewMatrix(pRegion->GetCamera()->GetViewMatrix());
		pTmp->GetAnimInfo()->SetProjectedMatrix(pRegion->GetCamera()->GetProjectionMatrix());

		switch (pTmp->GetType())
		{
		case TYPE_PLAYER:
			{
				CPlayer * pPlayer = (CPlayer *)pTmp;
				if (pPlayer->IsRide())
					pPlayer->GetHorse()->Display(dynamic_cast<CMoveShape *>(pTmp), false);

				pTmp->GetDisplayModel()->ProcessAnimJoint(pTmp->GetAnimInfo());
				pTmp->GetDisplayModel()->ProcessLocator(pTmp->GetAnimInfo());
				pTmp->GetDisplayModel()->MultiplyModelAlpha(pTmp->GetAlphaValue());
				pTmp->GetDisplayModel()->RenderModelTransparent();

				pPlayer->GetLeftHandWeapon()->Display(dynamic_cast<CShape *>(pTmp));
				pPlayer->GetRightHandWeapon()->Display(dynamic_cast<CShape *>(pTmp));
				pPlayer->GetWing()->Display(dynamic_cast<CShape *>(pTmp));

				//RenderShadow(pTmp);			//渲染阴影
			}
			break;
		case TYPE_PET:
		case TYPE_MONSTER:
		case TYPE_COLLECTION:
			{
				pTmp->GetDisplayModel()->ProcessAnimJoint(pTmp->GetAnimInfo());
				pTmp->GetDisplayModel()->ProcessLocator(pTmp->GetAnimInfo());
				pTmp->GetDisplayModel()->MultiplyModelAlpha(pTmp->GetAlphaValue());
				pTmp->GetDisplayModel()->RenderModelTransparent();
				//RenderShadow(pTmp);			//渲染阴影
			}
			break;
		case TYPE_EFFECT:
			{
				float fX = pTmp->GetPosX();
				float fY = pTmp->GetPosY();
				float fH = pTmp->GetHeight();
				float fNowDir = pTmp->GetNowDir();
				CEffect * pEffect = dynamic_cast<CEffect *>(pTmp);
				if (pEffect && pEffect->GetHostType() !=0 && pEffect->GetHostID() != CGUID::GUID_INVALID)
				{
					CShape * pShape = dynamic_cast<CShape *>(pRegion->FindChildObject(pEffect->GetHostType(), pEffect->GetHostID()));
					if (pShape)
					{
						pShape->GetLocatorPos(pEffect->GetHostLocator(), fX, fY, fH);
						fNowDir = pShape->GetNowDir();
					}
				}

				pTmp->GetAnimInfo()->SetupAnimInfo(fX, fY, fH, fNowDir, pRegion->GetCamera());
				pTmp->GetAnimInfo()->SetTextureProjectiveAlpha(pTmp->GetAlphaValue());
				pTmp->GetDisplayModel()->ProcessAnimJoint(pTmp->GetAnimInfo());
				pTmp->GetDisplayModel()->ProcessLocator(pTmp->GetAnimInfo());
				pTmp->GetDisplayModel()->ProcessVisibility(pTmp->GetAnimInfo());
				pTmp->GetDisplayModel()->ProcessAnimUV(pTmp->GetAnimInfo());
				pTmp->GetDisplayModel()->ProcessAnimTexture(pTmp->GetAnimInfo());
				pTmp->GetDisplayModel()->ProcessTextureProjective(pTmp->GetAnimInfo());
				pTmp->GetDisplayModel()->MultiplyModelAlpha(pTmp->GetAlphaValue());
				pTmp->GetDisplayModel()->RenderModelTransparent();
			}
			break;
		}
	}
}



/*
* 功能: 渲染指定模型阴影
* 摘要: -
* 参数: pShape	- 对象指针
* 返回值: -
* 作者: lpf
* 创建日期: 2008.10.13
* 修改日志:
*	2008.10.20 - lpf
*		对得到的绑定指针是否为空,进行了判断,防止切换场景时当机
*/
void CAlphaFadeObjectList::RenderShadow(CShape * pShape)
{
	D3DXVECTOR3 pos;

	pos.x = pShape->GetAnimInfo()->GetWorldMatrix()->_41;
	pos.y = pShape->GetHeight();
	pos.z = pShape->GetAnimInfo()->GetWorldMatrix()->_43;
	render::BoundingBox * pboundbox = pShape->GetDisplayModel()->GetSizeBox();
	if (pboundbox == NULL) return;
	float wid  = (max(pboundbox->GetWidth(),pboundbox->GetLength()));
	float hei  = pboundbox->GetHeight() * 3;
	float size = max(wid,hei) + 0.5f;

	if (size < 2.0f)
		size = 2.0f;

	if (pShape->GetDisplayModel()->GetRendShadow())
	{
		if (GetGame()->GetSetup()->lShadow == 2)
			CTextureProjective::SetSimpleShadow();
		else if (GetGame()->GetSetup()->lShadow == 0)
			CTextureProjective::SetSimpleShadow(true);

		CTextureProjective::BeginRendShadow(pos, (int)size);
		CTextureProjective::AddRendShadow(pShape->GetDisplayModel(),pShape->GetAnimInfo());
		CTextureProjective::EndRendShadow();
	}
}