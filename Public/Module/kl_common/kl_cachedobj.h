///
/// @file kl_cachedobj.h
/// @author Kevin Lynx
/// @brief A base class with a cached list to cache object.
///
#ifndef ___KL_CACHED_OBJ_H_
#define ___KL_CACHED_OBJ_H_

namespace kl_common
{
	///
	/// A simple object pool, inherite this class, and it will provide you
	/// the cached list and some create/destroy functions.
	///
	template <typename DerivedTp, size_t pool_size = 512,
			 typename ListTp = std::list<DerivedTp*>, 
			 typename Alloc = std::allocator<DerivedTp> >
	class cached_obj
	{
	public:
		typedef DerivedTp derived_type;
		typedef ListTp list_type;
		typedef Alloc allocator_type;

		enum { POOL_SIZE = pool_size };
	public:
		/// Create an object of type derived_type, this function will get the
		/// object from the pool, if the pool is empty, it will allocate a new
		/// object using the allocator.
		static derived_type *create()
		{
			derived_type *obj = 0;
			if( _list.size() == 0 )
			{
				obj = _alloc.allocate( 1 );
				// i use the default constructor to init the object, instead
				// the copy constructor in alloc.construct.
				new( obj ) derived_type();
			}
			else
			{
				obj =  _list.back();
				_list.pop_back();
			}
			return obj;
		}

		/// Destroy the object, this function will push the object into the pool
		/// if the pool is not full. Otherwise free the memory of the object.
		static void destroy( derived_type *obj )
		{
			if( _list.size() < POOL_SIZE )
			{
				_list.push_back( obj );
			}
			else
			{
				_alloc.destroy( obj );
				_alloc.deallocate( obj, 1 );
			}
		}

		/// Clear the object pool, and destruct all the objects in the pool.
		static void clear()
		{
			for( list_type::iterator it = _list.begin();
					it != _list.end(); ++ it )
			{
				derived_type *obj = *it;
				_alloc.destroy( obj );
				_alloc.deallocate( obj, 1 );
			}
			_list.clear();
		}
	private:
		static list_type _list;
		static allocator_type _alloc;
 	};

	template <typename DerivedTp, size_t pool_size,
			 typename ListTp, typename Alloc>
	typename cached_obj<DerivedTp, pool_size, ListTp, Alloc>::list_type
	cached_obj<DerivedTp, pool_size, ListTp, Alloc>::_list;

	template <typename DerivedTp, size_t pool_size,
			 typename ListTp, typename Alloc>
	typename cached_obj<DerivedTp, pool_size, ListTp, Alloc>::allocator_type 
	cached_obj<DerivedTp, pool_size, ListTp, Alloc>::_alloc;
}

#endif

