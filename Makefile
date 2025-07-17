all:
	gcc -Isrc/include -Lsrc/lib -o main main.c -lmingw32 -lSDL3