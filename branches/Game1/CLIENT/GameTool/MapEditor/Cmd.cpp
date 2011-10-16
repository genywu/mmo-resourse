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

void Cmd::Execute()
{
	m_bExecuted = TRUE;
}

void Cmd::UnExecute()
{
	m_bExecuted = FALSE;
}

void Cmd::Dump()
{

}