#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\public\GUID.h"

class CPlayer;
class CRegion;

// 变量结构
struct stVariable
{
	char *Name;		//名称
	long Array;		//数量 (=0) 普通变量 (>1) 数组型变量 (<0) 字符数组,即字符串
	union{
			double* Value;
			char* strValue;
	};
	stVariable(void)
	{
		memset(this, 0, sizeof(stVariable));
	}
};

// 变量列表类
class CVariableList :public BaseMemObj 
{
	vector<stVariable*> m_pVarList;		// 变量列表
public:
	typedef std::map<std::string, CGUID*> GuidList;
	
	enum eError
	{
		VAR_NOT_FOUND=-99999999,	// 该变量没有找到
	};

	CVariableList& operator=(CVariableList& right)
	{
		if(this == &right)
		{
			Release();
			GuidList::iterator guidItr = right.m_pGuidList.begin(); // GUID列表
			for(; guidItr != right.m_pGuidList.end(); guidItr++)
			{
				AddGuid(guidItr->first.c_str(), *guidItr->second);
			}

			vector<stVariable*>::iterator varItr = m_pVarList.begin();
			for(; varItr != m_pVarList.end(); varItr++)
			{
				stVariable* pVar = *varItr;
				if(pVar)
				{
					if(pVar->Array == 0)
						AddVar(pVar->Name, *pVar->Value);
					if(pVar->Array > 0)
					{
						AddVar(pVar->Name, pVar->Array, 0);
						for(int i=0; i<pVar->Array; i++)
							SetVarValue(pVar->Name, i, pVar->Value[i]);
					}
					else if(pVar->Array < 0)
						AddVar(pVar->Name, pVar->strValue);
				}
			}
		}
		return *this;
	};

	void IniVarNum(long lNum);		// 初始化变量个数

	void LoadVarList(char *file, char* index, const char* pData=NULL);	//读入变量列表

	void ReLoadVarList(char *file, char* index, const char* pData=NULL);	//重新读入变量列表
	
	void AddVar(const char* name, const char* value); //增加一个字符串变量
	void AddVar(const char* name,double value); //增加一个整型变量
	void AddVar(const char* name,long bound,double value); //增加一个整型数组变量
	bool AddVar(const char* name); //增加变量(根据name前缀自动判断类型)
	// [071116 AHC]
	void AddGuid(const char* name, const CGUID& guid); // 增加一个GUID变量

	bool RemoveVar(const char* name);//删除变量
	bool isExist(const char* name);	//检查变量是否存在

	// [071116 AHC]
	bool RemoveGuid(const char* name);//删除Guid变量
	bool isGuidExist(char* name);	//检查Guid变量是否存在

	long GetVarNum()		{ return m_pVarList.size(); }	// 获取变量个数
	void SetVarNum(long l)	{ m_pVarList.resize(l); }		// 设置变量个数

	int GetArrayNum(char *, CPlayer*, CRegion*);	// 获取数组变量的下标　( >0 数组，=0 简单 )
	void GetArrayName(char *, char *);				// 获取数组变量的名称
	double GetVarValue(const char *, int);					// 获取变量的值
	const char* GetVarValue(const char *);				// 获取变量的值(字符串)
	int SetVarValue(const char *, int=0, double=0);			// 设置变量的值
	int SetVarValue(const char *name, const char* value);		// 设置变量的值(字符串)
	
	CGUID GetGuidValue(const char *);				// 获取GUID的值
	int SetGuidValue(const char *, const CGUID*);				// 设置GUID的值

	char* GetVarName(int n)			{ return m_pVarList[n]->Name; }		// 获取变量的名字
	int GetVarArray(int n)			{ return m_pVarList[n]->Array; }		// 获取变量的下标值
	double GetVarValue(int n, int m=0)	{ return m_pVarList[n]->Value[m]; }	// 获取变量的值

	GuidList&	GetGuidList(void) { return m_pGuidList; }
public:
	CVariableList();
	virtual ~CVariableList();
	void Release();

	bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到vector<BYTE>
	bool DecordFromByteArray(BYTE* pByte, long& pos, long bufSize=-1);	// 解码
	bool DecodeFromDataBlock(DBReadSet& setReadDB);

	void LoadVarData(char *, char*);	//读入变量详细资料

	vector<stVariable*>& GetVarList()	{ return m_pVarList;}	// 返回变量列表的指针

	GuidList m_pGuidList; // GUID列表
};
