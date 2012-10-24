CC = gcc

SRCS = $(wildcard *.h) $(wildcard *.c)
APIHEADER = ../raspicomm/api.h

# APILIB = ../raspicomm/libraspicomm.so
APILIB = /lib/libraspicomm.so

all: libraspicommdisplay.so

libraspicommdisplay.so:  $(APIHEADER) $(SRCS) $(APILIB)
	$(CC) -shared $^ -o $@

clean:
	rm -f *.so