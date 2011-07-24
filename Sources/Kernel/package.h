#ifndef PACKAGE_H
#define PACKAGE_H

#include "xbuffer.h"

#define MAX_CID		256

class CPackager {
public:
	char *data_ptr[MAX_CID];
	int data_size[MAX_CID];

	char *send_data_ptr[MAX_CID];
	char *pack_data_ptr[MAX_CID];
	int send_data_size[MAX_CID];
	int split_pos[MAX_CID];
	int pack_len[MAX_CID];
	int pack_data_size[MAX_CID];
	unsigned long user_data[MAX_CID];
public:
	CPackager() {
		memset(&data_ptr[0], 0, MAX_CID * sizeof(char *));
		memset(data_size, 0, MAX_CID * sizeof(int));
		memset(&send_data_ptr[0], 0, MAX_CID * sizeof(char *));
		memset(&pack_data_ptr[0], 0, MAX_CID * sizeof(char *));
		memset(split_pos, 0, MAX_CID * sizeof(int));
		memset(pack_len, 0, MAX_CID * sizeof(int));
		memset(send_data_size, 0, MAX_CID * sizeof(int));
		memset(pack_data_size, 0, MAX_CID * sizeof(int));
	}
	~CPackager() {
		int index;
		for(index = 0; index < MAX_CID; index++) {
			if(data_ptr[index]) delete[] data_ptr[index];
		}
		for(index = 0; index < MAX_CID; index++) {
			if(send_data_ptr[index]) delete[] send_data_ptr[index];
		}
		for(index = 0; index < MAX_CID; index++) {
			if(pack_data_ptr[index]) delete[] pack_data_ptr[index];
		}
	}
	char *getpackdata(unsigned char cID)
	{
#define PACK_DATA_INIT_SIZE	1024
		size_t datalen = pack_len[cID] + 16;
		if (pack_data_size[cID] >= datalen)
			return pack_data_ptr[cID];

		//realloc
		if (pack_data_ptr[cID])
			delete pack_data_ptr[cID];
		pack_data_size[cID] = PACK_DATA_INIT_SIZE > datalen ? PACK_DATA_INIT_SIZE : datalen;
		pack_data_ptr[cID] = new char[pack_data_size[cID]];
		return pack_data_ptr[cID];
	}
	char *PackUp(const void *data, int size);

	/*
	 * Send
	 */
	char *GetHeadPack( BYTE cID, size_t& actualLength, size_t packLength = 512 );
	char *GetNextPack( BYTE cID, size_t& actualLength);
	
	void AddData( BYTE cID, const char * const pData, size_t dataLength, unsigned long lnUserData = 0 );
	void AddData( BYTE cID, const BYTE * const pData, size_t dataLength, unsigned long lnUserData = 0 )
	{
		AddData( cID, reinterpret_cast< const char * >( pData ), dataLength, lnUserData );
	}

	void DelData( BYTE cID );

};

#define MAX_PACKAGE 65500

#endif
