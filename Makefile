.PHONY: all clean
.SUFFIXES:
.SUFFIXES: .c .o

CFLAGS=-Wall -Wextra -Wshadow -D_POSIX_C_SOURCE=200112L -D_ISOC99_SOURCE -D_SVID_SOURCE -O2

all: gai_test
clean:
	rm -f gai_test *.o

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

gai_test: gai_test.c
	$(CC) $(CFLAGS) -o $@ $<
