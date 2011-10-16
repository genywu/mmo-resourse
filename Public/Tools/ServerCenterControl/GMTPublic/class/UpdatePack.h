

#include "UpdateFolder.h"

#pragma once


class UpdatePack : public UpdateFolder
{
public:
	UpdatePack(VOID);
	~UpdatePack(VOID);

public:
	//!		编码到数据集
	BOOL	Code(DBWriteSet &setWriteDB);
	//!		从数据集解码
	BOOL	DeCode(DBReadSet &setReadDB);

	//!		设置更新包的服务器类型和版本号
	VOID	SetBaseInfo(DWORD dwGappType, LPCSTR pVersion, LPCSTR pReadMe);
	//!		获得服务器类型
	DWORD	GetGappType(VOID);
	//!		获得版本号
	LPCSTR	GetVersion(VOID);
	//!		得到说明文本
	LPCSTR	GetReadMe(VOID);

protected:
	//!		递归的编码接口
	VOID	Code(DBWriteSet &setWriteDB, tagDirectory &Directory);
	//!		递归的解码接口
	VOID	DeCode(DBReadSet &setReadDB, tagDirectory &Directory);

protected:
	//! 保护的属性
	/////////////////////////////////////////////////////////////////////////////////////////
	//!				更新目标服务器类型
	DWORD			m_dwGappType;
	//!				更新包版本号
	char			m_szGappVersion[MAX_GAPP_NAME_SZIE];

	//!				更新包描述
	char			m_szReadMe[PACK_README_TEXT_SIZE];

private:
	//! 对象析构会释放数据，因此不允许对象拷贝
	UpdatePack(const UpdatePack&);
	UpdatePack& operator=(const UpdatePack&);
};