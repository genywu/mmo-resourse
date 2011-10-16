


#ifndef _mail_h_
#define _mail_h_
#pragma once


class CMail
{
public:
	CMail(void);
	~CMail(void);

	//	@ 是否得到数据
	bool	IsGetData(void)		{ return m_bHasGetData; }
	void	SetGetData(bool b)	{ m_bHasGetData = b; }

	//	@	删除邮件物品
	void ReleaseMailGoods(stMailData& stMail);

	//	@	邮件解码
	void DecodeFromByteArray(stMailData& stMail,BYTE* pBytes,long& lpos);

	//	@ 更新邮件数据
	void RefreshData();

private:

	//	@ 是否收到数据
	bool	m_bHasGetData;

	//	@	邮件列表
	vector<stMailData>	m_vecMailData;
	//	@	待删除邮件列表
	vector<stMailData>	m_vecShouldDelMailData;
	//	@	收件人名字
	vector<string>			m_vecRecvMailName;
};
#endif//_mail_h_