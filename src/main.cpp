//---------------------------------------------------------------------------------------------------------------------+
// MD5 Hashing Algorithm                                                                                               |
// USU ECE 5760 Project 1                                                                                              |
// Braydn Clark A01091991                                                                                              |
// Joshua Lynn A01799554                                                                                               |
// John Call A01283897                                                                                                 |
//---------------------------------------------------------------------------------------------------------------------+
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include "benchtime.h"

int main(int argc, char **argv) {
	const int N = 6;

	// Ensure all hashes are searched
	uint hash[4] = { 0 };

	uint threads = ATTACK_STOP - ATTACK_START;
	std::vector<std::thread*> pool(threads);

	uint i = 0;

	// Begin timed routine
	uint64 before = GetTimeMs64();

	// Start up each thread
	while (i < threads) {
		pool[i] = new std::thread(md5_attack, hash, N, ATTACK_START + i);
		++i;
	}

	// Wait on each thread
	i = 0;
	while (i < threads) {
		pool[i]->join();
		delete pool[i];
		pool[i++] = nullptr;
	}

	// End timed routine
	uint64 after = GetTimeMs64();

	printf("Program ran in %u ms\n", after - before);

	return 0;
}
