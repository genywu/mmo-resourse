#include "StdAfx.h"
#include "effectsetup.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

map<DWORD, CEffectSetup::tagEffectSetup>		CEffectSetup::m_mapEffectSetup;

CEffectSetup::CEffectSetup(void)
{
}

CEffectSetup::~CEffectSetup(void)
{
	m_mapEffectSetup.clear();	
}

// 从文件读取
BOOL CEffectSetup::LoadEffectSetup(const char* filename)
{
	m_mapEffectSetup.clear();

	tagEffectSetup stEffectSetup;
	int a=255,r=0,g=0,b=0,type = 0;
	DWORD dwFS,dwFL;
	int nVal=0;
	float fVal=0.0f;

	stringstream stream;	
	CRFile* prfile = rfOpen(filename);
	if( !prfile )
	{
		char str[256];
		sprintf(str, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return FALSE;
	}
	prfile->ReadToStream(stream);
	rfClose(prfile);

	while(ReadTo(stream, "*"))
	{
		//load default setup
		stream>>stEffectSetup.dwEffectKey>>nVal>>stEffectSetup.fRadius>>type>>r>>g>>b>>dwFS>>dwFL;
		stEffectSetup.bEnable = (BOOL)nVal;
		stEffectSetup.dwColor=D3DCOLOR_ARGB(a,r,g,b);
		stEffectSetup.dwType = (int)type;
		stEffectSetup.dwFadeStartTime = dwFS;
		stEffectSetup.dwFadeLength	  = (dwFL)? dwFL: 1;
		m_mapEffectSetup[stEffectSetup.dwEffectKey]=stEffectSetup;		
	}

	return TRUE;
}