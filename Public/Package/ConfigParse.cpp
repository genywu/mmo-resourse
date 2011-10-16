#include "StdAfx.h"
#include "ConfigParse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ConfigParse::ConfigParse(const char* file, bool sync)
    : m_root(NULL)
    , m_curElem(NULL)
    , m_doc(NULL)
{
    if (sync)
    {
        CRFile* doc = rfOpen(file);
        if (doc != NULL)
        {
            m_doc = new TiXmlDocument(file);
            m_doc->LoadData(doc->GetData(), doc->GetDatalen());
            m_curElem = m_root = m_doc->RootElement();
        }
        rfClose(doc);
    }
    else
    {
        m_doc = new TiXmlDocument;
        if (m_doc->LoadFile(file))
        {
            m_curElem = m_root = m_doc->RootElement();
        }
    }
}

ConfigParse::ConfigParse()
{
}

ConfigParse::~ConfigParse()
{
	DELETE_SAFE(m_doc);
}

bool ConfigParse::SetCursor(int count, const char* attr, ...)
{
	va_list argptr;
	va_start(argptr, attr);

	// 从根节点处查找
	TiXmlElement* elem = m_root;

	while (NULL != elem)
	{
		m_curElem = elem;
		if (--count < 0)
			break;
		elem = elem->FirstChildElement(attr);
		attr = va_arg(argptr, const char*);
	}

	va_end(argptr);

	if (NULL == m_curElem)
	{
		Log4c::Warn(ROOT_MODULE, attr);
		return false;
	}
	return true;
}

bool ConfigParse::SetRoot()
{
	if (NULL != m_root)
	{
		m_curElem = m_root;
		return true;
	}
	return false;
}

bool ConfigParse::SetParent()
{
	if (NULL == m_curElem)
	{
		Log4c::Warn(ROOT_MODULE, "NULL == m_curElem");
		return false;
	}

	TiXmlNode* node = m_curElem->Parent();
	if (NULL != node)
	{
		m_curElem = node->ToElement();
		return true;
	}
	return false;
}

bool ConfigParse::SetPrevSibling(const char* name)
{
	return false;
}

bool ConfigParse::SetNextSibling(const char* name)
{
	if (NULL == m_curElem)
	{
		Log4c::Warn(ROOT_MODULE, "NULL == m_curElem");
		return false;
	}

	TiXmlElement* temp = NULL;
	if (NULL == name)
		temp = m_curElem->NextSiblingElement();
	else
		temp = m_curElem->NextSiblingElement(name);

	if (NULL != temp)
	{
		m_curElem = temp;
		return true;
	}
	else
		return false;
}

bool ConfigParse::SetFirstChild(const char* name)
{
	if (NULL == m_curElem)
	{
		Log4c::Warn(ROOT_MODULE, "NULL == m_curElem");
		return false;
	}

	TiXmlElement* temp = NULL;
	if (NULL == name)
		temp = m_curElem->FirstChildElement();
	else
		temp = m_curElem->FirstChildElement(name);

	if (NULL != temp)
	{
		m_curElem = temp;
		return true;
	}
	else
		return false;
}

bool ConfigParse::SetLastChild(const char* name)
{
	return false;
}

bool ConfigParse::SetCurNode(int ind)
{
    if (!SetRoot() || !SetFirstChild())
        return false;

    while (ind-- && SetNextSibling());
    if (ind >= 0) return false;

    return true;
}

const char* ConfigParse::GetAttr(const char* attr) const
{
	const char* res = NULL;
	if (NULL != m_curElem)
	{
		res = m_curElem->Attribute(attr);
	}

	if (NULL == res)
	{
		res = "";
		//Log4c::Warn(ROOT_MODULE, attr);
	}

	return res;
}

const char* ConfigParse::GetChildAttr(const char* tag,
                                      const char* attr) const
{
	const char* res = NULL;
	TiXmlElement* childElem = NULL;
	if (NULL != m_curElem &&
		NULL != (childElem = m_curElem->FirstChildElement(tag)))
	{
		
		res = childElem->Attribute(attr);
	}

	if (NULL == res)
	{
        Log4c::Warn(ROOT_MODULE,"%s/%s", tag, attr);
	}

	return res;
}

/// 删除当前节点
void ConfigParse::RemoveCurNode()
{
    if (NULL == m_curElem)
        return;

    TiXmlElement* parent = dynamic_cast<TiXmlElement*>(m_curElem->Parent());
    if (NULL == parent)
        return;

    TiXmlElement* tmp = m_curElem;
    m_curElem = dynamic_cast<TiXmlElement*>(m_curElem->NextSibling());
    parent->RemoveChild(tmp);
}

/// 插入当前节点的一个副本到当前节点位置
void ConfigParse::CopyAndInsertCurNode()
{
    if (NULL == m_curElem)
        return;

    TiXmlElement* parent = dynamic_cast<TiXmlElement*>(m_curElem->Parent());
    if (NULL == parent)
        return;

    parent->InsertBeforeChild(m_curElem, *m_curElem);
}