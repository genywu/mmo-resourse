#pragma once
#include "specialequip.h"

class CBindModel : public CSpecialEquip
{
	struct stGPL
	{
		DWORD dwGroup;
		DWORD dwPart;
		DWORD dwLevel;
	};

	int    m_iCurrentSelLocatorIndex;			//当前选择的绑定点
	int	   m_iCurrentSelActionIndex;			//当前选择的动作索引
	string m_sFilePathName;						//绑定模型路径名

	vector<stGPL> m_vecGPLs;

public:
	enum WEAPON_STATUS 
	{
		WEAPON_ACTION0 = MAKEFOURCC('A','C','T','0'),
		WEAPON_ACTION1 = MAKEFOURCC('A','C','T','1'),
		WEAPON_ACTION2 = MAKEFOURCC('A','C','T','2'),
		WEAPON_ACTION3 = MAKEFOURCC('A','C','T','3'),
		WEAPON_ACTION4 = MAKEFOURCC('A','C','T','4'),
		WEAPON_ACTION5 = MAKEFOURCC('A','C','T','5'),
		WEAPON_ACTION6 = MAKEFOURCC('A','C','T','6'),
		WEAPON_ACTION7 = MAKEFOURCC('A','C','T','7'),
	};

	CBindModel();
	~CBindModel();

	int		 GetCurrentSelLocatorIndex()						{ return m_iCurrentSelLocatorIndex; }
	int		 GetCurrentSelActionIndex()							{ return m_iCurrentSelActionIndex; }
	string & GetFilePathName()									{ return m_sFilePathName; }
	void	 SetCurrentSelLocatorIndex(int iLocatorIndex)		{ m_iCurrentSelLocatorIndex = iLocatorIndex; }
	void	 SetCurrentSelActionIndex(int iActionIndex)			{ m_iCurrentSelActionIndex = iActionIndex; }
	void	 SetFilePathName(string & sPathName)				{ m_sFilePathName = sPathName; }

	void ChangeStatus(DWORD eStatus);
	void SaveShowState();
	void SetShowState();

	void Display(CDisplayModel * pParentModel, AnimInfo * pParentAnimInfo);
};