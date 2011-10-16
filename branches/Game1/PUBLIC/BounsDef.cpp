#include "StdAfx.h"
#include "BounsDef.h"


// Ω±¿¯–≈œ¢±‡¬Î
void tagBounsInfo::EncodeToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray(guid);
	setWriteDB.AddToByteArray((char*)strAccount.c_str());
	setWriteDB.AddToByteArray((BYTE)bStatus);	
	setWriteDB.AddToByteArray(dwBounsID);
    setWriteDB.AddToByteArray((BYTE)bType);
    setWriteDB.AddToByteArray((char*)strName.c_str());
    setWriteDB.AddToByteArray(dwValue);
//	setWriteDB.AddToByteArray((long)tValidTime);
	setWriteDB.AddToByteArray((void*)&tValidTime, sizeof(time_t));
	
}

// Ω±¿¯–≈œ¢Ω‚¬Î
void tagBounsInfo::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	char strTemp[256];
	setReadDB.GetBufferFromByteArray(guid);
	strAccount	= setReadDB.GetStringFromByteArray(strTemp, 256);
	bStatus		= setReadDB.GetByteFromByteArray();
	dwBounsID	= setReadDB.GetDwordFromByteArray();	
    bType		= setReadDB.GetByteFromByteArray();
    strName		= setReadDB.GetStringFromByteArray(strTemp, 256);
    dwValue		= setReadDB.GetDwordFromByteArray();
//	tValidTime	= setReadDB.GetLongFromByteArray();
	setReadDB.GetBufferFromByteArray((void*)&tValidTime, sizeof(time_t));
	
}

// Ω±¿¯√Ë ˆ±‡¬Î
void tagBounsDes::EncodeToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray(dwBounsID);
	setWriteDB.AddToByteArray((char*)strTitle.c_str());
	setWriteDB.AddToByteArray((char*)strDes.c_str());
	
}

// Ω±¿¯√Ë ˆΩ‚¬Î
void tagBounsDes::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	char strTemp[256];
	dwBounsID	= setReadDB.GetDwordFromByteArray();
	strTitle	= setReadDB.GetStringFromByteArray(strTemp, 256);
	strDes		= setReadDB.GetStringFromByteArray(strTemp, 256);
}