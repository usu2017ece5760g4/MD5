#pragma once

// Used lowercase alpha for benchmarking
#define ATTACK_START 'a'
#define ATTACK_STOP  'z'

typedef unsigned char byte;
typedef unsigned int uint;

#ifdef __cplusplus
extern "C" {
#endif

extern void md5_attack(volatile int* result, byte* preimage, const uint* hash, const uint n, const char msb);
extern void print_md5(const uint* hash);

#ifdef __cplusplus
}
#endif

// Code Below Borrowed from http://stackoverflow.com/a/1861337
// Granularity is 15 ms on windows; on linux it is implementation dependent, but it usually 15 ms as well

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

/* Remove if already defined */
typedef unsigned long long uint64;

/* Returns the amount of milliseconds elapsed since the UNIX epoch. Works on both windows and linux. */

inline uint64 GetTimeMs64() {
#ifdef _WIN32
	/* Windows */
	FILETIME ft;
	LARGE_INTEGER li;

	/* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
	* to a LARGE_INTEGER structure. */
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	uint64 ret = li.QuadPart;
	ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
	ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

	return ret;
#else
	/* Linux */
	struct timeval tv;

	gettimeofday(&tv, 0);

	uint64 ret = tv.tv_usec;
	/* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
	ret /= 1000;

	/* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
	ret += (tv.tv_sec * 1000);

	return ret;
#endif
}
