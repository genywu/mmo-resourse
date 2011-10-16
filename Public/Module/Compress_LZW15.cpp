
#include "StdAfx.h"
#include "compress_lzw15.h"
#include "..\DataStream\OutputBitStream.h"
#include "..\DataStream\InputBitStream.h"
Compress_LZW15::Compress_LZW15(void)
{
	dict = new dictionary[ TABLE_SIZE];
}

Compress_LZW15::~Compress_LZW15(void)
{
	delete dict;
}




/*
 * This routine is used to initialize the dictionary, both when the
 * compressor or decompressor first starts up, and also when a flush
 * code comes in.  Note that even thought the decompressor sets all
 * the code_value elements to LZW_UNUSED, it doesn't really need to.
 */

void Compress_LZW15::InitializeDictionary()
{
    unsigned int i;

    for ( i = 0 ; i < TABLE_SIZE ; i++ )
        dict[ i].code_value = LZW_UNUSED;
    next_code = FIRST_CODE;
    putc( 'F', stdout );
    current_code_bits = 9;
    next_bump_code = 511;
}

/*
 * The compressor is short and simple.  It reads in new symbols one
 * at a time from the input file.  It then  checks to see if the
 * combination of the current symbol and the current code are already
 * defined in the dictionary.  If they are not, they are added to the
 * dictionary, and we start over with a new one symbol code.  If they
 * are, the code for the combination of the code and character becomes
 * our new code.  Note that in this enhanced version of LZW, the
 * encoder needs to check the codes for boundary conditions.
 */

void Compress_LZW15::CompressFile(FILE *input,OutputBitStream *output)
{
    int character;
    int string_code;
    unsigned int index;

    InitializeDictionary();
    if ( ( string_code = getc( input ) ) == EOF )
        string_code = LZW_END_OF_STREAM;
    while ( ( character = getc( input ) ) != EOF ) {
        index = find_child_node( string_code, character );
        if ( dict[index].code_value != - 1 )
            string_code = dict[ index ].code_value;
        else {
            dict[ index ].code_value = next_code++;
            dict[ index ].parent_code = string_code;
            dict[ index ].character = (char) character;
            output->OutputBits((ulong) string_code, current_code_bits );
            string_code = character;
            if ( next_code > MAX_CODE ) {
                output->OutputBits((ulong) FLUSH_CODE, current_code_bits );
                InitializeDictionary();
            } else if ( next_code > next_bump_code ) {
                output->OutputBits((ulong) BUMP_CODE, current_code_bits );
                current_code_bits++;
                next_bump_code <<= 1;
                next_bump_code |= 1;
                putc( 'B', stdout );
            }
        }
    }
    output->OutputBits((ulong) string_code, current_code_bits );
    output->OutputBits((ulong) LZW_END_OF_STREAM, current_code_bits);
}

void Compress_LZW15::ExpandFile(InputBitStream *input,FILE *output)
{
    unsigned int new_code;
    unsigned int old_code;
    int character;
    unsigned int count;

    for ( ; ; ) {
        InitializeDictionary();
        old_code = (unsigned int) input->InputBits(current_code_bits );
        if ( old_code == LZW_END_OF_STREAM )
            return;
        character = old_code;
        putc( old_code, output );
        for ( ; ; ) {
            new_code = (unsigned int) input->InputBits(current_code_bits );
            if ( new_code == LZW_END_OF_STREAM )
                return;
            if ( new_code == FLUSH_CODE )
                break;
            if ( new_code == BUMP_CODE ) {
                current_code_bits++;
                putc( 'B', stdout );
                continue;
            }
            if ( new_code >= next_code ) {
                decode_stack[ 0 ] = (char) character;
                count = decode_string( 1, old_code );
            }
            else
                count = decode_string( 0, new_code );
            character = decode_stack[ count - 1 ];
            while ( count > 0 )
                putc( decode_stack[ --count ], output );
            dict[ next_code ].parent_code = old_code;
            dict[ next_code ].character = (char) character;
            next_code++;
            old_code = new_code;
        }
    }
}

/*
 * This hashing routine is responsible for finding the table location
 * for a string/character combination.  The table index is created
 * by using an exclusive OR combination of the prefix and character.
 * This code also has to check for collisions, and handles them by
 * jumping around in the table.
 */

unsigned int Compress_LZW15::find_child_node(int parent_code,int child_character)
{
    unsigned int index;
    unsigned int offset;

    index = ( child_character << ( BITS - 8 ) ) ^ parent_code;
    if ( index == 0 )
        offset = 1;
    else
        offset = TABLE_SIZE - index;
    for ( ; ; ) {
        if ( dict[ index ].code_value == LZW_UNUSED )
            return( (unsigned int) index );
        if ( dict[ index ].parent_code == parent_code &&
             dict[ index ].character == (char) child_character )
            return( index );
        if ( (int) index >= offset )
            index -= offset;
        else
            index += TABLE_SIZE - offset;
    }
}

/*
 * This routine decodes a string from the dictionary, and stores it
 * in the decode_stack data structure.  It returns a count to the
 * calling program of how many characters were placed in the stack.
 */

unsigned int Compress_LZW15::decode_string(unsigned int count,unsigned int code)
{
    while ( code > 255 ) {
        decode_stack[ count++ ] = dict[ code ].character;
        code = dict[ code ].parent_code;
    }
    decode_stack[ count++ ] = (char) code;
    return( count );
}



