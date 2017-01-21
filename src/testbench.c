char A[7] = "aaaaaa",
	B;

int digit = 6;

// aaaaaa

char** Messages;
int* Limits;

Messages = new char*[10];
Limits = new int(1);


loop_alpha(1, f);


void loop_alpha(unsigned int n, void (*f)(char*, unsigned int)) {
	//increment at digit
	// if > 'z' set digit = 'a' and increment digit - 1	
	// digit < 0 break
	while(true) {
		++A[digit];
		if (A[digit] > 'z') {
			A[digit] = 'a';
			if(--digit < 0) {
				break;
			}
		} else if(digit < 6) {
			digit = 6;
		}
	}

}

void f(char* message, unsigned int n) {
	Messages[n-1][i] = message;
}

loop_alpha(6, md5);
loop_alpha(7, md5);
loop_alpha(8, md5);
loop_alpha(9, md5);
loop_alpha(10, md5);


//A="aaaaaa";
B=A[5];
B=B+1;
A[5]=B;
printf("A= %s\n",A);

