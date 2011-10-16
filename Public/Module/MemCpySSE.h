#ifndef MEMCPY_SSE_H
#define MEMCPY_SSE_H

void memcpySSE(void* dest,const void* src,size_t _size);
void memcpySSE_Aligned(void* dest,const void* src,size_t _size);
void memcpyMMX(void *dst, void *src, int nQWORDs);
void memcpy_movsb(void* dst,void* src,size_t _size);
LONGLONG GetSecondCount();
ulong highGetTime();

#endif