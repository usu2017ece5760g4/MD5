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

typedef unsigned int uint;

unsigned int power(unsigned int a, unsigned int b) {
	unsigned int out = 1;
	unsigned int square = a*a;
	while (b >= 2) {
		out *= square;
		b -= 2;
	}
	if (b > 1) {
		out *= a;
	}
	return out;
}

unsigned int loop_alpha(unsigned int n, unsigned char** out) {
	// Compute the size we need for out: N = 26 ^ n
	unsigned int size;
	size = power(26, n);

	// Create a list of size N
	unsigned char** list = malloc(sizeof(char*) * size);
	for (uint i = 0; i < n; i++) {
		list[i] = malloc(sizeof(char) * n);
	}

	char* str = malloc(sizeof(char) * n);

	int i;
	for (i = 0; i < n; i++) {  // sets initial value to all a 
		str[i] = 'a';
	}

	list[0] = str;
	//increment at digit
	// if > 'z' set digit = 'a' and increment digit - 1	
	// digit < 0 break
	unsigned int digit = n - 1;

	i = 0;
	memcpy(list[i++], str, n);
	while (i < size) {
		++str[digit];
		if (str[digit] > 'z') {
			str[digit] = 'a';
			if (--digit < 0) {
				break;
			}
			continue;
		}
		else if (digit < n - 1) {
			digit = n - 1;
		}

		memcpy(list[i++], str, n);
	}

	// Give caller access to the list
	out = list;
	return size;
}

unsigned char* list;
unsigned int size;
inline void hash_n(unsigned int n) {
	size = loop_alpha(n, &list);
	int i;
	for (i = 0; i < size; ++i) {
		unsigned int* hash; // size 4
	//	hash = md5(list[i], n);
		printf("list[i] hashes to %02x\n", hash);
	}
	free(list);
}

inline void hash_f(unsigned int n) {
	size = loop_alpha(n, &list);
	int i;
	for (i = 0; i < size; ++i) {
		unsigned int* hash; // size 4
		//hash = md5(list[i], n);
		if (hash == t) {
			printf("%s hashes to %s\n", list[i], t);
			break; // this is not right and we need to convert the string to a 128 bit int					
		}
	}
	free(list);
}

void main(int argc, char **argv) {
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

				case 'b': // benchmark all posible 6 character pw
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
		unsigned int hash;
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

}
