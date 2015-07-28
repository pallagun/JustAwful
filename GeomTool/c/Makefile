# geomTool thing

CC=gcc
CFLAGS= -pg -Wall --std=c99 -O0 -fPIC -gdwarf-2 -g3 -ggdb3 --coverage

LD=ld -r
LD_LINK=$(LD) $(LDFLAGS) -L.


SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

all: geomTool.o

geomTool.o:  $(OBJECTS)
	$(LD_LINK) -o $@ $^ $(LDDEPS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.gcda *.gcno

