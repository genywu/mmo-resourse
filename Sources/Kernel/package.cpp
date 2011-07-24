#include "KTransferUnit.h"
#include "package.h"
#include "KWin32.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

KTransferUnit::KTransferUnit(DWORD dwIp, DWORD dwRelayID)
{
	m_dwIp = dwIp;
	m_dwRelayID = dwRelayID;
}

KTransferUnit::~KTransferUnit()
{
}

//如果组装成为一个完整的包则返回数据指针否则返回空
char *CPackager::PackUp(const void *data, int size) {
	int cID = (int)*(unsigned char *)data;
	unsigned char cFlag = *((unsigned char *)data + 1);
	
	if (!data_ptr[cID])
		data_ptr[cID] = new char[MAX_PACKAGE];
	
	if (!data_ptr[cID])
		return NULL;

	size -= 2 + sizeof(unsigned long);
	if(cFlag & 0xc0) {						//head
		data_size[cID] = size;
		memcpy(data_ptr[cID], (char *)data + 2 + sizeof(unsigned long), size);
		size = 0;
	}
	if(cFlag & 0x30) {						//middle
		if(size) {
			memcpy(data_ptr[cID] + data_size[cID], (char *)data + 2 + sizeof(unsigned long), size);
			data_size[cID] += size;
		}
		size = 0;
	}
	if(cFlag & 0xc) {					//tail
		if(size) {
			memcpy(data_ptr[cID] + data_size[cID], (char *)data + 2 + sizeof(unsigned long), size);
		}
		data_size[cID] = 0;
		return data_ptr[cID];
	}
	else return NULL;
}


void CPackager::AddData( BYTE cID, const char * const pData, size_t dataLength, unsigned long lnUserData /* = 0 */ )
{
	//printf("add data...%d, %d\n", cID, dataLength);
	char *pBuffer = NULL;
	int& nSize = send_data_size[cID];
	if (!send_data_ptr[cID])
	{
		send_data_ptr[cID] = new char[MAX_PACKAGE];
		send_data_size[cID] = 0;
	}
	pBuffer = send_data_ptr[cID];

	user_data[cID] = lnUserData;
	//ASSERT( pBuffer && dataLength + nSize < MAX_PACKAGE);
	memcpy(pBuffer + nSize, pData, dataLength);
	nSize += dataLength;
}

void CPackager::DelData( BYTE cID )
{
	//printf("del data...%d, %d\n", cID, send_data_size[cID]);
	send_data_size[cID] = 0;
	user_data[cID] = 0;
	split_pos[cID] = 0;
	pack_len[cID] = 0;
}

char *CPackager::GetHeadPack( BYTE cID, size_t& actualLength, size_t packLength /* = 512 */)
{
	if (send_data_size[cID] > 0)
	{
		char *pBuffer = send_data_ptr[cID];
		int nSize = send_data_size[cID];

		split_pos[cID] = 0;
		pack_len[cID] = packLength - 2;
		
		actualLength = nSize - split_pos[cID];
		actualLength = min(pack_len[cID], actualLength);

		split_pos[cID] += actualLength;

		if ( 0 == actualLength )
			return NULL;

		char* pPack = getpackdata(cID);

		if ( NULL == pPack )
		{
			return NULL;
		}

		/*
		 * Add a prototype
		 */
		pPack[0] = cID;

		/*
		 * Add a flag
		 */
		BYTE cFlag = 0xc0;

		if ( nSize <= split_pos[cID])//!pBuffer->HaveNextPack() )
		{
			cFlag |= 0xc;
		}
		
		pPack[1] = cFlag;

		/*
		 * Add a user data into this buffer
		 */

		*((unsigned long*)(pPack + 2)) = user_data[cID];

		/*
		 * Add some data into this buffer
		 */
		memcpy(pPack + 2 + sizeof(unsigned long), pBuffer, actualLength);

		//printf("head pack data...%d, %d/%d\n", cID, actualLength, send_data_size[cID]);
		actualLength += 2 + sizeof(unsigned long);
		return pPack;
	}

	return NULL;
}

char *CPackager::GetNextPack( BYTE cID, size_t& actualLength )
{
	if (send_data_size[cID] > split_pos[cID])
	{
		char *pBuffer = send_data_ptr[cID];
		pBuffer += split_pos[cID];

		char *pPack = getpackdata(cID);

		if ( NULL == pPack )
		{
			return NULL;
		}

		actualLength = send_data_size[cID] - split_pos[cID];
		actualLength = min(pack_len[cID], actualLength);

		split_pos[cID] += actualLength;

		/*
		 * Add a prototype
		 */
		pPack[0] = cID;

		/*
		 * Add a flag
		 */

		BYTE cFlag = 0x30;

		if ( send_data_size[cID] <= split_pos[cID])//!pBuffer->HaveNextPack() )
		{
			cFlag |= 0xc;
		}

		pPack[1] = cFlag;

		/*
		 * Add a user data into this buffer
		 */

		*((unsigned long*)(pPack + 2)) = user_data[cID];


		/*
		 * Add some data into this buffer
		 */
		memcpy(pPack + 2 + sizeof(unsigned long), pBuffer, actualLength);

		//printf("next pack data...%d, %d/%d\n", cID, actualLength, send_data_size[cID]);
		actualLength += 2 + sizeof(unsigned long);
		return pPack;
	}

	return NULL;
}


