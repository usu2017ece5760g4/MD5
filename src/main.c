//---------------------------------------------------------------------------------------------------------------------+
// MD5 Hashing Algorithm                                                                                               |
// USU ECE 5760 Project 1                                                                                              |
// Braydn Clark A01091991                                                                                              |
// Joshua Lynn A01799554                                                                                               |
// John Call A01283897                                                                                                 |
//---------------------------------------------------------------------------------------------------------------------+
#include <stdlib.h>
#include <stdio.h>
#include "strgen.h"

extern const uint* md5(const byte* msg, const uint n);

typedef union {
	uint raw[4];
	union {
		uint word;
		byte byte[4];
	} words[4];
} hash;

byte* list;
uint size;

int main(int argc, char **argv) {
	const int N = 5;
	size = loop_alpha(N, &list);

	for (uint i = 0; i < size; ++i) {
		printf("The hash of %.*s is: ", N, (list + (i * N)));

		hash* hash = md5((list + (i * N)), N);
		for (uint i = 0; i < 4; ++i) {
			printf("%02x", hash->words[i].byte[0]);
			printf("%02x", hash->words[i].byte[1]);
			printf("%02x", hash->words[i].byte[2]);
			printf("%02x", hash->words[i].byte[3]);
		}
		printf("\n");
	}

	free(list);
	size = 0;

	return 0;
}
