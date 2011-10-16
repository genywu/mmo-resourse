///
/// @file VarListUtils.h
///
#ifndef ___VARLIST_UTILS_H_
#define ___VARLIST_UTILS_H_

#include <vector>

class VariableList;

/// Provide some utils for VariableList.
namespace VarList
{
	/// Parse variable name from string like : $varName.
	const char *VarName( const char *prefixName );

	/// Parse variable type from string like : $varName.
	long VarType( const char *prefixName );

	/// Encode a variable list.
	void Encode( const VariableList *varList, DBWriteSet &db );
	void Encode( const VariableList *varList, std::vector<uchar> *buf );

	/// Decode a variable list.
	void Decode( VariableList *varList, DBReadSet &db );
	void Decode( VariableList *varList, const uchar *buf, size_t size, long &pos );

	/// Load variable list from config file.
	bool Load( VariableList *varList, const char *file );

	/// Copy variable list. Return the dest variable list.
	VariableList *Copy( const VariableList *src, VariableList *dest );
}

#endif

