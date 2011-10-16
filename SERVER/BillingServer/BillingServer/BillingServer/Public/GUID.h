//
// CGUID类 可以 直接转换GUID结构体
// 创建某个对象时，初始为0，通过调用Fill()成员函数生成GUID
// 重载了=、==。
// 2006/10/10 by bugs
//

// 使用该类 必须先调用Initialize初始COM

#pragma once
#ifndef _guid_h_
#define _guid_h_

#include <hash_map>
#include <string>

using namespace stdext;


class CGUID
{
private:
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];

	friend class hash_guid_compare;

public:
	static void Initialize();
	static void Uninitialize();

	static bool CreateGUID(CGUID &guid);
	static CGUID GUID_INVALID;

	CGUID(void)
	{
		/*Data1 = 0;
		Data2 = 0;
		Data3 = 0;
		Data4[0] = 0;
		Data4[1] = 0;
		Data4[2] = 0;
		Data4[3] = 0;
		Data4[4] = 0;
		Data4[5] = 0;
		Data4[6] = 0;
		Data4[7] = 0;*/

		memset(this, 0, sizeof(CGUID));
	}

	CGUID(const char* strGuid)
	{
		memset(this, 0, sizeof(CGUID));
		if(strGuid != NULL && strlen(strGuid) == 38)
		{
			char strtempt[37] = "";
			strncpy(strtempt,strGuid+1,36);
			strtempt[36] = '\0';
			char seps[] = "-";
			char *token = strtok( strtempt, seps );
			char *strstop = NULL;
			if(token != NULL)
			{
				Data1 = strtoul(token,&strstop,16);
				token = strtok( NULL, seps );
				if(token != NULL)
				{
					Data2 = (WORD)strtoul(token,&strstop,16);
					token = strtok( NULL, seps );
					if(token != NULL)
					{
						Data3 = (WORD)strtoul(token,&strstop,16);
						token = strtok( NULL, seps );
						if(token != NULL)
						{
							char strData[2] = "";
							for(int i=0;i<2;i++)
							{
								strncpy(strData,token+(i*2),2);								 
								Data4[i] = (BYTE)strtoul( strData,&strstop,16);
							}
							token = strtok( NULL, seps );
							if(token != NULL)
							{
								char strData[2] = "";
								for(int i=2;i<8;i++)
								{
									strncpy(strData,token+((i-2)*2),2);								 
									Data4[i] = (BYTE)strtoul( strData,&strstop,16);
								}
							}
						}
					}
				}
			}
		}
	}

	~CGUID(void)
	{
	}

	//bool Fill()
	//{
	//	if(FAILED(CoCreateGuid((GUID*)(this))))
	//		return false;
	//	return true;
	//}


	CGUID & operator = (const CGUID & guid)
	{
		if( &guid != this )
			memcpy(this, &guid, sizeof(CGUID));
		return *this;
	}

	bool operator == (const CGUID & guid) const
	{
		if( !memcmp(this, &guid, sizeof(CGUID)) )
			return true;
		return false;
	}

	bool operator != (const CGUID & guid) const
	{
		return !(operator ==(guid));
	}

	bool operator < (const CGUID& guid) const
	{
		return ( memcmp(this,&guid,sizeof(CGUID)) < 0 );
	}

	//strGuid的大小至少为36
	bool tostring(char * strGuid)
	{
		if(strGuid == 0 )	return false;
		sprintf(strGuid,"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		Data1, Data2, Data3, Data4[0],Data4[1],
		Data4[2],Data4[3],Data4[4],Data4[5],Data4[6],Data4[7]);
		return true;
	}
	
	bool IsInvalided()
	{
		if( memcmp(this, 0, sizeof(CGUID)) )
			return false;
		return true;
	}
};


// 提供给hash使用
class guid_compare
{
public:
	bool operator( )( const CGUID& Key1, const CGUID& Key2 ) const
	{
		if(Key1 < Key2)
			return true;
		return false;
	}

};

class hash_guid_compare:public hash_compare<CGUID,guid_compare>
{
private:
	guid_compare	comp;
public:
	//计算散列值
	size_t operator( )( const CGUID& Key ) const
	{
		return (Key.Data1 >> 24)&0xFF;
	}
	//less operator
	bool operator( )( const CGUID& _Key1,const CGUID& _Key2 ) const
	{
		return comp(_Key1,_Key2);
	}
};

#endif