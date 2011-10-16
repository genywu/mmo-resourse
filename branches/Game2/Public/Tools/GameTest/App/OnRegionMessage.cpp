


#include "StdAfx.h"
#include "GameClient.h"
#include "RgnObjMgr.h"
#include "..\Script\ScriptSys.h"
#include "..\GameTest\GameTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CGameClient::OnReginMessage(CMessage* pMsg)
{
	long lSocketID = pMsg->GetSocketID();

	switch(pMsg->GetType())
	{
	case MSG_S2C_RGN_ADDSHAPE:
		//	新加对象
		{
			long lType = pMsg->GetLong();

			CGUID guid;
			pMsg->GetGUID(guid);

			DBReadSet db_r;
			pMsg->GetDBReadSet(db_r);

			CBaseObject* pObject = m_pClientObjMgr->FindObject(lType,guid);
			if( pObject )
			{
				bool isSelf = false;
				switch(lType)
				{
				case TYPE_PET:
					{
						pMsg->GetLong();	//重新接收宠物类型
						isSelf = pMsg->GetChar() ==0 ? false : true;	//重新接收是否是自己的
					}
					break;
				}

				pObject->DecordFromDataBlock(db_r,isSelf);
				CShape* pShape = dynamic_cast<CShape*>(pObject);
				if(pShape)
				{
					CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pShape);
					if(pMoveShape)
						pMoveShape->SetDestXY(pMoveShape->GetPosX(),pMoveShape->GetPosY());
				}
			}
			else
			{
				CMoveShape* pShape = (CMoveShape*)m_pRegion->FindChildObject(lType,guid);
				if( pShape )
				{
					this->AddObjectToClientRgn(pShape);
				}
				else
				{
					bool bSelf = 0;
					switch( lType )
					{
					case TYPE_PLAYER:
						pShape = new CPlayer;
						break;
					case TYPE_MONSTER:
						pShape = new CMonster;
						break;
					case TYPE_GOODS:
						pShape = new CGoods;
						bSelf = false;
						pShape->SetExID(guid);
						break;
					case TYPE_COLLECTION:
						pShape = new CCollectionGoods;
						break;
					case TYPE_PET:
						{
							//long l = db_r.GetLongFromByteArray();
							//switch(l)
							//{
							//case PET_TYPE_ELF:
							//	pShape = new CFairy;
							//	break;
							//case PET_TYPE_HUNTER:
							//	pShape = new CFighterPet;
							//	break;
							//case PET_TYPE_CONJURED:
							//	pShape = new CSummonPet;
							//	break;
							//case PET_TYPE_TRAP:
							//	pShape = new CTrap;
							//	break;
							//default:
							//	break;
							//}
							//bSelf = (bool)db_r.GetCharFromByteArray();
						}
						break;
					}//end swtich

					if ( pShape )
					{
						pShape->SetUsedTimes(0);

						if ( bSelf )
						{
							pShape->DecordFromDataBlock(db_r,true);
						}
						else
						{
							pShape->DecordFromDataBlock(db_r,false);
						}

						if ( lType == TYPE_PET )
						{
							//CPet* pPet = dynamic_cast<CPet*>(pShape);
							//if( pPet )
							//{
							//	pPet->SetPetSocketID(pMsg->GetSocketID());
							//	pPet->SetMaster( m_pMainPlayer );
							//	pPet->SetMasterGuid(m_pMainPlayer->GetExID());
							//	pPet->SetPetRegion( m_pRegion );
							//	pPet->EnterRegion(bSelf);
							//}
						}

						this->AddObjectToClientRgn(pShape);

#ifdef _DEBUG
						char szInfo[512] = {};
						char szGuid[128] = {};
						pShape->GetExID().tostring(szGuid);
						sprintf(szInfo,"RgnAddShape(%0x-%s-%s)",pShape,pShape->GetName(),szGuid);
						PutStringToFile("DelShape",szInfo);
#endif // _DEBUG

						if( pShape->GetType() == TYPE_MONSTER )
						{
							CMonster* pMonster = dynamic_cast<CMonster*>(pShape);
							if(pMonster)
							{
								pMonster->SetBlock(pMonster->GetTileX(),pMonster->GetTileY(),pMonster->GetBlockType());
							}
						}

					}
				}// end if else
			}// end if else
		}
		break;
	case MSG_S2C_RGN_DELSHAPE:
		//	移出对象
		{
			long lType = pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);

			long lUseGoods = pMsg->GetLong();

			CMoveShape* pShape = (CMoveShape*)m_pClientObjMgr->FindObject(lType,guid);
			if( pShape )
			{
				if( lType == TYPE_MONSTER )
					pShape->SetBlock(pShape->GetTileX(),pShape->GetTileY(),CRegion::BLOCK_NO);

				if ( pShape != m_pMainPlayer )
				{
					this->RemoveObjectFromRgn(pShape);

					if( pShape->GetUsedTimes() == 0 )
					{
#ifdef _DEBUG
						if( pShape->GetType() == TYPE_PLAYER )
						{
							char szInfo[1024] = {};
							char szGuid[128] = {};
							pShape->GetExID().tostring(szGuid);
							sprintf(szInfo,"RgnDelShape(%0x-%s-%s)",pShape,pShape->GetName(),szGuid);
							PutStringToFile("DelShape",szInfo);
						}
#endif // _DEBUG
						SAFE_DELETE(pShape);
					}
				}
			}
		}
		break;
	case MSG_S2C_RGN_CHANGE:
		//	切换场景
		{
			long lType		= pMsg->GetLong();
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			long lUseGoods = pMsg->GetLong();

			//	self
			if( PlayerID == m_pMainPlayer->GetExID() )
			{
				CGUID RegionGuid;
				pMsg->GetGUID(RegionGuid);

				long lRegionID = pMsg->GetLong();
				long lPlayerX = pMsg->GetLong();
				long lPlayerY = pMsg->GetLong();
				long lPlayerDir = pMsg->GetLong();

				char strRegionName[256] = "";
				pMsg->GetStr(strRegionName,256);
				long lTaxRate = pMsg->GetLong();

				REGION_TYPE eWarRegionType = (REGION_TYPE)pMsg->GetByte();
				BYTE bWaterRegion = pMsg->GetByte();

				bool bChangeEquip = pMsg->GetByte() == 0 ?false:true;

				long lResourceID = pMsg->GetLong();
				float fExScale = pMsg->GetFloat();

				//	场景PK类型 0：未开启 1：开启，但不可PK 2：开启并可PK
				char cPkFlags = pMsg->GetByte();

				assert(lPlayerX>=0&&lPlayerY>=0);

				//	分线解码
				{
					DBReadSet db_read;
					pMsg->GetDBReadSet(db_read);
					this->DecordLineFromDataBlock(db_read);
				}

				// -- 请求进入场景
				//@清除原客户端数据
				if( m_pRegion )
				{
					CMapRgnObj* mapObjectList = m_pClientObjMgr->GetObjectList();
					CMapRgnObj::iterator it = mapObjectList->begin();
					for(; it != mapObjectList->end();++it)
					{
						CMoveShape* pShape = (CMoveShape*)it->second;
						//	更新引用计数
						pShape->ReduUsedTimes();

						m_pRegion->RemoveObject(pShape);

						if( pShape->GetUsedTimes() == 0 )
						{
							if( pShape != m_pMainPlayer )
							{
#ifdef _DEBUG
								if( pShape->GetType() == TYPE_PLAYER )
								{
									char szInfo[1024] = {};
									char szGuid[128] = {};
									pShape->GetExID().tostring(szGuid);
									sprintf(szInfo,"RgnChange(%0x-%s-%s)",pShape,pShape->GetName(),szGuid);
									PutStringToFile("DelShape",szInfo);
								}
#endif // _DEBUG
								SAFE_DELETE(pShape);

							}
						}
					}
					mapObjectList->clear();
				}

				//	获取一个对应的新场景
				m_pRegion = CScriptSys::getInstance()->GetTestDialog()->FindClientRegion(lResourceID,RegionGuid);
				m_pRegion->SetName(strRegionName);
				//
				m_pMainPlayer->SetPosXY(lPlayerX + 0.5f,lPlayerY + 0.5f);
				m_pMainPlayer->SetRegionID(lRegionID);
				m_pMainPlayer->SetDestXY(lPlayerX + 0.5f,lPlayerY + 0.5f);
				m_pMainPlayer->SetAction(CShape::ACT_STAND);
				m_pMainPlayer->SetFather(m_pRegion);

				this->AddObjectToClientRgn(m_pMainPlayer);

				if( m_pRegion )
				{
					m_pRegion->SetTaxRate(lTaxRate);
					m_pRegion->SetWarRegionType(eWarRegionType);
					m_pRegion->SetExpScale(fExScale);
				}

				char s[256];
				memset(s,0,sizeof(s));
				sprintf(s,"ChangeState(%d,\"%s\")",lSocketID,"run");
				CScriptSys::getInstance()->PerformString(s);

			}
		}
		break;
	case MSG_S2C_RGN_CHANGESERVER:
		//	切换服务器
		{
			long lChangeServerCode = pMsg->GetLong();
			long lSocketID = pMsg->GetSocketID();
			char strIP[256];
			pMsg->GetStr(strIP,256);
			DWORD dwPort = pMsg->GetDWord();

			char s[256];
			memset(s,0,sizeof(s));
			sprintf(s,"S2CChangeServer(%d,\"%s\",%d,%d)",lSocketID,strIP,dwPort,lChangeServerCode);

			CScriptSys::getInstance()->PerformString(s);
		}
		break;
	default:
		break;
	}
}