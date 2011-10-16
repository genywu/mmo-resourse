// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CVALUE_41A46B5600DA_INCLUDED
#define _INC_CVALUE_41A46B5600DA_INCLUDED

//主要是为了实现数值+-时不需要考虑边界的影响.自动作裁减
//##ModelId=41A46B5600DA
class CValue 
{
public:
	CValue();

	//##ModelId=41A46E2602FD
	CValue(INT64 iMin, INT64 iMax);

	//##ModelId=41FB210D000F
	INT64 BaseValue();

	//##ModelId=41FB210D0138
	VOID SetBaseValue( INT64 iValue );

	//##ModelId=41FB301B0399
	VOID SetMinValue(INT64 iMinValue);

	//##ModelId=41FB302C0222
	VOID SetMaxValue(INT64 iMaxValue);

	INT64 GetMinValue();

	INT64 GetMaxValue();

	VOID Clear();

	//##ModelId=41A46E2B036B
	virtual ~CValue();

	//##ModelId=41FAEC43004E
	operator CHAR();

	//##ModelId=41FAE9C80399
	operator SHORT();

	//##ModelId=41A473B601A5
	operator LONG();

	//##ModelId=41FAE9DB00AB
	operator BYTE();

	//##ModelId=41FAEC4300FA
	operator WORD();

	//##ModelId=41A4739102EE
	operator DWORD();

	//##ModelId=41FAEA4403D8
	operator FLOAT();

	CHAR operator =(CHAR cValue);

	SHORT operator =(SHORT sValue);

	//##ModelId=41A46C0B0167
	LONG operator =(LONG lValue);

	BYTE operator =(BYTE bValue);

	WORD operator =(WORD wValue);

	//##ModelId=41A46C8401B5
	DWORD operator =(DWORD dwValue);

	//##ModelId=41FAEC4302BF
	FLOAT operator =(FLOAT fValue);

	CHAR operator +(CHAR cValue);

	SHORT operator +(SHORT sValue);

	LONG operator +(LONG lValue);

	BYTE operator +(BYTE bValue);

	WORD operator +(WORD wValue);

	DWORD operator +(DWORD dwValue);

	FLOAT operator +(FLOAT fValue);

	CHAR operator -(CHAR cValue);

	SHORT operator -(SHORT sValue);

	LONG operator -(LONG lValue);

	BYTE operator -(BYTE bValue);

	WORD operator -(WORD wValue);

	DWORD operator -(DWORD dwValue);

	FLOAT operator -(FLOAT fValue);

	CHAR operator *(CHAR cValue);

	SHORT operator *(SHORT sValue);

	LONG operator *(LONG lValue);

	BYTE operator *(BYTE bValue);

	WORD operator *(WORD wValue);

	DWORD operator *(DWORD dwValue);

	FLOAT operator *(FLOAT fValue);

	CHAR operator /(CHAR cValue);

	SHORT operator /(SHORT sValue);

	LONG operator /(LONG lValue);

	BYTE operator /(BYTE bValue);

	WORD operator /(WORD wValue);

	DWORD operator /(DWORD dwValue);

	FLOAT operator /(FLOAT fValue);
private:
	//##ModelId=41FAEDFD0242
	INT64 m_iMaxValue;

	//##ModelId=41FAEE020271
	INT64 m_iMinValue;

	//##ModelId=41A46BB6033C
	INT64 m_iValue;

	//##ModelId=41FAEF800232
	INT64 m_iModifier;
};

#endif /* _INC_CVALUE_41A46B5600DA_INCLUDED */
