// Fox@20080929----------------------------------------------
// File:    MsgRegister.h
// Brief:   CMsgRegister用于消息注册.
//
// Author:  Fox (yulefox@gmail.com)
// Date:    Sep.29, 2008
// Fox@20080929----------------------------------------------

#ifndef _NET_MSG_ADAPTER_H_
#define _NET_MSG_ADAPTER_H_

#define DECLARE_MSG_MAP() \
protected: \
    void RegisterMsgFunc();

#define INITIAL_MSG_MAP() \
	RegisterMsgFunc();

#define ON_REGISTER_MEM_FUNC(msg, memFxn) \
		REG_MSG_MEM_FUNC(msg, this, memFxn); \

#define BEGIN_MSG_MAP(theClass) \
    void theClass::RegisterMsgFunc() \
    { \

#define END_MSG_MAP() \
    }

#define REG_MSG_FUNC(nMsgType, MsgFunc) \
	CMsgRegister::RegisterCallFunc(nMsgType, MsgFunc); \

#define REG_MSG_MEM_FUNC(nMsgType, Obj, MsgFunc) \
	CMsgRegister::RegisterCallFunc(nMsgType, Obj, MsgFunc); \

class CMessage;

class CHandler
{
public:
	virtual int operator()(CMessage* pMsg) = 0;
};

template<typename FuncType>
class CDefHandler : public CHandler
{
public:
	CDefHandler(){}
	CDefHandler(FuncType &Func)
		: m_Func(Func)
	{
	}

	virtual int operator()(CMessage* pMsg)
	{
		return m_Func(pMsg);
	}

protected:
	FuncType	m_Func;
};

template<typename ObjType, typename FuncType>
class CMemHandler : public CHandler
{
public:
	CMemHandler(){}
	CMemHandler(ObjType*const pObj, FuncType Func)
		: m_pObj(pObj)
		, m_Func(Func)
	{
	}

	virtual int operator()(CMessage* pMsg)
	{
		return (m_pObj->*m_Func)(pMsg);
	}

protected:
	FuncType		m_Func;
	ObjType*const	m_pObj;
};

class CFunction
{
public:
	CFunction()
		: m_pHandler(NULL)
	{
	}

	// 封装(C函数或静态成员函数)
	template<typename FuncType>
	CFunction( FuncType &Func )
		: m_pHandler(new CDefHandler<FuncType>(Func))
	{
	}

	// 封装(非静态成员函数)
	template<typename ObjType, typename FuncType>
	CFunction( ObjType*const pObj, FuncType Func )
		: m_pHandler(new CMemHandler<ObjType, FuncType>(pObj, Func))
	{
	}

	virtual ~CFunction()
	{
		DELETE_SAFE(m_pHandler);
	}

		// 函数调用
	int operator()(CMessage* pMsg)
	{
		return (*m_pHandler)(pMsg);
	}

private:
	CHandler	*m_pHandler;
};

typedef std::map<int, CFunction*> MSG_MAP;
typedef MSG_MAP::iterator MSG_ITR;

class CMsgRegister
{
public:
	// 注册消息函数(C函数或静态成员函数)
	template <typename FuncType>
	inline static void RegisterCallFunc(int nMsgType, FuncType &Func)
	{
		CFunction *func = new CFunction(Func);
		s_MsgMap[nMsgType] = func;
	}

	// 注册消息函数(非静态成员函数)
	template <typename ObjType, typename FuncType>
	inline static void RegisterCallFunc(int nMsgType, ObjType* pObj, FuncType Func)
	{
		CFunction *func = new CFunction(pObj, Func);
		s_MsgMap[nMsgType] = func;
	}

	// 执行消息
	inline static void RunCallFunc(int nMsgType, CMessage* pMsg)
	{
		MSG_ITR itr = s_MsgMap.find(nMsgType);
		if( s_MsgMap.end() != itr )
		{
			(*itr->second)(pMsg);
		}
	}

	static void ReleaseMsgMap()				// 释放消息映射表
	{
		MSG_ITR itr = s_MsgMap.begin();
		while( itr != s_MsgMap.end() )
		{
			DELETE_SAFE(itr->second);
			itr = s_MsgMap.erase(itr);
		}
	}

protected:
	static MSG_MAP			s_MsgMap;		// 消息映射表
};

#endif