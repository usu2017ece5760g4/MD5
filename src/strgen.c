#include "strgen.h"

const uint power(const uint a, uint b) {
	uint out = 1;
	uint square = a*a;
	while (b > 1) {
		out *= square;
		b -= 2;
	}
	if (b > 0) {
		out *= a;
	}
	return out;
}

const uint loop_alpha(const uint n, byte** out) {
	// Compute the size we need for out: N = 26 ^ n
	uint size = power(26, n);

	// Create a list of size N
	byte* list = malloc(sizeof(char*) * size * n);
	byte* str = (byte*)malloc(sizeof(char) * n);

	uint i;
	for (i = 0; i < n; i++) {  // sets initial value to all a
		str[i] = 'a';
	}

	//increment at digit
	// if > 'z' set digit = 'a' and increment digit - 1
	// digit < 0 break
	uint digit = n - 1;

	i = 0;
	memcpy((list + i * n)[i++], str, n);
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

		memcpy((list + i * n)[i++], str, n);
	}

	// Give caller access to the list
	*out = list;
	return size;
}
