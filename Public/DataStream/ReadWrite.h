#ifndef READWRITE_H
#define READWRITE_H

bool ReadTo(istream& stream, const char* name);
bool Read(istream& stream, const char* name, int& data);
bool Read(istream& stream, const char* name, long& data);
bool Read(istream& stream, const char* name, float& data);
bool Read(istream& stream, const char* name, bool& data);
bool Read(istream& stream, const char* name, ulong& data);
bool Read(istream& stream, const char* name, short& data);
bool Read(istream& stream, const char* name, char& data);
bool Read(istream& stream, const char* name, uchar& data);
//bool Read(istream& stream, const char* name, float& data);
bool Read(istream& stream, const char* name, string& str);
//bool Read(istream& stream, const char* name, CString& str);
bool Read(istream& stream, const char* name, char *str);

bool Write(ostream& stream, const char* name, char* str);
bool Write(ostream& stream, const char* name, string& str);
bool Write(ostream& stream, const char* name, int& data);
bool Write(ostream& stream, const char* name, long& data);
bool Write(ostream& stream, const char* name, bool& data);
bool Write(ostream& stream, const char* name, float& data);
bool Write(ostream& stream, const char* name, ulong& data);
bool Write(ostream& stream, const char* name, short& data);
bool Write(ostream& stream, const char* name, char& data);
bool Write(ostream& stream, const char* name, uchar& data);
//bool Write(ostream& stream, const char* name, CString& str);

#endif
