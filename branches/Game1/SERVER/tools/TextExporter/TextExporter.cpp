// TextExporter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "..\..\..\public\tools.h"
#include "..\..\..\public\debugnew.h"
#include "..\..\..\public\public.h"
#include "..\..\..\public\rfile.h"
#include "..\..\..\public\clientresource.h"
#include "..\..\..\public\readwrite.h"
#include "..\..\..\public\tinyxml.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


// 读取所有的脚本文件
void FindXmlFile(const char* strFile, list<string>* plistFileName)
{
	char strPath[MAX_PATH];
	GetFilePath(strFile, strPath);

	WIN32_FIND_DATA stFindData;
	HANDLE hFind = FindFirstFile(strFile, &stFindData);
	if (hFind==INVALID_HANDLE_VALUE)
	{
		return;
	}

	BOOL bNext = true;
	while(1)
	{
		bNext = FindNextFile(hFind, &stFindData);
		if (bNext==0)
		{
			break;
		}

		// 是目录
		if (stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (stricmp(stFindData.cFileName, "..")!=0)
			{
				char str[MAX_PATH];
				sprintf(str, "%s%s/*.*", strPath, stFindData.cFileName);
				FindXmlFile(str, plistFileName);
			}
		}
		else	// 文件
		{
			char strExtName[MAX_PATH];
			GetFileExtName(stFindData.cFileName, strExtName);
			if (stricmp(strExtName, ".xml")==0)
			{
				char str[MAX_PATH];
				sprintf(str, "%s%s", strPath, stFindData.cFileName);
				strlwr(str);	//  转换成小写
				plistFileName->push_back(str);
			}
		}
	}
	FindClose(hFind);
}

void ExportXml2Csv(const char* xmlName, FILE* csvFile)
{
	if(xmlName && csvFile)
	{
		TiXmlNode *pNode = NULL;
		TiXmlDocument doc(xmlName);

		FILE* xmlFile = fopen(xmlName, "rb");

		fseek(xmlFile, 0, SEEK_END);
		long size = ftell(xmlFile);
		fseek(xmlFile, 0, SEEK_SET);
		BYTE* pData = new BYTE[size+1];
		memset(pData,0,size);
		size_t nNumRead = fread(pData,size,1,xmlFile);
		pData[size] = '\0';
		doc.LoadData(pData, size);
		fclose(xmlFile);

		//取得第一个节点
		pNode = doc.FirstChild("StringTable");	

		if (NULL == pNode)
		{
			printf("error : %s xml file's scheme is not correct !\r\n", xmlName);
			return;
		}

		fwrite(xmlName, 1, strlen(xmlName), csvFile);
		fwrite("\n", 1, 1, csvFile);

		long childNum = 0;
		TiXmlElement *pElem = pNode->ToElement();

		for(TiXmlElement *pChildElem=pElem->FirstChildElement(); 
			pChildElem!=NULL; 
			pChildElem=pChildElem->NextSiblingElement())
		{
			childNum++;
		}

		char strNum[32];
		sprintf_s(strNum, "%d", childNum);
		fwrite(strNum, 1, strlen(strNum), csvFile);
		fwrite("\n", 1, 1, csvFile);

		for(TiXmlElement *pChildElem=pElem->FirstChildElement(); 
			pChildElem!=NULL; 
			pChildElem=pChildElem->NextSiblingElement())
		{
			if (!stricmp(pChildElem->Value(),"String"))
			{
				const char* id = pChildElem->Attribute("id");
				if( id )
					fwrite(id, 1, strlen(id), csvFile);
				else
					fwrite(" ", 1, 1, csvFile);
				fwrite(",", 1, 1, csvFile);

				const char* name = pChildElem->Attribute("text");
				if( name )
					fwrite(name, 1, strlen(name), csvFile);
				else
					fwrite(" ", 1, 1, csvFile);
				fwrite(",\n", 1, 2, csvFile);
			}
		}
	}
}

void ExportCsv2Xml(FILE* csvFile)
{
	if( csvFile )
	{
		std::string tmp;
		std::stringstream stream;
		
		fseek(csvFile, 0, SEEK_END);
		long size = ftell(csvFile);
		fseek(csvFile, 0, SEEK_SET);
		BYTE* pData = new BYTE[size+1];
		memset(pData,0,size);
		size_t nNumRead = fread(pData,size,1,csvFile);
		pData[size] = '\0';
		stream << pData;
		stream.seekg(0, std::ios_base::beg);

		stream >> tmp;

		long xmlNum = atoi(tmp.c_str());
		for(int i=0; i<xmlNum; i++)
		{
			stream >> tmp;
			std::string xmlName = tmp;
			FILE* xmlFile = fopen(tmp.c_str(), "wb");
			const char* s = "<?xml version=\"1.0\" encoding=\"gb2312\" standalone=\"no\"?>\n";
			fwrite(s, 1, strlen(s), xmlFile);
			const char* s1 = "<StringTable>\n";
			fwrite(s1, 1, strlen(s1), xmlFile);

			stream >> tmp;
			long num = atoi(tmp.c_str());
			for(int i=0; i<num; i++)
			{
				const char* s2 = "<String id=\"";
				fwrite(s2, 1, strlen(s2), xmlFile);
				char buffer[4096];
				stream.getline(buffer, 4096);
				if(buffer[0]=='\0') stream.getline(buffer, 4096);
				tmp = buffer;
				//stream >> tmp;

				long pos = tmp.find(',');
				char id[32];
				memcpy(id, &tmp[0], pos);
				id[pos] = '\0';
				fwrite(id, 1,  strlen(id), xmlFile);
				fwrite("\" text=\"", 1, 8, xmlFile);
				tmp.erase(0, pos+1);
				
				pos = tmp.find(',');
				char text[4096];
				memcpy(text, &tmp[0], pos);
				text[pos] = '\0';
				fwrite(text, 1,  strlen(text), xmlFile);
				fwrite("\"/>\n", 1, 4, xmlFile);
			}

			const char* s3 = "</StringTable>\n";
			fwrite(s3, 1, strlen(s3), xmlFile);
			fclose(xmlFile);

			printf("success : export csv file to %s xml file !\r\n", xmlName.c_str());
		}
	}
}

int _tmain(int argc, char* argv[])
{
	if(argv[1] == NULL)
	{
		printf("ERROR!please give correct args before running it!\r\npress ANY key continue...\r\n");
		getchar();
		return 0;
	}
	// 导入xml文件到csv文件
	if( strcmp(argv[1], "xml2csv")==0 )
	{
		FILE* csvFile = fopen(argv[2], "wb+");
		if( !csvFile ) 
		{
			printf("can not open %s csv file !\r\npress ANY key continue...\r\n", argv[2]);
			getchar();
			return 0;
		}

		char path[1024];
		GetFilePath(argv[0], path);
		sprintf_s(path, "%s%s", path, "*.*");
		// 读取Scripts/目录中的所有脚本文件
		list<string> listFileName;		
		FindXmlFile(path, &listFileName);

		// Record total xml files num.
		char num[128];
		sprintf_s(num, "%d", listFileName.size());
		fwrite(num, 1,  strlen(num), csvFile);
		fwrite("\r\n", 1, 2, csvFile);

		// 具体读取
		for(list<string>::iterator it=listFileName.begin(); it!=listFileName.end(); it++)
		{
			const char* filename = it->c_str();
			ExportXml2Csv(filename, csvFile);
		}

		fclose(csvFile);

		printf("xml files has been exported to %s csv file !\r\npress ANY key continue...\r\n", argv[2]);
	}
	
	// 导入csv文件到xml文件
	else if( strcmp(argv[1], "csv2xml")==0 )
	{
		FILE* csvFile = fopen(argv[2], "rb");

		if( !csvFile ) 
		{
			printf("can not open %s csv file !\r\npress ANY key continue...\r\n", argv[2]);
			getchar();
			return 0;
		}

		ExportCsv2Xml(csvFile);

		fclose(csvFile);

		printf("%s csv file has been exported to xml files !\r\npress ANY key continue...\r\n", argv[2]);
	}
	getchar();
	return 0;
}

