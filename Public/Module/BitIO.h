/************************** Start of BITIO.H *************************/

#pragma once

typedef struct bit_file {
    FILE *file;
    uchar mask;
    int rack;
    int pacifier_counter;
} BIT_FILE;

BIT_FILE     *OpenInputBitFile( char *name );
BIT_FILE     *OpenOutputBitFile( char *name );
void          OutputBit( BIT_FILE *bit_file, int bit );
void          OutputBits( BIT_FILE *bit_file,
                          ulong code, int count );
int           InputBit( BIT_FILE *bit_file );
ulong         InputBits( BIT_FILE *bit_file, int bit_count );
void          CloseInputBitFile( BIT_FILE *bit_file );
void          CloseOutputBitFile( BIT_FILE *bit_file );
void          FilePrintBinary( FILE *file, unsigned int code, int bits );






