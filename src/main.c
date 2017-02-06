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

#define N 6

int main(int argc, char **argv) {
	// Ensure all hashes are searched
	uint hash[4] = { 0xd2413e45, 0xcc71e018, 0x991c2dfb, 0x6a9023ce }; // zzzzzz // { 0 };
	byte preimage[N + 1] = { 0 };

	uint64 before = GetTimeMs64();
	int result = md5_attack(preimage, hash, N);
	uint64 after = GetTimeMs64();

	printf("Program ran in %llu ms\n", after - before);
	if (result == -1) {
		printf("Pre-image not found for ");
		print_md5(hash);
		printf("\n");
	}
	else {
		printf("The preimage of ");
		print_md5(hash);
		printf(" is \"%s\"\n", preimage);
	}

	system("pause");
	return 0;
}
