CC = gcc
LDLIBS = -lssl -lcrypto  -Wl,-rpath=/usr/local/lib
CFLAGS = -W -O

KeyGen: KeyGenerator.o
	$(CC) -o KeyGen KeyGenerator.o $(LDLIBS) $(LDOSSL)
KeyGenerator.o: KeyGenerator.c
	$(CC) $(CFLAGS) $(OSSLFLAG) -c  KeyGenerator.c -o KeyGenerator.o
clean:
	rm -f ksibe *.o
