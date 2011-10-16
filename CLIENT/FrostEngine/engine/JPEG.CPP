
#include "stdafx.h"
#include "futility.h"
#include "fconsole.h"
#include "frender.h"

void _stdcall MMX_IDCT8x8AAN(short *src, char *dst);

#pragma pack(push, old_align)
#pragma pack(1)

// HUFFMAN table content
struct tagHUFFMANTABLE_CONTENT
{
   WORD MASK;
   BYTE Size;
   BYTE Content;
};

// component information
struct tagCOMPONENT_INFO
{
   BYTE sampling_factors;
   BYTE QT_num;              // quantization table number
};

// JPEG decoder structure
struct tagJPEG_DECODER
{
   BYTE QuantizationTable[4][64];
   long AC_TABLE[4], DC_TABLE[4];
   BYTE HuffmanTable[8][16];
   long HuffmanTableSize[8]; // Total size of a huffman table
   tagHUFFMANTABLE_CONTENT HuffmanTableContent[8][256];

   long components_num;             // 1 = Grey, 3 = Color YCbCr
   tagCOMPONENT_INFO component_info[4];// 0 = Y, 1 = Cb, 2 = Cr

   long MCU_format;
   long DRI;

   DWORD *lpOutputBuffer;
   long lPitch;

   utility::File infile;         // source file
};

#pragma pack(pop, old_align)

////////////////////////////////////////////////////////////////////////////

#define JPEG_FORMAT_Y1     0x0001
#define JPEG_FORMAT_YUV111 0x0101  // no edit-tool support it
#define JPEG_FORMAT_YUV411 0x0102

static const BYTE * pStreamBuffer = NULL;       // stream buffer pointer
static long lStreamLength;						// length of the stream

static long ImageWidth, ImageHeight;			// actual image size
static long JPEG_Width, JPEG_Height;			// decode image size

static char *JPEG_Ybuf = NULL;					// Y buffer : Y Y Y Y Y Y ...
static char *JPEG_CbCrbuf = NULL;				// CbCr buffer : Cb Cr Cb Cr ...

static short JPEG_DC[3];

static DWORD StreamBitBuffer;
static long  StreamBitBufferCnt;

DWORD BRIGHTNESS[256];

static short YTABLE[256][4];
static short CbTABLE[256][4];
static short CrTABLE[256][4];

static tagHUFFMANTABLE_CONTENT *lpHuffmanTableContent;

////////////////////////////////////////////////////////////////

static DWORD HUFFMAN_BITMASK[33] =
{
   0x00000000, 0x00000001, 0x00000003, 0x00000007,
   0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
   0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
   0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
   0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
   0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
   0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
   0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
   0xffffffff
};

////////////////////////////////////////////////////////////////

static long ZIGZAG[64] =
{
    0,  8,  1,  2,  9, 16, 24, 17, // Transposed Matrix
   10,  3,  4, 11, 18, 25, 32, 40, // for MMX AAN IDCT
   33, 26, 19, 12,  5,  6, 13, 20,
   27, 34, 41, 48, 56, 49, 42, 35,
   28, 21, 14,  7, 15, 22, 29, 36,
   43, 50, 57, 58, 51, 44, 37, 30,
   23, 31, 38, 45, 52, 59, 60, 53,
   46, 39, 47, 54, 61, 62, 55, 63
};

////////////////////////////////////////////////////////////////


// <SetupYCbCrTable>
// # YCbCr => RGB
//
//     R = Y                        + 1.402   * (Cr - 128)
//     G = Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128)
//     B = Y + 1.772   * (Cb - 128)

static void SetupYCbCrTable(void)
{
	//这里改过i 
   long  i;
   short R, G, B;
   char  Cb, Cr;

   for(i = 0; i < 256; i ++)
   {
      BRIGHTNESS[i] = i * 0x10101;

      YTABLE[i][0] = (short)i;
      YTABLE[i][1] = (short)i;
      YTABLE[i][2] = (short)i;
      YTABLE[i][3] = 0;
   }

   R = 0;
   for(i = 0; i < 256; i ++)
   {
      Cb =(char) i - 128;
      G = (short)(- 0.34414 * (Cb));
      B = (short)(+ 1.772   * (Cb));

      CbTABLE[i][0] = B;
      CbTABLE[i][1] = G;
      CbTABLE[i][2] = R;
      CbTABLE[i][3] = 0;
   }

   B = 0;
   for(i = 0; i < 256; i ++)
   {
      Cr = (char)i - 128;

      R = (short)(+ 1.402   * (Cr));
      G = (short)(- 0.71414 * (Cr));

      CrTABLE[i][0] = B;
      CrTABLE[i][1] = G;
      CrTABLE[i][2] = R;
      CrTABLE[i][3] = 0;
   }
}

static BYTE JPEG_GetBYTE(tagJPEG_DECODER &jpeg)
{
   BYTE  res;

   if(lStreamLength > 0)
   {
      lStreamLength --;
      res = *pStreamBuffer;
	  pStreamBuffer++;
      return res;
   }
   else
   {
	   OutputError("  JPEG: STREAM_READ_OVERFLOW.\n");
      return NULL;
   }
}

static WORD JPEG_GetWORD(tagJPEG_DECODER &jpeg)
{
   BYTE  v[2];
   WORD  res;

   if(lStreamLength >= 2)
   {
      lStreamLength -= 2;
	
	  v[0] = pStreamBuffer[0];
	  v[1] = pStreamBuffer[1];

	  pStreamBuffer+=2;

      res = (v[0] << 8) + v[1];

      return res;
   }
   else
   {
		OutputError("  JPEG: STREAM_READ_OVERFLOW.\n");
		ASSERT(0);
   }
   return 0;
}

static void JPEG_GetBYTES(tagJPEG_DECODER &jpeg,
                          PVOID       pDst,
                          DWORD        dwSize)
{
   if(lStreamLength >= (long)dwSize)
   {
	   memcpy(pDst,pStreamBuffer,dwSize);
	   pStreamBuffer+=dwSize;
      lStreamLength -= dwSize;
   }
   else
   {
	   OutputError("  JPEG: STREAM_READ_OVERFLOW.\n");
   }
}

static void JPEG_SkipBYTES(tagJPEG_DECODER &jpeg, DWORD dwSize)
{
   if(lStreamLength >= (long)dwSize)
   {
      pStreamBuffer+=dwSize;
      lStreamLength -= dwSize;
   }
   else
   {
	   OutputError("  JPEG: STREAM_SKIP_OVERFLOW.\n");
   }
}

// FN: JPEG_GetStreamBYTE
//
 static BYTE JPEG_GetStreamBYTE(void)
 {
    if(lStreamLength > 0)
    {
       lStreamLength --;
       return *(pStreamBuffer ++);
    }
    else
    {
		OutputError("  JPEG: STREAM_READ_OVERFLOW.\n");
       return NULL;
    }
 }

static void JPEG_StreamFilter(void)
{
   __asm {
      MOV   ESI, pStreamBuffer
      MOV   ECX, lStreamLength
      MOV   EDI, ESI
      MOV   EDX, ECX          // EDX = ECX = lStreamLength
      MOV   AX, 0x00FF

Filter_Loop:

      MOVSB
      CMP   AX, WORD PTR [ESI - 1]
      JNZ   Filter_CheckNext
      INC   ESI               // "FF 00" found & skip a byte
      DEC   ECX
      DEC   EDX               // EDX (lStreamLength) --

Filter_CheckNext:

      DEC   ECX
      JG    Filter_Loop

      MOV   lStreamLength, EDX   // lStreamLength = EDX
   }
}

static long _fastcall ShowBits(long lBits)
{
   while(StreamBitBufferCnt < lBits)
   {
      StreamBitBuffer = (StreamBitBuffer << 8) + JPEG_GetStreamBYTE();
      StreamBitBufferCnt += 8;
   }

   return((StreamBitBuffer >> (StreamBitBufferCnt - lBits)) & HUFFMAN_BITMASK[lBits]);
}

static long _fastcall GetHuffmanBits(long lBits)
{
   DWORD res;

   while(StreamBitBufferCnt < lBits)
   {
      StreamBitBuffer = (StreamBitBuffer << 8) + JPEG_GetStreamBYTE();
      StreamBitBufferCnt += 8;
   }

   StreamBitBufferCnt -= lBits;
   res = ((StreamBitBuffer >> StreamBitBufferCnt) & HUFFMAN_BITMASK[lBits]);

   if(res <= HUFFMAN_BITMASK[lBits - 1])
   {
      res -= HUFFMAN_BITMASK[lBits];
   }

   return (short)res;
}

static short _fastcall GetHuffmanCode(void)
{
   tagHUFFMANTABLE_CONTENT *lpHufTable;
   long i;

   lpHufTable = lpHuffmanTableContent;

   for(i = 0; i < 256; i ++)
   {
      if(ShowBits(lpHufTable -> Size) == lpHufTable -> MASK)
      {
         StreamBitBufferCnt -= lpHufTable -> Size;
         return lpHufTable -> Content;
      }

      lpHufTable ++;
   }

   OutputError("  JPEG: ERROR_HUFFMAN_CODE_FOUND.\n");

   return NULL;
}

static void JPEG_DecodeDU(tagJPEG_DECODER &jpeg,
                          char *lpData, long lComponent)
{
   short    n, i, j;
   LPBYTE   lpQT;
   short    pBuf[64];

   // clear buffer
   __asm {
      LEA   EDI, pBuf
      XOR   EAX, EAX
      MOV   ECX, 128 / 4
      REP   STOSD
   }

   lpQT = jpeg.QuantizationTable[
                     jpeg.component_info[lComponent].QT_num];

   // get DC
   lpHuffmanTableContent =
         jpeg.HuffmanTableContent[jpeg.DC_TABLE[lComponent]];
   n = GetHuffmanCode();
   JPEG_DC[lComponent] += (short)GetHuffmanBits(n);
   pBuf[0] = ((JPEG_DC[lComponent] * lpQT[0]) << 4) + 8;  // + 8 = + .5 for I macroblock

   // get 63 AC
   lpHuffmanTableContent = jpeg.HuffmanTableContent[jpeg.AC_TABLE[lComponent]];

   i = 1;
   while(i < 64)
   {
      n = GetHuffmanCode();
      if(n != 0)
      {
         j = (n & 0x0F);
         i += (n >> 4);

		 pBuf[ZIGZAG[i]] =(short) ((GetHuffmanBits(j) * lpQT[i]) << 4);

         i ++;
      }
      else
      {
         i = 64;
      }
   }

   MMX_IDCT8x8AAN(pBuf, lpData);
}


static void JPEG_OutputDU(PVOID pSrc, PVOID pDst, long lPitch)
{
   __asm
   {
      MOV   EAX, lPitch                // EAX = lPitch
      MOV   ESI, pSrc
      LEA   EBX, [EAX + EAX * 2]       // EBX = 3 * lPitch
      MOV   EDI, pDst
      MOV   ECX, EAX
      MOV   EDX, EBX
      LEA   ECX, [ECX * 4 + ECX]       // ECX = 5 * lPitch
      LEA   EDX, [EDX * 2]             // EDX = 6 * lPitch

      MOVQ  MM0, [ESI]
      MOVQ  MM1, [ESI + 8]
      MOVQ  MM2, [ESI + 16]
      MOVQ  MM3, [ESI + 24]

      MOVQ  MM4, [ESI + 32]
      MOVQ  MM5, [ESI + 40]
      MOVQ  MM6, [ESI + 48]
      MOVQ  MM7, [ESI + 56]

      MOVQ  [EDI], MM0
      MOVQ  [EDI + EAX], MM1
      MOVQ  [EDI + EAX * 2], MM2
      MOVQ  [EDI + EBX], MM3

      MOVQ  [EDI + EAX * 4], MM4
      MOVQ  [EDI + ECX], MM5
      LEA   EBX, [EBX + EAX * 4]       // EBX = 7 * lPitch
      MOVQ  [EDI + EDX], MM6
      MOVQ  [EDI + EBX], MM7

      EMMS
   }
}

// FN: JPEG_OutputCbCrDU
// :CbCr use WORD buffer, HIGH-BYTE is Cb, LOW-BYTE is Cr

static void JPEG_OutputCbCrDU(PVOID lpCb, PVOID lpCr, PVOID pDst, long lPitch)
{
   __asm
   {
      MOV   ESI, lpCb
      MOV   EBX, lpCr
      MOV   EDI, pDst
      MOV   EDX, 8
      MOV   EAX, lPitch

LoopY:

      MOVD  MM0, [EBX]
      MOVD  MM1, [EBX + 4]

      PUNPCKLBW   MM0, [ESI]
      ADD   EBX, 8

      PUNPCKLBW   MM1, [ESI + 4]
      MOVQ  [EDI], MM0

      MOVQ  [EDI + 8], MM1
      ADD   ESI, 8

      ADD   EDI, EAX          // EAX = lPitch

      DEC   EDX
      JNZ   LoopY

      EMMS
   }
}

static __int64 x0001000100010001 = 0x0001000100010001;
static __int64 x0002000200020002 = 0x0002000200020002;

// The JPEG_UpSampleCbCr() handle Cb and Cr at the same time.
// The input data of is: Cb Cr Cb Cr Cb Cr  ...

static void JPEG_UpSampleCbCr(PVOID lpData)
{
   long  y, mul_size;

   y = JPEG_Height;
   mul_size = JPEG_Height * JPEG_Width;

   __asm
   {
      // Phase I :   Row-UpSample

      MOV   ESI, lpData    // pSrc = lpData
      MOV   EDI, ESI
      ADD   EDI, mul_size  // pDst = lpData + Width * Height
      PXOR  MM5, MM5
      MOVQ  MM6, x0001000100010001
      MOVQ  MM7, x0002000200020002

loopY_I_1:

      MOV   ECX, JPEG_Width
      SAR   ECX, 2         // JPEG_Width/2 samples per row

loopX_I_1:

      MOVD  MM0, [ESI - 2] // Cb1 Cr1 Cb0 Cr0
      MOVD  MM1, [ESI]     // Cb2 Cr2 Cb1 Cr1
      MOVD  MM2, [ESI + 2] // Cb3 Cr3 Cb2 Cr2

      PUNPCKLBW MM0, MM5
      PUNPCKLBW MM1, MM5
      PUNPCKLBW MM2, MM5

      MOVQ  MM4, MM1
      PADDUSW  MM1, MM1
      PADDUSW  MM1, MM4    // D2 * 3, D1 * 3

      PADDUSW  MM0, MM1    // D2 * 3 + D1, D1 * 3 + D0
      PADDUSW  MM2, MM1    // D2 * 3 + D3, D1 * 3 + D2

      PADDUSW  MM0, MM6    // D2 * 3 + D1 + 1,D1 * 3 + D0 + 1
      PADDUSW  MM2, MM7    // D2 * 3 + D3 + 2,D1 * 3 + D2 + 2

      PSRLW MM0, 2         // DATA / 4
      PSRLW MM2, 2         // DATA / 4

      MOVQ  MM3, MM0

      PUNPCKLDQ   MM0, MM2
      PUNPCKHDQ   MM3, MM2

      PACKUSWB MM0, MM0
      PACKUSWB MM3, MM3

      MOVD  [EDI], MM0
      MOVD  [EDI + 4], MM3

      ADD   ESI, 4
      ADD   EDI, 8

      DEC   ECX
      JNZ   loopX_I_1

      SUB   y, 2
      JG    loopY_I_1

      // patch first and last samples of each row

      MOV   ESI, lpData    // pSrc = lpData
      MOV   EDI, ESI
      ADD   EDI, mul_size  // pDst = lpData + Width * Height
      MOV   EDX, JPEG_Height
      MOV   EBX, JPEG_Width
      LEA   ECX, [EBX * 2 - 4]
      SUB   EBX, 4
      SAR   EDX, 2

loopY_I_2:

      MOVSW
      ADD   ESI, EBX
      ADD   EDI, ECX
      MOVSD
      ADD   ESI, EBX
      ADD   EDI, ECX
      MOVSW

      DEC   EDX
      JG    loopY_I_2


      // Phase II :  Col-UpSample

      // first row

      MOV   EDI, lpData    // pDst = lpData
      MOV   ECX, JPEG_Width
      MOV   ESI, EDI
      SAR   ECX, 1
      ADD   ESI, mul_size  // pSrc = lpData + Width * Height

      MOV   EAX, JPEG_Width
      ADD   EAX, EAX

      MOV   EBX, EAX
      NEG   EBX

      MOV   ECX, JPEG_Width
      SAR   ECX, 2

loopX_II_1:

      MOVD  MM0, [ESI]
      MOVD  MM1, [ESI + EAX]

      MOVD  [EDI], MM0
      PUNPCKLBW   MM0, MM5

      MOVD  MM2, [ESI + 4]
      PUNPCKLBW   MM1, MM5

      MOVD  MM3, [ESI + EAX + 4]
      MOVD  [EDI + 4], MM2

      MOVQ  MM4, MM0
      PADDUSW  MM1, MM7

      PUNPCKLBW   MM2, MM5
      PADDUSW  MM0, MM0

      PUNPCKLBW   MM3, MM5
      PADDUSW  MM1, MM0

      PADDUSW  MM3, MM7
      PADDUSW  MM1, MM4

      MOVQ  MM0, MM2
      PSRLW MM1, 2

      PADDUSW  MM2, MM2
      PACKUSWB MM1, MM1

      PADDUSW  MM2, MM0
      MOVD  [EDI + EAX], MM1

      PADDUSW  MM2, MM3
      ADD      ESI, 8

      PSRLW MM2, 2

      PACKUSWB MM2, MM2

      MOVD  [EDI + EAX + 4], MM2

      ADD   EDI, 8

      DEC   ECX
      JG    loopX_II_1

      ADD   EDI, EAX

      // middle row

      MOV   EDX, JPEG_Height
      SAR   EDX, 1
      SUB   EDX, 2

loopY_II_1:

      MOV   ECX, JPEG_Width
      SAR   ECX, 1

loopX_II_2:

      MOVD  MM0, [ESI + EBX]
      MOVD  MM1, [ESI]
      MOVD  MM2, [ESI + EAX]

      PUNPCKLBW MM0, MM5
      PUNPCKLBW MM1, MM5
      PUNPCKLBW MM2, MM5

      MOVQ  MM4, MM1
      PADDUSW  MM1, MM1
      PADDUSW  MM1, MM4

      PADDUSW  MM0, MM1
      PADDUSW  MM2, MM1

      PADDUSW  MM0, MM6
      PADDUSW  MM2, MM7

      PSRLW MM0, 2
      PSRLW MM2, 2

      PACKUSWB MM0, MM0
      PACKUSWB MM2, MM2

      MOVD  [EDI], MM0
      MOVD  [EDI + EAX], MM2

      ADD   ESI, 4
      ADD   EDI, 4

      DEC   ECX
      JG    loopX_II_2

      ADD   EDI, EAX

      DEC   EDX
      JG    loopY_II_1

      // last row

      MOV   ECX, JPEG_Width
      SAR   ECX, 2

loopX_II_3:

      MOVD  MM0, [ESI]
      MOVD  MM1, [ESI + EBX]

      MOVD  [EDI + EAX], MM0
      PUNPCKLBW   MM0, MM5

      MOVD  MM2, [ESI + 4]
      PUNPCKLBW   MM1, MM5

      MOVD  MM3, [ESI + EBX + 4]
      MOVD  [EDI + EAX + 4], MM2

      MOVQ  MM4, MM0
      PADDUSW  MM1, MM7

      PUNPCKLBW   MM2, MM5
      PADDUSW  MM0, MM0

      PUNPCKLBW   MM3, MM5
      PADDUSW  MM1, MM0

      PADDUSW  MM3, MM7
      PADDUSW  MM1, MM4

      MOVQ  MM0, MM2
      PSRLW MM1, 2

      PADDUSW  MM2, MM2
      PACKUSWB MM1, MM1

      PADDUSW  MM2, MM0
      MOVD  [EDI], MM1

      PADDUSW  MM2, MM3
      ADD   ESI, 8

      PSRLW MM2, 2

      PACKUSWB MM2, MM2

      MOVD  [EDI + 4], MM2

      ADD   EDI, 8

      DEC   ECX
      JG    loopX_II_3

      EMMS
   }
}

// FN: JPEG_DisplayMCU_Y1
// Display Brightness MCU (only one Y DU in the MCU)

static void JPEG_DisplayMCU_Y1(void  *pSrc,
                               DWORD *pDst, long lPitch)
{
   __asm
   {
      MOV   ESI, pSrc
      MOV   EDI, pDst
      MOV   EDX, 8
      LEA   EBX, BRIGHTNESS

LoopY:

      MOV   ECX, 8

LoopX:

      LODSB
      AND   EAX, 0x000000FF
      MOV   EAX, [EBX + EAX * 4]
      STOSD

      DEC   ECX
      JNZ   LoopX

      ADD   EDI, lPitch
      SUB   EDI, 32

      DEC   EDX
      JNZ   LoopY
   }
}

static void JPEG_DisplayMCU_YUV111(char *lpY, char *lpU, char *lpV, PDWORD pDst, long lPitch)
{
   long y = 8;

   __asm
   {
      MOV   EDI, pDst
      MOV   ESI, lpY
      MOV   EBX, lpU
      MOV   EDX, lpV
      XOR   EAX, EAX

LoopY:

      MOV   ECX, 7

LoopX:

      MOV   AL, [ESI + ECX]

      MOVQ  MM0, [YTABLE + EAX * 8]

      MOV   AL, [EDX + ECX]

      MOVQ  MM1, [CrTABLE + EAX * 8]

      MOV   AL, [EBX + ECX]

      MOVQ  MM2, [CbTABLE + EAX * 8]

      PADDSW   MM0, MM1
      PADDSW   MM0, MM2

      PACKUSWB MM0, MM0

      MOVD     [EDI + ECX * 4], MM0

      DEC   ECX
      JGE   LoopX

      ADD   ESI, 8
      ADD   EBX, 8
      ADD   EDX, 8
      ADD   EDI, lPitch

      DEC   DWORD PTR y
      JNZ   LoopY

      EMMS
   }
}

// YCbCr Image -> RGB Image
__declspec(naked) void _stdcall JPEG_ConvYUVtoRGB(DWORD *pDst, long lPitch)
{
   __asm
   {
      PUSH  EBP
      PUSH  ESI
      PUSH  EDI
      PUSH  EBX

      SUB   ESP, 4                  // long height;
      MOV   ESI, JPEG_Ybuf          // lpY
      MOV   EBP, JPEG_CbCrbuf       // lpCbCr

      MOV   EDI, [ESP + 20 + 4]     // [ESP + 24] = pDst
      MOV   EAX, ImageHeight

      XOR   EBX, EBX
      MOV   [ESP + 4], EAX          // height = ImageHeight

      XOR   EDX, EDX
      XOR   EAX, EAX

loopY:

      MOV   ECX, JPEG_Width

loopX:

      MOV   AL, [ESI]
      MOV   BL, [EBP]
      MOV   DL, [EBP + 1]

      MOVQ  MM0, [YTABLE + EAX * 8]
      MOVQ  MM1, [CrTABLE + EBX * 8]
      MOVQ  MM2, [CbTABLE + EDX * 8]

      PADDSW   MM0, MM1
      PADDSW   MM0, MM2
      PACKUSWB MM0, MM0

      MOVD  [EDI], MM0

      INC   ESI
      ADD   EBP, 2

      ADD   EDI, 4

      DEC   ECX
      JNZ   loopX

      MOV   ECX, JPEG_Width
      ADD   EDI, [ESP + 20 + 8]     // [ESP + 28] = lPitch
      LEA   ECX, [ECX * 4]
      SUB   EDI, ECX

      DEC   DWORD PTR [ESP + 4]
      JNZ   loopY

      ADD   ESP, 4

      POP   EBX
      POP   EDI
      POP   ESI
      POP   EBP

      EMMS

      RET   8
   }
}

static void JPEG_ProcessDQT(tagJPEG_DECODER &jpeg)
{
   long  SegLen, QT_num, QT_prec;
   BYTE  Buf;

   SegLen = (JPEG_GetWORD(jpeg) - 2);

   while(SegLen > 0)
   {
      // Get QT information
      Buf = JPEG_GetBYTE(jpeg);
      SegLen --;

      QT_num = (Buf & 0x0F);
      QT_prec = (Buf >> 4);

      if(QT_prec != 0)
      {  // 16-bit QT
		  OutputError("  JPEG: DQT_16BIT_QT_FOUND.\n");
      }
      else
      {  // 8-bit QT
         JPEG_GetBYTES(jpeg,
                       jpeg.QuantizationTable[QT_num], 64);
         SegLen -= 64;
      }
   }
}

static void JPEG_ProcessSOF0(tagJPEG_DECODER &jpeg)
{
   long  SegLen, i, j;
   BYTE  Buf[4];

   SegLen = JPEG_GetWORD(jpeg);

   // data precision, I only support 8-bits/sample
   if(JPEG_GetBYTE(jpeg) != 8)
   {
	   OutputError("  JPEG: SOF0_PRECISION_NOT_8BIT.\n");
   }

   ImageHeight = JPEG_GetWORD(jpeg);
   ImageWidth = JPEG_GetWORD(jpeg);

   if(ImageWidth <=0 || ImageHeight <= 0)
   {
	   OutputError("  JPEG: IMAGE_SIZE_IS_ZERO.\n");
   }

   jpeg.components_num = JPEG_GetBYTE(jpeg);

   if(jpeg.components_num != 1 && jpeg.components_num != 3)
   {
	   OutputError("  JPEG: SOF0_UNSUPPORT_COMPONENTS_NUM.\n");
   }

   SegLen -= 8;

   if(SegLen != jpeg.components_num * 3)
   {
	   OutputError("  JPEG: SOF0_ERROR_SEGMENT_LENGTH.\n");
   }

   for(i=0; i< jpeg.components_num; i++)
   {
      JPEG_GetBYTES(jpeg, Buf, 3);
      j = Buf[0] - 1;
      jpeg.component_info[j].sampling_factors = Buf[1];
      jpeg.component_info[j].QT_num = Buf[2];
   }

   // analyze JPEG MCU format
   if(jpeg.components_num == 1)  // Y (gray only)
   {
      if(jpeg.component_info[0].sampling_factors != 0x11)
      {
		  OutputError("  JPEG: SOF0_UNSUPPORT_FORMAT.\n");
      }

      JPEG_Width = (ImageWidth + 7) & 0xFFFFFFF8;
      JPEG_Height = (ImageHeight + 7) & 0xFFFFFFF8;

      jpeg.MCU_format = JPEG_FORMAT_Y1;
   }
   else
   if(jpeg.components_num == 3)  // Y Cb Cr
   {
	   if( jpeg.component_info[0].sampling_factors == 0x11 &&
		   jpeg.component_info[1].sampling_factors == 0x11 &&
		   jpeg.component_info[2].sampling_factors == 0x11)
	   {
		   JPEG_Width = (ImageWidth + 7) & 0xFFFFFFF8;
		   JPEG_Height = (ImageHeight + 7) & 0xFFFFFFF8;

		   jpeg.MCU_format = JPEG_FORMAT_YUV111;
	   }
	   else
		   if( jpeg.component_info[0].sampling_factors == 0x22 &&
			   jpeg.component_info[1].sampling_factors == 0x11 &&
			   jpeg.component_info[2].sampling_factors == 0x11)
		   {
			   JPEG_Width = (ImageWidth + 15) & 0xFFFFFFF0;
			   JPEG_Height = (ImageHeight + 15) & 0xFFFFFFF0;

			   jpeg.MCU_format = JPEG_FORMAT_YUV411;
		   }
		   else
		   {
			   OutputError("  JPEG: SOF0_UNSUPPORT_FORMAT.\n");
		   }
   }
   else
   {
	   OutputError("  JPEG: SOF0_UNSUPPORT_FORMAT.\n");
   }
}

static void JPEG_ProcessDHT(tagJPEG_DECODER &jpeg)
{
   long  SegLen, HT_num, i, j, count;
   DWORD code;
   BYTE  Info;

   SegLen = JPEG_GetWORD(jpeg) - 2;

   while(SegLen > 16)
   {
      Info = JPEG_GetBYTE(jpeg);
      HT_num = Info & 0x0F;
      if(HT_num > 3)
      {
		  OutputError("  JPEG: DHT_INFO_NUM_ERROR.\n");
      }
      if((Info & 0x10) != 0)
      {
         HT_num += 4;
      }
      JPEG_GetBYTES(jpeg, jpeg.HuffmanTable[HT_num], 16);
      SegLen -= 16;

      // HuffmanTableContent[8][256];
      // 2B - MASK
      // 1B - Size
      // 1B - Content

      count = 0;
      for(i = 1; i <= 16; i ++)
      {
         for(j = 0;
                  j < jpeg.HuffmanTable[HT_num][i - 1]; j ++)
         {
            jpeg.HuffmanTableContent[HT_num][count].Size
                                                  = (BYTE) i;
            jpeg.HuffmanTableContent[HT_num][count].Content
                                        = JPEG_GetBYTE(jpeg);
            count ++;
         }
      }
      jpeg.HuffmanTableSize[HT_num] = count;
      SegLen -= count;

      // generate the HUFFMAN codes
      code = 0;
      i = 0;
      j = jpeg.HuffmanTableContent[HT_num][0].Size;

      while(i < count)
      {
         while(i < count
            && jpeg.HuffmanTableContent[HT_num][i].Size == j)
         {
            jpeg.HuffmanTableContent[HT_num][i ++].MASK
                                                = (WORD)code;
            code ++;
         }

         if(code >= (DWORD)(1 << j))
         {
			 OutputError("  JPEG: DHT_CONTENT_ERROR.\n");
         }

         code <<= 1;
         j ++;
      }
   }
}

static void JPEG_ProcessSOS(tagJPEG_DECODER &jpeg)
{
   char  JPEG_DU_Ybuf[64 * 4];
   char  JPEG_DU_Cbbuf[64 * 1];
   char  JPEG_DU_Crbuf[64 * 1];
   long  SegLen, n, id, i, j, cnt = 0;

   // get length
   SegLen = JPEG_GetWORD(jpeg);

   // get number of components in scan
   n = JPEG_GetBYTE(jpeg);
   if(n != 1 && n != 3)
   {
	   OutputError("  JPEG: SOS_COMPONENT_NUM_ERROR.\n");
   }

   if(SegLen != (6 + (n << 1)))
   {
	   OutputError("  JPEG: SOS_SEGMENT_LENGTH_ERROR.\n");
   }

   for(i = 0; i < n; i ++)
   {
      // get component id
      id = JPEG_GetBYTE(jpeg);
      if(id < 1 || id > 3)
      {
		  OutputError("  JPEG: SOS_NOT_YUV_ID.\n");
      }
      id --;

      // get huffman table to use
      j = JPEG_GetBYTE(jpeg);
      jpeg.DC_TABLE[id] = (j >> 4);
      jpeg.AC_TABLE[id] = (j & 0x0F) + 4;
   }

   // skip the additional scan parameters Ss, Se, Ah/Al
   JPEG_SkipBYTES(jpeg, 3);

   // cache stream
   //OutputError("fuck");

   // check the stream to remove "FF 00"
   JPEG_StreamFilter();

   // stream cached
   ///////////////////

   // clear bit-read buffer
   StreamBitBuffer = 0;
   StreamBitBufferCnt = 0;

   switch(jpeg.MCU_format)
   {
      case JPEG_FORMAT_Y1:

         JPEG_DC[0] = 0;

         for(j = 0; j< JPEG_Height; j += 8)
         for(i = 0; i< JPEG_Width; i += 8)
         {
            JPEG_DecodeDU(jpeg, JPEG_DU_Ybuf, 0);
            JPEG_DisplayMCU_Y1(JPEG_DU_Ybuf,
                    jpeg.lpOutputBuffer+ j * JPEG_Width + i, jpeg.lPitch);

            if((++ cnt) == jpeg.DRI)
            {
               cnt = 0;

               if(*pStreamBuffer == 0xFF &&
                               *(pStreamBuffer + 1) >= 0xD0
                            && *(pStreamBuffer + 1) <= 0xD7)
               {
                  // RST0 - RST7    FF D0 - FF D7
                  pStreamBuffer += 2;
                  StreamBitBufferCnt = 0;
                  StreamBitBuffer = 0;
                  JPEG_DC[0] = 0;
               }
            }
		 }
		 break;
	  case JPEG_FORMAT_YUV111:

		  JPEG_DC[0] = JPEG_DC[1] = JPEG_DC[2] = 0;

		  for(j = 0; j< JPEG_Height; j += 8)
			  for(i = 0; i< JPEG_Width; i += 8)
			  {
				  JPEG_DecodeDU(jpeg, JPEG_DU_Ybuf, 0);
				  JPEG_DecodeDU(jpeg, JPEG_DU_Cbbuf, 1);
				  JPEG_DecodeDU(jpeg, JPEG_DU_Crbuf, 2);

				  JPEG_DisplayMCU_YUV111(JPEG_DU_Ybuf,
					  JPEG_DU_Cbbuf, JPEG_DU_Crbuf,
					  jpeg.lpOutputBuffer + j * JPEG_Width + i, jpeg.lPitch);

				  if((++ cnt) == jpeg.DRI)
				  {
					  cnt = 0;

					  if(*pStreamBuffer == 0xFF &&
						  *(pStreamBuffer + 1) >= 0xD0
						  && *(pStreamBuffer + 1) <= 0xD7)
					  {
						  // RST0 - RST7    FF D0 - FF D7
						  pStreamBuffer += 2;
						  StreamBitBufferCnt = 0;
						  StreamBitBuffer = 0;
						  JPEG_DC[0] = JPEG_DC[1] = JPEG_DC[2] = 0;
					  }
				  }
			  }
			  break;
      case JPEG_FORMAT_YUV411:
         // alloc Y, CbCr buffer
		  //注意释放
         JPEG_Ybuf = (char *)new BYTE[JPEG_Width * JPEG_Height * 3];
         JPEG_CbCrbuf = JPEG_Ybuf + JPEG_Width * JPEG_Height;


         JPEG_DC[0] = JPEG_DC[1] = JPEG_DC[2] = 0;

         // decode MCUs
         for(j = 0; j< JPEG_Height; j += 16)
         for(i = 0; i< JPEG_Width; i += 16)
         {
            JPEG_DecodeDU(jpeg, JPEG_DU_Ybuf, 0);
            JPEG_DecodeDU(jpeg, JPEG_DU_Ybuf + 64, 0);
            JPEG_DecodeDU(jpeg, JPEG_DU_Ybuf + 128, 0);
            JPEG_DecodeDU(jpeg, JPEG_DU_Ybuf + 192, 0);
            JPEG_DecodeDU(jpeg, JPEG_DU_Cbbuf, 1);
            JPEG_DecodeDU(jpeg, JPEG_DU_Crbuf, 2);

            JPEG_OutputDU(JPEG_DU_Ybuf,
                              JPEG_Ybuf + j * JPEG_Width + i,
                                                 JPEG_Width);
            JPEG_OutputDU(JPEG_DU_Ybuf + 64,
                          JPEG_Ybuf + j * JPEG_Width + i + 8,
                                                 JPEG_Width);
            JPEG_OutputDU(JPEG_DU_Ybuf + 128,
                        JPEG_Ybuf + (j + 8) * JPEG_Width + i,
                                                 JPEG_Width);
            JPEG_OutputDU(JPEG_DU_Ybuf + 192,
                    JPEG_Ybuf + (j + 8) * JPEG_Width + i + 8,
                                                 JPEG_Width);

            JPEG_OutputCbCrDU(JPEG_DU_Cbbuf, JPEG_DU_Crbuf,
                  JPEG_CbCrbuf + ((j * JPEG_Width) >> 1) + i,
                                                 JPEG_Width);

            if((++ cnt) == jpeg.DRI)
            {
               cnt = 0;

               if(*pStreamBuffer == 0xFF &&
                               *(pStreamBuffer + 1) >= 0xD0
                            && *(pStreamBuffer + 1) <= 0xD7)
               {
                  // RST0 - RST7    FF D0 - FF D7
                  pStreamBuffer += 2;
                  StreamBitBufferCnt = 0;
                  StreamBitBuffer = 0;
                  JPEG_DC[0] = JPEG_DC[1] = JPEG_DC[2] = 0;
               }
            }
         }

         // UpSample CbCr
         JPEG_UpSampleCbCr(JPEG_CbCrbuf);

         // YUV -> RGB
         JPEG_ConvYUVtoRGB(jpeg.lpOutputBuffer,jpeg.lPitch);

		 SAFEDELETE(JPEG_Ybuf);
         break;
   }
}

BOOL render::Texture::BitmapReadFileJPG(const char *pszFileName)
{
   tagJPEG_DECODER jpeg;

   if(!jpeg.infile.Open(pszFileName)) return FALSE;

   // get file length of source stream
   lStreamLength = jpeg.infile.GetLength();
   pStreamBuffer = jpeg.infile.GetData();

   SetupYCbCrTable();

   /* DECODE START FROM HERE */

   // SOI = Start Of Image
   if(JPEG_GetWORD(jpeg) != 0xFFD8)
   {
	   OutputError("  JPEG: header \"SOI\" (FF D8) not found.--- \"%s\".\n", pszFileName);
      return false;
   }

   jpeg.DRI = 0;

   // error handle

   {
      for(;;)
      {
         // MARKER analyze
         switch(JPEG_GetWORD(jpeg))
         {
            // APP0 = JFIF APP0 segment marker
            case 0xFFE0:
               // some information we don't need, skip them
               JPEG_SkipBYTES(jpeg, JPEG_GetWORD(jpeg) - 2);
               break;

            // DQT  = Define Quantization Table
            case 0xFFDB:
               JPEG_ProcessDQT(jpeg);
               break;

            // SOF0 = Start Of Frame 0                = FF C0
            case 0xFFC0:
               JPEG_ProcessSOF0(jpeg);

			   if(CreateBitmap(JPEG_Width, JPEG_Height) == false)
               {
                  OutputError("  JPEG: CrateBitmap %d x %d failed.---\"%s\".\n", JPEG_Width,JPEG_Height,pszFileName);
	  			    ASSERT(0);
               };

               // set output buffer and pitch
			   jpeg.lpOutputBuffer = m_pBitmap;
			   jpeg.lPitch         = m_dwWidth << 2;
			   //jpeg.lPitch         =  - jpeg.lPitch;		

               // set visible area
               m_dwWidth  = ImageWidth;
               m_dwHeight = ImageHeight;

               break;

            // DHT  = Define Huffman Table            = FF C4
            case 0xFFC4:
               JPEG_ProcessDHT(jpeg);
               break;

            // SOS  = Start Of Scan                   = FF DA
            case 0xFFDA:
               // decode scanline
               if(m_dwWidth  <=0 ||
                  m_dwHeight <= 0)
               {
				  
				  jpeg.infile.Close();
                  return FALSE;
               }

               JPEG_ProcessSOS(jpeg);

               // I assume only one SOS in the stream
               // that means we don't support multi-scan]

			   //setup alpha
			   for(DWORD i = 0;i < m_dwWidth * m_dwHeight; i++)
				{
					m_pBitmap[i] |= 0xff000000;
				}

				jpeg.infile.Close();
				

               return TRUE;

            // DRI  = Define Restart Interval         = FF DD
            case 0xFFDD:
               JPEG_GetWORD(jpeg);
               jpeg.DRI = JPEG_GetWORD(jpeg);
               break;

            // DNL  = Define Number of Lines          = FF DC
            case 0xFFDC:

            // COM  = Comment                         = FF FE
            case 0xFFFE:
               // skip them
               JPEG_SkipBYTES(jpeg, JPEG_GetWORD(jpeg) - 2);
               break;

            // EOI    = End Of Image
            case 0xFFD9:

			   
			   jpeg.infile.Close();

               return FALSE;
            // ???
            case 0xFFC2:
            case 0xFFE2:
            case 0xFFED:
            case 0xFFEE:
               JPEG_SkipBYTES(jpeg, JPEG_GetWORD(jpeg) - 2);
               break;

            default:
				OutputError("  JPEG: UNKNOWN_MARKER.\n");
			    jpeg.infile.Close();
               return FALSE;
         }
      }  
   }
   jpeg.infile.Close();
   
   return FALSE;
}

