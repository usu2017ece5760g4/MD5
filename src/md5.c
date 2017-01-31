#include <stdlib.h>
#include <cstdint>

typedef unsigned char byte;
typedef unsigned int uint;

typedef union {
	byte bytes[64];
	uint words[16];
} chunk;

//---------------------------------------------------------------------------------------------------------------------+
// <Precompiled constants used in the md5 hashing algorithm>                                                           |
//---------------------------------------------------------------------------------------------------------------------+
// Note: Precomputed values borrowed from https://en.wikipedia.org/wiki/MD5#Pseudocode
// Note: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating
// Note: Static linkage as these values are not useful outside of the md5 algorithm
static const uint IV[4] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 };

// Per-round shift amounts
static const uint s[4][16] = {
	{ 7, 12, 17, 22,    7, 12, 17, 22,    7, 12, 17, 22,    7, 12, 17, 22 },
	{ 5,  9, 14, 20,    5,  9, 14, 20,    5,  9, 14, 20,    5,  9, 14, 20 },
	{ 4, 11, 16, 23,    4, 11, 16, 23,    4, 11, 16, 23,    4, 11, 16, 23 },
	{ 6, 10, 15, 21,    6, 10, 15, 21,    6, 10, 15, 21,    6, 10, 15, 21 },
};

// Binary integer part of the sines of integers (Radians) as constants:
static const uint T[4][16] = {
	{ 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	  0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	  0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	  0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 },

	{ 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	  0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	  0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	  0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a },

	{ 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	  0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	  0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	  0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 },

	{ 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	  0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	  0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	  0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 },
};
//---------------------------------------------------------------------------------------------------------------------+
// </Precompiled constants used in the md5 hashing algorithm>                                                          |
//---------------------------------------------------------------------------------------------------------------------+

//---------------------------------------------------------------------------------------------------------------------+
// An unrolled version of the md5 compression function                                                                 |
//---------------------------------------------------------------------------------------------------------------------+
inline void md5_compress(uint* hash, chunk* block) {
	
}

//---------------------------------------------------------------------------------------------------------------------+
// This handles the case where the 1-bit of padding is the first bit of the nth chunk                                  |
// +---------------------------------------+   +---------------------------------------+                               |
// | message                               |   | 100000000000000000000000000000 | size |                               |
// +---------------------------------------+   +---------------------------------------+                               |
//---------------------------------------------------------------------------------------------------------------------+
inline uint md5_setup_nfullpadding(chunk*** blocks, byte* msg, uint n) {
	uint block_cnt = (n / 64) + 1;
	*blocks = malloc(block_cnt * sizeof(chunk*));

	uint i = 0;
	for (uint i = 0; i < (block_cnt - 1); ++i) {
		*blocks[i] = msg + (i * 64);
	}

	chunk last = {0};
	last.bytes[0] = 0x80;
	last.words[14] = n * 8;

	*blocks[block_cnt - 1] = &last;

	return block_cnt;
}

//---------------------------------------------------------------------------------------------------------------------+
// This handles the case where the 1-bit of padding is in the n-1 chunk                                                |
// +---------------------------------------+   +---------------------------------------+                               |
// | message                    | 10000000 |   | 000000000000000000000000000000 | size |                               |
// +---------------------------------------+   +---------------------------------------+                               |
//---------------------------------------------------------------------------------------------------------------------+
inline uint md5_setup_nm1padding(chunk*** blocks, byte* msg, uint n) {
	uint block_cnt = (n / 64) + 2;
	*blocks = malloc(block_cnt * sizeof(chunk*));

	uint i = 0;
	for (uint i = 0; i < (block_cnt - 2); ++i) {
		*blocks[i] = msg + (i * 64);
	}

	uint msg_m64 = n % 64;

	chunk nm1 = {0};
	memcpy(&nm1, msg + ((block_cnt - 2) * 64), msg_m64);

	chunk last = {0};
	last.bytes[0] = 0x80;
	last.words[14] = n * 8;

	*blocks[block_cnt - 2] = &nm1;
	*blocks[block_cnt - 1] = &last;

	return block_cnt;
}

//---------------------------------------------------------------------------------------------------------------------+
// This handles the case where part of the message is in the nth chunk                                                 |
// +---------------------------------------+                                                                           |
// | message    | 10000000000000000 | size |                                                                           |
// +---------------------------------------+                                                                           |
//---------------------------------------------------------------------------------------------------------------------+
inline uint md5_setup_npartpadding(chunk*** blocks, byte* msg, uint n) {
	uint block_cnt = (n / 64) + 1;
	*blocks = malloc(block_cnt * sizeof(chunk*));

	uint i = 0;
	for (uint i = 0; i < block_cnt - 1; ++i) {
		*blocks[i] = msg + (i * 64);
	}

	uint msg_m64 = n % 64;

	chunk last = {0};
	memcpy(&last, msg + ((block_cnt - 1) * 64), msg_m64);
	last.bytes[msg_m64] = 0x80;
	last.words[14] = n * 8;

	*blocks[block_cnt - 1] = &last;

	return block_cnt;
}

//---------------------------------------------------------------------------------------------------------------------+
// Primary md5 function: returns the 128-bit hash (dynamically allocated) of "msg" with "n" bytes                      |
//---------------------------------------------------------------------------------------------------------------------+
uint* md5(byte* msg, uint n) {
	uint block_cnt;
	chunk** blocks;
	uint* hash = malloc(4 * sizeof(uint));
	memcpy(hash, IV, 4);

	// Compartmentalize into 512 bit (64 byte) chunks (including the message length at the end)
	uint msg_m64 = n % 64;
	if (msg_m64 == 0) {
		block_cnt = md5_setup_nfullpadding(&blocks, msg, n);
	}
	else if (msg_m64 > 55) {
		block_cnt = md5_setup_nm1padding  (&blocks, msg, n);
	}
	else {
		block_cnt = md5_setup_npartpadding(&blocks, msg, n);
	}

	// Run the compression function each block (modifies hash in-place giving the iterative effect)
	for (uint i = 0; i < block_cnt; ++i) {
		md5_compress(hash, blocks[i]);
	}

	free(blocks);

	return hash;
}
