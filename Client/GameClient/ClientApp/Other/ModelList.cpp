#include "StdAfx.h"
#include ".\modellist.h"
#include "..\..\gameExtend\GameModel.h"
#include "..\BaseObject.h"
#include "..\shape.h"
#include "..\player.h"
#include "..\npc.h"
#include "..\monster.h"
#include "..\Horse.h"
#include "..\goods.h"
#include "../Effect.h"
#include "..\clientregion.h"
#include "../GameExtend/GameGraphics2D.h"
#include "..\gameExtend\GameMap.h"
#include "effectsetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CModelList::CModelList(void)
{
}

CModelList::~CModelList(void)
{
	ReleaseAll();
}

CModelList::tagModel* CModelList::Load(long lType, long lGraphicsID, bool bFind)
{
	// 要求查找以有的图像资源
	if( bFind == true )
	{
		tagModel* pModel = GetModel( lType, lGraphicsID );
		if( pModel )	// 查找到图像
		{
			pModel->lUseCount++;	// 使用数加1
			return pModel;
		}
	}

	// 从文件读取图像数据
	tagModel* pModel = new tagModel;
	pModel->lUseCount = 1;

	char strbuf[MAX_PATH];
	switch(lType)
	{
	case TYPE_PLAYER:
		sprintf(strbuf,	"model/player/%03d/model", lGraphicsID);
		break;

	case TYPE_MONSTER:
		sprintf(strbuf,	"model/monster/%03d/model", lGraphicsID);
		break;

	/*case TYPE_NPC:
		sprintf(strbuf,	"model/npc/%03d/model", lGraphicsID);
		break;*/

	case TYPE_GOODS:
		sprintf(strbuf,	"model/goods/%03d/model", lGraphicsID);
		break;

	case TYPE_EFFECT:
		sprintf(strbuf,	"model/effect/%03d/model", lGraphicsID);
		break;

	case TYPE_SKILL:
		sprintf(strbuf,	"model/skill/%03d/model", lGraphicsID);
		break;
	case TYPE_BUILD:
	case TYPE_CITYGATE:
		sprintf(strbuf,	"model/Construction/%03d/model", lGraphicsID);
		break;

	default:
		SAFE_DELETE( pModel );
		return NULL;
	}

	if(!GetGame()->IsFileExist(strbuf))
	{
		sprintf(strbuf,"%s","model/default/model");
		OutputError("Model ID : %d %d 不存在\n",lType,lGraphicsID);
	}
	pModel->pModel = new GameModel;
	pModel->pModel->Create(strbuf,(DWORD)GetGame()->GetSetup()->lTexture);

	tagModelKey key ={lType,lGraphicsID};
	m_Models[key] = pModel;
	return pModel;
}

BOOL CModelList::Release(long lType, long lGraphicsID)
{
	// 玩家资源不释放
	if (lType==TYPE_PLAYER)
		return FALSE;
	tagModelKey key = {lType,lGraphicsID};
	// 查找是否有该资源
	itModel it = m_Models.find(key);
	if(it != m_Models.end())
	{
		tagModel *pModel = (*it).second;
		if(pModel && --pModel->lUseCount == 0)
		{
			pModel->pModel->Destroy();
			SAFEDESTROY( pModel->pModel );
			SAFE_DELETE( pModel );
			m_Models.erase( it );
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CModelList::ReleaseAll(void)
{
	tagModel* pModel = NULL;
	// 查找是否有该资源
	for(itModel it = m_Models.begin(); it != m_Models.end(); it++ )
	{
		pModel = (*it).second;
		SAFEDESTROY( pModel->pModel );
		SAFE_DELETE( pModel );
	}
	m_Models.clear();

	return TRUE;
}
void CModelList::DisplaySpecialEquipment(CShape* pShape,render::Camera * pCamera)
{
	CPlayer *pPlayer = (CPlayer *)pShape;
	CModelList::tagModel *p = GetGame()->GetModelList()->GetModel(pShape->GetType(),pShape->GetGraphicsID());
	CWeapon* prighthandweapon = pPlayer->GetRightHandWeapon();
	CWeapon* plefthandweapon = pPlayer->GetLeftHandWeapon();
	CWing*   pwing = pPlayer->GetWing();
	GameModel::Locator *pLocator(NULL);

	CClientRegion *pClientRegion = GetGame()->GetRegion();

	if (pClientRegion)
	{
		GameMap *pGameMap = pClientRegion->GetGameMap();
		//翅膀
		if (pwing->IsVisible())
		{
			pLocator= p->pModel->GetLocator(CShape::LOCATOR_BACK);
			pwing->SetLocator(pLocator);
			pwing->Render(pCamera,pGameMap,!pPlayer->GetIsConceal());
			//D3DXMatrixTranslation(&matWorld,vLocator.x,vLocator.y,vLocator.z);


		}
		//右手武器
		if (prighthandweapon->IsVisible())
		{
			if (pPlayer->GetState() == CPlayer::STATE_PEACE)
			{
				switch(prighthandweapon->GetWeaponType())
				{
				case CWeapon::WEAPONSHOW_SINGLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTWAIST);
					break;
				case CWeapon::WEAPONSHOW_BIGSTAFF:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_BACKSTAFF);
					break;
				case CWeapon::WEAPONSHOW_BIGSWORD:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_BIGSWORD);
					break;
				case CWeapon::WEAPONSHOW_BOXING:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTWAIST);
					break;
				case CWeapon::WEAPONSHOW_DOUBLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTBACK);
					break;

				}


			}
			if (pPlayer->GetState() == CPlayer::STATE_FIGHT)
			{
				switch(prighthandweapon->GetWeaponType())
				{
				case CWeapon::WEAPONSHOW_SINGLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTHAND);
					break;
				case CWeapon::WEAPONSHOW_BIGSTAFF:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTHAND);
					break;
				case CWeapon::WEAPONSHOW_BIGSWORD:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTHAND);
					break;

				case CWeapon::WEAPONSHOW_DOUBLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTHAND);
					break;
				case CWeapon::WEAPONSHOW_BOXING:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTARM);
					break;


				}

			}
			prighthandweapon->SetLocator(pLocator);
			prighthandweapon->Render(pCamera,pGameMap,!pPlayer->GetIsConceal());

			//D3DXMatrixTranslation(&matWorld,vLocator.x,vLocator.y,vLocator.z);


		}
		//左手武器
		if (plefthandweapon->IsVisible())
		{


			if (pPlayer->GetState() == CPlayer::STATE_PEACE)
			{
				switch(plefthandweapon->GetWeaponType())
				{
				case CWeapon::WEAPONSHOW_SINGLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTWAIST);
					break;
				case CWeapon::WEAPONSHOW_BOXING:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTWAIST);
					break;
				case CWeapon::WEAPONSHOW_DOUBLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTBACK);
					break;
				case CWeapon::WEAPONSHOW_SHIELD:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTWAIST);
					break;
				case CWeapon::WEAPONSHOW_BOW:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_BACKBOW);
					break;

				}

			}
			if (pPlayer->GetState() == CPlayer::STATE_FIGHT)
			{
				switch(plefthandweapon->GetWeaponType())
				{
				case CWeapon::WEAPONSHOW_SINGLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTHAND);
					break;
				case CWeapon::WEAPONSHOW_BOXING:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTBOXING);
					break;
				case CWeapon::WEAPONSHOW_DOUBLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTHAND);
					break;
				case CWeapon::WEAPONSHOW_SHIELD:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTARM);
					break;
				case CWeapon::WEAPONSHOW_BOW:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTHAND);
					break;

				}

			}
			plefthandweapon->SetLocator(pLocator);

			plefthandweapon->Render(pCamera,pGameMap,!pPlayer->GetIsConceal());



			//D3DXMatrixTranslation(&matWorld,vLocator.x,vLocator.y,vLocator.z);

		}

	}

}
void CModelList::UpdateSpecialEquipment(CShape* pShape)
{
	CPlayer *pPlayer = (CPlayer *)pShape;
	CModelList::tagModel *p = GetGame()->GetModelList()->GetModel(pShape->GetType(),pShape->GetGraphicsID());
	CWeapon* prighthandweapon = pPlayer->GetRightHandWeapon();
	CWeapon* plefthandweapon = pPlayer->GetLeftHandWeapon();
	CWing*   pwing = pPlayer->GetWing();
	GameModel::Locator *pLocator(NULL);

	CClientRegion *pClientRegion = GetGame()->GetRegion();

	if (pClientRegion)
	{
		GameMap *pGameMap = pClientRegion->GetGameMap();
		//翅膀
		if (pwing->IsVisible())
		{
			pLocator= p->pModel->GetLocator(CShape::LOCATOR_BACK);
			pwing->SetLocator(pLocator);
			//pwing->Render(pCamera,pGameMap,!pPlayer->GetIsConceal());
			//D3DXMatrixTranslation(&matWorld,vLocator.x,vLocator.y,vLocator.z);


		}
		//右手武器
		if (prighthandweapon->IsVisible())
		{
			if (pPlayer->GetState() == CPlayer::STATE_PEACE)
			{
				switch(prighthandweapon->GetWeaponType())
				{
				case CWeapon::WEAPONSHOW_SINGLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTWAIST);
					break;
				case CWeapon::WEAPONSHOW_BIGSTAFF:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_BACKSTAFF);
					break;
				case CWeapon::WEAPONSHOW_BIGSWORD:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_BIGSWORD);
					break;
				case CWeapon::WEAPONSHOW_BOXING:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTWAIST);
					break;
				case CWeapon::WEAPONSHOW_DOUBLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTBACK);
					break;

				}


			}
			if (pPlayer->GetState() == CPlayer::STATE_FIGHT)
			{
				switch(prighthandweapon->GetWeaponType())
				{
				case CWeapon::WEAPONSHOW_SINGLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTHAND);
					break;
				case CWeapon::WEAPONSHOW_BIGSTAFF:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTHAND);
					break;
				case CWeapon::WEAPONSHOW_BIGSWORD:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTHAND);
					break;

				case CWeapon::WEAPONSHOW_DOUBLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTHAND);
					break;
				case CWeapon::WEAPONSHOW_BOXING:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_RIGHTARM);
					break;


				}

			}
			prighthandweapon->SetLocator(pLocator);
			//prighthandweapon->Render(pCamera,pGameMap,!pPlayer->GetIsConceal());

			//D3DXMatrixTranslation(&matWorld,vLocator.x,vLocator.y,vLocator.z);


		}
		//左手武器
		if (plefthandweapon->IsVisible())
		{


			if (pPlayer->GetState() == CPlayer::STATE_PEACE)
			{
				switch(plefthandweapon->GetWeaponType())
				{
				case CWeapon::WEAPONSHOW_SINGLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTWAIST);
					break;
				case CWeapon::WEAPONSHOW_BOXING:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTWAIST);
					break;
				case CWeapon::WEAPONSHOW_DOUBLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTBACK);
					break;
				case CWeapon::WEAPONSHOW_SHIELD:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTWAIST);
					break;
				case CWeapon::WEAPONSHOW_BOW:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_BACKBOW);
					break;

				}

			}
			if (pPlayer->GetState() == CPlayer::STATE_FIGHT)
			{
				switch(plefthandweapon->GetWeaponType())
				{
				case CWeapon::WEAPONSHOW_SINGLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTHAND);
					break;
				case CWeapon::WEAPONSHOW_BOXING:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTBOXING);
					break;
				case CWeapon::WEAPONSHOW_DOUBLEWEAPON:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTHAND);
					break;
				case CWeapon::WEAPONSHOW_SHIELD:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTARM);
					break;
				case CWeapon::WEAPONSHOW_BOW:
					pLocator= p->pModel->GetLocator(CShape::LOCATOR_LEFTHAND);
					break;

				}

			}
			plefthandweapon->SetLocator(pLocator);

			//plefthandweapon->Render(pCamera,pGameMap,!pPlayer->GetIsConceal());



			//D3DXMatrixTranslation(&matWorld,vLocator.x,vLocator.y,vLocator.z);

		}

	}

}
void CModelList::Display(CShape *pShape, float x, float y,float fHeight,render::Camera * pCamera)
{
	if (pShape == NULL)
		return;
	/*long lType = pShape->GetType();
	long lGraphicsID = pShape->GetGraphicsID();*/
	long lType,lGraphicsID;
	pShape->GetCurGraphicTypeID(lType,lGraphicsID);
	
	WORD wAct = pShape->GetAction();

	tagModel* pModel = GetModel( lType, lGraphicsID );

	


	if( pModel == NULL )
	{
		pModel = Load(lType, lGraphicsID, false);
		if(pModel==NULL)
			return;
	}
	GameModel *pGameModel = pModel->pModel;
	if( pGameModel)
	{
		// 如果是跟着定位点的特效更新位置
		if (lType == TYPE_EFFECT)
		{
			CEffect* pEffect = dynamic_cast<CEffect*>(pShape);

			// 有定位点
			if (pEffect && pEffect->GetHostType())
			{
				CClientRegion *pClientRegion = GetGame()->GetRegion();
				if (pClientRegion)
				{
					CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pClientRegion->FindChildObject(pEffect->GetHType(), pEffect->GetHostID()));
					if (pMoveShape)
					{
						pMoveShape->GetLocatorPos(pEffect->GetHostLocator(),x, y, fHeight);
						pEffect->SetPosXY(x, y);
						pEffect->SetHeight(fHeight);
					}
				}
			}
		}

		// 计算是否在视野范围内


		D3DXMATRIX matWorld;
		D3DXMatrixTranslation(&matWorld,y,fHeight,x);
		//骑马
		if (lType == TYPE_PLAYER)
		{
			CClientRegion *pClientRegion = GetGame()->GetRegion();
			if (pClientRegion)
			{
				CPlayer *pPlayer = (CPlayer *)pShape;
				CHorse *pHorse = pPlayer->GetHorse();
				if (pHorse->IsVisible())
				{
					GameMap *pGameMap = pClientRegion->GetGameMap();
					D3DXVECTOR3 vLocator;

					D3DXMATRIX matRot;
					D3DXMatrixRotationY(&matRot,pShape->GetNowDir());
					D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);
					pHorse->Render(&matWorld,pCamera,pGameMap,&vLocator);
					fHeight = vLocator.y;
					x = vLocator.z;
					y = vLocator.x;
					//D3DXMatrixTranslation(&matWorld,vLocator.x,vLocator.y,vLocator.z);
				}
			}
		}

		CClientRegion* pRegion = (CClientRegion*)pShape->GetFather();
		if (pRegion && pRegion->GetFrustum()->BoxIntersectFrustum(&matWorld,pModel->pModel->GetSizeBox()) == false)
		{
			pShape->SetInViewRange(false);
			return ;
		}
		else
		{
			pShape->SetInViewRange(true);
		}

		if (pShape->GetType() == TYPE_PLAYER)
		{
			//显示刀光
			CPlayer *pPlayer = (CPlayer*)pShape;
			int lWeapon = 0, lWeaponActType = 0;

			CGoods* pEquip = NULL;
			if (pPlayer==GetGame()->GetMainPlayer())	// 自己
			{
				pEquip = pPlayer->GetEquip(CPlayer::EQUIP_WEAPON);
				if (pEquip)
				{
					lWeapon = pEquip->GetProperty()->dwEquipID;
					lWeaponActType = pEquip->GetProperty()->dwWeaponActType;
				}
			}
			else	// 其他玩家
			{
				DWORD dwIndex = pPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON);
				CGoodsList::tagGoods2 *pst = CGoodsList::GetProperty(dwIndex);
				if (pst)
				{
					lWeapon = pst->BaseProperty.dwEquipID;
					lWeaponActType = pst->BaseProperty.dwWeaponActType;
				}
			}
			pGameModel->EnableTrailEmitter(FALSE);
			if (lWeapon)
			{
				CGame::tagRenderTrail* pRenderTrail = GetGame()->GetRenderTrail(pPlayer->GetOccupation(), wAct, lWeaponActType);
				if (pRenderTrail)
				{
					//刀光设置
					pGameModel->EnableTrailEmitter(pRenderTrail->dwTrailName,TRUE);
				}
			}

			//脚印
			CClientRegion *pRegion = GetGame()->GetRegion();
			if (pRegion)
			{
				GameMap *pMap = pRegion->GetGameMap();
				if (pMap)
				{
					static const int angle[] = {-90,-45,0,45,90,135,180,225};
					static D3DXVECTOR3 pos0[6] = { D3DXVECTOR3(-0.5f,0.0f,0.5f),
						D3DXVECTOR3(0.5f,0.0f,0.5f),
						D3DXVECTOR3(0.5f,0.0f,-0.5f),
						D3DXVECTOR3(-0.5f,0.0f,0.5f),
						D3DXVECTOR3(0.5f,0.0f,-0.5f),
						D3DXVECTOR3(-0.5f,0.0f,-0.5f) };	
					static D3DXVECTOR3 vIntersection,vNormal;

					CClientRegion::tagCell *pCell = pRegion->GetCell(pShape->GetTileX(),pShape->GetTileY());
					if (pShape->GetAction() == CShape::ACT_WALK || pShape->GetAction() == CShape::ACT_RUN)
					{
						GameFootprint *pGameFootprint = pMap->GetFootprint();
						DWORD dwLocName[] ={ MAKEFOURCC('0','0','0','5'),MAKEFOURCC('0','0','0','6')};
						GameModel::Locator *pLocate(NULL);
						DWORD dwCAST = pShape->GetActionTimer()->GetActionInfo()->dwCurActionStartTime;
						// 取得并更新当前玩家模型数据
						SetupAnimInfo(pGameModel, pShape, x, y,fHeight, pCamera->GetBillboard(),pCamera->GetBillboardY());
						pGameModel->ProcessLocator(pShape->GetActionTimer());

						for(DWORD n = 0; n < 2 ; n++)
						{
							pLocate = pGameModel->GetLocator(dwLocName[n]); 
							if (pLocate)
							{
								model::Buffer *pBuffer = pLocate->GetBuffer();
								D3DXVECTOR3   *pPosBuffer = pBuffer->GetPosBuffer();
								if (pShape->GetFMFTime(n) != dwCAST )
								{
									if (pMap->HitTestGrid(pPosBuffer,&vIntersection,&vNormal) )
									{
										if (!(pCell->lMaterial == CRegion::CM_GRASS || 
											pCell->lMaterial == CRegion::GM_WOOD || 
											pCell->lMaterial == CRegion::GM_STONE))
										{
											GameFootprint::tagFootprint *pFootprint = pGameFootprint->NewFootprint();
											pFootprint->dwLifeTimeLength = 1500;
											pFootprint->dwLifeTimeStart = timeGetTime();

											//****************************************************************
											//decal修改
											D3DXVECTOR3 tangent[8] = {//初始化方向
												D3DXVECTOR3(0.0f,		0.0f,		1.0f),
												D3DXVECTOR3(1.0f,		0.0f,		1.0f),
												D3DXVECTOR3(1.0f,		0.0f,		0.0f),
												D3DXVECTOR3(1.0f,		0.0f,		-1.0f),
												D3DXVECTOR3(0.0f,		0.0f,		-1.0f),
												D3DXVECTOR3(-1.0f,	0.0f,		-1.0f),
												D3DXVECTOR3(-1.0f,	0.0f,		0.0f),
												D3DXVECTOR3(-1.0f,	0.0f,		1.0f)
											};
											D3DXVec3Normalize(&tangent[1], &tangent[1]);
											D3DXVec3Normalize(&tangent[3], &tangent[3]);
											D3DXVec3Normalize(&tangent[5], &tangent[5]);
											D3DXVec3Normalize(&tangent[7], &tangent[7]);//将方向归一化

											//初始化进行计算的地形三角形数组
											int taIndex = 0;
											for (int z=(int)vIntersection.z-5;z<(int)vIntersection.z+5;z++)
											{
												if (z>0&&z<pMap->GetDepth())
												{
													for (int x=(int)vIntersection.x-5;x<(int)vIntersection.x+5;x++)
													{
														if (x>0&&x<pMap->GetWidth())
														{
															pMap->triangleArray[taIndex].index[0] = pMap->GetGrid(x, z)->dwVertexIndex[0];
															pMap->triangleArray[taIndex].index[1] = pMap->GetGrid(x, z)->dwVertexIndex[3];
															pMap->triangleArray[taIndex].index[2] = pMap->GetGrid(x, z)->dwVertexIndex[2];
															taIndex++;
															pMap->triangleArray[taIndex].index[0] = pMap->GetGrid(x, z)->dwVertexIndex[0];
															pMap->triangleArray[taIndex].index[1] = pMap->GetGrid(x, z)->dwVertexIndex[2];
															pMap->triangleArray[taIndex].index[2] = pMap->GetGrid(x, z)->dwVertexIndex[1];
															taIndex++;
														}
													}
												}

											}

											//初始化decal
											pGameFootprint->InitDecal(vIntersection, vNormal, tangent[pShape->GetDir()], 1, 1, 1,
												taIndex, pMap->triangleArray, 
												pMap->m_pVertexPosArray, pMap->m_pVertexNormalArray);

											pFootprint->dwCount = pGameFootprint->decalTriangleCount;
											int va2index = 0;
											for (int i=0; i<pFootprint->dwCount; i++)
											{
												pFootprint->pos[va2index] = pGameFootprint->vertexArray[pGameFootprint->triangleArray[i].index[0]];	pFootprint->m_pUV0[va2index] = pGameFootprint->texcoordArray[pGameFootprint->triangleArray[i].index[0]];
												pFootprint->pos[va2index].y+=0.04f; va2index++;
												pFootprint->pos[va2index] = pGameFootprint->vertexArray[pGameFootprint->triangleArray[i].index[1]];	pFootprint->m_pUV0[va2index] = pGameFootprint->texcoordArray[pGameFootprint->triangleArray[i].index[1]];
												pFootprint->pos[va2index].y+=0.04f; va2index++;
												pFootprint->pos[va2index] = pGameFootprint->vertexArray[pGameFootprint->triangleArray[i].index[2]];	pFootprint->m_pUV0[va2index] = pGameFootprint->texcoordArray[pGameFootprint->triangleArray[i].index[2]];
												pFootprint->pos[va2index].y+=0.04f; va2index++;
											}

											//D3DXMATRIX matWorld,matRotD,matTrs;
											//D3DXMatrixTranslation(&matTrs,vIntersection.x,vIntersection.y + 0.04f,vIntersection.z);
											//D3DXMatrixRotationY(&matRotD,D3DXToRadian(angle[pShape->GetDir()]));
											//D3DXMatrixMultiply(&matWorld,&matRotD,&matTrs);
											//原始代码
											//for(int i =0; i < 6; i++)
											//{
											//	D3DXVec3TransformCoord(&pFootprint->pos[i],&pos0[i],&matWorld);
											//}
											pShape->SetFMFTime(n,dwCAST);
										}

										//添加其他功能

										// 播放行走的声音
										// soft 2004-11-24
										//GetGame()->GetAudioList()->Play(pShape,TYPE_REGION,pShape->GetAction(),Vector3(pShape->GetPosY(), pShape->GetHeight(), pShape->GetPosX()) );
									}
								}	
							}
						}
					}
				}
			}
		}

		//特效光照
		if (pShape->GetType() == TYPE_EFFECT || pShape->GetType() == TYPE_SUMMON)
		{
			if(pShape->GetType()==TYPE_EFFECT)
			{
				CEffect* pEffect = dynamic_cast<CEffect*>(pShape);
				if (pEffect->IsLightAvailable())
				{
					D3DXVECTOR3 vLightPosition = D3DXVECTOR3(pShape->GetPosY(),pShape->GetHeight(),pShape->GetPosX());
					GetGame()->GetRegion()->GetGameMap()->AddPointLightSource(&vLightPosition,pEffect->GetLightType(),pEffect->GetLightRadius()
						,pEffect->GetLightColor());
				}
			}
		}

		//设置换装
		SetupEquipment(pGameModel, pShape);

		// 更新模型
		SetupAnimInfo(pGameModel, pShape, x, y,fHeight, pCamera->GetBillboard(),pCamera->GetBillboardY());

		// 光照
		static D3DXVECTOR3 vLightVector(-0.613793f,-0.789465f,0.001831f);

		GameModel::AnimInfo *pAnimInfo = pShape->GetActionTimer();
		pGameModel->ProcessAnimJoint(pAnimInfo);
		pGameModel->ProcessAnimTexture(pAnimInfo);
		pGameModel->ProcessAnimUV(pAnimInfo);
		pGameModel->ProcessParticleEmitter(pAnimInfo);
		pGameModel->ProcessTrailEmitter(pAnimInfo);
		pGameModel->ProcessEnvironmentMaps(pCamera->GetViewMatrix());
		pGameModel->ProcessLocator(pAnimInfo);
		//翅膀,武器
		if (lType == TYPE_PLAYER)
		{
			
			DisplaySpecialEquipment(pShape,pCamera);
			
		}



		if (lType == TYPE_EFFECT)
		{
			pGameModel->SetModelColor(0xFFFFFFFF);
		}
		else
		{
			if (GetGame()->GetSetup()->lLight)
			{
				pGameModel->SetModelColor(0xFF606060);
				pGameModel->ProcessDirectionalLight(pCamera->GetPosition(),(D3DXVECTOR3*)&vLightVector,0xFF808080);
				if (lType == TYPE_PLAYER || /*lType == TYPE_NPC ||*/ lType == TYPE_MONSTER || lType == TYPE_GOODS)//地表颜色
				{
					CClientRegion *pClientRegion = GetGame()->GetRegion();
					if (pClientRegion)
					{
						GameMap *pGameMap = pClientRegion->GetGameMap();
						DWORD dwColor = pGameMap->GetGridColor(pShape->GetTileY(),pShape->GetTileX());
						//颜色增加1.25倍
						DWORD r = ( (dwColor & 0x00FF0000) >> 16) * 20 / 15 + 0x000000a0;
						if (r > 255 )
							r = 255;
						DWORD g = ( (dwColor & 0x0000FF00) >>  8) * 20 / 15 + 0x000000a0;
						if (g > 255) 
							g = 255;
						DWORD b = (  dwColor & 0x000000FF) * 20 / 15 + 0x000000a0;
						if (b > 255)
							b = 255;
						dwColor = r << 16 | g << 8 | b;
						pGameModel->MultiplyModelColor(dwColor);
					}
				}
			}
			else
				pGameModel->SetModelColor(0xFFFFFFFF);

			pGameModel->MultiplyModelColor(pShape->GetModelColor());
		}
		pGameModel->ProcessAnimColor(pAnimInfo);	

		//选中的颜色叠加
		/*if (lType != TYPE_EFFECT)
		{
		if (GetGame()->GetRegion() && pShape==GetGame()->GetGameControl()->GetMouseShape())
		{
		pGameModel->MultiplyModelColor(CGlobeSetup::GetSetup()->colorMouseShape);
		}
		}*/

		pGameModel->ProcessVisibility(pAnimInfo);
		if (lType == TYPE_MONSTER)
		{	
			//FADE IN 怪物死亡时的淡出状态
			CMonster *pMonster = (CMonster *)pShape;
			if (pMonster->IsFade())
			{
				pGameModel->MultiplyModelAlpha(pMonster->GetFadeRatio());
				pGameModel->RenderModelTransparent();
			}
			else if(pMonster->GetIsConceal() == false)		// 怪物有隐身状态
			{
				pGameModel->MultiplyModelAlpha(0.1f);
				pGameModel->RenderModelTransparent();
			}
			else	// 正常显示
				pGameModel->RenderModel();
		}
		else if(lType == TYPE_PLAYER)
		{
			CPlayer *pPlayer = (CPlayer *)pShape;
			if(pPlayer->GetIsConceal() == false)				// 玩家有隐身状态
			{
				pGameModel->MultiplyModelAlpha(0.1f);
				pGameModel->RenderModelTransparent();
			}
			else		// 正常显示
				pGameModel->RenderModel();
		}
		else	// 正常显示
			pGameModel->RenderModel();




		if (g_bDebug)
		{
			if (GetKeyboard()->IsKeyDown(DIK_RSHIFT))
			{
				//显示选择盒子
				pGameModel->_DEBUG_RenderPickBox(pAnimInfo->GetWorldMatrix(),0xFF00FF00);
			}
		}
	}

	return ;
}

void CModelList::DisplayAsShadow(CShape *pShape, float x, float y,float fHeight,const D3DXMATRIX *pBillboardMatrix,const D3DXMATRIX * pBillboardMatrixY,DWORD dwColor)
{
	long lType = pShape->GetType();
	long lGraphicsID = pShape->GetGraphicsID();

	tagModel* pModel = GetModel( lType, lGraphicsID );
	if( pModel == NULL )
	{
		pModel = Load(lType, lGraphicsID, false);
	}
	if(pModel==0)return;

	GameModel *pGameModel = pModel->pModel;
	if( pGameModel)
	{
		// 计算是否在视野范围内
		D3DXMATRIX matWorld;
		D3DXMatrixTranslation(&matWorld,y,fHeight,x);
		CClientRegion* pRegion = (CClientRegion*)pShape->GetFather();
		if (pRegion && pRegion->GetFrustum()->BoxIntersectFrustum(&matWorld,pModel->pModel->GetSizeBox()) == false)
		{
			pShape->SetInViewRange(false);
			return ;
		}
		else
		{
			pShape->SetInViewRange(true);
		}

		//SetupEquipment(pGameModel, pShape);
		pGameModel->SelectDefaultPart();
		SetupAnimInfo(pGameModel, pShape, x, y,fHeight, pBillboardMatrix, pBillboardMatrixY);

		GameModel::AnimInfo *pAnimInfo = pShape->GetActionTimer();
		pGameModel->ProcessAnimJoint(pAnimInfo);
		pGameModel->ProcessAnimTexture(pAnimInfo);
		pGameModel->ProcessAnimUV(pAnimInfo);
		pGameModel->ProcessParticleEmitter(pAnimInfo);
		pGameModel->ProcessTrailEmitter(pAnimInfo);
		pGameModel->ProcessLocator(pAnimInfo);
		pGameModel->SetModelColor(dwColor);
		pGameModel->ProcessVisibility(pAnimInfo);
		pGameModel->RenderModelBlend();
	}
}

// 处理模型
BOOL CModelList::SetupAnimInfo(GameModel* pGameModel, CShape* pShape, float x, float y,float fHeight,const D3DXMATRIX *pBillboardMatrix,const D3DXMATRIX * pBillboardMatrixY)
{
	float fDir = pShape->GetNowDir();

	//计算
	D3DXMATRIX matWorld;
	D3DXMatrixTranslation(&matWorld,y,fHeight,x);
	D3DXMATRIX matRot;
	D3DXMatrixRotationY(&matRot,fDir);
	D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);

	long newtime = timeGetTime();

	GameModel::AnimInfo *pAnimInfo = pShape->GetActionTimer();
	GameModel::AnimInfo::tagActionInfo *pActionInfo = pAnimInfo->GetActionInfo();
	pAnimInfo->SetCurrentTime(newtime);

	pAnimInfo->SetWorldMatrix(&matWorld);
	pAnimInfo->SetBillboard(pBillboardMatrix);
	pAnimInfo->SetBillboardY(pBillboardMatrixY);

	return TRUE;
}

// 获取定位点坐标
BOOL CModelList::GetLocatorPos(CShape* pShape, DWORD dwLocatorName, const D3DXMATRIX *pBillboardMatrix,const D3DXMATRIX * pBillboardMatrixY, float &x, float &y, float &fHeight)
{
	long lType,lGraphicsID;
	pShape->GetCurGraphicTypeID(lType,lGraphicsID);
	tagModel* pModel = GetModel( lType, lGraphicsID );
	if( pModel == NULL )
	{
		pModel = Load(lType, lGraphicsID, false);
	}

	GameModel *pGameModel = pModel->pModel;
	if( pGameModel)
	{
		SetupAnimInfo(pGameModel, pShape, pShape->GetPosX(), pShape->GetPosY(), pShape->GetHeight(), pBillboardMatrix, pBillboardMatrixY);
		pGameModel->ProcessLocator(pShape->GetActionTimer());

		GameModel::Locator *pLocator = pGameModel->GetLocator(dwLocatorName);
		if (pLocator)
		{
			model::Buffer *pBuffer = pLocator->GetBuffer();
			D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();
			x = pPosBuffer[0].z;
			y = pPosBuffer[0].x;
			fHeight = pPosBuffer[0].y;
			return true;
		}
	}

	x = pShape->GetPosX();
	y = pShape->GetPosY();
	fHeight = pShape->GetHeight();
	return false;
}

// 获取
CModelList::tagModel* CModelList::GetModel(long lType, long lGraphicsID)
{
	tagModel* pModel = NULL;

	tagModelKey key = {lType,lGraphicsID};
	// 查找是否有该资源
	itModel it = m_Models.find(key);
	if(it != m_Models.end())
	{
		return (*it).second;
	}
	return NULL;
}

void CModelList::DisplayIcon(long lType, long lGraphicsID, int iLevel,int iDrawX, int iDrawY, float fRotation,bool bAction,GameModel::AnimInfo *pAnimInfo,float fScale)
{
	//画三维装备
	tagModel *pModel = GetModel(lType, lGraphicsID);
	if (!pModel)
		pModel = Load(lType, lGraphicsID);

	if (pModel)
	{
		GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();

		pModel->pModel->SetGroupVisible(TRUE);
		pModel->pModel->SelectDefaultPart();

		if (pAnimInfo)
		{
			GameModel::AnimInfo::tagActionInfo *pActionInfo = pAnimInfo->GetActionInfo();
			pActionInfo->dwCurAction = MAKEFOURCC('i', 't', 'e', 'm');
			pActionInfo->bInterpolation = FALSE;
			pActionInfo->bCurActionLooped = TRUE;
			if (!bAction)
			{
				pActionInfo->dwCurActionStartTime = timeGetTime();
			}
		}
		pModel->pModel->SetGroupLevel(iLevel);
		pGameGraphics2D->DrawGameModel(iDrawX,iDrawY,pModel->pModel ,fScale,fRotation,pAnimInfo);
	}
}

void CModelList::DisplayIcon(CShape *pShape,int iDrawX, int iDrawY, float fRotation)
{
	long lType = pShape->GetType();
	long lGraphicsID = pShape->GetGraphicsID();			 

	tagModel* pModel = GetModel( lType, lGraphicsID );
	if( pModel == NULL )
	{
		pModel = Load(lType, lGraphicsID, false);
	}

	if (pModel)
	{
		DWORD oldstate = 0;
		DWORD oldaction = 0;
		SetupEquipment(pModel->pModel, pShape);
		GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();

		//******ADD BY 彭谦*******//
		if (lType == TYPE_PLAYER)
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pShape);
			//pPlayer->GetRightHandWeapon()->SetLevel(2);
			//pPlayer->GetRightHandWeapon()->ChangeStatus(CWeapon::WEAPON_ACTION1);
			static CPlayer Player;
			Player.SetType(pPlayer->GetType());
			Player.SetGraphicsID(pPlayer->GetGraphicsID());
			//extern bool g_bForceUpdate;
			//g_bForceUpdate = true;
			Player.SetState(CShape::STATE_FIGHT);
			Player.SetAction(CShape::ACT_STAND);
			Player.GetWing()->Copy(pPlayer->GetWing());
			Player.GetLeftHandWeapon()->Copy(pPlayer->GetLeftHandWeapon());
			Player.GetRightHandWeapon()->Copy(pPlayer->GetRightHandWeapon());
			GameModel::AnimInfo* aninfo = Player.GetActionTimer();
			//g_bForceUpdate = false;
			UpdateSpecialEquipment(&Player);
			static GameModel model = *(pModel->pModel);
			model.ProcessLocator(aninfo);
			static D3DXMATRIX matmodelworld,matworld;
			CClientRegion *pRegion = GetGame()->GetRegion();
			render::Camera *pCamera = pRegion->GetCamera();
			pGameGraphics2D->SetUIModelRendState();
			pGameGraphics2D->DrawGameModel(iDrawX,iDrawY,&model,100.0f/* * WINDOW_SCALE_WIDTH */,fRotation,aninfo,matmodelworld,matworld);
			Player.GetRightHandWeapon()->RenderToScreen(iDrawX,iDrawY,100.0f,fRotation,pCamera,&matmodelworld,&matworld);
			Player.GetLeftHandWeapon()->RenderToScreen(iDrawX,iDrawY,100.0f,fRotation,pCamera,&matmodelworld,&matworld);
			Player.GetWing()->RenderToScreen(iDrawX,iDrawY,100.0f,fRotation,pCamera,&matmodelworld,&matworld);
			pGameGraphics2D->RecoverFogState();
			
			
		}
		//******ADD BY 彭谦*******//
		else
		{
			pGameGraphics2D->DrawGameModel(iDrawX,iDrawY,pModel->pModel,100.0f/* * WINDOW_SCALE_WIDTH */,fRotation,pShape->GetActionTimer());
			
		}
		
			
	}
}

void CModelList::SetupEquipment(GameModel* pGameModel, CShape *pShape)
{
	long lType = pShape->GetType();
	long lState = pShape->GetState();
	long lAct = pShape->GetAction();

	if( pGameModel )
	{
		switch(lType)
		{
			// 玩家
		case TYPE_PLAYER:
			{
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(pShape);
				if (pPlayer==NULL)
					return ;

				DWORD dwHelm=0, dwBody=0,dwGlove=0, dwShoes=0, dwWaistBand=0, dwWeaponActType=0,dwPifeng = 0;
				int   lHelmLevel=0, lBodyLevel=0,lGloveLevel=0, lShoesLevel=0, lBackLevel=0,dwPifengLevel = 0;

				// 主角自己
				if (pPlayer==GetGame()->GetMainPlayer())
				{
					CGoods* pEquip = pPlayer->GetEquip(CPlayer::EQUIP_HEAD);
					dwHelm = pEquip?pEquip->GetProperty()->dwEquipID:0;
					lHelmLevel = pEquip?pEquip->GetEffectVal(48,1):0;

					pEquip = pPlayer->GetEquip(CPlayer::EQUIP_BODY);
					dwBody = pEquip?pEquip->GetProperty()->dwEquipID:1;
					lBodyLevel = pEquip?pEquip->GetEffectVal(48,1):0;

					pEquip = pPlayer->GetEquip(CPlayer::EQUIP_PIFENG);
					dwPifeng = pEquip?pEquip->GetProperty()->dwEquipID:0;
					dwPifengLevel = pEquip?pEquip->GetEffectVal(48,1):0;

					dwWeaponActType = pEquip?pEquip->GetProperty()->dwWeaponActType:0;
					pEquip = pPlayer->GetEquip(CPlayer::EQUIP_GLOVE);
					dwGlove = pEquip?pEquip->GetProperty()->dwEquipID:1;
					lGloveLevel = pEquip?pEquip->GetEffectVal(48,1):0;

					pEquip = pPlayer->GetEquip(CPlayer::EQUIP_SHOES);
					dwShoes = pEquip?pEquip->GetProperty()->dwEquipID:1;
					lShoesLevel = pEquip?pEquip->GetEffectVal(48,1):0;

					pEquip = pPlayer->GetEquip(CPlayer::EQUIP_WAISTBAND);
					dwWaistBand = pEquip?pEquip->GetProperty()->dwEquipID:0;
					lBackLevel = pEquip?pEquip->GetEffectVal(48,1):0;
				}
				else	// 其他玩家
				{
					CGoodsList::tagGoods2 *pst;

					pst = CGoodsList::GetProperty(pPlayer->GetEquipIndex(CPlayer::EQUIP_HEAD));
					dwHelm = pst?pst->BaseProperty.dwEquipID:0;
					lHelmLevel = pPlayer->GetEquipLevel(CPlayer::EQUIP_HEAD);

					pst = CGoodsList::GetProperty(pPlayer->GetEquipIndex(CPlayer::EQUIP_BODY));
					dwBody = pst?pst->BaseProperty.dwEquipID:1;
					lBodyLevel = pPlayer->GetEquipLevel(CPlayer::EQUIP_BODY);

					pst = CGoodsList::GetProperty(pPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON));
					dwPifeng = pst?pst->BaseProperty.dwEquipID:0;
					dwPifengLevel = pPlayer->GetEquipLevel(CPlayer::EQUIP_WEAPON);
					dwWeaponActType = pst?pst->BaseProperty.dwWeaponActType:0;

					pst = CGoodsList::GetProperty(pPlayer->GetEquipIndex(CPlayer::EQUIP_GLOVE));
					dwGlove = pst?pst->BaseProperty.dwEquipID:1;
					lGloveLevel = pPlayer->GetEquipLevel(CPlayer::EQUIP_GLOVE);

					pst = CGoodsList::GetProperty(pPlayer->GetEquipIndex(CPlayer::EQUIP_SHOES));
					dwShoes = pst?pst->BaseProperty.dwEquipID:1;
					lShoesLevel = pPlayer->GetEquipLevel(CPlayer::EQUIP_SHOES);

					pst = CGoodsList::GetProperty(pPlayer->GetEquipIndex(CPlayer::EQUIP_WAISTBAND));
					dwWaistBand = pst?pst->BaseProperty.dwEquipID:0;
					lBackLevel = pPlayer->GetEquipLevel(CPlayer::EQUIP_WAISTBAND);
				}

				static char str[32];
				static DWORD dwFACE=MAKEFOURCC('F','A','C','E');
				static DWORD dwHAIR=MAKEFOURCC('H','A','I','R');
				//static DWORD dwFACE=MAKEFOURCC('F','A','C','E');
				static DWORD dwHELM=MAKEFOURCC('H','E','L','M');
				static DWORD dwARMO=MAKEFOURCC('A','R','M','O');
				static DWORD dwPIFE=MAKEFOURCC('P','I','F','E');//披风
				static DWORD dwHAND=MAKEFOURCC('H','A','N','D');
				static DWORD dwFOOT=MAKEFOURCC('F','O','O','T');
				static DWORD dwBACK=MAKEFOURCC('B','A','C','K');
				static DWORD dwSCAB=MAKEFOURCC('S','C','A','B');

				/*if (lState==CShape::STATE_FIGHT&&!pPlayer->IsEmotionShowing())
				{
				pGameModel->SetGroupVisible(dwRHWP, dwWeapon);	// 武器
				pGameModel->SetGroupVisible(dwBACK, FALSE);		// 背
				}
				else
				{
				pGameModel->SetGroupVisible(dwRHWP, FALSE);		// 武器
				pGameModel->SetGroupVisible(dwBACK, dwWeapon);	// 背
				}*/

				// 身体
				pGameModel->SelectDefaultPart(dwFACE);

				// 头发(固有)
				sprintf(str, "%04d", pPlayer->GetHeadPic());
				pGameModel->SelectNullPart(dwHAIR);
				pGameModel->SelectPart(dwHAIR, MAKEFOURCC(str[0],str[1],str[2],str[3]));

				//// 脸(固有)
				//sprintf(str, "%04d", dwHelm);
				//pGameModel->SelectDefaultPart(dwFACE);
				//pGameModel->SelectPart(dwFACE, MAKEFOURCC(str[0],str[1],str[2],str[3]));

				// 头盔
				sprintf(str, "%04d", dwHelm);
				pGameModel->SelectNullPart(dwHELM);
				if(pPlayer->GetDisplayHeadPiece())
				{
					pGameModel->SelectPart(dwHELM, MAKEFOURCC(str[0],str[1],str[2],str[3]));
					SetLevel(pGameModel,dwHELM,MAKEFOURCC(str[0],str[1],str[2],str[3]),lHelmLevel);
				}

				// 盔甲
				sprintf(str, "%04d", dwBody);
				pGameModel->SelectNullPart(dwARMO);
				pGameModel->SelectPart(dwARMO, MAKEFOURCC(str[0],str[1],str[2],str[3]));
				SetLevel(pGameModel,dwARMO,MAKEFOURCC(str[0],str[1],str[2],str[3]),lBodyLevel);

				// 披风
				sprintf(str, "%04d", dwPifeng);
				pGameModel->SelectNullPart(dwPIFE);
				pGameModel->SelectPart(dwPIFE, MAKEFOURCC(str[0],str[1],str[2],str[3]));
				SetLevel(pGameModel,dwPIFE,MAKEFOURCC(str[0],str[1],str[2],str[3]),lGloveLevel);

				// 护腕
				sprintf(str, "%04d", dwGlove);
				pGameModel->SelectNullPart(dwHAND);
				pGameModel->SelectPart(dwHAND, MAKEFOURCC(str[0],str[1],str[2],str[3]));
				SetLevel(pGameModel,dwHAND,MAKEFOURCC(str[0],str[1],str[2],str[3]),lGloveLevel);

				// 靴子
				sprintf(str, "%04d", dwShoes);
				pGameModel->SelectNullPart(dwFOOT);
				pGameModel->SelectPart(dwFOOT, MAKEFOURCC(str[0],str[1],str[2],str[3]));
				SetLevel(pGameModel,dwFOOT,MAKEFOURCC(str[0],str[1],str[2],str[3]),lShoesLevel);

				// 背
				/*sprintf(str, "%04d", dwWeapon);
				pGameModel->SelectNullPart(dwBACK);
				pGameModel->SelectPart(dwBACK, MAKEFOURCC(str[0],str[1],str[2],str[3]));
				SetLevel(pGameModel,dwBACK,MAKEFOURCC(str[0],str[1],str[2],str[3]),lWeaponLevel);*/

				// 刀鞘
				/*sprintf(str, "%04d", dwWeapon);
				pGameModel->SelectNullPart(dwSCAB);
				pGameModel->SelectPart(dwSCAB, MAKEFOURCC(str[0],str[1],str[2],str[3]));
				SetLevel(pGameModel,dwSCAB,MAKEFOURCC(str[0],str[1],str[2],str[3]),lWeaponLevel);*/

				bool bDisplayHair = TRUE;
				if (dwHelm && pPlayer->GetDisplayHeadPiece())
					bDisplayHair = GetGame()->IsNeedDisplayHair(pPlayer->GetOccupation(), static_cast<BYTE>(dwHelm));

				pGameModel->SetGroupVisible(dwFACE, TRUE);			// 脸(固有)
				pGameModel->SetGroupVisible(dwHAIR, bDisplayHair);	// 头发(固有)
				//	pGameModel->SetGroupVisible(dwFACE, TRUE);		// 脸(固有)
				pGameModel->SetGroupVisible(dwHELM, dwHelm);		// 头盔
				pGameModel->SetGroupVisible(dwARMO, TRUE);			// 盔甲
				pGameModel->SetGroupVisible(dwHAND, TRUE);			// 护腕
				pGameModel->SetGroupVisible(dwFOOT, TRUE);			// 靴子
				pGameModel->SetGroupVisible(dwPIFE,	TRUE);		    // 披风

				/*CHorse *pHorse = pPlayer->GetHorse();
				BOOL bRide(FALSE);
				if (pHorse && pHorse->IsVisible())
					bRide = TRUE;*/

				/*if ((lState == CShape::STATE_FIGHT) && !bRide)
				{
					if (dwWeapon && pPlayer->GetOccupation()==OCC_Fighter && dwWeaponActType==1)	// 武士的鞘刀
					{
						// 刀在手上
						if (lAct==CShape::ACT_ATTACK || lAct>=CShape::ACT_PERFORM || lAct>=CShape::ACT_SEL_X1)
						{
							pGameModel->SetGroupVisible(dwRHWP, dwWeapon);	// 武器
							pGameModel->SetGroupVisible(dwBACK, FALSE);		// 背
						}
						else	// 刀在腰间
						{
							pGameModel->SetGroupVisible(dwRHWP, FALSE);		// 武器
							pGameModel->SetGroupVisible(dwBACK, dwWeapon);	// 背
						}
					}
					else	// 普通武器
					{
						if(pPlayer->IsEmotionShowing())
						{
							pGameModel->SetGroupVisible(dwRHWP, FALSE);	// 武器
							pGameModel->SetGroupVisible(dwBACK, dwWeapon);		// 背
						}
						else
						{
							pGameModel->SetGroupVisible(dwRHWP, dwWeapon);	// 武器
							pGameModel->SetGroupVisible(dwBACK, FALSE);		// 背
						}
					}
				}
				else
				{
					pGameModel->SetGroupVisible(dwRHWP, FALSE);		// 武器
					pGameModel->SetGroupVisible(dwBACK, dwWeapon);	// 背
				}

				pGameModel->SetGroupVisible(dwSCAB, TRUE);		// 刀鞘*/
			}
			break;

			// 怪物、NPC
		case TYPE_MONSTER:
			{
				pGameModel->SetGroupVisible(TRUE);
				pGameModel->SelectDefaultPart();
				pGameModel->EnableParticleEmitter(TRUE);
				pGameModel->EnableTrailEmitter(TRUE);
				static DWORD dwSTND = MAKEFOURCC('S','T','N','D');
				static DWORD dwIDLE = MAKEFOURCC('I','D','L','E');
				static DWORD dwWalk = MAKEFOURCC('W','A','L','K');
				static DWORD dwDIED = MAKEFOURCC('D','I','E','D');
				static DWORD dwATK = MAKEFOURCC('A','T','K','0');
				static DWORD dwHURT = MAKEFOURCC('H','U','R','T');

				//设置LEVEL
				CMonster *pMonster = (CMonster *)pShape;
				pGameModel->SetGroupLevel(pMonster->GetPicLevel());
			}
			break;

		/*case TYPE_NPC:
			{
				pGameModel->SetGroupVisible(TRUE);
				pGameModel->SelectDefaultPart();
				pGameModel->EnableParticleEmitter(TRUE);
				pGameModel->EnableTrailEmitter(TRUE);
				static DWORD dwSTND = MAKEFOURCC('S','T','N','D');
				static DWORD dwIDLE = MAKEFOURCC('I','D','L','E');
				static DWORD dwWalk = MAKEFOURCC('W','A','L','K');
				static DWORD dwDIED = MAKEFOURCC('D','I','E','D');
				static DWORD dwATK = MAKEFOURCC('A','T','K','0');
				static DWORD dwHURT = MAKEFOURCC('H','U','R','T');
			}
			break;*/

			// 物品
		case TYPE_GOODS:
			{
				CGoods *pGoods = (CGoods*)pShape;
				int iLevel = pGoods->GetEffectVal(48,1);
				pGameModel->SetGroupLevel(iLevel);
			}
		case TYPE_EFFECT:
		case TYPE_BUILD:
		case TYPE_CITYGATE:
		case TYPE_SKILL:
			pGameModel->SetGroupVisible(TRUE);
			pGameModel->SelectDefaultPart();
			pGameModel->EnableParticleEmitter(TRUE);
			pGameModel->EnableTrailEmitter(TRUE);
			break;
		}
	}
}

// 是否显示完成一个循环
BOOL CModelList::IsFinished(CShape* pShape)
{
	////判断播放结束
	//GameModel::AnimInfo::tagActionInfo *pActionInfo = pShape->m_cAnimInfo.GetActionInfo();
	//return (pActionInfo->dwCurActionLoopCount > 0);

	return TRUE;
}

void	CModelList::SetLevel(GameModel* pGameModel,DWORD dwGroup, DWORD dwPart, int iLevel)
{
	GameModel::Group *pGroup = pGameModel->GetGroup(dwGroup);
	if (pGroup)
	{
		GameModel::Group::Part *pPart = pGroup->GetPart(dwPart);
		if (pPart)
			pPart->SetLevel(iLevel);
	}
}
