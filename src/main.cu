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
#include "benchtime.h"

int main(int argc, char **argv) {
	const int N = 5;

	byte* list;
	uint size = loop_alpha(N, &list);

	for (uint i = 0; i < size; ++i) {
		printf("The hash of %.*s is: ", N, (list + (i * N)));

		const uint* hash = md5((list + (i * N)), N);
		print_md5(hash);
		printf("\n");
	}

	free(list);
	size = 0;

	return 0;
}
