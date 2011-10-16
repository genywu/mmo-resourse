#include "StdAfx.h"
#include "Emotion.h"
#include "../Common/Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// 静态成员初始化
map<long, BOOL> EmotionSetup::s_mEmotions;

/// 构造
EmotionSetup::EmotionSetup(void)
{
}

/// 析构
EmotionSetup::~EmotionSetup(void)
{
	s_mEmotions.clear();
}

/// 加载表情列表
bool EmotionSetup::LoadSetup(const char* pFileName)
{
	s_mEmotions.clear();
	CRFile* prfile = rfOpen(pFileName);
	if(prfile)
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);

		while(ReadTo(stream, "*"))
		{
			long lID;
			BOOL bLoop;
			stream >> lID >> bLoop;
			s_mEmotions[lID] = bLoop;
		}
		return true;
	}
	return false;
}

/// 编码
bool EmotionSetup::Serialize(vector<uchar>* pByteArray)
{
	_AddToByteArray(pByteArray, static_cast<DWORD>(s_mEmotions.size()));

	map<long, BOOL>::iterator it = s_mEmotions.begin();
	for(; it!=s_mEmotions.end(); ++it)
	{
		_AddToByteArray(pByteArray, it->first);
		_AddToByteArray(pByteArray, static_cast<long>(it->second));
	}
	return true;
}

/// 解码
bool EmotionSetup::Unserialize(uchar* pByte, long& pos)
{
	s_mEmotions.clear();
	DWORD dwSize = _GetDwordFromByteArray(pByte, pos);

	for(DWORD i=0; i<dwSize; i++)
	{
		long lID = _GetLongFromByteArray(pByte, pos);
		long bLoop = _GetLongFromByteArray(pByte, pos);

		s_mEmotions[lID] = static_cast<BOOL>(bLoop);
	}
	return true;
}

/// 根据ID查询表情是否重复
BOOL EmotionSetup::IsEmotionRepeated(long lID)
{
	BOOL bResult = FALSE;
	map<long, BOOL>::iterator it = s_mEmotions.find(lID);
	if(it != s_mEmotions.end())
	{
		bResult = it->second;
	}
	return bResult;
}

/// 取得表情列表
map<long, BOOL>* EmotionSetup::GetEmotionList()
{
	return &s_mEmotions;
}