CC = gcc

CFLAGS = -o

LIBFLAGS = -lws2_32

all: client server
	@./server
	@./main

client:
	@$(CC) main.c $(CFLAGS) main $(LIBFLAGS)

server:
	@$(CC) server.c $(CFLAGS) main $(LIBFLAGS) 

spawn: client
	$(info Another client thread has been spawned...)