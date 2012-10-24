
CC = gcc

SRCS = $(wildcard *.h) $(wildcard *.c) 

all: api
api: libraspicomm.so

libraspicomm.so: $(SRCS)
	$(CC) -lpthread -shared -g $^ -o $@

clean:
	rm -f *.o
	rm -f *.so