#include "StdAfx.h"
#include "MultiHpBar.h"

CMultiHpBar::HpBar::HpBar()
{
	HpRange=0;
	SpringState= true;
}
CMultiHpBar::CMultiHpBar()
{
	//AddHpBar(100,0);
	//AddHpBar(10000,0);
	m_BaseMaxHp=0;
}
CMultiHpBar::~CMultiHpBar(){}
//
bool CMultiHpBar::Clean()
{
	m_BaseMaxHp=0;
	m_listHpBar.clear();
	return true;
}
//添加一个血条
bool CMultiHpBar::AddHpBar(int HpRange, const char* scriptname)
{
	HpBar bar;
	bar.HpRange= HpRange;
	if (scriptname!= NULL)
	{
		bar.ScriptName=scriptname;
	}
	m_listHpBar.push_back(bar);
	return true;
}

bool CMultiHpBar::AddDatatoByteArray(vector<uchar>* pByteArray)
{
	_AddToByteArray(pByteArray,m_BaseMaxHp);
	_AddToByteArray(pByteArray,(long)m_listHpBar.size());
	for (list<HpBar>::iterator it= m_listHpBar.begin();
		it!= m_listHpBar.end(); it++)
	{
		_AddToByteArray(pByteArray,(long)it->HpRange);
		//_AddToByteArray(pByteArray,it->Color);
		//_AddToByteArray(pByteArray,it->scriptname.c_str());
	}
	return true;
}

bool CMultiHpBar::DecordDataFromByteArray(uchar* pByte, long& pos)
{
	m_BaseMaxHp= _GetLongFromByteArray(pByte,pos);
	m_listHpBar.clear();
	long size=_GetLongFromByteArray(pByte,pos);
	for (int i=0; i<size; i++)
	{
		HpBar bar;
		bar.HpRange=_GetLongFromByteArray(pByte,pos);
		m_listHpBar.push_back(bar);
	}
	return true;
}

bool CMultiHpBar::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray(m_BaseMaxHp);
	setWriteDB.AddToByteArray((long)m_listHpBar.size());
	for (list<HpBar>::iterator it= m_listHpBar.begin();
		it!= m_listHpBar.end(); it++)
	{
		setWriteDB.AddToByteArray((long)it->HpRange);
	}
	return true;
}
bool CMultiHpBar::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	m_BaseMaxHp= setReadDB.GetLongFromByteArray();
	m_listHpBar.clear();
	long size=setReadDB.GetLongFromByteArray();
	for (int i=0; i<size; i++)
	{
		HpBar bar;
		bar.HpRange=setReadDB.GetLongFromByteArray();
		m_listHpBar.push_back(bar);
	}
	return true;
}

int  CMultiHpBar::GetMaxHp()
{
	int maxhp=m_BaseMaxHp;
	for (list<HpBar>::iterator it= m_listHpBar.begin();
		it!= m_listHpBar.end(); it++)
	{
		maxhp+= it->HpRange;
	}
	return maxhp;
}

CMultiHpBar::HpBarInfo CMultiHpBar::GetCurrentInfo(int Hp,int ChangeHp)
{
	int Level = 0;
	float Scale = 0.0f;
	const char *ScriptName = NULL;
	if( Hp <= m_BaseMaxHp )
	{
		// 第一个血条结束
		if( Hp + ChangeHp > m_BaseMaxHp && m_listHpBar.size() > 0 )
		{
			ScriptName = m_listHpBar.begin()->ScriptName.c_str();
		}
		return HpBarInfo( m_BaseMaxHp == 0 ? Scale : (float)Hp / (float)m_BaseMaxHp, m_BaseMaxHp, Level, ScriptName );
	}
	
	if( m_listHpBar.size() > 0 )
	{
		list<HpBar>::iterator it = m_listHpBar.begin();
		int HpBarMax = 0;
		Hp -= m_BaseMaxHp;
		// 从第一个额外血条开始判断
		for(Level = 1 ; it != m_listHpBar.end(); ++ it,++Level )
		{
			HpBarMax = it->HpRange ;
			if( Hp > HpBarMax )
			{
				Hp -= HpBarMax;
			}
			else if( Hp + ChangeHp > HpBarMax && it->SpringState )
			{
				++ it;
				if( it != m_listHpBar.end() )
				{
					it->SpringState = false;
					ScriptName = it->ScriptName.c_str();
				}
				break;
			}
			else
				break;
		}

		return HpBarInfo( HpBarMax > 0 ? (float)Hp / (float)HpBarMax : 0.0f, HpBarMax, Level, ScriptName );
	}

	return HpBarInfo( 0.0f, 0, Level, ScriptName );
}

void CMultiHpBar::SetBaseMaxHp(int Hp)
{
	if (m_BaseMaxHp==0)
	{
		m_BaseMaxHp=Hp;
	}
}
