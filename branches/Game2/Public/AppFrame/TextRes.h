/**
* @file    TextRes.h
* 字符串资源管理
* @author  Fox(yulefox.at.gmail.com)
* @date    Mar.29, 2010
*/

#pragma once

namespace AppFrame
{
    /// 重读字符串资源配置
    bool LoadTextRes(const string& file = "textres/*.xml", bool sync = true);
    const char* GetText(const string& id);
    const char* FormatText(const char* id, ...);
    static bool LoadTextFile(const string& file_name, bool sync = true);
    void EncodeTextRes(vector<uchar>& buf, const vector<string>& prefixes);
    void DecodeTextRes(uchar* buf, long& pos);

    const char* LoadAppTextRes(const char* Prfex,long Num );
    const char* LoadQuestTextRes(long QId ,long Num );

    static map<string, string> s_texts;
}; // namespace AppFrame
