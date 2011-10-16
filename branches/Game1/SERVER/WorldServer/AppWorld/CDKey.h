#pragma once


class CCDKey
{
private:
	string	m_strCDKey;
	int		m_nPlayerID1;
	int		m_nPlayerID2;
	int		m_nPlayerID3;

public:
	CCDKey(void);
	virtual ~CCDKey(void);

	string GetCDKey() {return m_strCDKey;}
	int GetPlayerID1(){return m_nPlayerID1;}
	int GetPlayerID2(){return m_nPlayerID2;}
	int GetPlayerID3(){return m_nPlayerID3;}

	void SetCDKey(string strKey){m_strCDKey=strKey;}
	void SetPlayerID1(int nID1){m_nPlayerID1=nID1;}
	void SetPlayerID2(int nID2){m_nPlayerID2=nID2;}
	void SetPlayerID3(int nID3){m_nPlayerID3=nID3;}
};
