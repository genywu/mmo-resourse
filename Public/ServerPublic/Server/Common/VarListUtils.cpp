///
/// @file VarListUtils.cpp
///
#include "stdafx.h"
#include "VarListUtils.h"
#include "VariableList.h"
#include "../../../tinyxml/XMLUtils.h"

namespace VarList
{
	/// The default encoder support Number and CGUID.
	template <typename VarType>
	struct TypeEncoder
	{
		static void Encode( DBWriteSet &db, const void *rawPtr, long count )
		{
			const VarType *var = (const VarType*) rawPtr;
			for( long i = 0; i < count; ++ i )
			{
				db.AddToByteArray( var[i] );
			}
		}
		static void Encode( std::vector<uchar> *buf, const void *rawPtr, long count )
		{
			const VarType *var = (const VarType*) rawPtr;
			for( long i = 0; i < count; ++ i )
			{
				_AddToByteArray( buf, var[i] );
			}
		}
	};

	/// Support tString.
	template <>
	struct TypeEncoder<tString>
	{
		static void Encode( DBWriteSet &db, const void *rawPtr, long count )
		{
			const tString *var = (const tString*) rawPtr;
			for( long i = 0; i < count; ++ i )
			{
				db.AddToByteArray( var[i].Str() );
			}
		}
		static void Encode( std::vector<uchar> *buf, const void *rawPtr, long count )
		{
			const tString *var = (const tString*) rawPtr;
			for( long i = 0; i < count; ++ i )
			{
				_AddToByteArray( buf, var[i].Str() );
			}
		}
	};

	const char *VarName( const char *prefixName )
	{
		char c = prefixName[0];
		if( c != '$' && c != '#' && c != '@' )
		{
			return NULL;
		}
		return prefixName + 1;
	}

	long VarType( const char *prefixName )
	{
		char c = prefixName[0];
		switch( c )
		{
		case '#':
			return VariableList::VAR_STRING;
		case '$':
			return VariableList::VAR_NUM;
		case '@':
			return VariableList::VAR_GUID;
		default:
			return VariableList::VAR_INVALID;
		}
		return VariableList::VAR_INVALID;
	}

	void Encode( const VariableList *varList, DBWriteSet &db )
	{
		const VariableList::VarTableT &varTable = varList->GetVarTable();
		db.AddToByteArray( (long) varTable.size() );
		for( VariableList::VarTableT::const_iterator it = varTable.begin();
				it != varTable.end(); ++ it )
		{
			db.AddToByteArray( it->first.c_str() );
			const VariableList::Var *var = it->second;
			db.AddToByteArray( var->type );
			db.AddToByteArray( var->count );
			if( var->type == VariableList::VAR_NUM )
			{
				TypeEncoder<VariableList::Number>::Encode( db, var->p, var->count );
			}
			else if( var->type == VariableList::VAR_STRING )
			{
				TypeEncoder<tString>::Encode( db, var->p, var->count );
			}
			else if( var->type == VariableList::VAR_GUID )
			{
				TypeEncoder<CGUID>::Encode( db, var->p, var->count );
			}
		}
	}

	void Encode( const VariableList *varList, std::vector<uchar> *buf )
	{
		const VariableList::VarTableT &varTable = varList->GetVarTable();
		_AddToByteArray( buf, (long) varTable.size() );
		for( VariableList::VarTableT::const_iterator it = varTable.begin();
				it != varTable.end(); ++ it )
		{
			_AddToByteArray( buf, it->first.c_str() );
			const VariableList::Var *var = it->second;
			_AddToByteArray( buf, var->type );
			_AddToByteArray( buf, var->count );
			if( var->type == VariableList::VAR_NUM )
			{
				TypeEncoder<VariableList::Number>::Encode( buf, var->p, var->count );
			}
			else if( var->type == VariableList::VAR_STRING )
			{
				TypeEncoder<tString>::Encode( buf, var->p, var->count );
			}
			else if( var->type == VariableList::VAR_GUID )
			{
				TypeEncoder<CGUID>::Encode( buf, var->p, var->count );
			}
		}
	}

	void Decode( VariableList *varList, DBReadSet &db )
	{
		long size = db.GetLongFromByteArray();
		char name[256];
		for( long i = 0; i < size; ++ i )
		{
			db.GetStringFromByteArray( name, sizeof( name ) );
			long type = db.GetLongFromByteArray();
			long count = db.GetLongFromByteArray();
			if( type == VariableList::VAR_NUM )
			{
				varList->AddVar( name, (VariableList::Number)0, count );
			}
			else if( type == VariableList::VAR_STRING )
			{
				varList->AddVar( name, tString(), count );
			}
			else if( type == VariableList::VAR_GUID )
			{
				varList->AddVar( name, NULL_GUID, count );
			}
			for( long i = 0; i < count; ++ i )
			{
				if( type == VariableList::VAR_NUM )
				{
					VariableList::Number val = (VariableList::Number)db.GetDoubleFromByteArray();
					varList->SetVarValue( name, val, i );
				}
				else if( type == VariableList::VAR_STRING )
				{
					char val[512];
					db.GetStringFromByteArray( val, sizeof( val ) );
					varList->SetVarValue( name, tString( val ), i );
				}
				else if( type == VariableList::VAR_GUID )
				{
					CGUID val;
					db.GetBufferFromByteArray( val );
					varList->SetVarValue( name, val, i );
				}
			} // for
		} // for
	} // Decode

	void Decode( VariableList *varList, const uchar *buf_, size_t bufSize, long &pos )
	{
		uchar *buf = const_cast<uchar*>( buf_ );
		long size = _GetLongFromByteArray( buf, pos );
		char name[256];
		for( long i = 0; i < size && pos < (long)bufSize; ++ i )
		{
			_GetStringFromByteArray( buf, pos, name );
			long type = _GetLongFromByteArray( buf, pos );
			long count = _GetLongFromByteArray( buf, pos );
			if( type == VariableList::VAR_NUM )
			{
				varList->AddVar( name, (VariableList::Number)0, count );
			}
			else if( type == VariableList::VAR_STRING )
			{
				varList->AddVar( name, tString(), count );
			}
			else if( type == VariableList::VAR_GUID )
			{
				varList->AddVar( name, NULL_GUID, count );
			}
			for( long i = 0; i < count; ++ i )
			{
				if( type == VariableList::VAR_NUM )
				{
					VariableList::Number val = (VariableList::Number)_GetDoubleFromByteArray( buf, pos );
					varList->SetVarValue( name, val, i );
				}
				else if( type == VariableList::VAR_STRING )
				{
					char val[512];
					_GetStringFromByteArray( buf, pos, val );
					varList->SetVarValue( name, tString( val ), i );
				}
				else if( type == VariableList::VAR_GUID )
				{
					CGUID val;
					_GetBufferFromByteArray( buf, pos, val );
					varList->SetVarValue( name, val, i );
				}
			} // for
		} // for
	}

	VariableList *Copy( const VariableList *src, VariableList *dest )
	{
		const VariableList::VarTableT &varTable = src->GetVarTable();
		for( VariableList::VarTableT::const_iterator it = varTable.begin();
			it != varTable.end(); ++ it )
		{
			const VariableList::Var *var = it->second;
			const char *varName = it->first.c_str();
			if( var->type == VariableList::VAR_NUM )
			{
				dest->AddVar( varName, (VariableList::Number) 0 , var->count );
			}
			else if( var->type == VariableList::VAR_STRING )
			{
				dest->AddVar( varName, tString(), var->count );
			}
			else if( var->type == VariableList::VAR_GUID )
			{
				dest->AddVar( varName, NULL_GUID, var->count );
			}

			for( long i = 0; i < var->count; ++ i )
			{
				if( var->type == VariableList::VAR_NUM )
				{
					const VariableList::Number &val = VariableList::GetValue<VariableList::Number>( var, i );
					dest->SetVarValue( varName, val, i );
				}
				else if( var->type == VariableList::VAR_STRING )
				{
					const tString &val = VariableList::GetValue<tString>( var, i );
					dest->SetVarValue( varName, val, i );
				}
				else if( var->type == VariableList::VAR_GUID )
				{
					const CGUID &val = VariableList::GetValue<CGUID>( var, i );
					dest->SetVarValue( varName, val, i );
				}
			}
		}

		return dest;
	}

	bool Load( VariableList *varList, const char *file )
	{
		CRFile *rFile = rfOpen( file );
		if( rFile == NULL )
		{
			return false;
		}

		TiXmlDocument doc( file );
		doc.LoadData( rFile->GetData(), rFile->GetDatalen() );
		rfClose( rFile );

		TiXmlElement *root = doc.RootElement();
		if( root == NULL )
		{
			return false;
		}
		TiXmlElement *elem = root->FirstChildElement();
		for( ; elem != NULL; elem = elem->NextSiblingElement() )
		{
			const char *name;
			long count;
			const char *value;
			XML_QUERY_STR( "name", name, elem );
			XML_QUERY_NUM_DEFAULT( "array", count, elem, long, 1 );
			XML_QUERY_STR( "value", value, elem );

			long type = VarType( name );
			if( type == VariableList::VAR_NUM )
			{
				VariableList::Number val = (VariableList::Number) atof( value );
				varList->AddVar( name, val, count );
			}
			else if( type == VariableList::VAR_GUID )
			{
				varList->AddVar( name, CGUID( value ), count );
			}
			else if( type == VariableList::VAR_STRING )
			{
				varList->AddVar( name, tString( value ), count );	
			}
		}

		return true;
	}
}

