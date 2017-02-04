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

#define THREADS (ATTACK_STOP - ATTACK_START) * (ATTACK_STOP - ATTACK_START)

int main(int argc, char **argv) {
	const int N = 6;

	// Ensure the entire range is searched
	uint hash[4] = { 0 };


	md5_attack<<<1, THREADS >>>(hash, N);

	return 0;
}
