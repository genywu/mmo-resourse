#pragma once

#define BITS                       15
#define MAX_CODE                   ( ( 1 << BITS ) - 1 )
#define TABLE_SIZE                 35023L
#define TABLE_BANKS                ( ( TABLE_SIZE >> 8 ) + 1 )
#define LZW_END_OF_STREAM          256
#define BUMP_CODE                  257
#define FLUSH_CODE                 258
#define FIRST_CODE                 259
#define LZW_UNUSED                 -1

class OutputBitStream;
class InputBitStream;

class Compress_LZW15
{

	struct dictionary {
		int code_value;
		int parent_code;
		char character;
	} *dict;

	
	char decode_stack[ TABLE_SIZE ];
	unsigned int next_code;
	int current_code_bits;
	unsigned int next_bump_code;


	void InitializeDictionary();

	unsigned int find_child_node( int parent_code, int child_character );
	unsigned int decode_string( unsigned int offset, unsigned int code );

public:
	Compress_LZW15(void);
	~Compress_LZW15(void);

	void CompressFile(FILE *input,OutputBitStream *output);
	void ExpandFile(InputBitStream *input,FILE *output);
};
