#include "stdafx.h"
#include "ClientPublic.h"

char* ConvertToFourCC(int n, char* str)
{
    for (int i=0; i<4; ++i)
        str[i] = n >> (i * 8) & 0x000000FF;
    return str;
}