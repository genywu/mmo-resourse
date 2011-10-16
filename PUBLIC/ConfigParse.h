// Fox@20080917----------------------------------------------
// ConfigParse.h
// Author :		Fox(yulefox@gmial.com)
// Function :	用于解析配置文件，每个配置文件对应一个对象。
// Create :		2008-09-05
// Fox@20080917----------------------------------------------

#ifndef _PUBLIC_CONFIG_PARSE_H_
#define	_PUBLIC_CONFIG_PARSE_H_

#include "TinyXml.h"
#include "Tools.h"

class CConfigParse
{
public:
	CConfigParse(const char* file);
	~CConfigParse(void);

	bool IsLoaded(void)			// 是否加载成功
	{
		return NULL != m_pRoot;
	}

	// 根据节点层次关系获取指定节点值
	const char* GetVal(const char* attr) const;

	// 获取当前节点指定属性
	template<typename T>
	const char* GetAttr(const char* attr,
		T val) const;			// 无默认值，缺省由应用初始化时指定
	template<typename T, typename CT>
	const char* GetAttr(const char* attr,
		T val,
		CT def) const;			// 有默认值，缺省由应用调用时指定
	const char* GetAttr(const char* attr) const;

	// 获取当前节点之子节点指定属性
	template <typename T>
	const char* GetChildAttr(const char* tag,
		const char* attr,
		T val) const;			// 无默认值，缺省由应用初始化时指定
	template<typename T, typename CT>
	const char* GetChildAttr(const char* tag,
		const char* attr,
		T val,
		CT def) const;			// 有默认值，缺省由应用调用时指定
	const char* GetChildAttr(const char* tag,
		const char* attr) const;

	// 按指定路径重置当前节点
	bool SetCursor(int count, const char* attr, ...);

	// 置根节点为当前节点
	bool SetRoot();

	// 置父节点为当前节点
	bool SetParent();

	// 置上一兄弟节点为当前节点
	bool SetPrevSibling(const char* name = NULL);

	// 置下一兄弟节点为当前节点
	bool SetNextSibling(const char* name = NULL);

	// 置首子节点为当前节点
	bool SetFirstChild(const char* name = NULL);

	// 置末子节点为当前节点
	bool SetLastChild(const char* name = NULL);

protected:	// 外部不可用
	CConfigParse(void);

private:
	enum CONFIG_ERROR
	{
		CE_NO_ERROR	= 0,		// 正确
		CE_ROOT_NULL,			// 根节点为空
		CE_PARENT_NULL,			// 父节点为空
		CE_CURRENT_NULL,		// 当前节点为空
		CE_PRVIOUS_NULL,		// 前一节点为空
		CE_NEXT_NULL,			// 下一节点为空
		CE_FIRST_CHILD_NULL,	// 首子节点为空
		CE_LAST_CHILD_NULL,		// 末子节点为空
	};

	TiXmlDocument* m_pDoc;
	TiXmlElement* m_pRoot;
	TiXmlElement* m_pCurElem;
};

template <typename T>
const char* CConfigParse::GetAttr(const char* attr,
								  T val) const
{
	const char* res = NULL;
	if( NULL != m_pCurElem )
	{
		res = m_pCurElem->Attribute(attr, val);
	}

	if( NULL == res )
	{
		PutoutLog("Config", LT_WARNING, attr);
	}

	return res;
}

template <typename T, typename CT>
const char* CConfigParse::GetAttr(const char* attr,
								  T val,
								  CT def) const
{
	const char* res = NULL;
	if( NULL != m_pCurElem )
	{
		res = m_pCurElem->Attribute(attr, val);
	}

	if( NULL == res )
	{
		*val = def;
		PutoutLog("Config", LT_WARNING, attr);
	}

	return res;
}

template<typename T>
const char* CConfigParse::GetChildAttr(const char* tag,
			const char* attr,
			T val) const
{
	const char* res = NULL;
	TiXmlElement* pChildElem = NULL;
	if( NULL != m_pCurElem &&
		NULL != (pChildElem = m_pCurElem->FirstChildElement(tag) ) )
	{
		
		res = pChildElem->Attribute(attr, val);
	}

	if( NULL == res )
	{
		char str[128] = "";
		sprintf_s(str, sizeof(str), "%s/%s", tag, attr);
		PutoutLog("Config", LT_WARNING, str);
	}

	return res;
}

template<typename T, typename CT>
const char* CConfigParse::GetChildAttr(const char* tag,
			const char* attr,
			T val,
			CT def) const
{
	const char* res = NULL;
	TiXmlElement* pChildElem = NULL;
	if( NULL != m_pCurElem &&
		NULL != (pChildElem = m_pCurElem->FirstChildElement(tag) ) )
	{
		
		res = pChildElem->Attribute(attr, val);
	}

	if( NULL == res )
	{
		*val = def;
		char str[128] = "";
		sprintf_s(str, sizeof(str), "%s/%s", tag, attr);
		PutoutLog("Config", LT_WARNING, str);
	}

	return res;
}

#endif