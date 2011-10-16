//

#include "stdafx.h"

static __int64 x0005000200010001 = 0x0005000200010001;
static __int64 x0040000000000000 = 0x0040000000000000;
static __int64 x5a825a825a825a82 = 0x5a825a825a825a82;//23170
static __int64 x539f539f539f539f = 0x539f539f539f539f;//21407
static __int64 x4546454645464546 = 0x4546454645464546;//17734
static __int64 x61f861f861f861f8 = 0x61f861f861f861f8;//25080

static __int64 x8080808080808080 = 0x8080808080808080;//128

static __int64 scratch1 = 0;
static __int64 scratch3 = 0;
static __int64 scratch5 = 0;
static __int64 scratch7 = 0;

static unsigned short preSC[] =
{
   16384, 22725, 21407, 19266, 16384, 12873, 8867,  4520,
   22725, 31521, 29692, 26722, 22725, 17855, 12299, 6270,
   21407, 29692, 27969, 25172, 21407, 16819, 11585, 5906,
   19266, 26722, 25172, 22654, 19266, 15137, 10426, 5315,
   16384, 22725, 21407, 19266, 16384, 12873, 8867,  4520,
   12873, 17855, 16819, 15137, 25746, 20228, 13933, 7103,
   17734, 24598, 23170, 20853, 17734, 13933, 9597,  4892,
   18081, 25080, 23624, 21261, 18081, 14206, 9785,  4988
};

__declspec(naked) void _stdcall MMX_IDCT8x8AAN(short *src, char *dst)
{
   __asm {
      PUSH  ESI
      LEA   ECX, [preSC]

      PUSH  EDI
      MOV   ESI, [ESP + 12]            // source

      MOV   EDI, [ESP + 16]            // dest

      // column 0: even part
      // use V4, V12, V0, V8 to produce V22..V25
      MOVQ  MM0, [ECX + 8 * 12] // maybe the first mul can be
                                // done together with the
                                // dequantization in iHuff
                                // module ?

      PMULHW   MM0, [ESI + 8 * 12]     // V12

      MOVQ  MM1, [ECX + 8 * 4]

      PMULHW   MM1, [ESI + 8 * 4]      // V4

      MOVQ  MM3, [ECX + 8 * 0]
      PSRAW MM0, 1                     // t64=t66

      PMULHW   MM3, [ESI + 8 * 0]      // V0

      MOVQ  MM5, [ECX + 8 * 8]         // duplicate V4
      MOVQ  MM2, MM1                   // added 11/1/96

      PMULHW   MM5, [ESI + 8 * 8]      // V8
      PSUBSW   MM1, MM0                // V16


      PMULHW   MM1, x5a825a825a825a82  // 23170 -> V18
      PADDSW   MM2, MM0                // V17

      MOVQ  MM0, MM2                   // duplicate V17
      PSRAW MM2, 1                     // t75=t82

      PSRAW MM0, 2                     // t72
      MOVQ  MM4, MM3                   // duplicate V0

      PADDSW   MM3, MM5                // V19
      PSUBSW   MM4, MM5                // V20, mm5 free

      // moved from the block below
      MOVQ  MM7, [ECX + 8 * 10]
      PSRAW MM3, 1                     // t74=t81

      MOVQ  MM6, MM3                   // duplicate t74=t81
      PSRAW MM4, 2                     // t77=t79

      PSUBSW   MM1, MM0                // V21, mm0 free
      PADDSW   MM3, MM2                // V22

      MOVQ  MM5, MM1                   // duplicate V21
      PADDSW   MM1, MM4                // V23

      MOVQ  [ESI + 8 * 4], MM3         // V22
      PSUBSW   MM4, MM5                // V24, mm5 free

      MOVQ  [ESI + 8 * 12], MM1        // V23
      PSUBSW   MM6, MM2                // V25, mm2 free

      MOVQ  [ESI + 8 * 0], MM4         // V24

      // column 0: odd part
      // use V2, V6, V10, V14 to produce V31, V39, V40, V41

      PMULHW   MM7, [ESI + 8 * 10]     // V10

      MOVQ  MM0, [ECX + 8 * 6]

      PMULHW   MM0, [ESI + 8 * 6]      // V6

      MOVQ  MM5, [ECX + 8 * 2]
      MOVQ  MM3, MM7                   // duplicate V10

      PMULHW   MM5, [ESI + 8 * 2]      // V2

      MOVQ  MM4, [ECX + 8 * 14]
      PSUBSW   MM7, MM0                // V26

      PMULHW   MM4, [ESI + 8 * 14]     // V14
      PADDSW   MM3, MM0                // V29, free mm0

      MOVQ  MM1, MM7                   // duplicate V26
      PSRAW MM3, 1                     // t91=t94

      PMULHW   MM7, x539f539f539f539f  // V33
      PSRAW MM1, 1                     // t96

      MOVQ  MM0, MM5                   // duplicate V2
      PSRAW MM4, 2                     // t85=t87

      PADDSW   MM5, MM4                // V27
      PSUBSW   MM0, MM4                // V28, free mm4

      MOVQ  MM2, MM0                   // duplicate V28
      PSRAW MM5, 1                     // t90=t93

      PMULHW   MM0, x4546454645464546  // V35
      PSRAW MM2, 1                     // t97

      MOVQ  MM4, MM5                   // duplicate t90=t93
      PSUBSW   MM1, MM2                // V32, free mm2

      PMULHW   MM1, x61f861f861f861f8  // V36
      PSLLW MM7, 1                     // t107

      PADDSW   MM5, MM3                // V31
      PSUBSW   MM4, MM3                // V30, free mm3

      PMULHW   MM4, x5a825a825a825a82  // V34
      NOP

      PSUBSW   MM0, MM1                // V38
      PSUBSW   MM1, MM7                // V37, free mm7

      PSLLW MM1, 1                     // t114
      // move from the next block
      MOVQ  MM3, MM6                   // duplicate V25

      // move from the next block
      MOVQ  MM7, [ESI + 8 * 4]         // V22
      PSLLW MM0, 1                     // t110

      PSUBSW MM0, MM5 // V39(mm5 still needed for next block)
      PSLLW MM4, 2                     // t112

      // move from the next block
      MOVQ  MM2, [ESI + 8 * 12]        // V23
      PSUBSW   MM4, MM0                // V40

      PADDSW   MM1, MM4                // V41, free mm0
      // move from the next block
      PSLLW MM2, 1                     // t117=t125

      // column 0: output butterfly

      PSUBSW   MM6, MM1                // tm6
      PADDSW   MM3, MM1                // tm8, free mm1

      MOVQ  MM1, MM7                   // duplicate V22
      PADDSW   MM7, MM5                // tm0

      MOVQ  [ESI + 8 * 8], MM3         // tm8, free mm3
      PSUBSW   MM1, MM5                // tm14, free mm5

      MOVQ  [ESI + 8 * 6], MM6         // tm6, free mm6
      MOVQ  MM3, MM2                   // duplicate t117=t125

      MOVQ  MM6, [ESI + 8 * 0]         // V24
      PADDSW   MM2, MM0                // tm2

      MOVQ  [ESI + 8 * 0], MM7         // tm0, free mm7
      PSUBSW   MM3, MM0                // tm12, free mm0

      MOVQ  [ESI + 8 * 14], MM1        // tm14, free mm1
      PSLLW MM6, 1                     // t119=t123

      MOVQ  [ESI + 8 * 2], MM2         // tm2, free mm2
      MOVQ  MM0, MM6                   // duplicate t119=t123

      MOVQ  [ESI + 8 * 12], MM3        // tm12, free mm3
      PADDSW   MM6, MM4                // tm4

      // move from the next block
      MOVQ  MM1, [ECX + 8 * 5]
      PSUBSW   MM0, MM4                // tm10, free mm4

      // move from the next block
      PMULHW   MM1, [ESI + 8 * 5]      // V5

      MOVQ  [ESI + 8 * 4], MM6         // tm4, free mm6

      MOVQ  [ESI + 8 * 10], MM0        // tm10, free mm0

      // column 1: even part
      // use V5, V13, V1, V9 to produce V56..V59

      MOVQ  MM7, [ECX + 8 * 13]
      PSLLW MM1, 1                     // t128=t130

      PMULHW   MM7, [ESI + 8 * 13]     // V13
      MOVQ  MM2, MM1                   // duplicate t128=t130

      MOVQ  MM3, [ECX + 8 * 1]

      PMULHW   MM3, [ESI + 8 * 1]      // V1

      MOVQ  MM5, [ECX + 8 * 9]
      PSUBSW   MM1, MM7                // V50

      PMULHW   MM5, [ESI + 8 * 9]      // V9
      PADDSW   MM2, MM7                // V51

      PMULHW   MM1, x5a825a825a825a82  // 23170 ->V52
      MOVQ  MM6, MM2                   // duplicate V51

      PSRAW MM2, 1                     // t138=t144
      MOVQ  MM4, MM3                   // duplicate V1

      PSRAW MM6, 2                     // t136
      PADDSW   MM3, MM5                // V53

      PSUBSW   MM4, MM5                // V54, mm5 free
      MOVQ  MM7, MM3                   // duplicate V53

      // move from the next block
      MOVQ  MM0, [ECX + 8 * 11]
      PSRAW MM4, 1                     // t140=t142

      PSUBSW   MM1, MM6                // V55, mm6 free
      PADDSW   MM3, MM2                // V56

      MOVQ  MM5, MM4                   // duplicate t140=t142
      PADDSW   MM4, MM1                // V57

      MOVQ  [ESI + 8 * 5], MM3         // V56
      PSUBSW   MM5, MM1                // V58, mm1 free

      MOVQ  [ESI + 8 * 13], MM4        // V57
      PSUBSW   MM7, MM2                // V59, mm2 free

      MOVQ  [ESI + 8 * 9], MM5         // V58

      PMULHW   MM0, [ESI + 8 * 11]     // V11

      MOVQ  MM6, [ECX + 8 * 7]

      PMULHW   MM6, [ESI + 8 * 7]      // V7

      MOVQ  MM4, [ECX + 8 * 15]
      MOVQ  MM3, MM0                   // duplicate V11

      PMULHW   MM4, [ESI + 8 * 15]     // V15

      MOVQ  MM5, [ECX + 8 * 3]
      PSLLW MM6, 1                     // t146=t152

      PMULHW   MM5, [ESI + 8 * 3]      // V3
      PADDSW   MM0, MM6                // V63

      // note that V15 computation has a correction step:
      // this is a 'magic' constant that rebiases the results
      // to be closer to the expected result this magic
      // constant can be refined to reduce the error even
      // more by doing the correction step in a later stage
      // when the number is actually multiplied by 16

      PADDW MM4, x0005000200010001
      PSUBSW   MM3, MM6                // V60, free mm6

      PSRAW MM0, 1                     // t154=t156
      MOVQ  MM1, MM3                   // duplicate V60

      PMULHW   MM1, x539f539f539f539f  // V67
      MOVQ  MM6, MM5                   // duplicate V3

      PSRAW MM4, 2                     // t148=t150

      PADDSW   MM5, MM4                // V61
      PSUBSW   MM6, MM4                // V62, free mm4

      MOVQ  MM4, MM5                   // duplicate V61
      PSLLW MM1, 1                     // t169

      PADDSW   MM5, MM0                // V65 -> result
      PSUBSW   MM4, MM0                // V64, free mm0

      PMULHW   MM4, x5a825a825a825a82  // V68
      PSRAW MM3, 1                     // t158

      PSUBSW   MM3, MM6                // V66
      MOVQ  MM2, MM5                   // duplicate V65

      PMULHW   MM3, x61f861f861f861f8  // V70
      PSLLW MM6, 1                     // t165

      PMULHW   MM6, x4546454645464546  // V69
      PSRAW MM2, 1                     // t172

      // move from the next block
      MOVQ  MM0, [ESI + 8 * 5]         // V56
      PSLLW MM4, 1                     // t174

      // move from the next block
      PSRAW MM0, 1                     // t177=t188
      NOP

      PSUBSW   MM6, MM3                // V72
      PSUBSW   MM3, MM1                // V71, free mm1

      PSUBSW   MM6, MM2                // V73, free mm2
      // move from the next block
      PSRAW MM5, 1                     // t178=t189

      PSUBSW   MM4, MM6                // V74
      // move from the next block
      MOVQ  MM1, MM0                   // duplicate t177=t188

      PADDSW   MM3, MM4                // V75
      // move from the next block
      PADDSW   MM0, MM5                // tm1

      // location
      // 5 - V56
      // 13 - V57
      // 9 - V58
      // X - V59, mm7
      // X - V65, mm5
      // X - V73, mm6
      // X - V74, mm4
      // X - V75, mm3
      // free mm0, mm1 & mm2

      MOVQ  MM2, [ESI + 8 * 13]        // V57
      PSUBSW   MM1, MM5                // tm15, free mm5

      MOVQ  [ESI + 8 * 1], MM0         // tm1, free mm0
      PSRAW MM7, 1                     // t182=t184 ! new !!

      // save the store as used directly in the transpose
      // movq [esi+8*15], mm1          // tm15, free mm1
      MOVQ  MM5, MM7                   // duplicate t182=t184
      PSUBSW   MM7, MM3                // tm7

      PADDSW   MM5, MM3                // tm9, free mm3

      MOVQ  MM0, [ESI + 8 * 9]         // V58
      MOVQ  MM3, MM2                   // duplicate V57

      MOVQ  [ESI + 8 * 7], MM7         // tm7, free mm7
      PSUBSW   MM3, MM6                // tm13

      PADDSW   MM2, MM6                // tm3, free mm6
      // moved up from the transpose
      MOVQ  MM7, MM3

      // moved up from the transpose
      PUNPCKLWD   MM3, MM1
      MOVQ  MM6, MM0                   // duplicate V58

      MOVQ  [ESI + 8 * 3], MM2         // tm3, free mm2
      PADDSW   MM0, MM4                // tm5

      PSUBSW   MM6, MM4                // tm11, free mm4
      // moved up from the transpose
      PUNPCKHWD   MM7, MM1

      MOVQ  [ESI + 8 * 5], MM0         // tm5, free mm0
      // moved up from the transpose
      MOVQ  MM2, MM5

      // transpose - M4 part
      // ---------       ---------
      // | M1 | M2 |     | M1'| M3'|
      // ---------  -->  ---------
      // | M3 | M4 |     | M2'| M4'|
      // ---------       ---------
      // Two alternatives: use full mmword approach so the
      // following code can be scheduled before the transpose
      // is done without stores, or use the faster half
      // mmword stores (when possible)

      MOVD  [ESI + 8 * 9 + 4], MM3     // MS part of tmt9
      PUNPCKLWD   MM5, MM6

      MOVD  [ESI + 8 * 13 + 4], MM7    // MS part of tmt13
      PUNPCKHWD   MM2, MM6

      MOVD  [ESI + 8 * 9], MM5         // LS part of tmt9
      PUNPCKHDQ   MM5, MM3             // free mm3

      MOVD  [ESI + 8 * 13], MM2        // LS part of tmt13
      PUNPCKHDQ   MM2, MM7             // free mm7

      // moved up from the M3 transpose
      MOVQ  MM0, [ESI + 8 * 8]

      // moved up from the M3 transpose
      MOVQ  MM1, [ESI + 8 * 10]
      // moved up from the M3 transpose
      MOVQ  MM3, MM0

      // shuffle the rest of the data, and write it with
      // 2 mmword writes
      MOVQ  [ESI + 8 * 11], MM5  // tmt11
      // moved up from the M3 transpose
      PUNPCKLWD   MM0, MM1

      MOVQ  [ESI + 8 * 15], MM2        // tmt15
      // moved up from the M3 transpose
      PUNPCKHWD   MM3, MM1

      // transpose - M3 part

      MOVQ MM6, [ESI + 8 * 12]

      MOVQ MM4, [ESI + 8 * 14]
      MOVQ MM2, MM6

      // shuffle the data and write out the lower parts of
      // the transposed in 4 dwords
      PUNPCKLWD   MM6, MM4
      MOVQ  MM1, MM0

      PUNPCKHDQ   MM1, MM6
      MOVQ  MM7, MM3

      PUNPCKHWD   MM2, MM4             // free mm4

      PUNPCKLDQ   MM0, MM6             // free mm6

      // move from the next block
      MOVQ  MM4, [ESI + 8 * 13]        // tmt13
      PUNPCKLDQ   MM3, MM2

      PUNPCKHDQ   MM7, MM2             // free mm2
      // move from the next block
      MOVQ  MM5, MM3                   // duplicate tmt5

      // column 1: even part (after transpose)

      PSUBSW   MM3, MM4                // V134

      PMULHW   MM3, x5a825a825a825a82  // 23170 ->V136

      MOVQ  MM6, [ESI + 8 * 9]         // tmt9
      PADDSW   MM5, MM4                // V135, mm4 free

      MOVQ  MM4, MM0                   // duplicate tmt1
      PADDSW   MM0, MM6                // V137

      PSUBSW   MM4, MM6                // V138, mm6 free
      PSLLW MM3, 2                     // t290
      PSUBSW   MM3, MM5                // V139
      MOVQ  MM6, MM0                   // duplicate V137

      PADDSW   MM0, MM5                // V140
      MOVQ  MM2, MM4                   // duplicate V138

      PADDSW   MM2, MM3                // V141
      PSUBSW   MM4, MM3                // V142, mm3 free

      MOVQ  [ESI + 8 * 9], MM0         // V140
      PSUBSW   MM6, MM5                // V143, mm5 free

      // move from the next block
      MOVQ  MM0, [ESI + 8 * 11]        // tmt11

      MOVQ  [ESI + 8 * 13], MM2        // V141
      // move from the next block
      MOVQ  MM2, MM0                   // duplicate tmt11

      // column 1: odd part (after transpose)

      MOVQ  MM5, [ESI + 8 * 15]        // tmt15
      PSUBSW   MM0, MM7                // V144

      MOVQ  MM3, MM0                   // duplicate V144
      PADDSW   MM2, MM7                // V147, free mm7

      PMULHW   MM0, x539f539f539f539f  // 21407-> V151
      MOVQ  MM7, MM1                   // duplicate tmt3

      PADDSW   MM7, MM5                // V145
      PSUBSW   MM1, MM5                // V146, free mm5

      PSUBSW   MM3, MM1                // V150
      MOVQ  MM5, MM7                   // duplicate V145

      PMULHW   MM1, x4546454645464546  // 17734-> V153
      PSUBSW   MM5, MM2                // V148

      PMULHW   MM3, x61f861f861f861f8  // 25080-> V154
      PSLLW MM0, 2                     // t311

      PMULHW   MM5, x5a825a825a825a82  // 23170-> V152
      PADDSW   MM7, MM2                // V149, free mm2

      PSLLW MM1, 1                     // t313
      NOP

      // without the nop above - freeze here for one clock
      // the nop cleans the mess a little bit
      MOVQ  MM2, MM3                   // duplicate V154
      PSUBSW   MM3, MM0                // V155, free mm0

      PSUBSW   MM1, MM2                // V156, free mm2
      // moved from the next block
      MOVQ  MM2, MM6                   // duplicate V143

      // moved from the next block
      MOVQ  MM0, [ESI + 8 * 13]        // V141
      PSLLW MM1, 1                     // t315

      PSUBSW   MM1, MM7                // V157 (keep V149)
      PSLLW MM5, 2                     // t317

      PSUBSW   MM5, MM1                // V158
      PSLLW MM3, 1                     // t319

      PADDSW   MM3, MM5                // V159

      // column 1: output butterfly (after transform)

      PSUBSW   MM2, MM3                // V163
      PADDSW   MM6, MM3                // V164, free mm3

      MOVQ  MM3, MM4                   // duplicate V142
      PSUBSW   MM4, MM5                // V165, free mm5

      MOVQ  scratch7, MM2              // out7
      PSRAW MM6, 4

      PSRAW MM4, 4
      PADDSW   MM3, MM5                // V162

      MOVQ  MM2, [ESI + 8 * 9]         // V140
      MOVQ  MM5, MM0                   // duplicate V141

      // in order not to perculate this line up,
      // we read [esi+8*9] very near to this location
      MOVQ  [ESI + 8 * 9], MM6         // out9
      PADDSW   MM0, MM1                // V161

      MOVQ  scratch5, MM3              // out5
      PSUBSW   MM5, MM1                // V166, free mm1

      MOVQ  [ESI + 8 * 11], MM4        // out11
      PSRAW MM5, 4

      MOVQ  scratch3, MM0              // out3
      MOVQ  MM4, MM2                   // duplicate V140

      MOVQ  [ESI + 8 * 13], MM5        // out13
      PADDSW   MM2, MM7                // V160

      // moved from the next block
      MOVQ  MM0, [ESI + 8 * 1]
      PSUBSW   MM4, MM7                // V167, free mm7

      // moved from the next block
      MOVQ  MM7, [ESI + 8 * 3]
      PSRAW MM4, 4

      MOVQ  scratch1, MM2              // out1
      // moved from the next block
      MOVQ  MM1, MM0

      MOVQ  [ESI + 8 * 15], MM4        // out15
      // moved from the next block
      PUNPCKLWD   MM0, MM7

      // transpose - M2 parts

      MOVQ  MM5, [ESI + 8 * 5]
      PUNPCKHWD   MM1, MM7

      MOVQ  MM4, [ESI + 8 * 7]
      MOVQ  MM3, MM5

      // shuffle the data and write out the lower parts of
      // the trasposed in 4 dwords
      MOVD  [ESI + 8 * 8], MM0         // LS part of tmt8
      PUNPCKLWD   MM5, MM4

      MOVD  [ESI + 8 * 12], MM1        // LS part of tmt12
      PUNPCKHWD   MM3, MM4

      MOVD  [ESI + 8 * 8 + 4], MM5     // MS part of tmt8
      PUNPCKHDQ   MM0, MM5             // tmt10

      MOVD  [ESI + 8 * 12 + 4], MM3    // MS part of tmt12
      PUNPCKHDQ   MM1, MM3             // tmt14


      // transpose - M1 parts
      MOVQ  MM7, [ESI]

      MOVQ  MM2, [ESI + 8 * 2]
      MOVQ  MM6, MM7

      MOVQ  MM5, [ESI + 8 * 4]
      PUNPCKLWD   MM7, MM2

      MOVQ  MM4, [ESI + 8 * 6]
      PUNPCKHWD   MM6, MM2             // free mm2

      MOVQ  MM3, MM5
      PUNPCKLWD   MM5, MM4

      PUNPCKHWD   MM3, MM4             // free mm4
      MOVQ  MM2, MM7

      MOVQ  MM4, MM6
      PUNPCKLDQ   MM7, MM5             // tmt0

      PUNPCKHDQ   MM2, MM5             // tmt2, free mm5

      // shuffle the rest of the data, and write it with
      // 2 mmword writes
      PUNPCKLDQ   MM6, MM3             // tmt4
      // move from next block
      MOVQ  MM5, MM2                   // duplicate tmt2

      PUNPCKHDQ   MM4, MM3             // tmt6, free mm3
      // move from next block
      MOVQ  MM3, MM0                   // duplicate tmt10

      // column 0: odd part (after transpose)

      PSUBSW   MM0, MM4                // V110
      PADDSW   MM3, MM4                // V113, free mm4

      MOVQ  MM4, MM0                   // duplicate V110
      PADDSW   MM2, MM1                // V111

      PMULHW   MM0, x539f539f539f539f  // 21407-> V117
      PSUBSW   MM5, MM1                // V112, free mm1

      PSUBSW   MM4, MM5                // V116
      MOVQ  MM1, MM2                   // duplicate V111

      PMULHW   MM5, x4546454645464546  // 17734-> V119
      PSUBSW   MM2, MM3                // V114

      PMULHW   MM4, x61f861f861f861f8  // 25080-> V120
      PADDSW   MM1, MM3                // V115, free mm3

      PMULHW   MM2, x5a825a825a825a82  // 23170-> V118
      PSLLW MM0, 2                     // t266

      MOVQ  [ESI + 8 * 0], MM1         // save V115
      PSLLW MM5, 1                     // t268

      PSUBSW   MM5, MM4                // V122
      PSUBSW   MM4, MM0                // V121, free mm0

      PSLLW MM5, 1                     // t270

      PSUBSW   MM5, MM1                // V123, free mm1
      PSLLW MM2, 2                     // t272

      PSUBSW   MM2, MM5                // V124 (keep V123)
      PSLLW MM4, 1                     // t274

      MOVQ  [ESI + 8 * 2], MM5         // save V123, free mm5
      PADDSW   MM4, MM2                // V125 (keep V124)

      // column 0: even part (after transpose)
      MOVQ  MM0, [ESI + 8 * 12]        // tmt12
      MOVQ  MM3, MM6                   // duplicate tmt4

      PSUBSW   MM6, MM0                // V100
      PADDSW   MM3, MM0                // V101, free mm0

      PMULHW   MM6, x5a825a825a825a82  // 23170 ->V102
      MOVQ  MM5, MM7                   // duplicate tmt0

      MOVQ  MM1, [ESI + 8 * 8]         // tmt8

      PADDSW   MM7, MM1                // V103
      PSUBSW   MM5, MM1                // V104, free mm1

      MOVQ  MM0, MM7                   // duplicate V103
      PSLLW MM6, 2                     // t245

      PADDSW   MM7, MM3                // V106
      MOVQ  MM1, MM5                   // duplicate V104

      PSUBSW   MM6, MM3                // V105
      PSUBSW   MM0, MM3                // V109, free mm3

      PADDSW   MM5, MM6                // V107
      PSUBSW   MM1, MM6                // V108, free mm6

      // column 0: output butterfly (after transform)
      MOVQ  MM3, MM1                   // duplicate V108
      PADDSW   MM1, MM2                // out4

      PSRAW MM1, 4
      PSUBSW   MM3, MM2                // out10, free mm2

      PSRAW MM3, 4
      MOVQ  MM6, MM0                   // duplicate V109

      MOVQ  [ESI + 8 * 4], MM1         // out4, free mm1
      PSUBSW   MM0, MM4                // out6

      MOVQ  [ESI + 8 * 10], MM3        // out10, free mm3
      PSRAW MM0, 4

      PADDSW   MM6, MM4                // out8, free mm4
      MOVQ  MM1, MM7                   // duplicate V106

      MOVQ  [ESI + 8 * 6], MM0         // out6, free mm0
      PSRAW MM6, 4

      MOVQ  MM4, [ESI + 8 * 0]         // V115

      MOVQ  [ESI + 8 * 8], MM6         // out8, free mm6
      MOVQ  MM2, MM5                   // duplicate V107

      MOVQ  MM3, [ESI + 8 * 2]         // V123
      PADDSW   MM7, MM4                // out0

      // moved up from next block
      MOVQ  MM0, scratch3
      PSRAW MM7, 4

      // moved up from next block
      MOVQ  MM6, scratch5
      PSUBSW   MM1, MM4                // out14, free mm4

      PADDSW   MM5, MM3                // out2
      PSRAW MM1, 4

      MOVQ  [ESI + 8 * 14], MM1        // out14, free mm1
      PSRAW MM5, 4

      PSUBSW   MM2, MM3                // out12, free mm3
      PSRAW MM2, 4

      MOVQ  MM4, scratch7
      PSRAW MM0, 4

      MOVQ  MM1, scratch1
      PSRAW MM6, 4

      // MM7   = out0,  MM5 = out2, MM2 = out12,
      // MM0 = out3, MM6 = out5, MM4 = out7, MM1 = out1

      // Saturation PACK data into the byte-group

      PACKSSWB MM5, MM0
      MOVQ     MM3, x8080808080808080

      PSRAW    MM1, 4
      PADDB    MM5, MM3

      PACKSSWB MM7, MM1
      MOVQ     [EDI + 4 * 2], MM5      // out2, out3

      PSRAW    MM4, 4
      PADDB    MM7, MM3

      MOVQ     MM5, [ESI + 8 * 9]
      MOVQ     [EDI], MM7              // out0, out1

      PACKSSWB MM2, [ESI + 8 * 13]
      MOVQ     MM7, [ESI + 8 * 4]

      MOVQ     MM1, [ESI + 8 * 8]
      PACKSSWB MM7, MM6

      PACKSSWB MM1, MM5
      PADDB    MM7, MM3

      MOVQ     MM6, [ESI + 8 * 6]
      MOVQ     [EDI + 4 * 4], MM7      // out4, out5

      PADDB    MM1, MM3
      PACKSSWB MM6, MM4

      MOVQ     MM0, [ESI + 8 * 11]
      MOVQ     [EDI + 4 * 8], MM1      // out8, out9

      MOVQ     MM7, [ESI + 8 * 10]
      PADDB    MM2, MM3

      MOVQ     MM5, [ESI + 8 * 14]
      MOVQ     [EDI + 4 * 12], MM2     // out12, out13

      PACKSSWB MM7, MM0
      MOVQ     MM1, [ESI + 8 * 15]

      PADDB    MM7, MM3
      PADDB    MM6, MM3

      PACKSSWB MM5, MM1
      MOVQ     [EDI + 4 * 10], MM7     // out10, out11

      PADDB    MM5, MM3
      MOVQ     [EDI + 4 * 6], MM6      // out6, out7

      MOVQ     [EDI + 4 * 14], MM5     // out14, out15

      EMMS

      POP      EDI
      POP      ESI

      RET      8
   }
}
