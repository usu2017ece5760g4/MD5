md5:
	gcc -o md5 md5.c

debug:
	gcc -ggdb -o build/md5 md5.c

clean:
	rm build/*.o

