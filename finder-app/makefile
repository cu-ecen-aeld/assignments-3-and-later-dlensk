#Makefile

ifdef CROSS_COMPILE
CC = $(CROSS_COMPILE)gcc
else
CC := gcc
endif

SOURCE = writer.c
TARGET = writer
OBJECT = writer.o

FileResultPath =/home/ld/EmbeddedLinux/CourseOne/student-test/assignment2/fileresult.txt

all : $(TARGET)


(TARGET): $(OBJECT)
	(CC) $(OBJECT) -o $(TARGET)
	
%.o: %.c
	$(CC) -c $< -o $@
	
clean:

	rm -f $(TARGET) $(OBJECT)
	
.PHONY: clean




	
