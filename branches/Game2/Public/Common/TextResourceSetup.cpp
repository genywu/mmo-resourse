#include "stdafx.h"
#include "TextResourceSetup.h"
#include <fstream>
#include <map>
#include <string>
using namespace std;


map<string, string> g_Prefixes;//prefix -- desc
map<string, string> g_Texts;//text id -- text

void AddPrefix(const string& prefix, const string& desc)
{
    g_Prefixes[prefix] = desc;//如果存在则替换，不能用insert
}
void DelPrefix(const string& prefix)
{
    g_Prefixes.erase(prefix);
}

void AddText(const string& id, const string& text)
{
    g_Texts[id] = text;//如果存在则替换，不能用insert
}
void DelText(const string& id)
{
    g_Texts.erase(id);
}
const string& GetTextRes(const string& id)
{
    static string s;
    map<string, string>::iterator it = g_Texts.find(id);
    if (it != g_Texts.end())
        return it->second;
    else
    {
        s = "not found: " + id;
        return s;
    }
}

bool IsPrefixExist(const string& prefix)
{
    return (g_Prefixes.count(prefix) > 0);
}
bool IsIdExist(const string& id)
{
    return (g_Texts.count(id) > 0);
}


static const char* s_FileName = "textresource.ini";
void SaveTextResourceSetup()
{
    ofstream s(s_FileName);
    if (!s.is_open())
        return;

    s << g_Texts.size() << endl;
    for (map<string, string>::iterator it = g_Texts.begin();
        it != g_Texts.end(); ++it)
    {
        s << it->first << endl << it->second << endl;
    }

    s << g_Prefixes.size() << endl;
    for (map<string, string>::iterator it = g_Prefixes.begin();
        it != g_Prefixes.end(); ++it)
    {
        s << it->first << endl << it->second << endl;
    }

    s.close();
}

void LoadTextResourceSetup()
{
    ifstream s(s_FileName);
    if (!s.is_open())
        return;

    string nop, prefix, desc, id, text;

    size_t num = 0;
    s >> num;
    getline(s, nop);
    for (size_t i=0; i<num; ++i)
    {
        getline(s, id);
        getline(s, text);
        g_Texts.insert(make_pair(id, text));
    }

    num = 0;
    s >> num;
    getline(s, nop);
    for (size_t i=0; i<num; ++i)
    {
        getline(s, prefix);
        getline(s, desc);
        g_Prefixes.insert(make_pair(prefix, desc));
    }
    s.close();
}