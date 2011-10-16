#include "stdafx.h"
#include "Basemessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//BYTE CBaseMessage::s_pRLEBuffer[524288];		// RLE数据缓冲
DWORD CBaseMessage::s_dwRLEBufferLen = 524288;
BYTE *CBaseMessage::s_pRLEBuffer;		// RLE数据缓冲

DWORD CBaseMessage::s_dwTemptBufferLen = 5000000;
BYTE *CBaseMessage::s_pTemptBuffer;		// 临时缓冲区

CRITICAL_SECTION CBaseMessage::m_CriticalSectionCreateMSG;
CRITICAL_SECTION CBaseMessage::m_CSTemptBuffer;

CBaseMessage::CBaseMessage()
{
//	m_MsgData.SetSize(0, 128);
	stMsg st = {0, 0, 0, 0};
	Add(&st, sizeof(stMsg));
	SetSize(sizeof(stMsg));
	m_lPtr = sizeof(stMsg);
}

CBaseMessage::~CBaseMessage()
{
//	m_MsgData.clear();
}

bool CBaseMessage::Initial()
{
	InitializeCriticalSection(&m_CriticalSectionCreateMSG);
	InitializeCriticalSection(&m_CSTemptBuffer);	

	s_pRLEBuffer = new BYTE[CBaseMessage::s_dwRLEBufferLen];		// RLE数据缓冲
	s_pTemptBuffer = new BYTE[CBaseMessage::s_dwTemptBufferLen];
	return true;
}
bool CBaseMessage::Release()
{
	delete []s_pRLEBuffer;
	delete []s_pTemptBuffer;
	DeleteCriticalSection(&m_CriticalSectionCreateMSG);
	DeleteCriticalSection(&m_CSTemptBuffer);
	return true;
}

////////////////////////////////////////////////////////////////////////
//	添加和获取数据
////////////////////////////////////////////////////////////////////////

/// 添加 ///

void CBaseMessage::Add(char l)
{
	m_MsgData.push_back( (BYTE)l );
	SetSize(GetSize()+sizeof(char));
}

void CBaseMessage::Add(BYTE l)
{
	m_MsgData.push_back( (BYTE)l );
	SetSize(GetSize()+sizeof(BYTE));
}

void CBaseMessage::Add(short l)
{
	for(int i=0; i<sizeof(short); i++)
	{
		m_MsgData.push_back( ((BYTE*)(&l))[i] );
	}

	SetSize(GetSize()+sizeof(short));
}

void CBaseMessage::Add(WORD l)
{
	for(int i=0; i<sizeof(WORD); i++)
	{
		m_MsgData.push_back( ((BYTE*)(&l))[i] );
	}

	SetSize(GetSize()+sizeof(WORD));
}

void CBaseMessage::Add(long l)
{
	for(int i=0; i<sizeof(long); i++)
	{
		m_MsgData.push_back( ((BYTE*)(&l))[i] );
	}

	SetSize(GetSize()+sizeof(long));
}

void CBaseMessage::Add(LONG64 l)
{
	for(int i=0; i<sizeof(LONG64); i++)
	{
		m_MsgData.push_back( ((BYTE*)(&l))[i] );
	}

	SetSize(GetSize()+sizeof(LONG64));
}

void CBaseMessage::Add(DWORD l)
{
	for(int i=0; i<sizeof(DWORD); i++)
	{
		m_MsgData.push_back( ((BYTE*)(&l))[i] );
	}

	SetSize(GetSize()+sizeof(DWORD));
}

void CBaseMessage::Add(float l)
{
	for(int i=0; i<sizeof(float); i++)
	{
		m_MsgData.push_back( ((BYTE*)(&l))[i] );
	}

	SetSize(GetSize()+sizeof(float));
}

void CBaseMessage::Add(char* pStr)
{
	if(pStr)
	{
		long size = (long)(lstrlen(pStr));
		for(int i=0; i<size; i++)
		{
			m_MsgData.push_back(pStr[i]);
		}
		m_MsgData.push_back(0);
		SetSize(GetSize()+size+1);
	}
}

void CBaseMessage::Add(const CGUID&	guid)
{
	long size = sizeof(CGUID);
	if(guid == CGUID::GUID_INVALID)
	{
		size = 0;
	}
	Add((BYTE)size);
	if(size != 0)
		Add((void*)&guid,size);
}

void CBaseMessage::Add(void* pBuf, long size)
{
	BYTE* p = (BYTE*)pBuf;
	for(int i=0; i<size; i++)
	{
		m_MsgData.push_back(p[i]);
	}

	SetSize(GetSize()+size);
}

void CBaseMessage::AddEx(void* pBuf, long size)
{
	BYTE* p = (BYTE*)pBuf;
	Add(size);
	for(int i=0; i<size; i++)
	{
		m_MsgData.push_back(p[i]);
	}

	SetSize(GetSize()+size);
}

/// 获取 ///

char CBaseMessage::GetChar()
{
	long size = sizeof(char);
	if(m_lPtr+size > m_MsgData.size())	return 0;

	char l = (char)GetMsgBuf()[m_lPtr];
	m_lPtr += size;
	return l;
}

BYTE CBaseMessage::GetByte()
{
	long size = sizeof(BYTE);
	if(m_lPtr+size > m_MsgData.size())	return 0;

	BYTE l = (BYTE)GetMsgBuf()[m_lPtr];
	m_lPtr += size;
	return l;
}

short CBaseMessage::GetShort()
{
	long size = sizeof(short);
	if(m_lPtr+size > m_MsgData.size())	return 0;

	short l = *(short*)(&GetMsgBuf()[m_lPtr]);
	m_lPtr += size;
	return l;
}

WORD CBaseMessage::GetWord()
{
	long size = sizeof(WORD);
	if(m_lPtr+size > m_MsgData.size())	return 0;

	WORD l = *(WORD*)(&GetMsgBuf()[m_lPtr]);
	m_lPtr += size;
	return l;
}

long CBaseMessage::GetLong()
{
	long size = sizeof(long);
	if(m_lPtr+size > m_MsgData.size())	return 0;

	long l = *(long*)(&GetMsgBuf()[m_lPtr]);
	m_lPtr += size;
	return l;
}

LONG64 CBaseMessage::GetLONG64()
{
	long size = sizeof(LONG64);
	if(m_lPtr+size > m_MsgData.size())	return 0;

	LONG64 l = *(LONG64*)(&GetMsgBuf()[m_lPtr]);
	m_lPtr += size;
	return l;
}

DWORD CBaseMessage::GetDWord()
{
	long size = sizeof(DWORD);
	if(m_lPtr+size > m_MsgData.size())	return 0;

	DWORD l = *(DWORD*)(&GetMsgBuf()[m_lPtr]);
	m_lPtr += size;
	return l;
}

float CBaseMessage::GetFloat()
{
	long size = sizeof(float);
	if(m_lPtr+size > m_MsgData.size())	return 0;

	float l = *(float*)(&GetMsgBuf()[m_lPtr]);
	m_lPtr += size;
	return l;
}

bool  CBaseMessage::GetGUID(CGUID& guid)
{
	long size = GetByte();
	if(size == 0)
	{
		guid = CGUID::GUID_INVALID;
		return false;
	}

	if( Get(&guid,sizeof(CGUID)) == NULL)
		return false;
	return true;
}

//char* CBaseMessage::Get(char* pStr)
//{
//	char c = 0;
//	long n = 0;
//	
//	do
//	{
//		c = GetMsgBuf()[ m_lPtr++ ];
//		pStr[n++] = c;
//	}
//	while( c!=0 );
//
//	return pStr;
//}

char* CBaseMessage::GetStr( char* pStr,long lMaxLen )
{
	char c = 0;
	long n = 0;

	do
	{
		if(m_lPtr+1 > m_MsgData.size())
		{
			pStr[0] = '\0';
			return pStr;
		}
		c = GetMsgBuf()[ m_lPtr++ ];
		pStr[n++] = c;
	}
	while( c!=0 && n < lMaxLen );

	if( n == lMaxLen && pStr[lMaxLen-1] != 0 )
	{
		pStr[0] = '\0';
	}

	return pStr;
}


void* CBaseMessage::Get(void* pBuf, long size)
{
	if(m_lPtr+size > m_MsgData.size())	return NULL;

	memcpy(pBuf, &GetMsgBuf()[m_lPtr], size);
	m_lPtr += size;
	return pBuf;
}

void* CBaseMessage::GetEx(void* pBuf, long size)
{
	if(m_lPtr+size > m_MsgData.size())	return NULL;

	long lSize = GetLong();
	if(lSize != size)	return NULL;
	memcpy(pBuf, &GetMsgBuf()[m_lPtr], size);
	m_lPtr += size;
	return pBuf;
}

void CBaseMessage::Reset(void)
{
	m_MsgData.clear();
	stMsg st = {0, 0, 0, 0};
	Add(&st, sizeof(stMsg));
	SetSize(sizeof(stMsg));
	m_lPtr = sizeof(stMsg);
	Update();
}

////////////////////////////////////////////////////////////////////////
//	消息压缩
////////////////////////////////////////////////////////////////////////

#define MAX_RUN_LENGTH	8
//#define XOR_CODE		0x55
//#define XOR_CODE2		0x8f

//无压缩到Buffer
int CBaseMessage::DoTemptBuffer(void* pIN,int nAddPos,int nLen)
{
	if( nLen + nAddPos > (int)s_dwTemptBufferLen )
	{
		SAFE_DELETE(s_pTemptBuffer);
		s_dwRLEBufferLen = nLen + nAddPos;
		s_pTemptBuffer = new BYTE[s_dwRLEBufferLen];
	}
	else if( nLen + nAddPos <= 5000000 && s_dwTemptBufferLen > 5000000 )	// 还原消息缓冲区大小
	{
		SAFE_DELETE(s_pTemptBuffer);
		s_dwRLEBufferLen = 5000000;
		s_pTemptBuffer = new BYTE[s_dwRLEBufferLen];
	}
	memcpy(s_pTemptBuffer+nAddPos,pIN,nLen);
	return nLen+nAddPos;
}
// 压缩
int CBaseMessage::DoRLE(void* pIn, BYTE* pOut, int len)
{
	// 效验指针合法性
	int n  = (int)(pOut-s_pRLEBuffer);
	if(n < 0)
		return 0;
	
	// 如果压缩前的消息超过256k
	if(len>524288/2 && s_dwRLEBufferLen<(DWORD)len*2)
	{
		SAFE_DELETE(s_pRLEBuffer);
		s_dwRLEBufferLen = len * 2;
		s_pRLEBuffer = new BYTE[s_dwRLEBufferLen];
		pOut = s_pRLEBuffer + n;
	}
	else if( len <= 524288/2 && s_dwRLEBufferLen > 524288 )	// 还原消息缓冲区大小
	{
		SAFE_DELETE(s_pRLEBuffer);
		s_dwRLEBufferLen = 524288;
		s_pRLEBuffer = new BYTE[s_dwRLEBufferLen];
		pOut = s_pRLEBuffer + n;
	}

	//////////////////////////////////////////////////////////////////////////
	BYTE* Data = (BYTE*)pIn;

	int i, j = 0;
	int rl = 1;

//	for(i = 0; i < len; i++)
//		Data[i] ^= XOR_CODE;

	BYTE t = Data[0];
	for(i = 0; i < len - 1; i++)
	{
		if(t == Data[i+1])
			rl++;
		else
		{
			if(rl == 1)
			{
				if(t > 255 - MAX_RUN_LENGTH)
				{
					pOut[j++] = 255 - MAX_RUN_LENGTH + rl;
					pOut[j++] = t;
				}
				else
					pOut[j++] = t;
				t = Data[i+1];
			}
			else
			{
				while(rl > MAX_RUN_LENGTH)
				{
					pOut[j++] = 255;
					pOut[j++] = t;
					rl -= MAX_RUN_LENGTH;
				}
				pOut[j++] = 255 - MAX_RUN_LENGTH + rl;
				pOut[j++] = t;
				rl = 1;
				t = Data[i+1];
			}
		}
	}

	if(rl != 1)
	{
		while(rl > MAX_RUN_LENGTH)
		{
			pOut[j++] = 255;
			pOut[j++] = t;
			rl -= MAX_RUN_LENGTH;
		}
		pOut[j++] = 255 - MAX_RUN_LENGTH + rl;
		pOut[j++] = t;
	}
	else
	{
		if(t > 255 - MAX_RUN_LENGTH)
		{
			pOut[j++] = 255 - MAX_RUN_LENGTH + rl;
			pOut[j++] = t;
		}
		else
			pOut[j++] = t;
	}

//	for(i = 0; i < j; i++)
//		pOut[i] ^= XOR_CODE2;
//	for(i = 0; i < len; i++)
//		Data[i] ^= XOR_CODE;

	return j;
}

// 解压
int CBaseMessage::DecodeRLE(BYTE* pIn, BYTE* pOut, int len)
{
	int i;
	int j = 0;
//	for(i = 0; i < len; i++)
//		pIn[i] ^= XOR_CODE2;
	for(i = 0; i < len; i++)
	{
		if(pIn[i] <= 255 - MAX_RUN_LENGTH)
			pOut[j++] = pIn[i];
		else
		{
			int rl = pIn[i] -  255 + MAX_RUN_LENGTH;
			memset(pOut + j, pIn[i + 1], rl);
			j += rl;
			i++;
		}
	}

	//超出大小
	if( j > 1048576 )
	{
		return false;
	}
//	for(i = 0; i < j; i++)
//		pOut[i] ^= XOR_CODE;
	return j;
}

// 解压
int CBaseMessage::DecodeRLE_SAFE(BYTE* pIn, BYTE* pOut, int nInBufLen,int nOutBufLen)
{
	int i;
	int j = 0;
//用32bit的xor提高效率

//	static unsigned int xorcode2=XOR_CODE2 | XOR_CODE2<<8 | XOR_CODE2<<16 | XOR_CODE2<<24;
//	for(i=0;i<nInBufLen-4;i+=4)
//		*((unsigned int*)&pIn[i])^=xorcode2;
//补充最后少于32bit的部分
//	for(;i<nInBufLen;i++)
//		pIn[i] ^= XOR_CODE2;


	for(i = 0; i < nInBufLen; i++)
	{
		if(pIn[i] <= 255 - MAX_RUN_LENGTH)
			if(j<nOutBufLen)	//检查边界
				pOut[j++] = pIn[i];
			else	//超出边界
				return false;
		else
		{
			int rl = pIn[i] -  255 + MAX_RUN_LENGTH;
			if((j+rl)<nOutBufLen){	//检查边界
				memset(pOut + j, pIn[i + 1], rl);
				j += rl;
				i++;
			}
			else//超出边界
				return false;
		}
	}

	//超出大小
	if( j >= nOutBufLen )
	{
		return false;
	}

	//用32bit的xor提高效率
//	static unsigned int xorcode=XOR_CODE | XOR_CODE<<8 | XOR_CODE<<16 | XOR_CODE<<24;
//	for(i=0;i<j-4;i+=4)
//	*((unsigned int*)&pOut[i])^=xorcode;
//	//补充最后少于32bit的部分
//	for(;i<j;i++)
//		pOut[i] ^= XOR_CODE;

	return j;
}

////////////////////////////////////////////////////////////////////////
//	消息处理
////////////////////////////////////////////////////////////////////////
