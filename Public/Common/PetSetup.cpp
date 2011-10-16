#include "StdAfx.h"
#include "PetSetup.h"

PetSetup::PetSetup(void)
{

}

PetSetup::~PetSetup(void)
{

}

bool PetSetup::LoadPetMaterialConfig(void)
{
	// 侍从强化材料
	ConfigParse petSetup("Data/PetSetup.xml");
	if( petSetup.IsLoaded() )
	{
		petSetup.SetCursor(1, "FactureList");
		do 
		{
			int lMaterialID;
			std::string origName;
			std::string explainText;
			//int goodsID;
			int goodsNum;
			int orderFlag;
			int rate;
			tagMaterialList materialList;

			petSetup.SetFirstChild("Material");
			petSetup.GetAttr("id", &lMaterialID, 0);
			explainText = petSetup.GetAttr("text",explainText.c_str());

			petSetup.SetFirstChild("Goods");
			do 
			{	
				tagMaterialGoods goods;
				petSetup.GetAttr("OrderFlag", &orderFlag, 0);
				petSetup.GetAttr("KeyNum", &goods.keyNum, 0);
				if (goods.keyNum==0)
				{
					//petSetup.GetAttr("OrderFlag", &orderFlag, 0);
					origName = petSetup.GetAttr("OrigName", origName.c_str());
					//goodsID = GoodsSetup::QueryGoodsIDByOriginalName(origName.c_str());
					petSetup.GetAttr("Num", &goodsNum, 0);
					petSetup.GetAttr("Rate", &rate, 0);
					tagMaterial materialElem(orderFlag, origName,goodsNum, rate );
					goods.goodstable.push_back(materialElem);
				}
				else
				{
					goods.key = petSetup.GetAttr("Key", goods.key.c_str());
					petSetup.SetFirstChild("Eff");
					do 
					{
						origName = petSetup.GetAttr("OrigName", origName.c_str());
						//goodsID = GoodsSetup::QueryGoodsIDByOriginalName(origName.c_str());
						petSetup.GetAttr("Num", &goodsNum, 0);
						petSetup.GetAttr("Rate", &rate, 0);
						tagMaterial materialElem(orderFlag, origName,goodsNum, rate );
						goods.goodstable.push_back(materialElem);
					} while(petSetup.SetNextSibling());
					petSetup.SetParent();
				}
				materialList.goodsList.push_back(goods);
				materialList.explainText = explainText;
			} while(petSetup.SetNextSibling());
			petSetup.SetParent();

			m_materialList.insert(make_pair(lMaterialID,materialList));
		} while(petSetup.SetNextSibling());
		petSetup.SetParent();
		return true;
	}
	else
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "Can't find file Data/PetSetup.xml.");
		return false;
	}
}

bool PetSetup::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	long lNum = (long)m_materialList.size();
	setWriteDB.AddToByteArray(lNum);
	ITR_MATERIAL_LIST iterM = m_materialList.begin();
	for (; iterM != m_materialList.end(); ++iterM)
	{
		setWriteDB.AddToByteArray(iterM->first);
		tagMaterialList& materialList = iterM->second;
		lNum = (long)materialList.goodsList.size();
		setWriteDB.AddToByteArray(lNum);
		vector<tagMaterialGoods>::iterator iterGoods = materialList.goodsList.begin();
		for (; iterGoods != materialList.goodsList.end(); ++iterGoods)
		{
			tagMaterialGoods& goods = *iterGoods;
			setWriteDB.AddToByteArray((long)goods.keyNum);
			setWriteDB.AddToByteArray(goods.key.c_str());
			lNum = (long)goods.goodstable.size();
			setWriteDB.AddToByteArray(lNum);
			vector<tagMaterial>::iterator it = goods.goodstable.begin();
			for (; it!= goods.goodstable.end(); ++it)
			{
				setWriteDB.AddToByteArray((long)it->orderFlag);
				setWriteDB.AddToByteArray(it->goodsID.c_str());
				setWriteDB.AddToByteArray((long)it->goodsNum);
				setWriteDB.AddToByteArray((long)it->rate);
			}
		}
	}

	return true;
}

bool PetSetup::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	long lNum = setReadDB.GetLongFromByteArray();
	for (long l=0; l<lNum; ++l)
	{
		tagMaterialList materialList;
		int goodsNum;
		int orderFlag;
		int rate;
		//int goodsID;
		std::string origName;
		long materialID = setReadDB.GetLongFromByteArray();

		long goodsSize = setReadDB.GetLongFromByteArray();
		for (long m=0; m<goodsSize; ++m)
		{
			tagMaterialGoods goods;
			goods.keyNum = setReadDB.GetLongFromByteArray();
			char strTemp[256];
			setReadDB.GetStringFromByteArray(strTemp, 256);
			goods.key = strTemp;
			long goodsTableSize = setReadDB.GetLongFromByteArray();
			for (long i=0; i<goodsTableSize; ++i)
			{
				orderFlag = setReadDB.GetLongFromByteArray();
				char strTemp2[256];
				setReadDB.GetStringFromByteArray(strTemp2, 256);
				origName = strTemp2;
				goodsNum = setReadDB.GetLongFromByteArray();
				rate = setReadDB.GetLongFromByteArray();
				tagMaterial materialElem(orderFlag, origName,goodsNum, rate );
				goods.goodstable.push_back(materialElem);
			}
			materialList.goodsList.push_back(goods);
		}

		m_materialList.insert(make_pair(materialID,materialList));
	}
	return true;
}

string PetSetup::GetExplainTextByID(long type)
{
	string text = "";
	ITR_MATERIAL_LIST it = m_materialList.find(type);
	if (it!=m_materialList.end())
	{
		text = it->second.explainText;
	}
	return text;
}