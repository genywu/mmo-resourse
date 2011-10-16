#pragma once

// 可配置序列字符串表
typedef vector<unsigned long> SequenceString;
class CSequenceString
{
	/*
	 * 	静态
	 */
public:
	static SequenceString m_sSequenceString;	// 从文件读出的序列串表
	static void Initialize(ULONG cnt);
	static void Uninitialize();
	void Serialize(vector<BYTE>* pByteArray);		// 添加到CByteArray
	//static void Unserialize(BYTE* pByte, long& pos);	// 解码

	/*
	 * 	非静态
	 */
public:
	CSequenceString();

	~CSequenceString();

private:
	long m_pos;
	
public:
	bool m_bIsUsable;
	void SetPos(long pos) { m_pos = pos; }
	long GetPos() { return m_pos; }
	unsigned long GetElement();
	bool Check(unsigned long lElement);
};