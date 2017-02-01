full: bin/md5.o bin/strgen.o bin/main.o bin
	gcc bin/*.o -o bin/md5

bin/md5.o: bin
	gcc -std=c99 -O3 -c src/md5.c -o bin/md5.o -w

bin/strgen.o: bin
	gcc -std=c99 -O3 -m64 -c src/strgen.c -o bin/strgen.o -w

bin/main.o: bin
	gcc -std=c99 -O3 -c src/main.c -o bin/main.o -w

debug: bin/md5.debug.o bin/strgen.debug.o bin/main.debug.o bin
	gcc bin/*.debug.o -o bin/md5.debug

bin/md5.debug.o: bin
	gcc -std=c99 -O3 -ggdb -c src/md5.c -o bin/md5.debug.o -w

bin/strgen.debug.o: bin
	gcc -std=c99 -O3 -ggdb -m64 -c src/strgen.c -o bin/strgen.debug.o -w

bin/main.debug.o: bin
	gcc -std=c99 -O3 -ggdb -c src/main.c -o bin/main.debug.o -w

bin:
	mkdir -p bin

clean:
	rm bin/*
