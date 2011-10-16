//======================================================================================
/**
*	@file		Emotion.h
*	@data		
*	@author		
*/
//======================================================================================

#pragma once

/**
*	@class EmotionSetup
*
*	@brief 表情配置类
*/

class EmotionSetup
{
public:
	/// 构造
	EmotionSetup();

	/// 析构
	~EmotionSetup();

	/// 加载表情列表
	static bool LoadSetup(const char* pFileName);

	/// 编码
	static bool Serialize(vector<uchar>* pByteArray);

	/// 解码
	static bool Unserialize(uchar* pByte, long& pos);

	/// 根据ID查询表情是否重复
	static BOOL IsEmotionRepeated(long lID);

	/// 取得表情列表
	static map<long, BOOL>* GetEmotionList();

private:
	/// 表情集合
	static map<long, BOOL>	s_mEmotions;
};
