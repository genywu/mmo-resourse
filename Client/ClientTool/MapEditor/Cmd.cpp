#include "StdAfx.h"
#include ".\cmd.h"

Cmd::Cmd(void)
{
	m_pPrev = NULL;
	m_pNext = NULL;
	m_bExecuted = FALSE;
}

Cmd::~Cmd(void)
{
}

bool Cmd::Execute()
{
	m_bExecuted = TRUE;
	return true;
}

void Cmd::UnExecute()
{
	m_bExecuted = FALSE;
}

void Cmd::Dump()
{

}