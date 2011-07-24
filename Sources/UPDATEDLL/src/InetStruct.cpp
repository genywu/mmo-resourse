////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   InetStruct.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-09  15:50:32
//  Comment     :   Internet structure used tcp/ip, source file (from RFC)
//  
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "InetStruct.h"


unsigned short inet_chksum(unsigned short *data, int len)
{
    register int nleft = len;
    register int sum   = 0;
    
    register unsigned short *w = data;
    register unsigned short answer;
    
    /*
    *  Our algorithm is simple, using a 32 bit accumulator (sum),
    *  we add sequential 16 bit words to it, and at the end, fold
    *  back all the carry bits from the top 16 bits into the lower
    *  16 bits.
    */
    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }
    
    /* mop up an odd byte, if necessary */
    if (nleft == 1)
    {
        unsigned short u = 0;
        
        *(unsigned char *)(&u) = *(unsigned char *)w ;
        sum += u;
    }
    
    /*
     * add back carry outs from top 16 bits to low 16 bits
     */
    sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
    sum += (sum >> 16);			        /* add carry */
    answer = ~sum;				        /* truncate to 16 bits */
    
    return (answer);
}
