#pragma once

#pragma warning (disable : 4183)

#define DECLARE_SINGLETON(ClassName); \
private: \
	static ClassName * m_pInstance; \
public: \
	static void InitInstance() \
	{ \
		m_pInstance = new ClassName; \
	} \
	static void ExitInstance() \
	{ \
		delete m_pInstance; \
	} \
	static ClassName * GetInstance() \
	{ \
		return m_pInstance; \
	}\


#define IMPLEMENT_SINGLETON(ClassName); \
ClassName * ClassName::m_pInstance = NULL; 
