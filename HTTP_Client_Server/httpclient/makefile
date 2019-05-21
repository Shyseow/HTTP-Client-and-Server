# makefile for myhttp

myhttp: myhttp.o client.o
	gcc myhttp.o client.o -o myhttp

myhttp.o: myhttp.c client.h
	gcc -c myhttp.c

client.o: client.c client.h 
	gcc -c client.c

clean: 
	rm *.o
