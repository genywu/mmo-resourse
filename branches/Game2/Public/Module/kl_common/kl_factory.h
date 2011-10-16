///
/// @file kl_factory.h
/// @author Kevin Lynx
/// @brief Implement factory pattern.
///
#ifndef ___KL_FACTORY_H_
#define ___KL_FACTORY_H_

#include <map>
#include "kl_macro_params.h"

namespace kl_common
{
	/// 
	/// Wrap the implemention to create products.
	///
	template <typename Id, typename Product, typename Creator, typename Destroyer>
	class Factory
	{
	public:
		typedef Id IdType;
		typedef Product ProductType;
		typedef Creator CreatorType;
		typedef Destroyer DestroyerType;

		struct LifePolicy
		{
			LifePolicy( CreatorType c, DestroyerType d ) :
				creator( c ), destroyer( d )
			{
			}
			LifePolicy() : creator( CreatorType() ), destroyer( DestroyerType() )
			{
			}
			CreatorType creator;
			DestroyerType destroyer;
		};

		typedef std::map<IdType, LifePolicy> PolicyTableType;
	public:
		Factory()
		{
			Clear();
		}

		~Factory()
		{
			Clear();
		}

		/// Clear all creators in the factory.
		void Clear()
		{
			m_Policies.clear();
		}

		/// Check whether the creator already exists.
		bool Exist( IdType id ) const
		{
			return m_Policies.find( id ) != m_Policies.end();
		}

		/// Add a creator in the factory.
		bool Add( IdType id, const LifePolicy &lp )
		{
			if( Exist( id ) )
			{
				return false;
			}
			m_Policies.insert( PolicyTableType::value_type( id, lp ) );
			return true;
		}

		bool Add( IdType id, CreatorType c, DestroyerType d )
		{
			return Add( id, LifePolicy( c, d ) );
		}

		/// Remove a creator from the factory.
		bool Remove( IdType id )
		{
			return m_Policies.erase( id ) != 0;
		}

		/// Create a product, return the pointer. If there's no creator
		/// for this product, return NULL.
		ProductType *Create( IdType id ) const
		{
			PolicyTableType::const_iterator it = m_Policies.find( id );
			if( it == m_Policies.end() )
			{
				return 0;
			}
			return it->second.creator();
		}

		/// To expand many 'create' functions to support multiple parameters creation.
	#define EXPAND_CREATE_FUNC( n ) \
		template <DEF_PARAM( n )> \
		ProductType *Create( IdType id, DEF_FUNC_PARAM_P( n ) ) \
		{ \
			PolicyTableType::const_iterator it = m_Policies.find( id ); \
			if( it == m_Policies.end() ) \
			{ \
				return 0; \
			} \
			return it->second.creator( DEF_FUNC_ARG( n ) ); \
		}

		EXPAND_CREATE_FUNC( 1 );
		EXPAND_CREATE_FUNC( 2 );
		EXPAND_CREATE_FUNC( 3 );
		EXPAND_CREATE_FUNC( 4 );
		EXPAND_CREATE_FUNC( 5 );
		EXPAND_CREATE_FUNC( 6 );
		EXPAND_CREATE_FUNC( 7 );
		EXPAND_CREATE_FUNC( 8 );
		EXPAND_CREATE_FUNC( 9 );
		EXPAND_CREATE_FUNC( 10 );
		EXPAND_CREATE_FUNC( 11 );
		EXPAND_CREATE_FUNC( 12 );
		EXPAND_CREATE_FUNC( 13 );
		EXPAND_CREATE_FUNC( 14 );
		EXPAND_CREATE_FUNC( 15 );

		/// Delete a product. If there's no deletor for this product,
		/// return false.
		bool Destroy( IdType id, ProductType *p ) const
		{
			PolicyTableType::const_iterator it = m_Policies.find( id );
			if( it == m_Policies.end() )
			{
				return false;
			}
			it->second.destroyer( p );
			return true;
		}

	private:
		PolicyTableType m_Policies;
	};
}

#endif

