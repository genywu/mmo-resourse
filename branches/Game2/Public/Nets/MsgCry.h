#pragma once

/// 数据加密算法
enum EAType
{
	EAT_NULL	= 0x00,
	EAT_DES		= 0x01,
	EAT_TDEA	= 0x02,
};

void EAInit(EAType eT = EAT_DES);					/// 初始化
void KeyInit(const unsigned char *pKey,
			unsigned char kn[16][6]);				/// 设置密钥
int Encrypt(unsigned char *pBlock,
			int l,
			const unsigned char kn[16][6]);			/// 对任意长度数据进行加密
int Decrypt(unsigned char *pBlock,
			int l,
			const unsigned char kn[16][6]);			/// 对任意长度数据进行解密