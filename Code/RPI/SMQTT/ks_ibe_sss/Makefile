CC = gcc
LDLIBS = -lpbc -lgmp -lssl -lcrypto -lmosquitto -Wl,-rpath=/usr/local/lib
CFLAGS = -W -O

ksibe: hks_ibe.o
	$(CC) -o ksibe hks_ibe.o $(LDLIBS) $(LDOSSL)
hks_ibe.o: hks_ibe.c  KeyStore_ibe.h
	$(CC) $(CFLAGS) $(OSSLFLAG) -c  hks_ibe.c -o hks_ibe.o
clean:
	rm -f ksibe *.o
