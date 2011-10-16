#include "StdAfx.h"
#include ".\skilllist.h"
#include "../../public/ini.h"

extern HWND g_hWnd;

map<DWORD, CSkillList::tagSkill>		CSkillList::m_mapSkillList;		// 原始Skill列表(dwIndex, tagSkill)
map<string, WORD>			CSkillList::m_mapStepNameID;	// 步骤名称对应的ID

CSkillList::CSkillList(void)
{
}

CSkillList::~CSkillList(void)
{
	Release();
}

void CSkillList::Release()
{
	for(map<DWORD, tagSkill>::iterator it=m_mapSkillList.begin(); it!=m_mapSkillList.end(); it++)
	{
		tagSkill *p = &(*it).second;
		if( p )
		{
			long lLevelNum = (long)p->vectorLevels.size();
			for (int i=0; i<lLevelNum; i++)
			{
				long lStepNum = (long)p->vectorLevels[i].vectorSteps.size();
				for (int j=0; j<lStepNum; j++)
				{
					if (p->vectorLevels[i].vectorSteps[j].bStringParam)
					{
						char *pstr = p->vectorLevels[i].vectorSteps[j].pstrParam;
						delete pstr;
					}
				}
			}
		}
	}
	m_mapSkillList.clear();
	m_mapStepNameID.clear();
}

//// 从文件读取原始列表
//BOOL CSkillList::LoadSkillList(const char* filename)
//{
//	m_mapSkillList.clear();
//
//	ifstream stream;
//	stream.open(filename);
//	if( !stream.is_open() )
//	{
//		char str[256];
//		sprintf(str, "file '%s' can't found!", filename);
//		MessageBox(g_hWnd, str, "error", MB_OK);
//		return FALSE;
//	}
//
//	tagSkill stSkill;
//	while(ReadTo(stream, "*"))
//	{
//		stream  >> stSkill.dwIndex
//				>> stSkill.strName
//				>> stSkill.dwActionID
//				>> stSkill.dwCastEffectID
//				>> stSkill.dwHitEffectID
//				>> stSkill.dwRange
//				>> stSkill.dwObject
//				>> stSkill.dwCastTimePlus
//				>> stSkill.dwCastTimeMul
//				>> stSkill.dwDelayTime
//				>> stSkill.dwCoolDownTime
//				>> stSkill.dwInterrupt
//				>> stSkill.dwCostHp
//				>> stSkill.dwCostHpPer
//				>> stSkill.dwCostMp
//				>> stSkill.dwCostMpPer
//				>> stSkill.dwCostYao
//				>> stSkill.lParameter01
//				>> stSkill.lParameter02
//				>> stSkill.lParameter03
//				>> stSkill.lParameter04
//				>> stSkill.lParameter05
//				>> stSkill.lParameter06
//				>> stSkill.lParameter07
//				>> stSkill.lParameter08
//				>> stSkill.lParameter09
//				>> stSkill.lParameter10
//				>> stSkill.strDescribe;
//
//		m_mapSkillList[stSkill.dwIndex] = stSkill;
//	}
//
//	stream.close();
//	return true;
//}

// 从文件读取原始列表
BOOL CSkillList::LoadSkillList(const char* stepfilename, const char* dir_name)
{
	Release();

	// 读取技能名称对应的CommandID
	char *index="SkillStep";
	CIni Ini("data/SkillStep.ini");
	int num=Ini.GetContinueDataNum(index);

	for(int i=0; i<num; i++)
	{
		int nCommandID;
		char *tmp=Ini.ReadCaption(index, i);
		nCommandID=atoi(tmp);		//获得指令Id
		SAFE_DELETE(tmp);
		tmp=Ini.ReadText(index, i);
		m_mapStepNameID[tmp]=nCommandID;
		SAFE_DELETE(tmp);
	}	

	// 读取目录中的所有技能文件
	list<string> listFileName;
	FindFile(dir_name, ".skill", &listFileName);

	for(list<string>::iterator it=listFileName.begin(); it!=listFileName.end(); it++)
	{
		const char* filename = it->c_str();
		LoadOneSkill(filename);
	}

	return true;
}

// 读取一个技能
BOOL CSkillList::LoadOneSkill(const char* filename)
{
	tagSkill stSkill;

	ifstream stream;
	stream.open(filename);
	if( !stream.is_open() )
	{
		char str[256];
		_snprintf(str, 256, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return FALSE;
	}

	ReadTo(stream, "名称");
	stream >> stSkill.strName;
	ReadTo(stream, "描述");
	stream >> stSkill.strDescribe;
	Read(stream, "编号", stSkill.dwIndex);
	long lDestType;
	Read(stream, "目标类型", lDestType);
	stSkill.lDestType = (eSkillDestType)lDestType;

	char str[256];
	long lLevel = -1;
	while(1)
	{
		if (stream.eof())
			break;

		stream >> str;

		// 下一个等级
		if (strcmp(str, "<level>")==0)
		{
			stream >> str;
			lLevel++;

			tagLevel stLevel;
			stSkill.vectorLevels.push_back(stLevel);
			stSkill.vectorLevels[lLevel].wRange = 0;
			continue;
		}

		// 范围
		if (strcmp(str, "<range>")==0)
		{
			stream >> stSkill.vectorLevels[lLevel].wRange;
			continue;
		}

		// 是step
		if (m_mapStepNameID.find(str)!=m_mapStepNameID.end())
		{
			tagStep stStep;
			stStep.wCommandID = m_mapStepNameID[str];
			if (str[0] == '$')
				stStep.bStringParam = true;
			else
				stStep.bStringParam = false;

			if (stStep.bStringParam)
			{
				string strTemp;
				stream >> strTemp;
				stStep.pstrParam = new char[strTemp.size()+1];
				strcpy_s(stStep.pstrParam, strTemp.size()+1, strTemp.c_str());
			}
			else
				stream >> stStep.lParam;

			stSkill.vectorLevels[lLevel].vectorSteps.push_back(stStep);
		}
	}

	m_mapSkillList[stSkill.dwIndex] = stSkill;

	stream.close();
	return true;
}

// 添加到CByteArray
bool CSkillList::AddToByteArray(vector<BYTE>* pByteArray)
{
	long lSkillNum = (long)m_mapSkillList.size();
	_AddToByteArray(pByteArray, lSkillNum);
	for(map<DWORD, tagSkill>::iterator it=m_mapSkillList.begin(); it!=m_mapSkillList.end(); it++)
	{
		tagSkill *p = &(*it).second;
		if( p )
		{
			_AddToByteArray(pByteArray, (char*)p->strName.c_str());
			_AddToByteArray(pByteArray, (char*)p->strDescribe.c_str());
			_AddToByteArray(pByteArray, p->dwIndex);
			_AddToByteArray(pByteArray, (long)p->lDestType);

			long lLevelNum = (long)p->vectorLevels.size();
			_AddToByteArray(pByteArray, lLevelNum);
			for (int i=0; i<lLevelNum; i++)
			{
				_AddToByteArray(pByteArray, p->vectorLevels[i].wRange);

				long lStepNum = (long)p->vectorLevels[i].vectorSteps.size();
				_AddToByteArray(pByteArray, lStepNum);
				for (int j=0; j<lStepNum; j++)
				{
					_AddToByteArray(pByteArray, p->vectorLevels[i].vectorSteps[j].wCommandID);
					_AddToByteArray(pByteArray, p->vectorLevels[i].vectorSteps[j].bStringParam);
					if (p->vectorLevels[i].vectorSteps[j].bStringParam)
					{
						_AddToByteArray(pByteArray, (long)lstrlen(p->vectorLevels[i].vectorSteps[j].pstrParam));
						_AddToByteArray(pByteArray, p->vectorLevels[i].vectorSteps[j].pstrParam);
					}
					else
						_AddToByteArray(pByteArray, p->vectorLevels[i].vectorSteps[j].lParam);
				}
			}
		}
	}
	return true;
}

// 解码
bool CSkillList::DecordFromByteArray(BYTE* pByte, long& pos)
{
	Release();

	char strTemp[256];
	long lSkillNum = _GetLongFromByteArray(pByte, pos);
	for(int n=0; n<lSkillNum; n++)
	{
		tagSkill stSkill;
		stSkill.strName = _GetStringFromByteArray(pByte, pos, strTemp);
		stSkill.strDescribe = _GetStringFromByteArray(pByte, pos, strTemp);
		stSkill.dwIndex = _GetDwordFromByteArray(pByte, pos);
		stSkill.lDestType = (eSkillDestType)_GetLongFromByteArray(pByte, pos);

		long lLevelNum = _GetLongFromByteArray(pByte, pos);
		for (int i=0; i<lLevelNum; i++)
		{
			tagLevel stLevel;
			stSkill.vectorLevels.push_back(stLevel);

			stSkill.vectorLevels[i].wRange = _GetWordFromByteArray(pByte, pos);

			long lStepNum = _GetLongFromByteArray(pByte, pos);
			for (int j=0; j<lStepNum; j++)
			{
				tagStep stStep;
				stSkill.vectorLevels[i].vectorSteps.push_back(stStep);
				
				stSkill.vectorLevels[i].vectorSteps[j].wCommandID = _GetWordFromByteArray(pByte, pos);
				stSkill.vectorLevels[i].vectorSteps[j].bStringParam = _GetByteFromByteArray(pByte, pos);
				if (stSkill.vectorLevels[i].vectorSteps[j].bStringParam)
				{
					int lsize = _GetLongFromByteArray(pByte, pos);
					stSkill.vectorLevels[i].vectorSteps[j].pstrParam = new char[lsize+1];
					_GetStringFromByteArray(pByte, pos, stSkill.vectorLevels[i].vectorSteps[j].pstrParam);
				}
				else
					stSkill.vectorLevels[i].vectorSteps[j].lParam = _GetLongFromByteArray(pByte, pos);
			}
		}
		
		m_mapSkillList[stSkill.dwIndex] = stSkill;
	}

	return true;
}

// 返回指定的Skill属性
CSkillList::tagSkill* CSkillList::GetPropertiy(DWORD dwIndex)
{
	return m_mapSkillList.find(dwIndex)!=m_mapSkillList.end() ? &m_mapSkillList[dwIndex] : NULL;
}

CSkillList::tagSkill* CSkillList::GetPropertiyByName(const char* strName)
{
	for(map<DWORD, CSkillList::tagSkill>::iterator it=m_mapSkillList.begin(); it!=m_mapSkillList.end(); it++)
	{
		if( (*it).second.strName == strName )
		{
			return &(*it).second;
		}
	}
	return NULL;
}

// 设置指定Skill的属性
bool CSkillList::SetPropertiy(CSkill* pSkill, const char* strOrginName)
{
	return true;
}
