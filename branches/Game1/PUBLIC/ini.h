//********************************************
//	Ini 相关函数
//********************************************

#pragma once

// 错误返回值，如果ReadInt没有发现数据则返回该值
#define ERROR_DATA -99999999

//配置文件类
class CIni
{
///////////////////////////////////////////////////////////////////////
//	外部接口
///////////////////////////////////////////////////////////////////////
public:
	CIni();
	CIni(const char* filename, const char* pData=NULL);		//初始化打开配置文件
	virtual ~CIni();						//释放内存

	char *GetData();						//返回文件内容
	int GetLines(int);						//返回文件的行数

	bool Open(const char* filename, const char* pData=NULL);	//打开配置文件
	void Close();							//关闭配置文件
	bool Save(const char *filename=NULL);	//保存配置文件

public:
	int ReadInt(char*, char*);				//读一个整数
	int ReadInt(char*, int );				//在指定的行读一整数

	char *ReadText(char*, char*);			//读一个字符串
	char *ReadText(char*, int);				//在指定的行读一字符串

	char *ReadCaption(char*, int);			//在指定行读一字符名称( '=' 前面的内容)

	bool Write(char*, char*, int);			//写入一个整数
	bool Write(char*, char*, char*);		//写入一个字符串

	int GetContinueDataNum(char* index);	//返回连续的行数（从INDEX到第一个空行）
	int GotoLastLine(char* index);			//把指针移动到本INDEX的最后一行

///////////////////////////////////////////////////////////////////////
//	内部实现
///////////////////////////////////////////////////////////////////////
private:
	void InitIndex();			//初始化索引
	int FindIndex(char *);		//返回标题位置
	int FindIndexPos(char *);	//返回第几个标题
	int FindIndex(int nPos);	//返回第几个标题位置
	int FindNextIndex(int nPos);	// 返回下一个标题位置
	int FindData(int, char *);	//返回数据位置
	int GotoNextLine(int); 		//提行
	char *ReadDataName(int &);	//在指定位置读一数据名称
	char *ReadText(int);		//在指定位置读字符串

	bool AddIndex(char *);					//加入一个索引
	bool AddData(int, char *, char *);		//在当前位置加入一个数据
	bool ModityData(int, char *, char *);	//在当前位置修改一个数据的值

private:
	char m_strFileName[MAX_PATH];	//文件名
	int m_lDataLen;					//文件长度
	char *m_pData;					//文件内容

	int m_lIndexNum;				//索引数目（[]的数目）
	int* m_pIndexList;				//索引点位置列表

	int m_lPoint;					//当前指针
	int m_lLine, m_lWord;			//当前行列

};
