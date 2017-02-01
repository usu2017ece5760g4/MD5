//---------------------------------------------------------------------------------------------------------------------+
// MD5 Hashing Algorithm                                                                                               |
// USU ECE 5760 Project 1                                                                                              |
// Braydn Clark A01091991                                                                                              |
// Joshua Lynn A01799554                                                                                               |
// John Call A01283897                                                                                                 |
//---------------------------------------------------------------------------------------------------------------------+
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
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

void temp() {
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
}
/*
inline void hash_n(uint n) {
	size = loop_alpha(n, &list);
	int i;
	for (i = 0; i < size; ++i) {
		uint* hash; // size 4
	//	hash = md5(list[i], n);
		printf("list[i] hashes to %02x\n", hash);
	}
	// need to free(list[i])
	free(list);
}

inline void hash_f(uint n) {
	size = loop_alpha(n, &list);
	int i;
	for (i = 0; i < size; ++i) {
		uint* hash; // size 4
		//hash = md5(list[i], n);
		if (hash == t) {
			printf("%s hashes to %s\n", list[i], t);
			break; // this is not right and we need to convert the string to a 128 bit int
		}
	}
	// need to free(list[i])
	free(list);
}
*/
int main(int argc, char **argv) {
	temp();
	return 0;
	/*
	extern char *optarg; // these are for getopt.
	extern int optind;

// getopt is awesome but it took a while to figure out how to use it
// this site helped me out https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/getopt.html
// i put that here more for my own reference because I'm sure ill use getopt again.
	int verbose = 0, sflag = 0, bflag = 0, tflag = 0;
	char *t, *s;
	char c;
	//this is just for taking in command line arguments.
		while ((c = getopt(argc, argv, "hvbs:t:")) != -1) {
			switch (c) {
				case 'h':
					// print usage info here
					printf("Usage: ./alpha [-hvb] -s <string> -t <string>\n"
						   " Options:\n  -h           Print this help message.\n"
						   "  -v           Optional verbose flag.\n"
						   "  -b           Run benchmark hashing all strings aaaaaa - zzzzzz\n"
						   "  -s <string>  Run md5 hash on single string.\n"
						   "  -t <string>  Brute force md5 hash and compare to this string.\n"
						   "\n"
						   "Examples:\n"
						   "  linux>  ./alpha -s helloworld\n"
						   "  linux>  ./alpha -t 6f5902ac237024bdd0c176cb93063dc4\n"
						   "");
					exit(1);
					break;

				case 'v':  // verbose mode does nothing at the moment
					verbose = 1;
					break;

				case 's':  // run a single md5 hash on given string
					sflag = sflag + 1;
					s = optarg;
					break;

				case 'b': // benchmark all possible 6 character pw
					bflag = bflag + 1;
					break;

				case 't': //find target hash
					tflag = tflag + 1;
					t = optarg;
					break;

				case '?':
					// print something about not having the right stuffs
					exit(1);
					break;
			}
		}

	if (sflag == 0 && bflag == 0 && tflag == 0) {
		printf("you didn't have all the needed command-line arguments\n");
		exit(1);
	}
	else if (sflag + bflag + tflag > 1) {
		printf("you have too many command-line arguments\n");
	}
	else if (sflag > 1 || bflag > 1 || tflag > 1) {
		printf("you used some arguments more than once\n");
		exit(1);
	};



	printf("worked so far\n");


	if (sflag == 1) {
		uint hash;
		md5(s, sizeof(s), hash);
		printf("%02x\n", hash);
	}
	else if (bflag == 1) {
		hash_n(6);
	}
	else if (tflag == 1) {
		hash_f(1);
		hash_f(2);
		hash_f(3);
		hash_f(4);
		hash_f(5);
		hash_f(6);
		hash_f(7);
		hash_f(8);
		hash_f(9);
		hash_f(10);
	}

	//uint x = str2hex(t);
	*/
	return 0;
}
