#include "StdAfx.h"
#include "variablelist.h"
#include "script.h"

//#include "vld.h"


// stVariable
stVariable::stVariable()
: Array(0),Value(NULL)
{
}

stVariable::~stVariable(void)
{
	if( Array==0 )
	{
		M_FREE(Value, sizeof(double));
	}
	else if( Array>0 )
	{
		M_FREE(Value, sizeof(double)*Array);
	}
	else if( Array<0 )
	{
		M_FREE(strValue, sizeof(char)*abs(Array));
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVariableList::CVariableList()
{
	
}

CVariableList::~CVariableList()
{
	Release();
}

void CVariableList::Release()
{
	if( m_pVarList.size() == 0 && m_pGuidList.size() == 0 ) return;

    varlistitr itr = m_pVarList.begin();
	for(; itr != m_pVarList.end(); itr++)
	{
        MP_DELETE(itr->second);
	}
	m_pVarList.clear();
   
	ReleaseGuidList();
}

/////////////////////////////
// 读取变量列表
void CVariableList::LoadVarList(char *filename, char *index, const char* pData)
{
	Release();

	CIni Ini(filename, pData);
    int varNum = 0;
	varNum=Ini.GetContinueDataNum(index);

	for(int i=0; i<varNum; i++)
	{
		//获得变量名字
		char *tmp=Ini.ReadCaption(index, i);

        stVariable* tv = MP_NEW stVariable;
              
        tv->Array = GetArrayNum( tmp, NULL, NULL );	//获取数组下标

        if( tv->Array > 0 )	//数组型变量
		{
			//去掉数组变量下标
			GetArrayName( tmp, tmp );

			tv->Value=(double*)M_ALLOC(sizeof(double)*tv->Array);

			int x=Ini.ReadInt(index, i);
			for(int j=0; j<tv->Array; j++)
			{
				tv->Value[j] = x;
			}
		}
		else if( tv->Array==0 )	//简单变量
		{
			char* pValue;
			pValue=Ini.ReadText(index,i);
			if (*pValue=='\"'){	//字符串
				int len=lstrlen(pValue)-2+1;//去掉前后引号,加上结束符0

				tv->Array=-len;	//负数表示类型为字符串

				tv->strValue=(char*)M_ALLOC(sizeof(char)*len);
				memcpy(tv->strValue,pValue+1,len-1);
				//字符串结束符
				tv->strValue[len-1]=0;
			}
			else				//数字
			{
				if(tv->Value == NULL)
					tv->Value = (double*)M_ALLOC(sizeof(double));

				*tv->Value=atof(pValue);
			}
			SAFE_DELETE(pValue);

		}
		else	//出错
		{
			assert(0);
			return;
		}

        m_pVarList[string(tmp)] = tv;
        SAFE_DELETE_ARRAY(tmp);
	}
}
bool CVariableList::AddVar(const char* name)
{
	if (*name=='$')
		AddVar(name,0.0f);
	else if(*name=='#')
		AddVar(name,"");
    else if(*name=='@') // GUID
        AddGuid(name, NULL_GUID);
	else
		return false;

	return true;
}

void CVariableList::AddVar(const char* name,double value)
{
	if (name == NULL)
	{
		return;
	}

	//先检查变量是不是已经存在
    if(m_pVarList.find( string(name) ) != m_pVarList.end())
	{
		//如果存在
        SetVarValue(name,0,value);	//直接赋值退出
        return;
	}

	stVariable* pVarList = MP_NEW stVariable;		// 变量列表

	//创建变量
	pVarList->Array=0;	//0表示类型为整型
	if(pVarList->Value == NULL)
		pVarList->Value = (double*)M_ALLOC(sizeof(double));

	*pVarList->Value=value;

	m_pVarList[string(name)] = pVarList;
}

void CVariableList::AddVar(const char* name,const char* value)
{
	if (name == NULL)
	{
		return;
	}
	//先检查变量是不是已经存在
	if(m_pVarList.find( string(name) ) != m_pVarList.end())
	{
		//如果存在
        SetVarValue(name,value);	//直接赋值退出
        return;
	}

	stVariable* pVarList = MP_NEW stVariable;		// 变量列表

	int len=lstrlen(value)+1;

	pVarList->Array=-len;	//负数表示类型为字符串

	pVarList->strValue=(char*)M_ALLOC(sizeof(char)*len);
	memcpy(pVarList->strValue,value,len);
	
    //pVarList->strValue[len-1]=0;
    //pVarList->strSValue[len-1]=0;

	m_pVarList[string(name)] = pVarList;
}

// [070528 AHC]
void CVariableList::AddGuid(const char* name, const CGUID& guid) // 增加一个GUID变量
{
	if (name == NULL)
	{
		return;
	}

    //先检查变量是不是已经存在
    string guidName = name;
    if(m_pGuidList.find( guidName ) != m_pGuidList.end())
    {
        //如果存在
        SetGuidValue(name,&guid);	//直接赋值退出
        return;
    }

    CGUID* pGuidList = OBJ_CREATE(CGUID);		// 变量列表

    //拷贝赋值创建变量
    *pGuidList = guid;
    m_pGuidList[guidName] = pGuidList;
}

void CVariableList::AddVar(const char* name,long bound,double value) //增加一个整型数组变量
{ 
	if (name == NULL)
	{
		return;
	}

	//先检查变量是不是已经存在
	if(isExist(name))
    {
		return;
	}

	//bound只能大于0
	if (bound<=0)
		return;


	stVariable* pVarList = MP_NEW stVariable;		// 变量列表

	//创建变量

	pVarList->Array=bound;

	pVarList->Value=(double*)M_ALLOC(sizeof(double)*bound);

	for(int i=0; i<bound; i++)
	{
		pVarList->Value[i]=value;
	}

	m_pVarList[string(name)] = pVarList;

}

void CVariableList::SetVarList(const VarList* VarList)
{
	if( VarList==NULL )
		return;

	Release();
	
	VarList::const_iterator itr = VarList->begin();
    for(; itr != VarList->end(); itr++)
	{
		long lLen = lstrlen( itr->first.c_str() );

		if(NULL == m_pVarList[itr->first])
		{
			m_pVarList[itr->first] = MP_NEW stVariable;
		}

		m_pVarList[itr->first]->Array = itr->second->Array;
		lLen = itr->second->Array;
		if(lLen == 0)
		{
			m_pVarList[itr->first]->Value = (double*)M_ALLOC(sizeof(double));
			*(m_pVarList[itr->first]->Value) = *itr->second->Value;
		}
		else if( lLen > 0 )	//数组
		{ 
			m_pVarList[itr->first]->Value = (double*)M_ALLOC(sizeof(double)*lLen);
			memcpy(m_pVarList[itr->first]->Value,  itr->second->Value,  sizeof(double)*lLen);
		}
		else if(lLen < 0)
		{	
			//字符串
			lLen = lstrlen( itr->second->strValue );
			m_pVarList[itr->first]->strValue = (char*)M_ALLOC(sizeof(char)*(lLen+1));
			strcpy_s( m_pVarList[itr->first]->strValue, lLen+1, itr->second->strValue );
			m_pVarList[itr->first]->strValue[lLen]='\0';
		}
	}
}

//更新列表中的变量
void CVariableList::UpdateVarList(const VarList* VarList)
{
	if( VarList==NULL )
		return;

    VarList::const_iterator itr = VarList->begin();
    for(; itr != VarList->end(); itr++)
	{
		long lLen = itr->second->Array;

		if(lLen == 0)	//简单整数型
		{
			AddVar((char*)itr->first.c_str(), *itr->second->Value);
		}
		else if( lLen > 0 )	//数组
		{ 
			//缺省
			AddVar((char*)itr->first.c_str(), (long)lLen, *itr->second->Value);
		}
		else if(lLen < 0) //字符串
        {	
			AddVar((char*)itr->first.c_str(), itr->second->strValue);
		}
	}
}
void CVariableList::ReleaseGuidList(void)
{
	GuidList::iterator gitr = m_pGuidList.begin();
	for(; gitr != m_pGuidList.end(); gitr++)
	{
		OBJ_RELEASE(CGUID, gitr->second );
	}
	m_pGuidList.clear();
}
void CVariableList::SetGuidList(const GuidList* guidList)
{
	if( guidList==NULL )
		return;

	ReleaseGuidList();

	GuidList::const_iterator itr = guidList->begin();
	for(; itr != guidList->end(); itr++)
	{
		CGUID* pGuid = OBJ_CREATE(CGUID);		// 变量列表
		if(pGuid)
		{
			//拷贝赋值创建变量
			*pGuid = *(itr->second);
			m_pGuidList[itr->first] = pGuid;
		}
	}
}
bool CVariableList::isExist(const char* name)
{
	if (name == NULL)
	{
		return false;
	}

	if(m_pVarList.find( string(name) ) != m_pVarList.end())
    {
		//如果存在
        return true;
	}
	return false;
}
bool CVariableList::isGuidExist(char* name)
{
	if (name == NULL)
	{
		return false;
	}

    if(m_pGuidList.find( string(name) ) != m_pGuidList.end())
    {	
        //如果存在
        return true;
    }

    return false;
}

bool CVariableList::RemoveVar(const char* name)
{
	if (name == NULL)
	{
		return false;
	}

	varlistitr it = m_pVarList.find( name );
	if( it != m_pVarList.end() )
	{
		stVariable *var = it->second;
		if( var->Array == 0 )
		{
			M_FREE( var->Value, sizeof(double) );
		}
		else if(var->Array < 0)
		{
			M_FREE( var->strValue, sizeof(char)*abs(var->Array) );
		}
		else if(var->Array > 0)
		{
			M_FREE( var->Value, sizeof(double)*var->Array );
		}

		MP_DELETE(var);
		m_pVarList.erase( it );
		return true;
	}
	
	return false;
}

bool CVariableList::RemoveGuid(const char* name)//删除Guid变量
{
	if (name == NULL)
	{
		return false;
	}

	GuidList::iterator it = m_pGuidList.find( name );
	if( it != m_pGuidList.end() )
	{
		OBJ_RELEASE(CGUID, it->second );
		m_pGuidList.erase( it );
		return true;
	}

    return false;
}
//////////////////////////

/////////////////////////////
// 读取变量详细资料
void CVariableList::LoadVarData(char* file, char* index)
{
	CIni Ini(file);
	int num=Ini.GetContinueDataNum(index);

	for(int i=0; i<num; i++)
	{
		char* strCaption = Ini.ReadCaption(index, i);
		int array = GetArrayNum(strCaption, NULL, NULL);	// 取变量下标
		char name[NAME_STR_NUM];
		GetArrayName(strCaption, name);		// 取变量名称
		SetVarValue(name, array, Ini.ReadInt(index, i));
		SAFE_DELETE_ARRAY( strCaption );
	}
}

/////////////////////////////
// 写入变量详细资料
//TODO:加入字符串类型支持(m_pVarList[i].Array<0)
void CVariableList::SaveVarData(char* file, char* index)
{
	CIni Ini(file);
    hash_map<string, stVariable*>::iterator itr = m_pVarList.begin();
    for(; itr != m_pVarList.end(); itr++)
	{
		// 简单变量
		if( itr->second->Array == 0 )
		{
			Ini.Write(index, (char*)itr->first.c_str(), (int)(itr->second->Value));
		}
		else if( itr->second->Array > 0)	// 数组变量
		{
			for(int j=0; j<itr->second->Array; j++)
			{
				char str[NAME_STR_NUM];
				_snprintf(str, 256, "%s[%d]", itr->first.c_str(), j);
				Ini.Write(index, str, itr->second->Value[j]);
			}
		}
		else{	//字符串变量
		
		}

	}
	Ini.Save();
}

/////////////////////////////
// 获取数组变量的下标
int CVariableList::GetArrayNum(const char *name, CPlayer* pPlayer, CRegion* pRegion, CMonster* pNpc, CGUID& guUsedItemID, DWORD dwDiedMonsterIndex)
{
	int n=0;
	char temp[NAME_STR_NUM];

	unsigned int len = (unsigned int)strlen(name);
	for(unsigned int i=0; i<len; i++)
	{
		if( name[i]=='[' )
		{
			i++;
			while( name[i] != ']' )
			{
				temp[n]=name[i];
				n++;
				i++;
			}

			temp[n]=';';
			temp[n+1] = '\0';
			CScript SubScript((CShape*)pPlayer, pRegion, (CShape*)pNpc, guUsedItemID, dwDiedMonsterIndex);
			SubScript.RunLine(temp);
			return SubScript.GetValue(0);
		}
	}

	return 0;
}

////////////////////////////
// 获取数组变量的名字
void CVariableList::GetArrayName(const char *name, char *ret)
{
	strcpy(ret, name);

	//去掉数组变量下标
	int len = (int)strlen(ret)-1;
	for(int k=len; k>=0; k--)
	{
		if( ret[k] == '[' )
		{
			ret[k]=0;
			break;
		}
	}
}

///////////////////////
// 获取变量的值
double CVariableList::GetVarValue(const char *name, int num)
{
	if (name == NULL)
	{
		return 0.0f;
	}

    string varName = name;
    varlistitr itr = m_pVarList.find( varName );
    if( itr != m_pVarList.end() )
	{
        if( itr->second->Array == 0 && num==0 ) // 普通变量,整型变量
        {
            return *itr->second->Value;
        }
        else if( itr->second->Array <= num || num < 0)	//下标越界或者类型不匹配(期望整型,实际为字符串)
        {
			//ShowMessage("'%s' array out of range!", name);
			assert(0);
			return (double)VAR_NOT_FOUND;
		}
		else	//数组
		{
			return (double)itr->second->Value[num];
		}
	}

	return (double)VAR_NOT_FOUND;
}

const CGUID &CVariableList::GetGuidValue(const char *name)				// 获取GUID的值
{
	if(name == NULL) return NULL_GUID;

    string guidName = name;
    hash_map<string, CGUID*>::iterator itr = m_pGuidList.find( guidName );
    if( itr != m_pGuidList.end() )
    {
        return *(itr->second);
    }

    return CGUID::GUID_INVALID;
}
///////////////////////
// 获取变量的值(字符串)
const char* CVariableList::GetVarValue(const char *name)
{
	if (name == NULL)
	{
		return NULL;
	}

    string varName = name;
    hash_map<string, stVariable*>::iterator itr = m_pVarList.find( varName );
    if( itr != m_pVarList.end() )
	{
		if( itr->second->Array < 0) // 是字符串
				return itr->second->strValue;
	}

	return NULL;
}
//////////////////////////
// 设置变量的值（变量名，数组下标，值）
int CVariableList::SetVarValue(const char *name, int num, double value)
{
	if (name == NULL)
	{
		return 0;
	}

    string varName = name;
    hash_map<string, stVariable*>::iterator itr = m_pVarList.find( varName );
    if( itr != m_pVarList.end() )
    {
        if( itr->second->Array > num )	//数组
        {
            itr->second->Value[num]=value;
            return 1;
        }
        else if(itr->second->Array == 0)
        {
			if(itr->second->Value == NULL)
			{
				itr->second->Value= (double*)M_ALLOC(sizeof(double));
			}

            *itr->second->Value=value;
            return 1;
        }
    }

	return VAR_NOT_FOUND;
}
//////////////////////////
// 设置变量的值（变量名，值）
int CVariableList::SetVarValue(const char *name,const char* value)
{
	if (name == NULL)	//如果是NULL,则默认为空串.
	{
		return -1;
	}
	if(NULL==value) 
		value="";

    string varName = name;
    hash_map<string, stVariable*>::iterator itr = m_pVarList.find( varName );
    if( itr != m_pVarList.end() )
    {
        int len=lstrlen(value)+1;	//新字符串长度,要考虑末尾的结束符0多占一个字节
        //重新强制分配,分配效率由内存池保证
        if ( itr->second->Array < 0 ) //原来该变量是字符串
        {	
			long lOldSize = abs( itr->second->Array );
            //新开缓存
			M_FREE(itr->second->strValue, sizeof(char)*lOldSize);
			itr->second->strValue = (char*)M_ALLOC(sizeof(char)*len);
        }
        else if( itr->second->Array > 0 )  //原来变量是数组
        {	
			long lOldSize = (itr->second->Array * (double)sizeof(double));
            M_FREE(itr->second->Value, lOldSize);
			itr->second->strValue=(char*)M_ALLOC(sizeof(char)*len);
        }
        else //原来变量是整数
        {	
            M_FREE(itr->second->strValue, sizeof(double));
            itr->second->strValue=(char*)M_ALLOC(sizeof(char)*len);
        }

        //复制字符串
        strcpy(itr->second->strValue,value);
        itr->second->Array=-len;
        return 1;
	}
	
	return VAR_NOT_FOUND;
}

int CVariableList::SetGuidValue(const char *name, const CGUID* value)				// 设置GUID的值
{
	if (name == NULL)
	{
		return 0;
	}
    string guidName = name;
    hash_map<string, CGUID*>::iterator itr = m_pGuidList.find( guidName );
    if( itr != m_pGuidList.end() )
    {
        *itr->second = *value; // 拷贝赋值
        return 1;
    }

    return VAR_NOT_FOUND;
}

// 添加到vector<BYTE>
//TODO:加入字符串类型支持(m_pVarList[i].Array<0)
bool CVariableList::AddToByteArray( vector<BYTE>* pByteArray )
{
	_AddToByteArray( pByteArray, (long)m_pVarList.size() );				// 变量个数	

	vector<BYTE> vectorTemp;
    hash_map<string, stVariable*>::iterator itr = m_pVarList.begin();
    for(; itr != m_pVarList.end(); itr++)
	{
		_AddToByteArray( &vectorTemp, itr->first.c_str() );	// 名字
		_AddToByteArray( &vectorTemp, itr->second->Array );
		if( itr->second->Array == 0 )		// 个体变量
		{
			_AddToByteArray(&vectorTemp, *itr->second->Value);
		}
		else if( itr->second->Array > 0)	// 数组
		{
			for(int j=0; j<itr->second->Array; j++)
			{
				_AddToByteArray(&vectorTemp, itr->second->Value[j]);
			}
		}
		else{		//字符串
			_AddToByteArray(&vectorTemp, itr->second->strValue);
		}
	}

	// GUID
	_AddToByteArray( pByteArray, (long)m_pGuidList.size() );		// 变量个数
	GuidList::iterator gitr = m_pGuidList.begin();
	for(; gitr != m_pGuidList.end(); gitr++)
	{		
		_AddToByteArray( &vectorTemp, gitr->first.c_str() );	// 名字	
		_AddToByteArray(&vectorTemp, *(gitr->second));				// GUID变量
	}

	_AddToByteArray(pByteArray, (long)vectorTemp.size());
	if(vectorTemp.size())
		_AddToByteArray(pByteArray, &vectorTemp[0], (long)vectorTemp.size());

	return true;
}

// 解码
//TODO:加入字符串类型支持(m_pVarList[i].Array<0)
bool CVariableList::DecordFromByteArray( BYTE* pByte, long& pos, bool isPlayerVar, long bufSize )
{
	char strName[NAME_STR_NUM];
	char strValue[VALUE_STR_NUM]="";
	long lVarNum;
	long lDataSize;
	long lArray;
	
	if(isPlayerVar)
		LoadVarList("data/variable.ini", "VariableList", GetGame()->GetVariableList_FileData());

	//普通变量个数
	lVarNum = _GetLongFromByteArray(pByte,pos,bufSize);  
	//GUID个数
	int num = _GetLongFromByteArray( pByte, pos,bufSize );		// 变量个数

	lDataSize=_GetLongFromByteArray(pByte,pos,bufSize);

	double value = 0;

	for(int i=0; i<lVarNum; i++)
	{
		_GetStringFromByteArray(pByte, pos, strName,bufSize);
        lArray = _GetLongFromByteArray(pByte,pos,bufSize);
		if( lArray == 0 )	// 个体变量
		{
			value = _GetDoubleFromByteArray(pByte,pos,bufSize);
			AddVar(strName, value);
		}
		else if(lArray > 0)	// 数组
		{
			AddVar(strName, lArray, 0.0f);

			for(int j=0; j<lArray; j++)
			{
				value = _GetDoubleFromByteArray(pByte,pos,bufSize);
				SetVarValue(strName, j, value);
			}
		}
		else
        {		//字符串
			_GetStringFromByteArray(pByte, pos, strValue,bufSize);
			AddVar(strName,strValue);			
		}
	}

	// GUID
	char GuidName[NAME_STR_NUM];
	CGUID tGuid;
	for(int i=0; i<num; i++)
	{
		_GetStringFromByteArray( pByte, pos, GuidName,bufSize );	// 名字	
		_GetBufferFromByteArray(pByte, pos, tGuid,bufSize);	// GUID变量
		AddGuid(GuidName, tGuid);
	}


	return true;
}

bool CVariableList::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray( (long)m_pVarList.size() );				// 变量个数	

	vector<BYTE> vectorTemp;
    hash_map<string, stVariable*>::iterator itr = m_pVarList.begin();
    for(; itr != m_pVarList.end(); itr++)
	{
		_AddToByteArray( &vectorTemp, itr->first.c_str() );	// 名字
		_AddToByteArray( &vectorTemp, itr->second->Array );
		if( itr->second->Array == 0 )		// 个体变量
		{
			_AddToByteArray(&vectorTemp, *itr->second->Value);
		}
		else if( itr->second->Array > 0)	// 数组
		{
			for(int j=0; j<itr->second->Array; j++)
			{
				_AddToByteArray(&vectorTemp, itr->second->Value[j]);
			}
		}
		else{		//字符串
			_AddToByteArray(&vectorTemp, itr->second->strValue);
		}
	}

	// GUID
	setWriteDB.AddToByteArray( (long)m_pGuidList.size() );		// 变量个数
	GuidList::iterator gitr = m_pGuidList.begin();
	for(; gitr != m_pGuidList.end(); gitr++)
	{		
		_AddToByteArray( &vectorTemp, gitr->first.c_str() );	// 名字	
		_AddToByteArray(&vectorTemp, *(gitr->second));				// GUID变量
	}

	setWriteDB.AddToByteArray((long)vectorTemp.size());
	if(vectorTemp.size())
		setWriteDB.AddToByteArray(&vectorTemp[0], (long)vectorTemp.size());

	return true;
}
bool CVariableList::DecodeFromDataBlock(DBReadSet& setReadDB,bool isPlayerVar)
{
	char strName[NAME_STR_NUM];
	char strValue[VALUE_STR_NUM]="";
	long lVarNum;
	long lDataSize;
	long lArray;
	
	if(isPlayerVar)
		LoadVarList("data/variable.ini", "VariableList", GetGame()->GetVariableList_FileData());

	//普通变量个数
	lVarNum = setReadDB.GetLongFromByteArray();  
	//GUID个数
	int num = setReadDB.GetLongFromByteArray( );		// 变量个数

	lDataSize=setReadDB.GetLongFromByteArray();

	double value = 0;

	for(int i=0; i<lVarNum; i++)
	{
		setReadDB.GetStringFromByteArray(strName,NAME_STR_NUM);
        lArray = setReadDB.GetLongFromByteArray();
		if( lArray == 0 )	// 个体变量
		{
			value = setReadDB.GetDoubleFromByteArray();
			AddVar(strName, value);
		}
		else if(lArray > 0)	// 数组
		{
			AddVar(strName, lArray, 0.0f);

			for(int j=0; j<lArray; j++)
			{
				value = setReadDB.GetDoubleFromByteArray();
				SetVarValue(strName, j, value);
			}
		}
		else
        {		//字符串
			setReadDB.GetStringFromByteArray(strValue,VALUE_STR_NUM);
			AddVar(strName,strValue);			
		}
	}

	// GUID
	char GuidName[NAME_STR_NUM];
	CGUID tGuid;
	for(int i=0; i<num; i++)
	{
		setReadDB.GetStringFromByteArray( GuidName,NAME_STR_NUM );	// 名字	
		setReadDB.GetBufferFromByteArray( tGuid);	// GUID变量
		AddGuid(GuidName, tGuid);
	}
	return true;
}
