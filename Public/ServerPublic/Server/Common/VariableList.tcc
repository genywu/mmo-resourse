///
/// Some template functions of VariableList. Internal use only.
///
namespace Private
{
	typedef VariableList::Number Number;
	
	/// Define some base functions for types.
	template <typename T, long type>
	struct TypeBase
	{
		enum { TYPE = type };
		typedef T VarType;

		static VarType *Alloc( long c )
		{
			return c > 0 ? new VarType [c] : new VarType;
		}

		static void Dealloc( VarType *v, long c )
		{
			c > 0 ? delete [] v : delete v;
		}

		/// Init the var, if it's an array, set all value as all the elements value.
		static void InitValue( VarType *var, const VarType &value, long c )
		{
			if( c == 0 )
			{
				*var = value;
				return ;
			}
			for( long i = 0; i < c; ++ i )
			{
				var[i] = value;
			}
		}

		/// Set a specified element value of var.
		static void SetValue( VarType *var, const VarType &value, long index = 0 )
		{
			var[index] = value;
		}

		/// Get an element value.
		static const VarType &GetValue( VarType *var, long index = 0 )
		{
			return var[index];
		}
	};

	/// To trait the variable type from a variable value.
	template <typename T>
	struct TypeTrait;
	
	/// Number
	template <>
	struct TypeTrait<Number> : public TypeBase<Number, VariableList::VAR_NUM>
	{
		static const Number &InvalidVal()
		{
			static Number invalid = -99999999;
			return invalid;
		}
	};
	
	/// tString
	template <>
	struct TypeTrait<tString> : public TypeBase<tString, VariableList::VAR_STRING>
	{
		static const tString &InvalidVal()
		{
			static tString invalid;
			return invalid;
		}
	};
	
	/// CGUID
	template <>
	struct TypeTrait<CGUID> : public TypeBase<CGUID, VariableList::VAR_GUID>
	{
		static const CGUID &InvalidVal()
		{
			return NULL_GUID;
		}
	};

	/// TODO: can and more variable types here...
}

template <typename VarT>
inline bool VariableList::AddVar( const char *name, const VarT &initValue, long count )
{
	if( HasVar( name ) || count < 1 )
	{
		return false;
	}
	using namespace Private;
	Var *var = new Var();
	var->type = TypeTrait<VarT>::TYPE;
	var->count = count;
	var->p = TypeTrait<VarT>::Alloc( count );
	TypeTrait<VarT>::InitValue( (VarT*) var->p, initValue, count );
	AddVar( name, var );
	return true;
}

template <typename VarT>
inline const VarT &VariableList::GetVarValue( const char *name, long index )
{
	Var *var = GetVar( name );
	if( var == NULL ||
		var->type != Private::TypeTrait<VarT>::TYPE ||
		var->count <= index )
	{
		return Private::TypeTrait<VarT>::InvalidVal();
	}
	return Private::TypeTrait<VarT>::GetValue( (VarT*) var->p, index );
}

template <typename VarT>
inline bool VariableList::SetVarValue( const char *name, const VarT &value, long index )
{
	Var *var = GetVar( name );
	if( var == NULL || 
		var->type != Private::TypeTrait<VarT>::TYPE ||
		var->count <= index )
	{
		// the variable does not exist or the type does not match.
		return false;
	}
	Private::TypeTrait<VarT>::SetValue( (VarT*) var->p, value, index );
	return true;
}

template <typename VarT>
inline bool VariableList::IsValueValid( const VarT &value )
{
	return !( Private::TypeTrait<VarT>::InvalidVal() == value );
}

template <typename VarT>
inline const VarT &VariableList::GetValue( const Var *var, long index )
{
	if( var->type != Private::TypeTrait<VarT>::TYPE ||
		var->count <= index )
	{
		return Private::TypeTrait<VarT>::InvalidVal();
	}
	const VarT *varPtr = (const VarT*) var->p;
	return varPtr[index];
}
