#include "..\..\public\LotteryXml.h"

class CLotterySystem
{
public:
	CLotterySystem();
	~CLotterySystem();

	static		CLotterySystem&	GetInstance(void);

	void SetWorldCredit(long l){m_lWorldCredit = l;};
	long GetWorldCredit(){return m_lWorldCredit;};
	void SetCreditHighest(long l){m_lCreditHighest = l;};
	long GetCreditHighest(){return m_lCreditHighest;};
	void SetCreditHighestName(string str){m_strCreditHighestName = str;};
	string GetCreditHighestName(){return m_strCreditHighestName;};
	void SendCreditHighest();

	void SetIsSendWorldCredit(bool b);
	void Run();
private:
	long m_lWorldCredit;

	long m_lCreditHighest;

	string m_strCreditHighestName;

	DWORD m_dwLastRefreshTime;

	bool m_bSendWorldCredit;

};