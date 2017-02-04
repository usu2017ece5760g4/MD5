//---------------------------------------------------------------------------------------------------------------------+
// MD5 Hashing Algorithm                                                                                               |
// USU ECE 5760 Project 1                                                                                              |
// Braydn Clark A01091991                                                                                              |
// Joshua Lynn A01799554                                                                                               |
// John Call A01283897                                                                                                 |
//---------------------------------------------------------------------------------------------------------------------+
#include <stdlib.h>
#include <stdio.h>
#include "benchtime.h"

typedef unsigned char byte;
typedef unsigned int uint;

extern void md5_attack(const uint* hash, const uint n);;

void print_hash(uint* hash) {
	union {
		uint raw[4];
		union {
			uint word;
			byte byte[4];
		} words[4];
	} *hout = hash;

	for (uint i = 0; i < 4; ++i) {
		printf("%02x", hout->words[i].byte[0]);
		printf("%02x", hout->words[i].byte[1]);
		printf("%02x", hout->words[i].byte[2]);
		printf("%02x", hout->words[i].byte[3]);
	}
}

int main(int argc, char **argv) {
	const int N = 6;

	// Ensure all hashes are searched
	uint hash[4] = { 0 };

	uint64 before = GetTimeMs64();
	md5_attack(hash, N);
	uint64 after = GetTimeMs64();

	printf("Program ran in %u ms\n", after - before);

	return 0;
}
