#include "stdafx.h"

#include "futility.h"
#include "fconsole.h"

#ifdef __CLIENT_File__
#include "..\..\gameclient\gameclient\game.h"
#include "..\..\..\public\RFile.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//__CLASS_(utility::File)_____

utility::File::File()
{	
	m_pbyData = NULL;
	ZeroMemory(m_pszFileName,MAX_FILE_NAME_SAVED);
	m_iPosition = m_iLength = 0;
}

utility::File::~File()
{
	ASSERT(!m_pbyData);//文件没有被关闭
	SAFEDELETE(m_pbyData);
}

////////////////////////////////////////////////
//功能：取得文件路径
///////////////////////////////////////////////
char *utility::File::GetFilePath(const char *szFileName, char* szOutput)//不带"/" 和 "\\"
{
	int iLen=lstrlen(szFileName);
	strcpy(szOutput, szFileName);

	for(int i = iLen-1; i >= 0; i--)
	{
		szOutput[i]=0;

		if( szFileName[i] == '\\' || szFileName[i] == '/' )
			break;		
	}
	if (szOutput[0] == 0)
	{
		szOutput[0] = '.';
		szOutput[1] = 0;
	}

	return szOutput;
}

BOOL utility::File::Open(const char *pszFileName)
{
	ASSERT(!m_pbyData);

#ifdef __CLIENT_File__
	strncpy((char*)m_pszFileName,pszFileName,MAX_FILE_NAME_SAVED - 1);
	m_pszFileName[MAX_FILE_NAME_SAVED-1] = 0;

	CRFile* prfile = rfOpen(pszFileName);
	if (!prfile)
	{
		OutputError("warring: File: open \"%s\" failed.\n",pszFileName);	
		return FALSE;
	}

	m_iLength = prfile->GetDatalen();

	if (m_iLength<=0)
	{
		ASSERT(m_iLength);
		OutputError("warring: File: \"%s\" length == 0.\n",pszFileName);

		rfClose(prfile);
		return FALSE;
	}

	m_pbyData = new unsigned char [m_iLength];

	bool bRead = prfile->ReadData(m_pbyData, m_iLength);

	VERIFY(bRead);//读取数据失败

	m_iPosition = 0;

	rfClose(prfile);

	return TRUE;
#else
	FILE *pFile(NULL);	
	strncpy((char*)m_pszFileName,pszFileName,MAX_FILE_NAME_SAVED - 1);
	m_pszFileName[MAX_FILE_NAME_SAVED-1] = 0;
	pFile = fopen(pszFileName,"rb");						//先打开原始文件
	if (!pFile)
	{
		OutputWarring("warring: FFile: open \"%s\" failed.\n",pszFileName);	
		return FALSE;
	}

	fseek(pFile,0,SEEK_END);
	m_iLength = ftell(pFile);
	fseek(pFile,0,SEEK_SET);

	if (!m_iLength)
	{
		ASSERT(m_iLength);
		OutputWarring("warring: FFile: \"%s\" length == 0.\n",pszFileName);   
		return FALSE;
	}
	m_pbyData = new unsigned char [m_iLength];
	long lReadSize = (long)fread(m_pbyData,1,m_iLength,pFile);
	VERIFY(lReadSize == m_iLength);//读取数据失败
	m_iPosition = 0;
	fclose(pFile);
	return TRUE;
#endif

}

void utility::File::Close()
{
	SAFEDELETEARRAY(m_pbyData);
	m_iPosition = 0;
	m_iLength = -1;
}

int utility::File::Seek(int iPosition,int iOrigin)
{
	int iCurPosition = m_iPosition;
	switch(iOrigin)
	{
	case SEEK_CUR:
		m_iPosition +=iPosition;
		break;
	case SEEK_END:
		m_iPosition  = m_iLength + iPosition;// 必须为负数
		ASSERT(m_iPosition >= 0);
		break;
	case SEEK_SET:
	default:
		m_iPosition= iPosition;
	}
	return iCurPosition;
}

char * utility::File::ReadLine(char * pszLineBuffer, int iBufferLen)
{
	int  iLineLen = 0;
	BOOL bCount = FALSE;

	ZeroMemory(pszLineBuffer,iBufferLen);

	//从当前为止读取字符
	while(m_iPosition < m_iLength && iLineLen < iBufferLen - 1)
	{
		char c = m_pbyData[m_iPosition++];
		if ( '\r' == c )
		{
			if (m_iPosition+1 < m_iLength)
			{
				c = m_pbyData[m_iPosition+1];
				if ( '\n' == c )
				{
					m_iPosition++;
				}
			}
			break;
		}
		else
			if ( '\n' == c )
			{
				break;
			}

			pszLineBuffer[iLineLen++] = c;

	}

	return pszLineBuffer;
}

int utility::File::Read(void *pbyBuffer, int iSize)
{
	int iLeft = m_iLength - m_iPosition;
	int iWantReadSize  = (iSize > iLeft)? iLeft : iSize;
	ASSERT(iWantReadSize>0);

	memcpy(pbyBuffer,m_pbyData+m_iPosition,iWantReadSize);

	m_iPosition +=iWantReadSize;

	return iWantReadSize;
}

BOOL utility::File::ReadChar(unsigned char &c)
{
	if (m_iPosition >= m_iLength) return FALSE;//越界返回

	c = m_pbyData[m_iPosition++];	
	return TRUE;
}

void utility::File::SkipBytes(int iBytes)
{
	m_iPosition += iBytes;
}
//___CLASS__(utility::Text)____
const char szDefaultSpliter[16] = " \r\n\t;,";
//___CLASS__(utility::Text::Stream)___________
void utility::Text::Stream::Create(const BYTE *pData,int iLength)
{
	ASSERT(iLength > 0);//make sure script length > 0

	m_pData = new BYTE[iLength + 1];
	memcpy(m_pData,pData,iLength);
	m_pData[iLength] = '\0';

	m_iLength = iLength;
	m_iPosition = 0;
}

void utility::Text::Stream::Destroy()
{
	SAFEDELETE(m_pData);
}

const char *utility::Text::Stream::GetWord()
{
	int  iWordLen = 0;
	BOOL bCount = FALSE;

	ZeroMemory(m_pTempWord,_MAX_STEAM_WORD_LENGTH);

	while((m_iPosition< m_iLength && iWordLen < _MAX_STEAM_WORD_LENGTH - 1 ))
	{
		char c = m_pData[m_iPosition]; 

		if (!bCount)
		{
			if (!strchr(szDefaultSpliter,c))
				bCount = TRUE;
		}

		if (bCount)
		{
			//开始记数
			if (strchr(szDefaultSpliter,c))
				break;//结束
			m_pTempWord[iWordLen++] = c;
		}

		m_iPosition++;
	}
	return m_pTempWord;
}


void utility::Text::Stream::SkipWord()
{
	BOOL bCount = FALSE;
	while(m_iPosition< m_iLength)
	{
		char c = m_pData[m_iPosition]; 

		if (!bCount)
		{
			if (!strchr(szDefaultSpliter,c))
				bCount = TRUE;
		}

		if (bCount)
		{
			//开始记数
			if (strchr(szDefaultSpliter,c))
				break;//结束
		}

		m_iPosition++;
	}
}

void utility::Text::Stream::SkipLine()
{
	BOOL bCount = FALSE;

	//从当前为止读取字符
	while(m_iPosition < m_iLength)
	{
		char c = m_pData[m_iPosition++];
		if ( '\r' == c )
		{
			if (m_iPosition+1 < m_iLength)
			{
				c = m_pData[m_iPosition+1];
				if ( '\n' == c )
				{
					m_iPosition++;
				}
			}
			break;
		}
		else
			if ( '\n' == c )
			{
				break;
			}
	}
}

//___CLASS__(utility::Text::Node)_____________
void utility::Text::Node::Create(utility::Text::Stream *pStream)
{
	ASSERT(pStream);

	static char szPrevWord[_MAX_STEAM_WORD_LENGTH];
	static char szWord[_MAX_STEAM_WORD_LENGTH];
	static char szTemp[_MAX_STEAM_WORD_LENGTH];

	ZeroMemory(szPrevWord,_MAX_STEAM_WORD_LENGTH);

	for(;;)
	{
		if (pStream->EndOFStream())
			return;

		strncpy(szWord,pStream->GetWord(),_MAX_STEAM_WORD_LENGTH - 1);
		szWord[_MAX_STEAM_WORD_LENGTH - 1] = 0;


		eCMD cmd = CheckCmd(szWord);

		switch(cmd)
		{
		case CMD_COMMENT:
			pStream->SkipLine();
			break;
		case CMD_NEWNODE:
			{
				Node * pNode = new Node;
				pNode->SetName(szPrevWord);
				pNode->Create(pStream);
				AddChildNode(pNode);
			}
			break;
		case CMD_EQUAL:
			{
				Variable *pVar = new Variable;
				pVar->SetName(szPrevWord);
				strncpy(szTemp,pStream->GetWord(),_MAX_STEAM_WORD_LENGTH - 1);
				szTemp[_MAX_STEAM_WORD_LENGTH - 1] = 0;
				pVar->SetValue(szTemp);
				AddVar(pVar);
			}
			break;
		case CMD_ENDNODE:
			return ;
		case CMD_UNKNOWN:
		default:
			strncpy(szPrevWord,szWord,_MAX_STEAM_WORD_LENGTH - 1);
			szPrevWord[_MAX_STEAM_WORD_LENGTH - 1] = 0;
			break;
		}
	}
}
void utility::Text::Node::SetName(const char *pszName)
{
	strncpy(m_szName,pszName,_MAX_STEAM_WORD_LENGTH - 1);
	m_szName[_MAX_STEAM_WORD_LENGTH - 1] = 0;
}

void utility::Text::Node::AddChildNode(utility::Text::Node *pNode)
{
	m_pChildNodeArray.push_back(pNode);
}

void utility::Text::Node::AddVar(utility::Text::Variable *pVar)
{
	m_pVarArray.push_back(pVar);
}

utility::Text::Variable * utility::Text::Node::GetVar(const char *pszVarName)
{
	//释放变量
	std::vector<Variable *>::iterator itVar;
	for(itVar = m_pVarArray.begin(); itVar != m_pVarArray.end(); itVar++)
	{
		Variable *pVar = *itVar;
		if (stricmp(pszVarName,pVar->GetName()) == 0) 
		{
			return pVar;
		}
	}

	//#ifdef _DEBUG
	//Not found
	//	OutputConsole("warring: FText:Node: [%s] : \"%s\" not found in varialbe array.\n",GetName(),pszVarName);
	//#endif
	return NULL;
}

void utility::Text::Node::Destroy()
{
	//释放节点
	std::vector<Node *>::iterator itNode;
	for(itNode = m_pChildNodeArray.begin(); itNode != m_pChildNodeArray.end(); itNode++)
	{
		Node *pNode = *itNode;

		SAFEDESTROY(pNode);
	}

	m_pChildNodeArray.clear();

	//释放变量
	std::vector<Variable *>::iterator itVar;
	for(itVar = m_pVarArray.begin(); itVar != m_pVarArray.end(); itVar++)
	{
		Variable *pVar = *itVar;

		SAFEDELETE(pVar);
	}
	m_pVarArray.clear();

}

utility::Text::Node::eCMD utility::Text::Node::CheckCmd(const char *pszWord)
{
	if (stricmp(pszWord,"{") == 0) return CMD_NEWNODE;
	if (stricmp(pszWord,"=") == 0) return CMD_EQUAL;
	if (stricmp(pszWord,"}") == 0) return CMD_ENDNODE;
	if (strstr(pszWord,"//") == pszWord) return CMD_COMMENT;
	return CMD_UNKNOWN;
}

utility::Text::Node *utility::Text::Node::GetChildNode(const char *pszNodeName)
{
	//释放节点
	std::vector<Node *>::iterator itNode;
	for(itNode = m_pChildNodeArray.begin(); itNode != m_pChildNodeArray.end(); itNode++)
	{
		Node *pNode = *itNode;

		if (stricmp(pszNodeName,pNode->GetName()) == 0) 
		{
			return pNode;
		}
	}
	//#ifdef _DEBUG
	//	OutputConsole("error: FText:Node: \"%s\" not found in childnode array.\n",pszNodeName);
	//#endif
	return NULL;
}

void utility::Text::Node::ExportScript(char *pszBuffer)
{
	strcat(pszBuffer,m_szName);
	strcat(pszBuffer,"\n{\n");
	int i = 0;
	for(; i < (int)m_pVarArray.size();i++)
	{
		utility::Text::Variable *pVar = m_pVarArray[i];
		strcat(pszBuffer,pVar->GetName());
		strcat(pszBuffer," = ");
		strcat(pszBuffer,pVar->GetStringValue());
		strcat(pszBuffer,";\n");
	}

	for(i = 0; i < (int)m_pChildNodeArray.size(); i++)
	{
		utility::Text::Node *pNode = m_pChildNodeArray[i];
		pNode->ExportScript(pszBuffer);
	}

	strcat(pszBuffer,"}\n");
}  

void utility::Text::Node::ExportContent(char *pszBuffer)
{
	int i = 0;
	for(; i < (int)m_pVarArray.size();i++)
	{
		utility::Text::Variable *pVar = m_pVarArray[i];
		strcat(pszBuffer,pVar->GetName());
		strcat(pszBuffer," = ");
		strcat(pszBuffer,pVar->GetStringValue());
		strcat(pszBuffer,";\n");
	}

	for(i = 0; i < (int)m_pChildNodeArray.size(); i++)
	{
		utility::Text::Node *pNode = m_pChildNodeArray[i];
		pNode->ExportScript(pszBuffer);
	}
}

//___CLASS__(utility::Text::Variable)_________
void utility::Text::Variable::SetName(const char *pszName)
{
	strncpy(m_szName,pszName,_MAX_STEAM_WORD_LENGTH);
}
void utility::Text::Variable::SetValue(const char *pszValue)
{
	strncpy(m_szValue,pszValue,_MAX_STEAM_WORD_LENGTH);
}

BOOL  utility::Text::Variable::GetBOOLValue() const
{
	if (stricmp(m_szValue,"on") ==0 ||
		stricmp(m_szValue,"true") == 0 ||
		stricmp(m_szValue,"yes") == 0) 
		return TRUE;
	else
		return FALSE;
}
DWORD utility::Text::Variable::GetHexValue()
{
	//转换16进制到10
	DWORD dwValue = 0;
	int p = 0;
	int l = lstrlen(m_szValue) - 1;
	while(l >= 0)
	{
		BYTE c = m_szValue[l];      
		if (c >='0' && c <='9')
		{
			c = c - '0';
		}
		else
			if (c >= 'a' && c <='f')
			{
				c = c - 'a' + 10;
			}
			else
				if (c >='A' && c <= 'F')
				{
					c = c - 'A' + 10;
				}
				else 
				{
					l--;
					continue;
				}
				dwValue += (DWORD)c * (DWORD)pow((float)16,p);;
				l--;p++;
	}
	return dwValue;
}

DWORD utility::Text::Variable::GetEnumValue(const char * szEnumString[], DWORD dwEnumMax)
{
	for(DWORD dwEunmIndex = 0; dwEunmIndex < dwEnumMax; dwEunmIndex++)
	{
		if (stricmp(szEnumString[dwEunmIndex],m_szValue) == 0)
			return dwEunmIndex;
	}
	return 0xFFFFFFFF;
}

//___CLASS__(utility::Text::Script)_____________
utility::Text::Text(void)
{
	m_pRootNode= NULL;

}

utility::Text::~Text(void)
{
	ASSERT(!m_pRootNode);
}

void utility::Text::Create(utility::File * pFile)
{
	Stream stream;
	stream.Create(pFile->GetData(),pFile->GetLength());

	//#if _DEBUG
	//OutputConsole("utility::Text: Parse \"%s\"\n",pFile->GetPathName());
	//#endif

	m_pRootNode = new Node;
	m_pRootNode->SetName("main");
	m_pRootNode->Create(&stream);

	stream.Destroy();
}

void utility::Text::Create(const char *pszText)
{
	int iLen = lstrlen(pszText);

	Stream stream;
	stream.Create((const BYTE *)pszText,iLen);

	m_pRootNode = new Node;
	m_pRootNode->SetName("main");
	m_pRootNode->Create(&stream);

	stream.Destroy();
}

void utility::Text::Destroy()
{
	SAFEDESTROY(m_pRootNode);
}


////////////////////////////////////////////////
//功能：取得文件名
///////////////////////////////////////////////
char *utility::File::GetFileName(const char *szFileName)//不带"/" 和 "\\"
{
	const char *p = strrchr(szFileName,'\\');
	if (!p)
	{
		p = strrchr(szFileName,'/');

		if (p)
		{
			return const_cast<char*>(p) + 1;
		}
	}
	else
	{
		return const_cast<char*>(p) + 1;

	}

	return NULL;
}

BOOL utility::File::FileExist(const char *szFileName)
{
	struct _finddata_t c_file;
	intptr_t hFile;
	bool bExist = ((hFile = _findfirst( szFileName, &c_file )) != -1L );

#ifdef ___CLIENT___
	if (!bExist)
	{
		bExist = GetGame()->IsFileExist(szFileName);
	}
#endif

	return bExist;
}