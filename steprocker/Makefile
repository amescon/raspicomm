
CC = gcc

SRCS = $(wildcard *.h) $(wildcard *.c) 
APILIB = /lib/libraspicomm.so

all: libraspicommsteprocker.so

libraspicommsteprocker.so: $(SRCS) $(APILIB)
	$(CC) -shared $^ -o $@

clean:
	rm -f *.o
	rm -f *.so