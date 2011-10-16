#include "StdAfx.h"
#include "StrUnifyInput.h"
#include "Public.h"
#include "FunctionKeyMapSystem.h"

#ifndef USE_IN_SERVER
#include "../client/GameClient/GameClient/Game.h"
#include "../client/GameClient/Input/KeyBoard.h"
#include "../client/GameClient/Guiextend/CGuiEx.h"
#include "../client/GameClient/Guiextend/HotKeySetupPageEx.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//MAPFKEY CFuncKeyMapSystem::m_FuncKey;

#ifndef USE_IN_SERVER

MAPINFOTYPE CFuncKeyMapSystem::m_DefultFuncKeyInfo;


bool CFuncKeyMapSystem::LoadDefaultFuncMapKey(string strFileName)
{
	if (""==strFileName) return false;
	Release();
	char strError[256];
	//=================================================================//
	CRFile *prfile = rfOpen(strFileName.c_str());
	if(NULL == prfile)
	{
		//输出错误信息
		_snprintf(strError, 256, "LoadFileError:%s",strFileName);
		MessageBox(NULL,strError,"",MB_OK); 
		return false;
	}
	TiXmlNode *pNode = NULL; 		
	TiXmlDocument m_Tdoc(strFileName.c_str());
	//载入配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		_snprintf(strError, 256, "Data of %s error!",strFileName.c_str());
		MessageBox(NULL,strError,"",MB_OK); 
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);
	//=================================================================//
	//找到根节点，如果无节点，输出错误提示
	pNode = m_Tdoc.FirstChild("FMK");
	if (pNode==NULL)
	{
		string strNode = "FMK";
		_snprintf(strError, 256, "Load %s node of %s error!",strNode.c_str(),strFileName.c_str());
		MessageBox(NULL,strError,"",MB_OK); 
		return false;
	}
	TiXmlElement* pElem = pNode->ToElement();


	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{

		if (!stricmp(pChildElem->Value(),"KeyTypeIndex"))
		{			
			INFOTYPE tgTypeInfo;
			tgTypeInfo.typeId  = (WORD)atoi(pChildElem->Attribute("id"));
			DWORD typedescid = (DWORD)atoi(pChildElem->Attribute("typedesc"));
			tgTypeInfo.strTypeDesc = CStringReading::LoadString(eIDS_MAPKEY,typedescid);
			if (!LoadFuncKeyConfig(pChildElem,tgTypeInfo))
			{
				_snprintf(strError, 256, "the file %s has the same id,please check it",strFileName.c_str());
				MessageBox(NULL,strError,"",MB_OK); 
				return false;
			}
			m_DefultFuncKeyInfo.insert(pair<WORD,INFOTYPE>(tgTypeInfo.typeId,tgTypeInfo));
		}
	}

	return true;
}

bool CFuncKeyMapSystem::LoadFuncKeyConfig(TiXmlElement *pElem,INFOTYPE& typeinfo)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"FuncIndex"))
		{
			FKMS funcid = (FKMS)atol(pChildElem->Attribute("id"));
			MAPFKEYINFOITER iter = typeinfo.FuncKeyInfo.find(funcid);
			if (iter!=typeinfo.FuncKeyInfo.end()) return false;
			
			INFOFUNC keyinfo;

			keyinfo.key.funcIndex = funcid;											// 功能索引值
			keyinfo.key.keyArray.mkey = (int)strtol(pChildElem->Attribute("mainkey"),NULL,16);		// 主键
			keyinfo.key.keyArray.askey1 = (int)strtol(pChildElem->Attribute("asskey1"),NULL,16);	// 辅助键1
			keyinfo.key.keyArray.askey2 = (int)strtol(pChildElem->Attribute("asskey2"),NULL,16);	// 辅助键2
			//keyinfo.key.keyArray.mkey = GetIntAttribute(pChildElem,"mainkey");		
			//keyinfo.key.keyArray.askey1 = GetIntAttribute(pChildElem,"asskey1");	
			//keyinfo.key.keyArray.askey2 = GetIntAttribute(pChildElem,"asskey2");	
			keyinfo.strFuncDesc = CStringReading::LoadString(eIDS_MAPKEY,GetIntAttribute(pChildElem,"keydesc"));	// 描述功能的文字信息
			keyinfo.key.modify = GetIntAttribute(pChildElem,"modify");				// 是否可修改

			typeinfo.FuncKeyInfo.insert(pair<FKMS,INFOFUNC>(funcid,keyinfo));
		}
	}
	return true;
}

//static void AddToByteArray(vector<BYTE> &pByteArray);
//static void DecordFromByteArray(BYTE *pByte, long& pos);

//获取整型
int	CFuncKeyMapSystem::GetIntAttribute(TiXmlElement *pElem,string strName)
{
	int iValue;
	if(pElem->QueryIntAttribute(strName.c_str(),&iValue))
	{
		iValue = 0;
	}
	return iValue;
}

// 释放
void CFuncKeyMapSystem::Release()
{
	m_DefultFuncKeyInfo.clear();
}

// 将一个虚拟键值转化为它对应的可视化名字（符合我们习惯的按键名称）
string& CFuncKeyMapSystem::ConvertVirKeyToString(string& strName,const int virKey)
{
	if(virKey!=0)
		strName = CStringReading::LoadString(eIDS_KEYNAME,virKey);
	else
		strName = "";
	return strName;
}

// 将热键由虚拟键值转换成可见的字符串
string& CFuncKeyMapSystem::GetHotKeyName(string& keyname,const SETKEY& keys)
{
	//SHORT  mkey = 0,askey1 = 0,askey2 = 0;
	string mkeyname = "",askey1name = "",askey2name = "";
	ConvertVirKeyToString(mkeyname,keys.mkey); //mkey = MapVirtualKey(keys.mkey,2);
	ConvertVirKeyToString(askey1name,keys.askey1);//askey1 = MapVirtualKey(keys.askey1,2);
	ConvertVirKeyToString(askey2name,keys.askey2);//askey2 = MapVirtualKey(keys.askey2,2);

	if (mkeyname!="")
	{
		if (askey1name!="")
		{
			//keyname.push_back(askey1);
			keyname += askey1name;
			keyname += "+";
		}
		if (askey2name!="")
		{
			//keyname.push_back(askey2);
			keyname += askey2name;
			keyname += "+";
		}
		//keyname.push_back(mkey); 
		keyname += mkeyname;
	}else
		keyname = CStringReading::LoadString(eIDS_KEYNAME,20000);

	return keyname;
}

// 根据参数dwFuncIndex指定的某个功能检测此功能的热键是否被触发,bContinuous指定此功能是否可连续，即按住键不放的情况
//bool CFuncKeyMapSystem::CheckFuncKey(MAPFKEY& mapkey,FKMS FuncIndex,SHORT appointkey,bool bContinuous)
//{	
//	BYTE  dif_key = GetGame()->GetKeyboard()->GetCurrentKey();
//
//	// 当热键设置界面打开时，暂时屏蔽所有的热键功能
//	if (GetGame()->GetCGuiEx()->GetHotKeySetupPageEx()->IsOpen()&&(appointkey!=VK_ESCAPE||dif_key!=DIK_ESCAPE)) return false;
//	
//	MAPFKEYITER iter = mapkey.find(FuncIndex);
//	if (iter!=mapkey.end())
//	{
//		SHORT mkey = 0,askey1 = 0,askey2 = 0;
//		int askey1_q = 0,askey2_q = 0;		// 用来暂存要查询的辅助键1、辅助键2
//		if (iter->second.keyArray.mkey!=0)
//		{
//			// 如果虚拟键值是小键盘里的↑↓←→的话， 就将扫描码强制设置为↑↓←→的扫描码
//			//（因为这四个键没有自己的虚拟键值，它们是与小键盘的方向键共享虚拟键值，MapVirtualKey不能转换出它们相应的扫描码）
//			if (iter->second.keyArray.mkey==0x26)
//			{
//				mkey = 0xC8;
//			}else if (iter->second.keyArray.mkey==0x28)
//			{
//				mkey = 0xD0;
//			}
//			else if (iter->second.keyArray.mkey==0x25)
//			{
//				mkey = 0xCB;
//			}
//			else if (iter->second.keyArray.mkey==0x27)
//			{
//				mkey = 0xCD;
//			}
//			else
//				mkey = MapVirtualKey(iter->second.keyArray.mkey,0);
//		}
//		else
//			return false;
//		// 取第一辅助键的状态
//		askey1_q = iter->second.keyArray.askey1;
//		if (askey1_q!=0)
//			askey1 = MapVirtualKey(askey1_q,0);
//
//		// 取第二辅助键的状态
//		askey2_q = iter->second.keyArray.askey2;
//		if (askey2_q!=0)
//			askey2 = MapVirtualKey(askey2_q,0);
//
//
//		if (appointkey!=0)
//		{
//			appointkey = MapVirtualKey(appointkey,0);	//将传入的虚拟键值转换为扫描码值
//			if (appointkey==DIK_RETURN)
//			{
//				int i = 1;
//			}
//			if ((!bContinuous&&appointkey==mkey||bContinuous&&GetGame()->GetKeyboard()->IsKeyDown(mkey))&&
//				(askey1_q==0||(askey1_q!=0&&askey1!=0&&GetGame()->GetKeyboard()->IsKeyDown(askey1)))&&
//				(askey2_q==0||(askey2_q!=0&&askey2!=0&&GetGame()->GetKeyboard()->IsKeyDown(askey2)))
//				)
//				return true;
//		}else
//		{
//			//appointkey = MapVirtualKey(appointkey,0);	//将传入的虚拟键值转换为扫描码值
//			if (dif_key==DIK_RETURN)
//			{
//				int i = 1;
//			}
//			if ((!bContinuous&&dif_key==mkey||bContinuous&&GetGame()->GetKeyboard()->IsKeyDown(mkey))&&
//				(askey1_q==0||(askey1_q!=0&&askey1!=0&&GetGame()->GetKeyboard()->IsKeyDown(askey1)))&&
//				(askey2_q==0||(askey2_q!=0&&askey2!=0&&GetGame()->GetKeyboard()->IsKeyDown(askey2)))
//				)
//				return true;
//		}
//
//		//SHORT mkey = 0,askey1 = 0,askey2 = 0;
//		//int askey1_q = 0,askey2_q = 0;		// 用来暂存要查询的辅助键1、辅助键2
//		//// 取主键的状态
//		//if (iter->second.keyArray.mkey!=0)
//		//	mkey = GetKeyState(iter->second.keyArray.mkey);
//		//else
//		//	return false;
//
//		//// 首先检测主键是否被按下，如果没有就直接返回false，不再继续检测辅助键位
//		//if (mkey==0||!(mkey&0x80)) return false;
//		//
//		//// 取第一辅助键的状态
//		//askey1_q = iter->second.keyArray.askey1;
//		//if (askey1_q!=0)
//		//	askey1 = GetKeyState(askey1_q);
//
//		//// 取第二辅助键的状态
//		//askey2_q = iter->second.keyArray.askey2;
//		//if (askey2_q!=0)
//		//	askey2 = GetKeyState(askey2_q);
//
//		//// 检测第一、二辅助键是否满足需要的状态
//		////（要求：1、都不需要被按下？2、需要其中之一被按下？3、需要两者都被按下？
//		//// 当属于这三种情况之一的时候，检测实际情况满足要求不，不满足就返回false）
//		//if ((askey1_q!=0&&(askey1==0||!(askey1&0x80))) ||
//		//    (askey2_q!=0&&(askey2==0||!(askey2&0x80)))) 
//		//	return false;
//
//		//return true;
//	}
//	return false;
//}

// 根据参数dwFuncIndex指定的功能索引值检测此功能的热键是否被触发,bContinuous指定此功能是否可连续，即按住键不放的情况（适用于按当前键盘扫描码取出的值来使用）
bool CFuncKeyMapSystem::CheckFuncKey(MAPFKEY& mapkey,FKMS FuncIndex,bool bContinuous)
{
	BYTE  dif_key = GetGame()->GetKeyboard()->GetCurrentKey();

	// 当热键设置界面打开时，暂时屏蔽所有的热键功能
	if (GetGame()->GetCGuiEx()->GetHotKeySetupPageEx()->IsOpen()&&dif_key!=DIK_ESCAPE) return false;

	MAPFKEYITER iter = mapkey.find(FuncIndex);
	if (iter!=mapkey.end())
	{
		SHORT mkey = 0,askey1 = 0,askey2 = 0;
		int askey1_q = 0,askey2_q = 0;		// 用来暂存要查询的辅助键1、辅助键2
		if (iter->second.keyArray.mkey!=0)
		{
			// 如果虚拟键值是小键盘里的↑↓←→的话， 就将扫描码强制设置为↑↓←→的扫描码
			//（因为这四个键没有自己的虚拟键值，它们是与小键盘的方向键共享虚拟键值，MapVirtualKey不能转换出它们相应的扫描码）
			if (iter->second.keyArray.mkey==0x26)
			{
				mkey = 0xC8;
			}else if (iter->second.keyArray.mkey==0x28)
			{
				mkey = 0xD0;
			}
			else if (iter->second.keyArray.mkey==0x25)
			{
				mkey = 0xCB;
			}
			else if (iter->second.keyArray.mkey==0x27)
			{
				mkey = 0xCD;
			}
			else
				mkey = MapVirtualKey(iter->second.keyArray.mkey,0);
		}
		else
			return false;
		// 取第一辅助键的状态
		askey1_q = iter->second.keyArray.askey1;
		if (askey1_q!=0)
			askey1 = MapVirtualKey(askey1_q,0);

		// 取第二辅助键的状态
		askey2_q = iter->second.keyArray.askey2;
		if (askey2_q!=0)
			askey2 = MapVirtualKey(askey2_q,0);


		if ((!bContinuous&&dif_key==mkey||bContinuous&&GetGame()->GetKeyboard()->IsKeyDown(mkey))&&
			(askey1_q==0||(askey1_q!=0&&askey1!=0&&GetGame()->GetKeyboard()->IsKeyDown(askey1)))&&
			(askey2_q==0||(askey2_q!=0&&askey2!=0&&GetGame()->GetKeyboard()->IsKeyDown(askey2)))
			)
			return true;
	}
	return false;
}

// 根据参数dwFuncIndex指定的某个功能检测此功能的热键是否被触发,bContinuous指定此功能是否可连续，即按住键不放的情况(适用于有指定虚拟键情况使用)
bool CFuncKeyMapSystem::CheckFuncKey(MAPFKEY& mapkey,FKMS FuncIndex,SHORT appointkey,bool bContinuous)
{	
	// 当热键设置界面打开时，暂时屏蔽所有的热键功能
	if (GetGame()->GetCGuiEx()->GetHotKeySetupPageEx()->IsOpen()&&appointkey!=VK_ESCAPE) return false;

	MAPFKEYITER iter = mapkey.find(FuncIndex);
	if (iter!=mapkey.end()&&appointkey!=0)
	{
		SHORT mkey = 0,askey1 = 0,askey2 = 0;
		int askey1_q = 0,askey2_q = 0;		// 用来暂存要查询的辅助键1、辅助键2
		if (iter->second.keyArray.mkey!=0)
		{
			// 如果虚拟键值是小键盘里的↑↓←→的话， 就将扫描码强制设置为↑↓←→的扫描码
			//（因为这四个键没有自己的虚拟键值，它们是与小键盘的方向键共享虚拟键值，MapVirtualKey不能转换出它们相应的扫描码）
			//if (iter->second.keyArray.mkey==0x26)
			//{
			//	mkey = 0xC8;
			//}else if (iter->second.keyArray.mkey==0x28)
			//{
			//	mkey = 0xD0;
			//}
			//else if (iter->second.keyArray.mkey==0x25)
			//{
			//	mkey = 0xCB;
			//}
			//else if (iter->second.keyArray.mkey==0x27)
			//{
			//	mkey = 0xCD;
			//}
			//else
			//	mkey = MapVirtualKey(iter->second.keyArray.mkey,0);
			mkey = iter->second.keyArray.mkey;
		}
		else
			return false;
		// 取第一辅助键的状态
		askey1_q = iter->second.keyArray.askey1;
		if (askey1_q!=0)
			askey1 = MapVirtualKey(askey1_q,0);

		// 取第二辅助键的状态
		askey2_q = iter->second.keyArray.askey2;
		if (askey2_q!=0)
			askey2 = MapVirtualKey(askey2_q,0);

		//appointkey = MapVirtualKey(appointkey,0);	//将传入的虚拟键值转换为扫描码值
		if ((!bContinuous&&appointkey==mkey/*||bContinuous&&GetGame()->GetKeyboard()->IsKeyDown(mkey)*/)&&
			(askey1_q==0||(askey1_q!=0&&askey1!=0&&GetGame()->GetKeyboard()->IsKeyDown(askey1)))&&
			(askey2_q==0||(askey2_q!=0&&askey2!=0&&GetGame()->GetKeyboard()->IsKeyDown(askey2)))
			)
			return true;
	}
	return false;
}



#endif

// 将默认的配置信息倒入当前使用的m_FuncKey中
//bool CFuncKeyMapSystem::DefaultToCurrent()
//{
//	if (m_DefultFuncKeyInfo.empty()) return false;
//	m_FuncKey.clear();
//
//	MAPINFOTYPEITER defultiter_b = m_DefultFuncKeyInfo.begin();
//	MAPINFOTYPEITER defultiter_e = m_DefultFuncKeyInfo.end();
//	for (;defultiter_b!=defultiter_e;defultiter_b++)
//	{
//		MAPFKEYINFOITER funciter_b = defultiter_b->second.FuncKeyInfo.begin();
//		MAPFKEYINFOITER funciter_e = defultiter_b->second.FuncKeyInfo.end();
//		for (;funciter_b!=funciter_e;funciter_b++)
//		{
//			m_FuncKey.insert(pair<FKMS,MAPKEY>(funciter_b->first,funciter_b->second.key));
//		}
//	}
//	return true;
//}


// 插入值
bool CFuncKeyMapSystem::InsertKeyToCurMap(MAPFKEY& mapkey,FKMS eIndex,MAPKEY key)
{
	if (mapkey.find(eIndex)!=mapkey.end()) return false;
	mapkey.insert(pair<FKMS,MAPKEY>(eIndex,key));
	return true;
}

// 删除值
bool CFuncKeyMapSystem::DelKeyToCurMap(MAPFKEY& mapkey,FKMS eIndex)
{
	MAPFKEYITER KeyIter = mapkey.find(eIndex);
	if (KeyIter!=mapkey.end())
		mapkey.erase(KeyIter);
	else
		return false;
	return true;
}

// 检测某个功能当前是否定义了热键值
bool CFuncKeyMapSystem::CheckFunHasAvailableKey(MAPFKEY& mapkey,FKMS eIndex)
{
	MAPFKEYITER KeyIter = mapkey.find(eIndex);
	if (KeyIter!=mapkey.end())
		return true;
	else
		return false;
}

// 修改某个功能的热键值
bool CFuncKeyMapSystem::AlterKeyofCurMap(MAPFKEY& mapkey,FKMS eIndex,MAPKEY key)
{
	MAPFKEYITER KeyIter = mapkey.find(eIndex);
	if (KeyIter!=mapkey.end())
		KeyIter->second = key;
	else 
		return false;
	return true;
}

