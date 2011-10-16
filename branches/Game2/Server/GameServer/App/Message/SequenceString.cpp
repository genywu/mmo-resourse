#include "StdAfx.h"
#include "sequencestring.h"
#include <Mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/*
 * 	static for class
 */
SequenceString CSequenceString::m_sSequenceString;


// 从文件读取列表
void CSequenceString::Initialize(ULONG cnt)
{

	srand(timeGetTime());
	for(ULONG i=0; i<cnt; i++)
	{
		ULONG temp = (ULONG)rand();
		if(temp==0) temp=1; // 验证序列不要出现0
		m_sSequenceString.push_back( temp );
	}
}

void CSequenceString::Uninitialize()
{
	m_sSequenceString.clear();
}

// 添加到CByteArray
void CSequenceString::Serialize(vector<BYTE>* pByteArray)
{
	if(m_sSequenceString.size())
	{
		long start_pos = rand() % (long)m_sSequenceString.size();
		_AddToByteArray(pByteArray, (long)start_pos);
		SetPos(start_pos);
		m_bIsUsable = true;

#ifdef _DEBUG
		char debug[64];
		_snprintf(debug, 64, "客户端请求服务器序列 开始位子%d\r\n", start_pos);
		OutputDebugString(debug);
#endif

	}
	else
	{
		_AddToByteArray(pByteArray, (long)-1 );
		SetPos(-1);
		m_bIsUsable = false;
	}

	_AddToByteArray(pByteArray, (long)m_sSequenceString.size() );
	for(UINT i=0; i<m_sSequenceString.size(); i++)
	{
		_AddToByteArray(pByteArray, (long)m_sSequenceString[i]);
	}
}

//// 解码
//void CSequenceString::DecordFromByteArray(BYTE* pByte, long& pos)
//{
//	_GetBufferFromByteArray(pByte, pos, &m_SequenceStringTable, sizeof(tagSeqenceString));
//}


/*
 * 	for object
 */
CSequenceString::CSequenceString()
{
	m_pos = -1;
	m_bIsUsable = false;
}


CSequenceString::~CSequenceString()
{
}
unsigned long CSequenceString::GetElement()
{

	if(m_sSequenceString.size() == 0) return 0;
	if(m_pos >= (long)m_sSequenceString.size() )
		m_pos = 0;

	return m_sSequenceString[m_pos++];
}

bool CSequenceString::Check(unsigned long lElement)
{
	ULONG elm = GetElement();

#ifdef _DEBUG
	//Log4c::Trace(ROOT_MODULE, FormatText("GS_SEQUENCE_0", elm, lElement));//序列 S:%d C:%d
#endif // _DEBUG
	if( elm == lElement ) return 1;

	Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_SEQUENCE_1", elm, lElement));//序列验证不符 S:%d C:%d
	return 0;
}
