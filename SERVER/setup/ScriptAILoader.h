///
/// @file ScriptAILoader.h
/// @author Kevin Lynx
/// @brief To manage the script ai function table.
///
#ifndef ___SCRIPT_AI_LOADER_H_
#define ___SCRIPT_AI_LOADER_H_

#include <string>
#include <map>
#include <vector>

class TiXmlElement;

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

	/// load the config file
	bool Load( const std::string &file );

	/// encode the data to the byte array.
	void AddToByteArray( std::vector<unsigned char> &OutBuf );

	/// decode data from byte array
	void DecodeFromByteArray( const unsigned char *pBuf, long &lPos );

	/// query function table
	FuncTableType *QueryFuncTable( long id ) const;

private:
	CScriptAILoader() {}

	~CScriptAILoader();

	/// load one chunk
	bool LoadOneChunk( TiXmlElement *pElem );

private:
	FuncRefTableType mFuncRefTable;
};

#endif // ___SCRIPT_AI_LOADER_H_