#include "stdafx.h"
#include "memcpySSE.h"
/*
SSE扩展指令主要针对8个128位寄存器(xmm0 - xmm7)
内存间的数据拷贝以这8个CPU寄存器作为中转
在完全利用8个xmm寄存器最小化LOOP次数的原则下,一次移动的数据量是8*128bit=128byte
参数_size最好能被128整除 源地址与目的地址同时16字节对齐的情况下 效率最佳
太小的内存块拷贝不会有明显的性能提升
*/

void memcpySSE(void* dest,const void* src,size_t _size)
{
	if(_size<=1024)
	{
		memcpy(dest,src,_size);
	}
	else
	{
		__asm
		{
			//首先检测src地址是否16byte对齐
			mov eax,src;
			mov ebx,16;
			mov edx,0;
			div ebx;
			cmp edx,0;
			//edx非0则跳转
			jnz addr_unaligned;
			//采用地址对齐mov指令 比非对齐方式快一倍
			//addr_aligned: 
			//数据块是否按128byte对齐
			mov eax,_size;
			mov ebx,128;
			mov edx,0;
			div ebx;

			mov esi,src;
			mov edi,dest;
aligned_loop:
			//数据预取 32byte偏移的寻址 添充128byte的CPU CACHE
			prefetchnta 128[ESI];
			prefetchnta 160[ESI];
			prefetchnta 192[ESI];
			prefetchnta 224[ESI];

			movdqa xmm0,0[ESI];//src to 寄存器
			movdqa xmm1,16[ESI];
			movdqa xmm2,32[ESI];
			movdqa xmm3,48[ESI];
			movdqa xmm4,64[ESI];
			movdqa xmm5,80[ESI];
			movdqa xmm6,96[ESI];
			movdqa xmm7,112[ESI];

			movntdq 0[EDI],xmm0;//寄存器 to dst
			movntdq 16[EDI],xmm1;
			movntdq 32[EDI],xmm2;
			movntdq 48[EDI],xmm3;
			movntdq 64[EDI],xmm4;
			movntdq 80[EDI],xmm5;
			movntdq 96[EDI],xmm6;
			movntdq 112[EDI],xmm7;

			add esi,128;
			add edi,128;
			dec eax;
			jnz aligned_loop;
			//aligned_loop_end:
			//如果edx非0 操作剩余byte
			mov ecx,edx;
			rep movsb;	
			jmp memcpy_sse_end;
addr_unaligned:
			mov eax,_size;
			mov ebx,128;
			mov edx,0;
			div ebx;			

			mov esi,src;
			mov edi,dest;
unaligned_loop:
			prefetchnta 128[ESI];
			prefetchnta 160[ESI];
			prefetchnta 192[ESI];
			prefetchnta 224[ESI];

			movdqu xmm0,0[ESI];//src to 寄存器
			movdqu xmm1,16[ESI];
			movdqu xmm2,32[ESI];
			movdqu xmm3,48[ESI];
			movdqu xmm4,64[ESI];
			movdqu xmm5,80[ESI];
			movdqu xmm6,96[ESI];
			movdqu xmm7,112[ESI];

			movdqu 0[EDI],xmm0;//寄存器 to dst
			movdqu 16[EDI],xmm1;
			movdqu 32[EDI],xmm2;
			movdqu 48[EDI],xmm3;
			movdqu 64[EDI],xmm4;
			movdqu 80[EDI],xmm5;
			movdqu 96[EDI],xmm6;
			movdqu 112[EDI],xmm7;

			add esi,128;
			add edi,128;
			dec eax;
			jnz unaligned_loop;
			//unaligned_loop_end:
			mov ecx,edx;
			rep movsb;
memcpy_sse_end:
		}	
	}
}

void memcpySSE_Aligned(void* dest,const void* src,size_t _size)
{
	__asm
	{
		mov esi,src;
		mov edi,dest;
		mov ebx,_size;
		shr ebx,7;
aligned_loop:
		//数据预取 32byte偏移的寻址 添充128byte的CPU CACHE
		prefetchnta 128[ESI];
		prefetchnta 160[ESI];
		prefetchnta 192[ESI];
		prefetchnta 224[ESI];

		movdqa xmm0,0[ESI];//src to 寄存器
		movdqa xmm1,16[ESI];
		movdqa xmm2,32[ESI];
		movdqa xmm3,48[ESI];
		movdqa xmm4,64[ESI];
		movdqa xmm5,80[ESI];
		movdqa xmm6,96[ESI];
		movdqa xmm7,112[ESI];

		movntdq 0[EDI],xmm0;//寄存器 to dst
		movntdq 16[EDI],xmm1;
		movntdq 32[EDI],xmm2;
		movntdq 48[EDI],xmm3;
		movntdq 64[EDI],xmm4;
		movntdq 80[EDI],xmm5;
		movntdq 96[EDI],xmm6;
		movntdq 112[EDI],xmm7;

		add esi,128;
		add edi,128;
		dec ebx;
		jnz aligned_loop;
	}
}

void memcpy_movsb(void* dst,void* src,size_t _size)
{
	__asm
	{
		mov esi,src;
		mov edi,dst;
		mov ecx,_size;
		rep movsb;
	}
}

void memcpyMMX(void *dst, void *src, int nQWORDs)
{
#define CACHEBLOCK 1024 //一个块中有多少QWORDs
	//修改此值有可能实现更高的速度
	int n=((int)(nQWORDs/CACHEBLOCK))*CACHEBLOCK;
	int m=nQWORDs-n;
	if (n)
	{
		_asm //下面先拷贝整数个块
		{
			mov esi, src
			mov edi, dst
			mov ecx, n			//要拷贝多少个块
			lea esi, [esi+ecx*8]
			lea edi, [edi+ecx*8]
			neg ecx
mainloop:
			mov eax, CACHEBLOCK / 16
prefetchloop:
			mov ebx, [esi+ecx*8] 		//预读此循环
			mov ebx, [esi+ecx*8+64]	//预读下循环
			add ecx, 16
			dec eax
			jnz prefetchloop
			sub ecx, CACHEBLOCK
			mov eax, CACHEBLOCK / 8
writeloop:
			movq mm0, qword ptr [esi+ecx*8    ]
			movq mm1, qword ptr [esi+ecx*8+8 ]
			movq mm2, qword ptr [esi+ecx*8+16]
			movq mm3, qword ptr [esi+ecx*8+24]
			movq mm4, qword ptr [esi+ecx*8+32]
			movq mm5, qword ptr [esi+ecx*8+40]
			movq mm6, qword ptr [esi+ecx*8+48]
			movq mm7, qword ptr [esi+ecx*8+56]

			movntq qword ptr [edi+ecx*8   ], mm0
			movntq qword ptr [edi+ecx*8+8 ], mm1
			movntq qword ptr [edi+ecx*8+16], mm2
			movntq qword ptr [edi+ecx*8+24], mm3
			movntq qword ptr [edi+ecx*8+32], mm4
			movntq qword ptr [edi+ecx*8+40], mm5
			movntq qword ptr [edi+ecx*8+48], mm6
			movntq qword ptr [edi+ecx*8+56], mm7
			add ecx, 8
			dec eax
			jnz writeloop
			or ecx, ecx
			jnz mainloop
		}
	}
	if (m)
	{
		_asm
		{
			mov esi, src
			mov edi, dst
			mov ecx, m
			mov ebx, nQWORDs
			lea esi, [esi+ebx*8]
			lea edi, [edi+ebx*8]
			neg ecx
copyloop:
			prefetchnta [esi+ecx*8+512]  //预读
			movq mm0, qword ptr [esi+ecx*8   ]
			movq mm1, qword ptr [esi+ecx*8+8 ]
			movq mm2, qword ptr [esi+ecx*8+16]
			movq mm3, qword ptr [esi+ecx*8+24]
			movq mm4, qword ptr [esi+ecx*8+32]
			movq mm5, qword ptr [esi+ecx*8+40]
			movq mm6, qword ptr [esi+ecx*8+48]
			movq mm7, qword ptr [esi+ecx*8+56]

			movntq qword ptr [edi+ecx*8   ], mm0
			movntq qword ptr [edi+ecx*8+8 ], mm1
			movntq qword ptr [edi+ecx*8+16], mm2
			movntq qword ptr [edi+ecx*8+24], mm3
			movntq qword ptr [edi+ecx*8+32], mm4
			movntq qword ptr [edi+ecx*8+40], mm5
			movntq qword ptr [edi+ecx*8+48], mm6
			movntq qword ptr [edi+ecx*8+56], mm7
			add ecx, 8
			jnz copyloop
			sfence
			emms
		}
	}
	else
	{
		_asm
		{
			sfence
			emms
		}
	}
}


LONGLONG GetSecondCount()
{
	static LARGE_INTEGER liCounter={0};
	if(0==liCounter.QuadPart)
		QueryPerformanceFrequency(&liCounter);
	return liCounter.QuadPart;
}

DWORD highGetTime()
{
	LARGE_INTEGER liCurrent={0};
	QueryPerformanceCounter(&liCurrent);
	return (DWORD)(liCurrent.QuadPart*1000/GetSecondCount());
}