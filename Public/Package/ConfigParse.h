/**
 * @file ConfigParse.h
 * @author Fox (yulefox@gmial.com)
 * @date Sep.05, 2008
 * 用于解析配置文件，每个配置文件对应一个对象。
 */

#ifndef _PUBLIC_CONFIG_PARSE_H_
#define	_PUBLIC_CONFIG_PARSE_H_

class ConfigParse
{
public:
	ConfigParse(const char* file, bool sync = true);
	~ConfigParse();

    /// 是否加载成功
	bool IsLoaded() { return NULL != m_root; }

    /// 保存文件
    bool Save() { return m_doc->SaveFile(); }

	/// 根据节点层次关系获取指定节点值
	const char* GetVal(const char* attr) const;

	/// 获取当前节点指定属性
	/// 无默认值，缺省由应用初始化时指定
	template<typename T>
	const char* GetAttr(const char* attr,
                        T val) const;

	/// 设置当前节点指定属性
    /// T 支持类型: int, const char*
	template<typename T>
	void SetAttr(const char* attr,
                        T val) const;

    /// 有默认值，缺省由应用调用时指定
	template<typename T, typename CT>
	const char* GetAttr(const char* attr,
                        T val,
                        CT def) const;

	const char* GetAttr(const char* attr) const;

	/// 获取当前节点之子节点指定属性
	/// 无默认值，缺省由应用初始化时指定
	template <typename T>
	const char* GetChildAttr(const char* tag,
                             const char* attr,
                             T val) const;

    /// 有默认值，缺省由应用调用时指定
	template<typename T, typename CT>
	const char* GetChildAttr(const char* tag,
                             const char* attr,
                             T val,
                             CT def) const;

	const char* GetChildAttr(const char* tag, const char* attr) const;

	/// 按指定路径重置当前节点
	bool SetCursor(int count, const char* attr, ...);

	/// 置根节点为当前节点
	bool SetRoot();

	/// 置父节点为当前节点
	bool SetParent();

	/// 置上一兄弟节点为当前节点
	bool SetPrevSibling(const char* name = NULL);

	/// 置下一兄弟节点为当前节点
	bool SetNextSibling(const char* name = NULL);

	/// 置首子节点为当前节点
	bool SetFirstChild(const char* name = NULL);

	/// 置末子节点为当前节点
	bool SetLastChild(const char* name = NULL);

    /**
     * 将指定index的结节置为当前节点
     * 用于单一层次类型xml文件的索引.
     * @param ind, zero-based(firstchild) index
     */
    bool SetCurNode(int ind);

    /// 删除当前节点
    void RemoveCurNode();

    /// 插入当前节点的一个副本到当前节点位置
    void CopyAndInsertCurNode();

protected:
	/// 外部不可用
	ConfigParse();

private:
	enum CONFIG_ERROR
	{
		CE_NO_ERROR	= 0,		///< 正确
		CE_ROOT_NULL,			///< 根节点为空
		CE_PARENT_NULL,			///< 父节点为空
		CE_CURRENT_NULL,		///< 当前节点为空
		CE_PRVIOUS_NULL,		///< 前一节点为空
		CE_NEXT_NULL,			///< 下一节点为空
		CE_FIRST_CHILD_NULL,	///< 首子节点为空
		CE_LAST_CHILD_NULL,		///< 末子节点为空
	};

	TiXmlDocument* m_doc;
	TiXmlElement* m_root;
	TiXmlElement* m_curElem;
};

template <typename T>
const char* ConfigParse::GetAttr(const char* attr,
                                 T val) const
{
	const char* res = NULL;
	if (NULL != m_curElem)
	{
		res = m_curElem->Attribute(attr, val);
	}

	if (NULL == res)
	{
		Log4c::Warn(ROOT_MODULE, attr);
	}

	return res;
}

template <typename T>
void ConfigParse::SetAttr(const char* attr,
                                 T val) const
{
	if (NULL != m_curElem)
	{
		m_curElem->SetAttribute(attr, val);
	}
}

template <typename T, typename CT>
const char* ConfigParse::GetAttr(const char* attr,
                                 T val,
                                 CT def) const
{
	const char* res = NULL;
	if (NULL != m_curElem)
	{
		res = m_curElem->Attribute(attr, val);
	}

	if (NULL == res)
	{
		*val = def;
		Log4c::Warn(ROOT_MODULE, attr);
	}

	return res;
}

template<typename T>
const char* ConfigParse::GetChildAttr(const char* tag,
                                      const char* attr,
                                      T val) const
{
	const char* res = NULL;
	TiXmlElement* childElem = NULL;
	if (NULL != m_curElem &&
		NULL != (childElem = m_curElem->FirstChildElement(tag)))
	{
		
		res = childElem->Attribute(attr, val);
	}

	if (NULL == res)
	{
		char str[128] = "";
		sprintf_s(str, sizeof(str), "%s/%s", tag, attr);
		Log4c::Warn(ROOT_MODULE, str);
	}

	return res;
}

template<typename T, typename CT>
const char* ConfigParse::GetChildAttr(const char* tag,
                                      const char* attr,
                                      T val,
                                      CT def) const
{
	const char* res = NULL;
	TiXmlElement* childElem = NULL;
	if (NULL != m_curElem &&
		NULL != (childElem = m_curElem->FirstChildElement(tag)))
	{
		
		res = childElem->Attribute(attr, val);
	}

	if (NULL == res)
	{
		*val = def;
		//char str[128] = "";
		//sprintf_s(str, sizeof(str), "%s/%s", tag, attr);
		//PutoutLog("Config", LT_WARNING, str);
        Log4c::Warn(ROOT_MODULE,"%s/%s", tag, attr);
	}

	return res;
}

#endif
