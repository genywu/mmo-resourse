/**************************************************************
	LZARI.C -- A Data Compression Program
	(tab = 4 spaces)
***************************************************************
	4/7/1989 Haruhiko Okumura
	Use, distribute, and modify this program freely.
	Please send me your improved versions.
		PC-VAN		SCIENCE
		NIFTY-Serve	PAF01022
		CompuServe	74050,1022
**************************************************************/

/********************************************************************
	lzari.cpp -- A Data Compression Class
	created:	2004/10/04
	created:	4:10:2004   16:44
	file base:	lzari
	file ext:	cpp
	author:	阙荣文	(querw@sina.com)
	
	purpose: 如上所述,lzari.c提供了lzari压缩算法的实现,基于lzari.c我把它
			 做成了一个c++类方便使用
*********************************************************************/

#include "StdAfx.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <ctype.h>
#include "Lzari.h"

LZARI::LZARI()
{
	infile = NULL;
	outfile = NULL;

	textsize = 0;
	codesize = 0;
	printcount = 0;

	low = 0;
	high = Q4;
	value = 0;
	shifts = 0;/* counts for magnifying low and high around Q2 */
	
	m_bMem = FALSE;

	m_pInBuffer = NULL;
	m_nInLength = 0;
	m_nInCur = 0;

	//m_pOutBuffer = NULL;
	m_nOutLength = 0;
//	m_nOutCur = 0;

	buffer_putbit = 0;
	mask_putbit = 128;

	buffer_getbit = 0;
	mask_getbit = 0;

}

LZARI::~LZARI()
{
	Release();
}

void LZARI::Error(char *message)
{
#ifdef _OUTPUT_STATUS
	printf("\n%s\n", message);
#endif
	//exit(EXIT_FAILURE);
	int e = 1;
	throw e;
}

void LZARI::PutBit(int bit)  /* Output one bit (bit = 0,1) */
{
	if (bit) buffer_putbit |= mask_putbit;
	if ((mask_putbit >>= 1) == 0) 
	{
		if (!m_bMem)
		{
			if (putc(buffer_putbit, outfile) == EOF) Error("Write Error");
		}
		else
		{
			//if (m_nOutCur == m_nOutLength) Error("Write Error");
			//m_pOutBuffer[m_nOutCur++] = buffer;
			m_OutBuffer.push_back(buffer_putbit);
			//m_nOutCur++;
		}
		buffer_putbit = 0;  
		mask_putbit = 128;  
		codesize++;
	}
}

void LZARI::FlushBitBuffer(void)  /* Send remaining bits */
{
	int  i;
	
	for (i = 0; i < 7; i++) PutBit(0);
}

int LZARI::GetBit(void)  /* Get one bit (0 or 1) */
{	
	if ((mask_getbit >>= 1) == 0) 
	{
		if (!m_bMem)
			buffer_getbit = getc(infile);
		else
			buffer_getbit = m_pInBuffer[m_nInCur++];
		mask_getbit = 128;
	}
	return ((buffer_getbit & mask_getbit) != 0);
}

/********** LZSS with multiple binary trees **********/

void LZARI::InitTree(void)  /* Initialize trees */
{
	int  i;

	/* For i = 0 to N - 1, rson[i] and lson[i] will be the right and
	   left children of node i.  These nodes need not be initialized.
	   Also, dad[i] is the parent of node i.  These are initialized to
	   NIL (= N), which stands for 'not used.'
	   For i = 0 to 255, rson[N + i + 1] is the root of the tree
	   for strings that begin with character i.  These are initialized
	   to NIL.  Note there are 256 trees. */

	for (i = N + 1; i <= N + 256; i++) rson[i] = NIL;	/* root */
	for (i = 0; i < N; i++) dad[i] = NIL;	/* node */
}

void LZARI::InsertNode(int r)
	/* Inserts string of length F, text_buf[r..r+F-1], into one of the
	   trees (text_buf[r]'th tree) and returns the longest-match position
	   and length via the global variables match_position and match_length.
	   If match_length = F, then removes the old node in favor of the new
	   one, because the old one will be deleted sooner.
	   Note r plays double role, as tree node and position in buffer. */
{
	int  i, p, cmp, temp;
	unsigned char  *key;

	cmp = 1;  key = &text_buf[r];  p = N + 1 + key[0];
	rson[r] = lson[r] = NIL;  match_length = 0;
	for ( ; ; ) 
	{
		if (cmp >= 0) 
		{
			if (rson[p] != NIL) p = rson[p];
			else {  rson[p] = r;  dad[r] = p;  return;  }
		} else 
		{
			if (lson[p] != NIL) p = lson[p];
			else {  lson[p] = r;  dad[r] = p;  return;  }
		}
		for (i = 1; i < F; i++)
			if ((cmp = key[i] - text_buf[p + i]) != 0)  break;
		if (i > THRESHOLD) 
		{
			if (i > match_length) 
			{
				match_position = (r - p) & (N - 1);
				if ((match_length = i) >= F) break;
			} else if (i == match_length) 
			{
				if ((temp = (r - p) & (N - 1)) < match_position)
					match_position = temp;
			}
		}
	}
	dad[r] = dad[p];  lson[r] = lson[p];  rson[r] = rson[p];
	dad[lson[p]] = r;  dad[rson[p]] = r;
	if (rson[dad[p]] == p) rson[dad[p]] = r;
	else                   lson[dad[p]] = r;
	dad[p] = NIL;  /* remove p */
}

void LZARI::DeleteNode(int p)  /* Delete node p from tree */
{
	int  q;
	
	if (dad[p] == NIL) return;  /* not in tree */
	if (rson[p] == NIL) q = lson[p];
	else if (lson[p] == NIL) q = rson[p];
	else 
	{
		q = lson[p];
		if (rson[q] != NIL) 
		{
			do {  q = rson[q];  } while (rson[q] != NIL);
			rson[dad[q]] = lson[q];  dad[lson[q]] = dad[q];
			lson[q] = lson[p];  dad[lson[p]] = q;
		}
		rson[q] = rson[p];  dad[rson[p]] = q;
	}
	dad[q] = dad[p];
	if (rson[dad[p]] == p) rson[dad[p]] = q;
	else                   lson[dad[p]] = q;
	dad[p] = NIL;
}

/********** Arithmetic Compression **********/

/*  If you are not familiar with arithmetic compression, you should read
		I. E. Witten, R. M. Neal, and J. G. Cleary,
			Communications of the ACM, Vol. 30, pp. 520-540 (1987),
	from which much have been borrowed.  */

	/* character code = 0, 1, ..., N_CHAR - 1 */


void LZARI::StartModel(void)  /* Initialize model */
{
	int ch, sym, i;
	
	sym_cum[N_CHAR] = 0;
	for (sym = N_CHAR; sym >= 1; sym--) 
	{
		ch = sym - 1;
		char_to_sym[ch] = sym;  sym_to_char[sym] = ch;
		sym_freq[sym] = 1;
		sym_cum[sym - 1] = sym_cum[sym] + sym_freq[sym];
	}
	sym_freq[0] = 0;  /* sentinel (!= sym_freq[1]) */
	position_cum[N] = 0;
	for (i = N; i >= 1; i--)
		position_cum[i - 1] = position_cum[i] + 10000 / (i + 200);
			/* empirical distribution function (quite tentative) */
			/* Please devise a better mechanism! */
}

void LZARI::UpdateModel(int sym)
{
	int i, c, ch_i, ch_sym;
	
	if (sym_cum[0] >= MAX_CUM) 
	{
		c = 0;
		for (i = N_CHAR; i > 0; i--) 
		{
			sym_cum[i] = c;
			c += (sym_freq[i] = (sym_freq[i] + 1) >> 1);
		}
		sym_cum[0] = c;
	}
	for (i = sym; sym_freq[i] == sym_freq[i - 1]; i--) ;
	if (i < sym) 
	{
		ch_i = sym_to_char[i];    ch_sym = sym_to_char[sym];
		sym_to_char[i] = ch_sym;  sym_to_char[sym] = ch_i;
		char_to_sym[ch_i] = sym;  char_to_sym[ch_sym] = i;
	}
	sym_freq[i]++;
	while (--i >= 0) sym_cum[i]++;
}

void LZARI::Output(int bit)  /* Output 1 bit, followed by its complements */
{
	PutBit(bit);
	for ( ; shifts > 0; shifts--) PutBit(! bit);
}

void LZARI::EncodeChar(int ch)
{
	int  sym;
	unsigned long int  range;

	sym = char_to_sym[ch];
	range = high - low;
	high = low + (range * sym_cum[sym - 1]) / sym_cum[0];
	low +=       (range * sym_cum[sym    ]) / sym_cum[0];
	for ( ; ; ) 
	{
		if (high <= Q2) Output(0);
		else if (low >= Q2) 
		{
			Output(1);  low -= Q2;  high -= Q2;
		} 
		else if (low >= Q1 && high <= Q3) 
		{
			shifts++;  low -= Q1;  high -= Q1;
		} 
		else break;
		low += low;
		high += high;
	}
	UpdateModel(sym);
}

void LZARI::EncodePosition(int position)
{
	unsigned long int  range;

	range = high - low;
	high = low + (range * position_cum[position    ]) / position_cum[0];
	low +=       (range * position_cum[position + 1]) / position_cum[0];
	for ( ; ; ) 
	{
		if (high <= Q2) Output(0);
		else if (low >= Q2) 
		{
			Output(1);  low -= Q2;  high -= Q2;
		} 
		else if (low >= Q1 && high <= Q3) 
		{
			shifts++;  low -= Q1;  high -= Q1;
		} 
		else break;
		low += low;
		high += high;
	}
}

void LZARI::EncodeEnd(void)
{
	shifts++;
	if (low < Q1) Output(0);  else Output(1);
	FlushBitBuffer();  /* flush bits remaining in buffer */
}

int LZARI::BinarySearchSym(unsigned int x)
	/* 1      if x >= sym_cum[1],
	   N_CHAR if sym_cum[N_CHAR] > x,
	   i such that sym_cum[i - 1] > x >= sym_cum[i] otherwise */
{
	int i, j, k;
	
	i = 1;  j = N_CHAR;
	while (i < j) 
	{
		k = (i + j) / 2;
		if (sym_cum[k] > x) i = k + 1;  else j = k;
	}
	return i;
}

int LZARI::BinarySearchPos(unsigned int x)
	/* 0 if x >= position_cum[1],
	   N - 1 if position_cum[N] > x,
	   i such that position_cum[i] > x >= position_cum[i + 1] otherwise */
{
	int i, j, k;
	
	i = 1;  j = N;
	while (i < j)
	{
		k = (i + j) / 2;
		if (position_cum[k] > x) i = k + 1;  else j = k;
	}
	return i - 1;
}

void LZARI::StartDecode(void)
{
	int i;

	for (i = 0; i < M + 2; i++)
		value = 2 * value + GetBit();
}

int LZARI::DecodeChar(void)
{
	int	 sym, ch;
	unsigned long int  range;
	
	range = high - low;
	sym = BinarySearchSym((unsigned int)
		(((value - low + 1) * sym_cum[0] - 1) / range));
	high = low + (range * sym_cum[sym - 1]) / sym_cum[0];
	low +=       (range * sym_cum[sym    ]) / sym_cum[0];
	for ( ; ; ) {
		if (low >= Q2) {
			value -= Q2;  low -= Q2;  high -= Q2;
		} else if (low >= Q1 && high <= Q3) {
			value -= Q1;  low -= Q1;  high -= Q1;
		} else if (high > Q2) break;
		low += low;  high += high;
		value = 2 * value + GetBit();
	}
	ch = sym_to_char[sym];
	UpdateModel(sym);
	return ch;
}

int LZARI::DecodePosition(void)
{
	int position;
	unsigned long int  range;
	
	range = high - low;
	position = BinarySearchPos((unsigned int)
		(((value - low + 1) * position_cum[0] - 1) / range));
	high = low + (range * position_cum[position    ]) / position_cum[0];
	low +=       (range * position_cum[position + 1]) / position_cum[0];
	for ( ; ; ) {
		if (low >= Q2) {
			value -= Q2;  low -= Q2;  high -= Q2;
		} else if (low >= Q1 && high <= Q3) {
			value -= Q1;  low -= Q1;  high -= Q1;
		} else if (high > Q2) break;
		low += low;  high += high;
		value = 2 * value + GetBit();
	}
	return position;
}

/********** Encode and Decode **********/

void LZARI::Encode(void)
{
	int  i, c, len, r, s, last_match_length;
	
	if(!m_bMem)
	{
		fseek(infile, 0L, SEEK_END);
		textsize = ftell(infile);
		if (fwrite(&textsize, sizeof textsize, 1, outfile) < 1)
			Error("Write Error");  /* output size of text */
		codesize += sizeof textsize;
		if (textsize == 0) return;
		rewind(infile);
		textsize = 0;
	}
	else
	{
		textsize = m_nInLength;
		m_OutBuffer.resize(sizeof textsize);
		memcpy(&m_OutBuffer[0],&textsize,sizeof textsize);
		//m_nOutCur += sizeof textsize;
		codesize += sizeof textsize;
		if(textsize == 0) return;
		m_nInCur = 0;
		textsize = 0;
	}
	
	StartModel(); 
	InitTree();
	s = 0;  r = N - F;
	for (i = s; i < r; i++) text_buf[i] = ' ';
	if(!m_bMem)
		for (len = 0; len < F && (c = getc(infile)) != EOF; len++) text_buf[r + len] = c;
	else
		for (len = 0; len < F && m_nInCur < m_nInLength ; len++)
		{
			c = m_pInBuffer[m_nInCur++];
			text_buf[r + len] = c;
		}
	
	textsize = len;
	for (i = 1; i <= F; i++) InsertNode(r - i);
	
	InsertNode(r);
	
	do {
		if (match_length > len) match_length = len;
		if (match_length <= THRESHOLD) 
		{
			match_length = 1;  EncodeChar(text_buf[r]);
		} 
		else 
		{
			EncodeChar(255 - THRESHOLD + match_length);
			EncodePosition(match_position - 1);
		}
		last_match_length = match_length;
		if(!m_bMem)
		{
			for (i = 0; i < last_match_length && (c = getc(infile)) != EOF; i++) 
			{
				DeleteNode(s);  text_buf[s] = c;
				if (s < F - 1) text_buf[s + N] = c;
				s = (s + 1) & (N - 1);
				r = (r + 1) & (N - 1);
				InsertNode(r);
			}
		}
		else
		{			
			for (i = 0; i < last_match_length && m_nInCur < m_nInLength ; i++) 
			{
				c = m_pInBuffer[m_nInCur++];
				DeleteNode(s);  
				text_buf[s] = c;
				if (s < F - 1) text_buf[s + N] = c;
				s = (s + 1) & (N - 1);
				r = (r + 1) & (N - 1);
				InsertNode(r);
			}
		}
		if ((textsize += i) > printcount) 
		{
#ifdef _OUTPUT_STATUS
			printf("%12ld\r", textsize);
#endif	
			printcount += 1024;
		}
		while (i++ < last_match_length) 
		{
			DeleteNode(s);
			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);
			if (--len) InsertNode(r);
		}
	} while (len > 0);
	
	EncodeEnd();

#ifdef _OUTPUT_STATUS	
	printf("In : %lu bytes\n", textsize);
	printf("Out: %lu bytes\n", codesize);
	printf("Out/In: %.3f\n", (double)codesize / textsize);
#endif
}

void LZARI::Decode(void)
{
	int  i, j, k, r, c;
	unsigned long int  count;

	if (!m_bMem)
	{
		if (fread(&textsize, sizeof textsize, 1, infile) < 1)
			Error("Read Error");  /* read size of text */
	}
	else
	{
		if(m_nInLength < sizeof textsize)
			Error("Read Error");
		memcpy(&textsize,m_pInBuffer + m_nInCur,sizeof textsize);
		
		//m_OutBuffer.reserve(textsize);
		m_nOutLength = textsize;
		//m_nOutCur = 0;
		
		m_nInCur += sizeof textsize;
	}
	
	if (textsize == 0) return;
	
	StartDecode();
	StartModel();
	
	for (i = 0; i < N - F; i++) text_buf[i] = ' ';
	r = N - F;
	for (count = 0; count < textsize; ) 
	{
		c = DecodeChar();
		if (c < 256) 
		{
			if(!m_bMem)
				putc(c, outfile);
			else
			{
				//m_OutBuffer[m_nOutCur++] = c;
				m_OutBuffer.push_back(c);
				//m_nOutCur++;
			}
			text_buf[r++] = c;
			r &= (N - 1);
			count++;
		}
		else
		{
			i = (r - DecodePosition() - 1) & (N - 1);
			j = c - 255 + THRESHOLD;
			for (k = 0; k < j; k++) 
			{
				c = text_buf[(i + k) & (N - 1)];
				if(!m_bMem)
					putc(c, outfile);
				else
				{
				//	m_pOutBuffer[m_nOutCur++] = c;
					m_OutBuffer.push_back(c);
					//m_nOutCur ++;
				}
				text_buf[r++] = c;
				r &= (N - 1);
				count++;
			}
		}
		if (count > printcount) 
		{
#ifdef _OUTPUT_STATUS
			printf("%12lu\r", count);
#endif			
			printcount += 1024;
		}
	}

#ifdef _OUTPUT_STATUS
	printf("%12lu\n", count);
#endif
}

void LZARI::Compress(const char *lpszInfile,const char *lpszOutfile)
{
	m_bMem = FALSE;
	
	infile = fopen(lpszInfile,"rb");
	outfile = fopen(lpszOutfile,"wb");
	if(infile && outfile)
	{
		Encode();
		fclose(infile);
		fclose(outfile);
		infile = NULL;
		outfile = NULL;
	}
}

void LZARI::UnCompress(const char *lpszInfile,const char *lpszOutfile)
{
	m_bMem = FALSE;

	infile = fopen(lpszInfile,"rb");
	outfile = fopen(lpszOutfile,"wb");
	if(infile && outfile)
	{
		Decode();
		fclose(infile);
		fclose(outfile);
		infile = NULL;
		outfile = NULL;
	}
}

void LZARI::UnCompress(const BYTE *pInBuffer,int nInLength,const BYTE *&pOutBuffer ,int &nOutLength)
{
	m_pInBuffer = pInBuffer;
	m_nInLength = nInLength;
	m_nInCur = 0;

	m_bMem = TRUE;
	Decode();
	pOutBuffer = &m_OutBuffer[0];
	nOutLength = m_OutBuffer.size();
	//m_OutBuffer.push_back(0);
}

void LZARI::Compress(const char *lpszInfile,const BYTE * &pOutBuffer ,int &nOutLength)
{
	FILE* fp = fopen(lpszInfile, "rb");

	Compress(fp,pOutBuffer,nOutLength);

	fclose(fp);	
}

void LZARI::UnCompress(const BYTE *pInBuffer,int nInLength,const char *lpszOutfile)
{
	FILE* outfile = fopen(lpszOutfile,"wb");
	
	UnCompress(pInBuffer,nInLength,outfile);

	fclose(outfile);
}

void LZARI::Compress(const BYTE *pInBuffer,int nInLength,const BYTE *&pOutBuffer ,int &nOutLength)
{
	m_pInBuffer = pInBuffer;
	m_nInLength = nInLength;
	m_nInCur = 0;

	//	m_nOutCur = 0;

	m_bMem = TRUE;
	Encode();
	pOutBuffer = &m_OutBuffer[0];
	nOutLength = m_OutBuffer.size();
}

void LZARI::Compress(FILE *pInfile,const BYTE * &pOutBuffer ,int &nOutLength)
{
	ASSERT(pInfile);
	fseek(pInfile, 0, SEEK_END);
	long lSize = ftell(pInfile);
	fseek(pInfile, 0, SEEK_SET);
	BYTE* pBuf = new BYTE[lSize];
	fread(pBuf, lSize, 1, pInfile);

	Compress(pBuf,lSize,pOutBuffer,nOutLength);

	delete []pBuf;
}

void LZARI::UnCompress(const BYTE *pInBuffer,int nInLength,FILE *pOutfile)
{
	ASSERT(pOutfile);

	const BYTE* pOutBuf = NULL;
	int  lOutSize = 0;
	UnCompress(pInBuffer,nInLength,pOutBuf,lOutSize);
	fwrite(pOutBuf,lOutSize,1,pOutfile);

	Release();
}



void LZARI::Release()
{
	if(!m_OutBuffer.empty())
	{
		infile = NULL;
		outfile = NULL;
		
		textsize = 0;
		codesize = 0;
		printcount = 0;
		
		low = 0;
		high = Q4;
		value = 0;
		shifts = 0;
		
		m_bMem = FALSE;
		
		m_pInBuffer = NULL;
		m_nInLength = 0;
		m_nInCur = 0;
		
		m_OutBuffer.clear();
		m_nOutLength = 0;

		buffer_putbit = 0;
		mask_putbit = 128;
		
		buffer_getbit = 0;
		mask_getbit = 0;
	}
}