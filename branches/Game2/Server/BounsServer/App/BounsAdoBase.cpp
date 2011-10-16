#include "StdAfx.h"
#include "BounsAdoBase.h"

extern void AddLogText(char*, ...);

CBounsAdoBase::CBounsAdoBase(string strConnectionString)
{
	m_strConnectionString = strConnectionString;
}

CBounsAdoBase::~CBounsAdoBase(void)
{
}


bool CBounsAdoBase::OpenCn()
{
	if( !SUCCEEDED( m_cnPtr.CreateInstance(__uuidof(Connection)) ) )
	{
		return false;
	}
	try
	{
		if(SUCCEEDED(m_cnPtr->Open(m_strConnectionString.c_str(), "", "", adConnectUnspecified)))
		{
			return true;
		}
	}
	catch(_com_error &e)
	{
		m_cnPtr.Release();
		m_cnPtr = NULL;
		PrintErr("CBounsAdoBase OpenCn Error:", e);
	}
	return false;
}

bool CBounsAdoBase::CloseCn()
{
	try
	{
		if(m_cnPtr)
		{
			if(m_cnPtr->GetState() == adStateOpen)
			{
				m_cnPtr->Close();
			}
		}
	}
	catch(_com_error &e)
	{
		PrintErr("CBounsAdoBase CloseCn Error:", e);
		return false;
	}

	if(m_cnPtr) 
	{
		m_cnPtr.Release();
	}
	return true;
}

bool CBounsAdoBase::ExcuteSQL(LPCSTR sql)
{
	return true;
}

void CBounsAdoBase::PrintErr(LPCSTR msg)
{	
	AddLogText("Connetcion Executing : %s",msg);
}

void CBounsAdoBase::PrintErr(LPCSTR msg, _com_error &e)
{
	char buf[1024];

	if (e.Error() == 0x80004005)
	{
		wsprintf( buf, "%s (“ª∞„–‘Õ¯¬Á¥ÌŒÛ)", msg);
	}
	else
	{
		wsprintf( buf, "%s\r\n \
					   Code = %08lx\r\n \
					   Cnode meaning = %d\r\n \
					   Source = %s\r\n \
					   Description = %s\r\n",
					   msg ,e.Error(), e.ErrorMessage(), (LPCSTR)((_bstr_t)e.Source()), (LPCSTR)((_bstr_t)e.Description()));
	}

	AddLogText(buf);
}