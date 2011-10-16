/// class : Singleton

#ifndef _Singleton_h_
#define _Singleton_h_

/// Singleton class
template<typename TYPE, typename LOCK = Lock>
class Singleton
{
public:
	static TYPE *GetInstance();
	static void CloseInstance();

protected:
	Singleton();
	virtual ~Singleton();

	static TYPE *_instance;
	static LOCK _lock;
};


//////////////////////////////////////////
template<typename TYPE, typename LOCK>
TYPE *Singleton<TYPE, LOCK>::_instance;

template<typename TYPE, typename LOCK>
LOCK Singleton<TYPE, LOCK>::_lock;

////////////////////////////////////
template<typename TYPE, typename LOCK>
inline Singleton<TYPE, LOCK>::Singleton()
{
	_instance = (TYPE*)this;
}

template<typename TYPE, typename LOCK>
inline Singleton<TYPE, LOCK>::~Singleton()
{
	_instance = 0;
}


template<typename TYPE, typename LOCK>
inline TYPE *Singleton<TYPE, LOCK>::GetInstance()
{
	if( !_instance ) {
		Guard<LOCK> guard(_lock);
		if(	!_instance )
			new TYPE;
	}
	return _instance;
}

template<typename TYPE, typename LOCK>
inline void Singleton<TYPE, LOCK>::CloseInstance()
{
	if( _instance ) {
		Guard<LOCK> guard(_lock);
		if(	_instance )
			delete _instance;
	}
}

#endif
