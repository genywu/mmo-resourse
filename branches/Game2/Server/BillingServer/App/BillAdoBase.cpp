#include "stdafx.h"
#include "BillAdoBase.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--------------------------------------------------------------------------------
BillAdoBase::BillAdoBase(string strConnectionString)
{
	m_strConnectionString.clear();
	m_strConnectionString = strConnectionString;
}

//--------------------------------------------------------------------------------
BillAdoBase::~BillAdoBase(void)
{
}
   
//--------------------------------------------------------------------------------
bool BillAdoBase::OpenCn(void)
{
	if(!SUCCEEDED(m_cnPtr.CreateInstance(__uuidof(Connection))))
		return false;

	try
	{
		m_cnPtr->ConnectionTimeout = 5;
		if(SUCCEEDED(m_cnPtr->Open(m_strConnectionString.c_str(), "", "", adConnectUnspecified)))
		{
			return true;
		}
	}
	catch(_com_error &e)
	{
		m_cnPtr.Release();
		m_cnPtr = NULL;
		PrintErr("conn open error", e);
	}
	return false;
}


//--------------------------------------------------------------------------------
bool BillAdoBase::CloseCn(void)
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
		PrintErr("LogAdoBase Close cn Failed", e);
		return false;
	}

	if(m_cnPtr) m_cnPtr.Release();
	return true;
}

//--------------------------------------------------------------------------------
bool BillAdoBase::ExecuteCn(LPCSTR sql)
{
	try
	{	
		m_cnPtr->Execute(sql, NULL, adCmdText);
		return true;
	}
	catch(...)
	{
		try
		{	
			CloseCn();
			if (OpenCn())
			{
				m_cnPtr->Execute(sql, NULL, adCmdText);
				return true;
			}
		}
		catch(_com_error &e)
		{
			PrintErr("conn execute error", e);
		}

		return false;
	}
}

//--------------------------------------------------------------------------------
void BillAdoBase::PrintErr(LPCSTR msg, _com_error &e)
{
	extern void AddLogText(char*, ...);
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

//--------------------------------------------------------------------------------
void BillAdoBase::PrintErr(LPCSTR msg)
{
	extern void AddLogText(char*, ...);
	AddLogText("Connetcion Executing : %s",msg);
}