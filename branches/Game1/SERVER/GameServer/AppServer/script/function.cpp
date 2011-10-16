#include "StdAfx.h"
#include "mmsystem.h"
#include "script.h"
#include "variablelist.h"
#include "../player.h"
#include "../setup/GMList.h"


double CScript::CheckParamStringForArray(const char* CmdStr, long cmdStrSize, char** MainVarName, long MainVarNameSize, char** InnVarName, long InnVarNameSize, long& countPos)
{
	int arrayCount = 0; //数组括号计数
	int innVarNum = 0; // 变量符号'$'个数
	bool isArrayVar = false; // 是否是数组

	long firstVarLen = cmdStrSize;
	//--begin 解析变量名,解析下标名或值
	int pos = 0; // 当前位置
	register int j = 0;// 变量名字符串计数值
	char tmpVarName[1024];
	(*MainVarName)[0] = '\0';
	(*InnVarName)[0]  = '\0';
	while( pos < firstVarLen ) // 读取第一个参数的字符
	{
		if( '[' == CmdStr[pos] )  // 是数组
		{
			tmpVarName[j] = '\0';
			isArrayVar = true;// 是数组
			arrayCount++;
			strcpy_s(*MainVarName, MainVarNameSize, tmpVarName);
			j = 0;
			pos++;
			continue;
		}
		else if( ']' == CmdStr[pos] || 0 == CmdStr[pos]) // 参数或者没有']'
		{ 
			tmpVarName[j] = '\0';
			arrayCount--;
			strcpy_s(*InnVarName, InnVarNameSize, tmpVarName);
			j = 0;
			countPos = pos+1; // 跳过
			pos++;
		}

		if( CmdStr[pos] != ' ' && CmdStr[pos] != '\t'
			&& CmdStr[pos] != ',' && CmdStr[pos] != ')' 
			&& CmdStr[pos] != '(' && CmdStr[pos] != ';'
			&& CmdStr[pos] != '\"'
			&& CmdStr[pos] != '[' && CmdStr[pos] != ']' )
		{
			tmpVarName[j] = CmdStr[pos];
			j++;
		}

		if(CmdStr[pos] == ',' || CmdStr[pos] == ')' ) // 读到','参数分割或者')'参数结束
		{
			tmpVarName[j] = '\0';
			if(!isArrayVar) // 不是数组
				strcpy_s(*MainVarName, MainVarNameSize, tmpVarName);
			countPos = pos+1; // 跳过
			return 0.0f;
		}	

		pos++;
	}
	if(pos == firstVarLen) // 整个字符串中无[]字符,为常量或普通变量
	{
		tmpVarName[j] = '\0';
		strcpy_s(*MainVarName, MainVarNameSize, tmpVarName);
		countPos = pos;
	}

	if(arrayCount != 0 ) // 括号不匹配
	{
		AddLogText(CStringReading::LoadString(IDS_GS_FUNCTION, STR_GS_FUNCTION_PARSEPARAMNOARRAYBLOCK));
		return -1.0f;
	} 
	//--end 解析变量名
	countPos = pos;
	if(!isArrayVar)
		return 0.0f;

	return 1.0f;
}



bool CScript::IsGM()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);

	if(pPlayer == NULL) return false;

	if(CGMList::GetInfoByName(pPlayer->GetName()) == NULL)
		return false;
	else 
		return true;
}

// 运行一条命令

double CScript::RunFunction(const char *CmdStr, char* retStr/*返回字符串指针*/)
{
	if( CmdStr == NULL )
		return 0;

	int CommandId=-1;
	long lCmdLen = 0;
	char *Cmd=GetFunctionName(CmdStr, lCmdLen);
	if ( Cmd == NULL )
		return 0;

	STRING2PFUNC::iterator it = s_FunctionList2->find(Cmd);
	if (it!=s_FunctionList2->end())
	{
		M_FREE(Cmd, lCmdLen);
		return (this->*(it->second))(CmdStr, retStr);
	}

	try
	{
		char str[1024 * 4];

		char strPlayerName[64]="";
		char strNpcName[64]="";
		if(p_SrcShape)
			_snprintf(strPlayerName, 64, "%s",p_SrcShape->GetName());
		if(p_DesShape)
			_snprintf(strNpcName, 64, "%s",p_DesShape->GetName());

		_snprintf(str, 1024 * 4, "%s (line %d) CmdStr [%s] is not define!(Player:%s,Npc:%s)", GetFileName(), m_Line, CmdStr, strPlayerName, strNpcName);
		PutDebugString(str);
		M_FREE(Cmd, lCmdLen);
	}
	catch(...)
	{
		char str[1024 * 4];
		char strPlayerName[64]="";
		char strNpcName[64]="";
		if(p_SrcShape)
			_snprintf(strPlayerName, 64, "%s",p_SrcShape->GetName());
		if(p_DesShape)
			_snprintf(strNpcName, 64, "%s",p_DesShape->GetName());

		_snprintf(str, 1024 * 4, "%s (line %d) CmdStr [%s] is not define!(Player:%s,Npc:%s)", GetFileName(), m_Line, CmdStr, strPlayerName, strNpcName);
		PutDebugString(str);
		M_FREE(Cmd, lCmdLen);
		return 0;
	}
	return 0;
}