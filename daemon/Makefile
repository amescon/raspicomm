CC = gcc

SRCS = $(wildcard *.h) $(wildcard *.c)
APIHEADER = ../raspicomm/api.h

APILIB = /lib/libraspicomm.so

all: raspicommd
raspicommd:  $(APIHEADER) $(SRCS) $(APILIB)
	$(CC) -g $^ -o $@
clean:
	rm -f raspicommd