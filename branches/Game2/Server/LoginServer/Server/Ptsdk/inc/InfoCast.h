#pragma once
#include "sdkconfig.h"
#include "Lock\lock.h"
#include <vector>
using namespace CleverLock;
using namespace std;

class CInfoViewer;

class PTSDK_STUFF CInfoNode: public CWithLock
{
public:
	CInfoNode();
	~CInfoNode();
	
	void SetViewer(CInfoViewer * p_viewer);
	void RemoveViewer(CInfoViewer * p_viewer, bool b_backCall=true);
	void SelfOut();

	void PostInfo(int intInfo, const char * strInfo);
	void PostInfo(const char* v_format, ...);

protected:
	vector<CInfoViewer *> m_List;

private:
	static int m_tick;
	static int m_lastNow;
};

class PTSDK_STUFF CInfoViewer: public CWithLock
{
public:
	CInfoViewer();
	~CInfoViewer();
	void RegistSource(CInfoNode * p_source);
	void UnRegistSource(CInfoNode * p_source, bool b_backCall=true);
	void SelfOut();
	virtual void GotInfo(int intInfo, const char * strInfo);
protected:
	vector<CInfoNode *> m_List;
private:
};

namespace InfoType
{
enum { work, result, state};
};