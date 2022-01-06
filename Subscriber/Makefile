CC = gcc
LDLIBS = -lssl -lcrypto -lmosquitto -Wl,-rpath=/usr/local/lib
CFLAGS = -W -O

Subscriber: sub.o ss.o
	$(CC) -o Subscriber subscribe.o  ss.o $(LDLIBS) $(LDOSSL)
subscribe.o: subscribe.c subscribe.h
	$(CC) $(CFLAGS) $(OSSLFLAG) -c subscribe.c -o subscribe.o
ss.o: shamirsecretshare.c shamirsecretshare.h
	$(CC) $(CFLAGS) $(OSSLFLAG) -c  shamirsecretshare.c -o ss.o
clean:
	rm -f Subscriber *.o
