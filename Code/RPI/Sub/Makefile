CC = gcc
LDLIBS = -lssl -lcrypto -lmosquitto -Wl,-rpath=/usr/local/lib
CFLAGS = -W -O

sub: sub.o ss.o
	$(CC) -o sub sub.o  ss.o $(LDLIBS) $(LDOSSL)
sub.o: sub.c sub.h 
	$(CC) $(CFLAGS) $(OSSLFLAG) -c sub.c -o sub.o
ss.o: shamiresecretshare.c shamiresecretshare.h 
	$(CC) $(CFLAGS) $(OSSLFLAG) -c  shamiresecretshare.c -o ss.o
clean:
	rm -f sub *.o

