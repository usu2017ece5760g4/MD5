#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include "benchtime.h"

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
inline const __m256i F(const __m256i x, const __m256i y, const __m256i z) {
	return _mm256_or_si256(_mm256_and_si256(x, y), _mm256_andnot_si256(x, z));
}
inline const __m256i G(const __m256i x, const __m256i y, const __m256i z) {
	return _mm256_or_si256(_mm256_and_si256(x, z), _mm256_andnot_si256(z, y));
}
inline const __m256i H(const __m256i x, const __m256i y, const __m256i z) {
	return _mm256_xor_si256(_mm256_xor_si256(x, y), z);
}
inline const __m256i I(const __m256i x, const __m256i y, const __m256i z) {
	return _mm256_xor_si256(y, _mm256_or_si256(x, _mm256_andnot_si256(z, _mm256_set1_epi32(0xffffffff))));
}

#define ITERATION_PARAMS \
	const __m256i (*const g)(const __m256i, const __m256i, const __m256i), \
	__m256i* a, const __m256i* b, const __m256i* c, const __m256i* d, \
	const __m256i Xk, const uint Ti, const uint s
inline void iteration(ITERATION_PARAMS) {
	*a = _mm256_add_epi32(_mm256_add_epi32(*a, g(*b, *c, *d)), _mm256_add_epi32(Xk, _mm256_set1_epi32(Ti)));
	*a = _mm256_or_si256(_mm256_slli_epi32(*a, s), _mm256_srli_epi32(*a, (32 - s))); // Circular left shift s
	*a = _mm256_add_epi32(*a, *b);
}

//---------------------------------------------------------------------------------------------------------------------+
// An unrolled version of the md5 compression function                                                                 |
//---------------------------------------------------------------------------------------------------------------------+
inline void md5_compress(__m256i* hash, const __m256i* block) {
	__m256i* a = &hash[0];
	__m256i* b = &hash[1];
	__m256i* c = &hash[2];
	__m256i* d = &hash[3];

	// Final step adds CVq to output of the 64th iteration to obtain CVq+1
	const __m256i
		cva = *a,
		cvb = *b,
		cvc = *c,
		cvd = *d;

	// Round 1
	iteration(F,    a, b, c, d,    block[k[0][ 0]],    T[0][ 0],    s[0][ 0]);
	iteration(F,    d, a, b, c,    block[k[0][ 1]],    T[0][ 1],    s[0][ 1]);
	iteration(F,    c, d, a, b,    block[k[0][ 2]],    T[0][ 2],    s[0][ 2]);
	iteration(F,    b, c, d, a,    block[k[0][ 3]],    T[0][ 3],    s[0][ 3]);
	iteration(F,    a, b, c, d,    block[k[0][ 4]],    T[0][ 4],    s[0][ 4]);
	iteration(F,    d, a, b, c,    block[k[0][ 5]],    T[0][ 5],    s[0][ 5]);
	iteration(F,    c, d, a, b,    block[k[0][ 6]],    T[0][ 6],    s[0][ 6]);
	iteration(F,    b, c, d, a,    block[k[0][ 7]],    T[0][ 7],    s[0][ 7]);
	iteration(F,    a, b, c, d,    block[k[0][ 8]],    T[0][ 8],    s[0][ 8]);
	iteration(F,    d, a, b, c,    block[k[0][ 9]],    T[0][ 9],    s[0][ 9]);
	iteration(F,    c, d, a, b,    block[k[0][10]],    T[0][10],    s[0][10]);
	iteration(F,    b, c, d, a,    block[k[0][11]],    T[0][11],    s[0][11]);
	iteration(F,    a, b, c, d,    block[k[0][12]],    T[0][12],    s[0][12]);
	iteration(F,    d, a, b, c,    block[k[0][13]],    T[0][13],    s[0][13]);
	iteration(F,    c, d, a, b,    block[k[0][14]],    T[0][14],    s[0][14]);
	iteration(F,    b, c, d, a,    block[k[0][15]],    T[0][15],    s[0][15]);

	// Round 2
	iteration(G,    a, b, c, d,    block[k[1][ 0]],    T[1][ 0],    s[1][ 0]);
	iteration(G,    d, a, b, c,    block[k[1][ 1]],    T[1][ 1],    s[1][ 1]);
	iteration(G,    c, d, a, b,    block[k[1][ 2]],    T[1][ 2],    s[1][ 2]);
	iteration(G,    b, c, d, a,    block[k[1][ 3]],    T[1][ 3],    s[1][ 3]);
	iteration(G,    a, b, c, d,    block[k[1][ 4]],    T[1][ 4],    s[1][ 4]);
	iteration(G,    d, a, b, c,    block[k[1][ 5]],    T[1][ 5],    s[1][ 5]);
	iteration(G,    c, d, a, b,    block[k[1][ 6]],    T[1][ 6],    s[1][ 6]);
	iteration(G,    b, c, d, a,    block[k[1][ 7]],    T[1][ 7],    s[1][ 7]);
	iteration(G,    a, b, c, d,    block[k[1][ 8]],    T[1][ 8],    s[1][ 8]);
	iteration(G,    d, a, b, c,    block[k[1][ 9]],    T[1][ 9],    s[1][ 9]);
	iteration(G,    c, d, a, b,    block[k[1][10]],    T[1][10],    s[1][10]);
	iteration(G,    b, c, d, a,    block[k[1][11]],    T[1][11],    s[1][11]);
	iteration(G,    a, b, c, d,    block[k[1][12]],    T[1][12],    s[1][12]);
	iteration(G,    d, a, b, c,    block[k[1][13]],    T[1][13],    s[1][13]);
	iteration(G,    c, d, a, b,    block[k[1][14]],    T[1][14],    s[1][14]);
	iteration(G,    b, c, d, a,    block[k[1][15]],    T[1][15],    s[1][15]);

	// Round 3
	iteration(H,    a, b, c, d,    block[k[2][ 0]],    T[2][ 0],    s[2][ 0]);
	iteration(H,    d, a, b, c,    block[k[2][ 1]],    T[2][ 1],    s[2][ 1]);
	iteration(H,    c, d, a, b,    block[k[2][ 2]],    T[2][ 2],    s[2][ 2]);
	iteration(H,    b, c, d, a,    block[k[2][ 3]],    T[2][ 3],    s[2][ 3]);
	iteration(H,    a, b, c, d,    block[k[2][ 4]],    T[2][ 4],    s[2][ 4]);
	iteration(H,    d, a, b, c,    block[k[2][ 5]],    T[2][ 5],    s[2][ 5]);
	iteration(H,    c, d, a, b,    block[k[2][ 6]],    T[2][ 6],    s[2][ 6]);
	iteration(H,    b, c, d, a,    block[k[2][ 7]],    T[2][ 7],    s[2][ 7]);
	iteration(H,    a, b, c, d,    block[k[2][ 8]],    T[2][ 8],    s[2][ 8]);
	iteration(H,    d, a, b, c,    block[k[2][ 9]],    T[2][ 9],    s[2][ 9]);
	iteration(H,    c, d, a, b,    block[k[2][10]],    T[2][10],    s[2][10]);
	iteration(H,    b, c, d, a,    block[k[2][11]],    T[2][11],    s[2][11]);
	iteration(H,    a, b, c, d,    block[k[2][12]],    T[2][12],    s[2][12]);
	iteration(H,    d, a, b, c,    block[k[2][13]],    T[2][13],    s[2][13]);
	iteration(H,    c, d, a, b,    block[k[2][14]],    T[2][14],    s[2][14]);
	iteration(H,    b, c, d, a,    block[k[2][15]],    T[2][15],    s[2][15]);

	// Round 4
	iteration(I,    a, b, c, d,    block[k[3][ 0]],    T[3][ 0],    s[3][ 0]);
	iteration(I,    d, a, b, c,    block[k[3][ 1]],    T[3][ 1],    s[3][ 1]);
	iteration(I,    c, d, a, b,    block[k[3][ 2]],    T[3][ 2],    s[3][ 2]);
	iteration(I,    b, c, d, a,    block[k[3][ 3]],    T[3][ 3],    s[3][ 3]);
	iteration(I,    a, b, c, d,    block[k[3][ 4]],    T[3][ 4],    s[3][ 4]);
	iteration(I,    d, a, b, c,    block[k[3][ 5]],    T[3][ 5],    s[3][ 5]);
	iteration(I,    c, d, a, b,    block[k[3][ 6]],    T[3][ 6],    s[3][ 6]);
	iteration(I,    b, c, d, a,    block[k[3][ 7]],    T[3][ 7],    s[3][ 7]);
	iteration(I,    a, b, c, d,    block[k[3][ 8]],    T[3][ 8],    s[3][ 8]);
	iteration(I,    d, a, b, c,    block[k[3][ 9]],    T[3][ 9],    s[3][ 9]);
	iteration(I,    c, d, a, b,    block[k[3][10]],    T[3][10],    s[3][10]);
	iteration(I,    b, c, d, a,    block[k[3][11]],    T[3][11],    s[3][11]);
	iteration(I,    a, b, c, d,    block[k[3][12]],    T[3][12],    s[3][12]);
	iteration(I,    d, a, b, c,    block[k[3][13]],    T[3][13],    s[3][13]);
	iteration(I,    c, d, a, b,    block[k[3][14]],    T[3][14],    s[3][14]);
	iteration(I,    b, c, d, a,    block[k[3][15]],    T[3][15],    s[3][15]);

	*a = _mm256_add_epi32(*a, cva);
	*b = _mm256_add_epi32(*b, cvb);
	*c = _mm256_add_epi32(*c, cvc);
	*d = _mm256_add_epi32(*d, cvd);
}

//---------------------------------------------------------------------------------------------------------------------+
// Hashes 8 pre-images at a time using AVX2 extensions and compare with the given hash 'needle'                        |
//---------------------------------------------------------------------------------------------------------------------+
inline const uint md5_hash(__m256i* needle, __m256i* block) {
	__m256i hash[4] = {
		_mm256_set1_epi32(IV[0]),
		_mm256_set1_epi32(IV[1]),
		_mm256_set1_epi32(IV[2]),
		_mm256_set1_epi32(IV[3]),
	};

	md5_compress(hash, block);

	// Compare hash with needle
	// return -1 if no match
	// return index (0 < i < 8) if match
	return -1;
}

//---------------------------------------------------------------------------------------------------------------------+
// It can't be helped, the setup in this function is a little confusing                                                |
// This function assumes password length less than 53 characters for simplicity                                        |
// Sets up batches of 8 pre-images to be hashed as well as a needle to test for equality                               |
//---------------------------------------------------------------------------------------------------------------------+
void md5_attack(const uint* hash, const uint n, const char msb) {
	// needle allows us to compare all 8 hashes to our target at once
	__m256i needle[4] = {
		_mm256_set1_epi32(hash[0]),
		_mm256_set1_epi32(hash[1]),
		_mm256_set1_epi32(hash[2]),
		_mm256_set1_epi32(hash[3])
	};

	// The format of a single 512 bit block
	chunk layout = { 0 };
	layout.bytes[0] = msb; // Thread scans only one msb
	for (uint i = 1; i < n; ++i) {
		layout.bytes[i] = ATTACK_START;
	}
	layout.bytes[n] = 0x80;
	layout.words[14] = n * 8;

	// A vector of 8 512-bit blocks (copied from layout)
	__m256i preimages[16] = {
		_mm256_set1_epi32(layout.words[0]),
		_mm256_set1_epi32(layout.words[1]),
		_mm256_set1_epi32(layout.words[2]),
		_mm256_set1_epi32(layout.words[3]),
		_mm256_set1_epi32(layout.words[4]),
		_mm256_set1_epi32(layout.words[5]),
		_mm256_set1_epi32(layout.words[6]),
		_mm256_set1_epi32(layout.words[7]),
		_mm256_set1_epi32(layout.words[8]),
		_mm256_set1_epi32(layout.words[9]),
		_mm256_set1_epi32(layout.words[10]),
		_mm256_set1_epi32(layout.words[11]),
		_mm256_set1_epi32(layout.words[12]),
		_mm256_set1_epi32(layout.words[13]),
		_mm256_set1_epi32(layout.words[14]),
		_mm256_set1_epi32(layout.words[15])
	};

	// Get all hashes incrementally different from each other
	int digit = n - 1;
	for (uint i = 1; i < 8; ++i) {
		byte* const letter = (byte*)(&(preimages[digit / 4].m256i_u32[i])) + digit % 4;
		(*letter) += i;
	}

	// At this point we have our first 8 hashes, so do a hash before entering the loop
	if (md5_hash(needle, preimages) != -1) {
		return;
	}

	// Each run through the loop will iterate over one of the 8 hashes in the pre-images buffer
	uint i = 0;

	// Increment by 8 in the least significant digit and by 1 in more significant digits in the case of overflow
	uint increment = 8;

	// An in-place loop to check all the hashes for a password of length n (within our range)
	while (1) {
		byte* const letter = (byte*)(&(preimages[digit / 4].m256i_u32[i])) + digit % 4;
		(*letter) += increment;

		if ((*letter) > ATTACK_STOP) {
			increment = 1; // Increment by 1 when handling overflow
			(*letter) = ATTACK_START;
			if (--digit < 1) { // Thread stops at msb + 1
				break;
			}
			continue;
		}
		else if (digit < n - 1) {
			digit = n - 1;
			increment = 8; // Finished with overflow
		}

		if (++i > 7) {
			i = 0;
			if (md5_hash(needle, preimages) != -1) {
				return;
			}
		}
	}

	// There may still be a couple outliers
	if (i > 0) {
		if (md5_hash(needle, preimages) != -1) {
			return;
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------+
// Prints out an md5 hash in big endian order given a little endian input                                              |
//---------------------------------------------------------------------------------------------------------------------+
void print_md5(const uint* hash) {
	union {
		uint raw[4];
		union {
			uint word;
			byte byte[4];
		} words[4];
	} const *hout = hash;

	for (uint i = 0; i < 4; ++i) {
		printf("%02x", hout->words[i].byte[0]);
		printf("%02x", hout->words[i].byte[1]);
		printf("%02x", hout->words[i].byte[2]);
		printf("%02x", hout->words[i].byte[3]);
	}
}
