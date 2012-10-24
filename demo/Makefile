CC = gcc

SRCS = $(wildcard *.h) $(wildcard *.c)
APIHEADER = ../raspicomm/api.h

APILIB = /lib/libraspicomm.so
STEPROCKERLIB = /lib/libraspicommsteprocker.so
DISPLAYLIB = /lib/libraspicommdisplay.so

all: raspicommdemo
raspicommdemo:  $(APIHEADER) $(SRCS) $(APILIB) $(STEPROCKERLIB) $(DISPLAYLIB)
	$(CC) $^ -o $@
clean:
	rm -f raspicommdemo