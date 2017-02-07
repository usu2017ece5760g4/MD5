full: bin/md5.o bin/main.o
	gcc bin/*.o -o bin/md5

bin/md5.o: bin
	gcc -std=c99 -mavx2 -O0 -c src/md5.c  -o bin/md5.o  -w

bin/main.o: bin
	gcc -std=c99        -O0 -c src/main.c -o bin/main.o -w

bin:
	mkdir -p bin

clean:
	rm bin/*
