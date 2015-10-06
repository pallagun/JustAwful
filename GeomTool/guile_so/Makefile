CC=gcc
GUILEFLAGS=`guile-config compile`
CFLAGS= -fPIC -pg -Wall --std=c99 -O0 -gdwarf-2 -g3 -ggdb3 --coverage -I ../c/include
LIBS=-lm
GUILELIBS=`guile-config link`
SNARFOPTS=$(CFLAGS) $(GUILEFLAGS)

LIBTARGET=~/.ld_library_path/
LD=ld -r
LD_LINK=$(LD) $(LDFLAGS) -L.

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

all: libguile-geomtool.so

user-install: libguile-geomtool.so
	mkdir -p $(LIBTARGET)
	cp $< $(LIBTARGET)

libguile-geomtool.so: $(OBJECTS) geomTool.o 
	$(CC) $(GUILEFLAGS) $(CFLAGS) -shared -o libguile-geomtool.so -fPIC $(OBJECTS) geomTool.o

geomTool.o :
	make -C ../c geomTool.o
	cp ../c/geomTool.o ./

%.o : %.c
	$(CC) -c $(GUILEFLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f *.so *.o *.x *.gcda *.gcno
	rm -f $(LIBTARGET)/libguile-geomtool.so
	make -C ../c clean

