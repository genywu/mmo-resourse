/********************************************************************
created:	2010/09/06
created:	6:9:2010   11:39
filename: 	e:\SVN_P6\trunk\Client\GameClient\ClientApp\Skills\SkillIni.cpp
file path:	e:\SVN_P6\trunk\Client\GameClient\ClientApp\Skills
file base:	SkillIni
file ext:	cpp
author:		zhaohang(kaba) (kuailekaba@gmail.com)

purpose:	新技能模块文案内容配表、显示配置文件、姿态需求配置文件管理类的实现
*********************************************************************/
#include "stdafx.h"
#include "SkillIni.h"
#include "../Other/GlobeSetup.h"

///技能显示配置头检查项数目
#define SKILLSHOW_ATTR_COUNT 11
/***********************************************************************/
CheckSkillIni::CheckSkillIni()
{
}

CheckSkillIni::~CheckSkillIni()
{

}

bool CheckSkillIni::CheckFile(CRFile* file,const char* pathName)
{
	if(!file)
	{
		MsgBox(pathName,MBT_FILENOTFOUND);
		return false;
	}
	else
		return true;
}

void CheckSkillIni::MsgBox(const char* pathName,eMsgBoxType msgtype)
{
	char tip[256];
	if(msgtype == MBT_FILENOTFOUND)
	{
		sprintf(tip,AppFrame::GetText("SDCC_6"),pathName);
	}
	else if(msgtype == MBT_FORMATERROR)
	{
		sprintf(tip,AppFrame::GetText("SDCC_5"),pathName);
	}
	MSGBOX(tip);
}
/***********************************************************************/
tagSkillDocLevel::tagSkillDocLevel():level(0),strDesc("")
{
}

tagSkillDocLevel::~tagSkillDocLevel()
{

}

void tagSkillDocLevel::SetDesc(const char* desc)
{
	if(!desc)
	{
		strDesc = "";
	}
	else
		strDesc = desc;
}

void tagSkillDocLevel::SetDesc(std::string desc)
{
	strDesc = desc;
}

const char* tagSkillDocLevel::GetDesc()
{
	return strDesc.c_str();
}

void tagSkillDocLevel::SetLevel(uint Level)
{
	level = Level;
}

uint tagSkillDocLevel::GetLevel()
{
	return level;
}

tagSkillDoc::tagSkillDoc():skillID(0),strSkillName("")
{
	RemovedAllLevel();
}
tagSkillDoc::~tagSkillDoc()
{
	RemovedAllLevel();
}

uint tagSkillDoc::GetSkillID()
{
	return skillID;
}

void tagSkillDoc::SetSkillID(uint id)
{
	skillID = id;
}

const char* tagSkillDoc::GetSkillName()
{
	return strSkillName.c_str();
}

void tagSkillDoc::SetSkillName(const char* name)
{
	if(!name)
	{
		strSkillName = "";
	}
	else
		strSkillName = name;
}
void tagSkillDoc::SetSkillName(std::string name)
{
	strSkillName = name;
}

bool tagSkillDoc::AddLevel(tagSkillLD* Level)
{
	if(!Level)
		return false;
	MapSKILLLD::iterator iter = mapSkillLD.find(Level->GetLevel());
	if(iter == mapSkillLD.end() )
	{
		mapSkillLD[Level->GetLevel()] = Level;
		return true;
	}
	else
	{
		return false;
	}
}
void tagSkillDoc::RemovedLevel(uint Level)
{
	MapSKILLLD::iterator iter = mapSkillLD.find(Level);
	tagSkillDocLevel* ref = NULL;
	if(iter != mapSkillLD.end())
	{
		ref = (*iter).second;
		mapSkillLD.erase(iter);
		SAFEDELETE(ref);
	}
}

void tagSkillDoc::RemovedAllLevel()
{
	MapSKILLLD::iterator it = mapSkillLD.begin();
	for( ; it != mapSkillLD.end() ; ++it)
	{
		SAFEDELETE( (*it).second);
	}
	mapSkillLD.clear();
}

tagSkillLD* tagSkillDoc::GetLevel(uint Level)
{
	MapSKILLLD::iterator it = mapSkillLD.find(Level);
	if( it != mapSkillLD.end())
		return (*it).second;
	else
		return NULL;
}

/***********************************************************************/
bool CheckSkillDocIni::CheckFileHeader(std::vector<std::string>& vecStr,const char* pathName)
{
	std::string strID = AppFrame::GetText("SDCC_1");
	std::string strSkillName = AppFrame::GetText("SDCC_2");
	std::string strSkillLevel = AppFrame::GetText("SDCC_3");
	std::string strSkillLevelDesc = AppFrame::GetText("SDCC_4");
	if(strID != vecStr[0] || strSkillName != vecStr[1]
	|| strSkillLevel != vecStr[2] || strSkillLevelDesc != vecStr[3])
	{
		MsgBox(pathName,MBT_FORMATERROR);
		return false;
	}
	else
		return true;
}

bool CheckSkillDocIni::CheckID(uint id)
{
	return true;
}

bool CheckSkillDocIni::CheckSkillName(std::string str)
{
	return true;
}

bool CheckSkillDocIni::CheckLevel(uint level)
{
	return true;
}

bool CheckSkillDocIni::CheckLevelDesc(std::string str)
{
	return true;
}


/***********************************************************************/
/*  */
/***********************************************************************/
SkillDocIni::SkillDocIni()
{
	RemovedAllSkillDoc();
}
SkillDocIni::~SkillDocIni()
{
	RemovedAllSkillDoc();
}

bool SkillDocIni::LoadFile(const char* pathName /* =  */)
{
	if(!pathName)
		return false;
	stringstream stream;
	CRFile* prfile = rfOpen(pathName);
	check.CheckFile(prfile,pathName);
	if(prfile)
	{
		prfile->ReadToStream(stream);
		rfClose(prfile);
		std::vector<std::string>vecStr;
		std::string strTemp;
		//读SkillID 头
		stream >> strTemp;
		vecStr.push_back(strTemp);
		stream >> strTemp;
		vecStr.push_back(strTemp);
		stream >> strTemp;
		vecStr.push_back(strTemp);
		stream >> strTemp;
		vecStr.push_back(strTemp);
		if(!check.CheckFileHeader(vecStr,pathName))
		{
			return false;
		}
		//读具体数据
		uint id;
		uint level;
		std::string name;
		std::string leveDesc;
		RemovedAllSkillDoc();
		while (!stream.eof())
		{
			stream >> id >> name >> level >> leveDesc;
			//检查数据是否合法
			if( !check.CheckID(id) ||
				!check.CheckSkillName(name) ||
				!check.CheckLevel(level) || 
				!check.CheckLevelDesc(leveDesc))
			{
				return false;
			}
			tagSkillLD* skillLD = new tagSkillLD();
			skillLD->SetLevel(level);
			skillLD->SetDesc(leveDesc);

			tagSkillDoc* skilldoc = GetSkillDoc(id);
			if(skilldoc)
			{
				if(!skilldoc->AddLevel(skillLD))
				{
					SAFEDELETE(skillLD);
				}
			}
			else
			{
				skilldoc = new tagSkillDoc();
				skilldoc->SetSkillID(id);
				skilldoc->SetSkillName(name);
				if(!skilldoc->AddLevel(skillLD))
				{
					SAFEDELETE(skillLD);
				}
				if(!AddSkillDoc(skilldoc))
				{
					SAFEDELETE(skilldoc);
				}
			}
		}
		return true;
	}
	else
		return false;
}


bool SkillDocIni::AddSkillDoc(tagSkillDoc* skilldoc)
{
	MapSKILLDOC::iterator iter = mSkillDoc.find(skilldoc->GetSkillID());
	if( iter == mSkillDoc.end())
	{
		mSkillDoc[skilldoc->GetSkillID()] = skilldoc;
		return true;
	}
	else
		return false;

}
void SkillDocIni::RemovedSkillDoc(uint skillid)
{
	MapSKILLDOC::iterator iter = mSkillDoc.find(skillid);
	if(iter != mSkillDoc.end())
	{
		mSkillDoc.erase(iter);
	}
}
tagSkillDoc* SkillDocIni::GetSkillDoc(uint skillid)
{
	MapSKILLDOC::iterator iter = mSkillDoc.find(skillid);
	if(iter != mSkillDoc.end())
		return (*iter).second;
	else
		return NULL;
}

void SkillDocIni::RemovedAllSkillDoc()
{
	MapSKILLDOC::iterator iter = mSkillDoc.begin();
	for( ; iter != mSkillDoc.end() ; ++iter)
	{
		SAFEDELETE((*iter).second);
	}
	mSkillDoc.clear();
}

/***********************************************************************/
/* zhohang add 2010-9-7 */
tagSkillShowState::tagSkillShowState():
mlevel(-1),
mIntonateTime(-1),
mEnergy(-1),
mMP(-1),
mHP(-1),
mGoodsNum(-1),
mCoolDown(-1),
mMinAttackDistance(-1),
mMaxAttackDistance(-1)
{
	strcpy(mGoodsName,"");
}

tagSkillShowState::~tagSkillShowState()
{

}
tagSkillShowState& tagSkillShowState::operator =(const tagSkillShowState &ref)
{
	if( this == &ref )
		return *this;
	else
	{
		mlevel = ref.mlevel;
		mIntonateTime = ref.mIntonateTime;
		mEnergy = ref.mEnergy;
		mMP = ref.mMP;
		mHP = ref.mHP;
		mGoodsNum = ref.mGoodsNum;
		mCoolDown = ref.mCoolDown;
		mMinAttackDistance = ref.mMinAttackDistance;
		mMaxAttackDistance = ref.mMaxAttackDistance;
		strcpy(mGoodsName,ref.mGoodsName);
		return *this;
	}
}
/***********************************************************************/
SkillShow::SkillShow():
mSkillID(-1)
{
	RemovedAllSkillShowState();
}
SkillShow::~SkillShow()
{
	RemovedAllSkillShowState();
}
void SkillShow::SetID(int id)
{
	mSkillID = id;
}
int SkillShow::GetID()
{
	return mSkillID;
}

tagSkillShowState* SkillShow::GetSkillShowState(int level)
{
	MapSkillLevelData::iterator it = mLevelDataMap.find(level);
	if(it != mLevelDataMap.end())
		return (*it).second;
	else
		return NULL;
}
bool SkillShow::AddSkillShowState(tagSkillShowState* add)
{
	if(add && mLevelDataMap.find(add->mlevel) == mLevelDataMap.end())
	{
		mLevelDataMap[add->mlevel] = add;
		return true;
	}
	else 
		return false;
}

bool SkillShow::RemovedSkillShowState(int level)
{
	 MapSkillLevelData::iterator it = mLevelDataMap.find(level);
	 if( it != mLevelDataMap.end())
	 {
		 mLevelDataMap.erase(it);
		 SAFEDELETE((*it).second);
		 return true;
	 }
	 else 
		 return false;
}

void SkillShow::RemovedAllSkillShowState()
{
	MapSkillLevelData::iterator it = mLevelDataMap.begin() ;
	for(  ; it != mLevelDataMap.end() ; ++it)
	{
		SAFEDELETE( (*it).second );
	}
	mLevelDataMap.clear();
}
/***********************************************************************/
SkillShowIni::SkillShowIni()
{
	RemovedAllSkillShow();
}
SkillShowIni::~SkillShowIni()
{
	RemovedAllSkillShow();
}

bool SkillShowIni::AddSkillShow(SkillShow* item)
{
	if(!item)
	{
		return false;
	}
	MapSkillShow::iterator iter = mSkillShowMap.find(item->GetID());
	if( iter == mSkillShowMap.end())
	{
		mSkillShowMap[item->GetID()] = item;
		return true;
	}
	else 
		return false;
}
bool SkillShowIni::RemovedSkillShow(int id)
{
	MapSkillShow::iterator iter = mSkillShowMap.find(id);
	if( iter != mSkillShowMap.end() )
	{
		mSkillShowMap.erase(iter);
		SAFEDELETE( (*iter).second );
		return true;
	}
	else
		return false;
}

void SkillShowIni::RemovedAllSkillShow()
{
	MapSkillShow::iterator iter = mSkillShowMap.begin();
	for( ; iter != mSkillShowMap.end() ; ++iter)
	{
		SAFEDELETE( (*iter).second );
	}
	mSkillShowMap.clear();
}

SkillShow* SkillShowIni::GetSkillSow(int id)
{
	MapSkillShow::iterator iter = mSkillShowMap.find(id);
	if( iter != mSkillShowMap.end() )
	{
		return (*iter).second;
	}
	else 
		return NULL;
}

bool SkillShowIni::LoadFile(const char* pathName)
{
	if(!pathName)
		return false;
	stringstream stream;
	CRFile* file  = rfOpen(pathName);
	if(!mCheck.CheckFile(file,pathName))
		return false;
	file->ReadToStream(stream);
	rfClose(file);
	///技能ID
	int id;
	///检查属性头字符串
	std::vector<std::string>vecStr;
	std::string temp;
	//文件头
	for(int i = 0  ; i < SKILLSHOW_ATTR_COUNT ; ++i )
	{
		stream >> temp;
		vecStr.push_back(temp);
	}
	if( mCheck.CheckFileHeader(vecStr,pathName) == false)
	{
		return false;
	}
	//anItem读取数据
	RemovedAllSkillShow();
	while (ReadTo(stream,"#"))
	{
		tagSkillShowState* shState = new  tagSkillShowState();

		stream >> id >> shState->mlevel >> shState->mIntonateTime 
			>> shState->mEnergy >> shState->mMP >> shState->mHP 
			>> shState->mGoodsName >> shState->mGoodsNum
			>> shState->mCoolDown >> shState->mMinAttackDistance 
			>> shState->mMaxAttackDistance;
		SkillShow* ss = GetSkillSow(id);
		if(!ss)//没有指定ID的条目
		{
			ss = new SkillShow();
			ss->SetID(id);
			//添加等级state
			if(ss->AddSkillShowState(shState) == false)
			{
				char tip[256];
				sprintf(tip,AppFrame::GetText("SDCC_7"),shState->mlevel,id);
				MSGBOX(tip);
				SAFEDELETE(shState);
			}
			//添加show
			if( AddSkillShow(ss) == false)
			{
				SAFEDELETE(ss);
			}
		}
		else
		{
			//添加等级state
			if(ss->AddSkillShowState(shState) == false)
			{
				char tip[256];
				sprintf(tip,AppFrame::GetText("SDCC_7"),shState->mlevel,id);
				MSGBOX(tip);
				SAFEDELETE(shState);
			}
		}
	}
	return true;
}
/***********************************************************************/
bool CheckSkillShowIni::CheckFileHeader(std::vector<std::string>&vecStr,const char* pathName)
{
	if(vecStr.size() < SKILLSHOW_ATTR_COUNT )
	{
		return false;
	}
	std::string temp;
	for( int i = 0 ; i < SKILLSHOW_ATTR_COUNT ; ++i )
	{
		char name[256];
		///SDCC_28 - SDCC_38 
		sprintf(name,"SDCC_%d",i+28);
		temp = AppFrame::GetText(name);
		if( temp != vecStr[i])
		{
			MsgBox(pathName,MBT_FORMATERROR);
			return false;
		}
	}
	return true;
}
/***********************************************************************/
bool CheckSkillPoseIni::CheckFileHeader(std::vector<std::string>&vecStr,const char* pathName)
{
	std::string strID = AppFrame::GetText("SDCC_60");
	std::string strPose = AppFrame::GetText("SDCC_61");
	if(strID != vecStr[0] || strPose != vecStr[1])
	{
		MsgBox(pathName,MBT_FORMATERROR);
		return false;
	}
	else
		return true;
}
/***********************************************************************/
SkillPoseIni::SkillPoseIni()
{
	RemovedAllPose();
}

SkillPoseIni::~SkillPoseIni()
{
	RemovedAllPose();
}

bool SkillPoseIni::LoadFile(const char* pathName)
{
	if(!pathName)
		return false;
	CRFile* file = rfOpen(pathName);
	if(!file)
		return false;
	stringstream stream;
	file->ReadToStream(stream);
	rfClose(file);
	//读文件头
	std::string id;
	std::string pose;
	stream >> id >> pose;
	std::vector<std::string>vecStr;
	vecStr.push_back(id);
	vecStr.push_back(pose);
	if( mCheck.CheckFileHeader(vecStr,pathName) == false)
	{
		return false;
	}
	//读数据
	RemovedAllPose();
	while(ReadTo(stream,"#"))
	{
		tagSkillPose* pose = new tagSkillPose();
		stream >> pose->id >> pose->strPose;
		if( AddPose(pose) == false )
		{
			char tip[256];
			sprintf(tip,AppFrame::GetText("SDCC_62"));
			MSGBOX(tip);
			SAFEDELETE(pose);
		}
	}
	return true;
}
bool SkillPoseIni::AddPose(tagSkillPose* add)
{
	MapSkillPose::iterator iter = mSkillPoseList.find(add->id);
	if( iter == mSkillPoseList.end())
	{
		mSkillPoseList[add->id] = add;
		return true;
	}
	else 
		return false;
}
bool SkillPoseIni::RemovedPose(int key)
{
	MapSkillPose::iterator iter = mSkillPoseList.find(key);
	if( iter != mSkillPoseList.end())
	{
		mSkillPoseList.erase(iter);
		SAFEDELETE( (*iter).second);
		return true;
	}
	else 
		return false;
}
void SkillPoseIni::RemovedAllPose()
{
	MapSkillPose::iterator iter = mSkillPoseList.begin();
	for( ; iter != mSkillPoseList.end() ; ++iter)
	{
		SAFEDELETE( (*iter).second);
	}
	mSkillPoseList.clear();
}

tagSkillPose* SkillPoseIni::GetPose(int id)
{
	MapSkillPose::iterator iter = mSkillPoseList.find(id);
	if( iter != mSkillPoseList.end())
	{
		 return  (*iter).second;
	}
	else
		return NULL;
}
/***********************************************************************/