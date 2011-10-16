#include "StdAfx.h"
#include "TextRes.h"

namespace AppFrame
{
bool LoadTextRes(const string& dir, bool sync)
{
    Log4c::Trace(ROOT_MODULE,"[TEXTRES] Load TextRes (%s) ...",dir.c_str());
	list<string> files;
	FindFile(dir.c_str(), ".xml", &files);
    if (files.empty())
    {
        Log4c::Warn(ROOT_MODULE,"[TEXTRES] Load TextRes (%s) +++ FAILED +++, no files found",dir.c_str());
        return false;
    }

    s_texts.clear();
    list<string>::iterator itr = files.begin();
	for(; itr != files.end(); ++itr)
	{
        if (!LoadTextFile(*itr, sync))
        {
            Log4c::Warn(ROOT_MODULE,"[TEXTRES] Load TextRes (%s) +++ FAILED +++",dir.c_str());
            return false;
        }
    }
    return true;
}

bool LoadTextFile(const string& file_name, bool sync)
{
    ConfigParse file(file_name.c_str(), sync);
    if (!file.IsLoaded())
    {
        Log4c::Warn(ROOT_MODULE, "[TEXTRES] Load TextRes file (%s) +++ FAILED +++, ErrorID: %d",file_name.c_str(),::GetLastError());
        return false;
    }

    int type = 0;
    string id = "", res = "";
    file.SetFirstChild();
    do
    {
        id = file.GetAttr("id");
        res = file.GetAttr("text");
#ifdef _DEBUG
        if (s_texts.find(id) != s_texts.end())
        {
            Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "[TEXTRES] Same id (%s, %s) is DISALLOWED",id.c_str(),file_name.c_str());
            return false;
        }
#endif
        s_texts[id] = res;
    } while (file.SetNextSibling());

      Log4c::Trace(ROOT_MODULE, "[TEXTRES] Load TextRes file (%s) --- OK ---", file_name.c_str());
    return true;
}

const char* GetText(const string& id)
{
    static  char szText[256] = "";
    sprintf_s(szText,"[TEXTRES] TextRes [%s] NOT FOUND", id.c_str()); 
    
    if (s_texts.empty())
        return szText;

    map<string, string>::iterator itr = s_texts.find(id);
    if (itr != s_texts.end())
        return itr->second.c_str();

    Log4c::Warn(ROOT_MODULE,szText);

    return szText;
}


const char* FormatText(const char* id, ...)
{
    static const int maxLength = 4096;            // log信息最大长度
    static char info[maxLength];

    va_list va;
    va_start(va, id);
    vsprintf(info, GetText(id), va);
    va_end(va);

    return info;
}

bool DoNothing(const pair<string, string>& item)
{
    return true;
}

void EncodeTextRes(vector<uchar>& buf, const vector<string>& prefixes)
{
    vector<pair<string, string>> texts;
    vector<string>::const_iterator itr = prefixes.begin();
    map<string, string>::const_iterator cur;
    map<string, string>::const_iterator end;
    for (; itr != prefixes.end(); ++itr)
    {
        const string segend(*itr + '~');
        s_texts.insert(make_pair(*itr, ""));
        s_texts.insert(make_pair(segend, ""));
        cur = s_texts.find(*itr);
        end = s_texts.find(segend);
        for (++cur; cur != end; ++cur)
        {
            texts.push_back(*cur);
        }
    }
    long count = (long)texts.size();
    _AddToByteArray(&buf, count);
    vector<pair<string, string>>::iterator itr_dest = texts.begin();
    for (; itr_dest != texts.end(); ++itr_dest)
    {
        _AddToByteArray(&buf, itr_dest->first.c_str());
        _AddToByteArray(&buf, itr_dest->second.c_str());
    }
}

void DecodeTextRes(uchar* buf, long& pos)
{
    char id[128], text[102400];
    s_texts.clear();
    long count = _GetLongFromByteArray(buf, pos);
    for (long i = 0; i < count; ++i)
    {
        _GetStringFromByteArray(buf, pos, id);
        _GetStringFromByteArray(buf, pos, text);
        s_texts[id] = text;
    }
}

/** 
 * @brief  用于正则表达式的客户端文本替换  by:tp
 * @param  Prfex   文本前缀
 * @param  Num     文本Id
 * 修改新的客户端文本资源，在原来的文本索引前加入文本字符索引描述
 * 替换方式 利用Vs的CTrl+H 
 *           查找内容 CStringReading\:\:LoadText*\({[^,]+},{[^\,\)\;]+}\)  
             替换文本 AppFrame::LoadAppTextRes("FA_",\2)               
             比如家族新的文本资源为 "FA_"为新的资源索引前缀  \2为第二段需要替换的文本索引
 */ 
const char* LoadAppTextRes(const char* Prfex,long Num )
{
    char   quest[32];
    sprintf_s( quest ,"%s%d",Prfex,Num);
    return AppFrame::GetText(quest);
};

const char* LoadQuestTextRes(long QId ,long Num )
{
    char   quest[32];
    sprintf_s(quest,"Q%6d%02d",QId,Num);
    return AppFrame::GetText(quest);
};


}; // namespace AppFrame
