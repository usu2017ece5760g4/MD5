full: bin/md5.o bin/main.o
	gcc bin/*.o -o bin/md5

bin/md5.o: bin
	gcc -std=c99 -O3 -c src/md5.c -o bin/md5.o -w

bin/main.o: bin
	gcc -std=c99 -O3 -c src/main.c -o bin/main.o -w

debug: bin/md5.debug.o bin/main.debug.o
	gcc bin/*.debug.o -o bin/md5.debug

bin/md5.debug.o: bin
	gcc -std=c99 -O3   -c src/md5.c    -o bin/md5.debug.o    -w

bin/main.debug.o: bin
	gcc -std=c99 -ggdb -c src/main.c   -o bin/main.debug.o   -w

bin:
	mkdir -p bin

clean:
	rm bin/*
