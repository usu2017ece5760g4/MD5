//---------------------------------------------------------------------------------------------------------------------+
// MD5 Hashing Algorithm                                                                                               |
// USU ECE 5760 Project 1                                                                                              |
// Braydn Clark A01091991                                                                                              |
// Joshua Lynn A01799554                                                                                               |
// John Call A01283897                                                                                                 |
//---------------------------------------------------------------------------------------------------------------------+
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <cstdint>

typedef unsigned char byte;
typedef unsigned int uint;

//---------------------------------------------------------------------------------------------------------------------+
// <Precompiled constants used in the md5 hashing algorithm>                                                           |
//---------------------------------------------------------------------------------------------------------------------+
// Note: Precomputed values borrowed from https://en.wikipedia.org/wiki/MD5#Pseudocode (and converted to big endian)
// Note: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating
uint IV[4] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 };

// Per-round shift amounts
uint s[64] = {
	7, 12, 17, 22,    7, 12, 17, 22,    7, 12, 17, 22,    7, 12, 17, 22
	5,  9, 14, 20,    5,  9, 14, 20,    5,  9, 14, 20,    5,  9, 14, 20
	4, 11, 16, 23,    4, 11, 16, 23,    4, 11, 16, 23,    4, 11, 16, 23
	6, 10, 15, 21,    6, 10, 15, 21,    6, 10, 15, 21,    6, 10, 15, 21
};

// Binary integer part of the sines of integers (Radians) as constants:
uint K[64] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};
//---------------------------------------------------------------------------------------------------------------------+
// </Precompiled constants used in the md5 hashing algorithm>                                                          |
//---------------------------------------------------------------------------------------------------------------------+

union chunk {
	byte bytes[64];
	uint words[16];
};

//---------------------------------------------------------------------------------------------------------------------+
// Primary md5 function: outputs the 128-bit hash to the specified input "out" of "msg" with "n" bytes                 |
//---------------------------------------------------------------------------------------------------------------------+
void md5(byte* msg, uint n, uint*& out) {
	
	
	// Compartmentalize into 512 byte chunks
	uint chunks = (n / 512) + 1;
	uint remainder = n % 512;
	chunk** parts = malloc(chunks * sizeof(chunk*));
	
	
	
	out = hash;
}

int main(int argc, char **argv)
{
//getopt is awesome but it took a while to figure out how to use it
// this site helped me out https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/getopt.html
// i put that here more for my own reference because I'm sure ill use getopt again.

/*
extern char *optarg; // these are for getopt. 
extern int optind;

int verbose =0,sflag=0,Eflag=0,bflag=0,tflag=0;
char *t;
int s, E, b;
char c;
//this is just for taking in command line arguments.
	while ((c = getopt(argc, argv, "hvs:E:b:t:")) != -1){
		switch(c){
			case 'h':
				// print usage info here 
				printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n Options:\n  -h         Print this help message.\n  -v         Optional verbose flag.\n  -s <num>   Number of set index bits.\n  -E <num>   Number of lines per set.\n  -b <num>   Number of block offset bits.\n  -t <file>  Trace file.\n \nExamples:\n  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
				exit(1);
				break;	
			case 'v':
				verbose=1;
				break;			
			case 's':
				sflag=sflag+1;
				s = atoi(optarg);
				break;
			case 'E':
				Eflag=Eflag+1;
				E = atoi(optarg);
				break;
			case 'b':
				bflag=bflag+1;
				b = atoi(optarg);
				break;
			case 't':
				tflag=tflag+1;
				t=optarg;
				break;
			case '?':	
				// print something about not having the right stuffs
				exit(1);
				break;
				
		}

	}

if(sflag==0 || Eflag==0 || bflag==0 || tflag==0){
	printf("you didn't have all the needed command-line arguments\n");
	exit(1);
}
else if(sflag>1 || Eflag>1 || bflag>1 || tflag>1){
	printf("you used some arguments more than once\n");
	exit(1);
};

*/

uint size = 5;
byte message[5] = { 0x68, 0x65, 0x6c, 0x6c, 0x6f };

unsigned long long int pad, L, a,b,c,d;
//unsigned int t[64];

test = (test<<1)+1;

pad = ((size+1)%448);

if(pad>0)pad=448-pad; 

test = (test<<(pad+64))+size; 


L = sizeof(test)/512;

a=0x67452301;
b=0xEFCDAB89;
c=0x98BADCFE;
d=0x10325476;
int i;
/*	for (i =0;i<64;i++){
		t[i]=(unsigned) × fabs(sin(i));
	}
*/

int t[64]= {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

 

printf("ABCD = %02x%02x%02x%02x\n" , a,b,c,d);
/*

//for each 512-bit chunk of message
 //   break chunk into sixteen 32-bit words M[j], 0 ≤ j ≤ 15 // we only have one 512bit chunk right now so im not thingking about others 
//Initialize hash value for this chunk:
     int A = a;
     int B = b;
     int C = c;
     int D = d;
//Main loop:
    for ( i =0;i<64:i++){
        if (0 ≤ i ≤ 15){
            F = (B & C) | ((~B) & D);
            g = i;
	}
        else if (16 ≤ i ≤ 31){
            F = (D & B) | ((~D) & C);
            g = (5*i + 1) % 16;
	}
        else if (32 ≤ i ≤ 47){
            F = B ^ C ^ D;
            g = (3*i + 5) % 16;
	}
        else if (48 ≤ i ≤ 63){
            F = C ^ (B | (~D));
            g = (7*i) % 16;
	};
//Be wary of the below definitions of a,b,c,d
        dTemp = D;
        D = C;
        C = B;
        B = B + leftrotate((A + F + K[i] + M[g]), s[i]);
        A = dTemp;
    }
//Add this chunk's hash to result so far:
    a0 := a0 + A
    b0 := b0 + B
    c0 := c0 + C
    d0 := d0 + D

*/



	return 0;
}
