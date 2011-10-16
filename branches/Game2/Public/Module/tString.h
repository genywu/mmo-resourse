///
/// @file tString.h
/// @author Kevin Lynx
/// 
/// A tiny string util can only be assign, it's tiny and simple so it can be used
/// in some effective place as i wish.
///
#ifndef ___TSTRING_H_
#define ___TSTRING_H_

class tString
{
public:
	tString();

	tString( const char *s );

	tString( const tString &other );

	~tString();

	const tString &operator = ( const tString &other );

	const tString &operator = ( const char *s );

	bool operator == ( const tString &other ) const;

	bool operator != ( const tString &other ) const;

	operator const char* () const;

	const tString &Assign( const char *s );

	bool Valid() const;

	size_t Length() const;

	const char *Str() const;

private:
	size_t m_Len;
	char *m_Str;
};

#endif
