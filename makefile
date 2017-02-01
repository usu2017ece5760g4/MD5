full: build/md5.o build/strgen.o build/main.o
	gcc bin/*.o -o ./md5

build/md5.o: build
	gcc -std=c99 -O3 -c src/md5.c -o build/md5.o

build/strgen.o: build
	gcc -std=c99 -O3 -m64 -c src/strgen.c -o build/strgen.o -Wimplicit-function-declaration

build/main.o: build
	gcc -std=c99 -O3 -c src/main.c -o build/main.o

build:
	mkdir -p build

clean:
	rm build/*.o
