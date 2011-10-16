/*

            ○○○○○○
			 ○                                      ○
			○○○    ○○ ○     ○○    ○○○  ○○○
		   ○  ○      ○○    ○    ○  ○        ○
		  ○          ○      ○    ○     ○     ○
	  ○○○      ○○○       ○○    ○○○    ○○

			"FROST" 3D GRAPHICS RENDERING ENGINE.

					 WRITTEN BY JIATAO.
--------------------------------------------------------------------
					COPYRIGHT 2002-2003. 

描述: 工具模块
作者: 贾涛
建立: 
修改: 2003/10/31
*/

#pragma once
#include <vector>
#include <list>
#include <assert.h>
#include <crtdbg.h>
#include "fconsole.h" 


#define MAKEFOURCTAG(x)	 (DWORD)( (((DWORD)x & 0x0000ff00) << 8) + (((DWORD)x& 0x000000ff) << 24) + (((DWORD)x & 0x00ff0000) >> 8) + (((DWORD)x& 0xff000000) >> 24) )
//#define MAKEFOURCTAG(x)	 (DWORD)( (((DWORD)x & 0x0000ff00) << 8))

#define SIGN(x) ( ((x) > 0)? 1 : ( ( (x) == 0)? 0 : -1) )

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

#define SAFEDELETE(op)         if (op) { delete op;op=NULL;}
#define SAFEDELETEARRAY(op)    if (op) { delete [] op;op=NULL;}
#define SAFERELEASE(op)        if (op) { op->Release();op = NULL;}
#define SAFEDESTROY(op)        if (op) { op->Destroy();delete op; op=NULL;}

//确认有导致程序CRUSH的错误：例如 文件读写错误,正式，调试版本
#define VERIFY(op)  if (!(op)) { console::Manager::GetInstance()->Outpute(" [VERIFY]- \"%s\"-- \"%s\"() on \"%s\" %d(Line).\n",#op,__FUNCTION__,__FILE__,__LINE__);assert(op);}

//有关乎程序运行的正确性的错误，如不该出现的数值等，仅仅调试版本
//#if _DEBUG
#define ASSERT(op)  if (!(op)) { console::Manager::GetInstance()->Outpute(" [ASSERT]- \"%s\"-- \"%s\"() on \"%s\" %d(Line).\n",#op,__FUNCTION__,__FILE__,__LINE__);assert(op);}
//#else
//#define ASSERT(op)  ((void)0)
//#endif


#define OutputConsole console::Manager::GetInstance()->Output
#define OutputError   console::Manager::GetInstance()->Outpute
#define OutputWarring console::Manager::GetInstance()->Outputw

//__CLASS__(utility::File)___ DEFINE__
#define MAX_FILE_NAME_SAVED  	   1024
#define TEXT_BUFFER_SZIE	 	   4096

#define BITS                       15
#define MAX_CODE                   ( ( 1 << BITS ) - 1 )
#define TABLE_SIZE                 35023L
#define TABLE_BANKS                ( ( TABLE_SIZE >> 8 ) + 1 )
#define LZW_END_OF_STREAM          256
#define BUMP_CODE                  257
#define FLUSH_CODE                 258
#define FIRST_CODE                 259
#define LZW_UNUSED                 -1

//__CLASS__(utility::Text)____DEFINE__
#define _MAX_STEAM_WORD_LENGTH 256

inline unsigned __int64 RDTSC() 
{ 
	__asm RDTSC 
} 

class utility
{
public:
	class File
	{
	public:

		//---------------------------------------------
		BYTE * m_pbyData;

		int m_iPosition;	
		int m_iLength;

		char m_pszFileName[MAX_FILE_NAME_SAVED];

	public:
		File(void);
		~File(void);

		//FUNCTION
		BOOL Open(const char *pszFileName);
		void Close();
		const BYTE *GetData() const { return m_pbyData;}
		BOOL ReadChar(unsigned char &c);
		int  Read(void *pbyBuffer, int iSize);
		char * ReadLine(char * pszLineBuffer, int iBufferLen);
		int  Seek(int iPosition,int iOrigin = 0);
		void SetPosition(int iPosition) { m_iPosition = iPosition;}
		int  GetPosition() const  { return m_iPosition;}
		int  GetLength() const { return m_iLength;	}
		void SkipBytes(int iBytes);
		const char *GetPathName() const { return m_pszFileName;}

		static BOOL FileExist(const char *szFileName);
		static char *GetFilePath(const char *szFileName, char* szOutput);//不带"/" 和 "\\"
		static char *GetFileName(const char *szFileName);
	};

	//__CLASS__(utility::Text)___
	class Text
	{
	public:
		class Stream
		{
			//数据
			BYTE *m_pData;
			int   m_iLength;
			int   m_iPosition;


			char m_pTempWord[_MAX_STEAM_WORD_LENGTH];


			//函数
		public:
			void Create(const BYTE *pData,int iLength);
			void Destroy();

			const char *GetWord();
			void SkipWord();
			void SkipLine();

			BOOL EndOFStream(void) const {	return (m_iPosition >= m_iLength);}	

		};

		class Variable
		{
			char m_szName[_MAX_STEAM_WORD_LENGTH];
			char m_szValue[_MAX_STEAM_WORD_LENGTH];

		public:
			void SetName(const char *pszName);
			void SetValue(const char *pszValue);

			char * GetName() { return m_szName;}

			int	  GetIntValue() const { return atoi(m_szValue);}
			float GetFloatValue() const { return (float)atof(m_szValue);}
			const char * GetStringValue() const { return m_szValue;}
			BOOL  GetBOOLValue() const;
			DWORD GetEnumValue(const char * szEnumString[], DWORD dwEnumMax);
			DWORD GetHexValue();
		};
		class Node
		{
			enum eCMD { CMD_UNKNOWN,CMD_NEWNODE,CMD_EQUAL,CMD_ENDNODE,CMD_COMMENT};

			char m_szName[_MAX_STEAM_WORD_LENGTH];	

			std::vector<Node *>m_pChildNodeArray;
			std::vector<Variable *>m_pVarArray;	

		public:
			void Create(Stream *pStream);
			void Destroy();

			const char *GetName() const { return m_szName;}
			void SetName(const char *pszName);

			eCMD CheckCmd(const char *szWord);

			void AddChildNode(Node *pNode);
			void AddVar(Variable *pVar);	

			Node * GetChildNode(const char *pszNodeName);

			DWORD  GetChildCount() { return (DWORD)m_pChildNodeArray.size();}
			Node * GetChildNode(int iIndex) { return m_pChildNodeArray[iIndex];}

			DWORD  GetVarCount() { return (int)m_pVarArray.size();}
			Variable * GetVar(int iIndex) { return m_pVarArray[iIndex];}

			Variable * GetVar(const char *pszVarName);
			void ExportScript(char *pszBuffer);
			void ExportContent(char *pszBuffer);
		};
	private:
		Node *m_pRootNode;
	public:
		Text(void);
		~Text(void);

		void Create(File *pFile);
		void Create(const char *pszText);

		void Destroy();

		Node * GetRootNode() { return m_pRootNode;}
	};
};
