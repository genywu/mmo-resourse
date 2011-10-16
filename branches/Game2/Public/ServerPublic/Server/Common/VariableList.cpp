///
/// @file VariableList.cpp
/// @author Kevin Lynx
/// @brief VariableList is a variables table, to store some variables.
///
#include "stdafx.h"
#include "VariableList.h"

//
// Anytime you want to add a new variable type, you must add a new TypeTrait class
// in VariableList.tcc file, and modify the fu`cking function below.
//
static void DeallocValue( long type, void *p, long c )
{
	using namespace Private;
	if( type == VariableList::VAR_NUM )
	{
		TypeTrait<Number>::Dealloc( (Number*) p, c );
	}
	else if( type == VariableList::VAR_STRING )
	{
		TypeTrait<tString>::Dealloc( (tString*) p, c );
	}
	else if( type == VariableList::VAR_GUID )
	{
		TypeTrait<CGUID>::Dealloc( (CGUID*) p, c );
	}
}

VariableList::VariableList()
{
}

VariableList::~VariableList()
{
	RemoveAll();
}

bool VariableList::HasVar( const char *name ) const
{
	VarTableT::const_iterator it = m_VarTable.find( name );
	return it != m_VarTable.end();
}

void VariableList::AddVar( const char *name, Var *var )
{
	m_VarTable.insert( std::make_pair( name, var ) );
}

void VariableList::RemoveVar( const char *name )
{
	VarTableT::iterator it = m_VarTable.find( name );
	if( it == m_VarTable.end() )
	{
		return ;
	}
	Var *var = it->second;
	DeallocValue( var->type, var->p, var->count );
	delete var;
	m_VarTable.erase( it );
}

void VariableList::RemoveAll()
{
	for( VarTableT::iterator it = m_VarTable.begin();
		it != m_VarTable.end(); ++ it )
	{
		Var *var = it->second;
		DeallocValue( var->type, var->p, var->count );
		delete var;
	}
	m_VarTable.clear();
}

VariableList::Var *VariableList::GetVar( const char *name )
{
	VarTableT::iterator it = m_VarTable.find( name );
	return it == m_VarTable.end() ? NULL : it->second;
}
