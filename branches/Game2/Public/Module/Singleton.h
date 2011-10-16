/**
 * @file    Singleton.h
 * @author  Fox (yulefox@gmail.com)
 * @date    Nov.19, 2008
 * @brief   参考Boost的singleton实现的多线程安全的单件.
 * 单线程应用使用 GetInst(CLASS_NAME) 即可,
 * 多线程时需要使用 GetMTInst(CLASS_NAME).
 * eg. GetInst(CPetCtrl).LoadConfig();
 */

#pragma once

#define GetInst(CLASS_NAME)                     \
	CSingleton<CLASS_NAME>::Inst()

#define GetMTInst(CLASS_NAME)                   \
	CMTSingleton<CLASS_NAME>::Inst()

template<typename T>
class CSingleton
{
public:
	static T& Inst()
	{
		static T obj;
		return obj;
	}

private:
	// 禁止外部调用
	CSingleton(void) {}
	~CSingleton(void) {}
	CSingleton(const CSingleton&);
	CSingleton& operator=(const CSingleton&);
};

template<typename T>
class CMTSingleton
{
public:
	// 保证在被真正使用时, obj已经被初始化.
	static T& Inst()
	{
		static T obj;
		creator.DoNothing();
		return obj;
	}

private:
	// 禁止外部调用
	CMTSingleton(void) {}
	~CMTSingleton(void) {}
	CMTSingleton(const CMTSingleton&);
	CMTSingleton& operator=(const CMTSingleton&);

	// 保证在实例化时, obj已经被初始化.
	class CInstCreator
	{
	public:
		// 保证在main进入之前, Inst()已被调用.
		CInstCreator(void) { CMTSingleton<T>::Inst(); }
		inline void DoNothing(void) const {}
	};

	static CInstCreator creator;
};

template<typename T>
typename CMTSingleton<T>::CInstCreator CMTSingleton<T>::creator;
