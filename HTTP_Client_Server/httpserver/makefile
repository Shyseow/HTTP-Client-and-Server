# makefile for myhttpd

myhttpd: myhttpd.o httpserver.o
	gcc myhttpd.o httpserver.o -o myhttpd

myhttpd.o: myhttpd.c httpserver.h
	gcc -c myhttpd.c

httpserver.o: httpserver.c httpserver.h
	gcc -c httpserver.c

clean: 
	rm *.o
