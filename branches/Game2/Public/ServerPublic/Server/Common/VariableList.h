///
/// @file VariableList.h
/// @author Kevin Lynx
/// @brief VariableList is a variables table, to store some variables.
///
#ifndef ___VARIABLE_LIST_H_
#define ___VARIABLE_LIST_H_

#include "../../../Module/tString.h"
#include "../../../Module/GUID.h"
#include <map>
#include <string>

#ifdef VAR_TEST_ONLY
#define CGUID long
#define NULL_GUID 0
#endif

class VariableList
{
public:
	enum VarType
	{
		VAR_INVALID = -1,
		VAR_NUM,
		VAR_STRING,
		VAR_GUID,
	};

	typedef double Number;

	/// The array index is begins at 0, and the count >= 1
	struct Var
	{
		long type;
		long count;
		void *p;
	};

	/// <varName, var>
	typedef std::map<std::string, Var*> VarTableT;

public:
	VariableList();

	/// Rmove all the variables.
	~VariableList();

	///
	/// Add a variable.
	///  
	/// @param name the variable name
	/// @param initValue the initial value, if the variable is an array, it will set
	/// all the elements to this value.
	/// @param count the array's element count, 1 means the array only has one element.
	///
	template <typename VarT>
	bool AddVar( const char *name, const VarT &initValue, long count = 1 );

	/// Remove a variable.
	void RemoveVar( const char *name );

	/// Remove all variables.
	void RemoveAll();

	///
	/// Get a variable value, must specify the variable type, 
	/// i.e:
	///		const tString &str = varList.GetVarValue<tString>( "name" );
	/// If not found the variable, this function will return an invalid value which 
	/// can be checked by using IsValueValid function.
	///
	template <typename VarT>
	const VarT &GetVarValue( const char *name, long index = 0 );

	///
	/// Set a variable or an element of the array value.
	///
	template <typename VarT>
	bool SetVarValue( const char *name, const VarT &value, long index = 0 );

	/// Check whether it has a variable with name.
	bool HasVar( const char *name ) const;

	///
	/// Check whether a variable value is valid.
	/// i.e:
	///		const tString &str = varList.GetVarValue<tString>( "name" );	
	///		bool valid = VariableList::IsValudValid( str );
	///
	template <typename VarT>
	static bool IsValueValid( const VarT &value );

	///
	/// A helper function to get an element value of the variable.
	///
	template <typename VarT>
	static const VarT &GetValue( const Var *var, long index = 0 );

	///
	/// Get the original variable table.
	///
	const VarTableT &GetVarTable() const
	{
		return m_VarTable;
	}

	/// Variables count.
	size_t Count() const
	{
		return m_VarTable.size();
	}
private:
	Var *GetVar( const char *name );

	void AddVar( const char *name, Var *var );
private:
	VarTableT m_VarTable;
};

/// Some template functions are defined the file included below.
#include "VariableList.tcc"

#endif
