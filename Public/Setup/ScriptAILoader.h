//======================================================================================
/**
*	@file		ScriptAILoader.h
*	@data		Kevin Lynx
*	@author		
*	@brief		管理怪物AI脚本函数表
*/
//======================================================================================
#pragma once

/**
*	@class CScriptAILoader
*
*	@brief 管理怪物AI脚本函数表
*/
class CScriptAILoader
{
public:
	/// function table [function name] - [script file]
	typedef std::map<std::string, std::string> FuncTableType;
	/// funtion table reference table [ai type] - [function table]
	typedef std::map<long, FuncTableType*> FuncRefTableType;

public:
	static CScriptAILoader &Instance()
	{
		static CScriptAILoader instance;
		return instance;
	}

	/// 加载配置表
	bool Load( const std::string &file );

	/// 编码
	void AddToByteArray( std::vector<unsigned char> &OutBuf );

	/// 解码
	void DecodeFromByteArray( const unsigned char *pBuf, long &lPos );

	/// 根据ID查询函数表
	FuncTableType *QueryFuncTable( long id ) const;

private:
	/// 构造
	CScriptAILoader() {}

	/// 析构
	~CScriptAILoader();

	/// 加载块结构
	bool LoadOneChunk( TiXmlElement *pElem );

private:
	/// 函数表
	FuncRefTableType mFuncRefTable;
};
