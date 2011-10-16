#pragma once
#include <map>
#include <string>

struct tagCountTimerParam
{
	tagCountTimerParam(void)
		: m_dwType(-1)
		, m_strPicPath("")
		, m_strBackPicPath("")
		, m_strTextPath("")
		, m_dwTextTime(0)
		, m_dwTextViewTime(0)
		, m_strText("")
		, m_strTimeoutText("")
		, m_strScriptPath("")
	{

	}
	~tagCountTimerParam(void)
	{

	}

	void operator=(const tagCountTimerParam& r)
	{
		m_dwType = r.m_dwType;
		m_strPicPath = r.m_strPicPath;
		m_strBackPicPath = r.m_strBackPicPath;
		m_strTextPath = r.m_strTextPath;
		m_dwTextTime = r.m_dwTextTime;
		m_dwTextViewTime = r.m_dwTextViewTime;
		m_strText = r.m_strText;
		m_strTimeoutText = r.m_strTimeoutText;
		m_strScriptPath = r.m_strScriptPath;
	}

	DWORD m_dwType;				// 类型
	std::string m_strPicPath; // 客户端礼包图标路径
	std::string m_strBackPicPath; // 客户端礼包背景图标路径
	std::string m_strTextPath; // 客户端文字框图标路径
	DWORD m_dwTextTime; // 文本框弹出时间间隔 单位：秒
	DWORD m_dwTextViewTime; // 文本框显示时间时长 单位：秒
	std::string m_strText; // 客户端计时中文本框内容
	std::string m_strTimeoutText; // 客户端计时到点中文本框内容
	std::string m_strScriptPath; // 客户端点击运行脚本路径
};

class CCountTimerList
{
public:
	CCountTimerList(void);
	~CCountTimerList(void);

	static bool LoadSetup(char* pFileName);

	static bool Serialize(std::vector<BYTE>& pCountBA);
	static bool Unserialize(BYTE* countTimerBuf, long& pos);

	static const tagCountTimerParam& GetCountTimerParam(long id);

	static DWORD GetMaxTimerNum(void);
private:
	static std::map<long, tagCountTimerParam>	s_mCountTimer;
	static DWORD m_dwMaxTimerNum;
};

static tagCountTimerParam NULL_TIMERPARAM;