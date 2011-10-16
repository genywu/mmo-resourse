#pragma once

// ø…≈‰÷√–Ú¡–◊÷∑˚¥Æ±Ì
typedef vector<unsigned long> SequenceString;
class CSequenceString
{
public:
	static long m_sStartPos; 
	static SequenceString m_sSequenceString;
	static void Initialize(BYTE* pByte, long& pos);
	static void Uninitialize();
	static void Clearup();

public:
	CSequenceString();

	~CSequenceString();

private:
	static unsigned long m_pos;
public:
	static bool m_bIsUsable;

	static unsigned long GetElement();
};