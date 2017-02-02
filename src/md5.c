#include <stdlib.h>

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

// Per-iteration access of data in 512 bit block
static const uint k[4][16] = {
	{  0,  1,  2,  3,     4,  5,  6,  7,     8,  9, 10, 11,    12, 13, 14, 15 }, // i
	{  1,  6, 11,  0,     5, 10, 15,  4,     9, 14,  3,  8,    13,  2,  7, 12 }, // (5i + 1) % 16
	{  5,  8, 11, 14,     1,  4,  7, 10,    13,  0,  3,  6,     9, 12, 15,  2 }, // (3i + 5) % 16
	{  0,  7, 14,  5,    12,  3, 10,  1,     8, 15,  6, 13,     4, 11,  2,  9 }, // (7i) % 16
};

// Per-iteration circular left shift amounts
static const uint s[4][16] = {
	{  7, 12, 17, 22,     7, 12, 17, 22,     7, 12, 17, 22,     7, 12, 17, 22 },
	{  5,  9, 14, 20,     5,  9, 14, 20,     5,  9, 14, 20,     5,  9, 14, 20 },
	{  4, 11, 16, 23,     4, 11, 16, 23,     4, 11, 16, 23,     4, 11, 16, 23 },
	{  6, 10, 15, 21,     6, 10, 15, 21,     6, 10, 15, 21,     6, 10, 15, 21 },
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
// The compression unit 'g' of each round                                                                              |
//---------------------------------------------------------------------------------------------------------------------+
inline const uint F(const uint x, const uint y, const uint z) { return (x & y) | (~x & z); }
inline const uint G(const uint x, const uint y, const uint z) { return (x & z) | (y & ~z); }
inline const uint H(const uint x, const uint y, const uint z) { return x ^ y ^ z; }
inline const uint I(const uint x, const uint y, const uint z) { return y ^ (x | ~z); }

#define ITERATION_PARAMS \
	uint (*const g)(const uint, const uint, const uint), \
	uint* a, const uint* b, const uint* c, const uint* d, \
	const uint Xk, const uint Ti, const uint s
inline void iteration(ITERATION_PARAMS) {
	*a += g(*b, *c, *d) + Xk + Ti;
	*a = (*a << s) | (*a >> (32 - s)); // Circular left shift s
	*a += *b;
}

//---------------------------------------------------------------------------------------------------------------------+
// An unrolled version of the md5 compression function                                                                 |
//---------------------------------------------------------------------------------------------------------------------+
inline void md5_compress(uint* hash, const chunk* block) {
	uint* a = &hash[0];
	uint* b = &hash[1];
	uint* c = &hash[2];
	uint* d = &hash[3];

	// Final step adds CVq to output of the 64th iteration to obtain CVq+1
	const uint
		cva = *a,
		cvb = *b,
		cvc = *c,
		cvd = *d;

	// Round 1
	iteration(F,    a, b, c, d,    block->words[k[0][ 0]],    T[0][ 0],    s[0][ 0]);
	iteration(F,    d, a, b, c,    block->words[k[0][ 1]],    T[0][ 1],    s[0][ 1]);
	iteration(F,    c, d, a, b,    block->words[k[0][ 2]],    T[0][ 2],    s[0][ 2]);
	iteration(F,    b, c, d, a,    block->words[k[0][ 3]],    T[0][ 3],    s[0][ 3]);
	iteration(F,    a, b, c, d,    block->words[k[0][ 4]],    T[0][ 4],    s[0][ 4]);
	iteration(F,    d, a, b, c,    block->words[k[0][ 5]],    T[0][ 5],    s[0][ 5]);
	iteration(F,    c, d, a, b,    block->words[k[0][ 6]],    T[0][ 6],    s[0][ 6]);
	iteration(F,    b, c, d, a,    block->words[k[0][ 7]],    T[0][ 7],    s[0][ 7]);
	iteration(F,    a, b, c, d,    block->words[k[0][ 8]],    T[0][ 8],    s[0][ 8]);
	iteration(F,    d, a, b, c,    block->words[k[0][ 9]],    T[0][ 9],    s[0][ 9]);
	iteration(F,    c, d, a, b,    block->words[k[0][10]],    T[0][10],    s[0][10]);
	iteration(F,    b, c, d, a,    block->words[k[0][11]],    T[0][11],    s[0][11]);
	iteration(F,    a, b, c, d,    block->words[k[0][12]],    T[0][12],    s[0][12]);
	iteration(F,    d, a, b, c,    block->words[k[0][13]],    T[0][13],    s[0][13]);
	iteration(F,    c, d, a, b,    block->words[k[0][14]],    T[0][14],    s[0][14]);
	iteration(F,    b, c, d, a,    block->words[k[0][15]],    T[0][15],    s[0][15]);

	// Round 2
	iteration(G,    a, b, c, d,    block->words[k[1][ 0]],    T[1][ 0],    s[1][ 0]);
	iteration(G,    d, a, b, c,    block->words[k[1][ 1]],    T[1][ 1],    s[1][ 1]);
	iteration(G,    c, d, a, b,    block->words[k[1][ 2]],    T[1][ 2],    s[1][ 2]);
	iteration(G,    b, c, d, a,    block->words[k[1][ 3]],    T[1][ 3],    s[1][ 3]);
	iteration(G,    a, b, c, d,    block->words[k[1][ 4]],    T[1][ 4],    s[1][ 4]);
	iteration(G,    d, a, b, c,    block->words[k[1][ 5]],    T[1][ 5],    s[1][ 5]);
	iteration(G,    c, d, a, b,    block->words[k[1][ 6]],    T[1][ 6],    s[1][ 6]);
	iteration(G,    b, c, d, a,    block->words[k[1][ 7]],    T[1][ 7],    s[1][ 7]);
	iteration(G,    a, b, c, d,    block->words[k[1][ 8]],    T[1][ 8],    s[1][ 8]);
	iteration(G,    d, a, b, c,    block->words[k[1][ 9]],    T[1][ 9],    s[1][ 9]);
	iteration(G,    c, d, a, b,    block->words[k[1][10]],    T[1][10],    s[1][10]);
	iteration(G,    b, c, d, a,    block->words[k[1][11]],    T[1][11],    s[1][11]);
	iteration(G,    a, b, c, d,    block->words[k[1][12]],    T[1][12],    s[1][12]);
	iteration(G,    d, a, b, c,    block->words[k[1][13]],    T[1][13],    s[1][13]);
	iteration(G,    c, d, a, b,    block->words[k[1][14]],    T[1][14],    s[1][14]);
	iteration(G,    b, c, d, a,    block->words[k[1][15]],    T[1][15],    s[1][15]);

	// Round 3
	iteration(H,    a, b, c, d,    block->words[k[2][ 0]],    T[2][ 0],    s[2][ 0]);
	iteration(H,    d, a, b, c,    block->words[k[2][ 1]],    T[2][ 1],    s[2][ 1]);
	iteration(H,    c, d, a, b,    block->words[k[2][ 2]],    T[2][ 2],    s[2][ 2]);
	iteration(H,    b, c, d, a,    block->words[k[2][ 3]],    T[2][ 3],    s[2][ 3]);
	iteration(H,    a, b, c, d,    block->words[k[2][ 4]],    T[2][ 4],    s[2][ 4]);
	iteration(H,    d, a, b, c,    block->words[k[2][ 5]],    T[2][ 5],    s[2][ 5]);
	iteration(H,    c, d, a, b,    block->words[k[2][ 6]],    T[2][ 6],    s[2][ 6]);
	iteration(H,    b, c, d, a,    block->words[k[2][ 7]],    T[2][ 7],    s[2][ 7]);
	iteration(H,    a, b, c, d,    block->words[k[2][ 8]],    T[2][ 8],    s[2][ 8]);
	iteration(H,    d, a, b, c,    block->words[k[2][ 9]],    T[2][ 9],    s[2][ 9]);
	iteration(H,    c, d, a, b,    block->words[k[2][10]],    T[2][10],    s[2][10]);
	iteration(H,    b, c, d, a,    block->words[k[2][11]],    T[2][11],    s[2][11]);
	iteration(H,    a, b, c, d,    block->words[k[2][12]],    T[2][12],    s[2][12]);
	iteration(H,    d, a, b, c,    block->words[k[2][13]],    T[2][13],    s[2][13]);
	iteration(H,    c, d, a, b,    block->words[k[2][14]],    T[2][14],    s[2][14]);
	iteration(H,    b, c, d, a,    block->words[k[2][15]],    T[2][15],    s[2][15]);

	// Round 4
	iteration(I,    a, b, c, d,    block->words[k[3][ 0]],    T[3][ 0],    s[3][ 0]);
	iteration(I,    d, a, b, c,    block->words[k[3][ 1]],    T[3][ 1],    s[3][ 1]);
	iteration(I,    c, d, a, b,    block->words[k[3][ 2]],    T[3][ 2],    s[3][ 2]);
	iteration(I,    b, c, d, a,    block->words[k[3][ 3]],    T[3][ 3],    s[3][ 3]);
	iteration(I,    a, b, c, d,    block->words[k[3][ 4]],    T[3][ 4],    s[3][ 4]);
	iteration(I,    d, a, b, c,    block->words[k[3][ 5]],    T[3][ 5],    s[3][ 5]);
	iteration(I,    c, d, a, b,    block->words[k[3][ 6]],    T[3][ 6],    s[3][ 6]);
	iteration(I,    b, c, d, a,    block->words[k[3][ 7]],    T[3][ 7],    s[3][ 7]);
	iteration(I,    a, b, c, d,    block->words[k[3][ 8]],    T[3][ 8],    s[3][ 8]);
	iteration(I,    d, a, b, c,    block->words[k[3][ 9]],    T[3][ 9],    s[3][ 9]);
	iteration(I,    c, d, a, b,    block->words[k[3][10]],    T[3][10],    s[3][10]);
	iteration(I,    b, c, d, a,    block->words[k[3][11]],    T[3][11],    s[3][11]);
	iteration(I,    a, b, c, d,    block->words[k[3][12]],    T[3][12],    s[3][12]);
	iteration(I,    d, a, b, c,    block->words[k[3][13]],    T[3][13],    s[3][13]);
	iteration(I,    c, d, a, b,    block->words[k[3][14]],    T[3][14],    s[3][14]);
	iteration(I,    b, c, d, a,    block->words[k[3][15]],    T[3][15],    s[3][15]);

	*a += cva;
	*b += cvb;
	*c += cvc;
	*d += cvd;
}

static chunk* last_blocks;

//---------------------------------------------------------------------------------------------------------------------+
// This handles the case where the 1-bit of padding is the first bit of the nth chunk                                  |
// +---------------------------------------+   +---------------------------------------+                               |
// | message                               |   | 100000000000000000000000000000 | size |                               |
// +---------------------------------------+   +---------------------------------------+                               |
//---------------------------------------------------------------------------------------------------------------------+
inline const uint md5_setup_nfullpadding(chunk*** blocks, const byte* msg, const uint n) {
	const uint block_cnt = (n / 64) + 1;
	*blocks = malloc(block_cnt * sizeof(chunk*));
	last_blocks = calloc(1, sizeof(chunk));

	uint i = 0;
	for (uint i = 0; i < (block_cnt - 1); ++i) {
		(*blocks)[i] = msg + (i * 64);
	}

	last_blocks[0].bytes[0] = 0x80;
	last_blocks[0].words[14] = n * 8;

	(*blocks)[block_cnt - 1] = last_blocks;

	return block_cnt;
}

//---------------------------------------------------------------------------------------------------------------------+
// This handles the case where the 1-bit of padding is in the n-1 chunk                                                |
// +---------------------------------------+   +---------------------------------------+                               |
// | message                    | 10000000 |   | 000000000000000000000000000000 | size |                               |
// +---------------------------------------+   +---------------------------------------+                               |
//---------------------------------------------------------------------------------------------------------------------+
inline const uint md5_setup_nm1padding(chunk*** blocks, const byte* msg, const uint n) {
	const uint block_cnt = (n / 64) + 2;
	*blocks = malloc(block_cnt * sizeof(chunk*));
	last_blocks = calloc(2, sizeof(chunk));

	uint i = 0;
	for (uint i = 0; i < (block_cnt - 2); ++i) {
		(*blocks)[i] = msg + (i * 64);
	}

	const uint msg_m64 = n % 64;

	chunk* nm1 = &last_blocks[0];
	memcpy(nm1, msg + ((block_cnt - 2) * 64), msg_m64);
	nm1->bytes[msg_m64] = 0x80;

	chunk* last = &last_blocks[1];
	last->words[14] = n * 8;

	(*blocks)[block_cnt - 2] = nm1;
	(*blocks)[block_cnt - 1] = last;

	return block_cnt;
}

//---------------------------------------------------------------------------------------------------------------------+
// This handles the case where part of the message is in the nth chunk                                                 |
// +---------------------------------------+                                                                           |
// | message    | 10000000000000000 | size |                                                                           |
// +---------------------------------------+                                                                           |
//---------------------------------------------------------------------------------------------------------------------+
inline const uint md5_setup_npartpadding(chunk*** blocks, const byte* msg, const  uint n) {
	const uint block_cnt = (n / 64) + 1;
	*blocks = malloc(block_cnt * sizeof(chunk*));
	last_blocks = calloc(1, sizeof(chunk));

	uint i = 0;
	for (uint i = 0; i < block_cnt - 1; ++i) {
		(*blocks)[i] = msg + (i * 64);
	}

	const uint msg_m64 = n % 64;

	memcpy(last_blocks, msg + ((block_cnt - 1) * 64), msg_m64);
	last_blocks[0].bytes[msg_m64] = 0x80;
	last_blocks[0].words[14] = n * 8;

	(*blocks)[block_cnt - 1] = last_blocks;

	return block_cnt;
}

//---------------------------------------------------------------------------------------------------------------------+
// Primary md5 function: returns the 128-bit hash (dynamically allocated) of "msg" with "n" bytes                      |
//---------------------------------------------------------------------------------------------------------------------+
const uint* md5(const byte* msg, const uint n) {
	uint block_cnt;
	chunk** blocks;
	uint* hash = malloc(4 * sizeof(uint));
	memcpy(hash, IV, 4 * sizeof(uint));

	// Compartmentalize into 512 bit (64 byte) chunks (including the message length at the end)
	const uint msg_m64 = n % 64;
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

	free(last_blocks);
	free(blocks);

	return hash;
}
