#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\public\GUID.h"

#define NAME_STR_NUM 256
#define VALUE_STR_NUM 5120

class CPlayer;
class CRegion;
class CMonster;

// 变量结构
struct stVariable : public BaseMemObj
{
    stVariable();
	~stVariable(void);

	long Array;		//数量 (=0) 普通变量 (>1) 数组型变量 (<0) 字符数组,即字符串
	union{
			double* Value;
			char* strValue;
	};
};


/*-------------------------------------------*/
// 提供给hash使用
class str_compare
{
public:
	inline bool operator( )( const string& Key1, const string& Key2 ) const
	{
		if(Key1 < Key2)
			return true;
		return false;
	}

};

class hash_str_compare:public hash_compare<string,str_compare>
{
private:
	str_compare	comp;
public:
	//计算散列值
	size_t operator( )( const string& Key ) const
	{
		unsigned long __h = 0;
		for (size_t i = 0 ; i < Key.size() ; i ++)
			__h = 5*__h + Key[i];
		return size_t(__h);

	}
	//less operator
	bool operator( )( const string& _Key1,const string& _Key2 ) const
	{
		return comp(_Key1,_Key2);
	}
};

// 变量列表类
class CVariableList : public BaseMemObj 
{
public:
	typedef hash_map<string,stVariable*, hash_str_compare> VarList;

    typedef VarList::iterator varlistitr;
    varlistitr VarListBegin(void) { return m_pVarList.begin(); }
    varlistitr VarListEnd(void)   { return m_pVarList.end();   }
    varlistitr VarListNext(void)   { return m_VarListItr++;   }
    long GetVarNum()		{ return m_pVarList.size(); }	// 获取变量个数

   
	typedef hash_map<string,CGUID*, hash_str_compare> GuidList;

private:
    VarList m_pVarList; // 变量列表
    varlistitr m_VarListItr;

    GuidList m_pGuidList; // GUID列表

public:
	enum eError
	{
		VAR_NOT_FOUND=-99999999,	// 该变量没有找到
	};

	void LoadVarList(char *file, char* index, const char* pData=NULL);	//读入变量列表

	void AddVar(const char* name,const char* value); //增加一个字符串变量
	void AddVar(const char* name,double value); //增加一个整型变量

    // [070528 AHC]
    void AddGuid(const char* name, const CGUID& guid); // 增加一个GUID变量

	void AddVar(const char* name,long bound,double value); //增加一个整型数组变量
	bool AddVar(const char* name); //增加变量(根据name前缀自动判断类型)

	bool RemoveVar(const char* name);//删除变量
	bool isExist(const char* name);	//检查变量是否存在

    // [070528 AHC]
    bool RemoveGuid(const char* name);//删除Guid变量
    bool isGuidExist(char* name);	//检查Guid变量是否存在

    // [070528 AHC]
    long GetGuidNum()		{ return m_pGuidList.size(); }	// 获取Guid变量个数

	int GetArrayNum(const char *, CPlayer*, CRegion*, CMonster* pNpc=NULL, CGUID& guUsedItemID=CGUID::GUID_INVALID, DWORD dwDiedMonsterIndex=0);	// 获取数组变量的下标　( >0 数组，=0 简单 )
	void GetArrayName(const char *, char *);		// 获取数组变量的名称
	
    double GetVarValue(const char *, int);				// 获取变量的值
	const char* GetVarValue(const char *);			// 获取变量的值(字符串)

    const CGUID &GetGuidValue(const char *);				// 获取GUID的值
    int SetGuidValue(const char *, const CGUID*);				// 设置GUID的值

	int SetVarValue(const char *, int=0, double=0);	// 设置变量的值
	int SetVarValue(const char *name,const char* value);	// 设置变量的值(字符串)

	GuidList* GetGuidList(void) { return &m_pGuidList; }
public:
	CVariableList();
	virtual ~CVariableList();
	void Release();

	bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到vector<BYTE>
	bool DecordFromByteArray(BYTE* pByte, long& pos, bool isPlayerVar=true, long bufSize=-1);	// 解码

	bool CodeToDataBlock(DBWriteSet& setWriteDB);
	bool DecodeFromDataBlock(DBReadSet& setReadDB,bool isPlayerVar=true);

	void LoadVarData(char *, char*);	//读入变量详细资料
	void SaveVarData(char *, char*);	//写入变量详细资料

	VarList* GetVarList()	{ return &m_pVarList;}	// 返回变量列表的指针
	void SetVarList(const VarList* VarList);
	void UpdateVarList(const VarList* VarList);	//更新列表中的变量

	void ReleaseGuidList(void);
	void SetGuidList(const GuidList* guidList);
};
