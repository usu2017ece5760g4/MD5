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
