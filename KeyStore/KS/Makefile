CC = gcc
LDLIBS = -lpbc -lgmp -lssl -lcrypto -lmosquitto -Wl,-rpath=/usr/local/lib
CFLAGS = -W -O

KeyStore: KeyStore_ibe.o db.o
	$(CC) -o KeyStore KeyStore_ibe.o db.o $(LDLIBS) $(LDOSSL)
KeyStore_ibe.o: KeyStore_ibe.c KeyStore_ibe.h type.h
	$(CC) $(CFLAGS) $(OSSLFLAG) -c  KeyStore_ibe.c -o KeyStore_ibe.o
db.o:	db.c db.h type.h
	$(CC) $(CFLAGS) $(OSSLFLAG) -c  db.c -o db.o
clean:
	rm -f KeyStore *.o
