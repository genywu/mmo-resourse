/**
* @filename:GoodsOperate.cpp
* @date: 2010/3/15
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: GoodsOperate.h 的实现，从GoodsViewer.sln中copy过来
*/
#include "stdafx.h"
#include "GoodsOperate.h"
namespace UIData{
	const std::string GoodsOperate::s_vecGoodsType[gGoodsTypeNum] = {
			"普通0",					//0
			"消耗性道具1",				//1
			"卡片(镶嵌物)2",			//2
			"商品3",					//3
			"头盔4",					//4
			"衣服5",					//5
			"手套6",					//6
			"鞋子7",					//7
			"武器8",					//8
			"盾牌9",					//9
			"项链10",					//10
			"戒指11",					//11
			"腰带12",					//12
			"翅膀13",					//13
			"头饰14",					//14
			"外套15",					//15
			"精灵16",					//16
			"勋章17",					//17
			"背包18",					//18
			"箭矢19",					//19
			"金钱20",					//20
			"坐骑21",					//21
			"银币22",					//22
			"拾取品23",					//23
			"披风24",					//24
			"无类别25"
	};
	
	GoodsOperate::GoodsOperate()
	{

	}

	GoodsOperate::~GoodsOperate()
	{
		m_vecGoodAttr.clear();
		m_vecGoodItemType.clear();
	}

	bool GoodsOperate::LoadGoodsListData(const char* FileName /* ="goodslist.dat"  */)
	{
		FILE* fp;
		fp = fopen(FileName,"rb");
		if( fp == NULL )
		{
			char str[256];
			sprintf_s(str, _T("file '%s' can't found!"), _T("goodslist.dat"));
			MessageBox(NULL,str,"error",MB_OK);
			return false;
		}
		// 文件头和版本
		char szHeader[6];
		memset(szHeader, 0, 6);
		fread(szHeader, 5, 1, fp);
		if( strcmp(szHeader, "GOODS") != 0 )
		{
			char str[256];
			sprintf_s(str,_T("file '%s' is not goodslist file!"), "goodslist.dat");
			MessageBox(NULL,str, "error",MB_OK);	
			return false;
		}
		long lVersion = 0;
		fread(&lVersion, sizeof(long), 1, fp);
		// 具体内容	

		//物品数量
		long lNum = 0;
		fread(&lNum, sizeof(long), 1, fp);
		m_vecGoodAttr.reserve(lNum);

		for(int i=0; i<lNum; i++)
		{
			long lStrSize=0;
			char *pStr;
			tagGoodAttr stGoods;
			stGoods.dwSilverValue=0;
			stGoods.dwWeaponActType=0;

			fread(&stGoods.dwIndex, sizeof(stGoods.dwIndex), 1, fp);		//	编号

			//	原始名   < 20
			fread(&lStrSize,sizeof(long),1,fp);
			pStr=new char[lStrSize+1];
			fread(pStr,lStrSize,1,fp);
			pStr[lStrSize]='\0';
			stGoods.strOrigName=pStr;
			delete [] pStr;

			//	中文名称 < 20
			fread(&lStrSize,sizeof(long),1,fp);
			pStr=new char[lStrSize+1];
			fread(pStr,lStrSize,1,fp);
			pStr[lStrSize]='\0';
			stGoods.strName=pStr;
			delete [] pStr;

			fread(&stGoods.bSave, sizeof(stGoods.bSave), 1, fp);			//	是否需要存数据库
			fread(&stGoods.dwValue, sizeof(stGoods.dwValue), 1, fp);		//	价格
			fread(&stGoods.dwSilverValue, sizeof(stGoods.dwSilverValue), 1, fp);		//	价格
			fread(&stGoods.dwType, sizeof(stGoods.dwType), 1, fp);			//	物品类型

			// 客户端专用属性
			fread(&stGoods.dwIconId, sizeof(stGoods.dwIconId), 1, fp);		//	界面图形ID
			fread(&stGoods.dwGroundId, sizeof(stGoods.dwGroundId), 1, fp);		//	落地后图形ID
			fread(&stGoods.dwEquipID, sizeof(stGoods.dwEquipID), 1, fp);		//	换装图形ID
			fread(&stGoods.dwSound, sizeof(stGoods.dwSound), 1, fp);		//	拾取声音
			fread(&stGoods.dwSoundID1, sizeof(stGoods.dwSoundID1), 1, fp);		//	挥动/弱伤害声音
			fread(&stGoods.dwSoundID2, sizeof(stGoods.dwSoundID2), 1, fp);		//	特殊攻击/击中声音
			fread(&stGoods.bSoundSwitch, sizeof(stGoods.bSoundSwitch), 1, fp);	//	攻击时是否混音
			fread(&stGoods.dwWeaponActType, sizeof(stGoods.dwWeaponActType), 1, fp);//	武器对应的动作类型

			//	中文描述 < 64
			fread(&lStrSize,sizeof(long),1,fp);
			pStr=new char[lStrSize+1];
			fread(pStr,lStrSize,1,fp);
			pStr[lStrSize]='\0';
			stGoods.strContent=pStr;
			delete [] pStr;

			// 扩展属性
			long lEffectNum = 0;
			fread(&lEffectNum, sizeof(long), 1, fp);
			stGoods.vecAddAttr.reserve( lEffectNum );
			for(int j=0; j<lEffectNum; j++)
			{
				tagAddAttr stEffect;
				fread(&(stEffect.wType),sizeof(WORD),1,fp);
				fread(&(stEffect.bEnable),sizeof(bool),1,fp);
				fread(&(stEffect.bHide),sizeof(bool),1,fp);
				fread(&(stEffect.lValue1),sizeof(DWORD),1,fp);
				fread(&(stEffect.lValue2),sizeof(DWORD),1,fp);
				stGoods.vecAddAttr.push_back(stEffect);
			}

			// 套装属性
			lEffectNum = 0;
			fread(&lEffectNum, sizeof(long), 1, fp);
			stGoods.vecSuitAttr.reserve(lEffectNum);
			for(int j=0; j<lEffectNum; j++)
			{
				tagSuitAttr stEffect;
				fread(&(stEffect.wType),sizeof(WORD),1,fp);			
				fread(&(stEffect.lValue1),sizeof(DWORD),1,fp);
				fread(&(stEffect.lValue2),sizeof(DWORD),1,fp);
				fread(&(stEffect.wSuitNum),sizeof(WORD),1,fp);
				stGoods.vecSuitAttr.push_back(stEffect);
			}
			m_vecGoodAttr.push_back(stGoods);
		}
		fclose(fp);	
		return true;
	}

	bool GoodsOperate::LoadGoodsItemTypeData(const char* FileName /* = "itemtype.dat"  */)
	{
		std::ifstream ifsGoodsItemType( FileName );
		if ( ifsGoodsItemType.fail() )
		{
			MessageBox(NULL,"Load itemtype.dat error","Error",MB_OK);
			return false;
		}

		int iItemTypeCnt;
		std::string strItemIdx, strItemFlg, strItemName;
		ifsGoodsItemType >> iItemTypeCnt;
		if ( iItemTypeCnt > 0 )
		{
			m_vecGoodItemType.reserve( iItemTypeCnt );
			for ( int i = 0; i < iItemTypeCnt; ++i )
			{
				ifsGoodsItemType >> strItemIdx >> strItemFlg >> strItemName;
				m_vecGoodItemType.push_back( strItemName );
			}
		}
		else
		{
			MessageBox(NULL,"No have any ItemType","Error",MB_OK);
			return false;
		}
		return true;
	}

	const tagGoodAttr* GoodsOperate::GetGoodAttrByIndex(size_t Index) const
	{
		if(Index >= 0 || Index < m_vecGoodAttr.size())
			return &m_vecGoodAttr[Index];
		else
			return NULL;
	}

	const tagGoodAttr* GoodsOperate::GetGoodAttrByOgriName(const CEGUI::String &OgriName) const
	{
		size_t GoodsTotalNum = GetGoodsTotalNum();
		std::vector<UIData::tagGoodAttr>::const_iterator iter = m_vecGoodAttr.begin();
		for( ; iter != m_vecGoodAttr.end() ; ++iter)
		{
			if((*iter).strOrigName == OgriName)
				return &(*iter);
		}
		return NULL;
	}
	const std::string GoodsOperate::GetGoodsType(size_t Index)
	{
		if( Index >= gGoodsTypeNum)
			return std::string("");
		else
			return s_vecGoodsType[Index];
	}
}