#pragma once

class CEmotion
{
public:
	CEmotion									();
	~CEmotion									();

	static BOOL				LoadSetup			( CHAR* pFileName );

	static BOOL				Serialize			( vector<BYTE>& vIn );
	static BOOL				Unserialize			( UCHAR* pArray, LONG& lPos );

	static BOOL				IsEmotionRepeated	( LONG lID );

	static map<LONG, BOOL>	s_mEmotions;
};
