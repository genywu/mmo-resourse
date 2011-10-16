///
/// @file kl_singleton.h
/// @author Kevin Lynx
/// @date 3.11.2008
///
#ifndef ___KL_SINGLETON_H_
#define ___KL_SINGLETON_H_

namespace kl_common
{
	///
	/// This singleton class can make your job more easily whenever
	/// you want to create a singleton class.
	/// @note This class is not thread-safe.
	///
	template <typename T> 
	class singleton
	{
	public:
		singleton()
		{
#if defined( _MSC_VER ) && _MSC_VER < 1200	 
			int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
			ms_Singleton = (T*)((int)this + offset);
#else
			ms_Singleton = static_cast< T* >( this );
#endif
		}

		~singleton()
		{  
			ms_Singleton = 0;  
		}

		static T& getSingleton()
		{	
			return ( *ms_Singleton ); 
		}

		static T* getSingletonPtr()
		{ 
			return ms_Singleton; 
		}

	protected:
		static T* ms_Singleton;
	};

	template <typename T> T* singleton<T>::ms_Singleton = 0;

}

#endif // ___KL_SINGLETON_H_