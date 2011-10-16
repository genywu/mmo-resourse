#ifndef _CRC32STATIC_H_
#define _CRC32STATIC_H_

//功能:CRC交验类，得到一个文件的唯一的32位交验码
class CCrc32Static
{
public:
	CCrc32Static();
	virtual ~CCrc32Static();
	static ulong FileCrc32Filemap(LPCTSTR szFilename, ulong &dwCrc32);
	static ulong DataCrc32(const void* pBuff,long lLen, ulong &dwCrc32);

protected:
	static bool GetFileSizeQW(const HANDLE hFile, ulonglong &qwSize);
	static inline void CalcCrc32(const uchar byte, ulong &dwCrc32);
	static ulong s_arrdwCrc32Table[256];
};

#endif