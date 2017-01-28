#include "strgen.h"

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

unsigned int loop_alpha(unsigned int n, byte*** out) {
	// Compute the size we need for out: N = 26 ^ n
	unsigned int size;
	size = power(26, n);

	// Create a list of size N
	byte** list = malloc(sizeof(char*) * size);
	for (uint i = 0; i < size; i++) {
		list[i] = malloc(sizeof(char) * (n + 1)); // +1 for \0 (easier debugging)
	}

	byte* str = malloc(sizeof(char) * (n + 1)); // +1 for \0 (easier debugging)

	int i;
	for (i = 0; i < n; i++) {  // sets initial value to all a
		str[i] = 'a';
	}
	str[n] = 0; // null terminator for easier debugging

	list[0] = str;
	//increment at digit
	// if > 'z' set digit = 'a' and increment digit - 1
	// digit < 0 break
	unsigned int digit = n - 1;

	i = 0;
	memcpy(list[i++], str, n + 1); // +1 for \0 (easier debugging)
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

		memcpy(list[i++], str, n + 1); // +1 for \0 (easier debugging)
	}

	// Give caller access to the list
	*out = list;
	return size;
}
