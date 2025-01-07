CC = gcc

CFLAGS = -o

LIBFLAGS = -lws2_32

all: client server run

client:
	@$(CC) main.c $(CFLAGS) main $(LIBFLAGS)

server:
	@$(CC) server.c $(CFLAGS) server $(LIBFLAGS)

run: client server
	@echo "Starting server and client..."
	@./server &   # Run the server in the background
	@sleep 1      # Optionally, wait for the server to initialize
	@./main       # Run the client program
