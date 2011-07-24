////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   InetStruct.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-09  14:34:34
//  Comment     :   Internet structure used tcp/ip, header file (from RFC)
//  
////////////////////////////////////////////////////////////////////////////////

#ifndef _INETSTRUCT_H
#define _INETSTRUCT_H   1


// IP Header -- RFC 791
typedef struct tagIPHDR
{
    unsigned char   VIHL;           // Version and IHL
    unsigned char   TOS;            // Type Of Service
    short           TotLen;         // Total Length
    short           ID;             // Identification
    short           FlagOff;        // Flags and Fragment Offset
    unsigned char   TTL;            // Time To Live
    unsigned char   Protocol;       // Protocol
    unsigned short  Checksum;       // Checksum
    unsigned long   SrcAddr;        // Internet Address - Source
    unsigned long   DstAddr;        // Internet Address - Destination
    
    // optional data begins here
    
} IPHDR, IPHEADER, *PIPHDR, *PIPHEADER;


#define ICMPTYPE_ECHOREPLY                  0
#define ICMPTYPE_DSTNOTTOUCH                3
#define ICMPTYPE_SRCRESTRAIN                4
#define ICMPTYPE_RELOCATE                   5
#define ICMPTYPE_ECHOREQUEST                8
#define ICMPTYPE_ROUTEBOARDCAST             9
#define ICMPTYPE_ROUTEREQUEST               10
#define ICMPTYPE_DGTIMEOUT                  11
#define ICMPTYPE_DAPARAMERROR               12
#define ICMPTYPE_TIMETAGREQUEST             13
#define ICMPTYPE_TIMETAGREPLY               14
#define ICMPTYPE_INFOREQUEST                15
#define ICMPTYPE_INFOREPLY                  16
#define ICMPTYPE_ADDRMASKREQUEST            17
#define ICMPTYPE_ADDRMASKREPLY              18


#define ICMP_DATAMINSIZE                    32

// ICMP Header - RFC 792
typedef struct tagICMPHDR
{
    unsigned char   Type;           // Type
    unsigned char   Code;           // Code
    unsigned short  Checksum;       // Checksum
    unsigned short  ID;             // Identification
    unsigned short  Seq;            // Sequence
    
    // optional data begins here
    
} ICMPHDR, ICMPHEADER, *PICMPHDR, *PICMPHEADER;


unsigned short  inet_chksum(unsigned short *data, int len);


#endif  // #ifndef _INETSTRUCT_H
