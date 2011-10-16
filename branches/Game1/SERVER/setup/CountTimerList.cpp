#include "stdafx.h"
#include "CountTimerList.h"

std::map<long, tagCountTimerParam>	CCountTimerList::s_mCountTimer;
DWORD CCountTimerList::m_dwMaxTimerNum = 0;

CCountTimerList::CCountTimerList(void)
{

}

CCountTimerList::~CCountTimerList(void)
{

};

DWORD CCountTimerList::GetMaxTimerNum(void)
{
	return m_dwMaxTimerNum;
}

bool CCountTimerList::LoadSetup(char* pFileName)
{
	if(!pFileName) return false;
	
	CRFile* prfile = rfOpen(pFileName);
	if(prfile == NULL)
	{
		return false;
	}
	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	s_mCountTimer.clear();

	std::string temp;
	tagCountTimerParam param;
	stream >> temp >> m_dwMaxTimerNum;

	while(ReadTo(stream, "#"))
	{
		stream  >> param.m_dwType
			>> param.m_strPicPath
			>> param.m_strBackPicPath
			>> param.m_strTextPath
			>> param.m_dwTextTime
			>> param.m_dwTextViewTime
			>> param.m_strText
			>> param.m_strTimeoutText
			>> param.m_strScriptPath
			;

		if(param.m_strText=="null") param.m_strText="";
		if(param.m_strTimeoutText=="null") param.m_strTimeoutText="";
		if(param.m_strScriptPath=="null") param.m_strScriptPath="";

		s_mCountTimer[param.m_dwType] = param;
	}

	return true;
}

bool CCountTimerList::Serialize(std::vector<BYTE>& pCountBA)
{
	// 玩家定时器最大值
	_AddToByteArray(&pCountBA, (DWORD)m_dwMaxTimerNum);

	// 个数
	_AddToByteArray(&pCountBA, (DWORD)s_mCountTimer.size());

	std::map<long, tagCountTimerParam>::iterator it = s_mCountTimer.begin();
	for(; it!=s_mCountTimer.end(); it++)
	{
		_AddToByteArray(&pCountBA, (DWORD)it->second.m_dwType);
		_AddToByteArray(&pCountBA, it->second.m_strPicPath.c_str());
		_AddToByteArray(&pCountBA, it->second.m_strBackPicPath.c_str());
		_AddToByteArray(&pCountBA, it->second.m_strTextPath.c_str());
		_AddToByteArray(&pCountBA, (DWORD)it->second.m_dwTextTime); // 文本框弹出时间间隔
		_AddToByteArray(&pCountBA, (DWORD)it->second.m_dwTextViewTime); // 文本框显示时间时长
		_AddToByteArray(&pCountBA, it->second.m_strText.c_str());
		_AddToByteArray(&pCountBA, it->second.m_strTimeoutText.c_str());
		_AddToByteArray(&pCountBA, it->second.m_strScriptPath.c_str());
	}
	return true;
}
bool CCountTimerList::Unserialize( BYTE* countTimerBuf, long& pos )
{
	s_mCountTimer.clear();

	m_dwMaxTimerNum = _GetDwordFromByteArray(countTimerBuf, pos);

	DWORD num = _GetDwordFromByteArray(countTimerBuf, pos);

	for(DWORD i=0; i<num; i++)
	{
		char str[1024];
		tagCountTimerParam timer;
		
		timer.m_dwType = _GetDwordFromByteArray(countTimerBuf, pos); // 定时器类型

		_GetStringFromByteArray(countTimerBuf, pos, str);
		timer.m_strPicPath = str; // 客户端礼包图标路径

		_GetStringFromByteArray(countTimerBuf, pos, str);
		timer.m_strBackPicPath = str; // 客户端礼包背景图标路径

		_GetStringFromByteArray(countTimerBuf, pos, str);
		timer.m_strTextPath = str; // 客户端文字框图标路径

		timer.m_dwTextTime = _GetDwordFromByteArray(countTimerBuf, pos); // 文本框弹出时间间隔
		timer.m_dwTextViewTime = _GetDwordFromByteArray(countTimerBuf, pos); // 文本框显示时间时长

		_GetStringFromByteArray(countTimerBuf, pos, str);
		timer.m_strText = str; // 客户端计时中文本框内容

		_GetStringFromByteArray(countTimerBuf, pos, str);
		timer.m_strTimeoutText = str; // 客户端计时到点文本框内容
		
		_GetStringFromByteArray(countTimerBuf, pos, str);
		timer.m_strScriptPath = str; // 客户端点击运行脚本路径

		s_mCountTimer[timer.m_dwType] = timer;
	}
	return true;
}

const tagCountTimerParam& CCountTimerList::GetCountTimerParam(long id)
{
	std::map<long, tagCountTimerParam>::iterator it = s_mCountTimer.find(id);
	if(it!=s_mCountTimer.end())
		return it->second;
	return NULL_TIMERPARAM;
}