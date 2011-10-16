#include "StdAfx.h"
#include "variablelist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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
	if(m_pVarList.size() == 0) return;

	for(int i=0; i<m_pVarList.size(); i++)
	{
		SAFE_DELETE( m_pVarList[i]->Name );
		if( m_pVarList[i]->Array == 0 )	//整数
		{
			SAFE_DELETE( m_pVarList[i]->Value );
		}
		else
		{	
			//字符串
			SAFE_DELETE_ARRAY(m_pVarList[i]->strValue);
		}
		OBJ_RELEASE(stVariable, m_pVarList[i]);
	}
	m_pVarList.clear();

	GuidList::iterator guidItr = m_pGuidList.begin(); // GUID列表
	for(; guidItr != m_pGuidList.end(); guidItr++)
		delete guidItr->second;
	m_pGuidList.clear();
}

// 初始化变量个数
void CVariableList::IniVarNum(long lNum)
{
	Release();
	m_pVarList.resize(m_pVarList.size());
}

/////////////////////////////
//重新读入变量列表
void CVariableList::ReLoadVarList(char *filename, char *index, const char* pData)
{
	if(!filename || !index) return;

	CIni Ini(filename, pData);
	long lVarNum=Ini.GetContinueDataNum(index);

	for(int i=0; i<lVarNum; i++)
	{
		stVariable* pTmpVar = OBJ_CREATE(stVariable);
		memset(pTmpVar,0,sizeof(stVariable)); 

		//获得变量名字
		char *tmp=Ini.ReadCaption(index, i);

		// [070919]
		if(tmp[0] == '#') // 字符串
		{
			// 求下标变量的值
			char* ptr = tmp;

			int lArray = 0;
			char arrSize[128];
			memset(arrSize, 0, sizeof(arrSize));
			bool isX = false; // 当前字符是否'['
			int n = 0; // 下标字符串计数
			bool hasFindBlank = false; // 是否找到'['
			int blankPos = 0; // '['的位置
			while(*ptr != 0)
			{
				if(*ptr == '[')
				{
					isX = true;
					hasFindBlank = true;
				}
				if(*ptr == ']')
					isX = false;

				if( isX && *ptr != '[' && *ptr != ' ' && *ptr != '\t' )
				{
					arrSize[n] = *ptr;
					n++;
				}

				if(!hasFindBlank)
					blankPos++;
				ptr++;
			}

			// 改'[' ']'变量名为值
			if(hasFindBlank) // 字符串数组
			{
				if(arrSize)
				{
					long sizet = blankPos + strlen(arrSize) + 1;
					pTmpVar->Name = new char[sizet];
					memset(pTmpVar->Name, 0, sizet);
					memcpy_s(pTmpVar->Name, blankPos, tmp, blankPos );
					strcat_s(pTmpVar->Name, sizet, arrSize);
				}
			}
			else
			{
				long lTmpSize = strlen(tmp);
				pTmpVar->Name = new char[lTmpSize+1];
				strcpy_s( pTmpVar->Name, lTmpSize+1, tmp);
			}
		}
		else 
		{
			long lTmpSize = strlen(tmp);
			pTmpVar->Name = new char[lTmpSize+1];
			strcpy_s( pTmpVar->Name, lTmpSize+1, tmp);
		}

		pTmpVar->Array=GetArrayNum( pTmpVar->Name, NULL, NULL );	//获取数组下标
		if( pTmpVar->Array > 0 )	//数组型变量
		{
			//去掉数组变量下标
			GetArrayName( pTmpVar->Name, pTmpVar->Name );

			pTmpVar->Value=new double[ pTmpVar->Array ];
			int x=Ini.ReadInt(index, i);
			for(int j=0; j<pTmpVar->Array; j++)
			{
				pTmpVar->Value[j]=x;
			}
		}
		else if( pTmpVar->Array==0 )	//简单变量
		{
			char* pValue;
			pValue=Ini.ReadText(index,i);
			if (*pValue=='\"'){	//字符串
				int len=lstrlen(pValue)-2+1;//去掉前后引号,加上结束符0

				pTmpVar->Array=-len;	//负数表示类型为字符串

				pTmpVar->strValue=new char[len];
				memcpy(pTmpVar->strValue,pValue+1,len-1);
				//字符串结束符
				pTmpVar->strValue[len-1]=0;
			}
			else				//数字
			{
				if(pTmpVar->Value == NULL)
					pTmpVar->Value = new double;

				*pTmpVar->Value=atof(pValue);//Ini.ReadInt(index, i);
			}
			SAFE_DELETE_ARRAY(pValue);
		}
		else	//出错
		{
			assert(0);
			return;
		}

		// 原有数据查找
		bool isFindVarName = false;
		for(int j=0; j<m_pVarList.size(); j++)
		{
			if( strcmp(m_pVarList[i]->Name, pTmpVar->Name) == 0) // 找到有该名字变量
			{
				isFindVarName = true;
				break;
			}
		}

		if(isFindVarName == false) // 未找到已有变量名
		{
			m_pVarList.push_back(pTmpVar);
		}
		else
			SAFE_DELETE(pTmpVar);

		SAFE_DELETE_ARRAY(tmp);
	}
}
// 读取变量列表
void CVariableList::LoadVarList(char *filename, char *index, const char* pData)
{
	Release();

	if( !filename || !index ) return;

	CIni Ini(filename, pData);
	long lTotalVarNum = Ini.GetContinueDataNum(index);
	
	for(long i=0; i<lTotalVarNum; i++)
	{
		//获得变量名字
		char *tmp = Ini.ReadCaption(index, i);
		if( !tmp ) continue;

		// guid变量,不处理数组类型定义
		if( tmp[0] == '@' ) 
		{
			CGUID* tmpGuid = new CGUID();
			m_pGuidList[std::string(tmp)] = tmpGuid;
			SAFE_DELETE_ARRAY(tmp);
			continue;
		}

		stVariable* pVar = OBJ_CREATE(stVariable);
		// 保证ini解析出错也将变量保存到m_pVarList避免内存泄漏
		if( !pVar )
		{
			SAFE_DELETE_ARRAY(tmp);
			continue;
		}
		m_pVarList.push_back(pVar);

		// [070919]
		if( tmp[0] == '#' ) // 字符串
		{
			// 求下标变量的值
			char* ptr   = tmp;
			long lArray = 0;
			char arrSize[128] = {0};
			bool isX = false; // 当前字符是否'['
			long n   = 0; // 下标字符串计数
			bool hasFindBlank = false; // 是否找到'['
			int blankPos      = 0; // '['的位置
			while( *ptr != 0 )
			{
				if( *ptr == '[' )
				{
					isX          = true;
					hasFindBlank = true;
				}
				if( *ptr == ']' )
					isX          = false;

				if( isX && *ptr != '[' && *ptr != ' ' && *ptr != '\t' )
				{
					arrSize[n] = *ptr;
					n++;
				}

				if( !hasFindBlank )
					blankPos++;
				ptr++;
			}

			// 改'[' ']'变量名为值
			if( hasFindBlank ) // 字符串数组
			{
				if( arrSize )
				{
					long sizet = blankPos + strlen(arrSize) + 1;
					pVar->Name = new char[sizet];
					memset(pVar->Name, 0, sizet);
					memcpy_s(pVar->Name, blankPos, tmp, blankPos);
					strcat_s(pVar->Name, sizet, arrSize);
				}
			}
			else
			{
				long lTmpSize = strlen(tmp);
				pVar->Name    = new char[lTmpSize+1];
				strcpy_s( pVar->Name, lTmpSize+1, tmp);
			}
		}
		else if( tmp[0] == '$' ) // 数值型
		{
			long lTmpSize = strlen(tmp);
			pVar->Name    = new char[lTmpSize+1];
			strcpy_s(pVar->Name, lTmpSize+1, tmp);
		}
		SAFE_DELETE_ARRAY(tmp);
	
		pVar->Array = GetArrayNum(pVar->Name, NULL, NULL); //获取数组下标
		if( pVar->Array > 0 )	//数组型变量
		{
			//去掉数组变量下标
			GetArrayName(pVar->Name, pVar->Name);
			pVar->Value = new double[pVar->Array];
			long vlaue  = Ini.ReadInt(index, i);
			for( long j=0; j<pVar->Array; j++ )
			{
				pVar->Value[j] = vlaue;
			}
		}
		else if( pVar->Array==0 ) //简单变量
		{
			char* pValue = NULL;
			pValue = Ini.ReadText(index,i);
			if( pValue && pValue[0]=='\"' ) //字符串
			{	
				int len        = lstrlen(pValue)-2+1; //去掉前后引号,加上结束符0
				pVar->Array    = -len;	//负数表示类型为字符串
				pVar->strValue = new char[len];
				memcpy(pVar->strValue, pValue+1, len-1);
				pVar->strValue[len-1] = 0; //字符串结束符
			}
			else //数字
			{
				if( NULL == pVar->Value )
					pVar->Value = new double;
				*(pVar->Value) = atof(pValue);
			}
			SAFE_DELETE_ARRAY(pValue);
		}
		else //出错
		{
			assert(0);
			AddLogText("数组变量 %s 下标小于０！", pVar->Name);
			return;
		}
	}
}
bool CVariableList::AddVar(const char* name)
{
	if(!name) return false;

	if (*name=='$')
		AddVar(name,0.0f);
	else if(*name=='#')
		AddVar(name,"");
	else
		return false;

	return true;

}
void CVariableList::AddVar(const char* name, double value)
{
	if(!name) return;

	//先检查变量是不是已经存在
	for (int i=0;i<m_pVarList.size();i++)
	{
		if (strcmp(m_pVarList[i]->Name,name)==0)
		{	
			//如果存在
			SetVarValue(name,0,value);	
			//直接赋值退出
			return;
		}
	}

	stVariable* pVarList = OBJ_CREATE(stVariable);
	
	//创建变量名
	long nameSize = strlen(name);
	pVarList->Name = new char[nameSize+1];
	strcpy( pVarList->Name, name);

	pVarList->Array=0;	//0表示类型为整型


	if(pVarList->Value == NULL)
		pVarList->Value = new double;

	*pVarList->Value=value;

	m_pVarList.push_back(pVarList);
}
void CVariableList::AddVar(const char* name, const char* value)
{
	if(!name || !value) return;

	//先检查变量是不是已经存在
	for (int i=0;i<m_pVarList.size();i++)
	{
		if (strcmp(m_pVarList[i]->Name,name)==0)
		{	
			//如果存在
			SetVarValue(name,value);	
			//直接赋值退出
			return;
		}
	}

	// 变量列表
	stVariable* pVarList = OBJ_CREATE(stVariable);
	
	//创建变量名
	long nameSize = strlen(name);
	pVarList->Name = new char[nameSize+1];
	strcpy( pVarList->Name, name);
	int len=lstrlen(value)+1;
	pVarList->Array=-len;	//负数表示类型为字符串
	pVarList->strValue=new char[len];

	memcpy(pVarList->strValue,value,len);

		
	m_pVarList.push_back(pVarList);
}

void CVariableList::AddVar(const char* name, long bound, double value)
{
	if(!name) return;

	//先检查变量是不是已经存在
	if(isExist(name))
	{
		return;
	}
	//bound只能大于0
	if (bound<=0)
		return;

	stVariable* pVarList = OBJ_CREATE(stVariable);
	long nameSize = strlen(name);
	pVarList->Name = new char[nameSize+1];
	strcpy( pVarList->Name, name);

	pVarList->Array=bound;

	pVarList->Value=new double[ bound ];


	for(int i=0; i<bound; i++)
	{
		pVarList->Value[i]=value;
	}

	m_pVarList.push_back(pVarList);
}

// [070528 AHC]
void CVariableList::AddGuid(const char* name, const CGUID& guid) // 增加一个GUID变量
{
	if(!name) return;

	//先检查变量是不是已经存在
	string guidName = name;
	if(m_pGuidList.find( guidName ) != m_pGuidList.end())
	{
		//如果存在
		SetGuidValue(name,&guid);	//直接赋值退出
		return;
	}

	CGUID* pGuidList = new CGUID;		// 变量列表

	//拷贝赋值创建变量
	*pGuidList = guid;
	m_pGuidList[guidName] = pGuidList;
}
bool CVariableList::isExist(const char* name)
{
	if(!name) return false;

	for (int i=0;i<m_pVarList.size();i++){
		if (strcmp(m_pVarList[i]->Name,name)==0){	//如果存在
			return true;
		}
	}
	return false;
}


bool CVariableList::RemoveVar(const char* name)
{
	if(!name) return false;

	//TODO:实现
	//先检查变量是不是已经存在
	vector<stVariable*>::iterator itr = m_pVarList.begin();
	for (; itr != m_pVarList.end(); itr++)
	{
		//如果存在
		if(strcmp((*itr)->Name,name)==0)
		{
			if( (*itr)->Array == 0 )
			{
				SAFE_DELETE( (*itr)->Value );
			}
			else
			{
				SAFE_DELETE_ARRAY( (*itr)->strValue );
			}

			SAFE_DELETE_ARRAY( (*itr)->Name );
			OBJ_RELEASE(stVariable,*itr);
			itr = m_pVarList.erase(itr);
			return true;
		}
	}
	return false;
}

bool CVariableList::RemoveGuid(const char* name)//删除Guid变量
{
	if(!name) return false;

	//TODO:实现
	//先检查变量是不是已经存在
	string guidName = name;
	GuidList::iterator itr = m_pGuidList.find( guidName );
	if(itr != m_pGuidList.end())
	{
		//如果存在,删除旧变量     
		SAFE_DELETE( itr->second );
		m_pGuidList.erase(itr);
		return true;
	}

	return false;
}
//////////////////////////

/////////////////////////////
// 读取变量详细资料
void CVariableList::LoadVarData(char* file, char* index)
{
	
}

int CVariableList::GetArrayNum(char *name, CPlayer* pPlayer, CRegion* pRegion)
{
	if(!name) return -1;

	char* ptr = name;
	int lArray = 0;
	char arrSize[32];
	memset(arrSize, 0, sizeof(arrSize));
	bool isX = false; // 当前字符是否'['
	int n = 0; // 下标字符串计数
	bool hasFindBlank = false; // 是否找到'['
	int blankPos = 0; // '['的位置
	while(*ptr != 0)
	{
		if(*ptr == '[')
		{
			isX = true;
			hasFindBlank = true;
		}
		if(*ptr == ']')
			isX = false;

		if( isX && *ptr != '[' && *ptr != ' ' && *ptr != '\t' )
		{
			arrSize[n] = *ptr;
			n++;
		}

		if(!hasFindBlank)
			blankPos++;
		ptr++;
	}
	return atoi(arrSize);
}

////////////////////////////
// 获取数组变量的名字
void CVariableList::GetArrayName(char *name, char *ret)
{
	if(!name || !ret) return;

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
	//	ret[k]=0;
	}
}

///////////////////////
// 获取变量的值
double CVariableList::GetVarValue(const char *name, int num)
{
	if(!name) return VAR_NOT_FOUND;

	for(int i=0; i<m_pVarList.size(); i++)
	{
		if( stricmp(name, m_pVarList[i]->Name) == 0 )
		{
			if( m_pVarList[i]->Array == 0 && num==0 )
			{
				if(m_pVarList[i]->Value)
					return *m_pVarList[i]->Value;
			}
			else if( m_pVarList[i]->Array <= num )	//下标越界或者类型不匹配(期望整型,实际为字符串)
			{
				assert(0);
				return VAR_NOT_FOUND;
			}
			else	//数组
			{
				return m_pVarList[i]->Value[num];
			}
		}
	}

//	assert(0);
	return VAR_NOT_FOUND;
}
///////////////////////
// 获取变量的值(字符串)
const char* CVariableList::GetVarValue(const char *name)
{
	if(!name) return NULL;

	for(int i=0; i<m_pVarList.size(); i++)
	{
		if( stricmp(name, m_pVarList[i]->Name) == 0 )
		{
			if( m_pVarList[i]->Array < 0)
				return m_pVarList[i]->strValue;
			else
				return NULL;
		}
	}

//	assert(0);
	return NULL;
}
CGUID CVariableList::GetGuidValue(const char *name)				// 获取GUID的值
{
	if(!name) return CGUID::GUID_INVALID;

	string guidName = name;
	map<string, CGUID*>::iterator itr = m_pGuidList.find( guidName );
	if( itr != m_pGuidList.end() )
	{
		return *(itr->second);
	}

	return CGUID::GUID_INVALID;
}
//////////////////////////
// 设置变量的值（变量名，数组下标，值）
int CVariableList::SetVarValue(const char *name, int num, double value)
{
	if(!name) return VAR_NOT_FOUND;

	for(int i=0; i<m_pVarList.size(); i++)
	{
		if( stricmp(name, m_pVarList[i]->Name) == 0 )
		{
			if( m_pVarList[i]->Array > num )	//数组
			{
				m_pVarList[i]->Value[num]=value;
				return true;
			}
			else if(m_pVarList[i]->Array == 0)
			{
				*m_pVarList[i]->Value=value;
				return true;
			}
		}
	}

	//	assert(0);
	return VAR_NOT_FOUND;
}
//////////////////////////
// 设置变量的值（变量名，值）
int CVariableList::SetVarValue(const char *name, const char* value)
{
	if(!name) return VAR_NOT_FOUND;

	if (0==value)	//如果是NULL,则默认为空串.
		value="";
	for(int i=0; i<m_pVarList.size(); i++)
	{
		if( stricmp(name, m_pVarList[i]->Name) == 0 )
		{
			int len=lstrlen(value)+1;	//新字符串长度,要考虑末尾的结束符0多占一个字节
			//准备重新规划缓存

			if (m_pVarList[i]->Array<0){	//原来该变量是字符串
				if(abs(m_pVarList[i]->Array) < len){	//如果原来缓存空间少于需求
					//新开缓存
					SAFE_DELETE_ARRAY(m_pVarList[i]->strValue);
					m_pVarList[i]->strValue=new char[len];
				}
			}
			else if(m_pVarList[i]->Array>0)
			{	
				//原来变量是数组
				if(m_pVarList[i]->Array*(double)sizeof(double)<len)
				{
					SAFE_DELETE_ARRAY(m_pVarList[i]->Value);
					m_pVarList[i]->strValue=new char[len];
				}
			}
			else{	//原来变量是整数
				SAFE_DELETE(m_pVarList[i]->strValue);
				m_pVarList[i]->strValue=new char[len];
			}

			//复制字符串
			strcpy(m_pVarList[i]->strValue,value);
			m_pVarList[i]->Array=-len;
			return true;

		}
	}

//	assert(0);
	return VAR_NOT_FOUND;
}

int CVariableList::SetGuidValue(const char *name, const CGUID* value)				// 设置GUID的值
{
	if(!name) return VAR_NOT_FOUND;

	string guidName = name;
	map<string, CGUID*>::iterator itr = m_pGuidList.find( guidName );
	if( itr != m_pGuidList.end() )
	{
		*itr->second = *value; // 拷贝赋值
		return true;
	}

	return VAR_NOT_FOUND;
}

// 添加到vector<BYTE>
//TODO:加入字符串类型支持(m_pVarList[i]->Array<0)
bool CVariableList::AddToByteArray( vector<BYTE>* pByteArray )
{
	if(!pByteArray) return false;

	_AddToByteArray(pByteArray, (long)m_pVarList.size());				// 变量个数	

	vector<BYTE> vectorTemp;
	for(int i=0; i<m_pVarList.size(); i++)
	{
		_AddToByteArray(&vectorTemp,m_pVarList[i]->Name);	// 名字
		_AddToByteArray(&vectorTemp,m_pVarList[i]->Array);
		if( m_pVarList[i]->Array == 0 )		// 个体变量
		{
			if(m_pVarList[i]->Value)
				_AddToByteArray(&vectorTemp, *(m_pVarList[i]->Value));
		}
		else if( m_pVarList[i]->Array > 0)	// 数组
		{
			for(int j=0; j<m_pVarList[i]->Array; j++)
			{
				_AddToByteArray(&vectorTemp, m_pVarList[i]->Value[j]);
			}
		}
		else{		//字符串
			_AddToByteArray(&vectorTemp, m_pVarList[i]->strValue);
		}
	}

		// GUID
	_AddToByteArray( pByteArray, (long)m_pGuidList.size() );		// 变量个数
	GuidList::iterator gitr = m_pGuidList.begin();
	for(; gitr != m_pGuidList.end(); gitr++)
	{
		if( gitr->second != NULL )		// GUID变量
		{
			_AddToByteArray( &vectorTemp, gitr->first.c_str() );	// 名字	
			_AddToByteArray(&vectorTemp, *(gitr->second));			// GUID变量
		}
	}

	_AddToByteArray(pByteArray, (long)vectorTemp.size());
	if(vectorTemp.size())
		_AddToByteArray(pByteArray, &vectorTemp[0], (long)vectorTemp.size());

	return true;
}

// 解码
//TODO:加入字符串类型支持(m_pVarList[i]->Array<0)
bool CVariableList::DecordFromByteArray( BYTE* pByte, long& pos, long bufSize )
{
	if(!pByte) return false;

	char strName[256];
	long lVarNum;
	long lArray;

	// 普通变量个数
	lVarNum = _GetLongFromByteArray( pByte, pos, bufSize );
  
	//GUID个数
	int num = _GetLongFromByteArray( pByte, pos, bufSize );

	//buf size
	int tBufSize = _GetLongFromByteArray( pByte, pos, bufSize );

	double value = 0;

	for(int i=0; i<lVarNum; i++)
	{

		_GetStringFromByteArray(pByte, pos, strName, bufSize);
		lArray = _GetLongFromByteArray(pByte,pos, bufSize);
		if( lArray == 0 )	// 个体变量
		{
			value = _GetDoubleFromByteArray(pByte,pos, bufSize);
			AddVar(strName, value);
		}
		else if(lArray > 0)	// 数组
		{
			AddVar(strName, lArray, 0.0f);

			for(int j=0; j<lArray; j++)
			{
				value = _GetDoubleFromByteArray(pByte, pos, bufSize);
				SetVarValue(strName, j, value);
			}
		}
		else
		{		//字符串
			char szValue[1024];
			_GetStringFromByteArray(pByte, pos, szValue, bufSize);
			AddVar( strName, szValue );
		}
	}

	// GUID
	char GuidName[256];
	CGUID tGuid;
	for(int i=0; i<num; i++)
	{
		_GetStringFromByteArray( pByte, pos, GuidName, bufSize );	// 名字	
		_GetBufferFromByteArray(pByte, pos, tGuid, bufSize);	// GUID变量
		AddGuid(GuidName, tGuid);
	}

	return true;
}

bool CVariableList::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	char strName[256];
	char strValue[256]="";
	long lVarNum;
	long lDataSize;
	long lArray;

	//普通变量个数
	lVarNum = setReadDB.GetLongFromByteArray();  
	//GUID个数
	int num = setReadDB.GetLongFromByteArray( );		// 变量个数

	lDataSize=setReadDB.GetLongFromByteArray();

	double value = 0;

	for(int i=0; i<lVarNum; i++)
	{
		setReadDB.GetStringFromByteArray(strName, 256);
		lArray = setReadDB.GetLongFromByteArray();
		if( lArray == 0 )	// 个体变量
		{
			//SetVarValue(strName, 0, *(long*)(&pByte[pos]) );
			value = setReadDB.GetDoubleFromByteArray();
			AddVar(strName, value);
			//pos+=4;
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
			setReadDB.GetStringFromByteArray(strValue,256);
			AddVar(strName,strValue);			
		}
	}

	// GUID
	char GuidName[256];
	CGUID tGuid;
	for(int i=0; i<num; i++)
	{
		setReadDB.GetStringFromByteArray( GuidName,256 );	// 名字	
		setReadDB.GetBufferFromByteArray( tGuid);	// GUID变量
		AddGuid(GuidName, tGuid);
	}
	return true;
}
