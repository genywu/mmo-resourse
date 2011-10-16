#pragma once

#include <vector>

#define OUTPUT_DOT_COUNT 2047//2k每次

class OutputBitStream
{
	std::vector<unsigned char>m_byStreamArray;

	unsigned char m_byMask;
    int m_iRack;
    
	int m_iOutputDotCount;

	int m_iStreamPosition;

public:
	OutputBitStream(void);
	~OutputBitStream(void);

	void OutputBit(int bit);
	void OutputBits(unsigned int dwCode,int iCount);
	int  GetLength() const { return (int)m_byStreamArray.size();}//长度是字节长度

	void Flush() { m_iStreamPosition = 0;}
	unsigned char GetChar() { return m_byStreamArray[m_iStreamPosition++];}

	void CloseStream(void);
	void OpenStream(void);
};
