CC = gcc
LDLIBS = -lpbc -lgmp -lssl -lcrypto -Wl,-rpath=/usr/local/lib
CFLAGS = -W -O

bfibe: bfibe.o
        $(CC) -o bfibe bfibe.o $(LDLIBS) $(LDOSSL)
bfibe.o: bonehfranklin.c
        $(CC) $(CFLAGS) $(OSSLFLAG) -c bonehfranklin.c -o bfibe.o
clean:
        rm -f bfibe *.o