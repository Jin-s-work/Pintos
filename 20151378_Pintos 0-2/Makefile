testlib: main.o list.o hash.o bitmap.o
	gcc -o testlib main.o list.o hash.o bitmap.o

main.o : main.c
	gcc -c -o main.o main.c

list.o : list.c
	gcc -c -o list.o  list.c
hash.o : hash.c
	gcc -c -o hash.o  hash.c
bitmap.o : bitmap.c
	gcc -c -o bitmap.o bitmap.c

clean : 
	rm *.o testlib
