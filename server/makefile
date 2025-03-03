CC ?= $(CROSS_COMPILE)gcc
CFLAGS= -Werror -g -Wall
TARGET=aesdsocket

default: aesdsocket

all: aesdsocket

aesdsocket: aesdsocket.o
	$(CC) $(CFLAGS) aesdsocket.o -o aesdsocket
    
aesdsocket.o: aesdsocket.c
	$(CC) $(CFLAGS) -c aesdsocket.c
	
clean:
	rm -f aesdsocket.o aesdsocket 
	 


