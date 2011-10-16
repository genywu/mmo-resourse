/******************************************************************************
 *  MD5.H - header file for MD5.C                                             *
 *  Copyright (C) 2001-2002 by ShadowStar.                                    *
 *  Use and modify freely.                                                    *
 *  http://shadowstar.126.com/                                                *
 ******************************************************************************
 */
#ifndef _MD5_H
#define _MD5_H

//---------------------------------------------------------------------------
#include "../DefType.h"

/* Constants for MD5Transform routine.
 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

//---------------------------------------------------------------------------
/* MD5 context. */
typedef struct
{
	ulong state[4];           /* state (ABCD) */
	ulong count[2];           /* number of bits, modulo 2^64 (lsb first) */
	uchar buffer[64]; /* input buffer */
} MD5_CTX;

//---------------------------------------------------------------------------
class TMD5
{
public :
	void MessageDigest(const uchar *szInput, unsigned int inputLen,
			uchar szOutput[16], int iIteration = 1);
private :
	MD5_CTX context;
	void Init(MD5_CTX *context);
	void Update(MD5_CTX *context, const uchar *input, unsigned int inputLen);
	void Final(uchar digest[16], MD5_CTX *context);
	void Transform(ulong state[4], const uchar block[64]);
};

//---------------------------------------------------------------------------
/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

//---------------------------------------------------------------------------
/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac)\
{ \
	(a) += F ((b), (c), (d)) + (x) + (ulong)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define GG(a, b, c, d, x, s, ac)\
{ \
	(a) += G ((b), (c), (d)) + (x) + (ulong)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define HH(a, b, c, d, x, s, ac)\
{ \
	(a) += H ((b), (c), (d)) + (x) + (ulong)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define II(a, b, c, d, x, s, ac)\
{ \
	(a) += I ((b), (c), (d)) + (x) + (ulong)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
//---------------------------------------------------------------------------

#endif