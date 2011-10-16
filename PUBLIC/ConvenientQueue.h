
/*
*	file:		ConvenientQueue.h
*	Brief:		可随机访问的队列
*	detail:		
*				利用map的实现
*				可随机删除
*				可随机查找
*				
*				
*	Authtor:	张科智
*	Datae:		2008
*/


#pragma once

template<typename _KeyType, typename _ValueType>
class ConvenientQueue
{
public:
	//! _ValueType类型的无效值为该类的初始值
	ConvenientQueue(VOID);
	//! 为_ValueType类型设置一个特殊的无效值
	ConvenientQueue(const _ValueType &InvalidValue);
	~ConvenientQueue(VOID);
	
public:
	//!								
	ULONG							size(VOID);
	//!								添加一个元素，键值重复，则删除以前值，并把新值放到队伍末尾
	VOID							push_back(const _KeyType &_key, const _ValueType &_value);
	//!								清除队首元素
	VOID							pop_front(VOID);
	//!								如队列为空，返回构造对象时设置的无效值
	_ValueType&						get_front(VOID);
	//!								
	VOID							erase(const _KeyType &_key);
	//!								
	BOOL							find(const _KeyType &_key);
	//!								
	_ValueType&						find_and_get(const _KeyType &_key);
	//!								
	BOOL							find_and_erase(const _KeyType &_key);
	//!								
	VOID							clear(VOID);

	//!								为每一个元素执行函数
	VOID							each_run(VOID (*pRunFunc)(_ValueType&, LPVOID), LPVOID pParam);

private:
	ULONG							GetOrder(VOID);

private:
	struct tagSaveValue
	{
		_KeyType	_key;
		_ValueType	_value;
		///
		tagSaveValue(VOID){}
		tagSaveValue(const _KeyType &in_key, const _ValueType &in_value):_key(in_key), _value(in_value){}
	};

	typedef pair<_KeyType, ULONG>		KeyToOrder;
	typedef map<_KeyType, ULONG>		MapKeyToOrder;

	typedef pair<ULONG, tagSaveValue>	OrderToValue;
	typedef map<ULONG, tagSaveValue>	MapOrderToValue;

	

	//!									Key到先后顺序的映射
	MapKeyToOrder						m_mapKeyToOrderMapping;
	//!									先后顺序到排队值的映射
	MapOrderToValue						m_mapOrderToValueMapping;
	//!									顺序种子
	ULONG								m_uOrderSeed;
	//!									表示无效的值
	_ValueType							m_InvalidValue;
};


//! 类定义
/////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename _KeyType, typename _ValueType>
ConvenientQueue<_KeyType, _ValueType>::ConvenientQueue(VOID)
:m_uOrderSeed(0)
{
}

template<typename _KeyType, typename _ValueType>
ConvenientQueue<_KeyType, _ValueType>::ConvenientQueue(const _ValueType &InvalidValue)
:m_uOrderSeed(0), m_InvalidValue(InvalidValue)
{
}

template<typename _KeyType, typename _ValueType>
ConvenientQueue<_KeyType, _ValueType>::~ConvenientQueue(VOID)
{
}

template<typename _KeyType, typename _ValueType>
inline ULONG ConvenientQueue<_KeyType, _ValueType>::size(VOID)
{
	return (ULONG)m_mapKeyToOrderMapping.size();
}

template<typename _KeyType, typename _ValueType>
inline VOID ConvenientQueue<_KeyType, _ValueType>::push_back(const _KeyType &_key, const _ValueType &_value)
{
	//! 若有重复值、则删除
	erase(_key);

	ULONG lOrder = GetOrder();
	m_mapKeyToOrderMapping.insert(KeyToOrder(_key, lOrder));
	m_mapOrderToValueMapping.insert(m_mapOrderToValueMapping.end(), OrderToValue(lOrder, tagSaveValue(_key, _value)));	
}

template<typename _KeyType, typename _ValueType>
inline VOID ConvenientQueue<_KeyType, _ValueType>::pop_front(VOID)
{
	if(0 == m_mapKeyToOrderMapping.size())
		return;
	m_mapKeyToOrderMapping.erase(m_mapOrderToValueMapping.begin()->second._key);
	m_mapOrderToValueMapping.erase(m_mapOrderToValueMapping.begin());
}

template<typename _KeyType, typename _ValueType>
inline _ValueType& ConvenientQueue<_KeyType, _ValueType>::get_front(VOID)
{
	if(0 == m_mapOrderToValueMapping.size())
		return m_InvalidValue;
	return m_mapOrderToValueMapping.begin()->second._value;
}

template<typename _KeyType, typename _ValueType>
inline VOID ConvenientQueue<_KeyType, _ValueType>::erase(const _KeyType &_key)
{
	map<_KeyType, ULONG>::iterator iteK2O = m_mapKeyToOrderMapping.find(_key);
	if(m_mapKeyToOrderMapping.end() != iteK2O)
	{
		m_mapOrderToValueMapping.erase(iteK2O->second);
		m_mapKeyToOrderMapping.erase(iteK2O);
	}
}

template<typename _KeyType, typename _ValueType>
inline BOOL ConvenientQueue<_KeyType, _ValueType>::find(const _KeyType &_key)
{
	map<_KeyType, ULONG>::iterator iteK2O = m_mapKeyToOrderMapping.find(_key);
	return (m_mapKeyToOrderMapping.end() != iteK2O);
}

template<typename _KeyType, typename _ValueType>							
inline _ValueType& ConvenientQueue<_KeyType, _ValueType>::find_and_get(const _KeyType &_key)
{
	map<_KeyType, ULONG>::iterator iteK2O = m_mapKeyToOrderMapping.find(_key);
	if(m_mapKeyToOrderMapping.end() != iteK2O)
	{
		assert(m_mapOrderToValueMapping.end() != m_mapOrderToValueMapping.find(iteK2O->second));
		return m_mapOrderToValueMapping[iteK2O->second]._value;
	}
	return m_InvalidValue;
}

template<typename _KeyType, typename _ValueType>
inline BOOL ConvenientQueue<_KeyType, _ValueType>::find_and_erase(const _KeyType &_key)
{
	map<_KeyType, ULONG>::iterator iteK2O = m_mapKeyToOrderMapping.find(_key);
	BOOL bRe = m_mapKeyToOrderMapping.end() != iteK2O;
	if(bRe)
	{
		assert(m_mapOrderToValueMapping.end() != m_mapOrderToValueMapping.find(iteK2O->second));
		m_mapOrderToValueMapping.erase(iteK2O->second);
		m_mapKeyToOrderMapping.erase(iteK2O);
	}
	return bRe;
}

template<typename _KeyType, typename _ValueType>
inline VOID ConvenientQueue<_KeyType, _ValueType>::clear(VOID)
{
	m_mapOrderToValueMapping.clear();
	m_mapKeyToOrderMapping.clear();
	m_uOrderSeed = 0;
}

template<typename _KeyType, typename _ValueType>
VOID ConvenientQueue<_KeyType, _ValueType>::each_run(VOID (*pRunFunc)(_ValueType&, LPVOID), LPVOID pParam)
{
	MapOrderToValue::iterator ite = m_mapOrderToValueMapping.begin();
	for (; ite != m_mapOrderToValueMapping.end(); ++ite)
	{
		(*pRunFunc)(ite->second._value, pParam);
	}
}

template<typename _KeyType, typename _ValueType>
ULONG ConvenientQueue<_KeyType, _ValueType>::GetOrder(VOID)
{
	if(0xFFFFFFFF == m_uOrderSeed)
	{
		//! 种子归零，从新计算顺序
		m_uOrderSeed = 0;

		MapKeyToOrder	tmpMapKeyToOrder(m_mapKeyToOrderMapping);
		MapOrderToValue tmpMapOrderToValue(m_mapOrderToValueMapping);

		m_mapKeyToOrderMapping.clear();
		m_mapOrderToValueMapping.clear();

		map<ULONG, tagSaveValue>::iterator iteO2V = tmpMapOrderToValue.begin();
		for(; iteO2V != tmpMapOrderToValue.end(); ++iteO2V)
		{
			++m_uOrderSeed;
			tmpMapOrderToValue[m_uOrderSeed] = iteO2V->second;
			assert(tmpMapKeyToOrder.end() != tmpMapKeyToOrder.find(iteO2V->second._key));
			tmpMapKeyToOrder[iteO2V->second._key] = m_uOrderSeed;
		}
	}

	++m_uOrderSeed;
	return m_uOrderSeed;
}