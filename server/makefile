CC ?= $(CROSS_COMPILE)gcc
CFLAGS= -Werror -g -Wall
TARGET=aesdsocket
SRC=aesdsocket.c
OBJ=aesdsocket.o

all: $(TARGET)

$(OBJ) : %.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@
	
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(TARGET)

.PHONY:clean
clean:
	rm -f $(OBJ) $(TARGET)

	

	 


