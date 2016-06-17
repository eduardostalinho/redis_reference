osx:
	gcc -dynamic -fno-common -std=gnu99 -c -o reference.o reference.c
	ld -o reference.so reference.o -bundle -undefined dynamic_lookup -lc

linux:
	gcc -fPIC -std=gnu99 -c -o reference.o reference.c
	ld -o reference.so reference.o -shared -Bsymbolic -lc
