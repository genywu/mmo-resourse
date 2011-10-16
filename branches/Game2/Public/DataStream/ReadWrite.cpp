#include "StdAfx.h"
#include "ReadWrite.h"
#include "assert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


bool ReadTo(istream& stream, const char* name)
{
	string str;

	stream >> str;
	while (str != name)
	{
		if (stream.eof())
		{
			return false;
		}		
		stream >> str;
	}
	return true;
}

bool Read(istream& stream, const char* name, long& data)
{
	if(	ReadTo(stream,name) )
	{
		stream >> data;
		return true;
	}
	return false;
}

bool Read(istream& stream, const char* name, ulong& data)
{
	if( ReadTo(stream,name) )
	{
		stream >> data;
		return true;
	}
	return false;
}

bool Read(istream& stream, const char* name, int& data)
{
	if( ReadTo(stream,name) )
	{
		stream >> data;
		return true;
	}
	return false;
}

bool Read(istream& stream, const char* name, bool& data)
{
	if( ReadTo(stream,name) )
	{
		stream >> data;
		return true;
	}
	return false;
}

bool Read(istream& stream, const char* name, float& data)
{
	if( ReadTo(stream,name) )
	{
		stream >> data;
		return true;
	}
	return false;
}

bool Read(istream& stream, const char* name, short& data)
{
	if( ReadTo(stream,name) )
	{
		stream >> data;
		return true;
	}
	return false;
}

bool Read(istream& stream, const char* name, char& data)
{
	if( ReadTo(stream,name) )
	{
		stream >> data;
		return true;
	}
	return false;
}

bool Read(istream& stream, const char* name, uchar& data)
{
	if( ReadTo(stream,name) )
	{
		stream >> data;
		return true;
	}
	return false;
}

bool Read(istream& stream, const char* name, string& str)
{
	if( ReadTo(stream,name) )
	{
		stream >> str;
		return true;
	}
	return false;
}
/*
bool Read(istream& stream, const char* name, CString& str)
{
	if( ReadTo(stream,name) )
	{
		string s;
		stream >> s;
		str = s.c_str();
		return true;
	}
	return false;
}
*/
bool Read(istream& stream, const char* name, char *str)
{
	if( ReadTo(stream,name) )
	{
		stream >> str;
		return true;
	}
	return false;
}

bool Write(ostream& stream, const char* name, long& data)
{
	if(name[0])
	{
		stream << name; 
		stream << "\t" << data << "\n";
		return true;
	}
	return false;
}

bool Write(ostream& stream, const char* name, short& data)
{
	if(name[0])
	{
		stream << name; 
		stream << "\t" << data << "\n";
		return true;
	}
	return false;
}

bool Write(ostream& stream, const char* name, ulong& data)
{
	if(name[0])
	{
		stream << name; 
		stream << "\t" << data << "\n";
		return true;
	}
	return false;
}

bool Write(ostream& stream, const char* name, int& data)
{
	if(name[0])
	{
		stream << name; 
		stream << "\t" << data << "\n";
		return true;
	}
	return false;
}

bool Write(ostream& stream, const char* name, bool& data)
{
	if(name[0])
	{
		stream << name; 
		stream << "\t" << data << "\n";
		return true;
	}
	return false;
}

bool Write(ostream& stream, const char* name, float& data)
{
	if(name[0])
	{
		stream << name; 
		stream << "\t" << data << "\n";
		return true;
	}
	return false;
}

bool Write(ostream& stream, const char* name, char& data)
{
	if(name[0])
	{
		stream << name; 
		stream << "\t" << (int)data << "\n";
		return true;
	}
	return false;
}

bool Write(ostream& stream, const char* name, uchar& data)
{
	if(name[0])
	{
		stream << name; 
		stream << "\t" << (int)data << "\n";
		return true;
	}
	return false;
}

bool Write(ostream& stream, const char* name, string& str)
{
	if(name[0])
	{
		stream << name; 
		stream << "\t" << str << "\n";
		return true;
	}
	return false;
}
/*
bool Write(ostream& stream, const char* name, CString& str)
{
	if(name[0])
	{
		if(!str.IsEmpty())
		{
			stream << name; 
			string s = str;
			stream << "\t" << s << "\n";
			return true;
		}
	}
	return false;
}
*/
bool Write(ostream& stream, const char* name, char* str)
{
	if(name[0])
	{
		if(str[0])
		{
			stream << name; 
			stream << "\t" << str << "\n";
			return true;
		}
	}
	return false;
}
